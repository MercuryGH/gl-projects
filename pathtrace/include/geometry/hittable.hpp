#pragma once

#include <light/ray.hpp>
#include <light/hit_record.hpp>

namespace pathtrace {

struct BoundingBox;

// interface
class IHittable {
public:
    virtual bool hit(const Ray& ray, Vector2 t_range, HitRecord& hit_record) const = 0;
    virtual BoundingBox get_bounding_box() const = 0;

    virtual ~IHittable() {}
};

}
