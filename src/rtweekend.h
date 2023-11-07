#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <limits>
#include <memory>
#include <cstdlib>

// usings
using std::shared_ptr;
using std::sqrt;
using std::make_shared;

// constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// utility funcs
inline double degrees_to_radians(double degrees){
    return degrees * pi / 180.0;
}

inline double double_random(){
    return rand() / (RAND_MAX+1.0);
}

inline double double_random(double min, double max){
    return double_random()*(max-min) + min;
}

// common headers
#include "ray.h"
#include "vec3.h"
#include "interval.h"

#endif