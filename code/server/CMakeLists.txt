project(server)

include_directories(${ZLIB_INCLUDE_DIR})

file(GLOB SERVER_HEADERS
    "*.h"
)

file(GLOB SERVER_SOURCES
    "../qcommon/*.c"
    "*.c"
    "../null/null_client.c"
    "../null/null_input.c"
    "../null/null_snddma.c"
    "../game/q_*.c"
)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    file(GLOB SERVER_PLATFORM_SOURCES 
        "../sys/sys_main.c"
        "../sys/con_log.c"
        
        "../sys/sys_win32.c"
        "../sys/con_win32.c"
    )

    set(SERVER_PLATFORM_LIBRARIES "ws2_32.lib" "winmm.lib")
    set(SERVER_BINARY_TYPE WIN32)
else()
    file(GLOB SERVER_PLATFORM_SOURCES 
   		"../sys/sys_main.c"
        "../sys/con_log.c"
        
        "../sys/sys_unix.c"
        "../sys/con_tty.c"
    )

    set(SERVER_PLATFORM_LIBRARIES "pthread" "dl" "m")
    set(SERVER_BINARY_TYPE "")
endif()

qf_add_executable(xreal_server ${SERVER_BINARY_TYPE} ${SERVER_HEADERS} ${SERVER_SOURCES} ${SERVER_PLATFORM_SOURCES})
target_link_libraries(xreal_server ${ZLIB_LIBRARY} ${SERVER_PLATFORM_LIBRARIES})
qf_set_output_dir(xreal_server "")

set_target_properties(xreal_server PROPERTIES COMPILE_DEFINITIONS "DEDICATED;")
