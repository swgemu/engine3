/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * timeserialize.h
 *
 *  Created on: 31/01/2012
 *      Author: victor
 */

#ifndef TIMESERIALIZE_H_
#define TIMESERIALIZE_H_

#include "engine/engine.h"

void testTimeSerialization() {
	Time test;

	test.addMiliTime(9999999999);

	System::out << "test mili difference:" << test.miliDifference() << endl;

	ObjectOutputStream stream;

	TypeInfo<Time >::toBinaryStream(&test, &stream);

	String data = stream.toStringData();

	System::out << data << endl;

	test.updateToCurrentTime();

	stream.reset();

	ObjectInputStream* input = new ObjectInputStream(stream.getBuffer(), stream.size());

	TypeInfo<Time >::parseFromBinaryStream(&test, input);

	System::out << "test mili difference after parsing:" << test.miliDifference() << endl;

	delete input;
}

#endif /* TIMESERIALIZE_H_ */

