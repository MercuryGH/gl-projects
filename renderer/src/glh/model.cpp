#include <glh/model.hpp>

#include <cmrc/cmrc.hpp>
CMRC_DECLARE(assets);

namespace renderer {

ObjModel::ObjModel(const char* path) {
    tinyobj::ObjReader obj_reader;
    tinyobj::ObjReaderConfig config; // triangulate = true

    auto obj_file = cmrc::assets::get_filesystem().open(path);
    std::vector<uint8_t> obj_file_data(obj_file.size());
    std::copy(obj_file.begin(), obj_file.end(), obj_file_data.data());
    std::string obj_str(reinterpret_cast<char*>(obj_file_data.data()));

    bool flag = obj_reader.ParseFromString(obj_str, "", config); // empty material
    if (flag == false) {
        // error
        printf("tinyobj read error %s\n", obj_reader.Error().c_str());
    }

    if (obj_reader.Warning().empty() == false) {
        printf("tinyobj read warning %s\n", obj_reader.Warning().c_str());
    }

    shapes = obj_reader.GetShapes();
    attrib = obj_reader.GetAttrib();
}

}