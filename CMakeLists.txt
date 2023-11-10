cmake_minimum_required(VERSION 3.22)

cmake_policy(SET CMP0077 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

project(gl-template)

# thirdparties

function(add_thirdparties)
    add_subdirectory(thirdparty/cmrc)
endfunction()

add_thirdparties()

# assets

file(GLOB_RECURSE ASSET_FILES ${CMAKE_SOURCE_DIR}/assets/*)
cmrc_add_resource_library(assets ${ASSET_FILES})

# shaders

file(GLOB_RECURSE SHADERS_SOURCES ${CMAKE_SOURCE_DIR}/shaders/*.comp ${CMAKE_SOURCE_DIR}/shaders/*.vert ${CMAKE_SOURCE_DIR}/shaders/*.frag)
set(SHADERS_SPV "")
foreach(SHADER ${SHADERS_SOURCES})
    # SHADER_REL = current shader relative path (use in SHADER_OUT)
    file(RELATIVE_PATH SHADER_REL ${CMAKE_SOURCE_DIR}/shaders ${SHADER})

    set(SHADER_OUT ${CMAKE_SOURCE_DIR}/build-shaders/${SHADER_REL}.spv)
    add_custom_command(
        OUTPUT ${SHADER_OUT}
        DEPENDS ${SHADER}

        COMMAND glslangValidator --target-env opengl -o ${SHADER_OUT} ${SHADER}
    )
    list(APPEND SHADERS_SPV ${SHADER_OUT})
endforeach()
add_custom_target(build_shaders DEPENDS ${SHADERS_SPV})

cmrc_add_resource_library(shaders_spv ${SHADERS_SPV} WHENCE ${CMAKE_SOURCE_DIR}/build-shaders)
add_dependencies(shaders_spv build_shaders)

# project dirs

add_subdirectory(renderer)
add_subdirectory(brep)
# add_subdirectory(rasterize)

