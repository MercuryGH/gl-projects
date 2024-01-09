#pragma once

#include <geometry/hittable.hpp>
#include <geometry/primitive.hpp>
#include <material/material.hpp>

namespace pathtrace {

// inherit 2 interfaces
class Triangle: public IPrimitive, public IHittable {
public:
    Triangle(Vector3 p1, Vector3 p2, Vector3 p3) {
        pos[0] = p1;
        pos[1] = p2;
        pos[2] = p3;
        normal = glm::normalize(glm::cross(p(1) - p(0), p(2) - p(0)));
    }

    Vector3 p(int index) const { return pos[index]; }
    Vector3 get_normal() const { return normal; }
    IMaterial* get_material() const { return material; }
    void set_normal(Vector3 normal) { this->normal = normal; }
    void set_uv(Vector2 uv1, Vector2 uv2, Vector2 uv3) { 
        has_uv = true;
        uv[0] = uv1;
        uv[1] = uv2;
        uv[2] = uv3;
    }
    void set_material(IMaterial* material) { this->material = material; }

    ScalarType get_area() const override;
    Vector3 get_centroid() const override;

    bool hit(const Ray& ray, Vector2 t_range, HitRecord& hit_record) const override;
    void get_bounding_box(BoundingBox &ret_bb) const override;

private:
    Vector3 pos[3];
    Vector3 normal; // use normal from model data first

    bool has_uv{ false };
    Vector2 uv[3];

    IMaterial* material;
};

}