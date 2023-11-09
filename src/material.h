#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.h"

class hit_record;  // tells the compiler that this class will be defined later, solves circular import issue

class material{
    public:
        virtual ~material() = default;
        virtual bool scatter(ray& ray_in, color& attenuation, hit_record& rec, ray& scattered_ray) const = 0;

};

class lambertian : public material{
    public:
        lambertian(const color& _albedo): albedo(_albedo) {} 

        bool scatter(ray& ray_in, color& attenuation, hit_record& rec, ray& scattered_ray) const override {
            vec3 scatter_direction = rec.normal + random_in_unit_sphere();

            if(scatter_direction.near_zero()){
                scatter_direction = rec.normal;
            }

            scattered_ray = ray(rec.p, scatter_direction);
            attenuation = albedo;
            return true;
        }   

    private:
        color albedo;
};

class metal : public material{
    public:
        metal(const color& _albedo, double f) : albedo(_albedo), fuzz(f < 1 ? f : 1) {}

        bool scatter(ray& ray_in, color& attenuation, hit_record& rec, ray& scattered_ray) const override {

            vec3 refleted = reflect(unit_vector(ray_in.direction()), rec.normal);
            scattered_ray = ray(rec.p, refleted + fuzz*random_in_unit_sphere());
            attenuation = albedo;
            return dot(scattered_ray.direction(), rec.normal) > 0;
        }   

    private:
        color albedo;
        double fuzz;

};


class dielectric : public material{
    public:
        dielectric(const double& ir) : refractive_index(ir) {}

        bool scatter(ray& ray_in, color& attenuation, hit_record& rec, ray& scattered_ray) const override {
            attenuation = color(1.0, 1.0, 1.0);
            double ir_ratio = rec.front_face ? 1.0 / refractive_index : refractive_index;
            
            auto unit_direction = unit_vector(ray_in.direction());

            auto cos_theta = dot(unit_direction, rec.normal);
            auto sin_theta = sqrt(1 - cos_theta*cos_theta);
            auto param = ir_ratio * sin_theta;

            vec3 resulting_ray;

            if(param > 1 || reflectance(cos_theta, ir_ratio) > double_random()){
                // cannot refract, so reflect
                resulting_ray = reflect(unit_direction, rec.normal);
            } else {
                // can refract, so refract
                resulting_ray = refract(unit_direction, ir_ratio, rec.normal);
            }

            scattered_ray = ray(rec.p, resulting_ray);
            return true;
        }

    private:
        double refractive_index;

        static double reflectance(double cosine, double ref_idx) {
            // Use Schlick's approximation for reflectance.
            auto r0 = (1-ref_idx) / (1+ref_idx);
            r0 = r0*r0;
            return r0 + (1-r0)*pow((1 - cosine),5);
        }


};



#endif