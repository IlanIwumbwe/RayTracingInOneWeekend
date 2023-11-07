#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"
#include <vector>
#include <memory>

using std::shared_ptr;
using std::make_shared;

class hittable_list : public hittable {
    public:
        std::vector<shared_ptr<hittable>> objects;

        void add(shared_ptr<hittable> object){
            objects.push_back(object);
        }

        void clear(){
            objects.clear();
        }

        bool hit(ray& r, interval ray_t, hit_record& rec) const override {
            bool hit_anything = false;
            hit_record temp_rec;
            auto closest_so_far = ray_t.max;

            for(const auto object : objects){
                interval new_interval(ray_t.min, closest_so_far);
                if(object->hit(r, new_interval, temp_rec)){
                    hit_anything = true;
                    closest_so_far = temp_rec.t;
                    rec = temp_rec;
                }
            }

            return hit_anything;    
        }
};




#endif