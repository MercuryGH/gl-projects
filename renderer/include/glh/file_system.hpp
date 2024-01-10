#pragma once

#include <nfd.h>

namespace renderer {
    /**
     * simple function opening a single file
    */
    const char* open_file() {
        nfdchar_t* out_path = nullptr;
        auto res_code = NFD_OpenDialog(nullptr, nullptr, &out_path);
        if (res_code == NFD_OKAY) {
            return out_path;
        } else if (res_code == NFD_CANCEL) {
            return nullptr;
        } 

        printf("Error while opening file\n");
        assert(false);
        return nullptr;
    }

    /**
     * single directory
    */
    const char* select_dir() {
        nfdchar_t* out_path = nullptr;
        auto res_code = NFD_PickFolder(nullptr, &out_path);
        if (res_code == NFD_OKAY) {
            return out_path;
        } else if (res_code == NFD_CANCEL) {
            return nullptr;
        } 

        printf("Error while opening directory\n");
        assert(false);
        return nullptr;        
    }
}