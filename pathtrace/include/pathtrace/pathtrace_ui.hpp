#pragma once

#include <pathtrace/pathtrace_state.hpp>

namespace pathtrace {

class PathtraceSystemUI {
    friend class PathtraceSystem;
public:
    PathtraceSystemUI(PathtraceSystemState& state);

	void update(float delta_time);
private:
    PathtraceSystemState& state;

    bool import_scene_from_disk{ false };
    bool draw_dirty{ true };
    bool scene_dirty{ true };
    bool camera_dirty{ true };
    bool render_settings_dirty{ true };

    void draw_ui();
};

}
