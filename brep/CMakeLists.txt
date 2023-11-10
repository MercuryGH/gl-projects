cmake_minimum_required(VERSION 3.22)

# thirdparty

function(add_thirdparties)
    add_subdirectory(thirdparty/poly2tri)
endfunction()

add_thirdparties()

# executable

set(TARGET_NAME "brep")

file(GLOB_RECURSE PROJECT_SOURCES src/*.cpp include/*.hpp)

add_executable(${TARGET_NAME} ${PROJECT_SOURCES})

target_compile_features(${TARGET_NAME} PRIVATE cxx_std_20)
target_include_directories(${TARGET_NAME} PUBLIC include)

target_link_libraries(${TARGET_NAME} PRIVATE 
    renderer
    poly2tri
    glad 
    glm::glm 
    imgui 
    stbi 
)