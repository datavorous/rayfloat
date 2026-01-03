#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"
#include "aabb.h"
#include <vector>
#include <memory>

class HittableList : public Hittable {
public:

	// this is a dynamic array
	// having smart pointers
	// we store pointer to hittable rather than the objects themselves
	// this is req for polymorphism
	// if we stored just hittable objects the compiler wouldnt know
	// how much memory to allocate (since a sphere be a different size than a triangle)
	// pointers are always the same size
	std::vector<std::shared_ptr<Hittable>> objects;

	HittableList() {}

	void add(std::shared_ptr<Hittable> object) {
		objects.push_back(object);
	}

	void clear() {
		objects.clear();
	}

	bool hit(const Ray& ray, double t_min, double t_max, HitRecord& record) const override {
		HitRecord temp_record;
		bool hit_anything = false;
		double closest_so_far = t_max;

		for(const auto& object : objects) {
			// imagine a ray shooting through a scene with three spheres
			// the ray might technically hit all three, however the camera should
			// only "see" the one that is closest.
			// O(n) -> very slow 
			// we should have BVH to skip checking objects that the ray is nowhere near

			// jumping between pointers in a list is slower for the CPU 
			// than reading a contiguous block of math
			if (object->hit(ray, t_min, closest_so_far, temp_record)) {
				hit_anything = true;
				closest_so_far = temp_record.t;
				// shallow copy
				record = temp_record;
			}
		}
		return hit_anything;
	}
	
	bool bounding_box(AABB& output_box) const override {
		if (objects.empty()) return false;

		AABB temp_box;
		bool first_box = true;

		for (const auto& object : objects) {
			if (!object->bounding_box(temp_box))
				return false;
			output_box = first_box ? temp_box : AABB::surrounding_box(output_box, temp_box);
			first_box = false;
		}
		return true;
	}
};

#endif