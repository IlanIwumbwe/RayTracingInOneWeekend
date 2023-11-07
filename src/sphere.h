#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable{
    public:
        sphere(point3 _centre, double _radius) : centre(_centre), radius(_radius) {}
        
        bool hit(ray& r, interval ray_t, hit_record& rec) const override {
            vec3 oc = r.origin() - centre;
            auto a = r.direction().length_squared();
            auto half_b = dot(r.direction(), oc);
            auto c = oc.length_squared() - radius*radius;
            auto discriminant = half_b*half_b - a*c;

            if(discriminant < 0) return false;

            auto sqrtd = sqrt(discriminant);

            // find smallest root that is within the given range of t
            auto root = (-half_b - sqrtd) / a;

            if(!ray_t.surrounds(root)){
                root = (-half_b + sqrtd) / a;
                if(!ray_t.surrounds(root)){
                    return false;
                }
            }

            // record information
            rec.t = root;
            rec.p = r.at(rec.t);
            vec3 outward_normal = (rec.p - centre) / radius;
            rec.set_normal(r, outward_normal);

            return true;    
        }

    private:
        point3 centre;
        double radius;
};
#endif