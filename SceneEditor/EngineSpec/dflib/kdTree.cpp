// Title:   kdTree.cpp
// Created: Thu Sep 25 14:19:18 2003
// Authors: Richard Keiser, Oliver Knoll, Mark Pauly, Matthias Zwicker
//
// Copyright (c) 2001, 2002, 2003 Computer Graphics Lab, ETH Zurich
//
// This file is part of the Pointshop3D system.
// See http://www.pointshop3d.com/ for more information.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free
// Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
// MA 02111-1307, USA.
//
// Contact info@pointshop3d.com if any conditions of this
// licensing are not clear to you.
//


#include <stdlib.h>
#include <vector>
#include <cfloat>

#include "particles.h"
#include "kdTree.h"

using namespace std;

#define SWAP_POINTS(a,b) \
    Point tmp = points[a];\
    points[a] = points[b];\
    points[b] = tmp;


// ******************
// global definitions
// ******************
KdTree::KdTree(vector<vector3f> &positions, int nbpoints, const unsigned int maxBucketSize) {
    m_bucketSize	 = maxBucketSize;
    m_nOfPositions = nbpoints;
    m_points = new Point[nbpoints];
    m_neighbours	= NULL;
    m_nOfFoundNeighbours	= 0;
    m_nOfNeighbours = 0;
    m_queryPriorityQueue	= new PQueue();
    for (unsigned int i=0; i<nbpoints; i++) {
        m_points[i].pos = positions[i];
        m_points[i].index = i;
    }
    computeEnclosingBoundingBox(m_boundingBoxLowCorner, m_boundingBoxHighCorner);
    m_root = new KdNode();
    vector3f maximum, minimum;
    getSpread(m_points, nbpoints, maximum, minimum);
    createTree(*m_root, 0, nbpoints, maximum, minimum);
    setNOfNeighbours(1);
}

KdTree::KdTree(vector<Particle> &positions, int nbpoints, const unsigned int maxBucketSize) {
    m_bucketSize = maxBucketSize;
    //m_positions = positions;
    /*   {
   for (int i=0; i<positions.size(); i++) {
      m_positions.push_back(positions[i].getPosition());
   }
   }*/
    //m_positions				= positions;
    m_nOfPositions = nbpoints;
    m_points	 = new Point[nbpoints];
    m_neighbours = NULL;
    m_nOfFoundNeighbours = 0;
    m_nOfNeighbours = 0;
    m_queryPriorityQueue = new PQueue();
    for (unsigned int i=0; i<nbpoints; i++) {
        m_points[i].pos = positions[i];
        m_points[i].index = i;

    }
    computeEnclosingBoundingBox(m_boundingBoxLowCorner, m_boundingBoxHighCorner);
    m_root = new KdNode();
    vector3f maximum, minimum;
    getSpread(m_points, nbpoints, maximum, minimum);
    createTree(*m_root, 0, nbpoints, maximum, minimum);
    setNOfNeighbours(1);
}

KdTree::KdTree(KdTree * tree) {
    m_bucketSize			= tree->m_bucketSize;
    //m_positions = positions;

    //m_positions				= positions;
    m_nOfPositions			= tree->m_nOfPositions;
    m_points				= tree->m_points;
    m_neighbours			= NULL;
    m_nOfFoundNeighbours	= 0;
    m_nOfNeighbours			= 0;
    m_queryPriorityQueue	= new PQueue();
    /*for (unsigned int i=0; i<positions.size(); i++) {
                    m_points[i].pos = positions[i].getPosition();
                    m_points[i].index = i;

            }*/
    computeEnclosingBoundingBox(m_boundingBoxLowCorner, m_boundingBoxHighCorner);
    m_root = tree->m_root;
    //new KdNode();
    vector3f maximum, minimum;
    getSpread(m_points, m_nOfPositions, maximum, minimum);
    //createTree(*m_root, 0, m_nOfPositions, maximum, minimum);
    setNOfNeighbours(1);
}

KdTree::~KdTree() {
    delete m_root;
    delete[] m_points;
    if (m_neighbours) {
        delete m_neighbours;
    }
    delete m_queryPriorityQueue;
}

void KdTree::computeEnclosingBoundingBox(vector3f &lowCorner, vector3f &hiCorner) {
    vector3f tmp;
    hiCorner = lowCorner = m_points[0].pos;
    for (unsigned int i=1; i<m_nOfPositions; i++) {
        tmp = m_points[i].pos;
        if (hiCorner.x < tmp.x) {
            hiCorner.x = tmp.x;
        }
        else if (lowCorner.x > tmp.x) {
            lowCorner.x = tmp.x;
        }
        if (hiCorner.y < tmp.y) {
            hiCorner.y = tmp.y;
        }
        else if (lowCorner.y > tmp.y) {
            lowCorner.y = tmp.y;
        }
        if (hiCorner.z < tmp.z) {
            hiCorner.z = tmp.z;
        }
        else if (lowCorner.z > tmp.z) {
            lowCorner.z = tmp.z;
        }
    }
}

float KdTree::computeBoxDistance(const vector3f &q, const vector3f &lo, const vector3f &hi) {
    register float dist = 0.0;
    register float t;

    if (q.x < lo.x) {
        t = lo.x - q.x;
        dist = t*t;
    }
    else if (q.x > hi.x) {
        t = q.x - hi.x;
        dist = t*t;
    }
    if (q.y < lo.y) {
        t = lo.y - q.y;
        dist += t*t;
    }
    else if (q.y > hi.y) {
        t = q.y - hi.y;
        dist += t*t;
    }
    if (q.z < lo.z) {
        t = lo.z - q.z;
        dist += t*t;
    }
    else if (q.z > hi.z) {
        t = q.z - hi.z;
        dist += t*t;
    }

    return dist;
}

void KdTree::queryPosition(const vector3f &position) {

    if (!m_neighbours) {

        return;
    }
    vector3f g_queryOffsets(0,0,0);
    //g_queryOffsets[0] = g_queryOffsets[1] = g_queryOffsets[2] = 0.0;
    m_queryPriorityQueue->init();
    m_queryPriorityQueue->insert(-1, FLT_MAX);
    vector3f g_queryPosition = position;
    float dist = computeBoxDistance(position, m_boundingBoxLowCorner, m_boundingBoxHighCorner);
    m_root->queryNode(dist, m_queryPriorityQueue, g_queryPosition, g_queryOffsets);
    m_nOfFoundNeighbours = 0;

    for(int i=m_nOfNeighbours-1; i>=0; i--) {
        m_neighbours[i] = m_queryPriorityQueue->getMax();
        if (m_neighbours[i].index == -1) {
            break;
        }
        m_nOfFoundNeighbours++;
        m_queryPriorityQueue->removeMax();
    }


}

/*void KdTree::queryPosition2(const vector3f &position) {

        if (!m_neighbours) {

                return;
        }
        g_queryOffsets[0] = g_queryOffsets[1] = g_queryOffsets[2] = 0.0;
        m_queryPriorityQueue->init();
        m_queryPriorityQueue->insert(-1, FLT_MAX);
        g_queryPosition = position;
   cout << this << " before: " << g_queryPosition[0] << " " << g_queryPosition[1] << " " <<g_queryPosition[2] << " " << endl;
        float dist = computeBoxDistance(position, m_boundingBoxLowCorner, m_boundingBoxHighCorner);
        m_root->queryNode(dist, m_queryPriorityQueue);
        m_nOfFoundNeighbours = 0;

        for(int i=m_nOfNeighbours-1; i>=0; i--) {
                m_neighbours[i] = m_queryPriorityQueue->getMax();
                if (m_neighbours[i].index == -1) {
         break;
                }
                m_nOfFoundNeighbours++;
                m_queryPriorityQueue->removeMax();
        }
   cout << this << " after: " << g_queryPosition[0] << " " << g_queryPosition[1] << " " <<g_queryPosition[2] << " " << endl;
   //cout << this << " after: " << position << endl;
   
}*/

void KdTree::queryRange(const vector3f &position, const float maxSqrDistance) {

    if (!m_neighbours) {

        return;
    }
    vector3f g_queryOffsets(0,0,0);
    //g_queryOffsets[0] = g_queryOffsets[1] = g_queryOffsets[2] = 0.0;
    m_queryPriorityQueue->init();
    m_queryPriorityQueue->insert(-1, maxSqrDistance);
    vector3f g_queryPosition = position;

    float dist = computeBoxDistance(position, m_boundingBoxLowCorner, m_boundingBoxHighCorner);
    m_root->queryNode(dist, m_queryPriorityQueue, g_queryPosition, g_queryOffsets);
    m_nOfFoundNeighbours = 0;

    int cnt = m_queryPriorityQueue->getNofElements() - 1;

    while(cnt >= 0) {
        m_neighbours[cnt] = m_queryPriorityQueue->getMax();
        if (m_neighbours[cnt].index != -1) {
            m_nOfFoundNeighbours++;
        }
        m_queryPriorityQueue->removeMax();
        cnt--;
    }

}


void KdTree::setNOfNeighbours (const unsigned int newNOfNeighbours) {
    if (newNOfNeighbours != m_nOfNeighbours) {
        m_nOfNeighbours = newNOfNeighbours;
        if (m_neighbours) {
            delete[] m_neighbours;
        }
        m_queryPriorityQueue->setSize(m_nOfNeighbours);
        m_nOfNeighbours = newNOfNeighbours;
        m_neighbours = new 	Neighbour[m_nOfNeighbours];
        m_nOfFoundNeighbours = 0;
    }
}


void KdTree::createTree(KdNode &node, int start, int end, vector3f maximum, vector3f minimum) {
    int	mid;

    int n = end-start;
    vector3f diff = maximum - minimum;
    short dim;
    // get longest axe
    if (diff.x > diff.y) {
        if (diff.x > diff.z) {
            dim = 0;	//x-axe is longest axe
        }
        else {
            dim = 2;	// z-axe is longest axe
        }
    }
    else {
        if (diff.y > diff.z) {
            dim = 1;	// y-axe is longest axe
        }
        else {
            dim = 2;	// z-axe is longest axe
        }
    }

    node.m_dim = dim;
    float bestCut = (maximum[dim]+minimum[dim])/2.0;
    float min, max;
    getMinMax(m_points+start, n, dim, min, max);	// find min/max coordinates
    if (bestCut < min)		// slide to min or max as needed
        node.m_cutVal = min;
    else if (bestCut > max)
        node.m_cutVal = max;
    else
        node.m_cutVal = bestCut;

    int br1, br2;
    splitAtMid(m_points+start, n, dim, node.m_cutVal, br1, br2);	// permute points accordingly

    if (bestCut < min) mid = start+1;
    else if (bestCut > max) mid = end-1;
    else if (br1 > n/2.0) mid = start+br1;
    else if (br2 < n/2.0) mid = start+br2;
    else mid = start + (n>>1);

    Node** childNodes = new Node*[2];
    node.m_children = childNodes;
    if (mid-start <= m_bucketSize) {
        // new leaf
        KdLeaf* leaf = new KdLeaf();
        node.m_children[0] = leaf;
        leaf->m_points = (m_points+start);
        leaf->m_nOfElements = mid-start;
    }
    else {
        // new node
        KdNode* childNode = new KdNode();
        node.m_children[0] = childNode;
        float oldMax = maximum[dim];
        //maximum.set(dim,node.m_cutVal);
        maximum[dim] = node.m_cutVal;
        createTree(*childNode, start, mid, maximum, minimum);
        //maximum.set(dim,oldMax);
        maximum[dim] = oldMax;

    }

    if (end-mid <= m_bucketSize) {
        // new leaf
        KdLeaf* leaf = new KdLeaf();
        node.m_children[1] = leaf;
        leaf->m_points = (m_points+mid);
        leaf->m_nOfElements = end-mid;
    }
    else {
        // new node
        //minimum.set(dim,node.m_cutVal);
        minimum[dim] = node.m_cutVal;
        KdNode* childNode = new KdNode();
        node.m_children[1] = childNode;
        createTree(*childNode, mid, end, maximum, minimum);
    }
}

void KdTree::getSpread(Point* points, int nOfPoints, vector3f &maximum, vector3f &minimum) {
    vector3f pos = points->pos;
    maximum = vector3f(pos.x, pos.y, pos.z);
    minimum = vector3f(pos.x, pos.y, pos.z);
    points++;
    for (int i = 1; i < nOfPoints; i++) {
        pos = points->pos;
        if (pos.x < minimum.x) {
            minimum.x = pos.x;
        }
        if (pos.x > maximum.x) {
            maximum.x = pos.x;
        }
        if (pos.y < minimum.y) {
            minimum.y = pos.y;
        }
        if (pos.y > maximum.y) {
            maximum.y = pos.y;
        }
        if (pos.z < minimum.z) {
            minimum.z = pos.z;
        }
        if (pos.z > maximum.z) {
            maximum.z = pos.z;
        }
        points++;
    }
}

void KdTree::getMinMax(Point *points, int nOfPoints, int dim, float &mmin, float &mmax) {
    mmin = points->pos[dim];
    mmax = points->pos[dim];
    points++;
    for (int i=1; i<nOfPoints; i++) {
        if (points->pos[dim] < mmin) {
            mmin = points->pos[dim];
        }
        else if (points->pos[dim] > mmax) {
            mmax = points->pos[dim];
        }
        points++;
    }
}


void KdTree::splitAtMid(Point *points, int nOfPoints, int dim, float cutVal, int &br1, int &br2) {
    int l = 0;
    int r = nOfPoints-1;
    for(;;) {				// partition points[0..n-1] about the cut value
        while (l < nOfPoints && points[l].pos[dim] < cutVal) {
            l++;
        }
        while (r >= 0 && points[r].pos[dim] >= cutVal) {
            r--;
        }
        if (l > r)
            break;
        SWAP_POINTS(l,r);
        l++;
        r--;
    }
    br1 = l;			// now: points[0..br1-1] < cutVal <= points[br1..n-1]
    r = nOfPoints-1;
    for(;;) {				// partition points[br1..n-1] about cutVal
        while (l < nOfPoints && points[l].pos[dim] <= cutVal) {
            l++;
        }
        while (r >= br1 && points[r].pos[dim] > cutVal) {
            r--;
        }
        if (l > r)
            break;
        SWAP_POINTS(l,r);
        l++;
        r--;
    }
    br2 = l;			// now: points[br1..br2-1] == cutVal < points[br2..n-1]
}

void KdNode::queryNode(float rd, PQueue* queryPriorityQueue, vector3f &g_queryPosition, vector3f &g_queryOffsets) {
    register float old_off = g_queryOffsets[m_dim];
    register float new_off = g_queryPosition[m_dim] - m_cutVal;
    if (new_off < 0) {
        m_children[0]->queryNode(rd, queryPriorityQueue, g_queryPosition, g_queryOffsets);
        rd = rd - SQR(old_off) + SQR(new_off);
        if (rd < queryPriorityQueue->getMaxWeight()) {
            g_queryOffsets[m_dim] = new_off;
            m_children[1]->queryNode(rd, queryPriorityQueue, g_queryPosition, g_queryOffsets);
            g_queryOffsets[m_dim] = old_off;
        }
    }
    else {
        m_children[1]->queryNode(rd, queryPriorityQueue, g_queryPosition, g_queryOffsets);
        rd = rd - SQR(old_off) + SQR(new_off);
        if (rd < queryPriorityQueue->getMaxWeight()) {
            g_queryOffsets[m_dim] = new_off;
            m_children[0]->queryNode(rd, queryPriorityQueue, g_queryPosition, g_queryOffsets);
            g_queryOffsets[m_dim] = old_off;
        }
    }

}

void KdLeaf::queryNode(float rd, PQueue* queryPriorityQueue, vector3f &g_queryPosition, vector3f &g_queryOffsets) {
    float sqrDist;
    //use pointer arithmetic to speed up the linear traversing
    Point* point = m_points;
    for (register unsigned int i=0; i<m_nOfElements; i++) {
        sqrDist = (point->pos - g_queryPosition).getSquaredLength();
        if (sqrDist < queryPriorityQueue->getMaxWeight()) {
            queryPriorityQueue->insert(point->index, sqrDist);
        }
        point++;
    }
}

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
