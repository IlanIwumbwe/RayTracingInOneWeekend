#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"

class hit_record{
    public:
        point3 p;
        double t;
        vec3 normal;
        bool front_face;

        void set_normal(ray& r, const vec3& outward_normal){
            front_face = dot(r.direction(), outward_normal) < 0;
            normal = front_face ? outward_normal : -outward_normal;
        }

};

class hittable{
    public:
        virtual ~hittable(){};
        virtual bool hit(ray& r, interval ray_t, hit_record& rec) const = 0;
};

#endif