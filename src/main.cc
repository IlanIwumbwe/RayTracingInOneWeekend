#include <iostream>
#include "rtweekend.h"

#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"

int main(){
    // World
    hittable_list world;

    auto material_ground = make_shared<lambertian>(color(0.4, 0.4, 0.0));
    auto material_middle = make_shared<lambertian>(color(0.7, 0.2, 0.3));
    auto material_left = make_shared<metal>(color(0.8, 0.5, 0.7), 0.3);
    //auto material_right = make_shared<metal>(color(0.8, 0.8, 0.8), 1.0);
    auto material_right = make_shared<dielectric>(1.5);

    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5, material_middle));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100, material_ground));
    world.add(make_shared<sphere>(point3(-1, 0, -1), 0.5, material_left));
    world.add(make_shared<sphere>(point3(1, 0, -1), 0.5, material_right));

    // Camera
    camera cam;
    cam.ascpect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    cam.render(world);

}