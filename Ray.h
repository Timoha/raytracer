#ifndef RAY_H
#define RAY_H


#include <Eigen/Dense>


struct Ray {
    Eigen::Vector4f source;
    Eigen::Vector4f direction;
    float t_min, t_max;
};


// Let's wait and see if we need this
//struct LocalGeo {
//    Eigen::Vector4f position;
//    Eigen::Vector4f normal;
//};


#endif