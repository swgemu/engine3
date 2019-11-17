/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef OBJECTBROKERAGENT_H_
#define OBJECTBROKERAGENT_H_

#include "engine/util/Singleton.h"

#include "engine/orb/ObjectBroker.h"

#include "ObjectBrokerDirector.h"

namespace engine {
  namespace ORB {

	class ObjectBrokerAgent : public Logger, public Mutex, public Singleton<ObjectBrokerAgent>, public Object {
		Reference<DOBServiceClient*> directorPort;

	public:
		enum AgentState { STARTED, IDLE,
						  BACKUP_STARTED, BACKUP_FINISHED };

	public:
		ObjectBrokerAgent();
		~ObjectBrokerAgent();

		void start();

		void startBackup(bool forceFull);
		void finishBackup();

		void doCommand(ObjectBrokerDirector::Command command);

		void setState(AgentState state);

	protected:
		static const char* stateToString(int state);

		friend class ObjectBrokerDirector;
		friend class SingletonWrapper<ObjectBrokerAgent>;
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*OBJECTBROKERAGENT_H_*/
