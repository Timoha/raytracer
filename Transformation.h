#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include <vector>
#include <iostream>

#include <Eigen/Dense>


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
    friend Eigen::Vector4f operator* (const Transformation& x, const Eigen::Vector4f& y);
    friend Eigen::Vector4f operator* (const Eigen::Vector4f& x, const Transformation& y);

    Transformation getInverse();
    void compose(const vector<Transformation> &ts);

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
    out << t.matrix << endl;
    out << "Inverse Transpose" << endl;
    out << t.inverseTranspose << endl;
    return out;
}

Transformation operator* (const Transformation& x, const Transformation& y) {
    Transformation temp;

    temp.matrix = x.matrix * y.matrix;
    temp.inverseTranspose = x.inverseTranspose * y.inverseTranspose;

    return temp;
}

Eigen::Vector4f operator* (const Transformation& x, const Eigen::Vector4f& y) {
    return x.matrix * y;
}


Eigen::Vector4f operator* (const Eigen::Vector4f& x, const Transformation& y) {
    return y * x;
}


Transformation Transformation::getInverse() {
    Transformation temp;
    temp.matrix = (this->matrix).inverse();
    temp.inverseTranspose = (this->inverseTranspose).inverse(); // ? Not sure ??
    return temp;
}


void Transformation::compose(const vector<Transformation> &ts) {
    Transformation final;
    for (unsigned i = ts.size(); i-- > 0; ) {
        final = final * ts[i];
    }

    *this = final;
}


class Scaling : public Transformation
{
public:
    Scaling(float sx, float sy, float sz);

};

Scaling::Scaling(float sx, float sy, float sz) {
    matrix << sx, 0, 0, 0,
              0, sy, 0, 0,
              0, 0, sz, 0,
              0, 0, 0,  1;
    inverseTranspose = matrix.inverse().transpose();
}


class Translation : public Transformation
{
public:
    Translation(float tx, float ty, float tz);

};

Translation::Translation(float tx, float ty, float tz) {
    matrix << 1, 0, 0, tx,
              0, 1, 0, ty,
              0, 0, 1, tz,
              0, 0, 0, 1;
    inverseTranspose = matrix.inverse().transpose();
}


class Rotation : public Transformation
{
public:
    Rotation(float rx, float ry, float rz);
};



Rotation::Rotation(float rx, float ry, float rz) {
    Eigen::Vector3d k(rx, ry, rz);
    float theta = k.norm();
    k.normalize();


    Eigen::Matrix3f ux;
    ux << 0, -k[2], k[1],
          k[2], 0, -k[0],
          -k[1], k[0], 0;


    Eigen::Matrix3f I = Eigen::Matrix3f::Identity();

    float angle = theta*PI/180;

    Eigen::MatrixXf r = I + ux * sin(angle) + ux * ux * (1 - cos(angle));
    r.conservativeResize(4, 4);

    r(3, 3) = 1;

    matrix = r;

    inverseTranspose = matrix.inverse().transpose();
}


#endif