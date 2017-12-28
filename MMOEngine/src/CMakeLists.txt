# Copyright (C) 2007 <SWGEmu>. All rights reserved.
# Distribution of this file for usage outside of Core3 is prohibited.

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
			${DB_LIBRARIES}
			${LUA_LIBRARIES}
			${ZLIB_LIBRARIES}
			Threads::Threads
			${MYSQL_CLIENT_LIBS}
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
	
