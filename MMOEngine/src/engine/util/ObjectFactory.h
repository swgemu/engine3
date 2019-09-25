/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

///////////////////////////////////////////////////////////////////////////////
//
// ObjectFactory
//
// The ObjectFactory class is a object factory implementation.  It allows users
// to register and unregister classes during run-time by specifying a
// user-defined unique identifier per class.  Instances of any registered class
// can then be instantiated simply by calling the create method and passing the
// proper unique identifier.
//
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2004 Robert Geiman.
//
// Permission to copy, modify, and use this code for personal and commercial
// software is granted provided this copyright notice appears in all copies.
// This software is provided "as is" without any expressed or implied warranty.
//
// Any comments or questions can be sent to: rgeiman@buckeye-express.com
//
///////////////////////////////////////////////////////////////////////////////


#ifndef OBJECTFACTORY_H_
#define OBJECTFACTORY_H_

#include "system/lang.h"

namespace engine {
	namespace util {

	template <typename CtorSignature, typename UniqueIdType> class ObjectFactory;

	template<typename BaseClassType, typename ClassType> BaseClassType CreateObject() {
		return new ClassType();
	}

	template<typename BaseClassType, typename UniqueIdType>
	class ObjectFactory<BaseClassType (), UniqueIdType>
	{
	protected:
		typedef BaseClassType (*CreateObjectFunc)();

	public:
		BaseClassType createObject(UniqueIdType uniqueID) const {
			if (!objectCreator.containsKey(uniqueID))
				return nullptr;

			return objectCreator.get(uniqueID)();
		}

		template<typename ClassType> bool registerObject(UniqueIdType uniqueID) {
			if (objectCreator.containsKey(uniqueID))
				return false;

			objectCreator.put(uniqueID, &CreateObject<BaseClassType, ClassType>);

			return true;
		}

		bool unregisterObject(UniqueIdType uniqueID) {
			return objectCreator.drop(uniqueID);
		}

		bool containsObject(UniqueIdType uniqueID) const {
			return objectCreator.containsKey(uniqueID);
		}

	protected:
		HashTable<UniqueIdType, CreateObjectFunc> objectCreator;
	};

	template<typename BaseClassType, typename Param1Type, typename ClassType> BaseClassType CreateObject(Param1Type param1) {
		return new ClassType(param1);
	}

	template<typename BaseClassType, typename Param1Type, typename UniqueIdType>
	class ObjectFactory<BaseClassType (Param1Type), UniqueIdType>
	{
	protected:
		typedef BaseClassType (*CreateObjectFunc)(Param1Type);

	public:
		BaseClassType createObject(UniqueIdType uniqueID, Param1Type param1) const {
			if (!objectCreator.containsKey(uniqueID))
				return nullptr;

			return objectCreator.get(uniqueID)(param1);
		}

		template<typename ClassType> bool registerObject(UniqueIdType uniqueID) {
			if (objectCreator.containsKey(uniqueID))
				return false;

			objectCreator.put(uniqueID, &CreateObject<BaseClassType, Param1Type, ClassType>);

			return true;
		}

		bool unregisterObject(UniqueIdType uniqueID) {
			return objectCreator.drop(uniqueID);
		}

		bool containsObject(UniqueIdType uniqueID) const {
			return objectCreator.containsKey(uniqueID);
		}

	protected:
		HashTable<UniqueIdType, CreateObjectFunc> objectCreator;
	};

	template<typename BaseClassType, typename Param1Type, typename Param2Type, typename ClassType> BaseClassType CreateObject(Param1Type param1, Param2Type param2) {
		return new ClassType(param1, param2);
	}

	template<typename BaseClassType, typename Param1Type, typename Param2Type, typename UniqueIdType>
	class ObjectFactory<BaseClassType (Param1Type, Param2Type), UniqueIdType>
	{
	protected:
		typedef BaseClassType (*CreateObjectFunc)(Param1Type, Param2Type);

	public:
		BaseClassType createObject(UniqueIdType uniqueID, Param1Type param1, Param2Type param2) const {
			if (!objectCreator.containsKey(uniqueID))
				return nullptr;

			return objectCreator.get(uniqueID)(param1, param2);
		}

		template<typename ClassType> bool registerObject(UniqueIdType uniqueID) {
			if (objectCreator.containsKey(uniqueID))
				return false;

			objectCreator.put(uniqueID, &CreateObject<BaseClassType, Param1Type, Param2Type, ClassType>);

			return true;
		}

		bool unregisterObject(UniqueIdType uniqueID) {
			return objectCreator.drop(uniqueID);
		}

		bool containsObject(UniqueIdType uniqueID) const {
			return objectCreator.containsKey(uniqueID);
		}

	protected:
		HashTable<UniqueIdType, CreateObjectFunc> objectCreator;
	};

	template<typename BaseClassType, typename Param1Type, typename Param2Type, typename Param3Type, typename ClassType> BaseClassType CreateObject(Param1Type param1, Param2Type param2, Param3Type param3) {
		return new ClassType(param1, param2, param3);
	}

	template<typename BaseClassType, typename Param1Type, typename Param2Type, typename Param3Type, typename UniqueIdType>
	class ObjectFactory<BaseClassType (Param1Type, Param2Type, Param3Type), UniqueIdType>
	{
	protected:
		typedef BaseClassType (*CreateObjectFunc)(Param1Type, Param2Type, Param3Type);

	public:
		BaseClassType createObject(UniqueIdType uniqueID, Param1Type param1,
				Param2Type param2, Param3Type param3) const {
			if (!objectCreator.containsKey(uniqueID))
				return nullptr;

			return objectCreator.get(uniqueID)(param1, param2, param3);
		}

		template<typename ClassType> bool registerObject(UniqueIdType uniqueID) {
			if (objectCreator.containsKey(uniqueID))
				return false;

			objectCreator.put(uniqueID, &CreateObject<BaseClassType, Param1Type, Param2Type, Param3Type, ClassType>);

			return true;
		}

		bool unregisterObject(UniqueIdType uniqueID) {
			return objectCreator.drop(uniqueID);
		}

		bool containsObject(UniqueIdType uniqueID) const {
			return objectCreator.containsKey(uniqueID);
		}

	protected:
		HashTable<UniqueIdType, CreateObjectFunc> objectCreator;
	};

	template<typename BaseClassType, typename Param1Type, typename Param2Type, typename Param3Type, typename Param4Type, typename ClassType> BaseClassType CreateObject(Param1Type param1, Param2Type param2, Param3Type param3, Param4Type param4) {
		return new ClassType(param1, param2, param3, param4);
	}

	template<typename BaseClassType, typename Param1Type, typename Param2Type, typename Param3Type, typename Param4Type, typename UniqueIdType>
	class ObjectFactory<BaseClassType (Param1Type, Param2Type, Param3Type, Param4Type), UniqueIdType>
	{
	protected:
		typedef BaseClassType (*CreateObjectFunc)(Param1Type, Param2Type, Param3Type, Param4Type);

	public:
		BaseClassType createObject(UniqueIdType uniqueID, Param1Type param1,
				Param2Type param2, Param3Type param3, Param4Type param4) const {
			if (!objectCreator.containsKey(uniqueID))
				return nullptr;

			return objectCreator.get(uniqueID)(param1, param2, param3, param4);
		}

		template<typename ClassType> bool registerObject(UniqueIdType uniqueID) {
			if (objectCreator.containsKey(uniqueID))
				return false;

			objectCreator.put(uniqueID, &CreateObject<BaseClassType, Param1Type, Param2Type, Param3Type, Param4Type, ClassType>);

			return true;
		}

		bool unregisterObject(UniqueIdType uniqueID) {
			return objectCreator.drop(uniqueID);
		}

		bool containsObject(UniqueIdType uniqueID) const {
			return objectCreator.containsKey(uniqueID);
		}

	protected:
		HashTable<UniqueIdType, CreateObjectFunc> objectCreator;
	};

	}
}

using namespace engine::util;

#endif /* OBJECTFACTORY_H_ */
