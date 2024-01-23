## Particle System

There are 4 main parts in the particle system:

* emit - Particles will be emitted every `emit_interval` frames. Each new particle has an random initial position and velocity, and the initial accelerator is zero. See `emit.comp`.
* update - Update particles using Verlet method. A force input from UI, gravity and drag force are considered. See `update.comp`.
* compact - Compact array of particles due to dead particles every `compact_interval` frames. A two-level scan on GPU is performed to compute the new indices in the array for each particle (see `scan1.comp`, `scan2.comp` and `scan3.comp`), and then living particles are copied to the new position (see `compact.comp`).
* draw - Render each particle as a billboard using instanced draw call. See `draw.vert` and `draw.frag`.

![](./pic/readme.jpg)
