#include <iostream>
#include "vec3.h"
#include "ray.h"
#include "color.h"

bool hit_sphere(const point3& centre, double radius, ray& r){
    vec3 oc = r.origin() - centre;
    auto a = dot(r.direction(), r.direction());
    auto b = 2.0 * dot(r.direction(), oc);
    auto c = dot(oc, oc) - radius*radius;
    auto discriminant = b*b - 4*a*c;

    return (discriminant >= 0);
}

color ray_color(ray& r){
    if(hit_sphere(point3(0, 0, -1), 0.5, r))
        return color(1.0, 0.0, 0.0);

    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5*(unit_direction.y() + 1.0);
    return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
}

int main(){
    // image
    auto ascpect_ratio = 16.0 / 9.0;
    int image_width = 400;

    // setting image height based on aspect ratio, and clamping it below 1
    int image_height = static_cast<int> (image_width / ascpect_ratio);  
    image_height = image_height < 1 ? 1 : image_height;

    // camera
    auto focal_length = 1.0;
    auto viewport_height = 2.0;
    auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);
    auto camera_centre = point3(0, 0, 0);

    // vectors across the horizontal and vertical viewport edges
    auto viewport_u = vec3(viewport_width, 0 ,0);
    auto viewport_v = vec3(0, -viewport_height, 0);

    // calculate pixel to pixel delta vectors
    auto pixel_delta_u = viewport_u / image_width;
    auto pixel_delta_v = viewport_v / image_height;

    // find location of top left corner of image
    auto viewport_top_left = camera_centre - vec3(0, 0, focal_length) - viewport_v/2 - viewport_u/2;
    auto pixel00_loc = viewport_top_left + 0.5*(pixel_delta_u + pixel_delta_v);

    // render

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for(int j = 0; j < image_height; ++j){
        std::clog << "\rScanlines remaining: " << image_height-j << std::flush;
        for(int i = 0; i < image_width; ++i){
            auto pixel_centre = pixel00_loc + (i*pixel_delta_u) + (j*pixel_delta_v);
            auto ray_direction = pixel_centre - camera_centre; // use 2 points to find direction vector

            // define ray from camera to pixel on viewport
            ray r(camera_centre, ray_direction);
            color pixel_color = ray_color(r);

            write_color(std::cout, pixel_color);
        }
    }
    std::clog << "\rDone.             \n";
}