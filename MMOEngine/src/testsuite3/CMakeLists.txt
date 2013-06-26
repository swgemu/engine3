# Copyright (C) 2007 <SWGEmu>. All rights reserved.
# Distribution of this file for usage outside of Core3 is prohibited.

# Add testsuite3 sources
file(GLOB_RECURSE testsuite3_sources "*.cpp") 

# Add testsuite3 includes
include_directories(.)
include_directories(..)

# Create testsuite3 binary
add_executable(testsuite3 ${testsuite3_sources})

# Add libraries
target_link_libraries(testsuite3
	z rt dl pthread crypto
	engine3
	lua db-5.0 ${MYSQL_LIBRARIES})

# Install core3client binary
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/testsuite3 DESTINATION bin)
	