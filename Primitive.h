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

inline Primitive::~Primitive() {

}


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
    worldToObj = inTransform->getInverse();
    objToWorld = inTransform;
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

    LocalGeo geo = *objToWorld * shape->intersect(objRay);
    inter.local = geo;

    if (geo.isHit) {
        inter.primitive = this;
    } else {
        inter.primitive = NULL;
    }

    return inter;
}


class AggregatePrimitive : public Primitive
{

private:
    vector<Primitive*> primitives;
public:
    AggregatePrimitive(vector<Primitive*> &list);
    bool isHit(const Ray& ray) const;
    Intersection intersect(const Ray& ray) const;
    Material getBRDF() const { exit(1); }
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

AggregatePrimitive::AggregatePrimitive(vector<Primitive*> &list) {
    primitives = list;
}


bool AggregatePrimitive::isHit(const Ray &ray) const {
    for (int i = 0; i < primitives.size(); i++) {
        if (primitives[i]->isHit(ray)) {
            return true;
        }
    }
    return false;
}


Intersection AggregatePrimitive::intersect(const Ray &ray) const {

    float closest_t = numeric_limits<float>::infinity();
    Intersection finalInter, closestInter, intersect;
    bool isPrimitiveHit = false;

    for (int i = 0; i < primitives.size(); i++) {
        intersect = primitives[i]->intersect(ray);
        if (intersect.local.isHit){
            isPrimitiveHit = true;
            if (intersect.local.tHit < closest_t && intersect.local.tHit > 0.1f){
                closest_t = intersect.local.tHit;
                closestInter = intersect;
            }
        }
    }

    if (isPrimitiveHit) {
        finalInter = closestInter;
    } else {
        finalInter.primitive = NULL;
    }

    return finalInter;
}

#endif
