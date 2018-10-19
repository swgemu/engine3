# Copyright (C) 2007 <SWGEmu>. All rights reserved.
# Distribution of this file for usage outside of Core3 is prohibited.

cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

project(engine3 CXX C)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_ASAN "Enables address sanitizer compiling" OFF)
option(ENABLE_TSAN "Enables thread sanitizer compiling" OFF)
option(ENABLE_NATIVE "Enables -march=native compiler option" ON)

# Set cmake find moudles path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/build/cmake/Modules/")
MESSAGE(STATUS "CMAKE_MODULE_PATH: " ${CMAKE_MODULE_PATH})

# Add lua includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external)

find_package(Lua "5.3" REQUIRED)
include_directories(${LUA_INCLUDE_DIR})

# Find mysql includes
#find_package(MySql REQUIRED)

# Find berkeley includes
find_package(Berkeley REQUIRED)
include_directories(${DB_INCLUDE_DIR})

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})

find_package(Java 1.5 COMPONENTS Runtime REQUIRED)

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

if (NOT APPLE)
	find_package(GccAtomic REQUIRED)
endif ()

# Define build flags for public or private engine builds
if (ENGINE_TYPE EQUAL 2)
	message(STATUS "Building public engine")
	set(CMAKE_CXX_FLAGS "-g0 -DDEFAULT_DYNAMIC_CAST -Wall -Wextra -Wno-unused -Wno-unused-parameter -Wno-overloaded-virtual -Wno-write-strings -Wno-deprecated-register -DVERSION_PUBLIC -fPIC")
else ()
	message(STATUS "Building private engine")
	set(CMAKE_CXX_FLAGS "-ggdb -DDEFAULT_DYNAMIC_CAST -Wall -Wextra -Wno-unused -Wno-unused-parameter -Wno-overloaded-virtual -Wno-write-strings -Wno-deprecated-register -O2 -fno-omit-frame-pointer")

	if (ENABLE_NATIVE)
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
	endif(ENABLE_NATIVE)
endif ()

if (ENABLE_ASAN)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
	set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} -fsanitize=address")
	message(STATUS "Address sanitizer enabled")
endif(ENABLE_ASAN)

if (ENABLE_TSAN)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
	set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} -fsanitize=thread")
	message(STATUS "Thread sanitizer enabled")
endif(ENABLE_TSAN)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib/unix")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib/unix")

add_subdirectory(src)
add_subdirectory(src/testsuite3)
