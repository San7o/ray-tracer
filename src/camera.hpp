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
  int    max_depth     = 10;     // maximum number of ray bonces into scene

  
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
  
  void initialize()
  {
    int image_height   = image_width / aspect_ratio;
    this->image_height = (image_height < 1) ? 1 : image_height;
    this->pixel_samples_scale = 1.0 / this->samples_per_pixel;
    this->center = point3(0, 0, 0);
    
    //
    // The viewport
    // ------------
    //
    // The viewport is a poligon (2D) viewing region. This is a
    // mathematical object to transpose the space to.
    //
    auto viewport_height = 2.0;
    auto viewport_width  = viewport_height * (double(image_width) / image_height);

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
    point3 camera_center = point3(0, 0, 0);
    double focal_length  = 1.0;   // distance between the viewport and
    // the camera center
    // Calculate the vectors acress the horizontal and down the vertical
    // viewport edges
    // v = vertical, u = horizontal
    auto viewport_u = vec3(viewport_width, 0, 0);
    auto viewport_v = vec3(0, -viewport_height, 0);
    // Calcolate the horizontal and vertical delta vectors from pixel
    // to pixel
    this->pixel_delta_u = viewport_u / image_width;
    this->pixel_delta_v = viewport_v / image_height;
    // Calculate the location of the upper left pixel
    auto viewport_upper_left
      = camera_center
      - vec3(0, 0, focal_length)   // move it back by focal_length
      - viewport_u / 2             // move it left
      - viewport_v / 2;            // move it up
    // Move the upper left point in the upper left cornel of its pixel
    this->pixel00_loc
      = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
  
  }

  ray get_ray(int i, int j) const
  {
    // Construct a camera ray originating from the origin and directed
    // at randomly samples point around the pixel location i, j

    auto offset = sample_square();
    auto pixel_sample = pixel00_loc
      + ((i + offset.x()) * pixel_delta_u)
      + ((j + offset.y()) * pixel_delta_v);

    auto ray_origin = center;
    auto ray_direction = pixel_sample - ray_origin;

    return ray(ray_origin, ray_direction);
  }

  vec3 sample_square() const
  {
    // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5]
    // unit square
    return vec3(random_double() - 0.5, random_double() - 0.5, 0);
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
