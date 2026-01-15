cmake_minimum_required(VERSION 3.21)
project(fge LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external/tinyxml2)
add_subdirectory(external/DirectXTex)

find_package(anari REQUIRED)

add_library(fge STATIC)

target_include_directories(fge PUBLIC include)
target_include_directories(fge PUBLIC external/d3dx12)

target_link_libraries(fge
    PUBLIC
        d3d12
        dxgi
        dxguid
        d3d11
        d2d1
        dwrite
        tinyxml2
        DirectXTex
)

target_link_libraries(fge PRIVATE anari::anari)

#target_compile_features(fge PUBLIC cxx_std_20)

add_subdirectory(src)
add_subdirectory(src/fgewa)
add_subdirectory(shaders)

add_dependencies(fge shaders)

if (NOT DEFINED FGE_BUILD_DEMO)
    option(FGE_BUILD_DEMO "Build FGE sandbox executable" ON)
else()
    option(FGE_BUILD_DEMO "Build FGE sandbox executable" OFF)
endif()

if (FGE_BUILD_DEMO)
    add_executable(fge_example WIN32 main.cpp)
    target_link_libraries(fge_example PRIVATE fge)

    set(SCENE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/scene")
    set(OUTPUT_SCENE_DIR "$<TARGET_FILE_DIR:fge_example>/scene")
    
    set(SHADER_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders/bin")
    set(OUTPUT_SHADER_DIR "$<TARGET_FILE_DIR:fge_example>/shaders/bin")

    add_custom_target(fge_copy_assets ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_SHADER_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${SHADER_BIN_DIR}" "${OUTPUT_SHADER_DIR}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_SCENE_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${SCENE_DIR}" "${OUTPUT_SCENE_DIR}"
        COMMENT "Copying updated assets"
    )

    add_dependencies(fge_copy_assets shaders)
    add_dependencies(fge_example fge_copy_assets)
endif()
