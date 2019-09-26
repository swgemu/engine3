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
