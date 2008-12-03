# Copyright (C) 2007 <SWGEmu>. All rights reserved.
# Distribution of this file for usage outside of Core3 is prohibited.

AM_CXXFLAGS =-ggdb -Wall -Wextra -Wno-unused

INCLUDES =  $(SYSTEM_INCLUDES) \
	    	-I$(USER_INCLUDES_PATH)/mysql -I$(USER_INCLUDES_PATH)/lua -I$src

LIBS =  $(SYSTEM_LIBS) \
		-lmysqlclient -llua -lz \
		-lengine3

noinst_LIBRARIES = libengine3.a

libengine3_a_SOURCES = \
			system/lang.h \
			system/lang/System.cpp \
			system/lang/Exception.cpp \
			system/lang/String.cpp \
			system/lang/StringBuffer.cpp \
			system/lang/UnicodeString.cpp \
			system/lang/Math.cpp \
			system/lang/StackTrace.cpp \
			system/lang/ArrayIndexOutOfBoundsException.cpp \
			system/thread/Thread.cpp \
			system/thread/ReadWriteLock.cpp \
			system/net/Socket.cpp \
			system/net/SocketAddress.cpp \
			system/net/InetAddress.cpp \
			system/net/NetworkInterface.cpp \
			system/net/Packet.cpp \
			system/io/Stream.cpp \
			system/io/File.cpp \
			system/io/BufferedReader.cpp \
			system/io/StringTokenizer.cpp \
			system/io/UnicodeTokenizer.cpp \
			system/io/PrintStream.cpp \
			system/util/AdvancedBinaryHeap.cpp \
			engine/engine.h \
			engine/sched/ScheduleManager.cpp \
			engine/sched/EventQueue.cpp \
			engine/service/ServiceClient.cpp \
			engine/service/ServiceThread.cpp \
			engine/service/ServiceMessageHandlerThread.cpp \
			engine/service/ServiceProcessThread.cpp \
			engine/service/DatagramServiceClient.cpp \
			engine/service/DatagramServiceClientProxy.cpp \
			engine/service/DatagramServiceThread.cpp \
			engine/service/StreamServiceClient.cpp \
			engine/service/StreamServiceThread.cpp \
			engine/service/MessageQueue.cpp \
			engine/service/proxy/DatagramProxyClient.cpp \
			engine/service/proxy/DatagramProxyService.cpp \
			engine/service/proxy/DatagramProxyServiceClient.cpp \
			engine/service/proxy/StreamProxyClient.cpp \
			engine/service/proxy/StreamProxyService.cpp \
			engine/service/proxy/StreamProxyServiceClient.cpp \
			engine/service/proto/BaseProtocol.cpp \
			engine/service/proto/BasePacket.cpp \
			engine/service/proto/BaseMultiPacket.cpp \
			engine/service/proto/BaseFragmentedPacket.cpp \
			engine/service/proto/BasePacketHandler.cpp \
			engine/service/proto/BaseClient.cpp \
			engine/service/proto/BaseClientProxy.cpp \
			engine/service/proto/events/BasePacketChekupEvent.cpp \
			engine/orb/DistributedObjectBroker.cpp \
			engine/orb/DistributedObjectDirectory.cpp \
			engine/orb/NamingDirectoryService.cpp \
			engine/orb/NamingDirectoryServiceImpl.cpp \
			engine/orb/DistributedObjectBrokerClient.cpp \
			engine/orb/DOBPacketHandler.cpp \
			engine/orb/object/DistributedObject.cpp \
			engine/orb/object/DistributedObjectServant.cpp \
			engine/orb/object/DistributedObjectStub.cpp \
			engine/orb/object/DistributedMethod.cpp \
			engine/core/ManagedObject.cpp \
			engine/core/ManagedObjectImplementation.cpp \
			engine/util/QuadTree.cpp \
			engine/db/MySqlDatabase.cpp \
			engine/db/BinaryData.cpp \
			engine/log/Logger.cpp \
			engine/lua/Lua.cpp

.h.gch:
	$(CXXCOMPILE) $<
			