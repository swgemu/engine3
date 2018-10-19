# Copyright (C) 2007 <SWGEmu>. All rights reserved.
# Distribution of this file for usage outside of Core3 is prohibited.

IDLC = bin/idlc

INSTALL_PATH = /usr/local/engine3

ENGINE_PUB_PATH = /home/theanswer/git/PublicEngine/MMOEngine

IDL_SOURCES = engine/core/ManagedObject.idl \
			  engine/core/ManagedService.idl \
			  engine/core/util/ManagedVector.idl \
			  engine/util/Observer.idl \
			  engine/util/Observable.idl \
			  engine/util/Facade.idl \
			  testsuite3/tests/TestIDLClass.idl \
			  testsuite3/tests/TestNoOrbClass.idl

IDL_DIRECTIVES =

all: build-cmake-all
	#cd build/unix && cmake ../.. && make
	#cp build/unix/src/libengine3.so lib/unix

build: compile install-headers
	#done

#all-public:
	#make all-public64
	#make all-public32

#all-public32: clean configure-release32 compile install-public
#	rm -f $(ENGINE_PUB_PATH)/lib/linux32/libengine3.a
#	cp lib/unix/libengine3.a $(ENGINE_PUB_PATH)/lib/linux32

#all-public64: clean configure-release64 compile install-public
#	rm -f $(ENGINE_PUB_PATH)/lib/linux64/libengine3.a
#	cp lib/unix/libengine3.a $(ENGINE_PUB_PATH)/lib/linux64

#rebuild: clean configure-debug compile install-headers
	#done

#config:
#	autoreconf --force --install
	#done

#configure-release32:
#	cd build/unix && ../../configure --with-cross-host i686-linux

#configure-release64:
#	cd build/unix && ../../configure --with-cross-host amd64-linux

#configure-debug:
#	cd build/unix && ../../configure --enable-debug

compile: compile-idlc compile-engine install-lib

compile-idlc:
	$(IDLC) $(IDL_DIRECTIVES) -sd src $(IDL_SOURCES)

recompile-idlc:
	$(IDLC) -rb $(IDL_DIRECTIVES) -sd src $(IDL_SOURCES)

compile-engine: build-cmake
	#cd build/unix && make

modules:
	cd external/multimmap && make -C /usr/src/linux-headers-`uname -r` M=`pwd` modules
	cp external/multimmap/multimmap.ko bin/
	#rmmod multimmap
	#insmod bin/multimmap.ko

force-clean:
	rm -rf build/unix
	mkdir -p build/unix

clean: recompile-idlc
	cd build/unix && make clean
	#done

clean-distribution: recompile-idlc
	cd build/unix && make distclean
	#done

install:
	mkdir -p ${INSTALL_PATH}/lib
	cp bin/idlc /usr/local/bin
	build/cmake/install ${INSTALL_PATH} #> /dev/null 2>&1

install-headers:
	build/cmake/install $(ENGINE_PUB_PATH) -u > /dev/null

install-lib:
	cp build/unix/src/libengine3.a lib/unix
	#cp build/unix/src/testsuite3/testsuite3* bin
	#rm build/unix/src/testsuite3/testsuite3*

install-public:
	strip --strip-debug lib/unix/libengine3.a
	build/cmake/install $(ENGINE_PUB_PATH)

pch:
	cd src && gcc -c $(CXXFLAGS) $(INCLUDES) -o system/lang.h.gch system/lang.h
	cd src && gcc -c $(CXXFLAGS) $(INCLUDES) -o engine/engine.h.gch engine/engine.h

build-cmake:
	cd build/unix && cmake -DENGINE_TYPE=1 -DCMAKE_INSTALL_PREFIX=/opt/engine3 -DARCH=64 ../../ && ${MAKE}
	#done

build-cmake-all:
	rm -rf build/unix
	mkdir -p build/unix
	cd build/unix && cmake -DENGINE_TYPE=1 -DENABLE_ASAN=ON -DCMAKE_INSTALL_PREFIX=/opt/engine3 -DARCH=64 ../../ && ${MAKE}
	rm -rf build/unix
	mkdir -p build/unix
	cd build/unix && cmake -DENGINE_TYPE=1 -DENABLE_TSAN=ON -DCMAKE_INSTALL_PREFIX=/opt/engine3 -DARCH=64 ../../ && ${MAKE}
	rm -rf build/unix
	mkdir -p build/unix
	cd build/unix && cmake -DENGINE_TYPE=1 -DENABLE_UBSAN=ON -DCMAKE_INSTALL_PREFIX=/opt/engine3 -DARCH=64 ../../ && ${MAKE}
	rm -rf build/unix
	mkdir -p build/unix
	cd build/unix && cmake -DENGINE_TYPE=1 -DCMAKE_INSTALL_PREFIX=/opt/engine3 -DARCH=64 ../../ && ${MAKE}

build-ninja:
	mkdir -p build/unix/ninja
	cd build/unix/ninja && cmake -G Ninja -DENGINE_TYPE=1 -DCMAKE_INSTALL_PREFIX=/opt/engine3 -DARCH=64 ../../../ && ninja
	#done

clean-ninja:
	rm -rf build/unix/ninja
	#done

build-cmake-pub:
	rm -rf build/unix
	mkdir -p build/unix
	${MAKE} build-cmake-pub-asan
	${MAKE} build-cmake-pub-tsan
	${MAKE} build-cmake-pub-ubsan
	cd build/unix && cmake -DENGINE_TYPE=2 -DCMAKE_INSTALL_PREFIX=/opt/engine3pub -DARCH=64 ../../ && ${MAKE}

build-cmake-pub-asan:
	rm -rf build/unix
	mkdir -p build/unix
	cd build/unix && cmake -DENGINE_TYPE=2 -DENABLE_ASAN=ON -DCMAKE_CXX_COMPILER=clang++-7 -DCMAKE_C_COMPILER=clang-7 -DCMAKE_INSTALL_PREFIX=/opt/engine3pub -DARCH=64 ../../ && ${MAKE}

build-cmake-pub-tsan:
	rm -rf build/unix
	mkdir -p build/unix
	cd build/unix && cmake -DENGINE_TYPE=2 -DENABLE_TSAN=ON -DCMAKE_CXX_COMPILER=clang++-7 -DCMAKE_C_COMPILER=clang-7 -DCMAKE_INSTALL_PREFIX=/opt/engine3pub -DARCH=64 ../../ && ${MAKE}

build-cmake-pub-ubsan:
	rm -rf build/unix
	mkdir -p build/unix
	cd build/unix && cmake -DENGINE_TYPE=2 -DENABLE_UBSAN=ON -DCMAKE_CXX_COMPILER=clang++-7 -DCMAKE_C_COMPILER=clang-7 -DCMAKE_INSTALL_PREFIX=/opt/engine3pub -DARCH=64 ../../ && ${MAKE}

build-clang-pub:
	cd build/unix && cmake -DCMAKE_CXX_COMPILER=clang++-7 -DCMAKE_C_COMPILER=clang-7 -DENGINE_TYPE=2 -DCMAKE_INSTALL_PREFIX=/opt/engine3pub -DARCH=64 ../../ && ${MAKE}

install-cmake: build-cmake
	cd build/unix && make install

install-cmake-pub:
	cd build/unix && make install/strip
	cd build/unix32 && make install/strip

clean-cmake: recompile-idlc
	cd build/unix && make clean
	cd build/unix32 && make clean

