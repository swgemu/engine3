/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#pragma once

#include "system/lang.h"

#include "core/Core.h"

#include "core/Task.h"
#include "core/TaskManager.h"

#include "core/ManagedReference.h"
#include "core/ManagedWeakReference.h"
#include "core/ManagedObject.h"

//#include "core/util/ManagedVector.h"
//#include "core/util/ManagedVectorImplementation.h"
#include "core/util/ManagedSortedVector.h"
#include "core/util/ManagedVectorMap.h"

#include "log/Logger.h"

#include "service/ServiceException.h"
#include "service/ServiceClient.h"
#include "service/ServiceClientMap.h"
#include "service/DatagramServiceClient.h"
#include "service/DatagramServiceClientProxy.h"
#include "service/DatagramServiceThread.h"
#include "service/DatagramServiceThread.h"
#include "service/StreamServiceClient.h"
#include "service/StreamServiceClientProxy.h"
#include "service/StreamServiceThread.h"

#include "service/Message.h"

#include "service/proto/BaseClient.h"
#include "service/proto/BaseClientProxy.h"
#include "service/proto/BasePacket.h"
#include "service/proto/BasePacketHandler.h"
#include "service/proto/BaseMessage.h"
#include "service/proto/StandaloneBaseMessage.h"

#include "service/proto/packets/PingMessage.h"
#include "service/proto/packets/NetStatusResponseMessage.h"
#include "service/proto/packets/NetStatusRequestMessage.h"
#include "service/proto/packets/AcknowledgeMessage.h"
#include "service/proto/packets/AcknowledgeOkMessage.h"
#include "service/proto/packets/ConnectionServerMessage.h"
#include "service/proto/packets/LoginServerMessage.h"

#include "orb/DistributedObjectBroker.h"
#include "orb/db/DOBObjectManager.h"

#include "stm/TransactionalMemoryManager.h"

#include "db/Database.h"
#include "db/ObjectDatabase.h"
#include "db/ObjectDatabaseManager.h"
#include "db/BinaryData.h"
#include "db/berkeley/BerkeleyDatabase.h"

#include "lua/Lua.h"

#include "util/u3d/Quaternion.h"
#include "util/u3d/Vector3.h"
#include "util/u3d/AABBTree.h"
#include "util/u3d/AStarAlgorithm.h"
#include "util/u3d/TriangulationAStarAlgorithm.h"
#include "util/u3d/Funnel.h"
#include "util/iffstream/IffStream.h"

