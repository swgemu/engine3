# Copyright (C) 2007-2019 SWGEmu
# See file COPYING for copying conditions.

# Add testsuite3 sources
file(GLOB_RECURSE testsuite3_sources "*.cpp")

# Add testsuite3 includes
include_directories(.)
include_directories(..)

# Create testsuite3 binary
add_executable(testsuite3 ${testsuite3_sources})

# Add libraries
target_link_libraries(testsuite3
		engine3
		${Boost_LIBRARIES}
		${DB_LIBRARIES}
		${LUA_LIBRARIES}
		${ZLIB_LIBRARIES}
		Threads::Threads
		${GCCLIBATOMIC_LIBRARY}
		${CMAKE_DL_LIBS}
		${OPENSSL_LIBRARIES}
		${optional_atomic})

install(PROGRAMS testsuite3 DESTINATION bin)

add_custom_command(
	TARGET testsuite3 POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
	$<TARGET_FILE:testsuite3>
	${CMAKE_SOURCE_DIR}/bin/)
