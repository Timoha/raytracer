#ifndef SHAPE_H
#define SHAPE_H

#include <iostream>
#include <math.h>

#include <Eigen/Dense>
#include "Ray.h"

using namespace std;

class Shape
{
public:
    Shape() {};
    virtual ~Shape() = 0;
    virtual bool isHit(const Ray& ray) const = 0;
    virtual LocalGeo intersect(const Ray& ray) const = 0;
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};


inline Shape::~Shape() { }




class Sphere : public Shape
{
public:
    Sphere();
    Sphere(const Eigen::Vector4f& inOrigin, float inRadius);
    ~Sphere();
    bool isHit(const Ray& ray) const;
    LocalGeo intersect(const Ray& ray) const;
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:
    Eigen::Vector4f origin;
    float radius;
};

Sphere::~Sphere() {
}


Sphere::Sphere() {
    origin = Eigen::Vector4f(0, 0, 0, 1);
    radius = 0.0;
}


Sphere::Sphere(const Eigen::Vector4f& inOrigin, float inRadius) {
    origin = inOrigin;
    radius = inRadius;
}


bool Sphere::isHit(const Ray& r) const {
    Eigen::Vector4f d = r.direction;
    Eigen::Vector4f ec = r.source - origin;

    float term1 = pow(d.dot(ec), 2);
    float term2 = d.dot(d) * (ec.dot(ec) - radius * radius);
    float discr = term1 - term2;

    return discr >= 0;
}

LocalGeo Sphere::intersect(const Ray& ray) const {
    Eigen::Vector4f d = ray.direction;
    Eigen::Vector4f ec = ray.source - origin;

    LocalGeo local;

    float b = d.dot(ec);
    float dItself = d.dot(d);


    float term1 = b * b;
    float term2 = dItself * (ec.dot(ec) - radius * radius);
    float discr = term1 - term2;

    if (discr < 0.0f) {
        local.isHit = false;
        local.tHit = 0.0f;
    } else {
        float t1 = (-b - sqrt(discr)) / dItself;
        local.point = ray.source + t1 * d;
        local.normal = (local.point - origin).normalized();
        local.tHit = t1;
        local.isHit = true;
    }

    // float t2 = (-(d.dot(ec)) + sqrt(discr)) / (d.dot(d));

    return local;
}



//class Triangle : public Shape
//{
//public:
//    Triangle();
//    Triangle(const Eigen::Vector4f inA, const Eigen::Vector4f& inB, const Eigen::Vector4f& inC);
//    // include implementation virtual functions for intersection
//    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
//private:
//    Eigen::Vector4f a, b, c;
//};
//
//
//Triangle::Triangle() {
//    Eigen::Vector4f zero(0, 0, 0, 1);
//    a = zero;
//    b = zero;
//    c = zero;
//}
//
//
//Triangle::Triangle(const Eigen::Vector4f& inA, const Eigen::Vector4f& inB, const Eigen::Vector4f& inC) {
//    a = inA;
//    b = inB;
//    c = inC;
//}

#endif
