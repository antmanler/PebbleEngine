// ****************************************************************************************
//
// Pebble Scene Editor
// --------------------------------------
// Copyright (C) 2011 Antmanler
//
// This file is part of the Pebble Scene Editor.
//
// The Pebble Scene Editor is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation version 3 of the License
//
// The Pebble Scene Editor is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Credits : Volker Wiendl who created the major work of the original editor and Horde3D
// is a flexible graphics engine. see <http://www.horde3d.org> and <http://http://goo.gl/Me2HT>
//
// ****************************************************************************************


#include <cmath>
#include <cfloat>
#include <fstream>
#include <sstream>
#include <queue>

#include "kdTree.h"
#include "distancefield3d.h"

using namespace std;

namespace {
float mysign(float x) {
    if (x<0) return -1;
    else return +1;
}
inline float clamp(float a, float min, float max) {
    if (a < min) return min;
    if (a > max) return max;
    return a;
}
}

DistanceField3D::DistanceField3D(int nbx_, int nby_, int nbz_)
    : p0(), p1(), h(), invh(), nbx(nbx_), nby(nby_), nbz(nbz_), cancel_(false) {
    closestPoints = NULL;
    closestNormals = NULL;

    distances = new float[(nbx+1)*(nby+1)*(nbz+1)];
    weights = new float[(nbx+1)*(nby+1)*(nbz+1)];
    positions = new vector3f[(nbx+1)*(nby+1)*(nbz+1)];
}

DistanceField3D::~DistanceField3D() {
    delete[] distances;
    delete[] weights;
    delete[] positions;
    delete[] closestPoints;
    delete[] closestNormals;
}

void DistanceField3D::meshToDistanceField(Mesh &mesh) {

    cancel_ = false;
    p0 = mesh.m;
    p1 = mesh.M;
    h = (p1-p0)*vector3f(1.0f/float(nbx), 1.0f/float(nby), 1.0f/float(nbz));
    invh.set(1.0f/h.x, 1.0f/h.y, 1.0f/h.z);

    long nbtriangles = mesh.triangles.size();

    std::vector<vector3f> trianglecenters;
    trianglecenters.reserve(nbtriangles);

    float longestdiagonal = 0;
    const long steps = nbtriangles + (nbx+1)*(nby+1)*(nbz+1);

    // loop over triangles, compute triangle centers and longest diagonal
    for (int i=0; i<nbtriangles; i++) {
        if (0 == i % 1000) sig_notify_.Emit("Processing triangles...", i, nbtriangles);

        const vector3f center = mesh.triangles[i]->center;
        trianglecenters.push_back(center);
        float dist = mesh.triangles[i]->v[0]->distance(center);
        if (dist>longestdiagonal) longestdiagonal=dist;
        dist = mesh.triangles[i]->v[1]->distance(center);
        if (dist>longestdiagonal) longestdiagonal=dist;
        dist = mesh.triangles[i]->v[2]->distance(center);
        if (dist>longestdiagonal) longestdiagonal=dist;
    }

    // put trianles in kd-tree
    KdTree * tree = new KdTree(trianglecenters, trianglecenters.size(), 10);
    for (long i=0; i<=nbx; i++) {
        for (long j=0; j<=nby; j++) {
            for (long k=0; k<=nbz; k++) {
                int step = i*nbz*nby + j*nbz + k;
                if (0 == step % 200) sig_notify_.Emit("Computing distance",  step, steps-nbtriangles);
                if (cancel_) goto CANCEL;

                // real world coordinate
                vector3f p = p0 + vector3f(i,j,k)*h;
                // for each point on grid, compute closest triangle center
                tree->setNOfNeighbours(1);
                tree->queryPosition(p);
                float dist = sqrt(tree->getSquaredDistance(0));
                // possible closest point is at most this far away:
                //float closestpossibledist = max(0.0f, dist-longestdiagonal);
                // for grid points far away we don't compute distance
                // we will do flood filling at a later stage
                /*if (closestpossibledist > 10*h.length()) {
               setDistance(i,j,k,-1); // TODO: make it a marker or a boolean
               continue;
            }*/
                // do second (range) query with found closest distance + longestdiagonal
                float newdist = dist + longestdiagonal;
                tree->setNOfNeighbours(10000);
                tree->queryRange(p, newdist*newdist);
                int nbfound = tree->getNOfFoundNeighbours();
                // loop over all found triangles and find closest point
                float mindist = FLT_MAX;
                for (int m=0; m<nbfound; m++) {
                    int index = tree->getNeighbourPositionIndex(m);
                    // compute closest point on triangle
                    // use angle-weighted normal of closest point to define inside/outside
                    float dist = distanceToTriangle(p, mesh.triangles[index]);
                    if (fabs(dist)<fabs(mindist)) {
                        mindist = dist;
                    }
                }
                setDistance(i, j, k, mindist);
            }
        }
    }
    sig_notify_.Emit("Computing distance",  steps, steps);
CANCEL:
    delete tree;
    cancel_ = false;
}

float DistanceField3D::distPointSegment(const vector3f &p, const vector3f &p0, const vector3f &p1, vector3f &result) {
    vector3f v = p1-p0;
    vector3f w = p-p0;
    float c1 = w.dot(v);
    if ( c1 <= 0 ) return FLT_MAX; //p.distance(p0);
    float c2 = v.dot(v);
    if ( c2 <= c1 ) return FLT_MAX; //p.distance(p1);
    float t = c1 / c2;
    result = p0 + t * v;
    return p.distance(result);
}

float DistanceField3D::distPointPlane(const vector3f &p, const vector3f &o, const vector3f &n) {
    return fabs(n.dot(p-o));
}

float DistanceField3D::distanceToTriangle(const vector3f &p, Triangle *t) {
    float mindist = FLT_MAX;
    vector3f normal;
    vector3f point;
    for (int i=0; i<3; i++) {
        float dist = t->v[i]->distance(p);
        if (dist<mindist) {
            mindist = dist;
            normal = t->v[i]->normal;
            point = *t->v[i];
        }
    }
    for (int i=0; i<3; i++) {
        if (t->e[i]->border) continue;
        vector3f result;
        float dist = distPointSegment(p, *(t->e[i]->v[0]), *(t->e[i]->v[1]), result);
        if (dist<mindist) {
            mindist = dist;
            normal = t->e[i]->normal;
            point = result;
        }
    }
    // to plane
    float s = ((*t->v[0]) - p).dot(t->normal);
    vector3f projected = p+s*t->normal;
    float dist = projected.distance(p);
    if (dist<mindist) {
        vector3f v0 = *t->v[0];
        vector3f v1 = *t->v[1];
        vector3f v2 = *t->v[2];
        vector3f kDiff = v0 - p;
        vector3f kE0 = v1 - v0, kE1 = v2 - v0;
        float fA00 = kE0.getSquaredLength();
        float fA01 = kE0.dot(kE1);
        float fA11 = kE1.getSquaredLength();
        float fB0 = kDiff.dot(kE0);
        float fB1 = kDiff.dot(kE1);
        //        float fC = kDiff.getSquaredLength();
        float fDet = fabs(fA00*fA11-fA01*fA01);
        float fS = fA01*fB1-fA11*fB0;
        float fT = fA01*fB0-fA00*fB1;
        if (fS>0 && fT>0 && fS+fT<fDet) {
            mindist = dist;
            normal = t->normal;
            point = projected;
        }
    }
    float sign = (point-p).dot(normal);
    if (sign<0) sign = -1;
    else sign = +1;
    return mindist*sign;
}

void DistanceField3D::meshToDistanceField2(Mesh &mesh) {

    p0 = mesh.m;
    p1 = mesh.M;
    h = (p1-p0)*vector3f(1.0f/float(nbx), 1.0f/float(nby), 1.0f/float(nbz));
    invh.set(1.0f/h.x, 1.0f/h.y, 1.0f/h.z);

    int nbtriangles = mesh.triangles.size();
    const long steps = nbtriangles + nbx*nby*nbz;

    std::vector<vector3f> trianglecenters;
    longestdiagonal = 0;

    // loop over triangles, compute triangle centers and longest diagonal
    for (int i=0; i<nbtriangles; i++) {
        if (0 == i % 1000) sig_notify_.Emit("Processing triangles...", i, nbtriangles);

        const vector3f center = mesh.triangles[i]->center;
        trianglecenters.push_back(center);
        for (int j=0; j<3; j++) {
            float dist = mesh.triangles[i]->v[j]->distance(center);
            if (dist>longestdiagonal) longestdiagonal=dist;
        }
    }
    // put trianles in kd-tree
    KdTree *kdtree = new KdTree(trianglecenters, trianglecenters.size(), 10);
    for (int i=0; i<=nbx; i++) {
        for (int j=0; j<=nby; j++) {
            for (int k=0; k<=nbz; k++) {
                int step = i*nbz*nby + j*nbz + k;
                if (0 == step % 200) sig_notify_.Emit("Computing distance",  step, steps-nbtriangles);
                if (cancel_) goto CANCEL;

                // real world coordinate
                vector3f p = p0 + vector3f(i,j,k)*h;
                // for each point on grid, compute closest triangle center
                kdtree->setNOfNeighbours(1);
                kdtree->queryPosition(p);
                float dist = sqrt(kdtree->getSquaredDistance(0));
                // possible closest point is at most this far away:
                float closestpossibledist = max(0.0f, dist-longestdiagonal);
                // for grid points far away we don't compute distance
                // we will do flood filling at a later stage
                /*if (closestpossibledist > 10*h.length()) {
               setDistance(i,j,k,-1); // TODO: make it a marker or a boolean
               continue;
            }*/
                // do second (range) query with found closest distance + longestdiagonal
                float newdist = dist + longestdiagonal;
                kdtree->setNOfNeighbours(10000);
                kdtree->queryRange(p, newdist*newdist);
                int nbfound = kdtree->getNOfFoundNeighbours();
                // loop over all found triangles and find closest point
                float mindist = FLT_MAX;
                for (int m=0; m<nbfound; m++) {
                    int index = kdtree->getNeighbourPositionIndex(m);
                    // compute closest point on triangle
                    // use angle-weighted normal of closest point to define inside/outside
                    float dist = distanceToTriangle(p,mesh.triangles[index]);
                    if (fabs(dist)<fabs(mindist)) {
                        mindist = dist;
                    }
                }
                float sign = inside(&mesh, kdtree, p);
                //            if (sign<0) sign = -1;
                //            else sign = +1;
                //            setDistance(i,j,k,sign*fabs(mindist));
                setDistance(i,j,k,sign);

            }
        }
    }
    sig_notify_.Emit("Computing distance",  steps, steps);
CANCEL:
    delete kdtree;
    cancel_ = false;
}


float DistanceField3D::inside(Mesh *mesh, KdTree *kdtree, const vector3f &p) {
    bool minVertexHit = true;
    int nbNeighbors = 1;
    int minFaceIndex;
    float minSqrDist;
    float mint;
    vector3f minIntersectionPoint;
    vector3f minIntersectionNormal;

    int count = 0;
    while (minVertexHit && count<10) {
        count++;
        kdtree->setNOfNeighbours(nbNeighbors);
        kdtree->queryPosition(p);

        // compute distance to this triangle, this will give us an upper bound for the next query
        minVertexHit = false;
        minFaceIndex = kdtree->getNeighbourPositionIndex(nbNeighbors-1);
        minSqrDist = kdtree->getSquaredDistance(nbNeighbors-1);
        minIntersectionPoint = kdtree->getNeighbourPosition(nbNeighbors-1);
        //minIntersectionPoint = _facecenters[minFaceIndex];
        vector3f dir = minIntersectionPoint - p;
        mint = dir.normalize();
        // this is the upper bound on query range distance
        float sqrQueryDist =  sqrt(minSqrDist) + longestdiagonal;
        sqrQueryDist*=sqrQueryDist;

        static int nbsearch = 1000;
        kdtree->setNOfNeighbours(nbsearch); // FIXME: this is arbitrary

        kdtree->queryRange(p,sqrQueryDist);
        int nbfound = kdtree->getNOfFoundNeighbours();
        while (nbfound==nbsearch) {
            nbsearch*=2;
            kdtree->setNOfNeighbours(nbsearch); // FIXME: this is arbitrary
            kdtree->queryRange(p,sqrQueryDist);
            nbfound = kdtree->getNOfFoundNeighbours();
        }
        //nbfound = 0;
        // loop over triangles found and compute distance to triangle.
        // take smallest distance and corresponding point
        for (int i=0; i<nbfound; i++) {
            const int faceIndex = kdtree->getNeighbourPositionIndex(i);
            bool vertexHit;
            float sqrDist;
            vector3f intersectionPoint;
            float t;
            bool intersected = mesh->triangles[faceIndex]->intersect(p, dir, intersectionPoint, sqrDist, vertexHit, t);
            //bool intersected = intersect3D(faceIndex, p, dir,t);
            if (intersected && t>=0) {
                //intersectionPoint = p+dir*t;
                //sqrDist = intersectionPoint.distanceSquared(p);
                if (t<=mint /*sqrDist < minSqrDist*/)  {
                    minVertexHit = vertexHit;
                    minFaceIndex = faceIndex;
                    minIntersectionPoint = intersectionPoint;
                    minSqrDist = sqrDist;
                    mint = t;
                }
            }
        }
        nbNeighbors++;
    }
    if (count == 10) {
        // if it does not converge it should be exactly on an edge, so we return zero
        vector3f dir = minIntersectionPoint-p;
        float dist = dir.normalize();
        minIntersectionPoint = p;
        minIntersectionNormal.set(0.0, 0.0, 0.0);
        return 0;
    }

    // check inside outside with intersection point
    vector3f dir = minIntersectionPoint-p;
    float dist = dir.normalize();
    minIntersectionNormal = mesh->triangles[minFaceIndex]->normal;
    float dot = minIntersectionNormal.dot(dir);
    /*if (fabs(dot)<1.0e-9) return 0;
    else */if (dot<0) return dist;
    else return -dist;

}

void DistanceField3D::meshToDistanceField3(Mesh &m) {

    p0 = m.m;
    p1 = m.M;
    h = (p1-p0)*vector3f(1.0f/float(nbx), 1.0f/float(nby), 1.0f/float(nbz));
    invh.set(1.0f/h.x, 1.0f/h.y, 1.0f/h.z);

    int nbtriangles = m.triangles.size();
    // loop over triangles, compute triangle centers and longest diagonal
    float minarea = FLT_MAX;
    for (int i=0; i<nbtriangles; i++) {
        Triangle *t = m.triangles[i];
        if (t->area < minarea && t->area>0) minarea = t->area;
    }
    std::vector<vector3f> samples;
    //    std::vector<vector3f> normals;
    std::vector<int> normals;
    const long steps = nbtriangles + nbx*nby*nbz;

    for (int i=0; i<nbtriangles; i++) {
        if (0 == i % 1000) sig_notify_.Emit("Processing triangles...", i, nbtriangles);
        Triangle *tri = m.triangles[i];
        vector3f dir1 = *(tri->v[1]) - *(tri->v[0]);
        vector3f dir2 = *(tri->v[2]) - *(tri->v[0]);
        int nb = (int)ceil(tri->area/minarea);
        if (nb>800) nb=800;
        if (nb<300) nb=300;
        for (int j=0; j<nb; j++) {
            float s = ((float)rand())/float(RAND_MAX);
            float t = ((float)rand())/float(RAND_MAX);
            vector3f p = *(tri->v[0])  + s*dir1 + t*dir2;
            samples.push_back(p);
            //            normals.push_back(tri->normal);
            normals.push_back(i);
        }
    }

    // put trianles in kd-tree
    KdTree *kdtree = new KdTree(samples, samples.size(), 1);
    for (int i=0; i<=nbx; i++) {
        if (cancel_) goto CANCEL;
        for (int j=0; j<=nby; j++) {
            for (int k=0; k<=nbz; k++) {
                int step = i*nbz*nby + j*nbz + k;
                if (0 == step % 200) sig_notify_.Emit("Computing distance",  step, steps-nbtriangles);
                // real world coordinate
                vector3f p = p0 + vector3f(i,j,k)*h;
                // for each point on grid, compute closest triangle center
                kdtree->setNOfNeighbours(1);
                kdtree->queryPosition(p);
                float dist = sqrt(kdtree->getSquaredDistance(0));
                vector3f normal = m.triangles.at(normals[kdtree->getNeighbourPositionIndex(0)])->normal;
                vector3f point = samples[kdtree->getNeighbourPositionIndex(0)];
                float sign = normal.dot(point-p);
                if (sign>0) sign=+1;
                else sign=-1;
                setDistance(i,j,k,sign*dist);
            }
        }
    }
CANCEL:
    delete kdtree;
}

void DistanceField3D::checkConsistency(int filter_radius) {
    int r = filter_radius;

    for (int i=0; i<=nbx; i++) {
        for (int j=0; j<=nby; j++) {
            for (int k=0; k<=nbz; k++) {

                float dist = getDistance(i,j,k);
                // border is outside!
                if (i==0 || i==nbx || j==0 || j==nby || k==0 || k==nbz) {
                    dist = fabs(dist);
                    setDistance(i,j,k,-dist);
                    continue;
                }
                int nbpos = 0;
                int nbneg = 0;
                bool inconsistent = false;
                for (int di=-r; di<=+r; di++) {
                    for (int dj=-r; dj<=+r; dj++) {
                        for (int dk=-r; dk<=+r; dk++) {
                            float ndist = getDistance(
                                              clamp(i+di, 0, nbx-1),
                                              clamp(j+dj, 0, nby-1),
                                              clamp(k+dk, 0, nbz-1)
                                              );
                            if (ndist<0) nbneg++;
                            else nbpos++;

                            if (dist*ndist<0) {
                                if (fabs(dist) > (h.dot(vector3f(fabs(float(di)),fabs(float(dj)),fabs(float(dk)))))) inconsistent = true;
                                if (fabs(ndist) > (h.dot(vector3f(fabs(float(di)),fabs(float(dj)),fabs(float(dk)))))) inconsistent = true;
                            }
                        }
                    }
                }
                if (inconsistent) {
                    //               cout << "Inconsistent: " << dist << " " << nbpos << " " << nbneg << endl;
                    if (nbpos>nbneg) setDistance(i,j,k,fabs(dist));
                    else if (nbneg>nbpos) setDistance(i,j,k,-fabs(dist));
                }
            }
        }
    }
    //   writeToFile("teapot.df");
}

void DistanceField3D::translate(const vector3f &t) {
    p0+=t;
    p1+=t;
}

void DistanceField3D::scale(const vector3f &s) {
    p0*=s;
    p1*=s;
    h*=s;
    invh.set(1.0f/h.x, 1.0f/h.y, 1.0f/h.z);
    for (int i=0; i<triangles.size()/3; i++) {
        vector3f v1 = s*triangles[3*i+0];
        vector3f v2 = s*triangles[3*i+1];
        vector3f v3 = s*triangles[3*i+2];
        vector3f n = -(v3-v1).cross(v2-v1);
        n.normalize();
        triangles[3*i+0] = v1;
        triangles[3*i+1] = v2;
        triangles[3*i+2] = v3;
        normals[i] = n;
    }
    for (int i=0; i<(nbx+1)*(nby+1)*(nbz+1); i++) {
        distances[i]=s.x*distances[i];
    }
}

void DistanceField3D::invert() {
    const int nb = (nbx+1)*(nby+1)*(nbz+1);
    for (int i=0; i<nb; i++) distances[i] = -distances[i];
}

void DistanceField3D::readFromFile(string filename) {
    ifstream input(filename.c_str(), ios::in|ios::binary);
    input.read((char*)&p0.x, sizeof(float));
    input.read((char*)&p0.y, sizeof(float));
    input.read((char*)&p0.z, sizeof(float));
    input.read((char*)&p1.x, sizeof(float));
    input.read((char*)&p1.y, sizeof(float));
    input.read((char*)&p1.z, sizeof(float));
    input.read((char*)&h.x, sizeof(float));
    input.read((char*)&h.y, sizeof(float));
    input.read((char*)&h.z, sizeof(float));
    input.read((char*)&nbx, sizeof(int));
    input.read((char*)&nby, sizeof(int));
    input.read((char*)&nbz, sizeof(int));
    distances = new float[(nbx+1)*(nby+1)*(nbz+1)];
    invh.set(1.0f/h.x, 1.0f/h.y, 1.0f/h.z);
    for (int i=0; i<=nbx; i++) {
        for (int j=0; j<=nby; j++) {
            for (int k=0; k<=nbz; k++) {
                float dist;
                input.read((char*)&dist, sizeof(float));
                setDistance(i,j,k,dist);
            }
        }
    }
    input.close();
    weights = NULL;
    positions = NULL;

}

void DistanceField3D::writeToFile(string filename) {
    ofstream output(filename.c_str(), ios::out|ios::binary);
    output.write((char*)&p0.x, sizeof(float));
    output.write((char*)&p0.y, sizeof(float));
    output.write((char*)&p0.z, sizeof(float));
    output.write((char*)&p1.x, sizeof(float));
    output.write((char*)&p1.y, sizeof(float));
    output.write((char*)&p1.z, sizeof(float));
    output.write((char*)&h.x, sizeof(float));
    output.write((char*)&h.y, sizeof(float));
    output.write((char*)&h.z, sizeof(float));
    output.write((char*)&nbx, sizeof(int));
    output.write((char*)&nby, sizeof(int));
    output.write((char*)&nbz, sizeof(int));
    for (int i=0; i<=nbx; i++) {
        for (int j=0; j<=nby; j++) {
            for (int k=0; k<=nbz; k++) {
                float dist = getDistance(i,j,k);
                output.write((char*)&dist, sizeof(float));
            }
        }
    }
    output.close();
}
