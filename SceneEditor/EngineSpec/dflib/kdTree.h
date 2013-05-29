// Title:   kdTree.h
// Created: Thu Sep 25 14:29:38 2003
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

#ifndef __KDTREE_H_
#define __KDTREE_H_

#include <vector>
#include "vector3f.h"
#include "PriorityQueue.h"

using namespace std;

#define SQR(x) (x*x)

typedef MaxPriorityQueue<int, float> PQueue;
typedef PQueue::Element Neighbour;

struct Particle;

typedef struct point {
    vector3f    pos;
    int         index;
} Point;


/**
 * abstract node class
 * base class for leaves and nodes
 *
 * @author Richard Keiser
 * @version 2.0
 */
class Node {
public:
    virtual ~Node() {}
    /**
         * look for the nearest neighbours
         * @param rd
         *		  the distance of the query position to the node box
         * @param queryPriorityQueue
         *		  a priority queue
         */
    virtual void queryNode(float rd, PQueue* queryPriorityQueue, vector3f &g_queryPosition, vector3f &g_queryOffsets) = 0;
};


/**
 * Node of a kd tree. The node stands for a box in space.
 *
 * @author Richard Keiser
 * @version 2.0
 */
class KdNode : public Node {
public:
    /**
         * deletes the children of the node
         */
    virtual ~KdNode() {
        delete m_children[0];
        delete m_children[1];
        delete[] m_children;
    }

    /**
         * array of child nodes
         */
    Node**  m_children;
    /**
         * cut value of the splitting plane
         */
    float  m_cutVal;
    /*
         * actual dimension of this node
         */
    unsigned char  m_dim;

    /**
         * look for the nearest neighbours
         * @param rd
         *		  the distance of the query position to the node box
         * @param queryPriorityQueue
         *		  a priority queue
         */
    void queryNode(float rd, PQueue* queryPriorityQueue, vector3f &g_queryPosition, vector3f &g_queryOffsets);
};


/**
 * A leaf node of the tree (bucket node) containing the primitives
 *
 * @author Richard Keiser
 * @version 2.0
 */
class KdLeaf : public Node {

public:
    /**
         * the primitives of this leaf
         */
    Point*  m_points;
    /**
         * the number of elements in this leaf
         */
    unsigned int	m_nOfElements;

    /**
         * look for the nearest neighbours
         * @param rd
         *		  the distance of the query position to the node box
         * @param queryPriorityQueue
         *		  a priority queue
         */
    void queryNode(float rd, PQueue* queryPriorityQueue, vector3f &g_queryPosition, vector3f &g_queryOffsets);

    virtual ~KdLeaf() {}
};


/**
 * An efficient k-d tree for 3 dimensions
 * It is very similar to the k-d tree
 * described by Arya and Mount in ``Algorithms
 *	for fast vector quantization,'' Proc.  of DCC '93: Data Compression
 *	Conference, eds. J. A. Storer and M. Cohn, IEEE Press, 1993, 381-390
 *  and their ANN software library
 *
 * @author Richard Keiser
 * @version 2.0
 */
class KdTree {

public:
    /**
         * Creates a k-d tree from the positions
         *
         * @param positions
         *			point positions
         * @param nOfPositions
         *			number of points
         * @param maxBucketSize
         *			number of points per bucket
         */
    KdTree(vector<Particle> &positions, int nbpoints,  const unsigned int maxBucketSize);
    KdTree(vector<vector3f> &positions, int nbpoints,  const unsigned int maxBucketSize);
    KdTree(KdTree * tree);
    /**
         * Destructor
         */
    ~KdTree();

    /**
         * look for the nearest neighbours at <code>position</code>
         *
         * @param position
         *			the position of the point to query with
         */
    void queryPosition(const vector3f &position);
    //void queryPosition2(const vector3f &position);
    /**
         * look for the nearest neighbours with a maximal squared distance <code>maxSqrDistance</code>.
         * If the set number of neighbours is smaller than the number of neighbours at this maximum distance,
         * this call is equal to queryPosition but may be faster if the maximum distance is close to the distance
         * of the nearest neighbour with the largest distance.
         *
         * @param position
         *			the position of the point to query with
         * @param maxSqrDistance
         *			the maximal squared distance of a nearest neighbour
         *
         */
    void queryRange(const vector3f &position, const float maxSqrDistance);
    /**
         * set the number of nearest neighbours which have to be looked at for a query
         *
         * @params newNOfNeighbours
         *			the number of nearest neighbours
         */
    void setNOfNeighbours (const unsigned int newNOfNeighbours);
    /**
         * get the index of the i-th nearest neighbour to the query point
         * i must be smaller than the number of found neighbours
         *
         * @param i
         *			index of the nearest neighbour
         * @return the index of the i-th nearest neighbour
         */
    inline unsigned int getNeighbourPositionIndex (const unsigned int i);
    /**
         * get the position of the i-th nearest neighbour
         * i must be smaller than the number of found neighbours
         *
         * @param i
         *			index of the nearest neighbour
         * @return the position of the i-th nearest neighbour
         */
    inline vector3f getNeighbourPosition(const unsigned int i);
    /**
         * get the squared distance of the query point and its i-th nearest neighbour
         * i must be smaller than the number of found neighbours
         *
         * @param i
         *			index of the nearest neighbour
         * @return the squared distance to the i-th nearest neighbour
         */
    inline float getSquaredDistance (const unsigned int i);
    /**
         * get the number of found neighbours
         * Generally, this is equal to the number of query neighbours
         * except for range queries, where this number may be smaller than the number of query neigbhbours
         *
         * @return the number of found neighbours
         */
    inline unsigned int getNOfFoundNeighbours();

protected:
    /**
         * compute distance from point to box
         *
         * @param q
         *		the point position
         * @param lo
         *		low point of box
         * @param hi
         *		high point of box
         * @return the distance to the box
         */
    float computeBoxDistance(const vector3f &q, const vector3f &lo, const vector3f &hi);
    /**
         * computes the enclosing box of the points
         *
         * @param lowCorner
         *			the low corner of the enclosing box is returned
         * @param hiCorner
         *			the high corner of the enclosing box is returned
         */
    void computeEnclosingBoundingBox(vector3f &lowCorner, vector3f &hiCorner);

    /**
         * creates the tree using the sliding midpoint splitting rule
         *
         * @param node
         *		  the node to split
         * @param start
         *		  first index in the data array
         * @param end
         *		  last index in the data array
         * @param maximum
         *		  maximum coordinates of the data points
         * @param minimum
         *		  minimum coordinates of the data points
         */
    void createTree(KdNode &node, int start, int end, vector3f maximum, vector3f minimum);


private:

    Point*						m_points;
    //vector<vector3f>			m_positions;
    Neighbour*					m_neighbours;
    int							m_bucketSize;
    KdNode*						m_root;
    unsigned int				m_nOfFoundNeighbours,
    m_nOfNeighbours,
    m_nOfPositions;
    PQueue*						m_queryPriorityQueue;
    vector3f					m_boundingBoxLowCorner;
    vector3f					m_boundingBoxHighCorner;

    // gets the minimum and maximum value of all points at dimension dim
    void getMinMax(Point *points, int nOfPoints, int dim, float &min, float &max);
    // splits the points such that on return for all points:
    //		points[0..br1-1] < cutVal
    //		points[br1-1..br2-1] == cutVal
    //		points[br2..nOfPoints-1] > cutVal
    void splitAtMid(Point *points, int nOfPoints, int dim, float cutVal, int &br1, int &br2);
    // get the axis aligned bounding box of points
    void getSpread(Point* points, int nOfPoints, vector3f &maximum, vector3f &minimum);
};

inline unsigned int KdTree::getNOfFoundNeighbours() {
    return m_nOfFoundNeighbours;
}

inline unsigned int KdTree::getNeighbourPositionIndex(const unsigned int neighbourIndex) {
    return m_neighbours[neighbourIndex].index;
}

inline vector3f KdTree::getNeighbourPosition(const unsigned int neighbourIndex) {
    return m_points[m_neighbours[neighbourIndex].index].pos;
}

inline float KdTree::getSquaredDistance (const unsigned int neighbourIndex) {
    return m_neighbours[neighbourIndex].weight;
}

#endif

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
