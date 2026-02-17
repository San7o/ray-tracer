// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#include "hittable.hpp"

class material
{
public:

  virtual ~material() = default;

  virtual bool scatter([[maybe_unused]] const ray& r_in,
                       [[maybe_unused]] const hit_record& rec,
                       [[maybe_unused]] color& attenuation,
                       [[maybe_unused]] ray& scattered) const
  {
    return false;
  }
};

class lambertian : public material
{
public:

  lambertian(const color& albedo) : albedo(albedo) {}

  bool scatter([[maybe_unused]] const ray& r_int,
               const hit_record& rec,
               color& attenuation,
               ray& scattered) const override
  {
    // Pick a direction according to lambertian distribution
    //
    // This distribution scatters reflected rays in a manner that is
    // proportional to cos(phi), where phi is the angle between the
    // reflected ray and the surface normal. This means that a
    // reflected ray is most likely to scatter in a direction near
    // the surface normal, and less likely to scatter in directions
    // away from the normal.
    // We do this by generating a sphere tangent to the hit point
    // and randmly selecting a point on its surface for the
    // direction    
    auto scatter_direction = rec.normal + random_unit_vector();

    if (scatter_direction.near_zero())
      scatter_direction = rec.normal;
 
    scattered = ray(rec.p, scatter_direction);
    attenuation = albedo;
    return true;
  }

private:

  color albedo;
};

class metal : public material
{
public:

  metal(const color& albedo, double fuzz)
    : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

  bool scatter(const ray& r_in, const hit_record &rec,
               color& attenuation, ray& scattered) const override
  {
    vec3 reflected = reflect(r_in.direction(), rec.normal);
    reflected = unit_vector(reflected) + (fuzz * random_unit_vector());
    scattered = ray(rec.p, reflected);
    attenuation = albedo;
    return (dot(scattered.direction(), rec.normal) > 0);
  }

private:
  color  albedo;
  double fuzz;
};

class dielectric : public material
{
public:

  dielectric(double refraction_index)
    : refraction_index(refraction_index) {}

  bool scatter(const ray& r_in, const hit_record& rec,
               color& attenuation, ray& scattered) const override
  {
    attenuation = color(1.0, 1.0, 1.0);
    double ri = rec.front_face ? (1.0 / refraction_index) : refraction_index;

    vec3 unit_direction = unit_vector(r_in.direction());
    double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
    double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

    bool cannot_refract = ri * sin_theta > 1.0;
    vec3 direction;

    if (cannot_refract || reflectance(cos_theta, ri) > random_double())
      direction = reflect(unit_direction, rec.normal);
    else
      direction = refract(unit_direction, rec.normal, ri);

    scattered = ray(rec.p, direction);
    return true;
  }

private:
  // Refractive index in vacuum on air, or the ration of the materia's
  // refractive index over the refractive index of the enclosing media
  double refraction_index;

  static double reflectance(double cosine, double refraction_index)

  {
    // Use Schlick's approximation for reflectance
    auto r0 = (1 - refraction_index) / (1 + refraction_index);
    r0 = r0 * r0;
    return r0 + (1 - r0) * std::pow((1-cosine), 5);
  }
};
