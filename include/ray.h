#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class Ray {
public:
	Vec3 origin;
	Vec3 direction;

	Ray() {}
	// unlike Vec3, which initialized its doubles to 0, this constructor does notthing
	// Since Vec3() defaults to (0,0,0), a Ray() will naturally start at the origin and 
	// point nowhere until you give it data

	Ray(const Vec3& origin, const Vec3& direction) : origin(origin), direction(direction) {}

	Vec3 at(double t) const {
		return origin + t * direction;
	}
};

#endif