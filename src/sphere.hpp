// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#pragma once

#include "hittable.hpp"
#include "vec3.hpp"

class sphere : public hittable {
public:
  sphere(const point3& center, double radius, std::shared_ptr<material> mat)
    :center(center), radius(std::fmax(0, radius)), mat(mat) {}

  bool hit(const ray& r, interval ray_t, hit_record& rec) const override
  {
    // Calculating if a ray hit the sphere is fairly simple. It is
    // the problem of calculating the intersection between a line
    // and a sphere, if this is found then the sphere is hit.
    // To do this, we solve the system of equation of the sphere
    // equation `||x-c||^2 = r^2` and the line equation `x = o + du`
    // solving for `d` distance from the origin of the line.
    // This implementation does some semplifications to speedup
    // calculation.
    
    vec3 oc = center - r.origin();
    auto a = r.direction().length_squared();
    auto h = dot(r.direction(), oc);
    auto c = oc.length_squared() - radius * radius;
    auto discriminant = h*h - a*c;

    if (discriminant < 0)
      return false;

    auto sqrtd = std::sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (h - sqrtd) / a;
    if (!ray_t.surrounds(root))
    {
      root = (h + sqrtd) / a;
      if (!ray_t.surrounds(root))
        return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    rec.mat = mat;
    
    return true;
  }

private:
  point3 center;
  double radius;
  std::shared_ptr<material> mat;
};
