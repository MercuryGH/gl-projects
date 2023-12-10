cmake_minimum_required(VERSION 3.22)

# thirdparty

function(add_thirdparties)
    # built at rasterize project
    # add_library(json INTERFACE)
    # target_include_directories(json INTERFACE thirdparty/json/include)
    # add_subdirectory(thirdparty/tinyobjloader)

    set(TINYEXR_BUILD_SAMPLE OFF)
    add_subdirectory(thirdparty/tinyexr)

    set(BUILD_TESTING OFF)
    add_subdirectory(thirdparty/tinyxml2)
endfunction()

add_thirdparties()

# executable

set(TARGET_NAME "pathtracer")
set(CMAKE_CXX_FLAGS "-fopenmp ${CMAKE_CXX_FLAGS}")

file(GLOB_RECURSE PROJECT_SOURCES src/*.cpp include/*.hpp)

add_executable(${TARGET_NAME} ${PROJECT_SOURCES})

target_compile_features(${TARGET_NAME} PRIVATE cxx_std_20)
target_include_directories(${TARGET_NAME} PUBLIC include)

target_link_libraries(${TARGET_NAME} PRIVATE 
    renderer
    glad 
    glm::glm 
    imgui 
    stbi 
    # json
    tinyobjloader
    tinyexr
    tinyxml2::tinyxml2
)