cmake_minimum_required(VERSION 3.22)

# thirdparties

function(add_thirdparties)
    set(GLFW_BUILD_EXAMPLES OFF)
    set(GLFW_BUILD_TESTS OFF)
    set(GLFW_BUILD_DOCS OFF)
    set(GLFW_INSTALL OFF)
    add_subdirectory(thirdparty/glfw)

    add_library(glad thirdparty/glad/src/glad.c)
    target_include_directories(glad PUBLIC thirdparty/glad/include)

    add_subdirectory(thirdparty/glm)

    file(GLOB IMGUI_SOURCES thirdparty/imgui/*.cpp)
    add_library(imgui ${IMGUI_SOURCES})
    target_include_directories(imgui PUBLIC thirdparty/imgui)
    target_link_libraries(imgui PUBLIC glfw)

    add_library(stbi INTERFACE)
    target_include_directories(stbi INTERFACE thirdparty/stb)

    add_subdirectory(thirdparty/tinyobjloader)
endfunction()

add_thirdparties()

# executable

set(TARGET_NAME "renderer")

file(GLOB_RECURSE PROJECT_SOURCES src/*.cpp include/*.hpp)

# add_executable(${TARGET_NAME} ${PROJECT_SOURCES})
# add_library(${TARGET_NAME} SHARED ${PROJECT_SOURCES})
add_library(${TARGET_NAME} STATIC ${PROJECT_SOURCES})

target_compile_features(${TARGET_NAME} PUBLIC cxx_std_20)
target_include_directories(${TARGET_NAME} PUBLIC include)

target_link_libraries(${TARGET_NAME} PUBLIC
    glfw
    glad
    glm::glm
    imgui
    stbi
    tinyobjloader
    assets shaders_spv
)
