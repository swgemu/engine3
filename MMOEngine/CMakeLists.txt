# Copyright (C) 2007 <SWGEmu>. All rights reserved.
# Distribution of this file for usage outside of Core3 is prohibited.

cmake_minimum_required(VERSION 2.6)

project(engine3)

# Set cmake find moudles path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_BINARY_DIR}/../cmake/Modules/")

# Add lua includes
include_directories(/usr/include/lua5.1)

# Find mysql includes
find_package(MySql REQUIRED)
include_directories(${MYSQL_INCLUDE_DIR})

# Find berkeley includes
find_package(Berkeley REQUIRED)
include_directories(${DB_INCLUDE_DIR})
link_directories(${DB_LIBRARIES})

# Define build flags for public or private engine builds
if (ENGINE_TYPE EQUAL 2)
	message("Building public engine")
	set(CMAKE_CXX_FLAGS "-g0 -Wall -Wextra -Wno-unused -Wno-unused-parameter -Wno-overloaded-virtual -Wno-write-strings")
else ()
	message("Building private engine")
	set(CMAKE_CXX_FLAGS "-ggdb -Wall -Wextra -Wno-unused -Wno-unused-parameter -Wno-overloaded-virtual -Wno-write-strings")
endif ()

add_subdirectory(src)
add_subdirectory(src/testsuite3)