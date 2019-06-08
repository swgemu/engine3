/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * referencetest.h
 *
 *  Created on: 14/12/2011
 *      Author: victor
 */

#ifndef ORBTEST_H_
#define ORBTEST_H_

#include "engine/engine.h"

#include "TestIDLClass.h"

void orbTestDeploy() {
	try {
		DistributedObjectBroker::initialize("", 44433);

		Reference<TestIDLClass*> object = new TestIDLClass(3);
		object->deploy("testCLass1");

		Thread::sleep(10000);

		object->undeploy();
	} catch (const Exception& e) {
		e.printStackTrace();
	}
}

void orbTestLookup() {
	try {
		DistributedObjectBroker::initialize("", 44433);

		Thread::sleep(5000);

		Reference<TestIDLClass*> object = Core::lookupObject<TestIDLClass>("testCLass1");

		if (object == nullptr) {
			System::out << "object \'testClass1\' not deployed" << PrintStream::endl;

			return;
		}

		Thread::sleep(15000);

		System::out << "value = " << object->getValue() << PrintStream::endl;

		object->setValue(4);

		System::out << "value = " << object->getValue() << PrintStream::endl;
	} catch (const StreamIndexOutOfBoundsException& e) {
		e.printStackTrace();

		System::out << e.getStream()->toStringData() << PrintStream::endl;
	} catch (const Exception& e) {
		e.printStackTrace();
	}
}


#endif /* ORBTEST_H_ */
