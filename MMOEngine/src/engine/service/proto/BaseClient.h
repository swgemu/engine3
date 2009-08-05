/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef BASECLIENT_H_
#define BASECLIENT_H_

#include "system/lang.h"

#include "../DatagramServiceClient.h"
#include "../DatagramServiceThread.h"

#include "engine/core/TaskManager.h"

#include "BasePacket.h"
#include "BaseFragmentedPacket.h"
#include "BaseMultiPacket.h"

#include "BaseProtocol.h"

namespace engine {
  namespace service {
    namespace proto {

	class BaseClient;

    } // namespace proto
  } // namespace service
} // namespace engine

#include "events/BasePacketChekupEvent.h"

namespace engine {
  namespace service {
    namespace proto {

	class BaseClientNetStatusCheckupEvent;

	class BaseClient : public DatagramServiceClient, public BaseProtocol, public ReentrantTask, public Mutex {
	protected:
		DatagramServiceThread* service;

		TaskManager* taskManager;

		Vector<BasePacket*> sequenceBuffer;
		BasePacketChekupEvent* checkupEvent;
		BaseClientNetStatusCheckupEvent* netcheckupEvent;

		BaseMultiPacket* bufferedPacket;
		BaseFragmentedPacket* fragmentedPacket;

		Vector<BasePacket*> sendBuffer;

		SortedVector<BasePacket*> receiveBuffer;

		String ip;

		Condition connectionEstablishedCondition;

		bool clientDisconnected;

		int acknowledgedServerSequence, realServerSequence;
		int resentPackets;

	public:
		static const int NETSTATUSCHECKUP_TIMEOUT = 50000;

	public:
		BaseClient(const String& addr, int port);
		BaseClient(Socket* sock, SocketAddress& addr);

		virtual ~BaseClient();

		void initialize();

		void send(Packet* pack, bool doLock = true);
		void sendPacket(BasePacket* pack, bool doLock = true);

		void read(Packet* pack, bool doLock = true);

		Packet* getBufferedPacket();

		BasePacket* recieveFragmentedPacket(Packet* pack);

		bool activate();

		bool validatePacket(Packet* pack);

		void checkupServerPackets(BasePacket* pack);

		void resendPackets(int seq);
		void resendPackets();

		void balancePacketCheckupTime();
		void resetPacketCheckupTime();
		void setPacketCheckupTime(sys::uint32 time);

		void acknowledgeClientPackets(sys::uint16 seq);
		void acknowledgeServerPackets(sys::uint16 seq);

		void updateNetStatus();
		bool checkNetStatus();

		bool connect();

		void notifyReceivedSeed(sys::uint32 seed);

		void disconnect(const String& msg, bool doLock = false);

		void disconnect();
		virtual void disconnect(bool doLock);

		void reportStats(bool doLog = false);

	private:
		void close();

		void bufferMultiPacket(BasePacket* pack);

		void sendSequenceLess(BasePacket* pack);
		void sendSequenced(BasePacket* pack);
		void sendFragmented(BasePacket* pack);

		BasePacket* getNextSequencedPacket();

		void flushSendBuffer(int seq);

	public:
		// setters
		inline void setClientDisconnected() {
			clientDisconnected = true;
		}

		// getters
		inline String& getAddress() {
			return ip;
		}

		inline String getIPAddress() {
			return addr.getIPAddress();
		}

		inline int getSentPacketCount() {
			return serverSequence;
		}

		inline int getResentPacketCount() {
			return resentPackets;
		}

	};

    } // namespace proto
  } // namespace service
} // namespace engine

using namespace engine::service::proto;

#endif /*BASECLIENT_H_*/
