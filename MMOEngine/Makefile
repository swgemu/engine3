# Copyright (C) 2007 <SWGEmu>. All rights reserved.
# Distribution of this file for usage outside of Core3 is prohibited.

IDLC = /usr/local/bin/idlc

IDL_SOURCES = engine/core/ManagedObject.idl \
			  engine/core/util/ManagedVector.idl
	
all: compile inst
	#done

allpub: config clean compile instpub
	#done

compile:
	cd src && $(IDLC) $(IDL_SOURCES)
	cd build/unix && ../../configure && make
	cp build/unix/src/libengine3.a lib/unix

config:
	autoreconf --force
	#done

clean: cleanidl
	cd src && $(IDLC) -rebuild $(IDL_SOURCES)
	cd build/unix && make clean
	#done

cleanidl:
	rm -f src/engine/core/ManagedObject.h
	rm -f src/engine/core/ManagedObject.cpp
	rm -f src/engine/core/util/ManagedVector.h
	rm -f src/engine/core/util/ManagedVector.cpp

inst:	
	build/unix/install . > /dev/null 2>&1

instpub:
	build/unix/install ../../swgemupub/MMOEngine -u > /dev/null
	strip --strip-debug lib/unix/libengine3.a
	cp lib/unix/libengine3.a ../../swgemupub/MMOEngine/lib/unix

pch:
	cd src && gcc -c $(CXXFLAGS) $(INCLUDES) -o system/lang.h.gch system/lang.h
	cd src && gcc -c $(CXXFLAGS) $(INCLUDES) -o engine/engine.h.gch engine/engine.h
	 	
rebuild: clean all
	