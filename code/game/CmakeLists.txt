project (game)

file(GLOB GAME_HEADERS
    "*.h"
)

file(GLOB GAME_SOURCES
    "*.c"
)

qf_add_library(game SHARED ${GAME_SOURCES} ${GAME_HEADERS})
qf_set_output_dir(game base)