/*
 * UpdatePersistentObjectResponseMessage.h
 *
 *  Created on: 21/08/2009
 *      Author: victor
 */

#ifndef UPDATEPERSISTENTOBJECTRESPONSEMESSAGE_H_
#define UPDATEPERSISTENTOBJECTRESPONSEMESSAGE_H_

namespace engine {
  namespace ORB {

	  class UpdatePersistentObjectResponseMessage : public Packet {
	  public:
		  UpdatePersistentObjectResponseMessage(int succes) : Packet(40) {
			  insertInt(succes);
		  }

		  static int parseReturnValue(Packet* pack) {
			  return pack->parseInt();
		  }
	  };

  }
}

using namespace engine::ORB;


#endif /* UPDATEPERSISTENTOBJECTRESPONSEMESSAGE_H_ */
