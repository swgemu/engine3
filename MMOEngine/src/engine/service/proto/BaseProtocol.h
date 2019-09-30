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
		sys::uint32 connectionID = 0;

		unsigned int crcSeed = 0;

		sys::uint32 serverSequence = 0;
		sys::uint32 clientSequence = 0;

		Time lastNetStatusTimeStamp;
		uint16 lastRecievedNetStatusTick = 0;
		uint16 erroneusTicks = 0;

		static const int COMPRESSION_BUFFER_MAX = 800;

	public:
		BaseProtocol();

		virtual ~BaseProtocol() {
		}

		virtual void prepareSend(BasePacket* pack);

		void prepareSequence(BasePacket* pack);
		void prepareEncryptionAndCompression(BasePacket* pack);

		virtual bool processRecieve(Packet* pack);

		void encrypt(Packet* pack, bool crc) const;
		void decrypt(Packet* pack) const;

		bool compress(Packet* pack) const;
		void decompress(Packet* pack) const;

		unsigned int generateCrc(const Packet* pack, int len) const;
		void appendCRC(Packet* pack, sys::uint16 crcLength = 2) const;
		bool testCRC(const Packet* pack, sys::uint16 crcLength = 2) const;

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
