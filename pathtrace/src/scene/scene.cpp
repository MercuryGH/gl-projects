#include <unordered_map>
#include <filesystem>

#include <glad/glad.h>

#include <glh/model.hpp>
#include <util/types.hpp>
#include <util/math.hpp>
#include <util/random.hpp>

#include <glm/gtx/component_wise.hpp>

#include <scene/scene.hpp>
#include <material/glass_material.hpp>
#include <material/phong_material.hpp>
#include <material/texture_rgb_f.hpp>
#include <light/triangular_area_light.hpp>
#include <scene/bvh_node.hpp>
#include <scene/bvh.hpp>

/**
 * maths based on https://raytracing.github.io/books/RayTracingTheRestOfYourLife.html and the book PBRT
*/
namespace pathtrace {

namespace {

    Vector3 read_comma_separated_vector3(const tinyxml2::XMLElement* xml_element, const char* name) {
        const char* val = xml_element->Attribute(name);
        Vector3 ret;
        int float_read = sscanf(val, "%f,%f,%f", &ret[0], &ret[1], &ret[2]);
        assert(float_read == 3);
        return ret;
    }

    Vector3 read_vector3(const tinyxml2::XMLElement* xml_element) {
        return {
            std::atof(xml_element->Attribute("x")),
            std::atof(xml_element->Attribute("y")),
            std::atof(xml_element->Attribute("z")),
        };
    }

    std::unordered_map<std::string, Vector3> fetch_light_config(const tinyxml2::XMLDocument& xml_doc) {
        std::unordered_map<std::string, Vector3> light_radiance_map;

        for (auto light_node = xml_doc.FirstChildElement("light"); light_node != nullptr; light_node = light_node->NextSiblingElement("light")) {
            const char* mtl_name = light_node->Attribute("mtlname");
            if (mtl_name == nullptr) {
                printf("Warning: no attribute in light node\n");
            }

            Vector3 radiance = read_comma_separated_vector3(light_node, "radiance");

            light_radiance_map.emplace(std::string(mtl_name), radiance);
        }

        return light_radiance_map;
    }

    // fetch material config
    std::vector<IMaterial*> fetch_material_config(const renderer::ObjModel& model, const std::unordered_map<std::string, Vector3>& light_radiance_map, bool read_from_cache) {
        const int n_materials = model.materials().size();
        std::vector<IMaterial*> materials(n_materials);

        for (int i = 0; i < n_materials; i++) {
            const auto& material = model.materials().at(i);
            if (material.ior > 1.0f) {
                // glass
                GlassMaterial* glass_material = new GlassMaterial(material.ior);
                materials.at(i) = glass_material;
            } else {
                // phong
                Vector3 diffuse = { material.diffuse[0], material.diffuse[1], material.diffuse[2] };
                Vector3 specular = { material.specular[0], material.specular[1], material.specular[2] };
                ScalarType phong_exponent = material.shininess;

                // light source
                Vector3 emissive = light_radiance_map.find(material.name) != light_radiance_map.end() ? light_radiance_map.at(material.name) : Vector3{ 0, 0, 0 };

                std::string texture_name = material.diffuse_texname;
                bool has_texture = texture_name.empty() == false;
                std::shared_ptr<TextureRGBf> texture = nullptr;
                if (has_texture) {
                    // load texture
                    std::string obj_file_path_str = model.get_obj_file_path();
                    std::filesystem::path obj_file_path(obj_file_path_str);
                    std::filesystem::path texture_dir = obj_file_path.parent_path() / texture_name;
                    auto[ data, width, height ] = renderer::ObjModel::read_texture_rgbf(texture_dir.generic_string().c_str(), read_from_cache);

                    // texture can be shared
                    texture = std::make_shared<TextureRGBf>(data, width, height);
                }

                PhongMaterial* phong_material = new PhongMaterial(diffuse, specular, emissive, phong_exponent, texture);
                materials.at(i) = phong_material;
            }
        }

        return materials;
    }

    PinholeCamera setup_camera(const tinyxml2::XMLDocument& xml_doc) {
        auto camera_node = xml_doc.FirstChildElement("camera");
        if (camera_node == nullptr) {
            printf("Error: no camera config\n");
            assert(false);
        }

        ScalarType width = std::atoi(camera_node->Attribute("width"));
        ScalarType height = std::atoi(camera_node->Attribute("height"));
        ScalarType fovy = std::atoi(camera_node->Attribute("fovy"));

        Vector3 eye = read_vector3(camera_node->FirstChildElement("eye"));
        Vector3 lookat = read_vector3(camera_node->FirstChildElement("lookat"));
        Vector3 up = read_vector3(camera_node->FirstChildElement("up"));

        auto pinhole_camera = PinholeCamera {
            .eye = eye,
            .lookat = lookat,
            .up = up,
            .fovy = fovy,
            .width = width,
            .height = height
        };
        pinhole_camera.update();
        return pinhole_camera;
    }

}

Scene::Scene(uint32_t width, uint32_t height) {
    set_size(width, height);
}

Scene::~Scene() {
    clear();
}

void Scene::clear() {
    clear_bvh(bvh_root);
    bvh_root = nullptr;
    for (auto object : objects) {
        delete object;
    }
    objects.clear();
    for (auto material : materials) {
        delete material;
    }
    materials.clear();
}

void Scene::set_size(uint32_t width, uint32_t height) {
    display_texture.init(GL_RGBA8, width, height);
    display_texture_buf.resize(width * height * 4);
    buf.set_size(width, height);
}

void Scene::import_scene_file(const char* obj_file_path, const char* mtl_file_path, const char* xml_file_path, bool read_from_cache) {
    clear();

    renderer::ObjModel obj_model(obj_file_path, mtl_file_path, xml_file_path, read_from_cache);

    camera = setup_camera(obj_model.xml_document());
    set_size(camera.width, camera.height);

    auto light_config = fetch_light_config(obj_model.xml_document());
    materials = fetch_material_config(obj_model, light_config, read_from_cache);

    std::vector<IAreaLight*> area_light_list;

    const auto& shapes = obj_model.shapes();
    const auto& attrib = obj_model.attrib();
    for (const auto& shape : shapes) {
        int vertex_idx_offset = 0;

        const int n_tris = shape.mesh.num_face_vertices.size();

        // foreach triangle
        for (int tri_idx = 0; tri_idx < n_tris; tri_idx++) {
            const int n_vertices = shape.mesh.num_face_vertices.at(tri_idx);
            assert(n_vertices == 3); // triangulated

            Vector3 p[3];
            Vector3 normal[3];
            Vector2 uv[3];
            bool has_uv = false;
            bool has_normal = false;
            // foreach vertex in triangle
            for (int i = 0; i < 3; i++) {
                tinyobj::index_t idx = shape.mesh.indices.at(vertex_idx_offset + i);

                // assign component
                for (int j = 0; j < 3; j++) {
                    p[i][j] = attrib.vertices.at(3 * idx.vertex_index + j);
                }

                // assign per-vertex normal
                if (idx.normal_index >= 0) {
                    has_normal = true;

                    for (int j = 0; j < 3; j++) {
                        normal[i][j] = attrib.normals[3 * idx.normal_index + j];
                    }
                }

                // assign uv
                if (idx.texcoord_index >= 0) {
                    has_uv = true;

                    for (int j = 0; j < 2; j++) {
                        uv[i][j] = attrib.texcoords[2 * idx.texcoord_index + j];
                    }
                }

            }
            vertex_idx_offset += 3;

            Triangle* triangle = new Triangle(p[0], p[1], p[2]);
            if (has_normal) {
                // only use vertex 0 normal here
                triangle->set_normal(normal[0]);
            }
            if (has_uv) {
                triangle->set_uv(uv[0], uv[1], uv[2]);
            }

            int material_id = shape.mesh.material_ids.at(tri_idx);
            triangle->set_material(materials.at(material_id));
            objects.push_back(triangle);

            // area light object
            if (triangle->get_material()->light_emitted() != Vector3{ 0, 0, 0 }) {
                TriangularAreaLight* tal = new TriangularAreaLight(*triangle);
                area_light_list.push_back(tal);
            }
        }
    }

    area_lights.set_area_lights(area_light_list);

    std::vector<IHittable*> objects_shallow_copy = objects;
    bvh_root = build_bvh(objects_shallow_copy);
}

void Scene::render(int spp) {
    if (bvh_root == nullptr) {
        return;
    }

    rendering = true;
    spp_progress_percentage.start(spp);

    buf.clear();

    for (cur_spp = 1; cur_spp <= spp; cur_spp++) {
        buf.foreach_pixel_parallel([&](int x, int y) {
            Ray ray_cast = camera.cast_ray(x, y);
            Vector3 color = path_tracing(ray_cast, *bvh_root, x, y);

            // debug check
            for (int i = 0; i < 3; i++) {
                if (std::isfinite(color[i]) == false) {
                    buf.set_pixel_debug(x, y, PixelDebugHintType::eNaNColor);
                }
            }

            buf.add_pixel_color(x, y, color);
        });

        if (true) {
            write_result_to_texture_buf();
        }

        spp_progress_percentage.increment_progress(1);
    }

    rendering = false;
}

Vector3 Scene::path_tracing(const Ray& init_ray, const IHittable& world, int x, int y) {
    Vector3 color{ 0, 0, 0 };
    Vector3 throughput{ 1.0f, 1.0f, 1.0f };

    constexpr int k_max_bounces = 10;

    HitRecord hit_record;
    bool light_source_in_camera = true; // view the light source directly
    Ray cur_ray = init_ray;
    for (int bounce_cnt = 0; bounce_cnt < k_max_bounces; bounce_cnt++) {
        if (world.hit(cur_ray, Vector2{ k_eps, k_max }, hit_record) == false)  {
            break;
        }

        Vector3 wo = -cur_ray.dir;

        ScalarType wo_normal_cosine = glm::dot(hit_record.normal, wo);
        // hit a light source, special case
        if (hit_record.material->light_emitted() != Vector3{ 0, 0, 0 }) {
            if (light_source_in_camera && wo_normal_cosine > 0) {
                color += throughput * hit_record.material->light_emitted();
            }
            break;
        }

        // invert normal if hits a backface
        hit_record.normal = wo_normal_cosine > 0 ? hit_record.normal : -hit_record.normal;
        auto [wi, pdf] = hit_record.material->sample_wi(wo, hit_record);

        // hit a glass, treat it as a special case
        if (hit_record.material->get_type() == MaterialType::eGlass) {
            throughput *= hit_record.material->bxdf(wi, wo, hit_record);
            cur_ray = Ray{ .origin = hit_record.pos, .dir = wi };
            continue;
        }

        light_source_in_camera = false;

        color += throughput * area_lights.sample_light(wo, world, hit_record);
        ScalarType wi_normal_cosine = glm::dot(hit_record.normal, wi);
        if (wi_normal_cosine > 0 && pdf > 0) {
            throughput *= hit_record.material->bxdf(wi, wo, hit_record) * wi_normal_cosine / pdf;
            cur_ray = Ray{ .origin = hit_record.pos, .dir = wi };
        } else {
            // cannot bounce
            break;
        }

        // Russian Roulette
        ScalarType p = glm::compMax(throughput);
        ScalarType russian_roulette = util::get_uniform_real_distribution(0.0f, 1.0f);
        if (russian_roulette > p) {
            break;
        }
        throughput /= p;
    }

    return color;
}

const renderer::GlTexture2D& Scene::get_display_texture() {
    return display_texture;
}

void Scene::write_result_to_texture_buf() {
    buf.foreach_pixel_parallel([&](int x, int y) {
        const int idx = buf.get_pixel_idx(x, y);
        Vector3 color = buf.get_pixel(x, y);
        color /= (float)(cur_spp);

        switch (buf.get_pixel_debug(x, y)) {
        case PixelDebugHintType::eNaNColor: {
            color = { 1, 0, 0 }; // red
            break;
        }
        case PixelDebugHintType::eWrongColor: {
            color = { 0, 1, 0 }; // green
            break;
        }
        case PixelDebugHintType::eSingularSurface: {
            color = { 0, 0, 1 }; // blue
            break;
        }
        default:
            break;
        }

        // clamp color
        for (int i = 0; i < 3; i++) {
            if (color[i] > 1.0f) {
                color[i] = 1.0f;
            }
            if (color[i] < 0) {
                // bug
                assert(false);
            }
        }

        for (int i = 0; i < 3; i++) {
            uint8_t byte_color = static_cast<uint8_t>(color[i] * 255);
            display_texture_buf.at(idx * 4 + i) = byte_color;
        }
        display_texture_buf.at(idx * 4 + 3) = 255; // alpha
    });
}

}
