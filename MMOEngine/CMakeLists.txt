# Copyright (C) 2007 <SWGEmu>. All rights reserved.
# Distribution of this file for usage outside of Core3 is prohibited.

cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

project(engine3 CXX C)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set cmake find moudles path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/build/cmake/Modules/")
MESSAGE(STATUS "CMAKE_MODULE_PATH:" ${CMAKE_MODULE_PATH})

# Add lua includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external)

find_package(Lua "5.3" REQUIRED)
include_directories(${LUA_INCLUDE_DIR})

# Find mysql includes
find_package(MySql REQUIRED)

# Find berkeley includes
find_package(Berkeley REQUIRED)
include_directories(${DB_INCLUDE_DIR})

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

find_program(CCACHE_FOUND ccache)

if(CCACHE_FOUND)
	set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
	set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost QUIET OPTIONAL_COMPONENTS thread)

if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
endif()

# Define build flags for public or private engine builds
if (ENGINE_TYPE EQUAL 2)
	message(STATUS "Building public engine")
	set(CMAKE_CXX_FLAGS "-g0 -Wall -Wextra -Wno-unused -Wno-unused-parameter -Wno-overloaded-virtual -Wno-write-strings -Wno-deprecated-register")
else ()
	message(STATUS "Building private engine")
	set(CMAKE_CXX_FLAGS "-ggdb -Wall -Wextra -Wno-unused -Wno-unused-parameter -Wno-overloaded-virtual -Wno-write-strings -Wno-deprecated-register")
endif ()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib/unix")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib/unix")

add_subdirectory(src)
add_subdirectory(src/testsuite3)
