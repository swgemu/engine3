/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef RUDPSESSION_H_
#define RUDPSESSION_H_

#include "system/lang.h"

#include "engine/log/Logger.h"

#include "engine/service/ServiceSession.h"

namespace engine {
  namespace proto {
    namespace rudp {

    class RUDPSession : ServiceSession {
		sys::uint32 connectionID;

		unsigned int crcSeed;

	    sys::uint32 serverSequence;
	    sys::uint32 clientSequence;

		Time lastNetStatusTimeStamp;

    public:
		RUDPSession() {
			crcSeed = 0;
		}

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

    };

    } // namespace rudp
  } // namespace proto
} // namespace engine

using namespace engine::proto::rudp;

#endif /* RUDPSESSION_H_ */
