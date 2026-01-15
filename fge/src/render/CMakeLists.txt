
add_subdirectory(data)
add_subdirectory(memory)
add_subdirectory(pipeline_state)

target_sources(fge PRIVATE
    utility.cpp
    CommandQueue.cpp
    FrameContext.cpp
    MainRenderer.cpp
    Scene.cpp
    WriterFrame.cpp)
