#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"
#include "rtweekend.h"
#include "material.h"

class camera{
    public:
        double ascpect_ratio = 1.0;
        int image_width = 100;
        int samples_per_pixel = 10;
        int max_depth = 10;
        double vfov = 90;
        point3 lookfrom = point3(0,0,-1); // point that camera is looking from
        point3 lookto = point3(0,0,0);   // point that the camera is looking to
        vec3 vup = vec3(0,1,0);   // difinition of vup

        double defocus_angle = 0;  
        double focus_dist = 10.0;  // distance to focus plane

        void render(const hittable& world){
            initialise();

            std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

            for(int j = 0; j < image_height; ++j){
                std::clog << "\rScanlines remaining: " << image_height-j << std::flush;
                for(int i = 0; i < image_width; ++i){
                    color pixel_color(0,0,0);

                    for(int k = 0; k < samples_per_pixel; ++k){
                        ray r = get_ray(i, j); 
                        pixel_color += ray_color(r,max_depth, world);
                    }

                    write_color(std::cout, pixel_color, samples_per_pixel);
                }
            }

            std::clog << "\rDone.                   \n";
        }

    private:
        int image_height;
        point3 camera_centre;
        vec3 pixel00_loc;
        vec3 pixel_delta_u;
        vec3 pixel_delta_v;
        vec3 u, v, w;  // orthonormal vectors that define camera orientation

        // defocus disk basis vectors
        vec3 defocus_disk_u;
        vec3 defocus_disk_v;
      
        color ray_color(ray& r, int max_depth, const hittable& world){
            if(max_depth <= 0){
                return color(0,0,0);
            }

            hit_record rec;

            if(world.hit(r, interval(0.001, infinity), rec)){
                color attenuation;
                ray scattered;

                if(rec.mat->scatter(r, attenuation, rec, scattered)){
                    return attenuation * ray_color(scattered, max_depth-1, world);
                }

                return color(0,0,0);
            }

            vec3 unit_direction = unit_vector(r.direction());
            auto a = 0.5*(unit_direction.y() + 1.0);
            return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
        }  

        void initialise(){
            // setting image height based on aspect ratio, and clamping it below 1
            image_height = static_cast<int> (image_width / ascpect_ratio);  
            image_height = image_height < 1 ? 1 : image_height;

            // camera
            camera_centre = lookfrom;
            // auto focal_length = (lookfrom - lookto).length();

            w = unit_vector(lookfrom - lookto); // points in opposite direction to direction in which we are looking 
            u = unit_vector(cross(vup, w)); // points up relative to camera
            v = cross(w, u);  // points to right of camera

            auto theta = degrees_to_radians(vfov);
            auto viewport_height = 2.0 * tan(theta / 2.0) * focus_dist;
            auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);

            // vectors across the horizontal and vertical viewport edges
            auto viewport_u = viewport_width * u;
            auto viewport_v = viewport_height * -v;

            // calculate pixel to pixel delta vectors
            pixel_delta_u = viewport_u / image_width;
            pixel_delta_v = viewport_v / image_height;

            // find location of top left corner of image
            auto viewport_top_left = camera_centre - (focus_dist*w) - viewport_v/2 - viewport_u/2;
            pixel00_loc = viewport_top_left + 0.5*(pixel_delta_u + pixel_delta_v);

            // find defocus disk basis vectors
            auto disk_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
            defocus_disk_u = disk_radius * u;
            defocus_disk_v = disk_radius * v;

        }

        ray get_ray(int i, int j){
            auto pixel_center = pixel00_loc + (i*pixel_delta_u) + (j*pixel_delta_v);
            auto pixel_sample = pixel_center + pixel_surrounding_sample();

            auto ray_origin = (defocus_angle <= 0) ? camera_centre : defocus_disk_sample();
            auto ray_direction = pixel_sample - ray_origin;

            return ray(ray_origin, ray_direction);
        }

        point3 defocus_disk_sample(){
            // return random point in defocus disk sample
            auto p = random_in_unit_disk();

            return camera_centre + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
        }

        vec3 pixel_surrounding_sample() const {
            auto px = -0.5 + double_random();
            auto py = -0.5 + double_random();

            return (px*pixel_delta_u) + (py*pixel_delta_v);
        }
};


#endif 