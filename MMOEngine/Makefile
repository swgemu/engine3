# Copyright (C) 2007 <SWGEmu>. All rights reserved.
# Distribution of this file for usage outside of Core3 is prohibited.

IDLC = /usr/local/bin/idlc

INSTALL_PATH = /usr/local/engine3

ENGINE_PUB_PATH = ../MMOEngine-Pub

IDL_SOURCES = engine/core/ManagedObject.idl \
			  engine/core/ManagedService.idl \
			  engine/core/util/ManagedVector.idl \
			  engine/util/Observer.idl \
			  engine/util/Observable.idl \
			  engine/util/Facade.idl \
			  engine/util/u3d/QuadTreeEntry.idl

IDL_DIRECTIVES =
	
all: 
	#cd build/unix && cmake ../.. && make
	#cp build/unix/src/libengine3.so lib/unix

build: compile install-headers
	#done

all-public: 
	make all-public64 
	make all-public32

all-public32: clean configure-release32 compile install-public
	rm -f $(ENGINE_PUB_PATH)/lib/linux32/libengine3.a
	cp lib/unix/libengine3.a $(ENGINE_PUB_PATH)/lib/linux32

all-public64: clean configure-release64 compile install-public
	rm -f $(ENGINE_PUB_PATH)/lib/linux64/libengine3.a
	cp lib/unix/libengine3.a $(ENGINE_PUB_PATH)/lib/linux64

rebuild: clean configure-debug compile install-headers
	#done

config:
	autoreconf --force --install
	#done

configure-release32:
	cd build/unix && ../../configure --with-cross-host i686-linux

configure-release64:
	cd build/unix && ../../configure --with-cross-host amd64-linux

configure-debug:
	cd build/unix && ../../configure --enable-debug

compile: compile-idlc compile-engine install-lib

compile-idlc:
	$(IDLC) $(IDL_DIRECTIVES) -sd src $(IDL_SOURCES)

recompile-idlc:
	$(IDLC) -rb $(IDL_DIRECTIVES) -sd src $(IDL_SOURCES)
	
compile-engine:
	cd build/unix && make

modules:
	cd external/multimmap && make -C /usr/src/linux-headers-`uname -r` M=`pwd` modules
	cp external/multimmap/multimmap.ko bin/
	#rmmod multimmap
	#insmod bin/multimmap.ko
	
clean: recompile-idlc
	cd build/unix && make clean
	#done

clean-distribution: recompile-idlc
	cd build/unix && make distclean
	#done

install:
	mkdir -p ${INSTALL_PATH}/lib
	cp bin/idlc /usr/local/bin
	build/unix/install ${INSTALL_PATH} #> /dev/null 2>&1

install-headers:	
	#build/unix/install . > /dev/null 2>&1

install-lib:
	cp build/unix/src/libengine3.a lib/unix
	cp build/unix/src/testsuite3/testsuite3* bin
	rm build/unix/src/testsuite3/testsuite3*	

install-public:	
	strip --strip-debug lib/unix/libengine3.a
	build/unix/install $(ENGINE_PUB_PATH) -u > /dev/null
	rm -f $(ENGINE_PUB_PATH)/lib/idlc.jar
	rm -f $(ENGINE_PUB_PATH)/lib/libengine3.a

pch:
	cd src && gcc -c $(CXXFLAGS) $(INCLUDES) -o system/lang.h.gch system/lang.h
	cd src && gcc -c $(CXXFLAGS) $(INCLUDES) -o engine/engine.h.gch engine/engine.h
	