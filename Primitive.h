#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "Shape.h"
#include "Transformation.h"
#include "Color.h"
#include "Ray.h"

#include <iostream>
#include <vector>

using namespace std;


class Material {
    Color specular, diffuse;
    float specularExponent;
};


class Primitive
{
public:


    class Intersection {
    public:
        Primitive* primitive;
        LocalGeo* local;
    };

    Primitive() {};
    virtual const bool isHit(const Ray& ray) = 0;
    virtual const Intersection intersect(const Ray& ray) = 0;
    virtual Material* getBRDF() = 0;
};


class GeometricPrimitive : public Primitive
{
private:
    Transformation objToWorld, worldToObj;
    Shape *shape;
    Material *material;
public:
    GeometricPrimitive(Shape* inShape, Material* inMaterial, Transformation inTransform);
    const bool isHit(const Ray& ray);
    const Intersection intersect(const Ray& ray);
    Material* getBRDF() { return material; }
};


GeometricPrimitive::GeometricPrimitive(Shape *inShape, Material* inMaterial, Transformation inTransform) {
    shape = inShape;
    material = inMaterial;
    worldToObj = inTransform;
    objToWorld = inTransform.getInverse();
}


const bool GeometricPrimitive::isHit(const Ray &ray) {
    return shape->isHit(ray);
}


const Intersection GeometricPrimitive::intersect(const Ray &ray) {
    Intersection inter;
    inter.primitive = this;
    inter.local = &(shape->intersect(ray));
    return inter;
}



//class AggregatePrimitive : public Primitive
//{
//
///// NOT IMPLEMENTED
//private:
//    vector<Primitive*> primitives;
//public:
//    AggregatePrimitive(vector<Primitive*> &list);
//    bool isHit(const Ray& ray) { } // not implemented
//    float intersect(const Ray& ray) { } // not impelemented
//    Material* getBRDF() { exit(1); }
//};
//
//AggregatePrimitive::AggregatePrimitive(vector<Primitive*> &list) {
//    primitives = list;
//}

#endif