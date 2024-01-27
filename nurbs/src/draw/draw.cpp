#include <draw/draw.hpp>

namespace nurbs {

namespace {
	void construct_surface_vao(uint32_t vao, uint32_t bind_vbo, uint32_t bind_ebo) {
		glVertexArrayVertexBuffer(vao, 0, bind_vbo, 0, sizeof(SurfaceVertex));
        glVertexArrayElementBuffer(vao, bind_ebo);

		glEnableVertexArrayAttrib(vao, 0);
		glEnableVertexArrayAttrib(vao, 1);

		glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(SurfaceVertex, pos));
		glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(SurfaceVertex, normal));

		glVertexArrayAttribBinding(vao, 0, 0);
		glVertexArrayAttribBinding(vao, 1, 0);
	}

	void construct_line_vao(uint32_t vao, uint32_t bind_vbo) {
		glVertexArrayVertexBuffer(vao, 0, bind_vbo, 0, sizeof(LineVertex));

		glEnableVertexArrayAttrib(vao, 0);

		glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(LineVertex, pos));

		glVertexArrayAttribBinding(vao, 0, 0);
	}

	void construct_line_vao(uint32_t vao, uint32_t bind_vbo, uint32_t bind_ebo) {
        glVertexArrayElementBuffer(vao, bind_ebo);
        construct_line_vao(vao, bind_vbo);
	}
}

// 1-dimension draw line
std::unique_ptr<GlBuffer> create_line_vbo(const std::vector<Vector3>& vertices, uint32_t vao, uint32_t usage) {
    assert(sizeof(LineVertex) == sizeof(Vector3));
    auto vbo = std::make_unique<GlBuffer>(vertices.size() * sizeof(LineVertex), usage, vertices.data());

    construct_line_vao(vao, vbo->id());

    return std::move(vbo);
}

// 1. draw equal length line in the surface
// 2. draw surface control points lines
auto create_line_vbo(const std::vector<std::vector<Vector3>>& vertices, uint32_t vao, uint32_t usage) -> std::tuple<std::unique_ptr<GlBuffer>, std::unique_ptr<GlBuffer>, uint32_t> {
    std::vector<LineVertex> draw_vertices;
    const int n = vertices.size();
    const int m = vertices.at(0).size();
    draw_vertices.resize(n * m);

    // construct vbo
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            const int idx = i * m + j;
            draw_vertices.at(idx).pos = vertices.at(i).at(j);
        }
    }

    auto vbo = std::make_unique<GlBuffer>(draw_vertices.size() * sizeof(LineVertex), usage, draw_vertices.data());

    // construct ebo
    const int n_indices = (n * (m - 1) + m * (n - 1)) * 2;
    std::vector<uint32_t> indices(n_indices);
    int cnt = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m - 1; j++) {
            indices.at(cnt++) = i * m + j;
            indices.at(cnt++) = i * m + j + 1;
        }
    }

    for (int j = 0; j < m; j++) {
        for (int i = 0; i < n - 1; i++) {
            indices.at(cnt++) = i * m + j;
            indices.at(cnt++) = (i + 1) * m + j;
        }
    }

    auto ebo = std::make_unique<GlBuffer>(indices.size() * sizeof(uint32_t), usage, indices.data());

    construct_line_vao(vao, vbo->id(), ebo->id());

    return { std::move(vbo), std::move(ebo), n_indices };
}

// draw blinn-phong shaded surface, uses EBO
auto create_surface_vbo(const std::vector<std::vector<Vector3>>& vertices, const std::vector<std::vector<Vector3>>& normals, uint32_t vao, uint32_t usage) -> std::tuple<std::unique_ptr<GlBuffer>, std::unique_ptr<GlBuffer>, uint32_t> {
    std::vector<SurfaceVertex> draw_vertices;
    const int n = vertices.size();
    const int m = vertices.at(0).size();
    draw_vertices.resize(n * m);

    // construct vbo
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            const int idx = i * m + j;
            draw_vertices.at(idx).pos = vertices.at(i).at(j);
            draw_vertices.at(idx).normal = normals.at(i).at(j);
        }
    }

    auto vbo = std::make_unique<GlBuffer>(draw_vertices.size() * sizeof(SurfaceVertex), usage, draw_vertices.data());

    // construct ebo
    const int n_indices = (n - 1) * (m - 1) * 6; // a quad has 6 vertices (2 triangles)
    std::vector<uint32_t> indices(n_indices);
    for (int i = 0, cnt = 0; i < n - 1; i++) {
        for (int j = 0; j < m - 1; j++) {
            // ABC
            indices.at(cnt++) = i * m + j;
            indices.at(cnt++) = i * m + (j + 1);
            indices.at(cnt++) = (i + 1) * m + (j + 1);
            
            // CDA
            indices.at(cnt++) = (i + 1) * m + (j + 1);
            indices.at(cnt++) = (i + 1) * m + j;
            indices.at(cnt++) = i * m + j;
        }
    }

    auto ebo = std::make_unique<GlBuffer>(indices.size() * sizeof(uint32_t), usage, indices.data());

    construct_surface_vao(vao, vbo->id(), ebo->id());

    return { std::move(vbo), std::move(ebo), n_indices };
}

}