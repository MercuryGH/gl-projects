#pragma once

#include <nurbs/nurbs_state.hpp>

namespace nurbs {

class NurbsSystemUI {
    friend class NurbsSystem;
public:
    NurbsSystemUI(NurbsSystemState& state);

	void update(float delta_time);
private:
    NurbsSystemState& state;

    void draw_ui();
};

}