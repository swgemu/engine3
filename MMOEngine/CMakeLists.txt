# Copyright (C) 2007 <SWGEmu>. All rights reserved.
# Distribution of this file for usage outside of Core3 is prohibited.

cmake_minimum_required(VERSION 2.6)

project(engine3)

# Set cmake find moudles path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/build/cmake/Modules/")
MESSAGE(STATUS "CMAKE_MODULE_PATH:" ${CMAKE_MODULE_PATH})

# Add lua includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external)

find_package(Lua "5.3")
include_directories(${LUA_INCLUDE_DIR})
link_directories(${LUA_LIBRARIES})

# Find mysql includes
find_package(MySql REQUIRED)
include_directories(${MYSQL_INCLUDE_DIR})

# Find berkeley includes
find_package(Berkeley REQUIRED)
include_directories(${DB_INCLUDE_DIR})
link_directories(${DB_LIBRARIES})
link_directories(/opt/local/lib)

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

# Define build flags for public or private engine builds
if (ENGINE_TYPE EQUAL 2)
	message("Building public engine")
	set(CMAKE_CXX_FLAGS "-std=c++11 -mmacosx-version-min=10.11 -g0 -Wall -Wextra -Wno-unused -Wno-unused-parameter -Wno-overloaded-virtual -Wno-write-strings -Wno-deprecated-register")
else ()
	message("Building private engine")
	set(CMAKE_CXX_FLAGS "-std=c++11 -mmacosx-version-min=10.11 -ggdb -Wall -Wextra -Wno-unused -Wno-unused-parameter -Wno-overloaded-virtual -Wno-write-strings -Wno-deprecated-register -march=native -O2")
endif ()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib/unix")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib/unix")


add_subdirectory(src)
add_subdirectory(src/testsuite3)
