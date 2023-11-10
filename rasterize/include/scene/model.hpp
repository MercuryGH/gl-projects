#pragma once

#include <filesystem>

#include <util/types.hpp>
#include <glh/resource.hpp>

#include <scene/bounding_box.hpp>

namespace rasterize {
    using namespace renderer;

// class Model {
// public:
//     Model(const std::filesystem::path &file_path);

//     // getters

//     size_t n_verts() const { return pos.size(); }
//     const std::vector<glm::vec3> &positions() const { return pos; }
//     const std::vector<glm::vec3> &normals() const { return nor; }
//     size_t n_indices() const { return ind.size(); }
//     const std::vector<uint32_t> &indices() const { return ind; }

//     const BoundingBox &bounding_box() const { return bbox; }

//     const GlBuffer *position_buffer() const { return position_buf.get(); }
//     const GlBuffer *normal_buffer() const { return normal_buf.get(); }
//     const GlBuffer *index_buffer() const { return index_buf.get(); }

// private:
//     std::vector<glm::vec3> pos;
//     std::vector<glm::vec3> nor;
//     std::vector<uint32_t> ind;
//     BoundingBox bbox;

//     std::unique_ptr<GlBuffer> position_buf;
//     std::unique_ptr<GlBuffer> normal_buf;
//     std::unique_ptr<GlBuffer> index_buf;
// };

}