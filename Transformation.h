#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include <vector>
#include <iostream>

#include <Eigen/Dense>
#include <Eigen/StdVector>
#include "Ray.h"


#define PI 3.14159265

using namespace std;

class Transformation
{
public:
    Transformation();
    Transformation(const Transformation &rhs);
    Transformation& operator=(const Transformation &rhs);
    friend ostream& operator<< (ostream &out, Transformation &t);
    friend Transformation operator* (const Transformation& x, const Transformation& y);
    friend Ray operator* (const Transformation& x, const Ray& y);
    friend LocalGeo operator* (const Transformation& x, const LocalGeo& y);

    virtual Transformation* getInverse() const;
    Transformation* compose(const vector<Transformation*, Eigen::aligned_allocator<Transformation*> > &ts);

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

protected:
    Eigen::Matrix4f matrix, inverseTranspose;
};


Transformation::Transformation() {
    matrix = Eigen::Matrix4f::Identity();
    inverseTranspose = Eigen::Matrix4f::Identity();
}

Transformation::Transformation(const Transformation &rhs) {
    matrix = rhs.matrix;
    inverseTranspose = rhs.inverseTranspose;
}

Transformation& Transformation::operator=(const Transformation &rhs) {
    if (this == &rhs) {
        return *this;
    }

    matrix = rhs.matrix;
    inverseTranspose = rhs.inverseTranspose;
    return *this;
}


ostream& operator<< (ostream &out, Transformation &t) {
    out << "Matrix: " << endl;
    out << t.matrix;
    out << "Inverse Transpose: " << endl;
    out << t.inverseTranspose;
    return out;
}

Transformation operator* (const Transformation& x, const Transformation& y) {
    Transformation temp;
    temp.matrix = x.matrix * y.matrix;
    temp.inverseTranspose =  x.inverseTranspose * y.inverseTranspose;
    return temp;
}

Ray operator* (const Transformation& x, const Ray& y) {
    Ray temp = y;
    temp.direction = (x.matrix * y.direction).normalized();
    temp.source = x.matrix * y.source;
    return temp;
}


LocalGeo operator* (const Transformation& x, const LocalGeo& y) {
    LocalGeo temp = y;
    if (y.isHit) {
        temp.normal = x.inverseTranspose * y.normal;
        temp.normal[3] = 0.0f;
        temp.normal.normalize();
        temp.point = x.matrix * y.point;
    }
    return temp;
}


Transformation* Transformation::getInverse() const {
    Transformation* temp = new Transformation();
    temp->matrix = (this->inverseTranspose).transpose();
    temp->inverseTranspose = (this->matrix).transpose();
    return temp;
}


Transformation* Transformation::compose(const vector<Transformation*, Eigen::aligned_allocator<Transformation*> > &ts) {
    Transformation* final = new Transformation();
    for (unsigned i = 0; i < ts.size(); i++ ) {
        *final = *final * *ts[i];
    }

    return final;
}


class Scaling : public Transformation
{
public:
    Scaling(float sx, float sy, float sz);
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

Scaling::Scaling(float sx, float sy, float sz) {
    matrix << sx, 0.0f, 0.0f, 0.0f,
              0.0f, sy, 0.0f, 0.0f,
              0.0f, 0.0f, sz, 0.0f,
              0.0f, 0.0f, 0.0f, 1.0f;
    inverseTranspose = matrix.inverse().transpose();
}


class Translation : public Transformation
{
public:
    Translation(float tx, float ty, float tz);
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};


Translation::Translation(float tx, float ty, float tz) {
    matrix << 1.0f, 0.0f, 0.0f, tx,
              0.0f, 1.0f, 0.0f, ty,
              0.0f, 0.0f, 1.0f, tz,
              0.0f, 0.0f, 0.0f, 1.0f;
    inverseTranspose = matrix.inverse().transpose();
}


class Rotation : public Transformation
{
public:
    Rotation(float rx, float ry, float rz);
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};



Rotation::Rotation(float rx, float ry, float rz) {
    Eigen::Vector3f k(rx, ry, rz);
    float theta = k.norm();
    k.normalize();


    Eigen::Matrix4f ux;
    ux << 0.0f, -k[2], k[1], 0.0f,
          k[2], 0.0f, -k[0], 0.0f,
          -k[1], k[0], 0.0f, 0.0f,
          0.0f, 0.0f, 0.0f, 1.0f;


    Eigen::Matrix4f I = Eigen::Matrix4f::Identity();

    float angle = theta*PI/180;

    matrix = I + ux * sin(angle) + ux * ux * (1 - cos(angle));
    matrix(3, 3) = 1.0f;
    inverseTranspose = matrix.inverse().transpose();
}

#endif
