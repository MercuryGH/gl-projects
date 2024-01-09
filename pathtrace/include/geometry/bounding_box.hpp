#pragma once

#include <geometry/hittable.hpp>

namespace pathtrace {

class Triangle; // link fails when writes "struct Triangle"

class BoundingBox: public IHittable {
public:
    BoundingBox() { make_empty(); }
    BoundingBox(Vector3 pmin, Vector3 pmax): pmin(pmin), pmax(pmax) {}
    BoundingBox(const BoundingBox& rhs): pmin(rhs.pmin), pmax(rhs.pmax) {}
    BoundingBox(const Triangle& tri);
    void make_empty();

    bool valid() const;
    Vector3 centroid() const;
    Vector3 extent() const;
    int max_extent_dim() const;

    void round_to_int();
    void transform(const Matrix4& mat);
    void merge(const BoundingBox& rhs);
    void merge(const Triangle& tri);
    void merge(Vector3 p);
    void intersect(const BoundingBox& rhs);

    bool intersect_with(const BoundingBox& rhs) const;
    bool contains(const BoundingBox& rhs) const;

    bool hit(const Ray& ray, Vector2 t_range, HitRecord& hit_record) const override;
    void get_bounding_box(BoundingBox &ret_bb) const override;

private:
    Vector3 pmin, pmax;
};

}
