cmake_minimum_required(VERSION 3.21)
project(fce LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FGE_BUILD_DEMO ON)
add_subdirectory(fge)

add_library(fce STATIC)

target_include_directories(fce PUBLIC include)

target_link_libraries(fce PUBLIC fge)

add_subdirectory(src)

add_executable(main WIN32 application/main.cpp)
target_link_libraries(main PRIVATE fce)

set(SCENE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/application/scene")
set(OUTPUT_SCENE_DIR "$<TARGET_FILE_DIR:main>/scene")
set(FGE_SHADER_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/fge/shaders/bin")
set(OUTPUT_SHADER_DIR "$<TARGET_FILE_DIR:main>/shaders/bin")

add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_SCENE_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${SCENE_DIR}" "${OUTPUT_SCENE_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_SHADER_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${FGE_SHADER_BIN_DIR}" "${OUTPUT_SHADER_DIR}"
    COMMENT "Copying updated assets"
)

add_dependencies(main copy_assets)
