#ifndef INTERVAL_H
#define INTERVAL_H

#include "rtweekend.h"

class interval{
    public:
        double min, max;
        static const interval empty, universe;

        interval(double _min, double _max) : min(_min), max(_max){}
        interval() : min(infinity), max(-infinity) {}

        bool contains(double x) const {  
            return x >= min && x <= max;
        }

        bool surrounds(double x) const {
            return x > min && x < max;
        }

        double clamp(double x) const {
            if(x < min) return min;
            if (x > max) return max;
            return x;
        }

};

static const interval empty(infinity, -infinity);
static const interval universe(-infinity, infinity);

#endif