include_directories("${CMAKE_SOURCE_DIR}/libs")
include_directories("${CMAKE_SOURCE_DIR}/include")

find_package(GTK2 REQUIRED)
include_directories(${GTK2_INCLUDE_DIRS} ${GTK2_GDK_INCLUDE_DIR} ${GTK2_GLIB_INCLUDE_DIR})

find_package(LibXml2 REQUIRED)
include_directories(${LIBXML2_INCLUDE_DIR})

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/modules")

add_custom_target(modules)
macro(radiant_plugin name)
    message(STATUS "Found Module ${name}")
    add_library(${name} MODULE ${ARGN})
    add_dependencies(modules ${name})
endmacro()

add_subdirectory(archivezip)
add_subdirectory(brushexport)
add_subdirectory(entity)
add_subdirectory(image)
add_subdirectory(imagepng)
add_subdirectory(mapq3)
add_subdirectory(model)
add_subdirectory(shaders)
add_subdirectory(vfspk3)