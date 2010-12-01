/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "events/BaseClientNetStatusCheckupEvent.h"

#include "BaseClientProxy.h"

BaseClientProxy::BaseClientProxy() : BaseClient() {
	bufferedPacket = NULL;
	receiveBuffer.setInsertPlan(SortedVector<BasePacket*>::NO_DUPLICATE);
}

BaseClientProxy::BaseClientProxy(Socket* sock, SocketAddress& addr) : BaseClient(sock, addr) {
	bufferedPacket = NULL;
	receiveBuffer.setInsertPlan(SortedVector<BasePacket*>::NO_DUPLICATE);

	//setLockName("ClientProxy " + ip);
   	//setMutexLogging(false);

   	//setFileLogger("log/" + addr.getFullPrintableIPAddress());
}

void BaseClientProxy::init(DatagramServiceThread* serv) {
	crcSeed = System::random();

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
}
