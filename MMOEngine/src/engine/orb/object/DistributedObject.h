/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef DISTRIBUTEDOBJECT_H_
#define DISTRIBUTEDOBJECT_H_

#include "system/lang.h"
#include <atomic>

#include "DistributedObjectPOD.h"

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

		class UpdatedHelper {
		public:
			DistributedObject* obj = nullptr;
			std::atomic<bool> _updated{true};
			AtomicReference<StackTrace*> lastModifiedTrace = nullptr;

			~UpdatedHelper();

			explicit operator bool() const {
				return _updated.load(std::memory_order_relaxed);
			}

			UpdatedHelper& operator=(bool val);

			void clearTrace();
		};

		class DeleteHelper {
		public:
			DistributedObject* obj = nullptr;
			std::atomic<bool> _delete{false};

			explicit operator bool() const {
				return _delete.load(std::memory_order_relaxed);
			}

			DeleteHelper& operator=(bool val);
		};

		UpdatedHelper _updated;
		DeleteHelper _markedForDeletion;
		bool _deletedFromDatabase;

	public:
		DistributedObject();

		virtual ~DistributedObject();

		virtual bool isPersistent() const {
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
			return _updated._updated.load(std::memory_order_relaxed);
		}

		inline bool _isMarkedForDeletion() const {
			return _markedForDeletion._delete.load(std::memory_order_relaxed);
		}

		inline const StackTrace* getLastModifiedTrace() const {
			return _updated.lastModifiedTrace;
		}
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*DISTRIBUTEDOBJECT_H_*/
