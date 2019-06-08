# Copyright (C) 2007-2019 SWGEmu
# See file COPYING for copying conditions.

if DEBUG
AM_CXXFLAGS =-ggdb -Wall -Wextra -Wno-unused -Wno-unused-parameter -Wno-overloaded-virtual
else
AM_CXXFLAGS =-g0 -Wall -Wextra -Wno-unused -Wno-unused-parameter -Wno-overloaded-virtual -DVERSION_PUBLIC
endif

#CC = /Users/victor/Documents/addresssanitizer/asan/asan_clang_Darwin/bin/clang
#CXX = /Users/victor/Documents/addresssanitizer/asan/asan_clang_Darwin/bin/clang++

#CC = clang
#CXX = clang++

INCLUDES =  $(SYSTEM_INCLUDES) \
	    	-I$(USER_INCLUDES_PATH)/mysql5/mysql -I$(USER_INCLUDES_PATH)/mysql -I$(USER_INCLUDES_PATH)/lua -I/usr/include/mysql \
	    	-I/usr/local/BerkeleyDB.5.0/include -I$(USER_INCLUDES_PATH) \
		-I../../../external \
	    	-I../../../external/dlmalloc \
	    	-I../../../external/multimmap

LIBS =  $(SYSTEM_LIBS) \
		-L/usr/local/BerkeleyDB.5.0/lib \
		-lmysqlclient -llua -lz -ldb \
		-lengine3

noinst_LIBRARIES = libengine3.a

libengine3_a_SOURCES = \
			system/lang.h \
			system/lang/System.cpp \
			system/lang/Exception.cpp \
			system/lang/Object.cpp \
			system/lang/String.cpp \
			system/lang/StringBuffer.cpp \
			system/lang/UnicodeString.cpp \
			system/lang/Math.cpp \
			system/lang/StackTrace.cpp \
			system/lang/types.cpp \
			system/lang/Integer.cpp \
			system/lang/ArrayIndexOutOfBoundsException.cpp \
			system/mm/MemoryManager.cpp \
			system/mm/AllocationHook.cpp \
			system/mm/AllocationReplacement.cpp \
			system/mm/AllocationTracker.cpp \
			system/mm/MultimapMemoryManager.cpp \
			system/mm/Heap.cpp \
			system/mm/ProtectedHeap.cpp \
			system/mm/ProcessHeap.cpp \
			system/mm/MemoryMappedFile.cpp \
			system/mm/DLAllocator.cpp \
			system/thread/Thread.cpp \
			system/thread/Process.cpp \
			system/thread/ChildProcess.cpp \
			system/thread/Lockable.cpp \
			system/thread/Mutex.cpp \
			system/thread/ReadWriteLock.cpp \
			system/thread/gdb/GdbStub.cpp \
			system/net/Socket.cpp \
			system/net/SocketAddress.cpp \
			system/net/InetAddress.cpp \
			system/net/NetworkInterface.cpp \
			system/net/Packet.cpp \
			system/io/IOProcessor.cpp \
			system/io/Stream.cpp \
			system/io/File.cpp \
			system/io/FileDescriptor.cpp \
			system/io/BufferedReader.cpp \
			system/io/StringTokenizer.cpp \
			system/io/UnicodeTokenizer.cpp \
			system/io/PrintStream.cpp \
			system/io/Serializable.cpp \
			system/io/Pipe.cpp \
			system/util/PriorityQueue.cpp \
			engine/engine.h \
			engine/core/Core.cpp \
			engine/core/Task.cpp \
			engine/core/TaskManagerImpl.cpp \
			engine/core/TaskScheduler.cpp \
			engine/core/TaskQueue.cpp \
			engine/core/TimedTaskQueue.cpp \
			engine/core/TaskWorkerThread.cpp \
			engine/core/ObjectUpdateToDatabaseTask.cpp \
			engine/core/ManagedObject.cpp \
			engine/core/ManagedService.cpp \
			engine/core/ManagedObjectImplementation.cpp \
			engine/core/ManagedProcess.cpp \
			engine/service/ServiceClient.cpp \
			engine/service/ServiceThread.cpp \
			engine/service/ServiceMessageHandlerThread.cpp \
			engine/service/DatagramServiceClient.cpp \
			engine/service/DatagramServiceClientProxy.cpp \
			engine/service/DatagramServiceThread.cpp \
			engine/service/StreamServiceClient.cpp \
			engine/service/StreamServiceThread.cpp \
			engine/service/MessageQueue.cpp \
			engine/service/proto/BaseProtocol.cpp \
			engine/service/proto/BasePacket.cpp \
			engine/service/proto/BaseMultiPacket.cpp \
			engine/service/proto/BaseFragmentedPacket.cpp \
			engine/service/proto/BasePacketHandler.cpp \
			engine/service/proto/BaseClient.cpp \
			engine/service/proto/BaseClientProxy.cpp \
			engine/service/proto/events/BasePacketChekupEvent.cpp \
			engine/orb/DistributedObjectBroker.cpp \
			engine/orb/NamingDirectoryService.cpp \
			engine/orb/db/DistributedObjectDirectory.cpp \
			engine/orb/db/DOBObjectManager.cpp \
			engine/orb/db/CommitMasterTransactionThread.cpp \
			engine/orb/db/UpdateModifiedObjectsThread.cpp \
			engine/orb/object/DistributedObject.cpp \
			engine/orb/object/DistributedObjectServant.cpp \
			engine/orb/object/DistributedObjectStub.cpp \
			engine/orb/object/DistributedMethod.cpp \
			engine/orb/messages/RemoteObjectBroker.cpp \
			engine/orb/messages/DOBServiceClient.cpp \
			engine/orb/messages/DOBMessageFactory.cpp \
			engine/orb/control/ObjectBrokerDirector.cpp \
			engine/orb/control/ObjectBrokerAgent.cpp \
			engine/stm/Transaction.cpp \
			engine/stm/TransactionalMemoryManager.cpp \
			engine/stm/mm/KernelCall.cpp \
			engine/stm/task/TransactionalTaskManager.cpp \
			engine/stm/task/LocalTaskManager.cpp \
			engine/stm/orb/TransactionalObjectManager.cpp \
			engine/stm/orb/LocalObjectManager.cpp \
			engine/stm/service/TransactionalSocketManager.cpp \
			engine/stm/service/TransactionalBaseClientManager.cpp \
			engine/stm/algos/FraserSTM.cpp \
			engine/util/Observer.cpp \
			engine/util/ObserverImplementation.cpp \
			engine/util/ObserverEventMap.cpp \
			engine/util/Observable.cpp \
.			engine/util/Facade.cpp \
			engine/util/u3d/QuadTree.cpp \
			engine/util/u3d/BasicQuadTree.cpp \
			engine/util/u3d/Segment.cpp \
			engine/util/u3d/QuadTreeEntry.cpp \
			engine/util/u3d/QuadTreeEntryImplementation.cpp \
			engine/util/u3d/TriangulationAStarAlgorithm.cpp \
			engine/util/u3d/AABBTree.cpp \
			engine/util/u3d/Sphere.cpp \
			engine/util/u3d/Triangle.cpp \
			engine/util/u3d/Matrix4.cpp \
			engine/util/u3d/Funnel.cpp \
			engine/util/iffstream/IffStream.cpp \
			engine/util/iffstream/chunks/Chunk.cpp \
			engine/util/iffstream/chunks/form/Form.cpp \
			engine/db/BinaryData.cpp \
			engine/db/ObjectDatabase.cpp \
			engine/db/DatabaseManager.cpp \
			engine/db/ObjectDatabaseManager.cpp \
			engine/db/LocalDatabase.cpp \
			engine/db/mysql/MySqlDatabase.cpp \
			engine/db/mysql/MysqlDatabaseManager.cpp \
			engine/db/berkley/BerkeleyDatabase.cpp \
			engine/db/berkley/BTransaction.cpp \
			engine/db/berkley/DatabaseEntry.cpp \
			engine/db/berkley/Environment.cpp \
			engine/db/berkley/Cursor.cpp \
			engine/log/Logger.cpp \
			engine/lua/Lua.cpp \
			engine/lua/LuaFunction.cpp \
			engine/lua/LuaObject.cpp

.h.gch:
	$(CXXCOMPILE) $<
			
