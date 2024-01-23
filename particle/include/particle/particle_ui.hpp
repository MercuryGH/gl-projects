#pragma once

#include <particle/particle_state.hpp>

namespace particle {

class ParticleSystemUI {
    friend class ParticleSystem;
public:
    ParticleSystemUI(ParticleSystemState& state);

	void update(float delta_time);
private:
    ParticleSystemState& state;

    void draw_ui();

	bool executing = true;
	bool emit_settings_dirty = true;
	bool render_settings_dirty = true;
};

}