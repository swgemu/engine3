/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "events/BaseClientNetStatusCheckupEvent.h"

#include "BaseClientProxy.h"

BaseClientProxy::BaseClientProxy(Socket* sock, SocketAddress& addr) : BaseClient(sock, addr) {
	scheduler = NULL;

	bufferedPacket = NULL;
	receiveBuffer.setInsertPlan(SortedVector<BasePacket*>::NO_DUPLICATE);

	setKeeping(true);

	//setLockName("ClientProxy " + ip);
   	//setMutexLogging(false);
   	
   	//setFileLogger("log/" + addr.getFullPrintableIPAddress());
}

void BaseClientProxy::init(DatagramServiceThread* serv) {
	crcSeed = System::random();

	serverSequence = 0;
   	clientSequence = 0;

    hasError = false;
   	disconnected = false;
   	clientDisconnected = false;
   	
   	acknowledgedServerSequence = -1;
	realServerSequence = 0;
	resentPackets = 0;

	service = serv;
	scheduler = service->getScheduler();

	checkupEvent = new BasePacketChekupEvent(this);
	netcheckupEvent = new BaseClientNetStatusCheckupEvent(this);

   	lastNetStatusTimeStamp.addMiliTime(NETSTATUSCHECKUP_TIMEOUT);
   	balancePacketCheckupTime();

	scheduler->addEvent(netcheckupEvent, NETSTATUSCHECKUP_TIMEOUT);
}
