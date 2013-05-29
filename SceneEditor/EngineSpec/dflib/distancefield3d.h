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

#ifndef __DISTANCEFIELD3D_H__
#define __DISTANCEFIELD3D_H__

#include <vector>
#include <string>

#include "sigslot.h"
#include "mesh.h"
#include "vector3f.h"

using namespace std;
using namespace sigslot;
class KdTree;

class DistanceField3D {
public:

    // public signals
    typedef ISignal3<const char*, int, int> NotifySignal;

    DistanceField3D()
        : distances (0x0)
        , weights (0x0)
        , positions (0x0)
        , closestPoints (0x0)
        , closestNormals(0x0)
        , cancel_(false)
    {}

    DistanceField3D(int nbx_, int nby_, int nbz_);

    DistanceField3D(string filename): distances (0x0)
      , weights (0x0)
      , positions (0x0)
      , closestPoints (0x0)
      , closestNormals(0x0)
      , cancel_(false)
    {
        readFromFile(filename);
    }

    virtual ~DistanceField3D();

    void writeToFile(string filename);
    void readFromFile(string filename);

    void setDistance(int i, int j, int k, float d) {
        const int index = i+nbx*(j+nby*k);
        distances[index] = d;
    }

    float getDistance(int i, int j, int k) const {
        return distances[i+nbx*(j+nby*k)];
    }

    void distance(const vector3f &p, float &d) const {
        // get cell and lerp values
        vector3f fp = (p-p0)*invh;
        const int i = (int)floor(fp.x);
        const int j = (int)floor(fp.y);
        const int k = (int)floor(fp.z);
        if (i<0 || i>=nbx || j<0 || j>=nby || k<0 || k>=nbz) {
            d = -10;
            return;
        }
        vector3f ip(i,j,k);

        vector3f alphav = fp-ip;
        float alpha = alphav.x;
        float beta = alphav.y;
        float gamma = alphav.z;

        const float d000 = getDistance(i,j,k);
        const float d100 = getDistance(i+1,j,k);
        const float d010 = getDistance(i,j+1,k);
        const float d110 = getDistance(i+1,j+1,k);
        const float d001 = getDistance(i,j,k+1);
        const float d101 = getDistance(i+1,j,k+1);
        const float d011 = getDistance(i,j+1,k+1);
        const float d111 = getDistance(i+1,j+1,k+1);

        const float dx00 = (1.0f-alpha) * d000 + alpha * d100;
        const float dx10 = (1.0f-alpha) * d010 + alpha * d110;

        const float dxy0 = (1.0f-beta) * dx00 + beta * dx10;

        const float dx01 = (1.0f-alpha) * d001 + alpha * d101;
        const float dx11 = (1.0f-alpha) * d011 + alpha * d111;

        const float dxy1 = (1.0f-beta) * dx01 + beta * dx11;

        d = (1.0f-gamma) * dxy0 + gamma * dxy1;
    }

    void distance(const vector3f &p, float &d, vector3f &g) const {
        // get cell and lerp values
        vector3f fp = (p-p0)*invh;
        const int i = (int)floor(fp.x);
        const int j = (int)floor(fp.y);
        const int k = (int)floor(fp.z);
        if (i<0 || i>=nbx || j<0 || j>=nby || k<0 || k>=nbz) {
            d = +10;
            g.zero();
            return;
        }
        vector3f ip(i,j,k);

        vector3f alphav = fp-ip;
        float alpha = alphav.x;
        float beta = alphav.y;
        float gamma = alphav.z;

        const float d000 = getDistance(i,j,k);
        const float d100 = getDistance(i+1,j,k);
        const float d010 = getDistance(i,j+1,k);
        const float d110 = getDistance(i+1,j+1,k);
        const float d001 = getDistance(i,j,k+1);
        const float d101 = getDistance(i+1,j,k+1);
        const float d011 = getDistance(i,j+1,k+1);
        const float d111 = getDistance(i+1,j+1,k+1);

        const float dx00 = lerp(d000, d100, alpha);
        const float dx10 = lerp(d010, d110, alpha);
        const float dxy0 = lerp(dx00, dx10, beta);

        const float dx01 = lerp(d001, d101, alpha);
        const float dx11 = lerp(d011, d111, alpha);
        const float dxy1 = lerp(dx01, dx11, beta);

        const float d0y0 = lerp(d000, d010, beta);
        const float d0y1 = lerp(d001, d011, beta);
        const float d0yz = lerp(d0y0, d0y1, gamma);

        const float d1y0 = lerp(d100, d110, beta);
        const float d1y1 = lerp(d101, d111, beta);
        const float d1yz = lerp(d1y0, d1y1, gamma);

        const float dx0z = lerp(dx00, dx01, gamma);
        const float dx1z = lerp(dx10, dx11, gamma);

        g.x = d0yz - d1yz;
        g.y = dx0z - dx1z;
        g.z = dxy0 - dxy1;

        float length = g.normalize();
        if (length<0.0001f) g.zero();

        d = (1.0f-gamma) * dxy0 + gamma * dxy1;
    }

    const float* distance() const {
        return distances;
    }

    inline float lerp(float a, float b, float alpha) const {
        return (1.0f-alpha)*a + alpha *b;
    }

    float inside(Mesh *mesh, KdTree *kdtree, const vector3f &p);

    void translate(const vector3f &t);
    void scale(const vector3f &s);
    void invert();

    void meshToDistanceField(Mesh &mesh);
    void meshToDistanceField2(Mesh &mesh);
    void meshToDistanceField3(Mesh &mesh);
    void checkConsistency(int filter_radius);

    float distPointSegment(const vector3f &p, const vector3f &p0, const vector3f &p1, vector3f &result);
    float distPointPlane(const vector3f &p, const vector3f &o, const vector3f &n);
    float distanceToTriangle(const vector3f &p, Triangle *t);

    NotifySignal *sigNotify() {return &sig_notify_;}
    void cancel() {cancel_=true;}


    vector3f p0; // lower left front corner
    vector3f p1; // upper right back corner
    vector3f h;  // single cell sizes
    vector3f invh;   // inverse of single cell sizes
    int nbx,nby,nbz; // number of cells in all dimensions

private:
    float * distances;
    float * weights;
    vector3f * positions;
    vector3f * closestPoints;
    vector3f * closestNormals;

    float longestdiagonal;
    std::vector<vector3f> triangles;
    std::vector<vector3f> normals;

    signal3<const char*, int, int> sig_notify_;
    bool cancel_;
};

#endif
