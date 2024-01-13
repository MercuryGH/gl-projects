#include <glh/model.hpp>

#include <unordered_map>
#include <string>

#include <stb_image.h>

#include <cmrc/cmrc.hpp>
CMRC_DECLARE(cached_assets);

#include <util/types.hpp>
#include <glh/pipeline.hpp>

namespace renderer {

ObjModel::ObjModel(const char* obj_file_path, bool read_from_cache): obj_file_path(obj_file_path) {
    tinyobj::ObjReaderConfig config; // triangulate = true

    bool flag = false;
    if (read_from_cache) {
        auto obj_file_data = get_cached_file_data(obj_file_path);
        std::string obj_str(reinterpret_cast<char*>(obj_file_data.data()));

        flag = obj_reader.ParseFromString(obj_str, "", config); // empty material
    } else {
        // read from disk
        flag = obj_reader.ParseFromFile(obj_file_path, config);
    }

    if (flag == false) {
        // error
        printf("tinyobj read error %s\n", obj_reader.Error().c_str());
        assert(false);
    }

    if (obj_reader.Warning().empty() == false) {
        printf("tinyobj read warning %s\n", obj_reader.Warning().c_str());
    }
}

ObjModel::ObjModel(const char* obj_file_path, const char* mtl_file_path, const char* xml_file_path, bool read_from_cache): obj_file_path(obj_file_path) {
    tinyobj::ObjReaderConfig config; // triangulate = true

    bool obj_mtl_flag = false;
    if (read_from_cache) {
        auto obj_file_data = get_cached_file_data(obj_file_path);
        std::string obj_str(reinterpret_cast<char*>(obj_file_data.data()));
        auto mtl_file_data = get_cached_file_data(mtl_file_path);
        std::string mtl_str(reinterpret_cast<char*>(mtl_file_data.data()));
        obj_mtl_flag = obj_reader.ParseFromString(obj_str, mtl_str, config); // empty material
    } else {
        // obj reader finds the mtl file automatically
        obj_mtl_flag = obj_reader.ParseFromFile(obj_file_path, config);
    }

    if (obj_mtl_flag == false) {
        // error
        printf("tinyobj read error %s\n", obj_reader.Error().c_str());
        assert(false);
    }
    if (obj_reader.Warning().empty() == false) {
        printf("tinyobj read warning %s\n", obj_reader.Warning().c_str());
    }

    tinyxml2::XMLError xml_flag;
    if (read_from_cache) {
        auto xml_file_data = get_cached_file_data(xml_file_path);
        xml_flag = xml_doc.Parse(reinterpret_cast<char*>(xml_file_data.data()), xml_file_data.size());
    } else {
        xml_flag = xml_doc.LoadFile(xml_file_path);
    }

    if (xml_flag != tinyxml2::XML_SUCCESS) {
        printf("tinyxml2 read error %s\n", xml_doc.ErrorStr());
        assert(false);
    }
}

std::tuple<std::vector<float>, int, int> ObjModel::read_texture_rgbf(const char* texture_path, bool read_from_cache) {
    // DEBUG 
    printf("Loading texture: %s\n", texture_path);

    std::vector<float> data;
    int width;
    int height;
    int load_channels;
    constexpr int disired_channels = 3;
    float* img_data;
    if (read_from_cache) {
        std::vector<uint8_t> file_data = get_cached_file_data(texture_path);
        img_data = stbi_loadf_from_memory(file_data.data(), file_data.size(), &width, &height, &load_channels, disired_channels);
    } else {
        img_data = stbi_loadf(texture_path, &width, &height, &load_channels, disired_channels);
    }

    if (load_channels > disired_channels) {
        printf("Warning: texture channels = %d > 3, use RGB channle only.\n", load_channels);
    } else if (load_channels < disired_channels) {
        printf("Warning: texture channels = %d < 3, not supported yet.\n", load_channels);
    }

    data.resize(width * height * disired_channels);
    for (int i = 0; i < width * height * disired_channels; i++) {
        data.at(i) = img_data[i];
    }

    stbi_image_free(img_data);

    return { data, width, height };
}

}