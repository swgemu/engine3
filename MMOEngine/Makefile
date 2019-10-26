# Copyright (C) 2007-2019 SWGEmu
# See file COPYING for copying conditions.

IDLC = bin/idlc

INSTALL_PATH = /usr/local/engine3

ENGINE_PUB_PATH = $(HOME)/workspace/PublicEngineTest/MMOEngine

IDL_SOURCES = engine/core/ManagedObject.idl \
			  engine/core/ManagedService.idl \
			  engine/core/util/ManagedVector.idl \
			  engine/util/Observer.idl \
			  engine/util/Observable.idl \
			  engine/util/Facade.idl \
			  testsuite3/tests/TestIDLClass.idl \
			  testsuite3/tests/TestNoOrbClass.idl

IDL_DIRECTIVES =

BUILD_PATH = build/unix

SANITIZERS_CXX_COMPILER = clang++-7
SANITIZERS_C_COMPILER = clang-7

all: build-cmake-all

build: build-cmake

compile-idlc:
	$(IDLC) $(IDL_DIRECTIVES) -sd src $(IDL_SOURCES)

recompile-idlc:
	$(IDLC) -rb $(IDL_DIRECTIVES) -sd src $(IDL_SOURCES)

compile-engine: build-cmake
	#cd $(BUILD_PATH) && make

modules:
	cd external/multimmap && make -C /usr/src/linux-headers-`uname -r` M=`pwd` modules
	cp external/multimmap/multimmap.ko bin/
	#rmmod multimmap
	#insmod bin/multimmap.ko

force-clean:
	rm -rf $(BUILD_PATH)
	mkdir -p $(BUILD_PATH)

clean: recompile-idlc
	cd $(BUILD_PATH) && ${MAKE} clean

clean-distribution: recompile-idlc
	cd $(BUILD_PATH) && ${MAKE} distclean

install:
	mkdir -p ${INSTALL_PATH}/lib
	cp bin/idlc /usr/local/bin
	build/cmake/install ${INSTALL_PATH} #> /dev/null 2>&1

install-headers:
	build/cmake/install $(ENGINE_PUB_PATH) -u > /dev/null

install-lib:
	cp $(BUILD_PATH)/src/libengine3.a lib/unix
	#cp $(BUILD_PATH)/src/testsuite3/testsuite3* bin
	#rm $(BUILD_PATH)/src/testsuite3/testsuite3*

install-public:
	strip --strip-debug lib/unix/libengine3.a
	build/cmake/install $(ENGINE_PUB_PATH)

pch:
	cd src && gcc -c $(CXXFLAGS) $(INCLUDES) -o system/lang.h.gch system/lang.h
	cd src && gcc -c $(CXXFLAGS) $(INCLUDES) -o engine/engine.h.gch engine/engine.h

build-cmake:
	cd $(BUILD_PATH) && cmake -DENGINE_TYPE=1 -DENABLE_LOCKFREE_BCLIENT_BUFFERS=ON -DCMAKE_INSTALL_PREFIX=/opt/engine3 -DARCH=64 ../../ && ${MAKE}

build-cmake-all:
	${MAKE} build-cmake-asan
	${MAKE} build-cmake-tsan
	${MAKE} build-cmake-ubsan
	${MAKE} force-clean
	${MAKE} build-cmake

build-tags:
	cd $(BUILD_PATH) && ${MAKE} tags

build-compdb:
	cd $(BUILD_PATH) && ${MAKE} compdb

build-cmake-asan: force-clean
	cd $(BUILD_PATH) && cmake -DENGINE_TYPE=1 -DENABLE_LOCKFREE_BCLIENT_BUFFERS=ON -DENABLE_ASAN=ON -DCMAKE_INSTALL_PREFIX=/opt/engine3 -DARCH=64 ../../ && ${MAKE}

build-cmake-tsan: force-clean
	cd $(BUILD_PATH) && cmake -DENGINE_TYPE=1 -DENABLE_LOCKFREE_BCLIENT_BUFFERS=ON -DENABLE_TSAN=ON -DCMAKE_INSTALL_PREFIX=/opt/engine3 -DARCH=64 ../../ && ${MAKE}

build-cmake-ubsan: force-clean
	cd $(BUILD_PATH) && cmake -DENGINE_TYPE=1 -DENABLE_LOCKFREE_BCLIENT_BUFFERS=ON -DENABLE_UBSAN=ON -DCMAKE_INSTALL_PREFIX=/opt/engine3 -DARCH=64 ../../ && ${MAKE}

build-ninja:
	mkdir -p $(BUILD_PATH)/ninja
	cd $(BUILD_PATH)/ninja && cmake -G Ninja -DENGINE_TYPE=1 -DENABLE_LOCKFREE_BCLIENT_BUFFERS=ON -DCMAKE_INSTALL_PREFIX=/opt/engine3 -DARCH=64 ../../../ && ninja

clean-ninja:
	rm -rf $(BUILD_PATH)/ninja

build-cmake-pub:
	${MAKE} build-cmake-pub-asan
	${MAKE} build-cmake-pub-tsan
	${MAKE} build-cmake-pub-ubsan
	${MAKE} force-clean
	cd $(BUILD_PATH) && cmake -DENGINE_TYPE=2 -DCMAKE_INSTALL_PREFIX=/opt/engine3pub -DARCH=64 ../../ && ${MAKE}

build-cmake-pub-asan: force-clean
	cd $(BUILD_PATH) && cmake -DENGINE_TYPE=2 -DENABLE_ASAN=ON -DCMAKE_CXX_COMPILER=$(SANITIZERS_CXX_COMPILER) -DCMAKE_C_COMPILER=$(SANITIZERS_C_COMPILER) -DCMAKE_INSTALL_PREFIX=/opt/engine3pub -DARCH=64 ../../ && ${MAKE}

build-cmake-pub-tsan: force-clean
	cd $(BUILD_PATH) && cmake -DENGINE_TYPE=2 -DENABLE_TSAN=ON -DCMAKE_CXX_COMPILER=$(SANITIZERS_CXX_COMPILER) -DCMAKE_C_COMPILER=$(SANITIZERS_C_COMPILER) -DCMAKE_INSTALL_PREFIX=/opt/engine3pub -DARCH=64 ../../ && ${MAKE}

build-cmake-pub-ubsan: force-clean
	cd $(BUILD_PATH) && cmake -DENGINE_TYPE=2 -DENABLE_UBSAN=ON -DCMAKE_CXX_COMPILER=$(SANITIZERS_CXX_COMPILER) -DCMAKE_C_COMPILER=$(SANITIZERS_C_COMPILER) -DCMAKE_INSTALL_PREFIX=/opt/engine3pub -DARCH=64 ../../ && ${MAKE}

build-clang-pub:
	cd $(BUILD_PATH) && cmake -DCMAKE_CXX_COMPILER=$(SANITIZERS_CXX_COMPILER) -DCMAKE_C_COMPILER=$(SANITIZERS_C_COMPILER) -DENGINE_TYPE=2 -DCMAKE_INSTALL_PREFIX=/opt/engine3pub -DARCH=64 ../../ && ${MAKE}

install-cmake: build-cmake
	cd $(BUILD_PATH) && ${MAKE} install

#install-cmake-pub:
#	cd $(BUILD_PATH) && make install/strip
#	cd $(BUILD_PATH)32 && make install/strip

clean-cmake: recompile-idlc
	cd $(BUILD_PATH) && ${MAKE} clean
#	cd $(BUILD_PATH)32 && make clean

