#include <light/area_light_group.hpp>

#include <util/math.hpp>
#include <util/random.hpp>
#include <material/material.hpp>

namespace pathtrace {

namespace {
    template<typename T>
    const T& uniform_sample(const std::vector<T>& list) {
        const int n = list.size();
        int random_index = util::get_uniform_int_distribution(0, n - 1);
        return list.at(random_index);
    }

    template<typename _, typename T>
    const T& uniform_sample(const std::unordered_map<_, T>& map) {
        const int n = map.size();
        int random_index = util::get_uniform_int_distribution(0, n - 1);
        auto itr = std::next(std::begin(map), random_index);
        return (*itr).second;
    }
}

AreaLightGroup::~AreaLightGroup() {
    clear();
}

void AreaLightGroup::clear() {
    for (auto& [_, area_light_list] : area_lights) {
        for (auto area_light : area_light_list) {
            delete area_light;
        }
        area_light_list.clear();
    }
    area_lights.clear();
}

int AreaLightGroup::get_n_lights() const {
    int cnt = 0;
    for (const auto& [_, light_list] : area_lights) {
        cnt += light_list.size();
    }
    return cnt;
}

std::pair<Vector3, ScalarType> AreaLightGroup::uniform_sample_ray(const IHittable& world, const HitRecord& prev_record, HitRecord& light_record) const {
    if (area_lights.size() == 0) {
        return { { 0, 0, 0 }, 0.0f };
    }

    const int n_light_groups = area_lights.size();
    const auto& light_list = uniform_sample(area_lights);
    const int n_lights_in_group = light_list.size();
    auto light = uniform_sample(light_list);

    auto [ wi, pdf ] = light->uniform_sample_ray(world, prev_record, light_record);
    pdf /= (n_light_groups * n_lights_in_group);
    return { wi, pdf };
}

ScalarType AreaLightGroup::pdf(const Ray& ray, const IHittable& world, HitRecord& light_record) const {
    if (area_lights.size() == 0) {
        return 0.0f;
    }

    bool ray_hits = world.hit(ray, Vector2{ k_eps, k_max }, light_record);

    bool hits_light = ray_hits && light_record.material->light_emitted() != Vector3{ 0, 0, 0 };
    bool hits_geometry = ray_hits && light_record.hit_geometry != nullptr;
    ScalarType light_cosine = -glm::dot(ray.dir, light_record.normal);
    bool cosine_judge = light_cosine > 0;

    // hits area light
    if (hits_light && hits_geometry && cosine_judge) {
        ScalarType sq_distance = glm::dot(ray.origin - light_record.pos, ray.origin - light_record.pos);
        ScalarType area = light_record.hit_geometry->get_area();
        ScalarType pdf = sq_distance / (area * light_cosine);

        const int n_light_groups = area_lights.size();
        MaterialIDType material_id = light_record.material->id();
        const int n_lights_in_group = area_lights.at(material_id).size();

        pdf /= (n_light_groups * n_lights_in_group);

        return pdf;
    }

    return 0.0f;
}

Vector3 AreaLightGroup::sample_light(Vector3 wo, const IHittable& world, const HitRecord& hit_record) {
    // power heuristic MIS
    const auto power_heuristic = [](ScalarType f, ScalarType g) {
        ScalarType sq_f = util::sq(f);
        ScalarType sq_g = util::sq(g);

        return sq_f / (sq_f + sq_g);
    };

    Vector3 color{ 0, 0, 0 };
    const auto& surface_material = hit_record.material;
    HitRecord light_record;
    Vector3 light_wi, bxdf_wi;
    ScalarType light_pdf, bxdf_pdf;

    // sample light directly
    std::tie(light_wi, light_pdf) = uniform_sample_ray(world, hit_record, light_record);
    // light > 0, but ignore the case that pdf is very small
    if (light_pdf > k_eps) {
        // see how the material *scatters* the light ray
        bxdf_pdf = surface_material->pdf(light_wi, wo, hit_record);
        if (bxdf_pdf > k_eps) {
            Vector3 bxdf = surface_material->bxdf(light_wi, wo, hit_record);
            ScalarType weight = power_heuristic(light_pdf, bxdf_pdf);
            ScalarType wi_normal_cosine = glm::dot(light_wi, hit_record.normal);
            Vector3 emissive = light_record.material->light_emitted();

            color += bxdf * emissive * wi_normal_cosine * weight / light_pdf;
        }
    }

    // sample wi via material scatter, etc
    std::tie(bxdf_wi, bxdf_pdf) = surface_material->sample_wi(wo, hit_record);
    if (bxdf_pdf > k_eps) {
        Ray wi_ray(hit_record.pos, bxdf_wi);
        // see if the ray hits a light
        light_pdf = pdf(wi_ray, world, light_record);
        if (light_pdf > k_eps) {
            Vector3 bxdf = surface_material->bxdf(bxdf_wi, wo, hit_record);
            ScalarType weight = power_heuristic(bxdf_pdf, light_pdf);
            ScalarType wi_normal_cosine = glm::dot(bxdf_wi, hit_record.normal);
            Vector3 emissive = light_record.material->light_emitted();

            color += bxdf * emissive * wi_normal_cosine * weight / bxdf_pdf;
        }
    }

    return color;
}

}
