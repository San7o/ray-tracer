// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#pragma once

#include "vec3.hpp"

class ray
{
public:
  
  ray() {}
  ray(const point3& origin, const vec3& direction)
    : orig(origin), dir(direction) {}

  const point3& origin() const  { return this->orig; }
  const vec3& direction() const { return this->dir; }

  point3 at(double t) const
  {
    return orig + t * dir;
  }

private:

  point3 orig;
  vec3 dir;
  
};
