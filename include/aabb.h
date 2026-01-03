#ifndef AABB_H
#define AABB_H

#include "vec3.h"
#include "ray.h"

#include <algorithm>

class AABB {
public:
    Vec3 minimum;
    Vec3 maximum;

    AABB() {}
    AABB(const Vec3& a, const Vec3& b) : minimum(a), maximum(b) {}

    bool hit(const Ray& r, double t_min, double t_max) const {
        for(int a = 0; a < 3; ++a) {
            double origin_component = (a==0)? r.origin.x : (a==1)? r.origin.y : r.origin.z;
            double direction_component = (a==0)? r.direction.x : (a==1)? r.direction.y : r.direction.z;
            double min_component = (a==0)? minimum.x : (a==1)? minimum.y : minimum.z;
            double max_component = (a==0)? maximum.x : (a==1)? maximum.y : maximum.z;

            double invD = 1.0 / direction_component;
            double t0 = (min_component - origin_component) * invD;
            double t1 = (max_component - origin_component) * invD;
            if (invD < 0.0) std::swap(t0, t1);
            t_min = t0 > t_min ? t0 : t_min;
            t_max = t1 < t_max ? t1 : t_max;
            if (t_max <= t_min)
                return false;
        }
        return true;
    }

    static AABB surrounding_box(const AABB& box0, const AABB& box1) {
        Vec3 small(
            std::min(box0.minimum.x, box1.minimum.x),
            std::min(box0.minimum.y, box1.minimum.y),
            std::min(box0.minimum.z, box1.minimum.z)
        );
        Vec3 big(
            std::max(box0.maximum.x, box1.maximum.x),
            std::max(box0.maximum.y, box1.maximum.y),
            std::max(box0.maximum.z, box1.maximum.z)
        );
        return AABB(small, big);
    }
};
#endif