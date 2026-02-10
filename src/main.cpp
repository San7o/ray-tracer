// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#include <iostream>

#include "raytracer.hpp"

int main(void)
{

  //
  // World
  // -----
  //
  // Aka scene, this contains all our objects
  //
  hittable_list world;
  world.add(std::make_shared<sphere>(point3(0, 0, -1), 0.5));
  world.add(std::make_shared<sphere>(point3(0, -100.5, -1), 100));

  camera cam;
  cam.aspect_ratio   = 16.0 / 9.0;
  cam.image_width    = 400;
  
  //
  // Image
  // -----
  //
  // The image is a realization of the framebuffer in an image format
  // such as .ppng or .jpeg. We will convert the framebuffer to a PBM
  // because of its simplicity.
  //

  cam.render(world);
  
  return 0;
}
