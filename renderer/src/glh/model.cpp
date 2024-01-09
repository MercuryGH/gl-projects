#include <glh/model.hpp>

#include <unordered_map>
#include <string>

#include <stb_image.h>

#include <cmrc/cmrc.hpp>
CMRC_DECLARE(assets);

#include <util/types.hpp>

namespace renderer {

ObjModel::ObjModel(const char* path) {
    obj_file_path = std::string(path);
    tinyobj::ObjReaderConfig config; // triangulate = true

    auto obj_file = cmrc::assets::get_filesystem().open(path);
    std::vector<uint8_t> obj_file_data(obj_file.size());
    std::copy(obj_file.begin(), obj_file.end(), obj_file_data.data());
    std::string obj_str(reinterpret_cast<char*>(obj_file_data.data()));

    bool flag = obj_reader.ParseFromString(obj_str, "", config); // empty material
    if (flag == false) {
        // error
        printf("tinyobj read error %s\n", obj_reader.Error().c_str());
        assert(false);
    }

    if (obj_reader.Warning().empty() == false) {
        printf("tinyobj read warning %s\n", obj_reader.Warning().c_str());
    }
}

ObjModel::ObjModel(const char* obj_file_path, const char* xml_file_path): ObjModel(obj_file_path) {
    // DEBUG
    printf("path: %s\n", xml_file_path);

    if (xml_doc.LoadFile(xml_file_path) != tinyxml2::XML_SUCCESS) {
        printf("tinyxml2 read error %s\n", xml_doc.ErrorStr());
        assert(false);
    }
}

std::vector<float> ObjModel::read_texture_rgbf(const char* texture_path) {
    // DEBUG 
    printf("path: %s\n", texture_path);

    std::vector<float> data;

    auto file = cmrc::assets::get_filesystem().open(texture_path);
    std::vector<uint8_t> file_data(file.size());
    std::copy(file.begin(), file.end(), file_data.data());

    int width;
    int height;
    int load_channels;
    constexpr int disired_channels = 3;
    float* img_data = stbi_loadf_from_memory(file_data.data(), file_data.size(), &width, &height, &load_channels, disired_channels);

    assert(load_channels == disired_channels);
    data.resize(width * height * load_channels);
    for (int i = 0; i < width * height * load_channels; i++) {
        data.at(i) = img_data[i];
    }

    stbi_image_free(img_data);

    return data;
}

}