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

    friend ostream& operator<< (ostream &out, Material &m);
};


Material::Material() {
    Color black(0.0f, 0.0f, 0.0f);
    ambient = black;
    specular = black;
    diffuse = black;
    reflective = black;
    specularExponent = 0.0f;
}

ostream& operator<< (ostream &out, Material &m) {
    out << "Ambient " << m.ambient << ", Diffuse " << m.diffuse;
    out << ", Specular " << m.specular << ", Exponent " << m.specularExponent;
    out << ", Reflective " << m.reflective;
    return out;
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
    virtual ~Primitive() = 0;
    virtual bool isHit(const Ray& ray) const = 0;
    virtual Intersection intersect(const Ray& ray) const = 0;
    virtual Material getBRDF() const = 0;
};

inline Primitive::~Primitive() { }


class GeometricPrimitive : public Primitive
{
private:
    Transformation *objToWorld, *worldToObj;
    Shape *shape;
    Material material;
public:
    GeometricPrimitive(Shape* inShape, Material inMaterial, Transformation* inTransform);
    ~GeometricPrimitive();
    bool isHit(const Ray& ray) const;
    Intersection intersect(const Ray& ray) const;
    Material getBRDF() const { return material; }
};


GeometricPrimitive::GeometricPrimitive(Shape* inShape, Material inMaterial, Transformation* inTransform) {
    shape = inShape;
    material = inMaterial;
    worldToObj = inTransform;
    objToWorld = inTransform->getInverse();
}


GeometricPrimitive::~GeometricPrimitive() {
    delete shape;
    delete worldToObj;
    delete objToWorld;
}


bool GeometricPrimitive::isHit(const Ray &ray) const {
    return shape->isHit(ray);
}


Intersection GeometricPrimitive::intersect(const Ray &ray) const {
    Intersection inter;
    Ray objRay = *worldToObj * ray;

    LocalGeo geo = shape->intersect(objRay);
    inter.local = *objToWorld * geo;

    if (geo.isHit) {
        inter.primitive = this;
    } else {
        inter.primitive = NULL;
    }

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
