#ifndef AABBNODE_H
#define AABBNODE_H

#include <stdexcept>
#include <limits>
#include <vector>
#include <iostream>
#include <algorithm>
#include <iterator>

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
    vector<const AABBNode*> children;
    bool isIntersect(const Ray& r) const;
    void getRelevantPrimitives(const AABBNode* node, const Ray& r, vector<const Primitive*>& result) const;
    const AABBNode* constructTree(vector<AABBNode*>::iterator start, vector<AABBNode*>::iterator end, int axis);
    friend bool sortByX(AABBNode* a, AABBNode* b);
    friend bool sortByY(AABBNode* a, AABBNode* b);
    friend bool sortByZ(AABBNode* a, AABBNode* b);

public:
    AABBNode();
    void constructTree(vector<Primitive*>& prims);
    ~AABBNode();
    void addNode(const AABBNode* node);
    void setPrimitive(Primitive* p);
    const vector<const Primitive*> getRelevantPrimitives(const Ray& r) const;

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};


const vector<const Primitive*> AABBNode::getRelevantPrimitives(const Ray& r) const {
    vector<const Primitive*> result;
    getRelevantPrimitives(this, r, result);
    return result;
}


void AABBNode::getRelevantPrimitives(const AABBNode* node, const Ray& r, vector<const Primitive*>& result) const {
    if (node == NULL) {
        return;
    }


    if (!node->isIntersect(r)) {
        return;
    }

    if (node->primitive != NULL) {
        result.push_back(node->primitive);
        return;
    }

    for(int i = 0; i < (node->children).size(); i++) {
        getRelevantPrimitives((node->children)[i], r, result);
    }
}



AABBNode::AABBNode() {
    primitive = NULL;
    minV << numeric_limits<double>::infinity(), numeric_limits<double>::infinity(), numeric_limits<double>::infinity(), 1.0;
    maxV << -numeric_limits<double>::infinity(), -numeric_limits<double>::infinity(), -numeric_limits<double>::infinity(), 1.0;
}



bool sortByX(AABBNode* a, AABBNode* b) {
    return a->minV[0] < b->minV[0];
}

bool sortByY(AABBNode* a, AABBNode* b) {
    return a->minV[1] < b->minV[1];
}

bool sortByZ(AABBNode* a, AABBNode* b) {
    return a->minV[2] < b->minV[2];
}


void AABBNode::constructTree(vector<Primitive*>& prims) {
    vector<AABBNode*> boxes;
    for (int i = 0; i < prims.size(); i++) {
        AABBNode* newNode = new AABBNode();
        newNode->setPrimitive(prims[i]);
        boxes.push_back(newNode);
    }

    *this = *constructTree(boxes.begin(), boxes.end(), 0);
}


// CREDITS:
// tree heuristing is adapted from http://www.cs.utah.edu/~bes/papers/fastRT/paper-node8.html
const AABBNode* AABBNode::constructTree(vector<AABBNode*>::iterator start, vector<AABBNode*>::iterator end, int axis) {


    AABBNode* rootNode = new AABBNode();

    if (end - start <= 0) {
        return NULL;
    }

    if (end - start == 1) {
        return *start;
    }


    rootNode->primitive = NULL;

    if (axis == 0) {
        sort(start, end, sortByX);
    } else if (axis == 1) {
        sort(start, end, sortByY);
    } else if (axis == 2) {
        sort(start, end, sortByZ);
    }

    axis = (axis + 1) % 3;

    vector<AABBNode*>::iterator endFirstHalf = start + (end - start) / 2;
    const AABBNode* leftNode = constructTree(start, endFirstHalf, axis);
    const AABBNode* rightNode = constructTree(endFirstHalf, end, axis);

    rootNode->addNode(leftNode);
    rootNode->addNode(rightNode);

    return rootNode;
}

AABBNode::~AABBNode() {
    for (int i = 0; i < children.size(); i++) {
        delete children[i];
    }
}

void AABBNode::setPrimitive(Primitive *p) {
    if (!children.empty()) {
        throw logic_error("It's illigal to set primitive for non-leaf AABBnode.");
    }

    primitive = p;

    p->getAABB(minV, maxV);
}

void AABBNode::addNode(const AABBNode* node) {
    if (primitive != NULL) {
        throw logic_error("It's illigal to add AABBnodes to leaf node.");
    }

    children.push_back(node);

    setExtrems(minV, node->minV, maxV, node->maxV);
}


// CREDITS:
// CODE FOR THIS FUNCTION WAS ADAPTED FROM
// THE FOLLOWING BLOG POST http://tavianator.com/2011/05/fast-branchless-raybounding-box-intersections/
bool AABBNode::isIntersect(const Ray& ray) const {
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