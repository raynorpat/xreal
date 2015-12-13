project (cgame)

file(GLOB CGAME_HEADERS
    	"*.h"
)

file(GLOB CGAME_SOURCES
    	"*.c"

)

list(APPEND CGAME_SOURCES
	../game/bg_misc.c
    	../game/bg_pmove.c
	../game/bg_slidemove.c
	../game/q_math.c
	../game/q_shared.c
)

qf_add_library(cgame SHARED ${CGAME_SOURCES} ${CGAME_HEADERS})
qf_set_output_dir(cgame base)