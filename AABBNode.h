#ifndef AABBNODE_H
#define AABBNODE_H

#include <stdexcept>
#include <limits>
#include <vector>
#include <iostream>

#include <Eigen/Dense>
#include <Eigen/StdVector>

#include "Ray.h"
#include "Shape.h"
#include "Primitive.h"
#include "Transformation.h"

using namespace std;

class AABBNode {

private:
    Eigen::Vector4d minV, maxV;
    Primitive* primitive;
    vector<AABBNode*> children;
    void calculateAABB(const GeometricPrimitive* gp, Eigen::Vector4d& currMin, Eigen::Vector4d& currMax);
    void calculateAABB(const AggregatePrimitive* ap, Eigen::Vector4d& currMin, Eigen::Vector4d& currMax);
    void setExtrems(Eigen::Vector4d& currMin, const Eigen::Vector4d& newMin, Eigen::Vector4d& currMax, const Eigen::Vector4d& newMax) const;
    vector<Eigen::Vector4d, Eigen::aligned_allocator<Eigen::Vector4d> > calcObjSpaceAABB(const Sphere* sp) const;
    vector<Eigen::Vector4d, Eigen::aligned_allocator<Eigen::Vector4d> > calcObjSpaceAABB(const Triangle* tr) const;
    bool isIntersect(const Ray& r) const;
public:
    AABBNode();
    ~AABBNode();
    void addNode(const AABBNode* node);
    void setPrimitive(const Primitive* p);
    const vector<Primitive*>& getRelevantPrimitives(const Ray& r) const;
};

void setExtrems(Eigen::Vector4d& currMin, const Eigen::Vector4d& newMin, Eigen::Vector4d& currMax, const Eigen::Vector4d& newMax) const {
    for (int i = 0; i < 3; i++) {
        currMin[i] = fmin(currMin[i], newMin);
        currMax[i] = fmax(currMax[i], newMax);
    }
}


vector<Eigen::Vector4d, Eigen::aligned_allocator<Eigen::Vector4d> >
AABBNode::calcObjSpaceAABB(const Sphere* sp) const {
    vector<Eigen::Vector4d, Eigen::aligned_allocator<Eigen::Vector4d> > corners;

    Eigen::Vector4d o = sp->getOrigin();
    double r = sp->getRadius();
    Eigen::Vector4d maxCorner = Eigen::Vector4d(r, r, r, 0.0);
    corners.push_back(maxCorner + o); // +, +, +

    Eigen::Vector4d temp = maxCorner;
    for (int i = 0; i < 5; i++) { // (-, +, +); (-, -, +); (-, -, -); (+, -, -); (+, +, -);
        temp[i % 3] *= -1;
        corners.push_back(temp + o);
    }

    temp[0] *= -1;
    corners.push_back(temp + o); // (-, +, -);

    corners.push_back(-temp + o); // (+, -, +);

    return corners;
}


vector<Eigen::Vector4d, Eigen::aligned_allocator<Eigen::Vector4d> >
AABBNode::calcObjSpaceAABB(const Triangle* tr) const {
    Eigen::Vector4d maxCorner, minCorner;
    vector<Eigen::Vector4d, Eigen::aligned_allocator<Eigen::Vector4d> > corners;
    // find extremes
    for (int i = 0; i < maxCorner.size() - 1; i++) {
        maxCorner[i] = fmax((tr->getA())[i], fmax((tr->getB())[i], (tr->getC())[i]));
        minCorner[i] = fmin((tr->getA())[i], fmin((tr->getB())[i], (tr->getC())[i]));
    }

    // for affine transforms
    maxCorner[3] = 1.0f;
    minCorner[3] = 1.0f;
    corners.push_back(maxCorner);
    corners.push_back(minCorner);


    // find other corners
    Eigen::Vector4d lbf;
    lbf << minCorner[0], minCorner[1], maxCorner[2], 1.0;
    corners.push_back(lbf);

    Eigen::Vector4d rbf;
    rbf << maxCorner[0], minCorner[1], maxCorner[2], 1.0;
    corners.push_back(rbf);

    Eigen::Vector4d rbb;
    rbb << maxCorner[0], minCorner[1], minCorner[2], 1.0;
    corners.push_back(rbb);

    Eigen::Vector4d rtb;
    rtb << maxCorner[0], maxCorner[1], minCorner[2], 1.0;
    corners.push_back(rtb);

    Eigen::Vector4d ltb;
    ltb << minCorner[0], maxCorner[1], minCorner[2], 1.0;
    corners.push_back(ltb);

    Eigen::Vector4d ltf;
    ltf << minCorner[0], maxCorner[1], maxCorner[2], 1.0;
    corners.push_back(ltf);

    return corners;
}


AABBNode::calculateAABB(const GeometricPrimitive* gp, Eigen::Vector4d& currMin, Eigen::Vector4d& currMax) {
    vector<Eigen::Vector4d, Eigen::aligned_allocator<Eigen::Vector4d> > corners;
    corners = calcObjSpaceAABB(gp->getShape());
    Transformation* otw = gp->getObjToWorld();

    double minX = numeric_limits<double>::infinity();
    double minY = minX;
    double minZ = minX;
    double maxX = -numeric_limits<double>::infinity();
    double maxY = maxX;
    double maxZ = maxX;

    // apply objectToWorld transformation and find extremes
    for (int i = 0; i < corners.size(); i++) {
        corners[i] = otw * corners[i];
        if (corners[i][0] > maxX) {
            maxX = corners[i][0];
        }

        if (corners[i][1] > maxY) {
            maxY = corners[i][1];
        }

        if (corners[i][2] > maxZ) {
            maxZ = corners[i][2];
        }

        if (corners[i][0] < minX) {
            minX = corners[i][0];
        }

        if (corners[i][1] < minY) {
            minY = corners[i][1];
        }

        if (corners[i][2] < minZ) {
            minZ = corners[i][2];
        }
    }

    setExtrems(minV, Eigen::Vector4d(minX, minY, minZ, 1.0), maxV, Eigen::Vector4d(maxX, maxY, maxZ, 1.0));
}


void AABBNode::calculateAABB(const AggregatePrimitive* ap, Eigen::Vector4d& currMin, Eigen::Vector4d& currMax) {
    vector<Primitive*> primitives = ap->getPrimitives();

    for(int i = 0; i < primitives.size; i++) {
        calculateAABB(primitives[i], currMin, currMax);
    }
}


AABBNode::AABBNode() {
    primitive = NULL;
    minV << numeric_limits<double>::infinity(), numeric_limits<double>::infinity(), numeric_limits<double>::infinity(), 1.0;
    maxV << -numeric_limits<double>::infinity(), -numeric_limits<double>::infinity(), -numeric_limits<double>::infinity(), 1.0;
}

AABBNode::~AABBNode() {
    for (vector<AABBNode*>::iterator it = v.children(); it != v.children(); ++it) {
        delete *it;
    }
}

void AABBNode::setPrimitive(const Primitive *p) {
    if (!children.empty()) {
        throw logic_error("It's illigal to set primitive for non-leaf AABBnode.");
    }

    primitive = p;

    calculateAABB(p, minV, maxV);
}

void AABBNode::addNode(const AABBNode* node) {
    if (primitive != NULL) {
        throw logic_error("It's illigal to add AABBnodes to leaf node.");
    }

    children.push_back(node);

    setExtrems(minV, node->minV, maxV, node->maxV);
}


// CREDITS:
// CODE FOR THIS FUNCTION WAS TAKEN FROM
// THE FOLLOWING BLOG POST http://tavianator.com/2011/05/fast-branchless-raybounding-box-intersections/
bool isIntersect(const Ray& ray) const {
    double tmin = -numeric_limits<double>::infinity(), tmax = numeric_limits<double>::infinity();


    Eigen::Vector4d t1, t2;

    t1 = minV - ray.source;
    t2 = maxV - ray.source;

    t1[0] /= ray.direction[0];
    t2[0] /= ray.direction[0];

    tmin = fmax(tmin, fmin(t1[0], t2[0]));
    tmax = fmin(tmax, fmax(t1[0], t2[0]));

    t1[1] /= ray.direction[1];
    t2[1] /= ray.direction[1];

    tmin = fmax(tmin, fmin(t1[1], t2[1]));
    tmax = fmin(tmax, fmax(t1[1], t2[1]));

    t1[2] /= ray.direction[2];
    t2[2] /= ray.direction[2];

    tmin = fmax(tmin, fmin(t1[2], t2[2]));
    tmax = fmin(tmax, fmax(t1[2], t2[2]));

    return tmax >= fmax(ray.t_min, tmin); // don't know
}



#endif