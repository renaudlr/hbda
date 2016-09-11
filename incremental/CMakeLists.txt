add_executable(incremental search_zone.c main.c)

#add_definitions (-DSZ_GP)
target_link_libraries(incremental common)
target_compile_options(incremental PUBLIC -Wall -Wextra -std=gnu99)

install (TARGETS incremental DESTINATION bin)
