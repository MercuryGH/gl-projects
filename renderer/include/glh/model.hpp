#pragma once

#include <tiny_obj_loader.h>

namespace renderer {

// simple obj model interface for tiny obj loader
struct ObjModel {
    ObjModel(const char* path);

    std::vector<tinyobj::shape_t> shapes;
    tinyobj::attrib_t attrib;
};

}