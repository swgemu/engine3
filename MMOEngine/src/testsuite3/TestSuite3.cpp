/*
 * TestSuite3.cpp
 *
 *  Created on: 05/11/2011
 *      Author: victor
 */

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>
#include <boost/accumulators/statistics/tail_quantile.hpp>

#include "engine/engine.h"
#include "tests/memprottest.h"
#include "tests/stmtest.h"
#include "tests/stmmemtest.h"
//#include "tests/qtstmtest.h"
#include "tests/referencetest.h"
#include "tests/allocatorhook.h"
#include "tests/qtboundingtest.h"
#include "tests/timeserialize.h"
#include "tests/loggertest.h"
#include "tests/orbtest.h"
#include "tests/lowerbound.h"
#include "tests/udptest.h"
#include "tests/sizeoftest.h"
#include "tests/hashtableiterator.h"
#include "tests/weakreftest.h"
#include "tests/regexptest.h"
#include "tests/ZoneServer.h"
#include "tests/NavWorker.h"

#include "engine/db/mysql/MySqlDatabase.h"
#include "engine/core/MetricsManager.h"

#include <boost/any.hpp>
#include <boost/regex.hpp>


float frand() {
	return System::getMTRand()->randExc();
}

class TestCore : public Object, public ReadWriteLock {
	int ebatsia;
public:
	TestCore(int log) : ebatsia(log) {

	}

	void initialize() {
	}

	void run() {

	}

	void setHuisia(int a) REQUIRES(this) {
		ebatsia = a;
	}

	int getEbatsia() REQUIRES_SHARED(this) {
		return ebatsia;
	}
};

void huiesa(TestCore* core) REQUIRES(core) {
	core->setHuisia(11);
}


class EbisiVRot {
public:
	void huisia() const {
		printf("bla");
	}
};


class NeNado {
	EbisiVRot* hui;

public:
	NeNado() {
		hui = new EbisiVRot();
	}

	void mda() {
		hui->huisia();
	}
};


template <typename T>
class data_filler
{
public:
  data_filler(){}
  T operator()() { return rand()/(T)RAND_MAX; }
};

class TestDtor {
public:
	TestDtor() {

	}

	~TestDtor() {
		printf("deleting test dtor\n");
	}
};


void testDestructor(void* value) {
	printf("main dtor\n");

	delete reinterpret_cast<int*>(value);
}


template<typename T>
class Bla {
	T* huisia;

public:
	Bla() : huisia(nullptr) {

	}

	Bla(std::initializer_list<T> v)  {
		huisia = (T*)malloc(sizeof(T) * v.size());

		int i = 0;
		for (auto& a : v) {
			huisia[i++] = a;
		}
	}
};

class TestSuiteCore : public Core {
public:
	void initialize() {

	}

	void run() {

	}
};


class SizeTest { //: public Variable {
public:
	union {
		char* value;
		char sso[SSO_SIZE];
	};

	int count;




	bool toBinaryStream(ObjectOutputStream* stream) {
		return true;
	}

	bool parseFromBinaryStream(ObjectInputStream* stream) {
		return true;
	}
};

class PersistentObjectManager : public DOBObjectManager {
public:
	PersistentObjectManager() {
		databaseManager = ObjectDatabaseManager::instance();
		databaseManager->loadDatabases(false);

		databaseManager->loadObjectDatabase("testobjects", true, 0);
	}
};

void printValue(TestIDLClass* value) {
	int valueInsideObject = value->getValue();

	printf("value is:%d\n", valueInsideObject);
}

ZoneServerImplementation::ZoneServerImplementation(int bla) {
	_initializeImplementation();
}

Vector3 testDirOriginal() {
	Vector3 pos(1, 2, 3);
	Quaternion quat;

	float length = pos.length();

	float angle = quat.getRadians();

	float posX = pos.getX() + (::sin(angle) * length);
	float posY = pos.getY() + (::cos(angle) * length);
	float posZ = pos.getZ() + length;

	return Vector3(posX, posY, posZ);
}

Vector3 testDirNew() {
	Vector3 pos(1, 2, 3);
	Quaternion quat;

	float length = pos.length();

	float angle = quat.getRadians();

	float posX = pos.getX() + (::sin(angle) * length);
	float posY = pos.getY() + (::cos(angle) * length);
	float posZ = pos.getZ() + length;

	return Vector3(posX, posY, posZ);
}

DistributedObjectBroker* runServerCode() {
	DistributedObjectBroker* orb = DistributedObjectBroker::initialize("", 44431);
	auto objectManager = new PersistentObjectManager();
	orb->setCustomObjectManager(objectManager);
	objectManager->scheduleUpdateToDatabase();

	Reference<TestIDLClass*> obj = new TestIDLClass(23);
	obj->deploy("TestObjectServer");

	Reference<ZoneServer*> srv = new ZoneServer(23);
	srv->deploy("ZoneServer");

	Reference<TestIDLClass*> parent = new TestIDLClass(1337);
	parent->deploy("TestObjectServerParent");
	obj->setParent(parent);

	printf("parent created with oid: %d\n", (int) parent->_getObjectID());

	printf("TestObjectServer oid in the server:%d\n", (int) obj->_getObjectID());

	Core::getTaskManager()->scheduleTask([=]() {
		printf("trying to see value after moving servant\n");

		printValue(obj.get());

		printf("requesting servant back\n");

		obj->_requestServant();

		printValue(obj.get());

		auto vector = obj->getTestVector(); // this is local

		for (auto a : *vector) {
			printf("value in vector is:%d\n", a);
		}

		auto blaParent = obj->getParent().get();

		if (blaParent != nullptr) {
			printf("blaParent not null on server\n");

			blaParent->testCallback();

			blaParent->setValue(-48);

			printf("value:%d\n", blaParent->getValue());
		} else {
			printf("blaParent null on server\n");
		}

		//auto value = orb->lookUp("TestObjectClient").castTo<TestIDLClass*>();

		//printf("test object client oid in server:%d\n", value->_getObjectID());

		//printValue(value.get());
	}, "testTask", 5000);


	std::string runLoop;

	while (runLoop != "stop") {
		Thread::sleep(250);

		std::cout << "> ";
		std::cin >> runLoop;
	}

	return NULL;

}

void TestIDLClassImplementation::testCallback() {
	auto obj = Core::getObjectBroker()->lookUp("ZoneServer");

	if (obj != nullptr) {
		printf("zone server not null in testCallback\n");
	} else {
		printf("zone server IS null in testCallback\n");
	}
}

void TestIDLClassImplementation::setBlaParent(TestIDLClass* val) {
	printf("set bla parent:%p\n", val);

	blaParent = val;
}

void TestIDLClassImplementation::moveImplementation() {
	_this.get()->_requestServant();
}

AtomicLong uniqueID;

DistributedObjectBroker* runClientCode() {
	DistributedObjectBroker* orb = DistributedObjectBroker::initialize("127.0.0.1", 44431);
	orb->setCustomObjectManager(new DOBObjectManager());


	for (int i = 0; i < 2; ++i) {
		Core::getTaskManager()->executeTask([]() {
			System::out << "starting creating objects..." << endl;

			Time ebatsiaTime;

			for (int j = 0; j < 100000; ++j) {
				Reference<TestIDLClass*> test = new TestIDLClass(System::random());
				test->deploy();

				assert(test->_getObjectID());
			}

			System::out << "finished creating objects in " << ebatsiaTime.miliDifference() << endl;
		}, "testTask");
	}

	for (int i = 0; i < 10000; ++i) {
		Reference<TestIDLClass*> testPersistence = new TestIDLClass(1234455);
		testPersistence->setPersistent(1);
		testPersistence->deploy();
	}

	auto wrongOid = orb->lookUp(1432).castTo<ZoneServer*>();

	auto zoneServerTest = orb->lookUp("ZoneServer").castTo<ZoneServer*>();
	printf("ZoneServer oid in the client:%d\n", (int) zoneServerTest->_getObjectID());

	Reference<TestIDLClass*> testClientObject = new TestIDLClass(99);
	testClientObject->deploy("TestObjectClient" + String::valueOf(System::random()));

	printf("test object client oid:%d\n", (int)testClientObject->_getObjectID());


	auto value = orb->lookUp("TestObjectServer").castTo<TestIDLClass*>();
	Reference<TestIDLClass*> testServerParent = new TestIDLClass(99110);
	testServerParent->deploy();

	printf("TestObjectServer oid in the client:%d\n", (int) value->_getObjectID());
	printf("TestObjectServer parent oid in the client:%d\n", (int) testServerParent->_getObjectID());

	ManagedReference<TestIDLClass*> parent = value->getParent();

	printf("parent pointer in the client before changing it in the client:%p\n", parent.get());
	printf("parent oid in the client:%d\n", (int)parent->_getObjectID());


	value->setParent(testServerParent);
	value->setBlaParent(value);

	parent = value->getParent();

	printf("parent pointer in the client:%p\n", parent.get());
	printf("parent oid in the client:%d\n", (int)parent->_getObjectID());

	printValue(parent);

	int persistenceLevel = value->getPersistenceLevel();

	printf("persistence level is:%d", persistenceLevel);

	uint64_t oid = value->_getObjectID();

	printf("oid is:%d\n", (int)oid);

	printValue(value);

	value->asyncTest();

	value->setValue(21);

	printValue(value);

	/*printf("requesting servant..\n");

	value->_requestServant();

	auto vector = value->getTestVector(); // this is local

	for (auto a : *vector) {
		printf("value in vector is:%d\n", a);
	}

	vector->add(1337);*/

	parent = value->getParent();

	printf("parent pointer in the client:%p\n", parent.get());
	printf("parent oid in the client:%d\n", (int)parent->_getObjectID());

	value->setValue(1);

	printValue(value);

	std::string runLoop;

	while (runLoop != "stop") {
		std::cout << "> ";
		std::cin >> runLoop;


		Thread::sleep(250);
	}

	return NULL;
}


class A {
public:
	const static bool bla = false;
};


class B : public A {
public:
	const static bool bla = true;
};


class C : public B {

};


template<class Ba, class Ob, bool virt>
class Help {
public:
	void static work() {
		/*if (virt) {
			Object* obj = nullptr;
			auto a = dynamic_cast<BaseClient*>(obj);
		} else {
			Object* obj = nullptr;
			auto a = static_cast<BaseClient*>(obj);
		}*/
	}
};

template<class Ba, class Ob>
class Help<Ba, Ob, false> {
public:
	void static work() {
		//printf("bla");
		Ob obj = new Object();
		auto a = static_cast<Ba>(obj);
		a->acquire();
	}
};

template<class Ba, class Ob>
class Help<Ba, Ob, true> {
public:
	void static work() {
		//printf("ne boisia");
		Ob obj = new Object();
		auto a = dynamic_cast<Ba>(obj);
		a->acquire();
	}
};

#define WTFSIZE 2
const void* wtfTable[WTFSIZE + 1] = {
		[0 ... WTFSIZE] = nullptr,

};

class StringSizeTest {
protected:
	union {
		char* value;
		char sso[16];
	};

	uint32 count;
};

class CompareTest : public Object {
	const char* bla;
public:

	int compareTo(const CompareTest* object) const {
		if (this == object)
			return 0;
		else if (this < object)
			return 1;
		else
			return -1;
	}

	void setBla(const char* b) {
		bla = b;
	}
};


class ShadowBase {
public:
	int doCombatAction(Object* obj, const uint64_t& target, const UnicodeString& args = "", Reference<Object*> weapon = NULL) {
		return (int) target * 3;
	}
};

class ShadowTest : public ShadowBase {
public:
	int doCombatAction(Object* obj, const uint64_t& target, const UnicodeString& args = "") {
		return (int) target * 0;
	}
};

class ConditionClass : public Condition, public Thread {
public:
	Mutex condMutex;


	void wake() {
		condMutex.lock();

		signal(&condMutex);

		condMutex.unlock();
	}


	void run() {
		condMutex.lock();

		Time futureTime;
		futureTime.addMiliTime(10000);

		int res = timedWait(&condMutex, &futureTime);

		printf("res is %d\n", res);

		condMutex.unlock();
	}
};

int hashTest(uint64_t keyValue) {
	uint64 key = keyValue;

	key = (~key) + (key << 18); // key = (key << 18) - key - 1;
	key = key ^ (key >> 31);
	key = key * 21;             // key = (key + (key << 2)) + (key << 4);
	key = key ^ (key >> 11);
	key = key + (key << 6);
	key = key ^ (key >> 22);
	return (int) key;
}

String formatDotsADS(uint64 n)  {
	int c;
	char buf[128];
	char *p;
	char outBuf[128];
	char *out;

	sprintf(buf, "%llu", n);
	c = 2 - strlen(buf) % 3;
	for (p = buf, out = outBuf; *p != 0; p++) {
		*out++ = *p;
		if (c == 1) {
			*out++ = '.';
		}
		c = (c + 1) % 3;
	}
	*--out = 0;

	return String(outBuf);
}

class TestGen {
public:
	int bla;
	String blaString;

	template <class V>
	V& getInternalBla() {
		throw Exception();
	};

	template <class V>
	V& getInternalBlaString() {
		throw Exception();
	};

	template<class V>
	V& getTemplatedValue(const String& var) {
		uint32 hash = var.hashCode();

		switch (hash) {
			case STRING_HASHCODE("bla"): {
				return getInternalBla<V>();
			}
			case STRING_HASHCODE("blaString"): {
				return getInternalBlaString<V>();
			}
			default: {
				break;
			}
		}

		throw Exception();
	}

	boost::any getValue(const String& var) {
		static boost::any emptyValue;

		uint32 hash = var.hashCode();

		switch (hash) {
			case STRING_HASHCODE("bla"): {
				return bla;
			}
			case STRING_HASHCODE("blaString"): {
				return blaString;
			}
			default: {
				//error invalid var
				return emptyValue;
			}
		}

		return emptyValue;
	}
};

template <>
int& TestGen::getInternalBla() {
	return bla;
};

template <>
String& TestGen::getInternalBlaString() {
	return blaString;
};

int main(int argc, char* argv[]) {
	setbuf(stdout, 0);

	using namespace boost;

	const static boost::regex expression("[a-zA-z0-9._]*+[@]+[a-zA-Z0-9-]*+[.]+[a-zA-Z0-9]*");

	/*Quaternion q(Vector3(0, 1, 0), Math::deg2rad(degrees));

	Matrix3 transform = q.toMatrix3();

	transform.
	*/
	std::string email, username;
	std::string user = "mdamda";

	if (boost::regex_match(user, expression)) {
		email = user;

		//boost::algorithm::trim(email);
	} else {
		username = user;

		//boost::algorithm::trim(username);
	}


	Logger testLogger("test");
	Logger::setGlobalFileLogger("test.log");
	Logger::setGlobalFileLogLevel(Logger::INFO);

	TestGen test;
	test.bla = 3;
	test.blaString = "test string";

	int valTest2 = test.getTemplatedValue<int>("bla");

	int intValTest = any_cast<int>(test.getValue("bla"));
	String stringValTest = any_cast<String>(test.getValue("blaString"));

	testLogger.info(String::valueOf(intValTest));
	testLogger.info(stringValTest);
	testLogger.info(String::valueOf(valTest2));

	testLogger.info(String::valueOf((int)sizeof(Task)));
	testLogger.info(username.c_str());
	testLogger.info(email.c_str());
	//Logger::setGlobalFileLogLevel(Logger::LOG);

	//testLogger.setLogLevel(Logger::INFO);
	/*testLogger.info("info wut");
	testLogger.debug("debug wut");
	testLogger.warning("warning wut");
	testLogger.error("error wut");*/

	return 0;

/*
	auto stats = MetricsManager::instance();
	stats->initializeStatsDConnection("127.0.0.1", 8125);

	//deploys.test.myservice:1|c
	//const char* name, const char* value, const char* type, const char* sampleTime
	stats->publish("deployts.test.myservice", "1", "c");
	stats->publish("deployts.test.myservice", "1", "c");
	stats->publish("deployts.test.myservice", "1", "c");
	stats->publish("deployts.test.myservice", "1", "c");
	stats->publish("deployts.test.myservice", "1", "c");
	//stats->publish("name", "value2", "type");

	sleep(20000);



	return 0;*/
/*
	Time huiTime(123);

	huiTime.updateToCurrentTime();

	Vector3 randShit(3, 6, -1);

	printf("%f\n", randShit.product());

	Reference<Object*> huZero = new Object();

	auto testType = new HashTable<uint64, char*>();

	typedef std::remove_reference<decltype(*testType)>::type huiTypeAsad;
	huiTypeAsad::value_type testKey = nullptr;

	Reference<const Object*> testConst = huZero.get();

	const HashTable<uint32, String> blblabla;

	const auto& huiasdas = blblabla.get(123);

	printf("%ull %ull %ull %s\n" , INT_MAX, RAND_MAX, System::random(0), testKey);

	return 0;
	for (int i = 0; i < 100; ++i) {
		printf("%f\n", frand());
	}

	return 0;

	SortedVector<Reference<CompareTest*>> mdaTest;

	for (int i = 0; i < 10; ++i) {
		mdaTest.emplace(new CompareTest());
	}

	UnicodeString message = "\\><";

	UnicodeString text = message;

	int index = -1;

	while (text.indexOf("\\>") >= 0) {
		text = text.replaceFirst("\\>", "");
	}

	Vector3 blaValues[3];

	printf("%s %d %d %d %d\n", text.toString().toCharArray(), sizeof(blaValues), sizeof(String), sizeof(Object*), std::is_move_constructible<String>::value);

	VectorMap<uint64, String> mapTest;
	mapTest.put(2, "num2");
	mapTest.put(1, "num1");
	mapTest.put(5, "num5");

	for (const auto& entry : mapTest) {
		printf("%s\n", mapTest.get(entry.getKey()).toCharArray());
	}

	Vector<String> strings;
	strings.emplace("blablablablablablablablablablablablablablablablablablablablablablabla");
	strings.emplace("blablablablablablablablablablablablablablablablablablablablablablabla");
	strings.emplace("bla");
	strings.emplace("bblablablablablablablablablablablablablablablablablablablablablablablala");
	strings.emplace("bla");

	auto removedString = strings.remove(0);

	printf("%s\n", removedString.toCharArray());

	return 0;

	HashTable<uint64, String> ttHashtable;

	ttHashtable.put(1, "123");
	VectorMap<uint64, String> ttVectorMap;
	ttVectorMap.put(2, "245");

	printf("%s %s", ttHashtable.get(1).toCharArray(), ttVectorMap.get((uint64)2).toCharArray());

	return 0;
*/
	DOBObjectManager::setUpdateToDatabaseTime(30000);

	TestSuiteCore core;
	core.start();

	DistributedObjectBroker* orb;

	if (argc <= 1) {
		runClientCode();
	} else {
		runServerCode();
	}

	Core::getTaskManager()->getInfo(true);


	return 0;

/*
//	std::function<void(void*)> testC = [](void*) { printf("mda");};

//	printf("%s\n", typeid(testC).name());
//	printf("%s\n", typeid(testDestructor).name());
	//testShit([](void* bla){ delete (int*)bla;});
	//testShit(testDestructor);
	//testShit(testC);

	//assert(Core::getTaskManager();

	printf("%d\n", (int)sizeof(SizeTest));

	return 0;

	Core::getTaskManager()->initializeCustomQueue("huisia", 1);


	for (int i = 0; i < 100; ++i) {
		Core::getTaskManager()->executeTask([]() { printf("nu da %p\n", Thread::getCurrentThread()); }, "testLambda");
		Core::getTaskManager()->executeTask([]() { printf("nu da %p\n", Thread::getCurrentThread()); }, "testLambda");
		Core::getTaskManager()->executeTask([]() { printf("nu da %p\n", Thread::getCurrentThread()); }, "mdadma");
		Core::getTaskManager()->executeTask([]() { printf("huisia nu da %p\n", Thread::getCurrentThread()); },
											"testLambda", "huisia");


		Core::getTaskManager()->scheduleTask([]() { printf("nu da %p\n", Thread::getCurrentThread()); }, "testLambda",
											 10);
		Core::getTaskManager()->scheduleTask([]() { printf("huisia nu da %p\n", Thread::getCurrentThread()); },
											 "testLambda", 10, "huisia");

		Core::getTaskManager()->scheduleTask([]() { printf("nu da %p\n", Thread::getCurrentThread()); }, "testLambda",
											 3000);
	}

	Thread::sleep(4000);
//	core.

	Core::getTaskManager()->getInfo(true);

	Vector3 blaVector = {1, 2, 3};

	const static SortedVector<int> huiInitTest = {54, 2, 1 , 64};

	for (auto i : huiInitTest) {
		printf("%d\n", i);
	}

	String blaStr = "bla";

	printf("%d %s\n", (int)sizeof(blaStr), blaStr.toCharArray());


	return 0;

	Vector<int> vec;

	for (int i = 0; i < 10; ++i) {
		vec.add(System::random(100));
	}

	std::sort(vec.begin(), vec.end());

	for (auto i : vec) {
		printf("%d\n", i);
	}

	return 0;


	float bla = 5;

	bla = Math::clamp(0, 5, 4);

	printf("%f\n", bla);

	Vector<AABB> testingA;
	testingA.add(AABB());

	Reference<Object*> obj = new Object();

	Reference<Object*> obj2 = std::move(obj);

	VectorMap<uint64, Reference<Object*>> objects;

	for (int i = 0; i < 3; ++i) {
		objects.put(i, obj2);
	}

	//obj2->printReferenceHolders();

	objects.removeAll();

	TestCore* testCore = new TestCore(2);

	printf("%d %d %d %d\n", DefaultThreadLocalDestructor<int>::hasDestructor, DefaultThreadLocalDestructor<int*>::hasDestructor,
		   DefaultThreadLocalDestructor<TestDtor>::hasDestructor, DefaultThreadLocalDestructor<TestDtor*>::hasDestructor);

	ThreadLocal<TestDtor*> mda;

	mda.set(new TestDtor());

	Logger::console.info("hui", true);

	Core::getObjectBroker();

	delete testCore;


//	int* bla = new int(32);

//	printf("hui is %d\n", *bla);

//	ThreadLocal<int> mda;

//	mda.set(23);

	//printf("mda is %d\n", *mda.get());

	static int returnVal = 0;

	pthread_exit(&returnVal);

	return 0;

*/
	/*
using namespace boost::accumulators;
//typedef accumulator_set<double, stats<boost::accumulators::tag::tail_quantile<left> > > accumulator_t_left;
typedef accumulator_set<double, stats<boost::accumulators::tag::tail_quantile<right> > > accumulator_t_right;

  //create some random data
  //std::vector<double> data(1000000);
  //std::generate(data.begin(), data.end(), data_filler<double>());
  //int c = data.size();//cache size for histogramm.
	int c = 1000000;
  //create a accumulator.
  accumulator_t_right acc0( boost::accumulators::tag::tail<right>::cache_size = c * 5);
  //fill accumulator
  for (int j = 0; j < c * 5; ++j)
  {
      acc0(23);
  }
  //ask some questions...
  double tqueryP = quantile(acc0, quantile_probability = 0.50 );
  std::cout << tqueryP << std::endl;

  return 0;

	String testHui = "/usr/bin/man";

	int idx = testHui.lastIndexOf("/");

	String huiString = testHui.subString(idx + 1);

	printf("idx:%d hui:%s\n", idx, huiString.toCharArray());

	return 0; */


	/*File file("wakelocks");

	String line;

	FileRFeader reader(&file);


	while (reader.readLine(line)) {

	}*/


//	huiTest.get((uint32)12);

	/*float v[4];

	printf("%d\n", sizeof(v));

	Vector<Vector3> huiTest;

	typedef Vector<Vector3> PointList;

	huiTest.add(Vector3(1, 1, 1));
	huiTest.add(Vector3(2, 2, 2));

	PointList::reverse_iterator current;

	for (PointList::reverse_iterator iterator = huiTest.rbegin(); iterator != huiTest.rend(); ++iterator) {
		printf("hui:%f\n", (*iterator).getX());

		iterator->setX(9);
		current = iterator;

		printf("hui2:%f\n", (*current).getX());
	}*/

	/*struct sockaddr_in addr;

	struct addrinfo *result = NULL;

	int error = getaddrinfo("localhost", NULL, NULL, &result);

	if (error != 0 || !result) {
		Logger::console.error("getaddrinfo failed");

		addr.sin_family = 0;

		if (result)
			freeaddrinfo(result);

		throw SocketException("unknown host");
	}


	printf("%d sizeof %d addrlen\n", sizeof(addr), result->ai_addrlen);

	memmove(&addr, result->ai_addr, sizeof(addr));

	freeaddrinfo(result);*/

//	return 0;


	///

/*
	NeNado nenado;

	nenado.mda();

	uint16_t seq = 0;

	int acknowledgedServerSequence = 0x200000 - 5;

	uint32 serverSequence = acknowledgedServerSequence + 5;

	int32 realseq = seq;
	if (seq < acknowledgedServerSequence) {
		realseq = (seq & 0xFFFF) | (serverSequence & 0xFFFF0000);
	}

	if ((uint32)realseq > serverSequence) {
		realseq -= 0x10000;
	}

	printf("relseq: %d %x ack: %d %x\n", realseq, realseq, acknowledgedServerSequence, acknowledgedServerSequence);

	if (realseq < acknowledgedServerSequence) {
		printf("hui\n");
		return 1;
	}

	return 0;


	static_assert( std::is_move_constructible<UnicodeString>::value
			, "This won't trip" );

	String huitsia = "test";
	String replaceHui = "hui_replace_hui";

	String replace = huitsia.replaceFirst("test", "");

	System::out << huitsia << " and replaceed with:" << replace << endl;

	String test = replaceHui.replaceAll("hui", "nehui");

	System::out << "huiReplaced:" << test;

	TestCore davaiPei(34);

	//Locker spocker(&davaiPei);

	//davaiPei.wlock();

	//davaiPei.setHuisia(12);

	//spo

	//Locker peesa(&davaiPei);

	davaiPei.lock();

	davaiPei.setHuisia(21);

	huiesa(&davaiPei);

	davaiPei.unlock();

	//davaiPei.unlock();
*//*
	int lastIndexOf = huitsia.lastIndexOf("_light");
	int len = strlen("_light");
	int hlen = huitsia.length();

	bool result = huitsia.endsWith("t");

	int huilen = hlen - len;

	printf("%d %d %d %d %d\n", result, lastIndexOf, len, hlen, huilen);
*/
	return 0;


	try {
		SortedVector<String> arguments;

		for (int i = 1; i < argc; ++i) {
			arguments.put(argv[i]);
		}

		TestCore core(Logger::INFO);

		StackTrace::setBinaryName("testsuite3");

		if (!arguments.isEmpty()) {
			if (arguments.contains("stmtest"))
				testTransactions();
			else if (arguments.contains("memtest"))
				testMemoryProtection();
			else if (arguments.contains("stmmemtest"))
				testSTMMemory();
			else if (arguments.contains("qtstmtest")) {
				//testQTSTM();
			} else if (arguments.contains("referencetest")) {
				referenceTest();
			} else if (arguments.contains("memfiletest")) {
				testMemoryMappedFile();
			} else if (arguments.contains("qtboundingtest")) {
				testQTBounding();
			} else if (arguments.contains("timeserialize")) {
				testTimeSerialization();
			} else if (arguments.contains("loggertest")) {
				loggertest();
			} else if (arguments.contains("orbtestmaster")) {
				orbTestDeploy();
			} else if (arguments.contains("orbtest")) {
				orbTestLookup();
			} else if (arguments.contains("lowerbound")) {
				lowerBoundTest();
			} else if (arguments.contains("udptest")) {
				udptest();
			} else if (arguments.contains("sizeoftest")) {
				testObjectSizes();
			} else if (arguments.contains("hashtableiterator")) {
				testHashTableIterator();
			} else if (arguments.contains("weakreftest")) {
				weakreftest();
			} else if (arguments.contains("regexptest")) {
				regexptest();
			}
		}

	} catch (Exception& e) {
		System::out << e.getMessage() << "\n";
		e.printStackTrace();
	}

	return 0;
}
