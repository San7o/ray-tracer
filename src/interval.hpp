// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#pragma once

#include "utils.hpp"

class interval
{
public:
  double min, max;

  interval() : min(+infinity), max(-infinity) {}

  interval(double min, double max) : min(min), max(max) {}

  double size() const
  {
    return this->max - this->min;
  }

  double contains(double x) const
  {
    return this->min <= x && x <= this->max;
  }

  bool surrounds(double x) const
  {
    return this->min < x && x < this->max;
  }

  static const interval empty, universe;
};

const interval interval::empty = interval(+infinity, -infinity);
const interval interval::universe = interval(-infinity, +infinity);
