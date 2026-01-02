// why choosing to define the Vec3 class in a header file?
// the comiler can see the entire function body when compiling other files,
// this allows it to inline the code, replacing the function call with the actual math 
// which is much faster for small ops like vec addition
#ifndef VEC3_H
#define VEC3_H
// if we include this header twice in one file, the first time defines VEC3_H
// the second time, #ifndef fails, and the compiler skips the file.

#include <cmath>


class Vec3 {
public:
	double x, y, z;

	Vec3(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}
	// note: if we had assigned the variables like `{ this->x = x; }` 
	// the variables would first be created with garbage values and then reassigned
	// for simple doubles, the difference is tiny, but for complex objects,
	// the initialiser list is much faster (Member Initializer List)

	Vec3 operator+(const Vec3& other) const {
		// we are using `const` in two distinct ways
		// const Vec3& other -> pass by reference to const 
		// it avoids makings a copy of the object (saves memory/time)
		// gurantees that the function wont accidently change the other vector.

		// the member function's const means that the function will not modify the object 
		// it is called on. it allows us to call these function on 
		// const Vec3 objects
		return Vec3(x + other.x, y + other.y, z + other.z);
	}
	Vec3& operator+=(const Vec3& other) {
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}
	Vec3 operator-(const Vec3& other) const {
		return Vec3(x - other.x, y - other.y, z - other.z);
	}
	Vec3 operator-() const {
		return Vec3(-x, -y, -z);
	}
	Vec3 operator*(double t) const {
		return Vec3(x * t, y * t, z * t);
	}
	Vec3 operator*(const Vec3& other) const {
		return Vec3(x * other.x, y * other.y, z * other.z);
	}
	Vec3 operator/(double t) const {
		return *this * (1.0 / t);
	}

	double dot(const Vec3& other) const {
		return x * other.x + y * other.y + z * other.z;
	}
	Vec3 cross(const Vec3& other) const {
		return Vec3(
				y * other.z - z * other.y,
				z * other.x - x * other.z,
				x * other.y - y * other.x
			);
	}
	double length_squared() const {
		return x * x + y * y + z * z;
	}
	double length() const {
		return std::sqrt(length_squared());
	}
	Vec3 unit_vector() const {
		return *this / length();
	}

	bool near_zero() const {
		const double epsilon = 1e-8;
		return (std::abs(x) < epsilon) && (std::abs(y) < epsilon) && (std::abs(z) < epsilon);
	}
};

// without inline, if two different .cpp files include this heade, the linker will
// find two definitions of the same function and throw and error 
// inline tells the linker that it is okay that this exists in multiple places, they
// are all the same, just pick one
inline Vec3 operator*(double t, const Vec3& v) {
	return v * t;
}

inline Vec3 reflect(const Vec3& v, const Vec3& n) {
	return v - 2 * v.dot(n) * n;
}

/**
 * TODO: Add documentation on how refraction works
 * Explain Snell's law and the decomposition into perpendicular and parallel components
 */
inline Vec3 refract(const Vec3& uv, const Vec3& n, double etai_over_etat) {
	auto cos_theta = std::fmin(-uv.dot(n), 1.0);
	Vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
	Vec3 r_out_parallel = -std::sqrt(std::abs(1.0 - r_out_perp.length_squared())) * n;
	return r_out_perp + r_out_parallel;
}

using Color = Vec3;

#endif