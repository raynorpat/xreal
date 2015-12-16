project(xmaster)

file(GLOB XMASTER_HEADERS
    "*.h"
)

file(GLOB XMASTER_SOURCES
	"*.c"
)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(XMASTER_PLATFORM_LIBRARIES "ws2_32.lib")
    set(XMASTER_BINARY_TYPE WIN32)
else()
    set(XMASTER_PLATFORM_LIBRARIES "m")
    set(XMASTER_BINARY_TYPE "")
endif()

qf_add_executable(xmaster ${XMASTER_BINARY_TYPE} ${XMASTER_HEADERS} ${XMASTER_SOURCES})
target_link_libraries(xmaster ${XMASTER_PLATFORM_LIBRARIES})
qf_set_output_dir(xmaster "")
