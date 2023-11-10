#pragma once

#include <rasterize/rasterize_state.hpp>

namespace rasterize {

class RasterizeSystemUI {
    friend class RasterizeSystem;
public:
    RasterizeSystemUI(RasterizeSystemState& state);

	void update(float delta_time);
private:
    RasterizeSystemState& state;

    bool draw_dirty{ true };
    bool model_dirty{ true };

    void draw_ui();
};

}