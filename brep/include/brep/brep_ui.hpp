#pragma once

#include <brep/brep_state.hpp>

namespace brep {

class BrepSystemUI {
    friend class BrepSystem;
public:
    BrepSystemUI(BrepSystemState& state);

	void update(float delta_time);
private:
    BrepSystemState& state;

	bool render_settings_dirty{ true };
    bool draw_dirty{ true };

    void draw_ui();
};

}