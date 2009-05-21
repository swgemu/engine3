/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "String.h"

#include "types.h"

#include "../io/ObjectOutputStream.h"
#include "../io/ObjectInputStream.h"

#include "Character.h"
#include "Long.h"
#include "Integer.h"
#include "Float.h"
#include "Bool.h"

template<> bool TypeInfoAtomicBase<bool>::toString(void* address, String& value) {
	value = String::valueOf((bool)(*(bool*)address));

	return true;
}

template<> bool TypeInfoAtomicBase<char>::toString(void* address, String& value) {
	value = String::valueOf((char)(*(char*)address));

	return true;
}

template<> bool TypeInfoAtomicBase<uint8>::toString(void* address, String& value) {
	value = String::valueOf((uint8)(*(uint8*)address));

	return true;
}

template<> bool TypeInfoAtomicBase<uint16>::toString(void* address, String& value) {
	value = String::valueOf((uint16)(*(uint16*)address));

	return true;
}

template<> bool TypeInfoAtomicBase<uint32>::toString(void* address, String& value) {
	value = String::valueOf((uint32)(*(uint32*)address));

	return true;
}

template<> bool TypeInfoAtomicBase<int8>::toString(void* address, String& value) {
	value = String::valueOf((int8)(*(int8*)address));

	return true;
}

template<> bool TypeInfoAtomicBase<int16>::toString(void* address, String& value) {
	value = String::valueOf((int16)(*(int16*)address));

	return true;
}

template<> bool TypeInfoAtomicBase<int32>::toString(void* address, String& value) {
	value = String::valueOf((int32)(*(int32*)address));

	return true;
}

template<> bool TypeInfoAtomicBase<uint64>::toString(void* address, String& value) {
	value = String::valueOf((uint64)(*(uint64*)address));

	return true;
}

template<> bool TypeInfoAtomicBase<int64>::toString(void* address, String& value) {
	value = String::valueOf((int64)(*(int64*)address));

	return true;
}

template<> bool TypeInfoAtomicBase<float>::toString(void* address, String& value) {
	value = String::valueOf((float)(*(float*)address));

	return true;
}

template<> bool TypeInfoAtomicBase<double>::toString(void* address, String& value) {
	value = String::valueOf((double)(*(double*)address));

	return true;
}

template<> bool TypeInfoAtomicBase<bool>::toBinaryStream(void* address, ObjectOutputStream* stream) {
	stream->writeBoolean(*(bool*)address);

	return true;
}

template<> bool TypeInfoAtomicBase<char>::toBinaryStream(void* address, ObjectOutputStream* stream) {
	stream->writeSignedByte(*(char*)address);

	return true;
}

template<> bool TypeInfoAtomicBase<uint8>::toBinaryStream(void* address, ObjectOutputStream* stream) {
	stream->writeByte(*(uint8*)address);

	return true;
}

template<> bool TypeInfoAtomicBase<uint16>::toBinaryStream(void* address, ObjectOutputStream* stream) {
	stream->writeShort(*(uint16*)address);

	return true;
}

template<> bool TypeInfoAtomicBase<uint32>::toBinaryStream(void* address, ObjectOutputStream* stream) {
	stream->writeInt(*(uint32*)address);

	return true;
}

template<> bool TypeInfoAtomicBase<uint64>::toBinaryStream(void* address, ObjectOutputStream* stream) {
	stream->writeLong(*(uint64*)address);

	return true;
}

template<> bool TypeInfoAtomicBase<int8>::toBinaryStream(void* address, ObjectOutputStream* stream) {
	stream->writeSignedByte(*(int8*)address);

	return true;
}

template<> bool TypeInfoAtomicBase<int16>::toBinaryStream(void* address, ObjectOutputStream* stream) {
	stream->writeSignedShort(*(int16*)address);

	return true;
}

template<> bool TypeInfoAtomicBase<int32>::toBinaryStream(void* address, ObjectOutputStream* stream) {
	stream->writeSignedInt(*(int32*)address);

	return true;
}

template<> bool TypeInfoAtomicBase<int64>::toBinaryStream(void* address, ObjectOutputStream* stream) {
	stream->writeSignedLong(*(int64*)address);

	return true;
}

template<> bool TypeInfoAtomicBase<float>::toBinaryStream(void* address, ObjectOutputStream* stream) {
	stream->writeFloat(*(float*)address);

	return true;
}

template<> bool TypeInfoAtomicBase<double>::toBinaryStream(void* address, ObjectOutputStream* stream) {
	stream->writeFloat(*(float*)address);

	return true;
}

template<> bool TypeInfoAtomicBase<bool>::parseFromString(void* address, const String& value, int version) {
	*(bool*)address = Bool::valueOf(value);

	return true;
}

template<> bool TypeInfoAtomicBase<char>::parseFromString(void* address, const String& value, int version) {
	*(char*)address = Character::valueOf(value);

	return true;
}

template<> bool TypeInfoAtomicBase<uint8>::parseFromString(void* address, const String& value, int version) {
	*(uint8*)address = Character::valueOf(value);

	return true;
}

template<> bool TypeInfoAtomicBase<uint16>::parseFromString(void* address, const String& value, int version) {
	*(uint16*)address = (uint16) UnsignedInteger::valueOf(value);

	return true;
}

template<> bool TypeInfoAtomicBase<uint32>::parseFromString(void* address, const String& value, int version) {
	*(uint32*)address = UnsignedInteger::valueOf(value);

	return true;
}

template<> bool TypeInfoAtomicBase<uint64>::parseFromString(void* address, const String& value, int version) {
	*(uint64*)address = UnsignedLong::valueOf(value);

	return true;
}

template<> bool TypeInfoAtomicBase<int8>::parseFromString(void* address, const String& value, int version) {
	*(int8*)address = Character::valueOf(value);

	return true;
}

template<> bool TypeInfoAtomicBase<int16>::parseFromString(void* address, const String& value, int version) {
	*(int16*)address = (int16) Integer::valueOf(value);

	return true;
}

template<> bool TypeInfoAtomicBase<int32>::parseFromString(void* address, const String& value, int version) {
	*(int32*)address = Integer::valueOf(value);

	return true;
}

template<> bool TypeInfoAtomicBase<int64>::parseFromString(void* address, const String& value, int version) {
	*(int64*)address = Long::valueOf(value);

	return true;
}

template<> bool TypeInfoAtomicBase<float>::parseFromString(void* address, const String& value, int version) {
	*(float*)address = Float::valueOf(value);

	return true;
}

template<> bool TypeInfoAtomicBase<double>::parseFromString(void* address, const String& value, int version) {
	*(double*)address = Float::valueOf(value);

	return true;
}

template<> bool TypeInfoAtomicBase<bool>::parseFromBinaryStream(void* address, ObjectInputStream* stream) {
	*(bool*)address = stream->readBoolean();

	return true;
}

template<> bool TypeInfoAtomicBase<char>::parseFromBinaryStream(void* address, ObjectInputStream* stream) {
	*(char*)address = stream->readSignedByte();

	return true;
}

template<> bool TypeInfoAtomicBase<uint8>::parseFromBinaryStream(void* address, ObjectInputStream* stream) {
	*(uint8*)address = stream->readByte();

	return true;
}

template<> bool TypeInfoAtomicBase<uint16>::parseFromBinaryStream(void* address, ObjectInputStream* stream) {
	*(uint16*)address = stream->readShort();

	return true;
}

template<> bool TypeInfoAtomicBase<uint32>::parseFromBinaryStream(void* address, ObjectInputStream* stream) {
	*(uint32*)address = stream->readInt();

	return true;
}

template<> bool TypeInfoAtomicBase<uint64>::parseFromBinaryStream(void* address, ObjectInputStream* stream) {
	*(uint64*)address = stream->readLong();

	return true;
}

template<> bool TypeInfoAtomicBase<int8>::parseFromBinaryStream(void* address, ObjectInputStream* stream) {
	*(int8*)address = stream->readSignedByte();

	return true;
}

template<> bool TypeInfoAtomicBase<int16>::parseFromBinaryStream(void* address, ObjectInputStream* stream) {
	*(int16*)address = stream->readSignedShort();

	return true;
}

template<> bool TypeInfoAtomicBase<int32>::parseFromBinaryStream(void* address, ObjectInputStream* stream) {
	*(int32*)address = stream->readSignedInt();

	return true;
}

template<> bool TypeInfoAtomicBase<int64>::parseFromBinaryStream(void* address, ObjectInputStream* stream) {
	*(int64*)address = stream->readSignedLong();

	return true;
}

template<> bool TypeInfoAtomicBase<float>::parseFromBinaryStream(void* address, ObjectInputStream* stream) {
	*(float*)address = stream->readFloat();

	return true;
}

template<> bool TypeInfoAtomicBase<double>::parseFromBinaryStream(void* address, ObjectInputStream* stream) {
	*(double*)address = stream->readFloat();

	return true;
}

