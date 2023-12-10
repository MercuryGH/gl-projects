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

    void draw_ui();
};

}