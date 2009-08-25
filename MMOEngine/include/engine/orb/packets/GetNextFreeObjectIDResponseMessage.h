/*
 * GetNextFreeObjectIDResponseMessage.h
 *
 *  Created on: 22/08/2009
 *      Author: victor
 */

#ifndef GETNEXTFREEOBJECTIDRESPONSEMESSAGE_H_
#define GETNEXTFREEOBJECTIDRESPONSEMESSAGE_H_

namespace engine {
  namespace ORB {

	class GetNextFreeObjectIDResponseMessage : public Packet {
	public:
		GetNextFreeObjectIDResponseMessage(uint64 objectID) : Packet(40) {
			insertLong(objectID);
		}

		static uint64 parseObjectID(Packet* pack) {
			return pack->parseLong();
		}
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;


#endif /* GETNEXTFREEOBJECTIDRESPONSEMESSAGE_H_ */
