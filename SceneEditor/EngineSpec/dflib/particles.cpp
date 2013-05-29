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


#include <iostream>
#include <cmath>
#include <cfloat>
#include <sstream>
#include <fstream>

#include "kdTree.h"
#include "particles.h"

#define EPSILON 0.000000001f
#define MYPI 3.14159265359f

using namespace std;

namespace
{

struct Kernels {

    static inline float spikykernel(const float R, const float r)
    {
        const float q = r/R;
        if (q>1.0f) return 0.0f;
        else {
            const float d = 1.0f-q;
            return 15.0f/(MYPI * R*R*R) * d * d * d;
        }
    }

};

}

inline float Particle::computeDensity(const vector<Particle> &particles) {
    const int nbneighbors = neighbors.size();
    rho = 0;
    for (int j=0; j<nbneighbors; j++) {
        const Particle &p = particles[neighbors[j].index];
        rho += p.m*Kernels::spikykernel(r, neighbors[j].r);
        if (p.hasghost) {
            rho+=p.m*Kernels::spikykernel(r, distance(p.ghostpos));
        }
    }
    return rho;
}

void Particles::createKdTree() {
    delete tree;
    // the 10 is the number of points in the leaf buckets
    tree = new KdTree(particles, particles.size(), 10);
    // we return by default maximally 1000 neighbors when
    // doing a range query.
    tree->setNOfNeighbours(1000);
}

Particles::~Particles()
{
    if (tree!=NULL) delete tree;
}

void Particles::computeNeighbors() {
    createKdTree();
    const int nbparticles = size();
    tree->setNOfNeighbours(30);
    for (int i=0; i<nbparticles; i++) {
        Particle &p = particles[i];
        const float sqrQueryDist = p.r*p.r;
        tree->queryRange(p, sqrQueryDist);
        const int nbfound = tree->getNOfFoundNeighbours();
        p.neighbors.clear();
        for (int j=0; j<nbfound; j++) {
            const float rr = tree->getSquaredDistance(j);
            const int index = tree->getNeighbourPositionIndex(j);
            p.neighbors.push_back(NeighborData(index,rr));
        }
    }
}

void Particles::computeParticlesMass() {
    const int nbparticles = size();
    float maxDensity = 0;
    float minDensity = FLT_MAX;
    // compute max density
    for (int i=0; i<nbparticles; i++) {
        Particle &p = particles[i];
        float density = p.computeDensity(particles);
        if (density>maxDensity) maxDensity = density;
        if (density<minDensity) minDensity = density;
    }
    // scale mass to correspond to given density
    for (int i=0; i<nbparticles; i++) {
        Particle &p = particles[i];
        p.m *= (p.rho0/maxDensity);
        p.V = p.m/p.rho0;
        if (p.rho==minDensity) {
            template_mass = p.m;
        }
    }
}

void Particles::writeToFile(string filename) {
    ofstream output(filename.c_str(), ios::out|ios::binary);
    const int nbparticles = size();
    //    output.write((char*)&nbparticles, sizeof(nbparticles));
    for (int i=0; i<nbparticles; i++) {
        const Particle &p = particles[i];
        output << p.x << " " << p.y << " " << p.z << endl;
        //        output.write((char*)&p.x, sizeof(float));
        //        output.write((char*)&p.y, sizeof(float));
        //        output.write((char*)&p.z, sizeof(float));
    }
    output.close();
    //    cout << "Written " <<nbparticles << " particles to " << filename << endl;
}
