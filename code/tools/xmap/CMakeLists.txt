project(xmap)

include_directories(${ZLIB_INCLUDE_DIR} ${JPEG_INCLUDE_DIR} ${PNG_INCLUDE_DIR})

file(GLOB XMAP_HEADERS
    "*.h"
)

file(GLOB XMAP_SOURCES
	"brush.c"
	"brush_primit.c"
	"bsp.c"
	"convert_ase.c"
	"convert_map.c"
	"convert_t3dtomap.c"
	"facebsp.c"
	"fog.c"
	"glfile.c"
	"leakfile.c"
	"light.c"
	"light_trace.c"
	"lightmaps.c"
	"lightv.c"
	"main.c"
	"map.c"
	"mesh.c"
	"misc_model.c"
	"patch.c"
	"portals.c"
	"prtfile.c"
	"shaders.c"
	"surface.c"
	"terrain.c"
	"tjunction.c"
	"tree.c"
	"vis.c"
	"visflow.c"
	"writebsp.c"
	"../common/cmdlib.c"
	"../common/mathlib.c"
	"../common/polylib.c"
	"../common/aselib.c"
	"../common/imagelib.c"
	"../common/inout.c"
	"../common/scriplib.c"
	"../common/threads.c"
	"../common/bspfile.c"
	"../common/mutex.c"
	"../common/unzip.c"
	"../common/ioapi.c"
	"../common/vfs.c"
	"../common/netlib.c"
	"../lwobject/*.c"
)

file(GLOB ZLIB_SOURCES
	"../../libs/zlib/adler32.c"
	"../../libs/zlib/compress.c"
	"../../libs/zlib/crc32.c"
	"../../libs/zlib/gzio.c"
	"../../libs/zlib/uncompr.c"
	"../../libs/zlib/deflate.c"
	"../../libs/zlib/trees.c"
	"../../libs/zlib/zutil.c"
	"../../libs/zlib/inflate.c"
	"../../libs/zlib/infback.c"
	"../../libs/zlib/inftrees.c"
	"../../libs/zlib/inffast.c"
	"../../libs/zlib/ioapi.c"
)

file(GLOB PNG_SOURCES
	"../../libs/libpng/png.c"
	"../../libs/libpng/pngset.c"
	"../../libs/libpng/pngget.c"
	"../../libs/libpng/pngrutil.c"
	"../../libs/libpng/pngtrans.c"
	"../../libs/libpng/pngwutil.c"
	"../../libs/libpng/pngread.c"
	"../../libs/libpng/pngrio.c"
	"../../libs/libpng/pngwio.c"
	"../../libs/libpng/pngwrite.c"
	"../../libs/libpng/pngrtran.c"
	"../../libs/libpng/pngwtran.c"
	"../../libs/libpng/pngmem.c"
	"../../libs/libpng/pngerror.c"
	"../../libs/libpng/pngpread.c"
)

file(GLOB JPEG_SOURCES
	"../../libs/libjpeg/jaricom.c"
	"../../libs/libjpeg/jcarith.c"
	"../../libs/libjpeg/jcapimin.c"
	"../../libs/libjpeg/jchuff.c"
	"../../libs/libjpeg/jcinit.c"
	"../../libs/libjpeg/jccoefct.c"
	"../../libs/libjpeg/jccolor.c"
	"../../libs/libjpeg/jfdctflt.c"
	"../../libs/libjpeg/jfdctfst.c"
	"../../libs/libjpeg/jfdctint.c"
	"../../libs/libjpeg/jidctflt.c"
	"../../libs/libjpeg/jidctfst.c"
	"../../libs/libjpeg/jidctint.c"
	"../../libs/libjpeg/jcdctmgr.c"
	"../../libs/libjpeg/jcphuff.c"
	"../../libs/libjpeg/jcmainct.c"
	"../../libs/libjpeg/jcmarker.c"
	"../../libs/libjpeg/jcmaster.c"
	"../../libs/libjpeg/jcomapi.c"
	"../../libs/libjpeg/jcparam.c"
	"../../libs/libjpeg/jcprepct.c"
	"../../libs/libjpeg/jcsample.c"
	"../../libs/libjpeg/jdarith.c"
	"../../libs/libjpeg/jdapimin.c"
	"../../libs/libjpeg/jdapistd.c"
	"../../libs/libjpeg/jdatasrc.c"
	"../../libs/libjpeg/jdcoefct.c"
	"../../libs/libjpeg/jdcolor.c"
	"../../libs/libjpeg/jddctmgr.c"
	"../../libs/libjpeg/jdhuff.c"
	"../../libs/libjpeg/jdinput.c"
	"../../libs/libjpeg/jdmainct.c"
	"../../libs/libjpeg/jdmarker.c"
	"../../libs/libjpeg/jdmaster.c"
	"../../libs/libjpeg/jdmerge.c"
	"../../libs/libjpeg/jdpostct.c"
	"../../libs/libjpeg/jdsample.c"
	"../../libs/libjpeg/jdtrans.c"
	"../../libs/libjpeg/jerror.c"
	"../../libs/libjpeg/jidctflt.c"
	"../../libs/libjpeg/jmemmgr.c"
	"../../libs/libjpeg/jmemansi.c"
	"../../libs/libjpeg/jquant1.c"
	"../../libs/libjpeg/jquant2.c"
	"../../libs/libjpeg/jutils.c"
)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    file(GLOB XMAP_PLATFORM_SOURCES 
        "nodraw.c"
    )

    set(XMAP_PLATFORM_LIBRARIES "ws2_32.lib")
    set(XMAP_BINARY_TYPE WIN32)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    file(GLOB XMAP_PLATFORM_SOURCES 
   		"gldraw.c"
    )

    set(XMAP_PLATFORM_LIBRARIES "pthread" "dl" "m" ${SDL_LIBRARY} "-framework OpenGL")
    set(XMAP_BINARY_TYPE "")
else()
    file(GLOB XMAP_PLATFORM_SOURCES 
   		"gldraw.c"
    )

    set(XMAP_PLATFORM_LIBRARIES "pthread" "dl" "m" ${SDL_LIBRARY} "GL" "GLU")
    set(XMAP_BINARY_TYPE "")
endif()

qf_add_executable(xmap ${XMAP_BINARY_TYPE} ${XMAP_HEADERS} ${XMAP_SOURCES} ${XMAP_PLATFORM_SOURCES} ${ZLIB_SOURCES} ${PNG_SOURCES} ${JPEG_SOURCES})
target_link_libraries(xmap ${XMAP_PLATFORM_LIBRARIES})
qf_set_output_dir(xmap "")

set_target_properties(xmap PROPERTIES COMPILE_DEFINITIONS "XMAP;")
