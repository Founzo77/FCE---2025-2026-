set(DXC_PATH "" CACHE PATH "Path to DXC bin folder")
find_program(DXC_COMPILER NAMES dxc HINTS "${DXC_PATH}" "$ENV{DXC_PATH}")
if(NOT DXC_COMPILER)
    message(FATAL_ERROR "dxc.exe not found! Set DXC_PATH or add it to PATH.")
endif()
message(STATUS "DXC EXECUTABLE USED: ${DXC_COMPILER}")

set(SHADER_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(SHADER_FLAGS -Od -Zi)
else()
    set(SHADER_FLAGS -O3 -Qstrip_debug -Qstrip_reflect -HV 2021 -enable-16bit-types -all_resources_bound)
endif()

set_property(GLOBAL PROPERTY ALL_SHADER_TARGETS "")

function(compile_shader SHADER_FILE ENTRY_NAME OUTPUT_NAME)
    set(SHADER_COMMON_DEPS ${ARGN})

    get_filename_component(FILE_NAME ${SHADER_FILE} NAME_WE)

    set(OUT_FILE ${SHADER_OUTPUT_DIR}/${OUTPUT_NAME}.dxil)
    set(SHADER_TARGET shader_${OUTPUT_NAME})

    add_custom_command(
        OUTPUT ${OUT_FILE}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_OUTPUT_DIR}
        COMMAND ${DXC_COMPILER}
            -T lib_6_5
            -exports ${ENTRY_NAME}
            ${SHADER_FLAGS}
            -Fo ${OUT_FILE}
            ${SHADER_FILE}
        DEPENDS ${SHADER_FILE} ${SHADER_COMMON_DEPS}
        COMMENT "Compiling HLSL Shader: ${FILE_NAME}"
        VERBATIM
    )

    add_custom_target(${SHADER_TARGET} DEPENDS ${OUT_FILE})
    set_property(GLOBAL APPEND PROPERTY ALL_SHADER_TARGETS ${SHADER_TARGET})
endfunction()

add_library(depend_shader_common INTERFACE)
add_library(depend_shader_maths INTERFACE)

add_library(depend_shader_raygen INTERFACE)
add_library(depend_shader_miss INTERFACE)
add_library(depend_shader_surface INTERFACE)
add_library(depend_shader_volume INTERFACE)

add_subdirectory(common)
add_subdirectory(integrator)
add_subdirectory(basic)
add_subdirectory(surface)
add_subdirectory(volume)

get_property(ALL_SHADER_TARGETS GLOBAL PROPERTY ALL_SHADER_TARGETS)
message(STATUS "Shaders targets: ${ALL_SHADER_TARGETS}")
add_custom_target(shaders ALL DEPENDS ${ALL_SHADER_TARGETS})
