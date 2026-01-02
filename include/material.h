#ifndef MATERIAL_H
#define MATERIAL_H

#include "vec3.h"
#include "ray.h"
#include "hittable.h"
#include <random>
#include <omp.h>

// Fast, thread-local XorShift32 RNG
// TODO: add further documentation on how this works
inline double random_double() {
	static thread_local uint32_t state = 123456789 + omp_get_thread_num();
	state ^= state << 13;
	state ^= state >> 17;
	state ^= state << 5;
	// Map to [0, 1) using bit magic
	return state / 4294967296.0;
}

inline double random_double(double min, double max) {
	return min + (max - min) * random_double();
}

inline Vec3 random_in_unit_sphere() {
	while (true) {
		Vec3 p(random_double(-1, 1), random_double(-1, 1), random_double(-1, 1));
		if (p.length_squared() < 1) return p;
	}
}

inline Vec3 random_unit_vector() {
	return random_in_unit_sphere().unit_vector();
}

class Material {
public:
	virtual ~Material() = default;
	virtual Color emitted() const {
		return Color(0, 0, 0);
	}
	virtual bool scatter(const Ray& ray_in, const HitRecord& record, Color& attenuation, Ray& scattered) const = 0;
};

class Lambertian : public Material {
public:
	Color albedo;
	Lambertian(const Color& albedo) : albedo(albedo) {}

	bool scatter(const Ray& ray_in, const HitRecord& record, Color& attenuation, Ray& scattered) const override {
		// the Lambertian is the mathematical model for ideal matte surfaces ; paper and unfinished wood 
		// to simulate light scattering in random directions, we take the surface normal and add a random unit vector to it
		// this creates a distributin of rays that are more likely to point near the normal but can scatter in many direction
		Vec3 scatter_direction = record.normal + random_unit_vector();

		if (scatter_direction.near_zero()) {
			scatter_direction = record.normal;
		}

		scattered = Ray(record.point, scatter_direction);
		attenuation = albedo;
		return true;
	}
};

/**
 * TODO: Add documentation on how metallic materials work
 * Explain the fuzziness parameter and its effect on reflections
 */

class Metal : public Material {
public:
	Color albedo;
	double fuzziness;
	Metal(const Color& albedo, double fuzziness) : albedo(albedo), fuzziness(fuzziness < 1 ? fuzziness : 1) {}

	bool scatter(const Ray& ray_in, const HitRecord& record, Color& attenuation, Ray& scattered) const override {
		Vec3 reflected = reflect(ray_in.direction.unit_vector(), record.normal);
		if (fuzziness > 0) {
			reflected = reflected + fuzziness * random_unit_vector();
		}
		scattered = Ray(record.point, reflected);
		attenuation = albedo;
		return (scattered.direction.dot(record.normal) > 0);
	}
};

/**
 * TODO: Add documentation on how dielectric materials work
 * Explain Snell's law, total internal reflection, and Schlick's approximation
 */

class Dielectric : public Material {
public:
	double ir;
	
	Dielectric(double refractive_index) : ir(refractive_index) {}

	bool scatter(const Ray& ray_in, const HitRecord& record, Color& attenuation, Ray& scattered) const override {
		attenuation = Color(1.0, 1.0, 1.0);
		double refraction_ratio = record.front_face ? (1.0 / ir) : ir;

		Vec3 unit_direction = ray_in.direction.unit_vector();
		double cos_theta = std::fmin(-unit_direction.dot(record.normal), 1.0);
		double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

		bool cannot_refract = refraction_ratio * sin_theta > 1.0;
		Vec3 direction;

		if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double()) {
			direction = reflect(unit_direction, record.normal);
		} else {
			direction = refract(unit_direction, record.normal, refraction_ratio);
		}

		scattered = Ray(record.point, direction);
		return true;
	}
private:
	static double reflectance(double cosine, double ref_idx) {
		auto r0 = (1 - ref_idx) / (1 + ref_idx);
		r0 = r0 * r0;
		return r0 + (1 - r0) * std::pow((1 - cosine), 5);
	}
};

/**
 * TODO: Add documentation on how diffuse light materials work
 * Explain that they emit light uniformly and do not scatter incoming rays
 * Also mention how I had messed up and it was just black (BUG REPORT)
 */

class DiffuseLight : public Material {
public:
	Color emit_color;
	double brightness;
	DiffuseLight(const Color& color, double brightness) : emit_color(color), brightness(brightness) {}

	bool scatter(const Ray& ray_in, const HitRecord& record, Color& attenuation, Ray& scattered) const override {
		return false;
	}

	Color emitted() const override {
		return brightness * emit_color;
	}
};

#endif