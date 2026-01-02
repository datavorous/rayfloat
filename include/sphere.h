#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "material.h"
#include <cmath>


class Sphere : public Hittable {
public:
	Vec3 center;
	double radius;
	std::shared_ptr<Material> material;
	
	Sphere(const Vec3& center, double radius, std::shared_ptr<Material> material)
		: center(center), radius(radius), material(material) {}
	
	// `override` -> safety feature introduced in c++11
	// it tells the complier to specifically replace the `hit` function from the parent class
	bool hit(const Ray& ray, double t_min, double t_max, 
			 HitRecord& record) const override {
		
		Vec3 oc = ray.origin - center;
		
		double a = ray.direction.length_squared();
		double half_b = oc.dot(ray.direction);
		double c = oc.length_squared() - radius * radius;
		
		double discriminant = half_b * half_b - a * c;
		
		if (discriminant < 0) {
			return false;
		}
		
		double sqrtd = std::sqrt(discriminant);
		
		double root = (-half_b - sqrtd) / a;
		// the t_min and t_max define the range the ray can see
		// we dont want to hit objects behind the camera (negative t)
		// when a ray hits a surface floating point math isnt perfect
		// the next ray might start slightly INSIDE the surface and hit
		// the same object again instantly
		// setting a small t_min like 0.001 prevent this self intersesction glitch
		if (root < t_min || root > t_max) {
			root = (-half_b + sqrtd) / a;
			if (root < t_min || root > t_max) {
				return false;
			}
		}
		
		// this snippet is the packaging part.
		// once the math determines that the ray has definitely hit the sphere
		// we need to store the details of exactly where and how the hit
		// happneded so the lighting engine use the data later

		// we store the "time" or distance along the ray
		// if the ray hits multiple spheres, the renderer uses this t to see
		// which sphere is closest to the camera
		record.t = root;
		// we calculate the exact 3D coordinates (x, y, z) in space where the 
		// intersection happened, we use the at() function we wrote in ray.h
		record.point = ray.at(root);
		
		// to get a vector pointing out from the sphere, we subtract the 
		// center from the hit_point
		// by dividing the radius, we make the vector a unit vector
		Vec3 outward_normal = (record.point - center) / radius;
		record.set_face_normal(ray, outward_normal);
		
		// i have been hit, here is the material the user gave me when i was created
		// the hitrecord now carries a pointer to that material
		record.material = material;
		
		return true;
	}
};

#endif