/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef BASECLIENT_H_
#define BASECLIENT_H_

#include "system/lang.h"

#include "engine/service/DatagramServiceClient.h"
#include "engine/service/DatagramServiceThread.h"

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

namespace engine {
 namespace stm {
   class TransactionalBaseClientManager;
 }
}

#include "events/BasePacketChekupEvent.h"

namespace engine {
  namespace service {
    namespace proto {

	class BaseClientNetStatusCheckupEvent;
	class BaseClientNetStatusRequestEvent;
	class BaseClientEvent;

	class BaseClient : public DatagramServiceClient, public BaseProtocol, public Mutex {
	protected:
		WeakReference<DatagramServiceThread*> service;

		Vector<BasePacket*> sequenceBuffer;

		Reference<BasePacketChekupEvent*> checkupEvent;
		Reference<BaseClientNetStatusCheckupEvent*> netcheckupEvent;
		Reference<BaseClientNetStatusRequestEvent*> netRequestEvent;

		BaseMultiPacket* bufferedPacket;
		BaseFragmentedPacket* fragmentedPacket;

		Vector<BasePacket*> sendBuffer;
#ifdef LOCKFREE_BCLIENT_BUFFERS
		typedef boost::lockfree::queue<BasePacket*, boost::lockfree::fixed_sized<false> > packet_buffer_t;

		packet_buffer_t* sendLockFreeBuffer;
#else
		Vector<BasePacket*> sendUnreliableBuffer;
#endif
		SortedVector<BasePacket*> receiveBuffer;

		String ip;

		Condition connectionEstablishedCondition;

		Reference<BaseClientEvent*> reentrantTask;

		bool clientDisconnected;

		int acknowledgedServerSequence, realServerSequence;
		int resentPackets;

		bool keepSocket;

	public:
		static const int NETSTATUSCHECKUP_TIMEOUT = 50000;
		static const int NETSTATUSREQUEST_TIME = 5000;

	public:
		BaseClient();
		BaseClient(const String& addr, int port);
		BaseClient(Socket* sock, SocketAddress& addr);

		virtual ~BaseClient();

		void initialize();

		void send(Packet* pack, bool doLock = true);
		void send(BasePacket* pack, bool doLock = true);
		void sendPacket(BasePacket* pack, bool doLock = true);

		void read(Packet* pack, bool doLock = true);

		Packet* getBufferedPacket();

		BasePacket* receiveFragmentedPacket(Packet* pack);

		void run();
		int sendReliablePackets(int count = 8);
		void sendUnreliablePackets();

		void sendUnreliablePacket(BasePacket* pack);

		bool validatePacket(Packet* pack);

		void checkupServerPackets(BasePacket* pack);

		void resendPackets(int seq);
		void resendPackets();

		void balancePacketCheckupTime();
		void resetPacketCheckupTime();
		void setPacketCheckupTime(sys::uint32 time);

		void acknowledgeClientPackets(sys::uint16 seq);
		void acknowledgeServerPackets(sys::uint16 seq);

		bool updateNetStatus(sys::uint16 recievedTick = 0);
		bool checkNetStatus();
		void requestNetStatus();

		bool connect();

		virtual void notifyReceivedSeed(sys::uint32 seed);

		void disconnect(const String& msg, bool doLock);

		inline void disconnect(const char* msg, bool doLock = false) {
			disconnect(String(msg), doLock);
		}

		void disconnect(bool doLock = true);

		void reportStats(bool doLog = false) const;

	protected:
		void close();

		void bufferMultiPacket(BasePacket* pack);

		void sendSequenceLess(BasePacket* pack);
		void sendSequenced(BasePacket* pack);
		void sendFragmented(BasePacket* pack);

		BasePacket* getNextSequencedPacket();
		BasePacket* getNextUnreliablePacket();

		void flushSendBuffer(int seq);

	public:
		inline bool isClientDisconnected() const {
			return clientDisconnected;
		}

		// setters
		inline void setClientDisconnected() {
			clientDisconnected = true;
		}

		inline void setNullBufferedPacket() {
			bufferedPacket = nullptr;
		}

		// getters
		inline const String& getAddress() const {
			return ip;
		}

		inline String getIPAddress() const {
			return addr.getIPAddress();
		}

		inline int getSentPacketCount() const {
			return serverSequence;
		}

		inline int getResentPacketCount() const {
			return resentPackets;
		}

		inline BaseMultiPacket* getRawBufferedPacket() {
			return bufferedPacket;
		}

		inline const BaseMultiPacket* getRawBufferedPacket() const {
			return bufferedPacket;
		}

		friend class engine::stm::TransactionalBaseClientManager;

	};

    } // namespace proto
  } // namespace service
} // namespace engine

using namespace engine::service::proto;

#endif /*BASECLIENT_H_*/
