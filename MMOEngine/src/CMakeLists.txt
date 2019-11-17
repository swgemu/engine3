# Copyright (C) 2007-2019 SWGEmu
# See file COPYING for copying conditions.

# Add sources
file(GLOB_RECURSE system3_sources "system/*.cpp")
file(GLOB_RECURSE engine3_sources "engine/*.cpp")

#Exclude sources from engine
#file(GLOB_RECURSE engine3_excludes "")
#list(REMOVE_ITEM engine3_sources ${engine3_excludes})

# Add includes
include_directories(.)
include_directories(../external/dlmalloc)
include_directories(../external/ptmalloc3)
include_directories(../external/multimmap)

#message("Compiling for ${ARCH}-bit architecture")

#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib/unix")
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib/unix")

if (ARCH EQUAL 32)
	# Create library
	add_library(engine3_32 SHARED ${system3_sources} ${engine3_sources})

	# Set target specific compile flags
	set_target_properties(engine3_32 PROPERTIES COMPILE_FLAGS -m32 LINK_FLAGS -m32)

	# Install library
	install(TARGETS engine3_32 DESTINATION lib)

else ()
	# Create library
	add_library(engine3 STATIC ${system3_sources} ${engine3_sources})

	TARGET_LINK_LIBRARIES(engine3
			${Boost_LIBRARIES}
			${DB_LIBRARIES}
			${LUA_LIBRARIES}
			${ZLIB_LIBRARIES}
			Threads::Threads
			#${MYSQL_CLIENT_LIBS}
			${CMAKE_DL_LIBS}
			${OPENSSL_LIBRARIES}
			)

	# Install headers and library
	install(TARGETS engine3 DESTINATION lib)
	install(DIRECTORY . DESTINATION include
		FILES_MATCHING PATTERN "*.h"
		PATTERN "testsuite3" EXCLUDE)

	# Install idl sources
	install(DIRECTORY . DESTINATION include
		FILES_MATCHING PATTERN "*.idl")

	# Install idlc
	install(PROGRAMS ../bin/idlc DESTINATION bin)
	install(FILES ../lib/idlc.jar DESTINATION lib)
endif ()

set_property(TARGET engine3 PROPERTY POSITION_INDEPENDENT_CODE ON)

add_custom_target(engine3idl
	COMMAND ${Java_JAVA_EXECUTABLE} -cp ${CMAKE_CURRENT_SOURCE_DIR}/../lib/idlc.jar org.sr.idlc.compiler.Compiler -sd src anyadEclipse
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../
	COMMENT "Building engine3 idl sources"
	VERBATIM)


add_custom_target(engine3rebuild-idl
	COMMAND ${Java_JAVA_EXECUTABLE} -cp ${CMAKE_CURRENT_SOURCE_DIR}/../lib/idlc.jar org.sr.idlc.compiler.Compiler -rb -sd src anyadEclipse
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../
	COMMENT "Rebuilding engine3 idl sources"
	VERBATIM)

add_dependencies(engine3 engine3idl)

if (ENGINE_TYPE EQUAL 2 AND NOT APPLE AND NOT (ENABLE_ASAN OR ENABLE_TSAN OR ENABLE_UBSAN))
	add_custom_command(
		TARGET engine3 POST_BUILD
		COMMAND strip --strip-debug lib/unix/libengine3.a
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../
		VERBATIM)
endif ()

if (ENABLE_ASAN)
	add_custom_command(
		TARGET engine3 POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
		$<TARGET_FILE:engine3>
		${CMAKE_CURRENT_SOURCE_DIR}/../lib/unix/libengine3-asan.a)
endif (ENABLE_ASAN)

if (ENABLE_TSAN)
	add_custom_command(
		TARGET engine3 POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
		$<TARGET_FILE:engine3>
		${CMAKE_CURRENT_SOURCE_DIR}/../lib/unix/libengine3-tsan.a)
endif (ENABLE_TSAN)

if (ENABLE_UBSAN)
	add_custom_command(
		TARGET engine3 POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
		$<TARGET_FILE:engine3>
		${CMAKE_CURRENT_SOURCE_DIR}/../lib/unix/libengine3-ubsan.a)
endif (ENABLE_UBSAN)
