// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#pragma once

#include "ray.hpp"
#include "interval.hpp"

#include <memory>

class material;

// Information about a hit
class hit_record
{
public:
  
  point3 p;
  vec3   normal;
  double t;
  bool   front_face;
  std::shared_ptr<material> mat;

  // Sets the hit record normal vector.
  // NOTE: the parameter `outward_normal` is assumed to have unit length.
  void set_face_normal(const ray& r, const vec3& outward_normal)
  {
    this->front_face = dot(r.direction(), outward_normal) < 0;
    this->normal     = front_face ? outward_normal : -outward_normal;
  }
};

// An object that can be hit
class hittable {
public:
  virtual ~hittable() = default;

  // Fills [rec] with hit information
  virtual bool hit(const ray& t, interval ray_t, hit_record& rec) const = 0;
};
