/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

//#define TRACE_CLIENTS

#include "engine/log/Logger.h"
#include "engine/core/Core.h"

#include "BaseClient.h"

#include "events/BaseClientNetStatusCheckupEvent.h"
#include "events/BaseClientCleanupEvent.h"
#include "events/BaseClientNetStatusRequestEvent.h"
#include "events/BaseClientEvent.h"
#include "events/BaseClientHealthEvent.h"

#include "packets/SessionIDRequestMessage.h"
#include "packets/AcknowledgeMessage.h"
#include "packets/OutOfOrderMessage.h"
#include "packets/DisconnectMessage.h"
#include "packets/NetStatusRequestMessage.h"
#include "packets/NetStatusResponseMessage.h"

#include "engine/stm/TransactionalMemoryManager.h"

#define CACHED_PROPERTY_VALUE(type, getter, defaultValue, key) \
	cachedValueInternal<type, String::hashCode(key)>(key, getter, defaultValue)

namespace {
	static Logger logger("BaseClient", Logger::WARNING);
	template<typename Type, uint32 instance>
	Type cachedValueInternal(const char* configKey, Type (getter)(const String&, Type), Type defaultValue)
	{
		static Mutex mutex;
		Locker guard(&mutex);
		static Type cachedValue = defaultValue;
		static int cachedVersion = 0;
		int currentVersion = Core::getPropertiesVersion();

		if (currentVersion > cachedVersion) {
			cachedVersion = currentVersion;
			cachedValue = getter(configKey, defaultValue);
			logger.info(true) << configKey << "=" << cachedValue;
		}

		return cachedValue;
	}

	int getMaxBufferPacketsTickCount() {
		return CACHED_PROPERTY_VALUE(int, Core::getIntProperty, 500, "BaseClient.maxBufferPacketsTickCount");
	}

	int getInitialLockfreeBufferCapacity() {
		return CACHED_PROPERTY_VALUE(int, Core::getIntProperty, 500, "BaseClient.initialLockfreeBufferCapacity");
	}

	int getMaxSentPacketsPerTick() {
		return CACHED_PROPERTY_VALUE(int, Core::getIntProperty, 20, "BaseClient.maxSentPacketsPerTick");
	}

	int getMaxOutstandingPackets() {
		auto value = CACHED_PROPERTY_VALUE(int, Core::getIntProperty, 0, "BaseClient.maxOutstandingPackets");

		if (value <= 0) {
			// If not set default to 10 x BaseClient.initialLockfreeBufferCapacity
			value = getInitialLockfreeBufferCapacity() * 10;
		}

		return value;
	}

	int getMaxCheckupTime() {
		return CACHED_PROPERTY_VALUE(int, Core::getIntProperty, 2000, "BaseClient.maxCheckupTime");
	}

	int getMinCheckupTime() {
		return CACHED_PROPERTY_VALUE(int, Core::getIntProperty, 100, "BaseClient.minCheckupTime");
	}

	int getHealthCheckInterval() {
		return CACHED_PROPERTY_VALUE(int, Core::getIntProperty, 60, "BaseClient.healthCheckInterval");
	}

	int getRemoteDeltaThreshold() {
		return CACHED_PROPERTY_VALUE(int, Core::getIntProperty, 25000, "BaseClient.remoteDeltaThreshold");
	}

	int getClockDeltaThreshold() {
		return CACHED_PROPERTY_VALUE(int, Core::getIntProperty, 1800, "BaseClient.clockDeltaThreshold");
	}
}

class AcknowledgeClientPackets : public Task {
        Reference<BaseClient*> client;
        uint16 seq;

public:
        AcknowledgeClientPackets(BaseClient* cl, uint16 s) {
                client = cl;
                seq = s;
        }

        void run() {
                client->acknowledgeClientPackets(seq);
        }
};

void BaseClient::initializeCommon(const String& addr) {
    ip = addr;

	bufferedPacket = nullptr;
	receiveBuffer.setInsertPlan(SortedVector<BasePacket*>::NO_DUPLICATE);

	fragmentedPacket = nullptr;

	checkupEvent = nullptr;
	netcheckupEvent = nullptr;
	netRequestEvent = nullptr;

	reentrantTask = new BaseClientEvent(this);

	keepSocket = false;

	setLockName("BaseClient " + ip);
	setLoggingName("BaseClient " + ip);
	setLogToConsole(false);

	info() << __FUNCTION__;

	const static int vars = Core::initializeProperties("BaseClient");

#ifdef LOCKFREE_BCLIENT_BUFFERS
	sendLockFreeBuffer = new packet_buffer_t(getInitialLockfreeBufferCapacity());

	fatal(sendLockFreeBuffer->is_lock_free(), "lock free buffer is not lock free");
#endif

	creationTime.updateToCurrentTime(Time::MONOTONIC_TIME);

	configureClient(true);

	healthEvent = new BaseClientHealthEvent(this);
	healthEvent->scheduleInIoScheduler(getHealthCheckInterval() * 1000);

   	//reentrantTask->schedulePeriodic(10, 10);
}

BaseClient::BaseClient() : DatagramServiceClient(),
		BaseProtocol(), Mutex(true) {
	initializeCommon("-");
}

BaseClient::BaseClient(const String& addr, int port) : DatagramServiceClient(addr, port),
		BaseProtocol(), Mutex(true) {
	initializeCommon(addr);
}

BaseClient::BaseClient(Socket* sock, SocketAddress& addr) : DatagramServiceClient(sock, addr),
		BaseProtocol(), Mutex(true) {
	initializeCommon(addr.getFullIPAddress());
}

BaseClient::~BaseClient() {
	if (healthEvent != nullptr) {
		healthEvent->shutdown();

		healthEvent = nullptr;
	}

	if (checkupEvent != nullptr) {
		checkupEvent->cancel();

		checkupEvent = nullptr;
	}

	if (netcheckupEvent != nullptr) {
		netcheckupEvent->cancel();

		netcheckupEvent = nullptr;
	}

	if (netRequestEvent != nullptr) {
		netRequestEvent->cancel();

		netRequestEvent = nullptr;
	}

	if (!keepSocket)
		ServiceClient::close();

#ifdef LOCKFREE_BCLIENT_BUFFERS
	delete sendLockFreeBuffer;
	sendLockFreeBuffer = nullptr;
#endif

	info() << "Deleted Session";
}

void BaseClient::initialize() {
	#ifdef VERSION_PUBLIC
	SocketAddress addr = ServiceClient::getAddress();
	uint16 port = addr.getPort();

	ServiceClient::setAddress("127.0.0.1", port);
	#endif

	crcSeed = 0;

	serverSequence = 0;
   	clientSequence = 0;

   	disconnected = false;
   	clientDisconnected = false;

   	acknowledgedServerSequence = -1;
	realServerSequence = 0;
	resentPackets = 0;

	service = nullptr;

	checkupEvent = new BasePacketChekupEvent(this, getMinCheckupTime(), getMaxCheckupTime());
	netcheckupEvent = new BaseClientNetStatusCheckupEvent(this);

	lastNetStatusTimeStamp.updateToCurrentTime(Time::MONOTONIC_TIME);
   	lastNetStatusTimeStamp.addMiliTime(NETSTATUSCHECKUP_TIMEOUT);
   	balancePacketCheckupTime();

   	//netcheckupEvent->schedule(NETSTATUSCHECKUP_TIMEOUT);

   	netRequestEvent = new BaseClientNetStatusRequestEvent(this);
}

void BaseClient::configureClient(bool force) {
	auto currentConfigVersion = Core::getPropertiesVersion();

	if (!force && currentConfigVersion <= configVersion) {
		return;
	}

	auto newMaxBufferPacketsTickCount = getMaxBufferPacketsTickCount();

	if (configMaxBufferPacketsTickCount != newMaxBufferPacketsTickCount) {
		configMaxBufferPacketsTickCount = newMaxBufferPacketsTickCount;
		info() << "configureClient: configMaxBufferPacketsTickCount=" << configMaxBufferPacketsTickCount;
	}

	auto newMaxSentPacketsPerTick = getMaxSentPacketsPerTick();

	if (configMaxSentPacketsPerTick != newMaxSentPacketsPerTick) {
		configMaxSentPacketsPerTick = newMaxSentPacketsPerTick;
		info() << "configureClient: configMaxSentPacketsPerTick=" << configMaxSentPacketsPerTick;
	}

	auto newMaxOutstandingPackets = getMaxOutstandingPackets();

	if (configMaxOutstandingPackets != newMaxOutstandingPackets) {
		configMaxOutstandingPackets = newMaxOutstandingPackets;
		info() << "configureClient: configMaxOutstandingPackets=" << configMaxOutstandingPackets;
	}

	configVersion = currentConfigVersion;
}

void BaseClient::close() {
	disconnected = true;

	healthEvent->shutdown();

	reentrantTask->cancel();

	checkupEvent->cancel();

	netcheckupEvent->cancel();
	netcheckupEvent->clearClient();

	if (netRequestEvent != nullptr) {
		netRequestEvent->cancel();

		netRequestEvent->clearClient();
	}

	/*Reference<Task*> task = new BaseClientCleanupEvent(this);
	task->scheduleInIoScheduler();*/

	for (int i = 0; i < sendBuffer.size(); ++i) {
		BasePacket* pack = sendBuffer.get(i);

		if (pack->getReferenceCount())
			pack->release();
		else
			delete pack;
	}

	sendBuffer.removeAll();

	for (int i = 0; i < sequenceBuffer.size(); ++i) {
		BasePacket* pack = sequenceBuffer.get(i);

		if (pack->getReferenceCount())
			pack->release();
		else
			delete pack;
	}

	sequenceBuffer.removeAll();

	int countDiscarded = 0;

#ifdef LOCKFREE_BCLIENT_BUFFERS
	while (!sendLockFreeBuffer->empty()) {
		++countDiscarded;
		BasePacket* pack;

		if (sendLockFreeBuffer->pop(pack)) {
			if (pack->getReferenceCount())
				pack->release();
			else
				delete pack;
		}
	}
#else
	for (int i = 0; i < sendUnreliableBuffer.size(); ++i) {
		++countDiscarded;
		delete sendUnreliableBuffer.get(i);
	}

	sendUnreliableBuffer.removeAll();
#endif

	if (countDiscarded > 0) {
		info() << "Close: countDiscarded = " << countDiscarded;
	}

	if (fragmentedPacket != nullptr) {
		if (fragmentedPacket->getReferenceCount())
			fragmentedPacket->release();
		else
			delete fragmentedPacket;

		fragmentedPacket = nullptr;
	}

	reportStats("Close");

	//serverSequence = 0;
	clientSequence = 0;

	acknowledgedServerSequence = -1;

	//ServiceClient::close();

	debug("client resources closed");
}

void BaseClient::send(Packet* pack, bool doLock) {
	//setDebugLogLevel();
	lock(doLock);

	try {
		if (isAvailable()) {
			#ifdef TRACE_CLIENTS
				debug("SEND(RAW) - " + pack->toString());
			#endif

			if (!DatagramServiceClient::send(pack)) {
			#ifdef TRACE_CLIENTS
				debug() << "LOSING " << pack->toStringData();
			#endif
			}
		}
	} catch (SocketException& e) {
		StringBuffer err;
		err << "SocketException in " << __PRETTY_FUNCTION__ << ": " << e.getMessage();
		error() << err;
		setError();
		disconnect(err.toString());
	}

	delete pack;

	unlock(doLock);
}

void BaseClient::send(BasePacket* pack, bool doLock) {
	//setDebugLogLevel();
	lock(doLock);

	try {
		if (isAvailable()) {
#ifdef TRACE_CLIENTS
			debug("SEND(RAW) - " + pack->toString());
#endif

			if (!DatagramServiceClient::send(pack))
				debug() << "LOSING " << *pack;
		}
	} catch (SocketException& e) {
		StringBuffer err;
		err << "SocketException in " << __PRETTY_FUNCTION__ << ": " << e.getMessage();
		error() << err;
		setError();
		disconnect(err.toString());
	}

	if (pack->getReferenceCount())
		pack->release();
	else
		delete pack;

	unlock(doLock);
}

void BaseClient::sendPacket(BasePacket* pack, bool doLock) {
#ifdef WITH_STM
	TransactionalMemoryManager::instance()->getBaseClientManager()->sendPacket(pack, this);

	return;
#endif

#ifdef LOCKFREE_BCLIENT_BUFFERS
	if (!isAvailable()) {
		if (!pack->getReferenceCount())
			delete pack;

		return;
	}

	if (pack->getReferenceCount()) {
		pack->acquire();
	}

	if (!sendLockFreeBuffer->push(pack)) {
		error("losing message in BaseClient::sendPacket due to a failed push in sendLockFreeBuffer");

		if (pack->getReferenceCount()) {
			pack->release();
		} else {
			delete pack;
		}
	}

	return;
#else
	lock(doLock);

	if (!isAvailable()) {
		delete pack;

		unlock(doLock);
		return;
	}

	/*#ifdef TRACE_CLIENTS
		debug("preapare SEND " + pack->toString());
	#endif*/

	try {
		if (pack->doSequencing()) {
			if (pack->size() >= 490) {
				if (bufferedPacket != nullptr) {
					sendSequenced(bufferedPacket->getPacket());
					bufferedPacket = nullptr;
				}

				sendFragmented(pack);
			} else
				bufferMultiPacket(pack);
		} else {
			sendSequenceLess(pack);
		}
	} catch (SocketException& e) {
		StringBuffer err;
		err << "SocketException in " << __PRETTY_FUNCTION__ << ": " << e.getMessage();
		error() << err;
		setError();
		disconnect(err.toString());
	} catch (...) {
		disconnect("unreported exception on sendPacket()");
	}

	unlock(doLock);
#endif
}

void BaseClient::bufferMultiPacket(BasePacket* pack) {
	if (bufferedPacket != nullptr) {
		if (pack->isDataChannelPacket() && !pack->isMultiPacket() && (pack->size() - 4 < 0xFF)) { // client is sending out of orders for our multi packets with size >= 0xFF
			if (!bufferedPacket->add(pack)) {
				sendSequenced(bufferedPacket->getPacket());

				bufferedPacket = new BaseMultiPacket(pack);
			}
		} else {
			sendSequenced(bufferedPacket->getPacket());
			bufferedPacket = nullptr;

			sendSequenced(pack);
		}
	} else {
		if (pack->isDataChannelPacket() && !pack->isMultiPacket() && (pack->size() - 4 < 0xFF))
			bufferedPacket = new BaseMultiPacket(pack);
		else
			sendSequenced(pack);

#ifndef LOCKFREE_BCLIENT_BUFFERS
		if (!reentrantTask->isScheduled())
			reentrantTask->scheduleInIoScheduler(10);
#endif
	}
}

void BaseClient::sendSequenceLess(BasePacket* pack) {
	try {
		#ifdef TRACE_CLIENTS
			debug("SEND(NOSEQ) - " + pack->toString());
		#endif

#ifdef LOCKFREE_BCLIENT_BUFFERS
		sendUnreliablePacket(pack);
#else
		sendUnreliableBuffer.add(pack);

		if (!reentrantTask->isScheduled())
			reentrantTask->scheduleInIoScheduler(10);
#endif
	} catch (SocketException& e) {
		if (pack->getReferenceCount())
			pack->release();
		else
			delete pack;

		StringBuffer err;
		err << "SocketException in " << __PRETTY_FUNCTION__ << ": " << e.getMessage();
		error() << err;
		setError();
		disconnect(err.toString());
	}
}

void BaseClient::sendSequenced(BasePacket* pack) {
	if (!isAvailable())
		return;

	try {
		pack->setTimeout(checkupEvent->getCheckupTime());
		sendBuffer.add(pack);

#ifndef LOCKFREE_BCLIENT_BUFFERS
		if (!reentrantTask->isScheduled())
			reentrantTask->scheduleInIoScheduler(10);
#endif
	} catch (SocketException& e) {
		StringBuffer err;
		err << "SocketException in " << __PRETTY_FUNCTION__ << ": " << e.getMessage();
		error() << err;
		setError();
		disconnect(err.toString());
	} catch (ArrayIndexOutOfBoundsException& e) {
		error("ArrayIndexOutOfBoundsException on sendSequenced() - " + e.getMessage());
	}

	/*#ifdef TRACE_CLIENTS
		StringBuffer msg;
		msg << "SEND(" << pack->getSequence() << ")" + pack->toString());
		debug(msg);
	#endif*/
}

void BaseClient::sendFragmented(BasePacket* pack) {
	if (!isAvailable())
		return;

	try {
		BaseFragmentedPacket* frag = new BaseFragmentedPacket(pack);

		while (frag->hasFragments())
			sendSequenced(frag->getFragment());

		if (frag->getReferenceCount())
			frag->release();
		else
			delete frag;
	} catch (SocketException& e) {
		StringBuffer err;
		err << "SocketException in " << __PRETTY_FUNCTION__ << ": " << e.getMessage();
		error() << err;
		setError();
		disconnect(err.toString());
	} catch (ArrayIndexOutOfBoundsException& e) {
		error("ArrayIndexOutOfBoundsException on sendFragmented() - " + e.getMessage());
	}
}

int BaseClient::sendReliablePackets(int count) {
	int sentPackets = 0;
	try {
		for (int i = 0; i < count; ++i) {
			if (isAvailable()) {
				BasePacket* pack = getNextSequencedPacket();

				if (pack == nullptr) {
#ifndef LOCKFREE_BCLIENT_BUFFERS
					if (!reentrantTask->isScheduled() && (!sendBuffer.isEmpty() || bufferedPacket != nullptr)) {
						try {
							reentrantTask->scheduleInIoScheduler(10);
						} catch (Exception& e) {

						}
					}
#endif

					return sentPackets;
				}

				pack->acquire();

				if (sequenceBuffer.isEmpty()) {
					checkupEvent->update(pack);
					pack->setTimeout(checkupEvent->getCheckupTime());

					if (!checkupEvent->isScheduled())
						checkupEvent->scheduleInIoScheduler(pack->getTimeout());
				}

#ifdef TRACE_CLIENTS
				StringBuffer msg;
				msg << "SEND(" << serverSequence << ") - " << pack->toString();
				debug(msg);
#endif

				pack->setTimestamp();

				prepareSequence(pack);

				if (pack->getSequence() != (uint32) realServerSequence++) {
					StringBuffer msg;
					msg << "invalid server Packet " << pack->getSequence() << " sent (" << realServerSequence - 1 << ")";
					error(msg);
				}

				unlock();

				prepareEncryptionAndCompression(pack);

				lock();

				if (!DatagramServiceClient::send(pack)) {
					debug() << "LOSING (" << pack->getSequence() << ") " << *pack;
				}

				++sentPackets;

				sequenceBuffer.add(pack);
			}
		}
#ifndef LOCKFREE_BCLIENT_BUFFERS
		if (!reentrantTask->isScheduled() && (!sendBuffer.isEmpty() || !sendUnreliableBuffer.isEmpty() || bufferedPacket != nullptr)) {
			reentrantTask->scheduleInIoScheduler(10);
		}
#endif

	} catch (SocketException& e) {
		StringBuffer err;
		err << "SocketException in " << __PRETTY_FUNCTION__ << ": " << e.getMessage();
		error() << err;
		setError();
		disconnect(err.toString());
	} catch (Exception& e) {
		error(e.getMessage());
		e.printStackTrace();

		disconnect("exception on sendReliablePackets()");
	} catch (...) {
		disconnect("unreported exception on sendReliablePackets()");
	}

	return sentPackets;
}

void BaseClient::sendUnreliablePacket(BasePacket* pack) {
	try {
		if (isAvailable()) {
			unlock();

			pack->close();

			prepareEncryptionAndCompression(pack);

			lock();

			if (!DatagramServiceClient::send(pack)) {
				debug() << "LOSING (" << pack->getSequence() << ") " << *pack;
			}
		}

		if (pack->getReferenceCount())
			pack->release();
		else
			delete pack;

	} catch (SocketException& e) {
		StringBuffer err;
		err << "SocketException in " << __PRETTY_FUNCTION__ << ": " << e.getMessage();
		error() << err;
	} catch (Exception& e) {
		error("Exception on sendUnreliablePacket() - " + e.getMessage());
		e.printStackTrace();
	} catch (...) {
		error("unreported exception on sendUnreliablePacket()");
	}
}

void BaseClient::sendUnreliablePackets() {
	try {
		for (int i = 0; i < 8; ++i) {
			if (isAvailable()) {
				BasePacket* pack = getNextUnreliablePacket();

				if (pack == nullptr) {
#ifndef LOCKFREE_BCLIENT_BUFFERS
					if (!reentrantTask->isScheduled() && (!sendBuffer.isEmpty() || !sendUnreliableBuffer.isEmpty() || bufferedPacket != nullptr)) {
						try {
							reentrantTask->scheduleInIoScheduler(10);
						} catch (Exception& e) {

						}
					}
#endif
					return;
				}

				unlock();

				pack->close();

				prepareEncryptionAndCompression(pack);

				lock();

				if (!DatagramServiceClient::send(pack)) {
					debug() << "LOSING (" << pack->getSequence() << ") " << *pack;
				}

				if (pack->getReferenceCount())
					pack->release();
				else
					delete pack;
			}
		}
#ifndef LOCKFREE_BCLIENT_BUFFERS
		if (!reentrantTask->isScheduled() && (!sendBuffer.isEmpty() || !sendUnreliableBuffer.isEmpty() || bufferedPacket != nullptr)) {
			reentrantTask->scheduleInIoScheduler(10);
		}
#endif
	} catch (SocketException& e) {
		StringBuffer err;
		err << "SocketException in " << __PRETTY_FUNCTION__ << ": " << e.getMessage();
		error() << err;
	} catch (Exception& e) {
		error("exception on sendUnreliablePackets()");
		error(e.getMessage());
		e.printStackTrace();
	} catch (...) {
		error("unreported exception on sendUnreliablePackets()");
	}
}

void BaseClient::run() {
	//info("run event", true);
#ifdef LOCKFREE_BCLIENT_BUFFERS
	int i = 0, j = 0;
	BasePacket* incomingPack;
#endif

	lock();

#ifdef LOCKFREE_BCLIENT_BUFFERS
	while ((i++ < configMaxBufferPacketsTickCount)
			&& (j < configMaxSentPacketsPerTick)
			&& sendLockFreeBuffer->pop(incomingPack)) {
		try {
			BasePacket* pack;

			if (incomingPack->getReferenceCount()) {
				pack = incomingPack->clone();
				incomingPack->release();
			} else {
				pack = incomingPack;
			}

			if (!pack->doSequencing()) {
				j += sendReliablePackets(2);

				sendSequenceLess(pack);
			} else {
				if (pack->size() >= 490) {
					if (bufferedPacket != nullptr) {
						sendSequenced(bufferedPacket->getPacket());
						bufferedPacket = nullptr;
					}

					sendFragmented(pack);
				} else {
					bufferMultiPacket(pack);
				}
			}
		} catch (...) {
			disconnect("unreported exception in run");

			unlock();

			return;
		}
	}

	if (i >= configMaxBufferPacketsTickCount) {
		warning() << "more than " << configMaxBufferPacketsTickCount << " packets in sendLockFreeBuffer on BaseClient tick";
		reportStats("configMaxBufferPacketsTickCount exceeded");
	}

	sendReliablePackets();
#else
	sendReliablePackets();

	sendUnreliablePackets();
#endif

#ifdef LOCKFREE_BCLIENT_BUFFERS
	if (isAvailable()) {
		try {
			auto ref = reentrantTask;

			if (ref != nullptr)
				ref->scheduleInIoScheduler(10);
		} catch (...) {

		}
	}
#endif

	unlock();
}

BasePacket* BaseClient::getNextUnreliablePacket() {
#ifdef LOCKFREE_BCLIENT_BUFFERS
		return nullptr;
#else
	if (!sendUnreliableBuffer.isEmpty()) {
		BasePacket* pack = sendUnreliableBuffer.remove(0);

		return pack;
	}

	return nullptr;
#endif
}

BasePacket* BaseClient::getNextSequencedPacket() {
	BasePacket* pack = nullptr;

	/*#ifdef TRACE_CLIENTS
		StringBuffer msg;
		msg << "SEQ = " << serverSequence << ", ACKSEQ = " << acknowledgedServerSequence << ", BUFFSIZE = "
			<< sendBuffer.size();
		debug(msg);
	#endif*/

	if (serverSequence - acknowledgedServerSequence > 50) { //originally 25
#ifndef LOCKFREE_BCLIENT_BUFFERS
		if ((!sendBuffer.isEmpty() || bufferedPacket != nullptr) && !reentrantTask->isScheduled())
			reentrantTask->scheduleInIoScheduler(10);
#endif
		try {
			if (!checkupEvent->isScheduled()) {
				checkupEvent->scheduleInIoScheduler(5);
			}
		} catch (...) {
		}

//      resendPackets();

		auto outstandingCount = sendBuffer.size();

		if (outstandingCount > maxOutstanding) {
			maxOutstanding = outstandingCount;
			reportStats("outstandingCount > maxOutstanding");
		}

		if (outstandingCount > configMaxOutstandingPackets) {
			error() << "WARNING - send buffer overload [" << outstandingCount << "], disconnecting.";

			disconnect(false);
		}

		return nullptr;
	} else if (!sendBuffer.isEmpty()) {
		pack = sendBuffer.remove(0);
	} else if (bufferedPacket != nullptr) {
		pack = bufferedPacket->getPacket();
		pack->setTimeout(checkupEvent->getCheckupTime());

		bufferedPacket = nullptr;
	} else
		return nullptr;

	return pack;
}

bool BaseClient::validatePacket(Packet* pack) {
	uint16 seq = pack->parseNetShort();

	Locker locker(this);

	/*if (clientSequence % 0xFF == 0) {
		info("current sequence " + String::valueOf((uint64) clientSequence), true);
	}*/

	if (seq < (clientSequence & 0xFFFF)) {
		acknowledgeClientPackets(seq);
		//Core::getTaskManager()->executeTask(new AcknowledgeClientPackets(this, seq), 9);

		return false;
	} else if (seq > (clientSequence & 0xFFFF)) {
		BasePacket* packet = new BasePacket(pack, seq);
		receiveBuffer.put(packet);

		/*StringBuffer msg3;
		msg3 << "READ buffer (";

		for (int i = 0; i < receiveBuffer.size(); ++i) {
			msg3 << receiveBuffer.get(i)->getSequence() << ", ";
		}

		debug(msg3);*/

		BasePacket* oor = new OutOfOrderMessage(seq);
		sendPacket(oor, false);

		#ifdef TRACE_CLIENTS
   			debug() << "OUT of order READ(" << seq << ") expected " << clientSequence;
		#endif

		numOutOfOrder++;

		return false;
	} /*else
		throw Exception("received same packet sequence");*/

	acknowledgeClientPackets(clientSequence++);
	//Core::getTaskManager()->executeTask(new AcknowledgeClientPackets(this, clientSequence++), 9);


	#ifdef TRACE_CLIENTS
		debug() << "READ(" << seq << ") - " << *pack;
	#endif

	return true;
}

Packet* BaseClient::getBufferedPacket() {
	if (!receiveBuffer.isEmpty()) {
		BasePacket* packet = receiveBuffer.get(0);

		uint32 packseq = packet->getSequence();
		if ((packseq & 0xFFFF) != (clientSequence & 0xFFFF))
			return nullptr;

		receiveBuffer.remove(0);

		acknowledgeClientPackets(clientSequence++);

		#ifdef TRACE_CLIENTS
			debug() << "BUFFERED READ(" << packseq << ")";
		#endif

		return packet;
	}

	return nullptr;
}

BasePacket* BaseClient::receiveFragmentedPacket(Packet* pack) {
	//Logger::console.info("recieveFragmentedPacket " + pack->toStringData(), true);

	BasePacket* packet = nullptr;

	if (fragmentedPacket == nullptr) {
		fragmentedPacket = new BaseFragmentedPacket();
		//Logger::console.info("creating new BaseFragmentedPacket", true);
	}

	if (!fragmentedPacket->addFragment(pack)) {
		error() << "addFragment failed: " << fragmentedPacket->getError() << "; fragmentedPacket: " << *fragmentedPacket << endl << "packet: " << *pack;

		if (fragmentedPacket->getReferenceCount())
			fragmentedPacket->release();
		else
			delete fragmentedPacket;

		fragmentedPacket = nullptr;

		throw FragmentedPacketParseException("could not insert frag");
	}

	try {
		if (fragmentedPacket->isComplete()) {
			fragmentedPacket->setOffset(0);

			//Logger::console.info("completed fragmented packet", true);

			packet = fragmentedPacket;
			fragmentedPacket = nullptr;
		}
	} catch (const Exception& e) {
		if (fragmentedPacket != nullptr) {
			error()
				<< "fragmentedPacket->isComplete() exception: " << e.getMessage()
				<< "; error: " << fragmentedPacket->getError()
				<< "; fragmentedPacket: " << *fragmentedPacket
				<< "; packet: " << *pack;

			if (fragmentedPacket->getReferenceCount())
				fragmentedPacket->release();
			else
				delete fragmentedPacket;

			fragmentedPacket = nullptr;
			packet = nullptr;
		} else {
			error() << "fragmentedPacket->isComplete() exception: " << e.getMessage() << "; packet: " << *pack;
		}
	} catch (...) {
		if (fragmentedPacket != nullptr) {
			error()
				<< "fragmentedPacket->isComplete() exception: unreproted exception caught"
				<< "; error: " << fragmentedPacket->getError()
				<< "; fragmentedPacket: " << *fragmentedPacket
				<< "; packet: " << *pack;

			if (fragmentedPacket->getReferenceCount())
				fragmentedPacket->release();
			else
				delete fragmentedPacket;

			fragmentedPacket = nullptr;
			packet = nullptr;
		} else {
			error() << "fragmentedPacket->isComplete() exception: unreproted exception caught; packet: " << *pack;
		}
	}

	return packet;
}

void BaseClient::checkupServerPackets(BasePacket* pack) {
	lock();

	try {
		if (!isAvailable() || checkupEvent->isScheduled() || sequenceBuffer.size() == 0) {
			unlock();
			return;
		}

		uint32 seq = pack->getSequence();

		#ifdef TRACE_CLIENTS
			debug() << "CHECKING UP sequence " << seq << "[" << acknowledgedServerSequence
				<< "]";
		#endif

		if (seq > (uint32) acknowledgedServerSequence) {
			resendPackets();

			checkupEvent->increaseCheckupTime(250);
			checkupEvent->update(pack);

			#ifdef TRACE_CLIENTS
				debug() << "checkup time incresed to " << checkupEvent->getCheckupTime();
			#endif

			if (!checkupEvent->isScheduled())
				checkupEvent->scheduleInIoScheduler(pack->getTimeout());
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
		debug(msg2, true);
	#endif*/

	if (sequenceBuffer.size() == 0)
		return;

	float checkupTime = (float) (float)(checkupEvent->getCheckupTime()) / 1000.f;
	int maxPacketResent = (int) Math::max(5.f, (float)30000.f * checkupTime / 496.f); //30kb * second assuming 496 packet size

	/*StringBuffer msg2;
	msg2 << "resending MIN(" << sequenceBuffer.size() << " and " << maxPacketResent << ") packets to \'" << ip << "\' ["
			<< ((BasePacketChekupEvent*)(checkupEvent.get()))->getCheckupTime() << "]";
	info(msg2, true);*/

	for (int i = 0; i < Math::min(sequenceBuffer.size(), maxPacketResent); ++i) {
//	for (int i = 0; i < sequenceBuffer.size(); ++i) {
//	for (int i = 0; i < MIN(sequenceBuffer.size(), 1); ++i) {
		BasePacket* packet = sequenceBuffer.get(i);

		/*if (packet->getTimeout().isFuture())
			break;*/

		packet->setTimeout(checkupEvent->getCheckupTime());

		if (!DatagramServiceClient::send(packet)) {
			debug() << "LOSING on resend (" << packet->getSequence() << ") " << *packet;
		}

		++resentPackets;

		#ifdef TRACE_CLIENTS
			debug() << "RESEND(" << packet->getSequence() << ") - " << *packet;
		#endif
	}
}

void BaseClient::resendPackets(int seq) {
        return; //disabled

	lock();

	int maxPackets = 5;

	for (int i = 0; i < sequenceBuffer.size(); ++i) {
		BasePacket* packet = sequenceBuffer.get(i);

		if (packet->getSequence() != (uint32)seq - 1) {
			continue;
		}

		if (!DatagramServiceClient::send(packet)) {
			debug() << "LOSING on resend (" << packet->getSequence() << ") " << *packet;
		}

		packet->setTimeout(checkupEvent->getCheckupTime());

		++resentPackets;

		break;
	}

	unlock();

	return;

	for (int i = 0; i < sequenceBuffer.size(); ++i) {

	        if (i >= maxPackets) {
	                break;
	        }

		BasePacket* packet = sequenceBuffer.get(i);

		if (packet->getSequence() == (uint32) seq) {
//			sequenceBuffer.remove(i);

//			delete packet;
			break;
		}

/*		if (packet->getTimeout().isFuture())
			continue;

		packet->setTimeout(((BasePacketChekupEvent*)(checkupEvent.get()))->getCheckupTime());
*/

		if (!DatagramServiceClient::send(packet)) {
			debug() << "LOSING on resend (" << packet->getSequence() << ") " << *packet;
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
		debug(msg);
	#endif
}

void BaseClient::resetPacketCheckupTime() {
	setPacketCheckupTime(100);

	#ifdef TRACE_CLIENTS
		StringBuffer msg;
		msg << "checkup time set to " << 100;
		debug(msg);
	#endif
}

void BaseClient::setPacketCheckupTime(uint32 time) {
	lock();

	try {
		#ifdef TRACE_CLIENTS
			StringBuffer msg;
			msg << "changing packet checkup time to " << time;
			debug(msg);
		#endif

		info() << "CheckupTime = " << time;
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
			debug(msg);
		#endif

		BasePacket* ack = new AcknowledgeMessage(seq);
		sendPacket(ack, false);
	} catch (SocketException& e) {
		disconnect("SocketException acknowledging client packets");
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
		if (seq < acknowledgedServerSequence) {
			realseq = (seq & 0xFFFF) | (serverSequence & 0xFFFF0000);
		}

		if ((uint32)realseq > serverSequence) {
			realseq -= 0x10000;
		}

		#ifdef TRACE_CLIENTS
			debug() << "ACKNOWLEDGED SEND(" << seq << ") [real = " << realseq << ", ackedseq = " << acknowledgedServerSequence << "]";
		#endif

		if (realseq < acknowledgedServerSequence) {
		        unlock();
		        return;
		}

		remoteStats.updateAckStats(checkupEvent->getElapsedTimeMs());

		checkupEvent->cancel();

		flushSendBuffer(realseq);
		acknowledgedServerSequence = realseq;

		checkupEvent->decreaseCheckupTime(1);

		if (!sequenceBuffer.isEmpty()) {
			#ifdef TRACE_CLIENTS
				StringBuffer msg;
				msg << "reschudeling for sequence " << realseq + 1;
				debug(msg);
			#endif

			BasePacket* pack = sequenceBuffer.get(0);

			checkupEvent->update(pack);
			pack->setTimeout(checkupEvent->getCheckupTime());

			if (!checkupEvent->isScheduled())
				checkupEvent->scheduleInIoScheduler(pack->getTimeout());
		}
	} catch (ArrayIndexOutOfBoundsException& e) {
		debug("on acknowledgeServerPackets() - " + e.getMessage());
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
			debug(msg);
		#endif*/

		sequenceBuffer.remove(0);
		if (pack->getReferenceCount())
			pack->release();
		else
			delete pack;
	}

	acknowledgedServerSequence = seq;

	#ifdef TRACE_CLIENTS
		StringBuffer msg;
		msg << sequenceBuffer.size() << " packet remained in queue";
		debug(msg);
	#endif
}

void BaseClient::resetNetStatusTimeout() {
	if (!isAvailable()) {
		return;
	}

	lock();

	lastNetStatusTimeStamp.updateToCurrentTime();
	lastRecievedNetStatusTick = 0;
	netcheckupEvent->rescheduleInIoScheduler(NETSTATUSCHECKUP_TIMEOUT);

	unlock();
}

bool BaseClient::handleNetStatusRequest(Packet* pack) {
	lock();

	try {
		if (!isAvailable() || pack == nullptr) {
			unlock();
			return false;
		}

		uint16 ourTick = Time().getMiliTime() & 0xFFFF;
		uint16 tick = pack->parseNetShort();
		uint32 unk1 = pack->parseInt();
		uint32 unk2 = pack->parseInt();
		uint32 unk3 = pack->parseInt();
		uint32 unk4 = pack->parseInt();
		uint32 unk5 = pack->parseInt();

		remoteStats.setTotalPacketsSent(pack->parseNetLong());
		remoteStats.setTotalPacketsReceived(pack->parseNetLong());
		remoteStats.setTickDelta(tickDiff(tick, ourTick));
		remoteStats.setTimeStamp(lastNetStatusTimeStamp);

		if (lastRecievedNetStatusTick != 0) {
			auto remoteDelta = tickDiff(tick, lastRecievedNetStatusTick);
			auto localDelta = lastNetStatusTimeStamp.miliDifference(Time::MONOTONIC_TIME);
			auto delta = abs(remoteDelta - localDelta);

			if (remoteDelta > getRemoteDeltaThreshold()) {
				StringBuffer msg;
				msg << __FUNCTION__ << ": remoteDeltaThreshold(" << getRemoteDeltaThreshold() << ") exceeded: remoteDelta=" << remoteDelta;
				reportStats(msg.toString());
			}

			if (delta > getClockDeltaThreshold()) {
				StringBuffer msg;
				msg << __FUNCTION__ << ": clockDeltaThreshold(" << getClockDeltaThreshold() << ") exceeded: delta=" << delta << "; remoteDelta=" << remoteDelta << "; localDelta=" << localDelta;
				reportStats(msg.toString());
			}

			/*StringBuffer msg;
			msg << "recievedTick: " << hostByte << " clientDelta:" << clientDelta << " serverDelta:" << serverDelta;
			debug(msg, true);*/
/*
			if (clientDelta > serverDelta) {
				uint16 difference = clientDelta - serverDelta;

				if ((difference > 200) && (++erroneusTicks > 10)) {
					disconnect("client clock desync");

					unlock();

					return false;
				}
			} else
				erroneusTicks = 0;

				*/
		}

		lastNetStatusTimeStamp.updateToCurrentTime(Time::MONOTONIC_TIME);
		lastRecievedNetStatusTick = tick;

		#ifdef TRACE_CLIENTS
			debug("setting net status");
		#endif

		netcheckupEvent->rescheduleInIoScheduler(NETSTATUSCHECKUP_TIMEOUT);

		BasePacket* resp = new NetStatusResponseMessage(tick);
		sendPacket(resp);

	} catch (Exception& e) {
		e.printStackTrace();
		disconnect("Exception on updateNetStatus()");
	} catch (...) {
		disconnect("unreported exception on updateNetStatus()");
	}

	unlock();

	return true;
}

void BaseClient::requestNetStatus() {
	lock();

	try {
		if (!isAvailable()) {
			unlock();

			return;
		}

		uint16 ourTick = Time().getMiliTime() & 0xFFFF;

		BasePacket* resp = new NetStatusRequestMessage(ourTick);
		sendPacket(resp, false);

		netRequestEvent->rescheduleInIoScheduler(NETSTATUSREQUEST_TIME);
	} catch (Exception& e) {
		e.printStackTrace();
		disconnect("Exception on requestNetStatus()");
	} catch (...) {
		disconnect("unreported exception caught in BaseClient::requestNetStatus()", true);
	}

	unlock();
}

bool BaseClient::checkNetStatus() {
	lock();

	try {
		#ifdef TRACE_CLIENTS
			debug("getting net status event");
		#endif

		/*if (!isAvailable()) {
			unlock();
			return false;
		}*/

		info("netStatusTimeout on client");

		setError();
		disconnect(false);
	} catch (Exception& e) {
		disconnect("Exception on checkNetStatus()");
		e.printStackTrace();
	} catch (...) {
		disconnect("unreported exception on checkNetStatus()");
	}

	unlock();
	return false;
}

class ConnectTask : public Task {
	Reference<BaseClient*> client;

public:
	ConnectTask(BaseClient* cli) {
		client = cli;
	}

	void run() {
		Packet* sreq = new SessionIDRequestMessage();
		client->send(sreq, false);
	}
};

bool BaseClient::connect() {
	try {
		lock();

		#ifdef VERSION_PUBLIC
		SocketAddress addr = ServiceClient::getAddress();
		uint16 port = addr.getPort();

		ServiceClient::setAddress("127.0.0.1", port);
		#endif

		debug("sending session request");

		Reference<ConnectTask*> task = new ConnectTask(this);
		task->execute();

		if (crcSeed == 0) {
			Time timeout;
			timeout.addMiliTime(15000);

			debug("waiting for connection established");

			if (connectionEstablishedCondition.timedWait(this, &timeout) != 0) {
				error("connection timeout");

				unlock();
				return false;
			}
		}

		debug("connection established");

		if (!netRequestEvent->isScheduled())
			netRequestEvent->scheduleInIoScheduler(NETSTATUSREQUEST_TIME);

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
	error() << "Force disconnect: " << msg;

	setError();
	disconnect(doLock);
}

void BaseClient::disconnect(bool doLock) {
	//lock(doLock);
	Locker locker(this);

	if (disconnected) {
		//unlock(doLock);
		return;
	}

	try {
		info() << "Disconnecting client, hasError = " << hasError();
		#ifdef TRACE_CLIENTS
			debug("disconnecting client");
		#endif

		if (!hasError()) {
			if (bufferedPacket != nullptr) {
				if (bufferedPacket->getReferenceCount())
					bufferedPacket->release();
				else
					delete bufferedPacket;
			}

			if (!clientDisconnected) {
				info() << "Sending DisconnectMessage";
				BasePacket* disc = new DisconnectMessage(this);
				prepareSend(disc);
				DatagramServiceClient::send(disc);

				delete disc;
			}
		} else {
			debug("kicking client");
		}
	} catch (const SocketException& e) {
		StringBuffer err;
		err << "SocketException in " << __PRETTY_FUNCTION__ << ": " << e.getMessage();
		error() << err;
		setError();
	} catch (...) {
		error("unreported exception on disconnect()");
		setError();
	}

	close();

	//unlock(doLock);
	locker.release();

	auto service = this->service.get();

	if (service != nullptr) {
		service->removeConnection(this);

		service = nullptr;
	}
}

void BaseClient::reportStats(const String& msg) {
	if (getLogLevel() < Logger::INFO) {
		return;
	}

	if (firstStatusReport.compareAndSet(false, true)) {
		info()
			<< "InitialLockfreeBufferCapacity=" << getInitialLockfreeBufferCapacity()
			<< ", MaxBufferPacketsTickCount=" << configMaxBufferPacketsTickCount
			<< ", MaxSentPacketsPerTick=" << configMaxSentPacketsPerTick
			<< ", MaxOutstandingPackets=" << configMaxOutstandingPackets
		    << ", MinCheckupTime=" << getMinCheckupTime()
			<< ", MaxCheckupTime=" << getMaxCheckupTime()
			<< ", HealthCheckInterval=" << getHealthCheckInterval()
			<< ", RemoteDeltaThreshold=" << getRemoteDeltaThreshold()
			<< ", ClockDeltaThreshold=" << getClockDeltaThreshold()
			;
	}

	int resentPercent;

	if (serverSequence == 0 || resentPackets == 0)
		resentPercent = 0;
	else
	 	resentPercent = (100 * resentPackets) / (serverSequence + resentPackets);

	uint32 checkupTime = 0;

	if (checkupEvent != nullptr) {
		checkupTime = checkupEvent->getCheckupTime();
	}

	Time now(Time::MONOTONIC_TIME);

	auto elaspedMs = creationTime.miliDifference(now);

	log()
		<< "reportStats:\n{\"@timestamp\":\"" << now.getFormattedTimeFull() << "\""
		<< ", \"ip\": \"" << ip << "\""
		<< ", \"elaspedMs\": " << elaspedMs
		<< ", \"serverSequence\": " << serverSequence
		<< ", \"resentPackets\": " << resentPackets
		<< ", \"resentPercent\": " << resentPercent
		<< ", \"checkupTime\": " << checkupTime
		<< ", \"configVersion\": " << configVersion
		<< remoteStats.asJSONFragment(false)
		<< ", \"numOutOfOrder\": " << numOutOfOrder
		<< ", \"acknowledgedServerSequence\": " << acknowledgedServerSequence
		<< ", \"realServerSequence\": " << realServerSequence
		<< ", \"sendBufferSize\": " << sendBuffer.size()
		<< ", \"receiveBufferSize\": " << receiveBuffer.size()
		<< ", \"sequenceBufferSize\": " << sequenceBuffer.size()
#ifdef LOCKFREE_BCLIENT_BUFFERS
		<< ", \"isSendLockFreeBufferEmpty\": " << sendLockFreeBuffer->empty()
#else // LOCKFREE_BCLIENT_BUFFERS
		<< ", \"sendUnreliableBufferSize\": " << sendUnreliableBuffer.size()
#endif // LOCKFREE_BCLIENT_BUFFERS
		<< ", \"hasError\": " << hasError()
		<< ", \"msg\": \"" << msg << "\""
        << "}";
}

void BaseClient::runHealthCheck() {
	Locker lock(this);

	if (healthEvent == nullptr) {
		return;
	}

	if (Core::getPropertiesVersion() > configVersion) {
		debug() << __FUNCTION__ << ": Detected new configVersion";
		configureClient(true);
	}

	reportStats("PeriodicReport");

	healthEvent->rescheduleInIoScheduler(getHealthCheckInterval() * 1000);
}
