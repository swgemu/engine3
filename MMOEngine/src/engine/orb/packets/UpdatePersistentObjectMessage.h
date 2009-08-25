/*
 * UpdatePersistentObjectMessage.h
 *
 *  Created on: 21/08/2009
 *      Author: victor
 */

#ifndef UPDATEPERSISTENTOBJECTMESSAGE_H_
#define UPDATEPERSISTENTOBJECTMESSAGE_H_

namespace engine {
  namespace ORB {

	  class UpdatePersistentObjectMessage : public Packet {
	  public:
		  UpdatePersistentObjectMessage(DistributedObject* object) : Packet(40) {
			  insertInt(0x09);
			  insertLong(object->_getObjectID());
		  }

		  static uint64 parseObjectID(Packet* pack) {
			  return pack->parseLong();
		  }
	  };

  }
}

using namespace engine::ORB;


#endif /* UPDATEPERSISTENTOBJECTMESSAGE_H_ */
