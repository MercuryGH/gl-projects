#pragma once

#include <unordered_map>
#include <graph/solid.hpp>

#include <util/types.hpp>

namespace brep {

class BrepSystemState {
    friend class BrepSystemUI;
    friend class BrepSystem;

private:
    std::unordered_map<uint32_t, Solid*> solids; // <id: solid*> pairs
    Solid* cur_solid{ nullptr };

    bool render_wireframe{ false };

	struct {
		Vector4 color = Vector4((float)0x66 / 0xff, (float)0xcc / 0xff, (float)0xff / 0xff, 1.0f);
	} render_settings;
};

}