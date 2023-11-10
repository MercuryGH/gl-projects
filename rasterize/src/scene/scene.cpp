#include <scene/scene.hpp>

#include <glad/glad.h>

#include <glh/model.hpp>

namespace rasterize {

Scene::Scene(CameraData& camera_data, uint32_t width, uint32_t height):
    camera(camera_data), 
    display_texture(GL_RGBA8, width, height),
    z_buf(width, height)
{
}

void Scene::import_obj_model(const char* path) {
    ObjModel obj_model(path);
    const auto& shapes = obj_model.shapes;
    const auto& attrib = obj_model.attrib;

    for (const auto& shape : shapes) {
        int vertex_idx_offset = 0;
        int triangle_idx_offset = 0;
        for (const auto& n_vertices : shape.mesh.num_face_vertices) {
            assert(n_vertices == 3); // triangulated

            Vector3 p[3];
            for (int i = 0; i < 3; i++) {
                tinyobj::index_t idx = shape.mesh.indices[vertex_idx_offset + i];
                tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];
                p[i] = Vector3(vx, vy, vz);                
            }
            triangles.push_back({ p[0], p[1], p[2] });
            vertex_idx_offset += 3;
            triangle_idx_offset++;
        }
    }
}

void Scene::render_basic() {
    // zbuffer
    for (auto& triangle : triangles) {
        z_buf.rasterize(triangle);
    }
}

void Scene::render_hierarchical() {

}

void Scene::render_octree() {

}

void Scene::render() {

}

const GlTexture2D& Scene::get_render_display_texture() {
    return display_texture;
}

/**
using namespace std;
ofstream file(s);
file << "P3\n"
        << width << " " << height << "\n255\n";
for (int x_t = 0; x_t < width; x_t++) {
    for (int y_t = 0; y_t < height; y_t++) {
        auto& color = n_array[array_id(x_t, y_t)].color;
        file << static_cast<int>(color.r) << " " << static_cast<int>(color.g) << " " << static_cast<int>(color.b) << "  ";
    }
    file << "\n";
}
*/

void Scene::write_render_result_to_texture() {
    std::vector<uint8_t> raw_data;

    z_buf.foreach_pixel([&](int x, int y) {
        const RgbColor& color = z_buf.get_color(x, y);
        for (int i = 0; i < 3; i++) {
            raw_data.push_back(color.data[i]);
        }
    });

    display_texture.set_data(raw_data.data());
}

void Scene::save_render_result_to_file() {

}

// Scene::Scene(const std::filesystem::path &scene_path) {

// }

// void Scene::foreach_instance(const std::function<void(const Instance &, const Model &)> &func) const {

// }

// void Scene::calc_bounding_box() {

// }

}
