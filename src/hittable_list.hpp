// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#pragma once

#include "hittable.hpp"
#include "interval.hpp"

#include <memory>
#include <vector>

class hittable_list : public hittable
{
public:
  std::vector<std::shared_ptr<hittable>> objects;

  hittable_list() {}
  hittable_list(std::shared_ptr<hittable> object)
  { this->add(object); }

  void clear() { this->objects.clear(); }

  void add(std::shared_ptr<hittable> object)
  { this->objects.push_back(object); }

  bool hit(const ray& r, interval ray_t, hit_record& rec) const override
  {
    hit_record temp_rec;
    bool hit_anything = false;
    auto closest_so_far = ray_t.max;

    for (const auto& object : objects)
    {
      if (object->hit(r, interval(ray_t.min, closest_so_far), temp_rec))
      {
        hit_anything = true;
        closest_so_far = temp_rec.t;
        rec = temp_rec;
      }
    }

    return hit_anything;
  }
};
