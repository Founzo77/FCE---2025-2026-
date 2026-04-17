
target_sources(anari_library_fgeia PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/anari_library_fgeia_queries.cpp
)

target_include_directories(anari_library_fgeia PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
)
