#ifndef BVH_H
#define BVH_H

#include "hittable.h"
#include "aabb.h"
#include "hittable_list.h"
#include <memory>
#include <algorithm>
#include <stdexcept>

class BVHNode : public Hittable {
public:
    std::shared_ptr<Hittable> left;
    std::shared_ptr<Hittable> right;
    AABB box;

    BVHNode() {}

    BVHNode(HittableList& list) : BVHNode(list.objects, 0, list.objects.size()) {}
    BVHNode(std::vector<std::shared_ptr<Hittable>>& objects, size_t start, size_t end) {

        int axis = random_int(0, 2);
        auto comparator = (axis == 0) ? box_x_compare
                        : (axis == 1) ? box_y_compare
                                      : box_z_compare;
        size_t object_span = end - start;

        if (object_span == 1) {
            left = right = objects[start];
        } else if (object_span == 2) {
            if (comparator(objects[start], objects[start + 1])) {
                left = objects[start];
                right = objects[start + 1];
            } else {
                left = objects[start + 1];
                right = objects[start];
            }
        } else {
            std::sort(objects.begin() + start, objects.begin() + end, comparator);

            size_t mid = start + object_span / 2;
            left = std::make_shared<BVHNode>(objects, start, mid);
            right = std::make_shared<BVHNode>(objects, mid, end);
        }

        AABB box_left, box_right;
        if (!left->bounding_box(box_left) || !right->bounding_box(box_right))
            throw std::runtime_error("No bounding box in BVHNode constructor.");
        box = AABB::surrounding_box(box_left, box_right);
    }

    bool hit(const Ray& ray, double t_min, double t_max, HitRecord& record) const override {
        if (!box.hit(ray, t_min, t_max))
            return false;

        bool hit_left = left->hit(ray, t_min, t_max, record);
        bool hit_right = right->hit(ray, t_min, hit_left ? record.t : t_max, record);

        return hit_left || hit_right;
    }

    bool bounding_box(AABB& output_box) const override {
        output_box = box;
        return true;
    }

private:
    static int random_int(int min, int max) {
        return static_cast<int>(random_double(min, max + 1));
    }

    static bool box_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b, int axis) {
        AABB box_a, box_b;
        if (!a->bounding_box(box_a) || !b->bounding_box(box_b))
            throw std::runtime_error("No bounding box in BVHNode comparison.");

        double a_min = (axis == 0) ? box_a.minimum.x : (axis == 1) ? box_a.minimum.y : box_a.minimum.z;
        double b_min = (axis == 0) ? box_b.minimum.x : (axis == 1) ? box_b.minimum.y : box_b.minimum.z;
        return a_min < b_min;
    }

    static bool box_x_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
        return box_compare(a, b, 0);
    }
    static bool box_y_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
        return box_compare(a, b, 1);
    }
    static bool box_z_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
        return box_compare(a, b, 2);
    }
};
#endif