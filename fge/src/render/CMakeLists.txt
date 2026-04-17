
add_subdirectory(data)
add_subdirectory(memory)
add_subdirectory(pipeline_state)
add_subdirectory(imgui)

target_sources(fge PRIVATE
    utility.cpp
    CommandQueue.cpp
    FrameContext.cpp
    MainRenderer.cpp
    Scene.cpp
    RenderEngine.cpp
    SwapChainPresenter.cpp
    FrameComposer.cpp
    FramePresentationContext.cpp
    FrameExecutionContext.cpp)
