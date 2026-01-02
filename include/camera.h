#ifndef CAMERA_H
#define CAMERA_H

#include "ray.h"
#include "vec3.h"
#include "hittable.h"
#include "material.h"
#include <iostream>

class Camera {
public:
	Camera(Vec3 lookfrom, Vec3 lookat, Vec3 vup, double vfov, double aspect_ratio) {
		auto theta = vfov * M_PI / 180.0;
		auto h = tan(theta / 2);
		auto viewport_height = 2.0 * h;
		auto viewport_width = aspect_ratio * viewport_height;

		w = (lookfrom - lookat).unit_vector();
		u = vup.cross(w).unit_vector();
		v = w.cross(u);

		origin = lookfrom;
		horizontal = viewport_width * u;
		vertical = viewport_height * v;
		
		lower_left_corner = origin - horizontal/2 - vertical/2 - w;
	}

	Ray get_ray(double s, double t) const {
		return Ray(origin, lower_left_corner + s*horizontal + t*vertical - origin);
	}

private:
	Vec3 origin;
	Vec3 lower_left_corner;
	Vec3 horizontal;
	Vec3 vertical;
	Vec3 u, v, w;
};

/**
 * Our previous simple camera implementation was fixed at a certain position and orientation.
 * This new Camera class allows us to specify the camera's position (lookfrom), the point
 * it is looking at (lookat), and the "up" direction (vup). We also specify the vertical field of view (vfov) and the aspect ratio.
 * With these parameters, we can compute the camera's coordinate system (u, v, w) and set up the viewport accordingly.
 * This makes the camera much more flexible, allowing us to easily change its position and orientation in the scene.
class Camera {
public:
	Vec3 origin;
	Vec3 lower_left_corner;
	Vec3 horizontal;
	Vec3 vertical;
	
	Camera(double aspect_ratio, double viewport_height) {
		double viewport_width = aspect_ratio * viewport_height;
		double focal_length = 1.0;
		
		origin = Vec3(0, 0, 0);
		horizontal = Vec3(viewport_width, 0, 0);
		vertical = Vec3(0, viewport_height, 0);
		
		lower_left_corner = origin - horizontal/2 - vertical/2 - Vec3(0, 0, focal_length);
	}
	
	Ray get_ray(double u, double v) const {
		return Ray(origin, lower_left_corner + u*horizontal + v*vertical - origin);
	}
};
*/


// ITERATIVE APPROACH
inline Color ray_color(const Ray& ray, const Hittable& world, int depth) {
	Ray cur_ray = ray;
	Color accumulated_attenuation(1.0, 1.0, 1.0);
	Color emitted_light(0.0, 0.0, 0.0);

	for(int i = 0; i < depth; ++i) {
		HitRecord record;

		if (world.hit(cur_ray, 0.001, INFINITY, record)) {
			Ray scattered;
			Color attenuation;


			// We pick up any light emitted by the surface we just hit
			// We multiply it by accumulated_attenuation because if this is a bounce,
			// the light is dimmed by the previous surfaces.
			emitted_light += accumulated_attenuation * record.material->emitted();

			if (record.material->scatter(cur_ray, record, attenuation, scattered)) {
				accumulated_attenuation = accumulated_attenuation * attenuation;
				cur_ray = scattered;
			}
			else {
				return emitted_light;
				// return Color(0, 0, 0);
			}
		}
		else {
			Vec3 unit_direction = cur_ray.direction.unit_vector();
			double t = 0.5 * (unit_direction.y + 1.0);
			Color sky_color = (1 - t) * Color(1, 1, 1) + t*Color(0.5, 0.7, 1.0);
			return emitted_light + (accumulated_attenuation * sky_color);
		}
	}
	return Color(0, 0, 0);
}

/**
 * RECURSIVE APPROACH
inline Color ray_color(const Ray& ray, const Hittable& world, int depth) {
	// the heart of the ray tracer
	// it is a recursive function

	if (depth <= 0) return Color(0, 0, 0);
	
	HitRecord record;
	
	if (world.hit(ray, 0.001, INFINITY, record)) {
		Ray scattered;
		Color attenuation;

		// we ask material to scatter the ray 
		// the matrial gives us a new scattered ray and a color attenuration
		// we multiply the color of the current surface of the color of 
		// whatever the next ray hits. this is how color bleeding and 
		// reflections happen
		
		if (record.material->scatter(ray, record, attenuation, scattered)) {
			return attenuation * ray_color(scattered, world, depth - 1);
		}
		
		return Color(0, 0, 0);
	}
	
	Vec3 unit_direction = ray.direction.unit_vector();
	double t = 0.5 * (unit_direction.y + 1.0);
	return (1.0 - t) * Color(1.0, 1.0, 1.0) + t * Color(0.5, 0.7, 1.0);
}
**/
#endif