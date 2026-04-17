
compile_shader(${CMAKE_CURRENT_SOURCE_DIR}/phong_closest_hit.hlsl PhongClosestHit phong_closest_hit 
    $<TARGET_PROPERTY:depend_shader_surface,INTERFACE_SOURCES>
    $<TARGET_PROPERTY:depend_shader_maths,INTERFACE_SOURCES>)

compile_shader(${CMAKE_CURRENT_SOURCE_DIR}/old_phong_closest_hit.hlsl OldPhongClosestHit old_phong_closest_hit 
    $<TARGET_PROPERTY:depend_shader_surface,INTERFACE_SOURCES>
    $<TARGET_PROPERTY:depend_shader_maths,INTERFACE_SOURCES>)

compile_shader(${CMAKE_CURRENT_SOURCE_DIR}/albedo_closest_hit.hlsl AlbedoClosestHit albedo_closest_hit 
    $<TARGET_PROPERTY:depend_shader_surface,INTERFACE_SOURCES>
    $<TARGET_PROPERTY:depend_shader_maths,INTERFACE_SOURCES>)

compile_shader(${CMAKE_CURRENT_SOURCE_DIR}/diffuse_closest_hit.hlsl DiffuseClosestHit diffuse_closest_hit 
    $<TARGET_PROPERTY:depend_shader_surface,INTERFACE_SOURCES>
    $<TARGET_PROPERTY:depend_shader_maths,INTERFACE_SOURCES>)

compile_shader(${CMAKE_CURRENT_SOURCE_DIR}/normal_closest_hit.hlsl NormalClosestHit normal_closest_hit 
    $<TARGET_PROPERTY:depend_shader_surface,INTERFACE_SOURCES>
    $<TARGET_PROPERTY:depend_shader_maths,INTERFACE_SOURCES>)