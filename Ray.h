#ifndef RAY_H
#define RAY_H


#include <Eigen/Dense>

class Ray {
public:
    Eigen::Vector4f source;
    Eigen::Vector4f direction;
    float t_min, t_max;
    Ray(const Eigen::Vector4f& inSource, const Eigen::Vector4f& inDirection, float tMinIn, float tMaxIn);
};

Ray::Ray(const Eigen::Vector4f& inSource, const Eigen::Vector4f& inDirection, float tMinIn, float tMaxIn) {
    source = inSource;
    direction = inDirection;
    t_min = tMinIn;
    t_max = tMaxIn;
}


// Let's wait and see if we need this
class LocalGeo {
public:
    Eigen::Vector4f point;
    Eigen::Vector4f normal;
    float tHit;
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

#endif
