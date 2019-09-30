/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "events/BaseClientNetStatusCheckupEvent.h"
#include "events/BaseClientEvent.h"

#include "BaseClientProxy.h"

BaseClientProxy::BaseClientProxy() : BaseClient() {
	bufferedPacket = nullptr;
	receiveBuffer.setInsertPlan(SortedVector<BasePacket*>::NO_DUPLICATE);
}

BaseClientProxy::BaseClientProxy(Socket* sock, SocketAddress& addr) : BaseClient(sock, addr) {
	bufferedPacket = nullptr;
	receiveBuffer.setInsertPlan(SortedVector<BasePacket*>::NO_DUPLICATE);

	//setLockName("ClientProxy " + ip);
   	//setMutexLogging(false);

   	//setFileLogger("log/" + addr.getFullPrintableIPAddress());
}

void BaseClientProxy::init(DatagramServiceThread* serv) {
	crcSeed = System::random();

	//info("crc seed:0x" + String::hexvalueOf((int)crcSeed), true);

	serverSequence = 0;
   	clientSequence = 0;

   	disconnected = false;
   	clientDisconnected = false;

   	acknowledgedServerSequence = -1;
	realServerSequence = 0;
	resentPackets = 0;

	service = serv;

	checkupEvent = new BasePacketChekupEvent(this);
	netcheckupEvent = new BaseClientNetStatusCheckupEvent(this);

   	lastNetStatusTimeStamp.addMiliTime(NETSTATUSCHECKUP_TIMEOUT);
   	balancePacketCheckupTime();

	netcheckupEvent->schedule(NETSTATUSCHECKUP_TIMEOUT);

#ifdef LOCKFREE_BCLIENT_BUFFERS
	reentrantTask->scheduleInIoScheduler(10);
#endif
}
