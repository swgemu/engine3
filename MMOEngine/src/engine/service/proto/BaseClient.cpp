/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

//#define TRACE_CLIENTS

#include "BaseClient.h"

#include "events/BaseClientNetStatusCheckupEvent.h"
#include "events/BaseClientCleanupEvent.h"

#include "packets/SessionIDRequestMessage.h"
#include "packets/SessionIDResponseMessage.h"
#include "packets/AcknowledgeMessage.h"
#include "packets/OutOfOrderMessage.h"
#include "packets/DisconnectMessage.h"

BaseClient::BaseClient(const String& addr, int port) : DatagramServiceClient(addr, port),
		BaseProtocol(),	Event(), Mutex("Client") {
	scheduler = NULL;

	bufferedPacket = NULL;
	receiveBuffer.setInsertPlan(SortedVector<BasePacket*>::NO_DUPLICATE);

	fragmentedPacket = NULL;

	setKeeping(true);

	setLogging(true);
   	setGlobalLogging(true);
}

BaseClient::BaseClient(Socket* sock, SocketAddress& addr) : DatagramServiceClient(sock, addr),
		BaseProtocol(),	Event(), Mutex("Client") {
	scheduler = NULL;

	bufferedPacket = NULL;

	fragmentedPacket = NULL;

  	ip = addr.getFullIPAddress();
   	setLockName("Client " + ip);
   	//setMutexLogging(false);

   	/*String prip = addr.getFullPrintableIPAddress();
   	setFileLogger("log/" + prip);*/

   	setLogging(true);
   	setGlobalLogging(true);
}

BaseClient::~BaseClient() {
	service->deleteConnection(this);

	if (checkupEvent != NULL) {
		if (checkupEvent->isQueued())
			scheduler->deleteEvent(checkupEvent);

		delete checkupEvent;
		checkupEvent = NULL;
	}

	if (netcheckupEvent != NULL) {
		if (netcheckupEvent->isQueued())
			scheduler->deleteEvent(netcheckupEvent);

		delete netcheckupEvent;
		netcheckupEvent = NULL;
	}

	info("deleted");
}

void BaseClient::init(ScheduleManager* sched) {
	crcSeed = 0;

	serverSequence = 0;
   	clientSequence = 0;

    hasError = false;
   	disconnected = false;
   	acknowledgedServerSequence = -1;
	realServerSequence = 0;
	resentPackets = 0;

	scheduler = sched;

	checkupEvent = new BasePacketChekupEvent(this);
	netcheckupEvent = new BaseClientNetStatusCheckupEvent(this);

   	lastNetStatusTimeStamp.addMiliTime(NETSTATUSCHECKUP_TIMEOUT);
   	balancePacketCheckupTime();

	scheduler->addEvent(netcheckupEvent, NETSTATUSCHECKUP_TIMEOUT);
}

void BaseClient::close() {
	if (scheduler != NULL) {
		scheduler->deleteEvent(this);
		scheduler->deleteEvent(checkupEvent);
		scheduler->deleteEvent(netcheckupEvent);
	}

	scheduler->addEvent(new BaseClientCleanupEvent(this));

	for (int i = 0; i < sendBuffer.size(); ++i) {
		BasePacket* pack = sendBuffer.get(i);
		delete pack;
	}

	sendBuffer.removeAll();

	for (int i = 0; i < sequenceBuffer.size(); ++i) {
		BasePacket* pack = sequenceBuffer.get(i);
		delete pack;
	}

	sequenceBuffer.removeAll();

	if (fragmentedPacket != NULL) {
		delete fragmentedPacket;
		fragmentedPacket = NULL;
	}

	//serverSequence = 0;
	clientSequence = 0;

	acknowledgedServerSequence = -1;

	disconnected = true;

	reportStats();

	closeFileLogger();
}

void BaseClient::send(Packet* pack, bool doLock) {
	lock(doLock);

	try {
		if (isAvailable()) {
			#ifdef TRACE_CLIENTS
				info("SEND(RAW) - " + pack->toString());
			#endif

			if (!DatagramServiceClient::send(pack))
				info("LOSING " + pack->toString());
		}
	} catch (SocketException& e) {
		error("on send()" + e.getMessage());

		hasError = true;
		disconnect(false);
	}

	delete pack;

	unlock(doLock);
}

void BaseClient::sendPacket(BasePacket* pack, bool doLock) {
	lock(doLock);

	if (!isAvailable()) {
		delete pack;

		unlock(doLock);
		return;
	}

	/*#ifdef TRACE_CLIENTS
		info("preapare SEND " + pack->toString());
	#endif*/

	try {
		if (pack->doSequencing())
			if (pack->size() >= 490) {
				if (bufferedPacket != NULL) {
					sendSequenced(bufferedPacket->getPacket());
					bufferedPacket = NULL;
				}

				sendFragmented(pack);
			} else
				bufferMultiPacket(pack);
		else
			sendSequenceLess(pack);
	} catch (...) {
		disconnect("unreported exception on sendPacket()");
	}

	unlock(doLock);
}

void BaseClient::bufferMultiPacket(BasePacket* pack) {
	if (bufferedPacket != NULL) {
		if (pack->isDataChannelPacket() && !pack->isMultiPacket()) {
			if (!bufferedPacket->add(pack)) {
				sendSequenced(bufferedPacket->getPacket());

				bufferedPacket = new BaseMultiPacket(pack);
			}
		} else {
			sendSequenced(bufferedPacket->getPacket());
			bufferedPacket = NULL;

			sendSequenced(pack);
		}
	} else {
		bufferedPacket = new BaseMultiPacket(pack);

		if (!isQueued())
			scheduler->addEvent(this, 10);
	}
}

void BaseClient::sendSequenceLess(BasePacket* pack) {
	try {
		#ifdef TRACE_CLIENTS
			info("SEND(NOSEQ) - " + pack->toString());
		#endif

		prepareSend(pack);
		if (!DatagramServiceClient::send(pack))
			info("LOSING " + pack->toString());

		delete pack;
	} catch (SocketException& e) {
		delete pack;

		disconnect("on sendPacket()" + e.getMessage());
	}
}

void BaseClient::sendSequenced(BasePacket* pack) {
	if (!isAvailable())
		return;

	try {
		pack->setTimeout(checkupEvent->getCheckupTime());
		sendBuffer.add(pack);

		if (!isQueued())
			scheduler->addEvent(this, 10);
	} catch (SocketException& e) {
		disconnect("sending packet");
	} catch (ArrayIndexOutOfBoundsException& e) {
		error("on sendQueued() - " + e.getMessage());
	}

	/*#ifdef TRACE_CLIENTS
		StringBuffer msg;
		msg << "SEND(" << pack->getSequence() << ")" + pack->toString());
		info(msg);
	#endif*/
}

void BaseClient::sendFragmented(BasePacket* pack) {
	if (!isAvailable())
		return;

	try {
		BaseFragmentedPacket* frag = new BaseFragmentedPacket(pack);

		while (frag->hasFragments())
			sendSequenced(frag->getFragment());

		delete frag;
	} catch (SocketException& e) {
		disconnect("sending packet");
	} catch (ArrayIndexOutOfBoundsException& e) {
		error("on sendFragmented() - " + e.getMessage());
	}
}

bool BaseClient::activate() {
	lock();

	try {
		if (isAvailable()) {
			BasePacket* pack = getNextSequencedPacket();
			if (pack == NULL) {
				unlock();
				return false;
			}

			if (sequenceBuffer.isEmpty()) {
				checkupEvent->update(pack);
				pack->setTimeout(checkupEvent->getCheckupTime());

				scheduler->addEvent(checkupEvent, pack->getTimeout());
			}

			#ifdef TRACE_CLIENTS
				StringBuffer msg;
				msg << "SEND(" << serverSequence << ") - " << pack->toString();
				info(msg);
			#endif

			pack->setTimestamp();

			prepareSend(pack);
			if (pack->getSequence() != (uint32) realServerSequence++) {
				StringBuffer msg;
				msg << "invalid server Packet " << pack->getSequence() << " sent (" << realServerSequence - 1 << ")";
				error(msg);
			}

			sequenceBuffer.add(pack);
			if (!DatagramServiceClient::send(pack)) {
				StringBuffer msg;
				msg << "LOSING (" << pack->getSequence() << ") " /*<< pack->toString()*/;
				info(msg);
			}

			if (!sendBuffer.isEmpty() || bufferedPacket != NULL) {
				scheduler->addEvent(this, 10);
			}
		}
	} catch (SocketException& e) {
		disconnect("on activate() - " + e.getMessage());
	} catch (...) {
		disconnect("unreported exception on activate()");
	}

	unlock();
	return true;
}

BasePacket* BaseClient::getNextSequencedPacket() {
	BasePacket* pack = NULL;

	/*#ifdef TRACE_CLIENTS
		StringBuffer msg;
		msg << "SEQ = " << serverSequence << ", ACKSEQ = " << acknowledgedServerSequence << ", BUFFSIZE = "
			<< sendBuffer.size();
		info(msg);
	#endif*/

	if (serverSequence - acknowledgedServerSequence > 25) {
		if (!sendBuffer.isEmpty() || bufferedPacket != NULL)
			scheduler->addEvent(this, 10);

		if (sendBuffer.size() > 3000) {
			StringBuffer msg;
			msg << "WARNING - send buffer overload [" << sendBuffer.size() << "]";
			error(msg);

			disconnect(false);
		}

		return NULL;
	} else if (!sendBuffer.isEmpty()) {
		pack = sendBuffer.remove(0);
	} else if (bufferedPacket != NULL) {
		pack = bufferedPacket->getPacket();
		pack->setTimeout(checkupEvent->getCheckupTime());

		bufferedPacket = NULL;
	} else
		return NULL;

	return pack;
}

bool BaseClient::validatePacket(Packet* pack) {
	uint16 seq = pack->parseNetShort();

	if (seq < clientSequence) {
		acknowledgeClientPackets(seq);

		return false;
	} else if (seq > clientSequence) {
		BasePacket* packet = new BasePacket(pack, seq);
		receiveBuffer.put(packet);

		/*StringBuffer msg3;
		msg3 << "READ buffer (";

		for (int i = 0; i < receiveBuffer.size(); ++i) {
			msg3 << receiveBuffer.get(i)->getSequence() << ", ";
		}

		info(msg3);*/

		BasePacket* oor = new OutOfOrderMessage(seq);
		sendPacket(oor);

		#ifdef TRACE_CLIENTS
			StringBuffer msg;
   			msg << "OUT of order READ(" << seq << ") expected " << clientSequence;
			info(msg);
		#endif

		return false;
	} /*else
		throw Exception("received same packet sequence");*/

	acknowledgeClientPackets(clientSequence++);

	#ifdef TRACE_CLIENTS
		StringBuffer msg;
		msg  << "READ(" << seq << ") - " << pack->toString();
		info(msg);
	#endif

	return true;
}

Packet* BaseClient::getBufferedPacket() {
	if (!receiveBuffer.isEmpty()) {
		BasePacket* packet = receiveBuffer.get(0);

		uint32 packseq = packet->getSequence();
		if (packseq != clientSequence)
			return NULL;

		receiveBuffer.remove(0);

		acknowledgeClientPackets(clientSequence++);

		#ifdef TRACE_CLIENTS
			StringBuffer msg;
			msg << "BUFFERED READ(" << packseq << ")";
			info(msg);
		#endif

		return packet;
	}

	return NULL;
}

BasePacket* BaseClient::recieveFragmentedPacket(Packet* pack) {
	BasePacket* packet = NULL;

	if (fragmentedPacket == NULL)
		fragmentedPacket = new BaseFragmentedPacket();

	fragmentedPacket->addFragment(pack);

	if (fragmentedPacket->isComplete()) {
		fragmentedPacket->setOffset(2);

		//Logger::console.info("completed fragmented packet");

		packet = fragmentedPacket;
		fragmentedPacket = NULL;
	}

	return packet;
}

void BaseClient::checkupServerPackets(BasePacket* pack) {
	lock();

	try {
		if (!isAvailable() || checkupEvent->isQueued() || sequenceBuffer.size() == 0) {
			unlock();
			return;
		}

		uint32 seq = pack->getSequence();

		#ifdef TRACE_CLIENTS
			StringBuffer msg;
			msg << "CHECKING UP sequence " << seq << "[" << acknowledgedServerSequence
				<< "]";
			info(msg);
		#endif

		if (seq > (uint32) acknowledgedServerSequence) {
			resendPackets();

			checkupEvent->increaseCheckupTime(100);
			checkupEvent->update(pack);

			#ifdef TRACE_CLIENTS
				StringBuffer msg;
				msg << "checkup time incresed to " << checkupEvent->getCheckupTime();
				info(msg);
			#endif

			scheduler->addEvent(checkupEvent, pack->getTimeout());
		}
	} catch (SocketException& e) {
		disconnect("on checkupServerPackets() - " + e.getMessage());
	} catch (ArrayIndexOutOfBoundsException& e) {
		error("on checkupServerPackets() - " + e.getMessage());
	} catch (...) {
		disconnect("unreported exception on checkupServerPackets()");
	}

	unlock();
}

void BaseClient::resendPackets() {
	/*#ifdef TRACE_CLIENTS
		StringBuffer msg2;
		msg2 << "[" << seq << "] resending " << MIN(sequenceBuffer.size(), 5) << " packets to \'" << ip << "\' ["
			 << checkupEvent->getCheckupTime() << "]";
		info(msg2, true);
	#endif*/

	for (int i = 0; i < MIN(sequenceBuffer.size(), 5); ++i) {
		BasePacket* packet = sequenceBuffer.get(i);

		if (packet->getTimeout().isFuture())
			break;

		packet->setTimeout(checkupEvent->getCheckupTime());

		if (!DatagramServiceClient::send(packet)) {
			StringBuffer msg;
			msg << "LOSING on resend (" << packet->getSequence() << ") " << packet->toString();
			info(msg);
		}

		++resentPackets;

		#ifdef TRACE_CLIENTS
			StringBuffer msg;
			msg << "RESEND(" << packet->getSequence() << ") - " << packet->toString();
			info(msg);
		#endif
	}
}

void BaseClient::resendPackets(int seq) {
	lock();

	for (int i = 0; i < sequenceBuffer.size(); ++i) {
		BasePacket* packet = sequenceBuffer.get(i);

		if (packet->getSequence() == (uint32) seq) {
			sequenceBuffer.remove(i);

			delete packet;
			break;
		}

		if (packet->getTimeout().isFuture())
			continue;

		packet->setTimeout(checkupEvent->getCheckupTime());

		if (!DatagramServiceClient::send(packet)) {
			StringBuffer msg;
			msg << "LOSING on resend (" << packet->getSequence() << ") " << packet->toString();
			info(msg);
		}

		++resentPackets;
	}

	unlock();
}

void BaseClient::balancePacketCheckupTime() {
	setPacketCheckupTime(2000);

	#ifdef TRACE_CLIENTS
		StringBuffer msg;
		msg << "checkup time set to " << 2000;
		info(msg);
	#endif
}

void BaseClient::resetPacketCheckupTime() {
	setPacketCheckupTime(100);

	#ifdef TRACE_CLIENTS
		StringBuffer msg;
		msg << "checkup time set to " << 100;
		info(msg);
	#endif
}

void BaseClient::setPacketCheckupTime(uint32 time) {
	lock();

	try {
		#ifdef TRACE_CLIENTS
			StringBuffer msg;
			msg << "changing packet checkup time to " << time;
			info(msg);
		#endif

		checkupEvent->setCheckupTime(time);
	} catch (...) {
		disconnect("unreported exception on setPacketCheckupTime()");
	}

	unlock();
}

void BaseClient::acknowledgeClientPackets(uint16 seq) {
	lock();

	try {
		if (!isAvailable()) {
			unlock();
			return;
		}

		#ifdef TRACE_CLIENTS
			StringBuffer msg;
			msg << hex << "ACKING READ(" << seq << ")";
			info(msg);
		#endif

		BasePacket* ack = new AcknowledgeMessage(seq);
		sendPacket(ack, false);
	} catch (SocketException& e) {
		disconnect("acknowledging client packets");
	} catch (...) {
		disconnect("unreported exception on acknowledgeClientPackets()");
	}

	unlock();
}

void BaseClient::acknowledgeServerPackets(uint16 seq) {
	lock();

	try {
		if (!isAvailable()) {
			unlock();
			return;
		}

		int32 realseq = seq;
		if (seq < acknowledgedServerSequence)
			realseq = (seq & 0xFFFF) | (serverSequence & 0xFFFF0000);

		#ifdef TRACE_CLIENTS
			StringBuffer msg;
			msg << "ACKNOWLEDGED SEND(" << seq << ") [real = " << realseq << ", ackedseq = " << acknowledgedServerSequence << "]";
			info(msg);
		#endif

		scheduler->deleteEvent(checkupEvent);

		flushSendBuffer(realseq);
		acknowledgedServerSequence = realseq;

		checkupEvent->decreaseCheckupTime(1);

		if (!sequenceBuffer.isEmpty()) {
			#ifdef TRACE_CLIENTS
				StringBuffer msg;
				msg << "reschudeling for sequence " << realseq + 1;
				info(msg);
			#endif

			BasePacket* pack = sequenceBuffer.get(0);

			checkupEvent->update(pack);
			pack->setTimeout(checkupEvent->getCheckupTime());

			scheduler->addEvent(checkupEvent, pack->getTimeout());
		}
	} catch (ArrayIndexOutOfBoundsException& e) {
		info("on acknowledgeServerPackets() - " + e.getMessage());
	} catch (...) {
		disconnect("unreported exception on acknowledgeServerPackets()");
	}

	unlock();
}

void BaseClient::flushSendBuffer(int seq) {
	while (!sequenceBuffer.isEmpty()) {
		BasePacket* pack = sequenceBuffer.get(0);
		if (pack->getSequence() > (uint32) seq)
			break;

		Time& timestamp = pack->getTimestamp();

		/*#ifdef TRACE_CLIENTS
			StringBuffer msg;
			msg << "deleting packet sequence number " << pack->getSequence() << " ("
				<< timestamp.miliDifference() << " ms)";
			info(msg);
		#endif*/

		sequenceBuffer.remove(0);
		delete pack;
	}

	acknowledgedServerSequence = seq;

	#ifdef TRACE_CLIENTS
		StringBuffer msg;
		msg << sequenceBuffer.size() << " packet remained in queue";
		info(msg);
	#endif
}

void BaseClient::updateNetStatus() {
	lock();

	try {
		if (!isAvailable()) {
			unlock();
			return;
		}

		lastNetStatusTimeStamp.update();

		#ifdef TRACE_CLIENTS
			info("setting net status", true);
		#endif

		scheduler->deleteEvent(netcheckupEvent);
		scheduler->addEvent(netcheckupEvent, NETSTATUSCHECKUP_TIMEOUT);
	} catch (...) {
		disconnect("unreported exception on checkNetStatus()");
	}

	unlock();
}

bool BaseClient::checkNetStatus() {
	lock();

	try {
		#ifdef TRACE_CLIENTS
			info("getting net status event");
		#endif

		/*if (!isAvailable()) {
			unlock();
			return false;
		}*/

		info("netStatusTimeout on client");

		hasError = true;
		disconnect(false);
	} catch (...) {
		disconnect("unreported exception on checkNetStatus()");
	}

	unlock();
	return false;
}

bool BaseClient::connect() {
	try {
		lock();

		info("sending session request");

		Packet* sreq = new SessionIDRequestMessage();
		send(sreq, false);

		if (crcSeed == 0) {
			Time timeout;
			timeout.addMiliTime(3000);

			info("waiting for connection established");

			if (connectionEstablishedCondition.timedWait(this, &timeout) != 0) {
				error("connection timeout");

				unlock();
				return false;
			}
		}

		info("connection established");

		unlock();
	} catch (...) {
		error("unable to connect");

		unlock();
	}

	return true;
}

void BaseClient::notifyReceivedSeed(uint32 seed) {
	lock();

	crcSeed = seed;
	connectionEstablishedCondition.signal(this);

	unlock();
}

void BaseClient::disconnect(const String& msg, bool doLock) {
	Logger::error(msg);

	hasError = true;
	disconnect(doLock);
}

void BaseClient::disconnect() {
	disconnect(true);
}

void BaseClient::disconnect(bool doLock) {
	lock(doLock);

	if (disconnected) {
		unlock(doLock);
		return;
	}

	try {
		#ifdef TRACE_CLIENTS
			info("disconnecting client");
		#endif

		if (!hasError) {
			if (bufferedPacket != NULL)
				delete bufferedPacket;

			if (!clientDisconnected) {
				BasePacket* disc = new DisconnectMessage(this);
				prepareSend(disc);
				DatagramServiceClient::send(disc);

				delete disc;
			}
		} else {
			info("kicking client");
		}
	} catch (SocketException& e) {
		error("disconnecting client");
		hasError = true;
	} catch (...) {
		error("unreported exception on disconnect()");
		hasError = true;
	}

	close();

	unlock(doLock);
}

void BaseClient::reportStats(bool doLog) {
	int packetloss;
	if (serverSequence == 0 || resentPackets == 0)
		packetloss = 0;
	else
	 	packetloss = (100 * resentPackets) / (serverSequence + resentPackets);

	//if (packetloss > 15)
	//	doLog = true;

	StringBuffer msg;
	msg << "STATS: sent = " << serverSequence << ", resent = " << resentPackets << " [" << packetloss << "%]";
	info(msg, doLog);
}
