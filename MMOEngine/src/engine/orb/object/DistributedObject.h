/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef DISTRIBUTEDOBJECT_H_
#define DISTRIBUTEDOBJECT_H_

#include "system/lang.h"

namespace engine {
  namespace ORB {

	class ObjectBroker;

	class DistributedObjectAdapter;
	class DistributedObjectStub;

	class DistributedObject : public ReadWriteLock, public Object {
	protected:
		String _className;
		String _name;

		uint64 _objectID;

		ObjectBroker* _objectBroker;

		bool _updated;
		bool _markedForDeletion;
		bool _deletedFromDatabase;

	public:
		DistributedObject();

		virtual ~DistributedObject();
		
		virtual bool isPersistent() {
  		    return false;
		}		

		// setters
		inline void _setClassName(const String& n) {
			_className = n;
		}

		inline void _setName(const String& n) {
			_name = n;
		}

		inline void _setObjectID(uint64 id) {
			_objectID = id;
		}

		inline void _setUpdated(bool var) {
			_updated = var;
		}
		
		inline void _setDeletedFromDatabase(bool val) {
		        _deletedFromDatabase = val;
		}

		inline void _setMarkedForDeletion(bool var) {
			_markedForDeletion = var;
		}

		inline void _setObjectBroker(ObjectBroker* broker) {
			_objectBroker = broker;
		}

		// getters
		inline const String& _getClassName() const {
			return _className;
		}
		
		inline bool _isDeletedFromDatabase() const {
			return _deletedFromDatabase;
		}

		inline const String& _getName() const {
			return _name;
		}

		inline uint64 _getObjectID() const {
			return _objectID;
		}

		inline ObjectBroker* _getObjectBroker() const {
			return _objectBroker;
		}

		inline bool _isUpdated() const {
			return _updated;
		}

		inline bool _isMarkedForDeletion() const {
			return _markedForDeletion;
		}
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*DISTRIBUTEDOBJECT_H_*/
