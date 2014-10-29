#ifndef SHAPE_H
#define SHAPE_H

#include <iostream>
#include <math.h>

#include <Eigen/Dense>
#include "Ray.h"

#define EPSILON 0.00001f

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
    Sphere(const Eigen::Vector4d& inOrigin, double inRadius);
    ~Sphere();
    bool isHit(const Ray& ray) const;
    LocalGeo intersect(const Ray& ray) const;
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:
    Eigen::Vector4d origin;
    double radius;
};

Sphere::~Sphere() {
}


Sphere::Sphere() {
    origin = Eigen::Vector4d(0.0, 0.0, 0.0, 1.0);
    radius = 0.0;
}


Sphere::Sphere(const Eigen::Vector4d& inOrigin, double inRadius) {
    origin = inOrigin;
    radius = inRadius;
}


bool Sphere::isHit(const Ray& r) const {
    Eigen::Vector4d d = r.direction;
    Eigen::Vector4d ec = r.source - origin;

    double term1 = pow(d.dot(ec), 2);
    double term2 = d.dot(d) * (ec.dot(ec) - radius * radius);
    double discr = term1 - term2;

    return discr >= 0;
}

LocalGeo Sphere::intersect(const Ray& ray) const {
    Eigen::Vector4d d = ray.direction;
    Eigen::Vector4d ec = ray.source - origin;

    LocalGeo local;

    double b = d.dot(ec);
    double dItself = d.dot(d);


    double term1 = b * b;
    double term2 = dItself * (ec.dot(ec) - radius * radius);
    double discr = term1 - term2;

    if (discr < 0.0) {
        local.isHit = false;
    } else {
        double t1 = (-b - sqrt(discr)) / dItself;
        if (t1 < ray.t_min + EPSILON || t1 > ray.t_max) {
            local.isHit = false;
            return local;
        }
        local.point = ray.source + t1 * d;
        local.normal = local.point - origin;
        local.tHit = t1;
        local.isHit = true;
    }

//    double t2 = (-(d.dot(ec)) + sqrt(discr)) / (d.dot(d));
    return local;
}



class Triangle : public Shape
{
public:
    Triangle();
    Triangle(const Eigen::Vector3d& inVertexA, const Eigen::Vector3d& inVertexB, const Eigen::Vector3d& inVertexC);
    Triangle(const Eigen::Vector3d& inVertexA, const Eigen::Vector3d& inVertexB, const Eigen::Vector3d& inVertexC,
             const Eigen::Vector4d& inNormalA, const Eigen::Vector4d& inNormalB, const Eigen::Vector4d& inNormalC);
    bool isHit(const Ray& ray) const;
    LocalGeo intersect(const Ray& ray) const;
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:
    Eigen::Vector3d vertexA, vertexB, vertexC;
    Eigen::Vector4d normalA, normalB, normalC;
};

Triangle::Triangle(const Eigen::Vector3d& inVertexA, const Eigen::Vector3d& inVertexB, const Eigen::Vector3d& inVertexC) {
    vertexA = inVertexA;
    vertexB = inVertexB;
    vertexC = inVertexC;

    Eigen::Vector3d ab = vertexA - vertexB;
    Eigen::Vector3d ac = vertexA - vertexC;
    Eigen::Vector3d n = ab.cross(ac);
    Eigen::Vector4d defaultNormal(n[0], n[1], n[2], 0.0);

    normalA = defaultNormal;
    normalB = defaultNormal;
    normalC = defaultNormal;
}


Triangle::Triangle(const Eigen::Vector3d& inVertexA, const Eigen::Vector3d& inVertexB, const Eigen::Vector3d& inVertexC,
        const Eigen::Vector4d& inNormalA, const Eigen::Vector4d& inNormalB, const Eigen::Vector4d& inNormalC) {
    vertexA = inVertexA;
    vertexB = inVertexB;
    vertexC = inVertexC;
    normalA = inNormalA;
    normalB = inNormalB;
    normalC = inNormalC;
}

bool Triangle::isHit(const Ray& ray) const {
    LocalGeo local;
    Eigen::Vector3d d(ray.direction[0], ray.direction[1], ray.direction[2]);
    Eigen::Vector3d s(ray.source[0], ray.source[1], ray.source[2]);

    Eigen::Vector3d ab = vertexA - vertexB;
    Eigen::Vector3d ac = vertexA - vertexC;
    Eigen::Vector3d as = vertexA - s;
    Eigen::Vector3d n = ab.cross(ac);

    double M = d.dot(n);
    // TODO: check for parallel rays maybe??


    Eigen::Vector3d asCrossD = as.cross(d);

    double beta = ac.dot(-asCrossD) / M;
    if (beta < 0.0 || beta > 1.0) {
        return false;
    }

    double gamma = ab.dot(asCrossD) / M;
    if (gamma < 0.0 || gamma > 1.0 - beta) {
        return false;
    }

    return true;
}

LocalGeo Triangle::intersect(const Ray& ray) const {

    LocalGeo local;
    Eigen::Vector3d d(ray.direction[0], ray.direction[1], ray.direction[2]);
    Eigen::Vector3d s(ray.source[0], ray.source[1], ray.source[2]);

    Eigen::Vector3d ab = vertexA - vertexB;
    Eigen::Vector3d ac = vertexA - vertexC;
    Eigen::Vector3d as = vertexA - s;
    Eigen::Vector3d n = ab.cross(ac);

    double M = d.dot(n);
    if (abs(M) < EPSILON * EPSILON) {
        local.isHit = false;
        return local;
    }

    double t = as.dot(n) / M;
    if (t < ray.t_min + EPSILON || t > ray.t_max) {
        local.isHit = false;
        return local;
    }

    Eigen::Vector3d asCrossD = as.cross(d);

    double beta = ac.dot(-asCrossD) / M;
    if (beta < 0.0 || beta > 1.0) {
        local.isHit = false;
        return local;
    }

    double gamma = ab.dot(asCrossD) / M;
    if (gamma < 0.0 || gamma > 1.0 - beta) {
        local.isHit = false;
        return local;
    }


    local.isHit = true;
    local.point = ray.source + t * ray.direction;
    local.tHit = t;
    local.normal = (normalA * (1.0 - gamma - beta) + normalB * gamma + normalC * beta).normalized();

    return local;
}

#endif
