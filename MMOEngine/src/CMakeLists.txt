# Copyright (C) 2007 <SWGEmu>. All rights reserved.
# Distribution of this file for usage outside of Core3 is prohibited.

FILE(GLOB_RECURSE engine3_sources "*.cpp") 

INCLUDE_DIRECTORIES(.)

ADD_LIBRARY(engine3 ${engine3_sources})