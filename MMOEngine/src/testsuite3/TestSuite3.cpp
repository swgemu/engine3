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
#include "tests/qtstmtest.h"
#include "tests/referencetest.h"
#include "tests/allocatorhook.h"
#include "tests/qtboundingtest.h"
#include "tests/timeserialize.h"
#include "tests/loggertest.h"
#include "tests/orbtest.h"
#include "tests/lowerbound.h"
#include "tests/udptest.h"
#include "tests/sizeoftest.h"

class TestCore : public Core {
public:
	void initialize() {
	}

	void run() {

	}
};

int main(int argc, char* argv[]) {
	try {
		SortedVector<String> arguments;

		for (int i = 1; i < argc; ++i) {
			arguments.put(argv[i]);
		}

		TestCore core;

		StackTrace::setBinaryName("testsuite3");

		if (!arguments.isEmpty()) {
			if (arguments.contains("stmtest"))
				testTransactions();
			else if (arguments.contains("memtest"))
				testMemoryProtection();
			else if (arguments.contains("stmmemtest"))
				testSTMMemory();
			else if (arguments.contains("qtstmtest"))
				testQTSTM();
			else if (arguments.contains("referencetest")) {
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
			}
		}

	} catch (Exception& e) {
		System::out << e.getMessage() << "\n";
		e.printStackTrace();
	}

	return 0;
}
