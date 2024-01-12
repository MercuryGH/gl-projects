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
    renderer::ObjModel obj_model(path, true);
    const auto& shapes = obj_model.shapes();
    const auto& attrib = obj_model.attrib();

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
    z_buf.clear();
    for (auto& triangle : transformed_triangles) {
        z_buf.rasterize(triangle);
        // break;
    }
}

void Scene::rasterize_hierarchical() {
    n_culled_triangles = 0;
    z_buf.build_hierarchical_z_buf();
    rasterize_hierarchical_triangles(transformed_triangles);
    printf("Culled #tri = %d\n", n_culled_triangles);
}

void Scene::rasterize_hierarchical_triangles(const std::vector<Triangle>& triangles) {
    for (auto& triangle : triangles) {
        if (z_buf.is_occluded(triangle) == false) {
            z_buf.rasterize(triangle);
        } else {
            n_culled_triangles++;
        }
    }
}

void Scene::build_octree_and_rasterize(const std::vector<Triangle>& triangles) {
    constexpr auto k_direct_rastrize_n_triangles = 16;
    if (triangles.size() < k_direct_rastrize_n_triangles) {
        rasterize_hierarchical_triangles(triangles);
        return;
    }

    // get all triangles bb
    BoundingCuboid bb;
    for (const auto& triangle : triangles) {
        bb.merge(triangle);
    }

    // 8 divide
    std::vector<BoundingCuboid> bbs(8, bb);
    Vector3 center = bb.centroid();
    int x_max_idx[4] = { 0, 2, 4, 6 };
    int x_min_idx[4] = { 1, 3, 5, 7 };
    int y_max_idx[4] = { 0, 1, 4, 5 };
    int y_min_idx[4] = { 2, 3, 6, 7 };
    int z_max_idx[4] = { 0, 1, 2, 3 }; // half front z
    int z_min_idx[4] = { 4, 5, 6, 7 }; // half back z
    for (int i = 0; i < 4; i++) {
        bbs.at(x_max_idx[i]).pmax.x = center.x;
        bbs.at(x_min_idx[i]).pmin.x = center.x;
        bbs.at(y_max_idx[i]).pmax.y = center.y;
        bbs.at(y_min_idx[i]).pmin.y = center.y;
        bbs.at(z_max_idx[i]).pmax.z = center.z;
        bbs.at(z_min_idx[i]).pmin.z = center.z;
    }

    std::vector<Triangle> test_tris[8];
    std::vector<Triangle> test_edge_tris;
    for (const auto& triangle : triangles) {
        BoundingCuboid tri_bb;
        tri_bb.merge(triangle);
        bool triangle_across_edge = true;
        for (int i = 0; i < 8; i++) {
            if (bbs.at(i).contains(tri_bb)) {
                test_tris[i].push_back(triangle);
                triangle_across_edge = false;
                break;
            }
        }
        if (triangle_across_edge == true) {
            test_edge_tris.push_back(triangle);
        }
    }

    for (int i = 0; i < 4; i++) { // front (half front z)
        build_octree_and_rasterize(test_tris[i]);
    }
    rasterize_hierarchical_triangles(test_edge_tris); // triangles across the divided area
    for (int i = 4; i < 8; i++) { // back (half back z)
        build_octree_and_rasterize(test_tris[i]);
    }
}

void Scene::rasterize_octree() {
    n_culled_triangles = 0;
    z_buf.build_hierarchical_z_buf();
    build_octree_and_rasterize(transformed_triangles);
    printf("Culled #tri = %d\n", n_culled_triangles);
}

const renderer::GlTexture2D& Scene::get_display_texture() {
    return display_texture;
}

void Scene::write_result_to_texture() {
    std::vector<uint8_t> raw_data;

    z_buf.foreach_pixel([&](int x, int y) {
        const ScalarType depth = z_buf.get_depth(x, y);

        const ScalarType rec = 1 / (z_buf.max_depth - z_buf.min_depth);
        ScalarType t = glm::clamp(z_buf.get_depth(x, y), z_buf.min_depth, z_buf.max_depth);
        ScalarType normalized_depth = (t - z_buf.min_depth) * rec;
        uint8_t byte_depth = static_cast<uint8_t>(normalized_depth * 255);
        // byte_depth = 255 - byte_depth; // invert black/white if necessary

        for (int i = 0; i < 3; i++) {
            raw_data.push_back(byte_depth); // rgb
            // raw_data.push_back(get_color(x, y).data[i]); // rgb
        }
        raw_data.push_back(255); // alpha
    });

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
