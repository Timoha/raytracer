#ifndef RAY_H
#define RAY_H

#include <limits>
#include <Eigen/Dense>

using namespace std;

class Ray {
public:
    Eigen::Vector4f source;
    Eigen::Vector4f direction;
    float t_min, t_max;
    Ray();
    Ray(const Eigen::Vector4f& inSource, const Eigen::Vector4f& inDirection, float tMinIn, float tMaxIn);
    Ray(const Ray &rhs);
    Ray& operator=(const Ray &rhs);
    friend ostream& operator<< (ostream &out, Ray &r);
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

Ray::Ray() {
    source = Eigen::Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
    direction = Eigen::Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
    t_min = -numeric_limits<float>::infinity();
    t_max = numeric_limits<float>::infinity();
}

Ray::Ray(const Eigen::Vector4f& inSource, const Eigen::Vector4f& inDirection, float tMinIn, float tMaxIn) {
    source = inSource;
    direction = inDirection;
    t_min = tMinIn;
    t_max = tMaxIn;
}

Ray::Ray(const Ray &rhs) {
    source = rhs.source;
    direction = rhs.direction;
    t_min = rhs.t_min;
    t_max = rhs.t_max;
}

Ray& Ray::operator=(const Ray &rhs) {
    if (this == &rhs) {
        return *this;
    }

    source = rhs.source;
    direction = rhs.direction;
    t_min = rhs.t_min;
    t_max = rhs.t_max;
    return *this;
}

ostream& operator<< (ostream &out, Ray &r) {
    out << "Source " << r.source << ", Direction " << r.direction;
    return out;

}


// Let's wait and see if we need this
class LocalGeo {
public:
    Eigen::Vector4f point;
    Eigen::Vector4f normal;
    float tHit;
    bool isHit;
    LocalGeo();
    LocalGeo(const LocalGeo &rhs);
    LocalGeo& operator=(const LocalGeo &rhs);
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};


LocalGeo::LocalGeo() {
    point = Eigen::Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
    normal = Eigen::Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
    tHit = numeric_limits<float>::infinity();
    isHit = false;
}


LocalGeo::LocalGeo(const LocalGeo &rhs) {
    point = rhs.point;
    normal = rhs.normal;
    tHit = rhs.tHit;
    isHit = rhs.isHit;
}


LocalGeo& LocalGeo::operator=(const LocalGeo &rhs) {
    if (this == &rhs) {
        return *this;
    }

    point = rhs.point;
    normal = rhs.normal;
    tHit = rhs.tHit;
    isHit = rhs.isHit;

    return *this;
}

#endif
