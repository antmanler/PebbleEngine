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

#pragma once

#include "vector3f.h"

using namespace std;

struct NeighborData {

    NeighborData(){}
    NeighborData(int i, float _rr)
        : index(i)
        , rr(_rr)
        , r(sqrt(rr))
    {}

    int index;
    float rr; // squared distance
    float r;  // regular distance
};

class KdTree;

struct Particle : public vector3f {

    Particle() {}

    Particle(float x, float y, float z, float radius=1.0f, float density = 1000.0f)
        : vector3f(x,y,z)
        , m(1)
        , r(radius)//Config::samplingdistance*Config::smoothinglength)
        , rho0(density)
        , hasghost(false)
    {}

    Particle(const vector3f& point, float radius=1.0f, float density = 1000.0f)
        : vector3f(point.x, point.y, point.z)
        , m(1)
        , r(radius)//Config::samplingdistance*Config::smoothinglength)
        , rho0(density)
        , hasghost(false)
    {}

    float computeDensity(const vector<Particle> &particles) ;

    float m;

    // volume of particle
    float V;
    // density of particle
    float rho;
    // initial density of particle
    float rho0;

    // smoothing length of particle
    float r;

    // do i create a gost particle
    bool hasghost;

    // position of ghost
    vector3f ghostpos;

    // the particle neighbor indices
    vector<NeighborData> neighbors;

};

class Particles {

public:
    Particles()
        :template_mass(0.0f)
    {
        tree = NULL; // kdtree for particle neighbor searching
    }

    ~Particles();

    /**
     * Returns the number of particles.
     */
    int size() const {
        return particles.size();
    }

    /**
     * Compute for each particle its particle neighbors.
     */
    void computeNeighbors();

    /**
     * Create a k-d tree for neighbor particle searching.
     */
    void createKdTree();

    /**
     * Compute each particle's density.
     */
    void computeDensity();

    /**
     * Initially, all particles have mass 1. We call this method
     * to give each particle the appropriate mass based on its density.
     */
    void computeParticlesMass();

    /**
     * Simple binary dumping of the x,y,z coordinates
     * of the particles.
     */
    void writeToFile(string filename);

public:
    // vector of particles
    vector<Particle> particles;
    float template_mass;
private:
    // kdtree search data structure for particle neighbor searching
    KdTree *tree;
};
