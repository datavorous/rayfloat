#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"
#include "vec3.h"
#include <memory>

class Material;
// "trust me bro it will exist" -> forward declaration

struct HitRecord {
	// we bungle together all the information about a collision.
	// now we can optimise for memory by changing the sequence of how we define
	// the struct's internals.

	Vec3 point;
	Vec3 normal;
	double t;
	bool front_face; // this is a clever design choice 
	// we need to know if the ray is hitting the outside or inside of an object 
	// if the ray and the outward normal point in opposite directions (dot product < 0)
	// we are hitting the front 
	// this logic ensures the normal always points against the ray


	// "smart"(?) pointer that manages memory for us
	// it keeps track of how many things using a piece of memory and deletes it 
	// only when thta count hits zero
	// it prevents that who is owning this memory headache 

	// NOTE: using shared_ptr and pointers to objects can cause cache misses
	std::shared_ptr<Material> material;

	void set_face_normal(const Ray& ray, const Vec3& outward_normal) {
		front_face = ray.direction.dot(outward_normal) < 0;
		normal = front_face? outward_normal : -outward_normal;
	}
};

class Hittable {
public:
	// virtual destructor, when we ue inheritance, we must have a virtual destructor
	// if we delete a sphere through a Hittable* pointer, the virtual destructor ensures 
	// the Sphere part is cleaned up properly, preventing memory leaks 
	// = default tells teh compiler to just use the standard cleanup logic
	virtual ~Hittable() = default;

	// virtual function means the function can be overridden by child classes
	// = 0 makes it a pure virtual function, we can not create an object of type Hittable
	// we can only create objects of classes that inherit from it and implement the hit logic

	// NOTE: calling a virtual function is slightly slower than a regular function
	// the computer has to look up which version of hit() to run at runtime ("vtable loopup")
	virtual bool hit(const Ray& ray, double t_min, double t_max, HitRecord& record) const = 0;

};

#endif