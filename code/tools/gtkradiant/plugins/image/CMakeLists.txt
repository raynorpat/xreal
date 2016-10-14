radiant_plugin(image
    bmp.cpp
    bmp.h
    dds.cpp
    dds.h
    image.cpp
    image.h
    jpeg.cpp
    jpeg.h
    pcx.cpp
    pcx.h
    tga.cpp
    tga.h
)

find_package(JPEG REQUIRED)
include_directories(${JPEG_INCLUDE_DIR})
target_link_libraries(image ddslib ${JPEG_LIBRARIES})