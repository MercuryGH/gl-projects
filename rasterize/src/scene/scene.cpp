#include <scene/scene.hpp>

#include <glad/glad.h>

#include <glh/model.hpp>

namespace rasterize {

Scene::Scene(uint32_t width, uint32_t height):
    display_texture(GL_RGBA8, width, height),
    z_buf(width, height)
{
}

void Scene::import_obj_model(const char* path) {
    renderer::ObjModel obj_model(path);
    const auto& shapes = obj_model.shapes;
    const auto& attrib = obj_model.attrib;

    triangles.clear();

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
    transformed_triangles.clear();
    transformed_triangles.resize(triangles.size());
}

void Scene::rasterize_basic() {
    for (auto& triangle : transformed_triangles) {
        z_buf.rasterize(triangle);
        // break;
    }
}

void Scene::rasterize_hierarchical() {
    z_buf.build_hierarchical_z_buf();
    for (auto& triangle : transformed_triangles) {
        if (z_buf.is_occluded(triangle) == false) {
            z_buf.rasterize(triangle);
        }
    }
}

void Scene::rasterize_octree() {

}

const renderer::GlTexture2D& Scene::get_display_texture() {
    return display_texture;
}

void Scene::write_result_to_texture() {
    std::vector<uint8_t> raw_data;

    z_buf.foreach_pixel([&](int x, int y) {
        const RgbColor& color = z_buf.get_color(x, y);
        const ScalarType depth = z_buf.get_depth(x, y);

        const ScalarType rec = 1 / (z_buf.max_depth - z_buf.min_depth);
        ScalarType t = glm::clamp(z_buf.get_depth(x, y), z_buf.min_depth, z_buf.max_depth);
        ScalarType normalized_depth = (t - z_buf.min_depth) * rec;
        uint8_t byte_depth = static_cast<uint8_t>(normalized_depth * 255);
        byte_depth = 255 - byte_depth;

        for (int i = 0; i < 3; i++) {
            raw_data.push_back(byte_depth); // rgb
            // raw_data.push_back(z_buf.get_color(x, y).data[i]); // rgb
        }
        raw_data.push_back(255); // alpha
    });

    // format: RGBA8
    display_texture.set_data(raw_data.data());
}

void Scene::vpv_transform(const renderer::CameraData& camera_data) {
    Vector4 viewport = Vector4(0.0f, 0.0f, (float)z_buf.width, (float)z_buf.height);
    for (int i = 0; i < triangles.size(); i++) {
        transformed_triangles[i] = triangles[i].vpv_transform(camera_data, viewport);
    }
}

void Scene::clear_zbuf() {
    z_buf.clear();
}

}
