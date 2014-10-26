#ifndef LIGHT_H
#define LIGHT_H

#include <Eigen/Dense>

#include "Color.h"


class Light {
public:
    Light();
    Light(const Color& inColor);
    Color getColor() { return color; }
    virtual Eigen::Vector4f getLightVector(const Eigen::Vector4f& surfacePoint) = 0;
protected:
    Color color;
};

Light::Light() {
    color = Color(0.0f, 0.0f, 0.0f);
}

Light::Light(const Color& inColor) {
    color = inColor;
}



class DLight : public Light {
public:
    DLight();
    DLight(const Color& inColor, const Eigen::Vector4f& inDir) : Light(inColor), direction(inDir) {}
    Eigen::Vector4f getDirection() { return direction; }
    Eigen::Vector4f getLightVector(const Eigen::Vector4f& surfacePoint);
    friend ostream& operator<< (ostream &out, DLight &m);
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:
    Eigen::Vector4f direction;
};

DLight::DLight() {
    direction = Eigen::Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
}

Eigen::Vector4f DLight::getLightVector(const Eigen::Vector4f& surfacePoint) {
    return direction;
}

ostream& operator<< (ostream &out, DLight &l) {
    out << "DLight " << l.color << " " << l.direction;
    return out;
}

//Class for point lights
class PLight : public Light {
public:
    PLight();
    PLight(const Color& inColor, const Eigen::Vector4f& inSource, float inFalloff = 0.0f) : Light(inColor), source(inSource), falloff(inFalloff) {}
    Eigen::Vector4f getSource() { return source; }
    Eigen::Vector4f getLightVector(const Eigen::Vector4f& surfacePoint);
    friend ostream& operator<< (ostream &out, PLight &l);
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:
    Eigen::Vector4f source;
    float falloff;
};

PLight::PLight(){
    source = Eigen::Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
    falloff = 0.0f;
}

Eigen::Vector4f PLight::getLightVector(const Eigen::Vector4f& surfacePoint) {
    return (source - surfacePoint).normalized();
}

ostream& operator<< (ostream &out, PLight &l) {
    out << "PLight " << l.color << " " << l.source;
    return out;
}


//Class for ambient lights
class ALight : public Light {
public:
    ALight();
    ALight(const Color& inColor) : Light(inColor) {}
    Eigen::Vector4f getLightVector(const Eigen::Vector4f& surfacePoint);
    friend ostream& operator<< (ostream &out, ALight &l);
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

Eigen::Vector4f ALight::getLightVector(const Eigen::Vector4f& surfacePoint) {
    return Eigen::Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
}

ostream& operator<< (ostream &out, ALight &l) {
    out << "ALight " << l.color;
    return out;
}

#endif