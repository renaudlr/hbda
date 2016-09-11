add_executable(nonincremental ubs.c kdtree.c main.c)

#add_definitions (-DSZ_GP)
target_link_libraries(nonincremental common)
target_compile_options(nonincremental PUBLIC -Wall -Wextra -std=gnu99)

install (TARGETS nonincremental DESTINATION bin)
