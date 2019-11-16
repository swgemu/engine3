# Copyright (C) 2007-2019 SWGEmu
# See file COPYING for copying conditions.

cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

project(engine3 CXX C)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CheckCXXSourceCompiles)
include(CheckCXXCompilerFlag)

option(ENABLE_ASAN "Enables address sanitizer compiling" OFF)
option(ENABLE_TSAN "Enables thread sanitizer compiling" OFF)
option(ENABLE_UBSAN "Enables undefined behavior sanitizer compiling" OFF)
option(ENABLE_NATIVE "Enables -march=native compiler option" ON)
option(ENABLE_ODB_REFERENCES "Enables ODB_REFERENCES define" ON)
option(ENABLE_LOCKFREE_BCLIENT_BUFFERS "Enables lock free base client buffers, requires boost thread" OFF)
option(ENABLE_OMIT_FRAMEPOINTER "Enables -fno-omit-frame-pointer compiler option" ON)

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

if (ENABLE_ODB_REFERENCES)
	add_definitions(-DODB_REFERENCES)
endif()

if (ENABLE_LOCKFREE_BCLIENT_BUFFERS)
	add_definitions(-DLOCKFREE_BCLIENT_BUFFERS)

	find_package(Boost REQUIRED COMPONENTS thread)
endif()

if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
endif()

function(check_working_cxx_atomics varname)
	set(OLD_CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS})
	set(CMAKE_REQUIRED_FLAGS "-std=c++11")
	CHECK_CXX_SOURCE_COMPILES("
	#include <atomic>
	struct test_type { float x[3]; };
	std::atomic<test_type> atom;
	test_type old;
	test_type newval;
	int main() {
	newval.x[2] = 3;
	return atom.compare_exchange_strong(old, newval);
	}
	" ${varname})
	set(CMAKE_REQUIRED_FLAGS ${OLD_CMAKE_REQUIRED_FLAGS})
endfunction(check_working_cxx_atomics)

check_working_cxx_atomics(HAVE_CXX_ATOMICS_WITHOUT_LIB)

if (NOT HAVE_CXX_ATOMICS_WITHOUT_LIB)
	find_package(GccAtomic REQUIRED)
endif()

add_definitions(-DDEFAULT_DYNAMIC_CAST)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # Define global build gcc compatible flags to check and add if available
    set(GCCEXTRAFLAGS -Wall
	-Wextra
	-Wno-unused
	-Wno-sign-compare
	-Wno-multichar
	-Wno-unused-parameter
	-Wno-overloaded-virtual
	-Wno-missing-declarations
	-Wno-return-std-move
	-Wno-implicit-fallthrough
	-Wno-class-memaccess
	-fcolor-diagnostics)

    foreach(flag_comp ${GCCEXTRAFLAGS})
	CHECK_CXX_COMPILER_FLAG(${flag_comp} CxxFlag${flag_comp})

	if (CxxFlag${flag_comp})
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag_comp}")
	endif()
    endforeach(flag_comp)

    if (ENABLE_ASAN)
	   set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
	   set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} -fsanitize=address")

	   message(STATUS "Adress sanitizer enabled")
    endif(ENABLE_ASAN)

    if (ENABLE_TSAN)
	   set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
	   set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} -fsanitize=thread")

	   message(STATUS "Thread sanitizer enabled")
    endif(ENABLE_TSAN)

    if (ENABLE_UBSAN)
	   set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
	   set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} -fsanitize=undefined")

	   message(STATUS "Undefined behavior sanitizer sanitizer enabled")
    endif(ENABLE_UBSAN)

    if (ENABLE_NATIVE)
	   CHECK_CXX_COMPILER_FLAG(-march=native CxxFlagMnative)

	   if (CxxFlagMnative)
	        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
	   endif (CxxFlagMnative)
    endif(ENABLE_NATIVE)

    if (ENABLE_OMIT_FRAMEPOINTER)
	   CHECK_CXX_COMPILER_FLAG(-fno-omit-frame-pointer CxxFlagFramePointer)

	   if (CxxFlagFramePointer)
           	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer")
	   endif (CxxFlagFramePointer)
    endif()
endif()



# Define build flags for public or private engine builds
if (ENGINE_TYPE EQUAL 2)
	message(STATUS "Building public engine")

	add_definitions(-DVERSION_PUBLIC)
else ()
	message(STATUS "Building private engine")
endif ()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib/unix")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib/unix")

add_subdirectory(src)
add_subdirectory(src/testsuite3)

set_source_files_properties(tags PROPERTIES GENERATED true)
add_custom_target(engine3tags
    COMMAND ctags -R --fields=+iaS --extra=+q src/
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set_source_files_properties(compile_commands.json PROPERTIES GENERATED true)
add_custom_target(engine3compdb
	COMMAND compdb -p ${CMAKE_BINARY_DIR} list > compile_commands.json
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_dependencies(engine3compdb engine3)

