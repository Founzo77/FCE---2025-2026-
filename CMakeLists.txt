cmake_minimum_required(VERSION 3.21)
project(fce LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FGE_BUILD_DEMO ON)

add_subdirectory(fge)

add_subdirectory(external/cpp-httplib)
add_subdirectory(external/json)

add_library(fce STATIC)

if(CMAKE_CXX_COMPILER_ID STREQUAL "IntelLLVM")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)

    target_compile_options(fce PRIVATE
        $<$<CONFIG:Release>:
            /O3
            /fp:fast
            /Qipo
            /Qvec
            /QxHost
            /Qunroll:3
            /Qopt-prefetch:3
            /Qopt-zmm-usage:high
            /GR-
            /EHs- /EHa-
            /EHc
            #/Qopt-report:3
            #/Qopt-report-phase:all
        >
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_property(TARGET fce PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
    target_compile_options(fce PRIVATE
        $<$<CONFIG:Release>:
            /O2
            /Ot
            /Ob3
            /Oi
            /GL
            /fp:fast
            /fp:except-
            /Gy /Gw
            /GS-
            /GR-
            /EHa-
            #/Qvec-report:2
        >
    )
endif()

target_include_directories(fce PUBLIC include)

target_link_libraries(fce PUBLIC 
    fge
    httplib::httplib
    nlohmann_json::nlohmann_json)

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

set(AI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ai")
set(OUTPUT_AI_DIR "$<TARGET_FILE_DIR:main>/ai")

add_custom_target(ai_assets ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_AI_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${AI_DIR}" "${OUTPUT_AI_DIR}"
    COMMENT "Copying updated ai assets"
)

add_dependencies(main ai_assets)

