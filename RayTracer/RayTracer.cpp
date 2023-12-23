#include <iostream>
#include "color.h"
#include "ray.h"
#include "vec3.h"

double hit_sphere(const point3& center, double radius, const ray& r)
{
    vec3 oc = r.origin() - center;
    double a = dot(r.direction(), r.direction());
    double b = 2.0 * dot(oc, r.direction());
    double c = dot(oc, oc) - radius * radius;
    double discriminant = b * b - 4 * a * c;
    
    return (discriminant < 0) ? -1.0 : (-b - sqrt(discriminant)) / (2.0 * a);

}

color ray_color(const ray& r)
{
    // Test if it hits a sphere at pos -1 y and a radius of 0.5
    double t = hit_sphere(point3(0, 0, -1), 0.5, r);
    if (t > 0.0)
    {
        // Get normal and then shift the hue 
        vec3 normal = unit_vector(r.at(t) - vec3(0, 0, -1));
        return 0.5 * color(normal.x() + 1, normal.y() + 1, normal.z() + 1);
    }

    vec3 unit_direction = unit_vector(r.direction());
    double a = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
}

int main() {

    // Image

    double aspect_ratio = 16.0 / 9.0;
    int image_width = 256;
    
    // Calc height based from ratio and meets min of 1 
    int image_height = static_cast<int>(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    

    #pragma region Camera

    double focal_length = 1.0f;
    double viewport_height = 2.0;
    // Use the ratio between image height and image width to find width but not
    // the aspect ratio variable since that's the ideal ratio and possibly not 
    // the real ratio  
    double viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);
    vec3 camera_center = point3(0, 0, 0);

    // Vectors for horizontal and vertical viewport edges 
    // Remember that we draw from top of the screen to bottom! This is why the
    // vertical vector is a negative value rather than positive 
    vec3 viewport_u = vec3(viewport_width, 0, 0);
    vec3 viewport_v = vec3(0, -viewport_height, 0);

    // Calc the chance between pixels
    vec3 pixel_delta_u = viewport_u / image_width;
    vec3 pixel_delta_v = viewport_v / image_height;

    // Calc top left pixel 
    vec3 viewport_upper_left = camera_center
                             - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
    // We start in the CENTER of the first pixel not its corner 
    vec3 pixel100_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    #pragma endregion


    // Render
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = 0; j < image_height; ++j) 
    {
        std::clog << "\rScaleLines remaining: " << (image_height - j) << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) 
        {
            vec3 pixel_center = pixel100_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            vec3 ray_direction = pixel_center - camera_center;

            ray r(camera_center, ray_direction);

            color pixel_color = ray_color(r);
            write_color(std::cout, pixel_color);
        }
    }

    std::clog << "\rDone.                 \n";
}