/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * TestSuite3.cpp
 *
 *  Created on: 05/11/2011
 *      Author: victor
 */

#include "engine/engine.h"
#include "tests/memprottest.h"
#include "tests/stmtest.h"
#include "tests/stmmemtest.h"
//#include "tests/qtstmtest.h"
#include "tests/referencetest.h"
#include "tests/allocatorhook.h"
//#include "tests/qtboundingtest.h"
#include "tests/timeserialize.h"
#include "tests/loggertest.h"
#include "tests/orbtest.h"
#include "tests/lowerbound.h"
#include "tests/udptest.h"
#include "tests/sizeoftest.h"
#include "tests/hashtableiterator.h"
#include "tests/weakreftest.h"
#include "tests/regexptest.h"

#include "system/thread/atomic/AtomicInteger.h"
#include "system/thread/atomic/AtomicTime.h"

#include "engine/util/Observable.h"

class TestCore : public Core {
	AtomicInteger ha;
public:
	TestCore(int log) : Core(log) {

	}

	void initialize() {
	}

	void run() {
		for (auto i = 0; i < 5; ++i) {
			System::out << "ok then" << ha << endl;

			Core::getTaskManager()->scheduleTask([this] () { ha.increment(); System::out << "test" << ha << endl; }, "TEstTask", 800);

			//Thread::sleep(1000);
		}
	}

};

class TestCShit {
public:


	static void mda(ConstString a) {
		printf("aaaaaaaaaaaaaaaaaaa\n");
	}

	static void mda(const char* a) {
		printf("bbbbbbbbbbbbbbbbbb\n");
	}

};

void haha() {
	System::out << "BASECLIENT_DISABLE_STATSD";

	auto bla = 0;

	for (int i = 0; i < 100; i++) {
		bla *= "asd"_hashCode;
	}

	System::out << bla << endl;
}

int main(int argc, char* argv[]) {
	StackTrace::setBinaryName("testsuite3");

	int aaa = -1;
	uint32 bbb = aaa;
	int ccc = bbb;

	VectorMap<String, Function<void()>> testArrayF;
	testArrayF.put("asd", []() { } );

	Vector<int> firstV = {1, 2, 3, 4, 5, 6};

	Timer blaTimer;

	auto elapsedRes = blaTimer.run([](){ return 1; });

	Vector<int> secondV;

	secondV.addAll(std::move(firstV));

	secondV.forEach([](auto& val){val *= 11;});


	System::out << "testun lasted: " << Timer().run(haha) << endl;

	secondV.forEach([](auto& val){System::out << val << endl;});

	E3_ASSERT(true && "shit");

	VectorMap<int, String> testShitMap;
	testShitMap.put(1, "asd");
	testShitMap.put(2, "dsadas");

	ObjectOutputStream outputShit;
	testShitMap.toBinaryStream(&outputShit);

	//testArrayF.toBinaryStream(&outputShit);

	System::out << "outoutShit: " << outputShit.toStringData();

	//static_assert(sizeof(bool) == 1, "nda");

	Logger testLogger("TestHelper");
	testLogger.setLogLevel(Logger::LogLevel::INFO);

	auto testSharedShit = makeShared<Object>();
	auto testStream = makeUnique<Stream>();
	auto testManagedShared = makeManagedShared<ManagedObject>();
	//testStream->add(0);

	constexpr ConstString testStr = "mdamada";
	constexpr auto indexTest = testStr.indexOf('a');
	constexpr auto index2Test = testStr.indexOf("mada");
	constexpr auto ihash = testStr.hashCode();
	constexpr uint32 tthash = "asd"_hashCode;
	auto tthash2 = String("asd").hashCode();
	auto aas = String("asda");
	auto bbs = StringBuffer("asda");

	testLogger.fatal(bbs == aas, "string not equal to stringbuff");

	testLogger.info(true) << "first:" << tthash << " second:" << tthash2;

	testLogger.fatal(tthash2 == tthash, "hash not equal");

	auto testVal = StringHashCodeHelper<indexTest>().value;
	auto testHVal = StringHashCodeHelper<testStr.hashCode()>().value;
	auto testBVal = StringHashCodeHelper<index2Test>().value;

	UnicodeString bla = "aoisdhaoifhsaoisfh ";

	String lala = testStr.toString();
	int i = 32;

	i = i / 2;
	float a = 1.23f;
	float b = 1.24f;

	auto formatTest = String::format("format test %d %d ..", 1, 2);
	Time timestShit;

	TestCShit::mda("asdadasdasd");

	testLogger.info() << hex << bbb << dec << " " << ccc << " rand:" << System::random() << "elapsedRes: " << elapsedRes << " " << *testStream << " sizeofLogger:" << sizeof(Logger) << " "
		<< " callback size: " << sizeof(Logger::LoggerCallback) << "  "
		<< " level size: " << sizeof(Logger::LogLevel) << "  "
		<< " LoggerHelper size: " << sizeof(LoggerHelper) << "  "
		<< " StringBuffer size: " << sizeof(StringBuffer) << "  "
		<< bla.toString()
		<< uppercase << formatTest << lala
		<< "index is:" << indexTest << " found2:" << testBVal << " " << testStr.hashCode() << Float::areAlmostEqualRelative(a, b) << nouppercase << endl <<  " testing shit" << "mda" << 1 << 2 << 3 << lala << Time().getFormattedTimeFull() << "mda "
		<< Time().getFormattedTime() << " mda2" << flush;

	System::out << flush;

	auto testMoveTo = testSharedShit.castMoveTo<ManagedObject*>();
	auto testMoveToManaged = testManagedShared.castMoveTo<Observable*>();

	System::err << uppercase << "got past info call" << nouppercase << " test" << endl;

	try {
		SortedVector<String> arguments;

		for (int i = 1; i < argc; ++i) {
			arguments.put(argv[i]);
		}

		UniqueReference<TestCore*> core(new TestCore(Logger::INFO));

		core->run();
		Thread::sleep(1000);
		//return 0;

		//StackTrace::setBinaryName("testsuite3");

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
				//testQTBounding();
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
