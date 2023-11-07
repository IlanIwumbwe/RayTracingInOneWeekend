#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"
#include "rtweekend.h"

class camera{
    public:
        double ascpect_ratio = 1.0;
        int image_width = 100;
        int samples_per_pixel = 10;

        void render(const hittable& world){
            initialise();

            std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

            for(int j = 0; j < image_height; ++j){
                std::clog << "\rScanlines remaining: " << image_height-j << std::flush;
                for(int i = 0; i < image_width; ++i){
                    color pixel_color(0,0,0);

                    for(int k = 0; k < samples_per_pixel; ++k){
                        ray r = get_ray(i, j); 
                        pixel_color += ray_color(r, world);
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

        color ray_color(ray& r, const hittable& world){
            hit_record rec;

            if(world.hit(r, interval(0, infinity), rec)){
                vec3 direction = random_in_hemisphere(rec.normal);
                ray r = ray(rec.p, direction);
                return 0.5 * ray_color(r, world);

                //return 0.5*(rec.normal + color(1,1,1));
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
            auto focal_length = 1.0;
            auto viewport_height = 2.0;
            camera_centre = point3(0, 0, 0);
            auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);

            // vectors across the horizontal and vertical viewport edges
            auto viewport_u = vec3(viewport_width, 0 ,0);
            auto viewport_v = vec3(0, -viewport_height, 0);

            // calculate pixel to pixel delta vectors
            pixel_delta_u = viewport_u / image_width;
            pixel_delta_v = viewport_v / image_height;

            // find location of top left corner of image
            auto viewport_top_left = camera_centre - vec3(0, 0, focal_length) - viewport_v/2 - viewport_u/2;
            pixel00_loc = viewport_top_left + 0.5*(pixel_delta_u + pixel_delta_v);

        }

        ray get_ray(int i, int j){
            auto pixel_center = pixel00_loc + (i*pixel_delta_u) + (j*pixel_delta_v);
            auto pixel_sample = pixel_center + pixel_surrounding_sample();

            auto ray_direction = pixel_sample - camera_centre;

            return ray(camera_centre, ray_direction);
        }

        vec3 pixel_surrounding_sample() const {
            auto px = -0.5 + double_random();
            auto py = -0.5 + double_random();

            return (px*pixel_delta_u) + (py*pixel_delta_v);
        }
};


#endif 