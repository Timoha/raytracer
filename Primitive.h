#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "Shape.h"
#include "Transformation.h"
#include "Color.h"
#include "Ray.h"

#include <iostream>
#include <vector>

using namespace std;

class Primitive;

class Material {
public:
    Material();
    Color ambient, specular, diffuse, reflective;
    float specularExponent;
};


Material::Material() {
    Color black(0.0f, 0.0f, 0.0f);
    ambient = black;
    specular = black;
    diffuse = black;
    reflective = black;
    specularExponent = 0.0f;
}



class Intersection {
public:
    const Primitive* primitive;
    LocalGeo local;
};

class Primitive
{
public:
    Primitive() {};
    virtual bool isHit(const Ray& ray) const = 0;
    virtual Intersection intersect(const Ray& ray) const = 0;
    virtual Material* getBRDF() const = 0;
};


class GeometricPrimitive : public Primitive
{
private:
    Transformation objToWorld, worldToObj;
    Shape *shape;
    Material *material;
public:
    GeometricPrimitive(Shape* inShape, Material* inMaterial, Transformation& inTransform);
    bool isHit(const Ray& ray) const;
    Intersection intersect(const Ray& ray) const;
    Material* getBRDF() const { return material; }
};


GeometricPrimitive::GeometricPrimitive(Shape* inShape, Material* inMaterial, Transformation& inTransform) {
    shape = inShape;
    material = inMaterial;
    worldToObj = inTransform;
    objToWorld = inTransform.getInverse();
}


bool GeometricPrimitive::isHit(const Ray &ray) const {
    return shape->isHit(ray);
}


Intersection GeometricPrimitive::intersect(const Ray &ray) const {
    Intersection inter;
    inter.primitive = this;
    inter.local = shape->intersect(ray);
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
