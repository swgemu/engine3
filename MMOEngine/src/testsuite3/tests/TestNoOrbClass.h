/*
 *	testsuite3/tests/TestNoOrbClass.h generated by engine3 IDL compiler 0.70
 */

#ifndef TESTNOORBCLASS_H_
#define TESTNOORBCLASS_H_

#include "engine/core/Core.h"

#include "engine/core/ManagedReference.h"

#include "engine/core/ManagedWeakReference.h"

#include "system/util/Optional.h"

#ifndef likely
#ifdef __GNUC__
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)
#else
#define likely(x)       (x)
#define unlikely(x)     (x)
#endif
#endif
#include "engine/util/json_utils.h"

#include "system/util/Vector.h"

#include "engine/core/ManagedObject.h"

#include "engine/lua/LuaCallbackException.h"

namespace testsuite3 {
namespace tests {

class TestNoOrbClass : public ManagedObject {
public:
	DistributedObjectServant* _getImplementation();
	DistributedObjectServant* _getImplementationForRead() const;

	void _setImplementation(DistributedObjectServant* servant);

protected:
	TestNoOrbClass(DummyConstructorParameter* param);

	virtual ~TestNoOrbClass();

	friend class TestNoOrbClassHelper;
};

} // namespace tests
} // namespace testsuite3

using namespace testsuite3::tests;

namespace testsuite3 {
namespace tests {

class TestNoOrbClassImplementation : public ManagedObjectImplementation {

public:
	TestNoOrbClassImplementation();
	TestNoOrbClassImplementation(DummyConstructorParameter* param);

	WeakReference<TestNoOrbClass*> _this;

	operator const TestNoOrbClass*();

	DistributedObjectStub* _getStub();
	virtual void readObject(ObjectInputStream* stream);
	virtual void writeObject(ObjectOutputStream* stream);
protected:
	virtual ~TestNoOrbClassImplementation();

	void finalize();

	void _initializeImplementation();

	void _setStub(DistributedObjectStub* stub);

	void lock(bool doLock = true);

	void lock(ManagedObject* obj);

	void rlock(bool doLock = true);

	void wlock(bool doLock = true);

	void wlock(ManagedObject* obj);

	void unlock(bool doLock = true);

	void runlock(bool doLock = true);

	void _serializationHelperMethod();
	bool readObjectMember(ObjectInputStream* stream, const uint32& nameHashCode);
	int writeObjectMembers(ObjectOutputStream* stream);

	friend class TestNoOrbClass;
};

class TestNoOrbClassAdapter : public ManagedObjectAdapter {
public:
	TestNoOrbClassAdapter(TestNoOrbClass* impl);

	void invokeMethod(sys::uint32 methid, DistributedMethod* method);

};

class TestNoOrbClassHelper : public DistributedObjectClassHelper, public Singleton<TestNoOrbClassHelper> {
	static TestNoOrbClassHelper* staticInitializer;

public:
	TestNoOrbClassHelper();

	void finalizeHelper();

	DistributedObject* instantiateObject();

	DistributedObjectPOD* instantiatePOD();

	DistributedObjectServant* instantiateServant();

	DistributedObjectAdapter* createAdapter(DistributedObjectStub* obj);

	friend class Singleton<TestNoOrbClassHelper>;
};

} // namespace tests
} // namespace testsuite3

using namespace testsuite3::tests;

namespace testsuite3 {
namespace tests {

class TestNoOrbClassPOD : public ManagedObjectPOD {
public:

	TestNoOrbClassPOD();
	virtual void readObject(ObjectInputStream* stream);
	virtual void writeObject(ObjectOutputStream* stream);
	bool readObjectMember(ObjectInputStream* stream, const uint32& nameHashCode);
	int writeObjectMembers(ObjectOutputStream* stream);
	void writeObjectCompact(ObjectOutputStream* stream);



	virtual ~TestNoOrbClassPOD();

};

} // namespace tests
} // namespace testsuite3

using namespace testsuite3::tests;

#endif /*TESTNOORBCLASSPOD_H_*/
