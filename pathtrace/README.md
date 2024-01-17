# Monte Carlo Path Tracer

## Description

This is a toy renderer using monte carlo path tracing.

* Multiple Importance Sampling is implemented, to render `veach-mis` scene efficiently.
* Support phong material, serve glass material as a special case.
* Support triangular area lights.
* Support triangular meshes.
* Use a simple BVH to accelerate ray-object intersection query.
* Pixel sampling is parallel using CPU multithreading.

## Build

* CMake >= 3.22
* C++20
* OpenGL 4.6
* `glslangValidator` in PATH

OpenGL 4.6 is required, which is not supported in MacOS. Windows (msvc) and Linux (gcc) build are passed.

### Dependency

My `renderer`, and thirdparties:

* [glad](https://github.com/Dav1dde/glad)
* [glm](https://github.com/g-truc/glm)
* [Dear ImGui](https://github.com/ocornut/imgui)
* [stb image](https://github.com/nothings/stb)

All thirdpartiy sources are included in `thirdparty/`, `renderer/thirdparty/`, which will be built by CMake commands.

## User Interface

1. Select a cached scene, or select a folder to load the model.
   * Scene `classic-cornell-box` and `cornell-box` are cached, i.e., load from memory. Scene `veach-mis`, `fruit` and `moss` can be loaded from the disk. They are located in the folder `assets/scene`.
2. Click `Load scene` button to load the scene. Then the number of triangles of the model is shown.
3. You can adjust the camera parameters in the GUI. Default camera parameters is setup when the scene is loaded.
4. Select the number of sampling per pixel (spp), and click the `Render` button to wait for a result. If `update result` is checked, the result of each sample is update real-time to the screen.
5. There is also a screenshot button on the GUI.

## Screenshots and Results

* UI and operation screenshot (GIF)

![](./pic/gui.gif)

* Classic cornell box (500 spp)

![](./pic/cornell-box-500spp.png)

* Glass material cornell box (1000 spp)

![](./pic/glass_cornell_box_1000spp.png)

* Cornell box with suzanne and bunny (1000 spp)

![](./pic/cornell-box-suzanne-bunny-1000spp.png)

We can see the noisy horizontal black line artifacts in the rendering result, which is caused by the floating point numerical errors accumulated, since the scene is large in scale, compared with smaller cornell box scene.

* Veach-MIS (1000 spp)

![](./pic/veach-mis-1000spp.png)

2 sampling methods, light and BRDF are verified.

* Fruit (250 spp)

This scene is provided from a senior student in the university.

![](./pic/fruit-250spp.png)

Weird triangle area artifacts can be seen in the rendering result, which have a deeper color compared with their neighbors. When adjusting the `epsilon` (i.e., `k_small_eps` in the code) in ray-triangle intersection algorithm, the smaller the `epsilon` is, the obvious the artifacts become. However, if we set the `epsilon` too large, i.e. `1e-4`, some small triangles will fail in the ray intersection test, thus won't appear in the rendering result, and the result will become totally wrong. This artifact is still caused by insufficient floating point numerical precision.

* MOSS (in the film *The Wandering Earth 2*)

The model in this scene is from https://www.aplaybox.com/details/model/aS63d8LVU8U5

Red light Off (750 spp):

![](./pic/550w-off-750spp.png)

Red light On (500 spp):

![](./pic/550w-on-1000spp.png)

* Bathroom (oops, rendering failed)

This toy renderer cannot render the scene `bathroom` in a reasonable time.

The scene is so complicated that every sample process consumes large time in ray-object intersection query, causing the renderer almost stucks in such a situation. Also, the scene is full of indirect illumination, that the process for a surface point to sample light is difficult, requires multiple bounces in various locations.

To improve this, first, a more efficient BVH should be designed, to allow the renderer to continue path tracing. Second, a bidirectional path tracing scheme should be implmented, and a GPU accleration may be necessary.

Here is a 60 * 60 rendering result (100 spp)

![](./pic/badroom-100spp.png)

## Conclusion

This renderer is not prepared for large and complicated scene rendering, but is tested correct in the above scene results, except for artifacts caused by floating point numerical error.

There are many spaces for improvement and optimization, though it should be OK to be a homework of a lesson.
