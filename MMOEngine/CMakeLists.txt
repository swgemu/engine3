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

# Define build flags for public or private engine builds
if (VERSION_PUBLIC)
	message("Building public engine")
	set(CMAKE_CXX_FLAGS "-g0 -Wall -Wextra -Wno-unused -Wno-unused-parameter -Wno-overloaded-virtual")
else ()
	message("Building private engine")
	set(CMAKE_CXX_FLAGS "-ggdb -Wall -Wextra -Wno-unused -Wno-unused-parameter -Wno-overloaded-virtual")
endif ()

add_subdirectory(src)