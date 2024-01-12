cmake_minimum_required(VERSION 3.22)

# executable

set(TARGET_NAME "pathtracer")
set(CMAKE_CXX_FLAGS "-fopenmp ${CMAKE_CXX_FLAGS}")

file(GLOB_RECURSE PROJECT_SOURCES src/*.cpp include/*.hpp)

add_executable(${TARGET_NAME} ${PROJECT_SOURCES})

target_compile_features(${TARGET_NAME} PRIVATE cxx_std_20)
target_include_directories(${TARGET_NAME} PUBLIC include)

# target_compile_options(${TARGET_NAME} PRIVATE -Wall)

target_link_libraries(${TARGET_NAME} PRIVATE
    renderer
    glad
    glm::glm
    imgui
    stbi
    tinyobjloader
    tinyxml2
)
