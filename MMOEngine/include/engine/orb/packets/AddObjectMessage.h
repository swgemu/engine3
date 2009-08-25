/*
 * AddObjectMessage.h
 *
 *  Created on: 21/08/2009
 *      Author: victor
 */

#ifndef ADDOBJECTMESSAGE_H_
#define ADDOBJECTMESSAGE_H_

namespace engine {
  namespace ORB {

	class AddObjectMessage : public Packet {
	public:
		AddObjectMessage(DistributedObjectStub* object) : Packet(40) {
			/*insertInt(0x03);
			insertAscii(object->);
			insertAscii(classname);*/
		}

		static void parseObjectName(Packet* pack, String& name) {
			pack->parseAscii(name);
		}
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;


#endif /* ADDOBJECTMESSAGE_H_ */
