#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "Shape.h"
#include "Transformation.h"
#include "Color.h"

#include <iostream>
#include <vector>

using namespace std;


struct Material {
    Color specular, diffuse;
    float specularExponent;
};


class Primitive
{
public:
    Primitive() {};
    virtual bool isHit(const Ray& ray) {};
    virtual float intersect(const Ray& ray) {};
    virtual Material* getBRDF();
};


class GeometricPrimitive : public Primitive
{
private:
    Transformation objToWorld, worldToObj;
    Shape *shape;
    Material *material;
public:
    GeometricPrimitive(Shape* inShape, Material* inMaterial, Transformation inTransform);
    bool isHit(const Ray& ray);
    float intersect(const Ray& ray);
    Material* getBRDF() { return material; }
};


GeometricPrimitive::GeometricPrimitive(Shape *inShape, Material* inMaterial, Transformation inTransform) {
    shape = inShape;
    material = inMaterial;
    worldToObj = inTransform;
    objToWorld = inTransform.getInverse();
}


bool GeometricPrimitive::isHit(const Ray &ray) {
    return shape->isHit(ray);
}


float GeometricPrimitive::intersect(const Ray &ray) {
    return shape->intersect(ray);
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