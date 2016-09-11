set (common_FILES
  macros.h
	typedefs.h
	dlist.c
	dlist.h
	int_fstack.c
	int_fstack.h
	util.c
	util.h
	dominance.c
	dominance.h
	input.c
	input.h
	output.c
	output.h
)

add_library(common ${common_FILES})
target_compile_options(common PUBLIC -Wall -Wextra -std=gnu99)
