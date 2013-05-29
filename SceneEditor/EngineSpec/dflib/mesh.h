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

#ifndef __MESH_H__
#define __MESH_H__

#include <vector>
#include <string>
#include <cfloat>
#include "vector3f.h"

using namespace std;

class Vertex;
class Edge;
class Triangle;

class Vertex: public vector3f {

public:

    Vertex(float x, float y, float z) : vector3f(x,y,z) {}

    vector3f normal; // vertex normal
};

class Edge {

public:

    Edge() {
        t[0] = NULL;
        t[1] = NULL;
    }

    Vertex  *v[2];

    Triangle  *t[2];

    vector3f normal;
    bool border;
};

#define MYEPSILON  0.000000001

class Triangle {

public:
    Vertex * v[3];

    Edge   * e[3];

    vector3f normal;
    vector3f center;
    float area;

    bool intersect(const vector3f &orig, const vector3f &dir, vector3f &intersectionPoint, float &sqrDist, bool &vertexHit, float &t) {
        const Vertex &vert0 = *(this->v[0]);
        const Vertex &vert1 = *this->v[1];
        const Vertex &vert2 = *this->v[2];
        const vector3f edge1 = vert1-vert0;
        const vector3f edge2 = vert2-vert0;


        const float angle_normal_dir = normal.dot(dir);
        const bool orthogonal = fabs(angle_normal_dir) < MYEPSILON;


        const vector3f pvec = dir.cross(edge2);
        const float det  =  edge1.dot(pvec);

        t=FLT_MAX;
        vertexHit = false;
        if (!orthogonal && det>MYEPSILON) {
            const vector3f tvec = orig-vert0;
            const float u = tvec.dot(pvec);
            if (u< 0.0 || u>det) {
                //qDebug("u=%f det=%f", u, det);
                return false;
            }

            const vector3f qvec = tvec.cross(edge1);
            const float v = dir.dot(qvec);
            if (v< 0.0 || u+v > det) {
                //qDebug("v=%f u+v=%fdet=%f", u,u+v, det);
                return false;
            }

            const float inv_det = 1.0 / det;

            /* calculate t, ray intersects triangle */
            t = edge2.dot(qvec) * inv_det;

            const float myeps = MYEPSILON*det;
            if (u<myeps/* || (det-u)<myeps*/) vertexHit = true;
            else if (v<myeps || (det-u-v)<myeps) vertexHit = true;

            intersectionPoint = orig+t*dir;
            sqrDist = intersectionPoint.distanceSquared(orig);

            return true;
        }
        else if (!orthogonal && det < -MYEPSILON) {


            const vector3f tvec = orig-vert0;
            const float u = tvec.dot(pvec);
            if (u>0.0 || u<det) return false;

            const vector3f qvec = tvec.cross(edge1);
            const float v = dir.dot(qvec);
            if (v>0.0 || u+v<det) return false;

            const float inv_det = 1.0 / det;

            /* calculate t, ray intersects triangle */
            t = edge2.dot(qvec) * inv_det;

            const float myeps = -MYEPSILON*det;
            if (-u<myeps/* || (u-det)<myeps*/) vertexHit = true;
            else if (-v<myeps || (u+v-det)<myeps) vertexHit = true;

            intersectionPoint = orig+t*dir;
            sqrDist = intersectionPoint.distanceSquared(orig);

            return true;

        }

        else return false;
    }

};

class Mesh {

public:

    Mesh() {}
    ~Mesh();

    void beginGemo();
    void pushGemo(const float *verts, long vertsize, const long *idx, long trisize, long index_offset=0);
    void endGeom();

    void computeNormals();

    void loadMesh(const char* filename);
    void saveMesh(const char* filename);
    void save2Obj(const char* filename);

    vector<Triangle*> triangles;
    vector<Vertex*>   vertices;
    vector<Edge*>     edges;

    vector3f m;
    vector3f M;
private:
    void reset();
    vector<long> indices_;
};

#endif
