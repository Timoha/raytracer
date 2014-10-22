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
    virtual bool isHit(const Ray& ray) {};
    virtual float intersect(const Ray& ray) {};
};




class Sphere : public Shape
{
public:
    Sphere();
    Sphere(Eigen::Vector4f inOrigin, float inRadius);
    bool isHit(const Ray& ray);
    float intersect(const Ray& ray);
private:
    Eigen::Vector4f origin;
    float radius;
};


Sphere::Sphere() {
    origin = Eigen::Vector4f(0, 0, 0, 1);
    radius = 0.0;
}


Sphere::Sphere(Eigen::Vector4f inOrigin, float inRadius) {
    origin = inOrigin;
    radius = inRadius;
}


bool Sphere::isHit(const Ray& r) {
    Eigen::Vector4f d = r.direction;
    Eigen::Vector4f ec = r.source - origin;

    float term1 = d.dot(ec) * d.dot(ec);
    float term2 = d.dot(d) * (ec.dot(ec) - radius*radius);
    float discr = term1 - term2;

    return discr >= 0;
}

float Sphere::intersect(const Ray& ray){
    Eigen::Vector4f d = r.direction;
    Eigen::Vector4f ec = r.source - origin;

//    LocalGeo intersection;

    float term1 = d.dot(ec) * d.dot(ec);
    float term2 = d.dot(d) * ec.dot(ec) - radius*radius;
    float discr = term1 - term2;

    float t1 = (-(d.dot(ec)) - sqrt(discr)) / (d.dot(d));
    // float t2 = (-(d.dot(ec)) + sqrt(discr)) / (d.dot(d));


    return t1;

//    intersection.position = r.source + t1 * r.direction;
//    intersection.normal = (intersection.position - origin).normalized();
//
//    return intersection;

}



//class Triangle : public Shape
//{
//public:
//    Triangle();
//    Triangle(Eigen::Vector4f inA, Eigen::Vector4f inB, Eigen::Vector4f inC);
//    // include implementation virtual functions for intersection
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
//Triangle::Triangle(Eigen::Vector4f inA, Eigen::Vector4f inB, Eigen::Vector4f inC) {
//    a = inA;
//    b = inB;
//    c = inC;
//}

#endif