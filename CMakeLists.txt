cmake_minimum_required(VERSION 2.6)

project(HBDA)

if ("${CMAKE_SYSTEM_NAME}" MATCHES Linux OR "${CMAKE_SYSTEM_NAME}" STREQUAL GNU)
	add_definitions (-D_GNU_SOURCE)
endif()

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -march=native")

add_definitions (-DMINIMIZE)
#add_definitions (-DMAXIMIZE)
include_directories ("${PROJECT_BINARY_DIR}")
include_directories ("${PROJECT_SOURCE_DIR}/common")
add_subdirectory (common)
set (EXTRA_LIBS ${EXTRA_LIBS} common)

add_subdirectory (incremental)
add_subdirectory (nonincremental)
