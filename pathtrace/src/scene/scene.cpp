#include <omp.h>

#include <scene/scene.hpp>

#include <glad/glad.h>

#include <glh/model.hpp>

namespace pathtrace {

Scene::Scene(uint32_t width, uint32_t height):
    display_texture(GL_RGBA8, width, height)
    // z_buf(width, height)
{
}

void Scene::import_scene_file(const char* path) {

}

void Scene::render() {
}

const renderer::GlTexture2D& Scene::get_display_texture() {
    return display_texture;
}

void Scene::write_result_to_texture() {
    std::vector<uint8_t> raw_data;

    display_texture.set_data(raw_data.data());
}

void Scene::vpv_transform(const renderer::CameraData& camera_data) {
//     Vector4 viewport = Vector4(0.0f, 0.0f, (float)z_buf.width, (float)z_buf.height);
//     for (int i = 0; i < triangles.size(); i++) {
//         transformed_triangles[i] = triangles[i].vpv_transform(camera_data, viewport);
//     }
}

// void Scene::clear_zbuf() {
//     z_buf.clear();
// }

}
