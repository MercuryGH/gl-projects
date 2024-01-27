#include <draw/scene.hpp>
#include <draw/draw.hpp>

namespace nurbs {

void Scene::load(const std::vector<tinynurbs::RationalCurve<ScalarType>>& curves, const std::vector<tinynurbs::RationalSurface<ScalarType>>& surfaces) {
    clear();
    this->curves = curves;
    this->surfaces = surfaces;
    const int n_curves = curves.size();
    const int n_surfaces = surfaces.size();

    // init
    curve_draw_data.resize(n_curves);
    curve_control_point_lines.resize(n_curves);
    curve_derivative_lines.resize(n_curves);

    surface_draw_data.resize(n_surfaces);
    surface_mesh_lines.resize(n_surfaces);
    surface_control_point_lines.resize(n_surfaces);
    surface_normal_lines.resize(n_surfaces);
    surface_u_derivative_lines.resize(n_surfaces);
    surface_v_derivative_lines.resize(n_surfaces);

    for (int i = 0; i < n_curves; i++) {
        glCreateVertexArrays(1, &curve_draw_data.at(i).vao);
        glCreateVertexArrays(1, &curve_control_point_lines.at(i).vao);
        glCreateVertexArrays(1, &curve_derivative_lines.at(i).vao);
    }
    for (int i = 0; i < n_surfaces; i++) {
        glCreateVertexArrays(1, &surface_draw_data.at(i).vao);
        glCreateVertexArrays(1, &surface_mesh_lines.at(i).vao);
        glCreateVertexArrays(1, &surface_control_point_lines.at(i).vao);
        glCreateVertexArrays(1, &surface_normal_lines.at(i).vao);
        glCreateVertexArrays(1, &surface_u_derivative_lines.at(i).vao);
        glCreateVertexArrays(1, &surface_v_derivative_lines.at(i).vao);
    }

    Vector3 k_zeros{ 0, 0, 0 };
    // fetch data
    for (int k = 0; k < n_curves; k++) {
        const auto& curve = curves.at(k);
        constexpr int resolution = 1000;
        std::vector<Vector3> vertices(resolution);
        ScalarType u = 0;
        ScalarType delta_u = 1.0 / (resolution - 1);
        for (int i = 0; i < resolution; i++) {
            Vector3 p = tinynurbs::curvePoint(curve, u);
            vertices.at(i) = p;
            u += delta_u;
        }

        auto volatile _vertices = vertices;
        curve_draw_data.at(k).vbo = create_line_vbo(vertices, curve_draw_data.at(k).vao);

        // may require type conversion between the 2 data
        // std::vector<Vector3> control_points = curve.control_points;

        std::vector<Vector3> control_points(curve.control_points.size());
        for (int i = 0; i < curve.control_points.size(); i++) {
            control_points.at(i) = curve.control_points.at(i);
        }
        curve_control_point_lines.at(k).vbo = create_line_vbo(control_points, curve_control_point_lines.at(k).vao);

        curve_derivative_lines.at(k).vbo = create_line_vbo({ k_zeros, k_zeros }, curve_derivative_lines.at(k).vao, GL_MAP_WRITE_BIT);
    }

    for (int k = 0; k < n_surfaces; k++) {
        const auto& surface = surfaces.at(k);
        constexpr int x_resolution = 40; // number of sample points in u dir
        constexpr int y_resolution = 40;

        std::vector<std::vector<Vector3>> vertices;
        std::vector<std::vector<Vector3>> normals;
        vertices.resize(x_resolution);
        normals.resize(x_resolution);

        ScalarType u = 0;
        const ScalarType delta_u = 1.0 / (x_resolution - 1);
        for (int x = 0; x < x_resolution; x++) {
            ScalarType v = 0;
            const ScalarType delta_v = 1.0 / (y_resolution - 1);
            vertices.at(x).resize(y_resolution);
            normals.at(x).resize(y_resolution);
            for (int y = 0; y < y_resolution; y++) {
                Vector3 p = tinynurbs::surfacePoint(surface, u, v);
                Vector3 n = tinynurbs::surfaceNormal(surface, u, v);

                vertices.at(x).at(y) = p;
                normals.at(x).at(y) = glm::normalize(n);
                v += delta_v;
            }
            u += delta_u;
        }

        std::tie(surface_draw_data.at(k).vbo, surface_draw_data.at(k).ebo, surface_draw_data.at(k).n_indices) = create_surface_vbo(vertices, normals, surface_draw_data.at(k).vao);
        std::tie(surface_mesh_lines.at(k).vbo, surface_mesh_lines.at(k).ebo, surface_mesh_lines.at(k).n_indices) = create_line_vbo(vertices, surface_mesh_lines.at(k).vao);

        // construct control points
        std::vector<std::vector<Vector3>> control_points;
        const int n = surface.control_points.rows();
        const int m = surface.control_points.cols();
        control_points.resize(n);
        for (int i = 0; i < n; i++) {
            control_points.at(i).resize(m);
            for (int j = 0; j < m; j++) {
                control_points.at(i).at(j) = surface.control_points[i * m + j];
            }
        }

        std::tie(surface_control_point_lines.at(k).vbo, surface_control_point_lines.at(k).ebo, surface_control_point_lines.at(k).n_indices) = create_line_vbo(control_points, surface_control_point_lines.at(k).vao);

        surface_u_derivative_lines.at(k).vbo = create_line_vbo({ k_zeros, k_zeros }, surface_u_derivative_lines.at(k).vao, GL_MAP_WRITE_BIT);
        surface_v_derivative_lines.at(k).vbo = create_line_vbo({ k_zeros, k_zeros }, surface_v_derivative_lines.at(k).vao, GL_MAP_WRITE_BIT);

        surface_normal_lines.at(k).vbo = create_line_vbo({ k_zeros, k_zeros }, surface_normal_lines.at(k).vao, GL_MAP_WRITE_BIT);
    }
}

void Scene::tick(float curve_u, float surface_u, float surface_v) {
    constexpr ScalarType k_line_length = 1.0;

    auto assign_line_vertex_data = [](GlBuffer& gl_buffer, Vector3 start, Vector3 end) {
        auto data = gl_buffer.typed_map<LineVertex>(true);
        data[0].pos = start;
        data[1].pos = end;
        gl_buffer.unmap();
    };

    const int n_curves = curves.size();
    const int n_surfaces = surfaces.size();
    for (int k = 0; k < n_curves; k++) {
        const auto& curve = curves.at(k);

        Vector3 p = tinynurbs::curvePoint(curve, curve_u);
        Vector3 d = glm::normalize(tinynurbs::curveDerivatives(curve, 1, curve_u).at(1));
        Vector3 end_p = p - k_line_length * d;

        // should check if dirty. Omit for simplicity
        assign_line_vertex_data(*curve_derivative_lines.at(k).vbo, p, end_p);
    }

    for (int k = 0; k < n_surfaces; k++) {
        const auto& surface = surfaces.at(k);

        Vector3 p = tinynurbs::surfacePoint(surface, surface_u, surface_v);
        auto res = tinynurbs::surfaceDerivatives(surface, 1, surface_u, surface_v);
        Vector3 u_d = glm::normalize(res(1, 0)); // u-dir derivative
        Vector3 v_d = glm::normalize(res(0, 1)); // v-dir derivative

        auto data = surface_u_derivative_lines.at(k).vbo->typed_map<LineVertex>(true);
        data[0].pos = p;
        data[1].pos = u_d;

        assign_line_vertex_data(*surface_u_derivative_lines.at(k).vbo, p, p - k_line_length * u_d);
        assign_line_vertex_data(*surface_v_derivative_lines.at(k).vbo, p, p - k_line_length * v_d);

        Vector3 normal = tinynurbs::surfaceNormal(surface, surface_u, surface_v);
        assign_line_vertex_data(*surface_normal_lines.at(k).vbo, p, p + k_line_length * normal);
    }
}

void Scene::render_lines(const std::vector<GlRender>& line_draw_data) {
    for (const auto& data : line_draw_data) {
        glBindVertexArray(data.vao);
        if (data.ebo == nullptr) {
            glDrawArrays(GL_LINE_STRIP, 0, data.vbo->size() / sizeof(LineVertex));
        } else {
            glDrawElements(GL_LINES, data.n_indices, GL_UNSIGNED_INT, 0);
        }
    }
}

void Scene::render_triangles(const std::vector<GlRender>& tri_draw_data) {
    for (const auto& data : tri_draw_data) {
        glBindVertexArray(data.vao);
        glDrawElements(GL_TRIANGLES, data.n_indices, GL_UNSIGNED_INT, 0);
    }
}

Scene::~Scene() {
    clear();
}

void Scene::clear() {
    const int n_curves = curves.size();
    const int n_surfaces = surfaces.size();
    for (int i = 0; i < n_curves; i++) {
        glDeleteVertexArrays(1, &curve_draw_data.at(i).vao);
        glDeleteVertexArrays(1, &curve_control_point_lines.at(i).vao);
        glDeleteVertexArrays(1, &curve_derivative_lines.at(i).vao);
    }
    for (int i = 0; i < n_surfaces; i++) {
        glDeleteVertexArrays(1, &surface_draw_data.at(i).vao);
        glDeleteVertexArrays(1, &surface_mesh_lines.at(i).vao);
        glDeleteVertexArrays(1, &surface_control_point_lines.at(i).vao);
        glDeleteVertexArrays(1, &surface_normal_lines.at(i).vao);
        glDeleteVertexArrays(1, &surface_u_derivative_lines.at(i).vao);
        glDeleteVertexArrays(1, &surface_v_derivative_lines.at(i).vao);
    }
}

}