/*
 * GetNextFreeObjectID.h
 *
 *  Created on: 22/08/2009
 *      Author: victor
 */

#ifndef GETNEXTFREEOBJECTIDMESSAGE_H_
#define GETNEXTFREEOBJECTIDMESSAGE_H_

#include "system/lang.h"

namespace engine {
  namespace ORB {

	class GetNextFreeObjectIDMessage : public Packet {
	public:
		GetNextFreeObjectIDMessage(bool doLock) : Packet(40) {
			insertInt(0x0B);
			insertBoolean(doLock);
		}

		static bool parseDoLock(Packet* pack) {
			return pack->parseBoolean();
		}
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;


#endif /* GETNEXTFREEOBJECTID_H_ */
