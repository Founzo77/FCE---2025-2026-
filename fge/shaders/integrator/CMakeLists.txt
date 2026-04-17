
target_sources(depend_shader_raygen INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/raygen_integrator.hlsli
    $<TARGET_PROPERTY:depend_shader_common,INTERFACE_SOURCES>
    $<TARGET_PROPERTY:depend_shader_maths,INTERFACE_SOURCES>
)

target_sources(depend_shader_surface INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/surface_integrator.hlsli
    ${CMAKE_CURRENT_SOURCE_DIR}/../surface/surface.hlsli
    $<TARGET_PROPERTY:depend_shader_common,INTERFACE_SOURCES>
    $<TARGET_PROPERTY:depend_shader_maths,INTERFACE_SOURCES>
)

target_sources(depend_shader_volume INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/volume_integrator.hlsli
    $<TARGET_PROPERTY:depend_shader_common,INTERFACE_SOURCES>
    $<TARGET_PROPERTY:depend_shader_maths,INTERFACE_SOURCES>
)

add_subdirectory(phong)
add_subdirectory(pathtracer)
