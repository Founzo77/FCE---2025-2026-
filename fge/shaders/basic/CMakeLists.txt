
compile_shader(${CMAKE_CURRENT_SOURCE_DIR}/raygen.hlsl RayGenShader raygen 
    $<TARGET_PROPERTY:depend_shader_common,INTERFACE_SOURCES>
    $<TARGET_PROPERTY:depend_shader_raygen,INTERFACE_SOURCES>)
    
compile_shader(${CMAKE_CURRENT_SOURCE_DIR}/miss.hlsl MissShader miss 
    $<TARGET_PROPERTY:depend_shader_common,INTERFACE_SOURCES>
    $<TARGET_PROPERTY:depend_shader_maths,INTERFACE_SOURCES>)