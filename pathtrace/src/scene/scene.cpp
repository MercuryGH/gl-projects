#include <unordered_map>

#include <omp.h>

#include <glad/glad.h>

#include <glh/model.hpp>

#include <scene/scene.hpp>
#include <material/glass_material.hpp>
#include <material/phong_material.hpp>
#include <material/texture_rgb_f.hpp>
#include <light/triangular_area_light.hpp>
#include <scene/bvh_node.hpp>
#include <scene/bvh.hpp>

/**
 * maths based on https://raytracing.github.io/books/RayTracingTheRestOfYourLife.html
*/
namespace pathtrace {

namespace {

    Vector3 read_3_floats(const char* text) {
        Vector3 ret;
        int float_read = sscanf(text, "%f,%f,%f", &ret[0], &ret[1], &ret[2]);
        assert(float_read == 3);
        return ret;
    }

    float read_float(const char* text) {
        float ret;
        int float_read = sscanf(text, "%f", &ret);
        assert(float_read == 1);
        return ret;
    }

    std::unordered_map<std::string, Vector3> fetch_light_config(const tinyxml2::XMLDocument& xml_doc) {
        std::unordered_map<std::string, Vector3> light_radiance_map;

        for (auto light_node = xml_doc.FirstChildElement("light"); light_node != nullptr; light_node = light_node->NextSiblingElement("light")) {
            const char* mtl_name = light_node->Attribute("mtlname");
            if (mtl_name == nullptr) {
                printf("Warning: no attribute in light node\n");
            }

            const char* radiance_str = light_node->Attribute("radiance");
            if (radiance_str == nullptr) {
                printf("Warning: no radiance in light node\n");
            }
            Vector3 radiance = read_3_floats(radiance_str);

            light_radiance_map.emplace(std::string(mtl_name), radiance);
        }

        return light_radiance_map;
    }

    // fetch material config
    std::vector<IMaterial*> fetch_material_config(const renderer::ObjModel& model, const std::unordered_map<std::string, Vector3>& light_radiance_map) {
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
                    std::string obj_file_path = model.get_obj_file_path();
                    std::vector<float> data = renderer::ObjModel::read_texture_rgbf((obj_file_path + texture_name).c_str());

                    // texture can be shared
                    texture = std::make_shared<TextureRGBf>(data);
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

        Vector3 eye = read_3_floats(camera_node->FirstChildElement("eye")->Attribute("value"));
        Vector3 lookat = read_3_floats(camera_node->FirstChildElement("lookat")->Attribute("value"));
        Vector3 up = read_3_floats(camera_node->FirstChildElement("up")->Attribute("value"));
        ScalarType fovy = read_float(camera_node->FirstChildElement("fovy")->Attribute("value"));
        ScalarType width = read_float(camera_node->FirstChildElement("width")->Attribute("value"));
        ScalarType height = read_float(camera_node->FirstChildElement("height")->Attribute("value"));

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

Scene::Scene(uint32_t width, uint32_t height):
    display_texture(GL_RGBA8, width, height),
    buf(width, height)
{
}

Scene::~Scene() {
    clear();
}

void Scene::clear() {
    clear_bvh(bvh_root);
    for (auto object : objects) {
        delete object;
    }
    objects.clear();
    for (auto material : materials) {
        delete material;
    }
    materials.clear();
}

void Scene::import_scene_file(const char* obj_file_path, const char* xml_file_path) {
    renderer::ObjModel obj_model(obj_file_path, xml_file_path);

    auto light_config = fetch_light_config(obj_model.xml_document());
    materials = fetch_material_config(obj_model, light_config);

    clear();

    std::vector<IAreaLight*> area_light_list;

    const auto& shapes = obj_model.shapes();
    const auto& attrib = obj_model.attrib();
    for (const auto& shape : shapes) {
        int vertex_idx_offset = 0;

        const int n_tris = shape.mesh.num_face_vertices.size();
        objects.resize(n_tris);

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
            objects.at(tri_idx) = triangle;

            // area light object
            if (triangle->get_material()->get_type() == MaterialType::eEmissive) {
                TriangularAreaLight* tal = new TriangularAreaLight(*triangle);
                area_light_list.push_back(tal);
            }
        }
    }

    area_lights.set_area_lights(area_light_list);

    std::vector<IHittable*> objects_shallow_copy = objects;
    bvh_root = build_bvh(objects_shallow_copy);

    camera = setup_camera(obj_model.xml_document());
}

void Scene::render() {
}

Vector3 Scene::path_tracing(const Ray& ray, const IHittable& world) {
    return { 0, 0, 0 };
}

Vector3 Scene::sample_light(Vector3 wo, const IHittable& world, const HitRecord& hit_record) {
    return { 0, 0, 0 };
}

const renderer::GlTexture2D& Scene::get_display_texture() {
    return display_texture;
}

void Scene::write_result_to_texture(int spp) {
    std::vector<uint8_t> raw_data(buf.get_n_pixels() * 4);

    buf.foreach_pixel([&](int x, int y) {
        const int idx = buf.get_pixel_idx(x, y);
        Vector3 color = buf.get_pixel(x, y);
        color /= (float)(spp);

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
            raw_data.at(idx * 4 + i) = byte_color;
        }
        raw_data.at(idx * 4 + 3) = 255; // alpha
    });

    display_texture.set_data(raw_data.data());
}

}
