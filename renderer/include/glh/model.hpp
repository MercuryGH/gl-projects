#pragma once

#include <tiny_obj_loader.h>
#include <tinyxml2.h>

namespace renderer {

// simple obj model interface wrapped for tiny obj loader
class ObjModel {
public:
    ObjModel(const char* path, bool read_from_cache=false);
    ObjModel(const char* obj_file_path, const char* mtl_file_path, const char* xml_file_path, bool read_from_cache=false);

    static std::tuple<std::vector<float>, int, int> read_texture_rgbf(const char* texture_path, bool read_from_cache=false);

    const std::vector<tinyobj::shape_t>& shapes() const { 
        return obj_reader.GetShapes();
    }

    const tinyobj::attrib_t& attrib() const { 
        return obj_reader.GetAttrib();
    }

    const std::vector<tinyobj::material_t>& materials() const {
        return obj_reader.GetMaterials();
    }

    const tinyxml2::XMLDocument& xml_document() const {
        return xml_doc;
    }

    std::string get_obj_file_path() const {
        return obj_file_path;
    }

private:
    std::string obj_file_path;

    tinyobj::ObjReader obj_reader;
    tinyxml2::XMLDocument xml_doc;
};

}