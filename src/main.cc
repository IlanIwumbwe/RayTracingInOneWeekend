#include <iostream>
#include "rtweekend.h"

#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"

int main(){
    // World
    hittable_list world;

    auto material_ground = make_shared<lambertian>(color(0.2, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0.0, -1000, 0.0), 1000, material_ground));

    for(int i = -11; i < 11; i++){
        for(int j = -11; j < 11; j++){
            auto choose_mat = double_random();
            auto centre = point3(i + 0.9*double_random(), 0.2, j + 0.9*double_random());

            shared_ptr<material> sphere_material;

            if((centre - point3(4, 0.2, 0)).length() > 0.9){
                if(choose_mat < 0.6){
                    // choose diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(centre, 0.2, sphere_material));

                } else if (choose_mat < 0.75){
                    // choose dielectric
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(centre, 0.2, sphere_material));
                } else {
                    // choose metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = double_random(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(centre, 0.2, sphere_material));
                }
            }
        }
    }

    // 3 large spheres
    auto material1 = make_shared<metal>(color(0.7, 0.2, 0.0), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.2, 0.5, 0.2));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material3));

    // Camera
    camera cam;
    cam.ascpect_ratio = 16.0 / 9.0;
    cam.image_width = 1200;
    cam.samples_per_pixel = 20;
    cam.max_depth = 50;

    cam.vfov     = 20;
    cam.lookfrom = point3(13,2,3);
    cam.lookto   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0.2;
    cam.focus_dist = 10.0;

    cam.render(world);
}