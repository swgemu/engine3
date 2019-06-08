/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * serializableconvert.h
 *
 *  Created on: 26/02/2013
 *      Author: victor
 */

#ifndef SERIALIZABLECONVERT_H_
#define SERIALIZABLECONVERT_H_

#include "engine/engine.h"

void serializableConverTest() {
	ObjectOutputStream* stream = new ObjectOutputStream();

	stream->writeShort(2); // 2 vars

	int persistenceLevel = 2;

	String name;
	int _offset;
	uint32 _totalSize;
	name = "ManagedObject.persistenceLevel";
	TypeInfo<String>::toBinaryStream(&name, stream);
	_offset = stream->getOffset();
	stream->writeInt(0);
	TypeInfo<int >::toBinaryStream(&persistenceLevel, stream);
	_totalSize = (uint32) (stream->getOffset() - (_offset + 4));
	stream->writeInt(_offset, _totalSize);

	name = "ManagedObject.huiLevel";
	TypeInfo<String>::toBinaryStream(&name, stream);
	_offset = stream->getOffset();
	stream->writeInt(0);
	TypeInfo<int >::toBinaryStream(&persistenceLevel, stream);
	_totalSize = (uint32) (stream->getOffset() - (_offset + 4));
	stream->writeInt(_offset, _totalSize);

	printf("pre convert:%s\n", stream->toStringData().toCharArray());

	ObjectInputStream* inputStream = new ObjectInputStream();
	stream->copy(inputStream);
	inputStream->reset();

	ObjectOutputStream* newOutput = Serializable::convertToHashCodeNameMembers(inputStream);

	printf("post convert:%s\n", newOutput->toStringData().toCharArray());

	delete newOutput;
	delete inputStream;
	delete stream;
}


#endif /* SERIALIZABLECONVERT_H_ */
