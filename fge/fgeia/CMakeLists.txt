
# fgeia
add_library(anari_library_fgeia SHARED)

target_link_libraries(anari_library_fgeia PRIVATE
    anari::anari
    anari::helium
    fge
)

set_target_properties(anari_library_fgeia PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
target_compile_definitions(anari_library_fgeia PRIVATE ANARI_BUILDING_LIBRARY)
set_target_properties(anari_library_fgeia PROPERTIES
    WINDOWS_EXPORT_ALL_SYMBOLS ON
)

target_include_directories(anari_library_fgeia PUBLIC include)
add_subdirectory(src)

option(FGEIA_AUTO_GENERATE_QUERIES "auto-generate the anari device queries" OFF)

if(FGEIA_AUTO_GENERATE_QUERIES)
    anari_generate_queries(
        DEVICE_TARGET anari_library_fgeia
        CPP_NAMESPACE fgeia
        JSON_DEFINITIONS_FILE ${CMAKE_CURRENT_SOURCE_DIR}/fgeia_device.json
        JSON_ROOT_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}
    )
else()
    add_subdirectory(generated)
endif()
