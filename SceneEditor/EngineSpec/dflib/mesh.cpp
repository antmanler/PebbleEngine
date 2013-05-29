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

#include <map>
#include <cfloat>
#include <cassert>
#include <fstream>

#include "mesh.h"

using namespace std;

Mesh::~Mesh()
{
    reset();
}

void Mesh::reset()
{
    if (vertices.size() > 0) {
        while (!vertices.empty()) {
            delete vertices.back();
            vertices.pop_back();
        }
    }

    if (triangles.size() > 0) {
        while (!triangles.empty()) {
            delete triangles.back();
            triangles.pop_back();
        }
    }

    if (edges.size() > 0) {
        while (!edges.empty()) {
            delete edges.back();
            edges.pop_back();
        }
    }

    indices_.clear();
}

void Mesh::beginGemo()
{
    reset();
    m = vector3f(FLT_MAX, FLT_MAX, FLT_MAX);
    M = vector3f(-FLT_MAX, -FLT_MAX, -FLT_MAX);
}

void Mesh::pushGemo(const float *verts, long vertsize, const long *idx, long trisize, long index_offset)
{
    long offset = vertices.size();
    for (long i=0; i<trisize; ++i) {
        indices_.push_back(idx[i*3]+offset-index_offset);
        indices_.push_back(idx[i*3+1]+offset-index_offset);
        indices_.push_back(idx[i*3+2]+offset-index_offset);
    }

    for (long i=0; i<vertsize; ++i) {
        Vertex *vert = new Vertex(verts[i*3], verts[i*3+1], verts[i*3+2]);
        vertices.push_back(vert);
        // track for bounding box
        m = m.vmin(*vert);
        M = M.vmax(*vert);
    }
}

void Mesh::endGeom()
{
    // scale to a unit box
    vector3f size = M-m;
    float scale = size.x;
    if (size.y>scale) scale = size.y;
    if (size.z>scale) scale = size.z;
    scale = 1.0f/scale;

    vector3f bbmin = m;

    long nbvertices = vertices.size();
    for (long i=0; i<nbvertices; i++) {
        vector3f *vert = vertices[i];
        vert->x = (vert->x - bbmin.x)*scale;
        vert->y = (vert->y - bbmin.y)*scale;
        vert->z = (vert->z - bbmin.z)*scale;
    }


    // reset the bounding box
    m = m.zero() - 0.05;//0.005*m.dot(m)*scale;
    M = M.one() + 0.05;//0.005*M.dot(M)*scale;

    // load triangles
    map<int,int> edgeids;
    long nbtriangles = indices_.size()/3;
    long index[3];

    for (int i=0; i<nbtriangles; i++) {
        Triangle * t = new Triangle();
        index[0] = indices_[i*3]; index[1] = indices_[i*3+1]; index[2] = indices_[i*3+2];
        t->v[0] = vertices.at(index[0]);
        t->v[1] = vertices.at(index[1]);
        t->v[2] = vertices.at(index[2]);

        t->normal = ((*t->v[1]) - (*t->v[0])).cross((*t->v[2]) - (*t->v[1]));
        t->area = 0.5f*t->normal.normalize();
        t->center = (1.0f/3.0f) * ((*t->v[0]) + (*t->v[1]) + (*t->v[2]));

        triangles.push_back(t);

        int edge[3];
        for (int j=0; j<3; j++) {
            edge[j] = (index[j]<index[(j+1)%3]) ? 10000*index[j]+index[(j+1)%3]: 10000*index[(j+1)%3]+index[j];
        }

        for (int j=0; j<3; j++) {
            map<int,int>::iterator iter;
            iter = edgeids.find(edge[j]);
            if (iter != edgeids.end()) {
                t->e[j] = edges[iter->second];
                edges[iter->second]->t[1] = t;
            }
            else {
                Edge *e = new Edge();
                e->v[0] = t->v[j];
                e->v[1] = t->v[(j+1)%3];
                e->t[0] = t;
                t->e[j] = e;
                edges.push_back(e);
                edgeids[edge[j]] = edges.size()-1;
            }
        }

    }

    for (int i=0; i<edges.size(); i++) {
        Edge * e = edges[i];
        if (e->t[0] == NULL || e->t[1] == NULL) e->border = true;
        else e->border = false;
    }

}

void Mesh::loadMesh(const char* filename)
{
    beginGemo();
    ifstream input(filename, ios::in);
    size_t nbvertices, nbtriangles;
    input >> nbvertices >> nbtriangles;

    // load vertices
    vector<float> verts; verts.reserve(nbvertices*3);
    for (size_t i=0; i < nbvertices; ++i) {
        float x = 0, y=0, z=0;
        input >> x >> y >> z;
        verts.push_back(x); verts.push_back(y); verts.push_back(z);
    }

    // load indices
    vector<long> indices; indices.reserve(nbtriangles*3);
    for (size_t i=0; i<nbtriangles; ++i) {
        long x = 0, y = 0, z = 0;
        input >> x >> y >> z;
        indices.push_back(x); indices.push_back(y); indices.push_back(z);
    }

    pushGemo(&verts[0], nbvertices, &indices[0], nbtriangles);
    endGeom();
}

void Mesh::saveMesh(const char* filename)
{
    ofstream output(filename, ios::out);
    output << vertices.size() << endl;
    output << indices_.size()/3 << endl;

    for (size_t i=0; i < vertices.size(); ++i) {
        vector3f *vert = vertices.at(i);
        output << vert->x << " " << vert->y << " "  << vert->z << endl;
    }

    for (size_t i=0; i < indices_.size()/3; ++i) {
        output << indices_[3*i] << " " << indices_[3*i+1] << " " << indices_[3*i+2] << endl;
    }
    output.close();
}

void Mesh::save2Obj(const char *filename)
{
    std::ofstream out(filename) ;

    out << "# Generated by mesh (antmanler@gmail.com)\n" ;
    out << "# " << vertices.size() << " vertices,"
        << " " << indices_.size()/3 << " triangles.\n\n" ;
    out << "g block 0\n" ;
    // for vertices
    for (size_t i=0; i < vertices.size(); ++i) {
        vector3f *vert = vertices.at(i);
        out << "v "
            << vert->x << " "
            << vert->y << " "
            << vert->z << "\n" ;

    }

    // for normals
    for (size_t i=0; i < vertices.size(); ++i) {
        Vertex *vert = vertices.at(i);
        out << "vn "
            << vert->normal.x << " "
            << vert->normal.y << " "
            << vert->normal.z << "\n" ;

    }
    out << "g block 0\n" ;

    // for triangles
    for (size_t i=0; i < indices_.size()/3; ++i) {
        int idx = i*3;
        out << "f "
            << indices_[idx]+1 << "//" << indices_[idx]+1 << " "
            << indices_[idx+1]+1 << "//" << indices_[idx+1]+1 << " "
            << indices_[idx+2]+1 << "//" << indices_[idx+2]+1 << "\n" ;
    }
    out << "\n\n" ;
    out.close() ;
}

void Mesh::computeNormals() {
    cout << "Computing normals" << endl;
    const int nbtriangles = triangles.size();
    const int nbvertices = vertices.size();
    const int nbedges = edges.size();

    // angle weighted normal for edges
    cout << "Computing edge normals" << endl;
    for (int i=0; i<nbedges; i++) {
        Edge * e = edges[i];
        e->normal.zero();
        if (e->t[0]!=NULL) e->normal+=e->t[0]->normal;
        if (e->t[1]!=NULL) e->normal+=e->t[1]->normal;
    }

    for (int i=0; i<nbvertices; i++) {
        Vertex * v = vertices[i];
        v->normal.zero();
    }

    // loop over triangles and add weighted normal to vertices
    cout << "Computing vertex normals" << endl;
    for (int i=0; i<nbtriangles; i++) {
        Triangle * t = triangles[i];
        for (int j=0; j<3; j++) {
            vector3f e1 = *t->v[(j+1)%3] - *t->v[j];
            vector3f e2 = *t->v[(j+2)%3] - *t->v[j];
            e1.normalize();
            e2.normalize();
            float weight = acos(e1.dot(e2));
            t->v[j]->normal=weight*t->normal;
        }
    }

    for (int i=0; i<nbvertices; i++) {
        Vertex * v = vertices[i];
        v->normal.normalize();
    }
    cout << "Computing normals ended" << endl;
}
