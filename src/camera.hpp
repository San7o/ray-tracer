// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#pragma once

#include "hittable.hpp"
#include "material.hpp"

class camera {
public:

  double aspect_ratio  = 1.0;
  int    image_width   = 100;
  int    samples_per_pixel = 10;  // count of random samples of each pixel
  int    max_depth     = 10;      // maximum number of ray bonces into scene

  double vfov = 90; // vertical view angle (field of view)
  point3 lookfrom = point3(0, 0, 0); // Point camera is looking from
  point3 lookat = point3(0, 0, -1);  // Point camera is looking at
  vec3 vup = vec3(0, 1, 0);          // Camera-relative up direction

  double defocus_angle = 0; // Variation angle of rays through each pixel
  double focus_dist    = 10; // Distance from camera lookfrom point to plane of perfect focus
  
  void render(const hittable& world)
  {
    initialize();

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = 0; j < image_height; ++j)
    {
      std::clog << "\rScanlines remaining: " << (image_height - j) << "\n" << std::flush; 
      for (int i = 0; i < image_width; ++i)
      {
        color pixel_color(0, 0, 0);
        for (int sample = 0; sample < samples_per_pixel; sample++)
        {
          ray r = get_ray(i, j);
          pixel_color += ray_color(r, max_depth, world);
        }
        
        write_color(std::cout, pixel_samples_scale * pixel_color);
      }
    }

    std::clog << "\rDone.             \n";
  
  }

private:

  int    image_height;
  point3 center;
  point3 pixel00_loc;   // location of pixel 00
  vec3   pixel_delta_u; // offset to pixel to the right
  vec3   pixel_delta_v; // offset to pixel below
  double pixel_samples_scale;  // color scale factor for a sum of pixel samples
  vec3 u, v, w;        // Camera frame basis vectors
  vec3 defocus_disk_u; // Defocus disk horizontal radius
  vec3 defocus_disk_v; // Defocus disk vertical radius
  
  void initialize()
  {
    int image_height   = image_width / aspect_ratio;
    this->image_height = (image_height < 1) ? 1 : image_height;
    this->pixel_samples_scale = 1.0 / this->samples_per_pixel;
    this->center = point3(0, 0, 0);

    center = lookfrom;
    
    //
    // The viewport
    // ------------
    //
    // The viewport is a poligon (2D) viewing region. This is a
    // mathematical object to transpose the space to.
    //
    auto theta = degrees_to_radians(vfov);
    auto h = std::tan(theta / 2);
    auto viewport_height = 2 * h * focus_dist;
    auto viewport_width  = viewport_height * (double(image_width) / image_height);

    // Calculate the u, v, w unit basis vectors for the camera
    // coordinate frame
    w = unit_vector(lookfrom - lookat);
    u = unit_vector(cross(vup, w));
    v = cross(w, u);
    
    //
    // Camera
    // ------
    //
    // The camera is a point in space where the rays are coming from.
    // It is often referred to as the "eye"
    //
    // The x coordinate is positive to the right
    // The y coordinate is positive up
    // The z coordinate is negative from the camera to the viewport
    //

    // Calculate the vectors acress the horizontal and down the vertical
    // viewport edges
    // v = vertical, u = horizontal
    auto viewport_u = viewport_width * u;
    auto viewport_v = viewport_height * -v;
    // Calcolate the horizontal and vertical delta vectors from pixel
    // to pixel
    this->pixel_delta_u = viewport_u / image_width;
    this->pixel_delta_v = viewport_v / image_height;
    // Calculate the location of the upper left pixel
    auto viewport_upper_left
      = center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
    // Move the upper left point in the upper left cornel of its pixel
    this->pixel00_loc
      = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
    defocus_disk_u = u * defocus_radius;
    defocus_disk_v = v * defocus_radius;
  }

  ray get_ray(int i, int j) const
  {
    // Construct a camera ray originating from the defocus disk and
    // directed at randomly samples point around the pixel location i,
    // j

    auto offset = sample_square();
    auto pixel_sample = pixel00_loc
      + ((i + offset.x()) * pixel_delta_u)
      + ((j + offset.y()) * pixel_delta_v);

    auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
    auto ray_direction = pixel_sample - ray_origin;

    return ray(ray_origin, ray_direction);
  }

  vec3 sample_square() const
  {
    // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5]
    // unit square
    return vec3(random_double() - 0.5, random_double() - 0.5, 0);
  }

  point3 defocus_disk_sample() const
  {
    // Returns a random point in the camera defocus disk
    auto p = random_in_unit_disk();
    return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
  }
  
  color ray_color(const ray& r, int depth, const hittable& world) const
  {
    if (depth <= 0) // limit recursion
      return color(0, 0, 0);
    
    hit_record rec;
    if (world.hit(r, interval(0.001, infinity), rec))
      // we use 0.001 to account for floating point inaccuracies
    {
      ray scattered;
      color attenuation;
      if (rec.mat->scatter(r, rec, attenuation, scattered))
        return attenuation * ray_color(scattered, depth-1, world);
      return color(0, 0, 0);
    }

    // Blend white and blue depending on the height of y (after
    // normalization)
    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5 * (unit_direction.y() + 1.0);
    // Here we are basically doing linear interpolation
    return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
  }
};
