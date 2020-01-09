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
#ifndef PLATFORM_WIN
#include "tests/memprottest.h"
#include "tests/stmtest.h"
#include "tests/stmmemtest.h"
#endif
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
		StackTrace::setBinaryName("testsuite3");

		using FunctorType = Function<int(const String&)>;

		VectorMap<String, FunctorType> test;

		test.put("b", [this](const String& a) -> int {
			printf("b\n");
			return 0;
			});

		test.put("c", [this](const String& a) -> int {
			printf("c\n");
			return 0;
			});

		test.put("d", [this](const String& a) -> int {
			printf("d\n");
			return 0;
			});

		test.put("e", [this](const String& a) -> int {
			printf("e\n");
			return 0;
			});


		test.put("f", [this](const String& a) -> int {
			printf("f\n");
			return 0;
			});

		test.put("g", [this](const String& a) -> int {
			printf("g\n");
			return 0;
			});

		test.put("asdf", [this](const String& a) -> int {
			printf("asdf\n");
			return 0;
			});


		test.get("asdf")("a");


		test.removeRange(1, 4);

		test.removeElementAt(0);


		for (auto i = 0; i < 5; ++i) {
			System::out << "ok then" << ha << endl;

			Core::getTaskManager()->scheduleTask([this] () { auto incr = ha.increment(); System::out << "test" << incr << endl; }, "TEstTask", 800);
		}
	}

};

int main(int argc, char* argv[]) {
	StackTrace::setBinaryName("testsuite3");

	try {
		SortedVector<String> arguments;

		for (int i = 1; i < argc; ++i) {
			arguments.put(argv[i]);
		}

		UniqueReference<TestCore*> core(new TestCore(Logger::INFO));

		core->run();
		Thread::sleep(1000);

		if (!arguments.isEmpty()) {
#ifndef PLATFORM_WIN
			if (arguments.contains("stmtest"))
				testTransactions();
			else if (arguments.contains("memtest"))
				testMemoryProtection();
			else if (arguments.contains("stmmemtest"))
				testSTMMemory();
			else if (arguments.contains("memfiletest"))
				testMemoryMappedFile();
#endif
			if (arguments.contains("qtstmtest")) {
				//testQTSTM();
			} else if (arguments.contains("referencetest")) {
				referenceTest();
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
