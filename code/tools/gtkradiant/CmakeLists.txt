project(GtkRadiant C CXX)

cmake_minimum_required(VERSION 2.8)

if(CMAKE_EXECUTABLE_SUFFIX)
    set(RADIANT_EXECUTABLE ${CMAKE_EXECUTABLE_SUFFIX})
else()
    execute_process(COMMAND uname -m
        OUTPUT_VARIABLE RADIANT_EXECUTABLE OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# Enable Debug by default, can be changed with -D CMAKE_BUILD_TYPE=Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-D_DEBUG=1)
endif()

set(CMAKE_FIND_FRAMEWORK NEVER)
find_package(OpenGL REQUIRED)

add_definitions(-DRADIANT_EXECUTABLE="${RADIANT_EXECUTABLE}" ${GLIB2_DEFINITIONS})

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${GLIB2_INCLUDE_DIRS}
)

if(NOT WIN32)
    add_definitions(
        -DPOSIX=1
        -DXWINDOWS=1
    )
else()
    add_definitions(
        -D_WIN32
    )
endif()

set(INCLUDELIST
    include/aboutmsg.h
    include/cullable.cpp
    include/cullable.h
    include/editable.cpp
    include/editable.h
    include/iarchive.cpp
    include/iarchive.h
    include/ibrush.cpp
    include/ibrush.h
    include/icamera.cpp
    include/icamera.h
    include/idatastream.cpp
    include/idatastream.h
    include/ieclass.cpp
    include/ieclass.h
    include/ientity.cpp
    include/ientity.h
    include/ifilesystem.cpp
    include/ifilesystem.h
    include/ifiletypes.cpp
    include/ifiletypes.h
    include/ifilter.cpp
    include/ifilter.h
    include/igl.cpp
    include/igl.h
    include/iglrender.cpp
    include/iglrender.h
    include/igtkgl.cpp
    include/igtkgl.h
    include/iimage.cpp
    include/iimage.h
    include/imap.cpp
    include/imap.h
    include/imodel.cpp
    include/imodel.h
    include/ipatch.cpp
    include/ipatch.h
    include/iplugin.cpp
    include/iplugin.h
    include/ireference.cpp
    include/ireference.h
    include/irender.cpp
    include/irender.h
    include/iscenegraph.cpp
    include/iscenegraph.h
    include/iscriplib.cpp
    include/iscriplib.h
    include/iselection.cpp
    include/iselection.h
    include/ishaders.cpp
    include/ishaders.h
    include/itexdef.cpp
    include/itexdef.h
    include/itextstream.cpp
    include/itextstream.h
    include/itextures.cpp
    include/itextures.h
    include/itoolbar.cpp
    include/itoolbar.h
    include/iundo.cpp
    include/iundo.h
    include/mapfile.cpp
    include/mapfile.h
    include/modelskin.cpp
    include/modelskin.h
    include/moduleobserver.cpp
    include/moduleobserver.h
    include/modulesystem.cpp
    include/modulesystem.h
    include/nameable.cpp
    include/nameable.h
    include/namespace.cpp
    include/namespace.h
    include/preferencesystem.cpp
    include/preferencesystem.h
    include/qerplugin.cpp
    include/qerplugin.h
    include/renderable.cpp
    include/renderable.h
    include/selectable.cpp
    include/selectable.h
    include/stream_version.h
    include/version.h
    include/warnings.h
    include/windowobserver.cpp
    include/windowobserver.h
)

add_subdirectory(libs)
add_subdirectory(plugins)
add_subdirectory(radiant build)