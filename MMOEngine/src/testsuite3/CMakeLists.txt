# Copyright (C) 2007 <SWGEmu>. All rights reserved.
# Distribution of this file for usage outside of Core3 is prohibited.

# Add testsuite3 sources
file(GLOB_RECURSE testsuite3_sources "*.cpp") 

# Add testsuite3 includes
include_directories(.)
include_directories(..)

# Create testsuite3 binary
add_executable(testsuite3 ${testsuite3_sources})

set(CMAKE_CXX_FLAGS "-std=c++11 -O2 -ggdb -Wall -Wextra -Wno-unused -Wno-unused-parameter -Wno-overloaded-virtual -Wno-write-strings -Wno-deprecated-register")

# Add libraries
target_link_libraries(testsuite3
	z dl pthread crypto
	engine3
	lua db-5.3 -L/opt/local/lib/mysql56/mysql ${MYSQL_LIBRARIES} boost_regex)

# Install core3client binary
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/testsuite3 DESTINATION bin)
	