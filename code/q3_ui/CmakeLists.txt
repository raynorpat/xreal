project (ui)

file(GLOB UI_HEADERS
    	"*.h"
)

file(GLOB UI_SOURCES
    	"*.c"
)

list(APPEND UI_SOURCES
	../game/bg_misc.c
	../game/q_math.c
	../game/q_shared.c
)

qf_add_library(ui SHARED ${UI_SOURCES} ${UI_HEADERS})
qf_set_output_dir(ui base)