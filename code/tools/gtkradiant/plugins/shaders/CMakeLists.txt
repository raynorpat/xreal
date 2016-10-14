radiant_plugin(shaders
    plugin.cpp
    plugin.h
    shaders.cpp
    shaders.h
)

find_package(GLIB REQUIRED)
include_directories(${GLIB2_INCLUDE_DIRS})
target_link_libraries(shaders ${GLIB2_LIBRARIES})