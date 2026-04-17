
target_sources(depend_shader_common INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/global_signature.hlsli
    ${CMAKE_CURRENT_SOURCE_DIR}/data.hlsli
)

target_sources(depend_shader_maths INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/maths.hlsli
    ${CMAKE_CURRENT_SOURCE_DIR}/data.hlsli
)

target_sources(depend_shader_surface INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/surfacique_local_signature.hlsli
)

target_sources(depend_shader_volume INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/volumique_local_signature.hlsli
)
