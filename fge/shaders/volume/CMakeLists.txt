
compile_shader(${CMAKE_CURRENT_SOURCE_DIR}/volume_intersection.hlsl 
    VolumeIntersection volume_intersection $<TARGET_PROPERTY:depend_shader_volume,INTERFACE_SOURCES>)

compile_shader(${CMAKE_CURRENT_SOURCE_DIR}/dvr_closest_hit.hlsl DvrClosestHit
    dvr_closest_hit $<TARGET_PROPERTY:depend_shader_volume,INTERFACE_SOURCES>)
    
compile_shader(${CMAKE_CURRENT_SOURCE_DIR}/dvr_any_hit.hlsl DvrAnyHit
    dvr_any_hit $<TARGET_PROPERTY:depend_shader_volume,INTERFACE_SOURCES>)

compile_shader(${CMAKE_CURRENT_SOURCE_DIR}/dvr_max_closest_hit.hlsl 
    DvrMaxClosestHit dvr_max_closest_hit $<TARGET_PROPERTY:depend_shader_volume,INTERFACE_SOURCES>)

compile_shader(${CMAKE_CURRENT_SOURCE_DIR}/dvr_min_closest_hit.hlsl 
    DvrMinClosestHit dvr_min_closest_hit $<TARGET_PROPERTY:depend_shader_volume,INTERFACE_SOURCES>)

compile_shader(${CMAKE_CURRENT_SOURCE_DIR}/isosurface_closest_hit.hlsl 
    IsosurfaceClosestHit isosurface_closest_hit $<TARGET_PROPERTY:depend_shader_volume,INTERFACE_SOURCES>)

compile_shader(${CMAKE_CURRENT_SOURCE_DIR}/particle_any_hit.hlsl 
    ParticleAnyHit particle_any_hit $<TARGET_PROPERTY:depend_shader_volume,INTERFACE_SOURCES>)

compile_shader(${CMAKE_CURRENT_SOURCE_DIR}/empty_volume_closest_hit.hlsl 
    EmptyVolumeClosestHit empty_volume_closest_hit $<TARGET_PROPERTY:depend_shader_volume,INTERFACE_SOURCES>)

compile_shader(${CMAKE_CURRENT_SOURCE_DIR}/empty_volume_any_hit.hlsl 
    EmptyVolumeAnyHit empty_volume_any_hit $<TARGET_PROPERTY:depend_shader_volume,INTERFACE_SOURCES>)