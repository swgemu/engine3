# Copyright (C) 2007 <SWGEmu>. All rights reserved.
# Distribution of this file for usage outside of Core3 is prohibited.

# set cmake find moudles path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/build/unix/cmake/Modules/")

# find mysql includes and libraries
find_package(MySql REQUIRED)
INCLUDE_DIRECTORIES(${MYSQL_INCLUDE_DIR})
set(LIBS ${LIBS} ${MYSQL_LIBRARY})

# find berkeley includes and libraries
find_package(Berkeley REQUIRED)
INCLUDE_DIRECTORIES(${DB_INCLUDE_DIR})
set(LIBS ${LIBS} ${DB_LIBRARIES})

# add engine3 sources
FILE(GLOB_RECURSE engine3_sources "*.cpp") 

# add engine3 includes
INCLUDE_DIRECTORIES(.)

# create engine3 library
ADD_LIBRARY(engine3 SHARED ${engine3_sources})