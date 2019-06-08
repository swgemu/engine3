/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include <limits>

#include "String.h"

#include "types.h"

#include "system/io/ObjectOutputStream.h"
#include "system/io/ObjectInputStream.h"

#include "Bool.h"
#include "Byte.h"
#include "Short.h"
#include "Long.h"
#include "Float.h"
#include "Double.h"

template<> unsigned int TypeInfoAtomicBase<const char*>::hashCode(const char* const& val) {
	return Long::hashCode(reinterpret_cast<uint64>(val));
}

template<> unsigned int TypeInfoAtomicBase<bool>::hashCode(const bool& val) {
	return Bool::hashCode(val);
}

template<> unsigned int TypeInfoAtomicBase<char>::hashCode(const char& val) {
	return Character::hashCode(val);
}

template<> unsigned int TypeInfoAtomicBase<uint8>::hashCode(const uint8& val) {
	return Byte::hashCode(val);
}

template<> unsigned int TypeInfoAtomicBase<uint16>::hashCode(const uint16& val) {
	return UnsignedShort::hashCode(val);
}

template<> unsigned int TypeInfoAtomicBase<int8>::hashCode(const int8& val) {
	return Character::hashCode(val);
}

template<> unsigned int TypeInfoAtomicBase<int16>::hashCode(const int16& val) {
	return Short::hashCode(val);
}

template<> unsigned int TypeInfoAtomicBase<uint32>::hashCode(const uint32& val) {
	return UnsignedInteger::hashCode(val);
}

template<> unsigned int TypeInfoAtomicBase<int32>::hashCode(const int32& val) {
	return Integer::hashCode(val);
}

template<> unsigned int TypeInfoAtomicBase<uint64>::hashCode(const uint64& val) {
	return UnsignedLong::hashCode(val);
}

template<> unsigned int TypeInfoAtomicBase<int64>::hashCode(const int64& val) {
	return Long::hashCode(val);
}

template<> unsigned int TypeInfoAtomicBase<float>::hashCode(const float& val) {
	return Float::hashCode(val);
}

template<> unsigned int TypeInfoAtomicBase<double>::hashCode(const double& val) {
	return Double::hashCode(val);
}

/*template<> unsigned int TypeInfoAtomicPointer<void*>::hashCode(const void*& val) {
#ifdef PLATFORM_64
	return UnsignedLong::hashCode((uint64) val);
#else
	return UnsignedInteger::hashCode((uint32) val);
#endif
}*/

template<> bool TypeInfoAtomicBase<const char*>::toString(void* address, String& value) {
	value = String((const char*)address);

	return true;
}

template<> bool TypeInfoAtomicBase<void*>::toString(void* address, String& value) {
	return true;
}

template<> bool TypeInfoAtomicBase<bool>::toString(void* address, String& value) {
	value = String::valueOf((bool)(*(bool*)address));

	return true;
}

template<> bool TypeInfoAtomicBase<char>::toString(void* address, String& value) {
	value = String::valueOf((char)(*(char*)address));

	return true;
}

template<> bool TypeInfoAtomicBase<uint8>::toString(void* address, String& value) {
	value = String::valueOf((uint32)(*(uint8*)address));

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

template<> bool TypeInfoAtomicBase<const char*>::toBinaryStream(const void* address, ObjectOutputStream* stream) {
	int count = strlen((const char*)address);

	stream->writeShort(count);
	stream->writeStream((const char*)address, count);

	return true;
}


template<> bool TypeInfoAtomicBase<void*>::toBinaryStream(const void* address, ObjectOutputStream* stream) {

	return true;
}

template<> bool TypeInfoAtomicBase<bool>::toBinaryStream(const void* address, ObjectOutputStream* stream) {
	stream->writeBoolean(*(bool*)address);

	return true;
}

template<> bool TypeInfoAtomicBase<char>::toBinaryStream(const void* address, ObjectOutputStream* stream) {
	stream->writeSignedByte(*(char*)address);

	return true;
}

template<> bool TypeInfoAtomicBase<uint8>::toBinaryStream(const void* address, ObjectOutputStream* stream) {
	stream->writeByte(*(uint8*)address);

	return true;
}

template<> bool TypeInfoAtomicBase<uint16>::toBinaryStream(const void* address, ObjectOutputStream* stream) {
	stream->writeShort(*(uint16*)address);

	return true;
}

template<> bool TypeInfoAtomicBase<uint32>::toBinaryStream(const void* address, ObjectOutputStream* stream) {
	stream->writeInt(*(uint32*)address);

	return true;
}

template<> bool TypeInfoAtomicBase<uint64>::toBinaryStream(const void* address, ObjectOutputStream* stream) {
	stream->writeLong(*(uint64*)address);

	return true;
}

template<> bool TypeInfoAtomicBase<int8>::toBinaryStream(const void* address, ObjectOutputStream* stream) {
	stream->writeSignedByte(*(int8*)address);

	return true;
}

template<> bool TypeInfoAtomicBase<int16>::toBinaryStream(const void* address, ObjectOutputStream* stream) {
	stream->writeSignedShort(*(int16*)address);

	return true;
}

template<> bool TypeInfoAtomicBase<int32>::toBinaryStream(const void* address, ObjectOutputStream* stream) {
	stream->writeSignedInt(*(int32*)address);

	return true;
}

template<> bool TypeInfoAtomicBase<int64>::toBinaryStream(const void* address, ObjectOutputStream* stream) {
	stream->writeSignedLong(*(int64*)address);

	return true;
}

template<> bool TypeInfoAtomicBase<float>::toBinaryStream(const void* address, ObjectOutputStream* stream) {
	stream->writeFloat(*(float*)address);

	return true;
}

template<> bool TypeInfoAtomicBase<double>::toBinaryStream(const void* address, ObjectOutputStream* stream) {
	stream->writeDouble(*(double*)address);

	return true;
}

template<> bool TypeInfoAtomicBase<const char*>::parseFromString(void* address, const String& value, int version) {
	return false;
}


template<> bool TypeInfoAtomicBase<void*>::parseFromString(void* address, const String& value, int version) {
	return false;
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
	*(uint8*)address = UnsignedInteger::valueOf(value);

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

template<> bool TypeInfoAtomicBase<const char*>::parseFromBinaryStream(void* address, ObjectInputStream* stream) {
	return false;
}

template<> bool TypeInfoAtomicBase<void*>::parseFromBinaryStream(void* address, ObjectInputStream* stream) {
	return false;
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
	*(double*)address = stream->readDouble();

	return true;
}

template<> uint8 TypeInfoAtomicBase<uint8>::nullValue() {
	return 0;
}

template<> int8 TypeInfoAtomicBase<int8>::nullValue() {
	return -1;
}

template<> uint16 TypeInfoAtomicBase<uint16>::nullValue() {
	return 0;
}

template<> int16 TypeInfoAtomicBase<int16>::nullValue() {
	return -1;
}

template<> uint32 TypeInfoAtomicBase<uint32>::nullValue() {
	return 0;
}

template<> int32 TypeInfoAtomicBase<int32>::nullValue() {
	return -1;
}

template<> uint64 TypeInfoAtomicBase<uint64>::nullValue() {
	return 0;
}

template<> int64 TypeInfoAtomicBase<int64>::nullValue() {
	return -1;
}

template<> void* TypeInfoAtomicBase<void*>::nullValue() {
	return nullptr;
}

template<> float TypeInfoAtomicBase<float>::nullValue() {
	return std::numeric_limits<float>::quiet_NaN();
}

template<> double TypeInfoAtomicBase<double>::nullValue() {
	return std::numeric_limits<double>::quiet_NaN();
}

template<> bool TypeInfoAtomicBase<bool>::nullValue() {
	return false;
}

template<> const char* TypeInfoAtomicBase<const char*>::nullValue() {
	return nullptr;
}
