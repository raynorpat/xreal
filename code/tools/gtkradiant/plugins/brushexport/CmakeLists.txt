radiant_plugin(brushexport
    callbacks.cpp
    callbacks.h
    export.cpp
    export.h
    interface.cpp
    plugin.cpp
    plugin.h
    support.cpp
    support.h
)

find_package(GTK2 REQUIRED)
find_package(GLIB REQUIRED)
include_directories(${GLIB2_INCLUDE_DIRS} ${GTK2_INCLUDE_DIRS} ${GTK2_GDK_INCLUDE_DIR} ${GTK2_GLIB_INCLUDE_DIR})
target_link_libraries(brushexport ${GLIB2_LIBRARIES} ${GTK2_LIBRARIES})
