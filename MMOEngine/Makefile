# Copyright (C) 2007 <SWGEmu>. All rights reserved.
# Distribution of this file for usage outside of Core3 is prohibited.

IDLC = /usr/local/bin/idlc

ENGINE_PATH = ../../core3-pub/MMOEngine

IDL_SOURCES = engine/core/ManagedObject.idl \
			  engine/core/util/ManagedVector.idl
	
all: compile install-headers
	#done

all-public: clean configure-release compile install-public
	#done

rebuild: clean configure-debug compile install-headers
	#done

config:
	autoreconf --force
	#done

configure-release:
	cd build/unix && ../../configure

configure-debug:
	cd build/unix && ../../configure --enable-debug

compile: compile-idlc compile-engine install-lib

compile-idlc:
	$(IDLC) -sd src $(IDL_SOURCES)

recompile-idlc:
	$(IDLC) -rb -sd src $(IDL_SOURCES)
	
compile-engine:
	cd build/unix && make

clean: recompile-idlc
	cd build/unix && make clean
	#done

clean-distribution: recompile-idlc
	cd build/unix && make distclean
	#done

install:
	cp bin/idlc /usr/local/bin
	cp bin/idlc.jar /usr/local/lib
	#cp lib/unix/libengine3.a /usr/local/lib	

install-headers:	
	build/unix/install . > /dev/null 2>&1

install-lib:
	cp build/unix/src/libengine3.a lib/unix

install-public:
	build/unix/install $(ENGINE_PATH) -u > /dev/null
	strip --strip-debug lib/unix/libengine3.a
	cp lib/unix/libengine3.a $(ENGINE_PATH)/lib/unix
	cp bin/idlc.jar $(ENGINE_PATH)/bin

pch:
	cd src && gcc -c $(CXXFLAGS) $(INCLUDES) -o system/lang.h.gch system/lang.h
	cd src && gcc -c $(CXXFLAGS) $(INCLUDES) -o engine/engine.h.gch engine/engine.h
	