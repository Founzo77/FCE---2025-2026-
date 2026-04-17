
target_sources(depend_shader_raygen INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/raygen_phong_integrator.hlsli
)

target_sources(depend_shader_surface INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/surface_phong_integrator.hlsli
)

target_sources(depend_shader_volume INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/volume_phong_integrator.hlsli
)
