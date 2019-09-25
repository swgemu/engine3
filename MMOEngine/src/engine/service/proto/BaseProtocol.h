/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef BASEPROTOCOL_H_
#define BASEPROTOCOL_H_

#include "system/lang.h"
#include "engine/log/Logger.h"

#include "BasePacket.h"

namespace engine {
  namespace service {
    namespace proto {

	class BaseProtocol : public Logger {
	protected:
		sys::uint32 connectionID;

		unsigned int crcSeed;

		sys::uint32 serverSequence;
		sys::uint32 clientSequence;

		Time lastNetStatusTimeStamp;
		uint16 lastRecievedNetStatusTick;
		uint16 erroneusTicks;

		static const int COMPRESSION_BUFFER_MAX = 800;

	public:
		BaseProtocol();

		virtual ~BaseProtocol() {
		}

		virtual void prepareSend(BasePacket* pack);

		void prepareSequence(BasePacket* pack);
		void prepareEncryptionAndCompression(BasePacket* pack);

		virtual bool processRecieve(Packet* pack);

		void encrypt(Packet* pack, bool crc);
		void decrypt(Packet* pack);

		bool compress(Packet* pack);
		void decompress(Packet* pack);

		unsigned int generateCrc(const Packet* pack, int len);
		void appendCRC(Packet* pack, sys::uint16 crcLength = 2);
		bool testCRC(const Packet* pack, sys::uint16 crcLength = 2);

		static unsigned int generateCRC(const Stream* stream, uint32 seed = 1);

		// setters
		inline void setConnectionID(sys::uint32 id) {
			connectionID = id;
		}

		inline void setSeed(sys::uint32 seed) {
			crcSeed = seed;
		}

		// getters
		inline sys::uint32 getConnectionID() const {
			return connectionID;
		}

		inline unsigned int getSeed() const {
			return crcSeed;
		}

	};

    } // namespace proto
  } // namespace service
} // namespace engine

using namespace engine::service::proto;

#endif /*BASEPROTOCOL_H_*/
