// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#pragma once

#include "vec3.hpp"
#include "interval.hpp"

#include <iostream>

// Colors should have values between 0 and 1
using color = vec3;

inline double linear_to_gamma(double linear_component)
{
  if (linear_component > 0)
    return std::sqrt(linear_component);

  return 0;
}

void write_color(std::ostream& out, const color& pixel_color)
{
  auto r = pixel_color.x();
  auto g = pixel_color.y();
  auto b = pixel_color.z();

  // Go to gamma domain
  r = linear_to_gamma(r);
  g = linear_to_gamma(g);
  b = linear_to_gamma(b);
  
  static const interval intensity(0.000, 0.999);
  int rbyte = int(255.999 * intensity.clamp(r));
  int gbyte = int(255.999 * intensity.clamp(g));   
  int bbyte = int(255.999 * intensity.clamp(b));

  out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}
