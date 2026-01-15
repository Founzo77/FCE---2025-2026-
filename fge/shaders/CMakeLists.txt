
find_program(DXC_COMPILER NAMES dxc HINTS "$ENV{DXC_PATH}")
if(NOT DXC_COMPILER)
    message(FATAL_ERROR "dxc.exe not found! Set DXC_PATH or add it to PATH.")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(SHADER_FLAGS -Od -Zi)
else()
    set(SHADER_FLAGS -O3 -Qstrip_debug -Qstrip_reflect -HV 2021 -enable-16bit-types -all_resources_bound)
endif()

set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SHADER_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin)

set(ALL_SHADER_OUTPUTS "")
set(SHADER_COMMON_DEPS ${SHADER_DIR}/basic/common.hlsli)

function(compile_shader SHADER_FILE ENTRY_NAME OUTPUT_NAME)
    get_filename_component(FILE_NAME ${SHADER_FILE} NAME_WE)
    set(OUT_FILE ${SHADER_OUTPUT_DIR}/${OUTPUT_NAME}.dxil)

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

    # Ajouter le fichier généré dans la liste globale
    list(APPEND ALL_SHADER_OUTPUTS ${OUT_FILE})
    set(ALL_SHADER_OUTPUTS "${ALL_SHADER_OUTPUTS}" PARENT_SCOPE)
endfunction()

# === Liste des shaders à compiler ===
compile_shader(${SHADER_DIR}/basic/raygen.hlsl RayGenShader raygen)
compile_shader(${SHADER_DIR}/basic/miss.hlsl MissShader miss)
compile_shader(${SHADER_DIR}/basic/closesthit.hlsl ClosestHitShader closesthit)

# Cible qui regroupe tous les shaders
add_custom_target(shaders ALL DEPENDS ${ALL_SHADER_OUTPUTS})
