

add_subdirectory(resources)
add_subdirectory(render)
add_subdirectory(memory)
add_subdirectory(io)

target_sources(fge PRIVATE
    Application.cpp
    MainWindow.cpp)
