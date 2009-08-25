/*
 * LoadPersistentObjectMessage.h
 *
 *  Created on: 20/08/2009
 *      Author: victor
 */

#ifndef LOADPERSISTENTOBJECTMESSAGE_H_
#define LOADPERSISTENTOBJECTMESSAGE_H_

namespace engine {
  namespace ORB {

	  class LoadPersistentObjectMessage : public Packet {
	  public:
		  LoadPersistentObjectMessage(uint64 objectid) : Packet(40) {
			  insertInt(0x07);
			  insertLong(objectid);
		  }

		  static uint64 parseObjectID(Packet* pack) {
			  return pack->parseLong();
		  }
	  };

  }
}

using namespace engine::ORB;


#endif /* LOADPERSISTENTOBJECTMESSAGE_H_ */
