#include <light/area_light_group.hpp>

#include <util/math.hpp>

namespace pathtrace {

ScalarType AreaLightGroup::pdf_hit(const Ray& ray, const IHittable& world, HitRecord& hit_record) const {

    return 0;
}

// Emissive* EmissiveGroup::get_emissive_object_by_index(ObjectIndexType index) {
//     if (emissive_objects.find(index) != emissive_objects.end()) {
//         return emissive_objects.at(index);
//     }
//     return nullptr;
// }


}