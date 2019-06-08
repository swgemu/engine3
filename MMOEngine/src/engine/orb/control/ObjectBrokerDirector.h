/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef OBJECTBROKERDIRECTOR_H_
#define OBJECTBROKERDIRECTOR_H_

#include "engine/util/Singleton.h"

#include "engine/orb/ObjectBroker.h"

#include "ObjectBrokerTable.h"

namespace engine {
  namespace ORB {

  	class ObjectBrokerDirector;

	class ObjectBrokerDirector : public Logger, public Mutex, public Singleton<ObjectBrokerDirector>, public Object {
		 ObjectBrokerTable objectBrokerTable;
	
		 VectorMap<ObjectBroker*, int> agentStates;

	public:
		 enum Command { CREATE_BACKUP };

	public:
		ObjectBrokerDirector();
		~ObjectBrokerDirector();
		
		void start();

		void createBackup();

		void handleStateUpdate(ObjectBroker* broker, int state);

		void brokerConnected(ObjectBroker* broker);
	
		void brokerDisconnected(ObjectBroker* broker);

	private:
		void sendCommand(Command command);

		void doStateUpdate(int state);

		static const char* commandToString(int state);

		friend class ObjectBrokerAgent;
		friend class SingletonWrapper<ObjectBrokerDirector>;
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*OBJECTBROKERDIRECTOR_H_*/
