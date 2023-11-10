#include <draw/draw.hpp>

#include <vector>
#include <iostream>

#include <util/types.hpp>

#include <glad/glad.h>
#include <poly2tri/poly2tri.h>
#include <dataop/graph.hpp>

namespace brep {
	using namespace renderer;

namespace {
	struct Triangle {
		Vector3 v[3];
		Vector3 normal;
	};

	struct Plane {
		Vector3 origin;
		Vector3 x, y, z; // right hand coordinate (unit vec)
		static constexpr float k_eps = 0.01f;
		Plane(Face* face) {
			auto hedge_head = face->loop_head->hedge_head;
			auto cur_hedge = hedge_head;

			auto v1 = cur_hedge->start_v->coordinate->pos;
			cur_hedge = cur_hedge->next;
			
			auto v2 = cur_hedge->start_v->coordinate->pos;
			cur_hedge = cur_hedge->next;

			origin = v1;
			x = glm::normalize(v2 - v1);
			
			while (cur_hedge != hedge_head) {
				auto v3 = cur_hedge->start_v->coordinate->pos;
				if (glm::length(glm::cross(v3 - v1, v3 - v2)) > k_eps) {
					z = glm::normalize(glm::cross(x, v3 - v2));
					y = glm::normalize(glm::cross(z, x));
					break;
				}
			}
		}
		Vector2 project_2d(Vector3 p) {
			auto diff = p - origin;

			if (glm::dot(diff, z) > k_eps) { 
				// p is not on the plane
				std::cout << "p = " << p.x << ", " << p.y << ", " << p.z << ", origin = " << origin.x << ", " << origin.y << ", " << origin.z << "\n";
				assert(false);
			}

			return Vector2(glm::dot(diff, x), glm::dot(diff, y));
		}
		Vector3 unproject_3d(Vector2 p) {
			return origin + p.x * x + p.y * y;
		}
	};

	std::vector<Triangle> generate_triangles(Solid* solid) {
		std::vector<Triangle> triangles;
		auto poly2tri = [&](Face* face) {
			Plane plane(face);
			std::vector<p2t::Point*> outer_points;

			auto handle_outer_loop = [&]() {
				Loop* outer_loop = face->get_first_outer_loop();

				HalfEdge* cur_hedge = outer_loop->hedge_head;
				do {
					Vector2 p = plane.project_2d(cur_hedge->start_v->coordinate->pos);

					p2t::Point* point = new p2t::Point(p.x, p.y);
					outer_points.push_back(point);

					cur_hedge = cur_hedge->next;
				} while (cur_hedge != outer_loop->hedge_head);
			};

			handle_outer_loop();

			p2t::CDT cdt(outer_points); // Constrained Delaunay Triangulation

			auto handle_inner_loops = [&]() {
				Loop* cur_loop = face->loop_head;
				do {
					if (cur_loop->is_inner_loop() == false) {
						cur_loop = cur_loop->next;
						continue;
					}

					HalfEdge* cur_hedge = cur_loop->hedge_head;
					std::vector<p2t::Point*> hole_points;
					do {
						Vector2 p = plane.project_2d(cur_hedge->start_v->coordinate->pos);

						p2t::Point* point = new p2t::Point(p.x, p.y);
						hole_points.push_back(point);

						cur_hedge = cur_hedge->next;
					} while (cur_hedge != cur_loop->hedge_head);

					cdt.AddHole(hole_points);
					cur_loop = cur_loop->next;
				} while (cur_loop != face->loop_head);
			};

			handle_inner_loops();

			cdt.Triangulate();

			for (const auto& tri : cdt.GetTriangles()) {
				Triangle t;
				for (int i = 0; i < 3; i++) {
					auto p = tri->GetPoint(i);
					t.v[i] = plane.unproject_3d(Vector2(p->x, p->y));
					t.normal = plane.z;
				}
				triangles.emplace_back(t);
			}
		};

		auto cur_face = solid->face_head;
		do {
			poly2tri(cur_face);
			cur_face = cur_face->next;
		} while (cur_face != solid->face_head);

		return triangles;
	}

	struct Segment {
		Vector3 start;
		Vector3 end;
	};

	std::vector<Segment> generate_segments(Solid* solid) {
		std::vector<Segment> segments;

		Edge* cur_edge = solid->edge_head;
		do {
			auto v1 = cur_edge->hedge1->start_v->coordinate->pos;
			auto v2 = cur_edge->hedge1->end_v->coordinate->pos;

			Segment segment{ .start = v1, .end = v2 };
			segments.emplace_back(segment);

			cur_edge = cur_edge->next;
		} while (cur_edge != solid->edge_head);

		return segments;
	}

	struct Vertex {
		Vector3 pos;
		Vector3 normal;
	};

	void construct_vao(uint32_t vao, uint32_t bind_vbo) {
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, bind_vbo);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		// GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, pos)));
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
	}

	std::unique_ptr<GlBuffer> create_triangle_vbo(std::vector<Triangle> triangles, uint32_t vao) {

		// construct vertex
		std::vector<Vertex> vertices(triangles.size() * 3);
		for (int i = 0; i < triangles.size(); i++) {
			for (int j = 0; j < 3; j++) {
				vertices[i * 3 + j].pos = triangles[i].v[j];
				vertices[i * 3 + j].normal = -triangles[i].normal;
			}
		}

		auto vbo = std::make_unique<GlBuffer>(vertices.size() * sizeof(Vertex), 0, vertices.data());

		construct_vao(vao, vbo->id());

		return std::move(vbo);
	}

	std::unique_ptr<GlBuffer> create_wireframe_vbo(std::vector<Segment> segments, uint32_t vao) {
		// construct vertex
		std::vector<Vertex> vertices(segments.size() * 2);
		for (int i = 0; i < segments.size(); i++) {
			vertices[i * 2 + 0].pos = segments[i].start;
			vertices[i * 2 + 0].normal = Vector3(0, 1, 0);
			vertices[i * 2 + 1].pos = segments[i].end;
			vertices[i * 2 + 1].normal = Vector3(0, 1, 0);
		}

		auto vbo = std::make_unique<GlBuffer>(vertices.size() * sizeof(Vertex), 0, vertices.data());

		construct_vao(vao, vbo->id());

		return std::move(vbo);		
	}
}

std::unique_ptr<GlBuffer> draw_triangles(Solid* solid, uint32_t tri_vao) {
	std::vector<Triangle> triangles = generate_triangles(solid);
	std::unique_ptr<GlBuffer> tri_vbo = create_triangle_vbo(triangles, tri_vao);

	return std::move(tri_vbo);
}

std::unique_ptr<GlBuffer> draw_wireframes(Solid* solid, uint32_t wireframe_vao) {
	std::vector<Segment> segments = generate_segments(solid);
	std::unique_ptr<GlBuffer> wireframe_vbo = create_wireframe_vbo(segments, wireframe_vao);

	return std::move(wireframe_vbo);
}

}