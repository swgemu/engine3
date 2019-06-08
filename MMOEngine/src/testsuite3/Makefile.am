# Copyright (C) 2007-2019 SWGEmu
# See file COPYING for copying conditions.

AM_CXXFLAGS=-ggdb -Wall -Wextra -Wno-unused -Wno-sign-compare -Wno-overloaded-virtual -Wno-unused-parameter
#CC = clang
#CXX = clang++

ENGINE3_PATH=../../../..

INCLUDES =  $(SYSTEM_INCLUDES) \
	    -I$(USER_INCLUDES_PATH) -I$(USER_INCLUDES_PATH)/mysql \
	    -I$(ENGINE3_PATH)/src -I/usr/include/lua5.1 -I/usr/include/mysql \
	   	-I/usr/local/BerkeleyDB.5.0/include
			
LIBS =	$(SYSTEM_LIBS)  \
	-L../../../build/unix/src/client \
	-L/usr/local/BerkeleyDB.5.0/lib \
	-L$(USER_LIBS_PATH)/ -L$(USER_LIBS_PATH)/mysql -L$(USER_LIBS_PATH)/mysql5/mysql -L$(USER_LIBS_PATH)/lua -L$(ENGINE3_PATH)/build/unix/src \
	-lengine3 -lmysqlclient -llua -lz -ldb

bin_PROGRAMS = testsuite3

testsuite3_SOURCES = \
		tests/TestIDLClass.cpp \
		TestSuite3.cpp
