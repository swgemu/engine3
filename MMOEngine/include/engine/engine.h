/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ENGINE_H_
#define ENGINE_H_

#include "../system/lang.h"

#include "log/Logger.h"

#include "sched/Event.h"
#include "sched/ScheduleManager.h"

#include "service/ServiceException.h"
#include "service/ServiceProcessThread.h"
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

#include "service/proxy/StreamProxyClient.h"
#include "service/proxy/StreamProxyServiceClient.h"
#include "service/proxy/StreamProxyService.h"
#include "service/proxy/StreamProxyServiceMap.h"
#include "service/proxy/DatagramProxyServiceClient.h"
#include "service/proxy/DatagramProxyClient.h"
#include "service/proxy/DatagramProxyService.h"
#include "service/proxy/DatagramProxyServiceMap.h"
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

#include "core/Core.h"

#include "core/ManagedReference.h"
#include "core/ManagedObject.h"
#include "core/ManagedObjectImplementation.h"

//#include "core/util/ManagedVector.h"
//#include "core/util/ManagedVectorImplementation.h"
#include "core/util/ManagedSortedVector.h"
#include "core/util/ManagedVectorMap.h"

#ifndef PLATFORM_WIN
#include "db/Database.h"
#include "db/MySqlDatabase.h"

#include "lua/Lua.h"
#endif

#include "util/QuadTree.h"

#endif /*ENGINE_H_*/
