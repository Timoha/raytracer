#ifndef LIGHT_H
#define LIGHT_H

#include <Eigen/Dense>

#include "Color.h"


class Light {
public:
    Light();
    Light(Color inColor);
    Color getColor() { return color; }
private:
    Color color;
};

Light::Light() {
    color = Color(0.0f, 0.0f, 0.0f);
}

Light::Light(Color inColor) {
    color = inColor;
}



class DLight : public Light {
public:
    DLight();
    DLight(Color inColor, Eigen::Vector4f inDir) : Light(inColor), direction(inDir) {}
    Eigen::Vector4f getDirection() { return direction; }

private:
    Eigen::Vector4f direction;
};

DLight::DLight() {
    direction = Eigen::Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
}

//Class for point lights
class PLight : public Light {
public:
    PLight();
    PLight(Color inColor, Eigen::Vector4f inSource) : Light(inColor), source(inSource) {}
    Eigen::Vector4f getSource() { return source; }
private:
    Eigen::Vector4f source;
};

PLight::PLight(){
    source = Eigen::Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
}

#endif