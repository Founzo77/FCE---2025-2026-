
target_sources(fce PRIVATE
    GameObject.cpp
    ObjectManager.cpp
    GameObjectMemoryManager.cpp
    Component.cpp)

add_subdirectory(scripts)
add_subdirectory(engine_components)
