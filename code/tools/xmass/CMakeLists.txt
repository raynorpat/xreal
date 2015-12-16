project(xmaster)

file(GLOB XMASTER_HEADERS
    "*.h"
)

file(GLOB XMASTER_SOURCES
	"*.c"
)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(XMAP_PLATFORM_LIBRARIES "ws2_32.lib")
    set(XMAP_BINARY_TYPE WIN32)
else()
    set(XMAP_PLATFORM_LIBRARIES "dl" "m")
    set(XMAP_BINARY_TYPE "")
endif()

qf_add_executable(xmaster ${XMAP_BINARY_TYPE} ${XMAP_HEADERS} ${XMAP_SOURCES})
target_link_libraries(xmaster ${XMAP_PLATFORM_LIBRARIES})
qf_set_output_dir(xmaster "")
