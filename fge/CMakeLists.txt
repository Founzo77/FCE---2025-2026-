cmake_minimum_required(VERSION 3.21)
project(fge LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DNOMINMAX)

add_subdirectory(external/tinyxml2)
add_subdirectory(external/DirectXTex)

add_library(fge STATIC)

if(CMAKE_CXX_COMPILER_ID STREQUAL "IntelLLVM")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)

    target_compile_options(fge PRIVATE
        $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Release>>:
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
    set_property(TARGET fge PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
    target_compile_options(fge PRIVATE
        $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Release>>:
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

target_compile_options(fge PRIVATE
    $<$<AND:$<COMPILE_LANGUAGE:CUDA>,$<CONFIG:Release>>:
        -O3
        --use_fast_math
    >
)

target_include_directories(fge PUBLIC include)
target_include_directories(fge PUBLIC external/d3dx12)

# ImGui

add_library(imgui STATIC
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/imgui_demo.cpp

    external/imgui/backends/imgui_impl_dx12.cpp
    external/imgui/backends/imgui_impl_win32.cpp
)

target_include_directories(imgui PUBLIC
    external/imgui
    external/imgui/backends
)

target_link_libraries(imgui PUBLIC
    d3d12
    dxgi
    d3d11
)

find_package(OpenMP REQUIRED)
find_package(TBB CONFIG REQUIRED)

target_link_libraries(fge PUBLIC
    d3d12
    dxgi
    dxguid
    d3d11
    d2d1
    dwrite
    tinyxml2
    DirectXTex
    imgui
    OpenMP::OpenMP_CXX
    TBB::tbb
)

#find_package(anari REQUIRED)
find_package(anari 0.15 REQUIRED COMPONENTS code_gen)

target_link_libraries(fge PUBLIC anari::anari)

# TO_DO if cuda
find_package(CUDAToolkit REQUIRED)

target_link_libraries(fge PUBLIC
    CUDA::cudart
    CUDA::cuda_driver
)

target_include_directories(fge PUBLIC
    ${CUDAToolkit_INCLUDE_DIRS}
)

# TO_DO OpenXR
find_package(OpenXR REQUIRED)

target_link_libraries(fge PUBLIC
    OpenXR::openxr_loader
)

add_subdirectory(src)
add_subdirectory(src/fgewa)
add_subdirectory(shaders)

add_dependencies(fge shaders)

add_subdirectory(fgeia)

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
