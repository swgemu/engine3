/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef BASECLIENT_H_
#define BASECLIENT_H_

#include "system/lang.h"

#include "engine/core/TaskManager.h"
#include "engine/core/ReentrantTask.h"

#include "engine/service/ServiceSession.h"
#include "engine/service/DatagramConnector.h"
#include "engine/service/DatagramAcceptor.h"

#include "engine/log/Logger.h"

#include "engine/proto/ProtocolCodec.h"

#include "RUDPPacket.h"
#include "BaseFragmentedPacket.h"
#include "BaseMultiPacket.h"

namespace engine {
  namespace proto {
    namespace rudp {

	class RUDPProtocol;

    } // namespace rudp
  } // namespace proto
} // namespace engine

#include "events/BasePacketChekupEvent.h"

namespace engine {
  namespace proto {
    namespace rudp {

	class BaseClientNetStatusCheckupEvent;
	class BaseClientNetStatusRequestEvent;
	class BaseClientEvent;

	class RUDPProtocol : public ProtocolCodec, public Logger, public Mutex {
	protected:
		sys::uint32 connectionID;

		unsigned int crcSeed;

	    sys::uint32 serverSequence;
	    sys::uint32 clientSequence;

		Time lastNetStatusTimeStamp;

		DatagramAcceptor* service;

		Vector<RUDPPacket*> sequenceBuffer;

		Reference<BasePacketChekupEvent*> checkupEvent;
		Reference<BaseClientNetStatusCheckupEvent*> netcheckupEvent;
		Reference<BaseClientNetStatusRequestEvent*> netRequestEvent;

		BaseMultiPacket* bufferedPacket;
		BaseFragmentedPacket* fragmentedPacket;

		Vector<RUDPPacket*> sendBuffer;

		SortedVector<RUDPPacket*> receiveBuffer;

		String ip;

		Condition connectionEstablishedCondition;

		Reference<BaseClientEvent*> reentrantTask;

		bool clientDisconnected;

		int acknowledgedServerSequence, realServerSequence;
		int resentPackets;

	public:
		static const int NETSTATUSCHECKUP_TIMEOUT = 50000;
		static const int NETSTATUSREQUEST_TIME = 5000;

	public:
		RUDPProtocol();

		virtual ~RUDPProtocol();

		void initialize();

		void send(Packet* pack, bool doLock = true);
		void sendPacket(RUDPPacket* pack, bool doLock = true);

		void read(Packet* pack, bool doLock = true);

		Packet* getBufferedPacket();

		RUDPPacket* recieveFragmentedPacket(Packet* pack);

		void run();

		bool validatePacket(Packet* pack);

		void checkupServerPackets(RUDPPacket* pack);

		void resendPackets(int seq);
		void resendPackets();

		void balancePacketCheckupTime();
		void resetPacketCheckupTime();
		void setPacketCheckupTime(sys::uint32 time);

		void acknowledgeClientPackets(sys::uint16 seq);
		void acknowledgeServerPackets(sys::uint16 seq);

		void updateNetStatus();
		bool checkNetStatus();
		void requestNetStatus();

		bool connect();

		virtual void notifyReceivedSeed(sys::uint32 seed);

		void disconnect(const String& msg, bool doLock);

		inline void disconnect(const char* msg, bool doLock = false) {
			disconnect(String(msg), doLock);
		}

		void disconnect();
		virtual void disconnect(bool doLock);

		void reportStats(bool doLog = false);

	private:
		void close();

		void bufferMultiPacket(RUDPPacket* pack);

		void sendSequenceLess(RUDPPacket* pack);
		void sendSequenced(RUDPPacket* pack);
		void sendFragmented(RUDPPacket* pack);

		RUDPPacket* getNextSequencedPacket();

		void flushSendBuffer(int seq);

	public:
		inline void setConnectionID(sys::uint32 id) {
			connectionID = id;
		}

		inline void setSeed(sys::uint32 seed) {
			crcSeed = seed;
		}

		// getters
		inline sys::uint32 getConnectionID() {
			return connectionID;
		}

		inline unsigned int getSeed() {
			return crcSeed;
		}

		// setters
		inline void setClientDisconnected() {
			clientDisconnected = true;
		}

		// getters
		inline String& getAddress() {
			return ip;
		}

		inline int getSentPacketCount() {
			return serverSequence;
		}

		inline int getResentPacketCount() {
			return resentPackets;
		}

	};

    } // namespace rudp
  } // namespace proto
} // namespace engine

using namespace engine::proto::rudp;

#endif /*BASECLIENT_H_*/
