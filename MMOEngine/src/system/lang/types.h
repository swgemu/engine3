/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#pragma once

#include "system/platform.h"

#if GCC_VERSION >= 40100
#include <typeinfo>
#include <cxxabi.h>
#endif

#include <type_traits>

#include "String.h"

namespace sys {
	namespace io {
		class ObjectOutputStream;
		class ObjectInputStream;
	}

	namespace lang {
		class Integer;
		class UnsignedInteger;
		class Long;
		class UnsignedLong;
		class String;
	}
}

using namespace sys::io;

template<typename T> class TypeInfo;

template<typename T> class TypeInfoAtomicBase {
public:
	static const bool needConstructor = false;

	static const int UINT8 = 0x02;
	static const int INT8 = 0x03;
	static const int CHAR = 0x03;

	static const int UINT16 = 0x04;
	static const int INT16 = 0x05;

	static const int UINT32 = 0x08;
	static const int INT32 = 0x09;

	static const int UINT64 = 0x10;
	static const int INT64 = 0x11;

	static const int DOUBLE = 0x20;
	static const int FLOAT = 0x21;

	static const int POINTER = 0x40;

	static const int BOOL = 0x80;

	static unsigned int hashCode(const T& val);

	static bool toString(void* address, sys::lang::String& value);
	static bool toBinaryStream(const void* address, ObjectOutputStream* stream);

	static bool parseFromString(void* address, const sys::lang::String& value, int version = 0);
	static bool parseFromBinaryStream(void* address, ObjectInputStream* value);

	static int compare(const T& val1, const T& val2) {
		if (val1 < val2)
			return 1;
		else if (val1 > val2)
			return -1;
		else
			return 0;
	}

	static T nullValue();
};

template<typename T> class TypeInfoConstructedBase {
public:
	static const bool needConstructor = true;

	static const int type = -1;

	static unsigned int hashCode(const T& val) {
		return val.hashCode();
	}

	static String getClassName(const T* val, bool withNamespace = false) {
		const char* name = typeid(*val).name();

#if GCC_VERSION >= 40100
		int stat;
		char* demangled = abi::__cxa_demangle(name, 0, 0, &stat);

		if (stat == 0) {
			String ret(demangled);

			free(demangled);

			return ret;
		}
#endif
		return name;
	}

	/*static bool toString(T* address, sys::lang::String& value) {
		return address->toString(value);
	}*/

	static bool toBinaryStream(T* address, ObjectOutputStream* stream) {
		return address->toBinaryStream(stream);
	}

	/*static bool parseFromString(T* address, const sys::lang::String& value, int version = 0) {
		return address->parseFromString(value, version);
	}*/

	static bool parseFromBinaryStream(T* address, ObjectInputStream* stream) {
		return address->parseFromBinaryStream(stream);
	}

	static int compare(const T& val1, const T& val2) {
		return val1.compareTo(val2);
	}

	static T nullValue() {
		return T();
	}
};

template<typename T> class TypeInfoAtomicUnsignedChar : public TypeInfoAtomicBase<T> {
public:
	static const int type = TypeInfoAtomicBase<T>::UINT8;
};

template<typename T> class TypeInfoAtomicSignedChar : public TypeInfoAtomicBase<T> {
public:
	static const int type = TypeInfoAtomicBase<T>::INT8;
};

template<typename T> class TypeInfoAtomicUnsignedShort : public TypeInfoAtomicBase<T> {
public:
	static const int type = TypeInfoAtomicBase<T>::UINT16;
};

template<typename T> class TypeInfoAtomicSignedShort : public TypeInfoAtomicBase<T> {
public:
	static const int type = TypeInfoAtomicBase<T>::INT16;
};

template<typename T> class TypeInfoAtomicUnsignedInteger : public TypeInfoAtomicBase<T> {
public:
	static const int type = TypeInfoAtomicBase<T>::UINT32;
};

template<typename T> class TypeInfoAtomicSignedInteger : public TypeInfoAtomicBase<T> {
public:
	static const int type = TypeInfoAtomicBase<T>::INT32;
};

template<typename T> class TypeInfoAtomicUnsignedLong : public TypeInfoAtomicBase<T> {
public:
	static const int type = TypeInfoAtomicBase<T>::UINT64;
};

template<typename T> class TypeInfoAtomicSignedLong : public TypeInfoAtomicBase<T> {
public:
	static const int type = TypeInfoAtomicBase<T>::INT64;
};

template<typename T> class TypeInfoAtomicFloat : public TypeInfoAtomicBase<T> {
public:
	static const int type = TypeInfoAtomicBase<T>::FLOAT;
};

template<typename T> class TypeInfoAtomicDouble : public TypeInfoAtomicBase<T> {
public:
	static const int type = TypeInfoAtomicBase<T>::DOUBLE;
};

template<typename T> class TypeInfoAtomicChar : public TypeInfoAtomicBase<T> {
public:
	static const int type = TypeInfoAtomicBase<T>::CHAR;
};

template<typename T> class TypeInfoAtomicPointer : public TypeInfoAtomicBase<T> {
public:
	static const int type = TypeInfoAtomicBase<T>::POINTER;
};

template<typename T> class TypeInfoAtomicBool : public TypeInfoAtomicBase<T> {
public:
	static const int type = TypeInfoAtomicBase<T>::BOOL;
};

template<typename T> class TypeInfo : public TypeInfoConstructedBase<T> {};

template<typename T> class TypeInfo<T*> : public TypeInfoAtomicPointer<T*> {
public:
	static bool toString(void* address, sys::lang::String& value) {
		return false;
	}

	static bool toBinaryStream(const void* address, ObjectOutputStream* stream) {
		return false;
	}

	static bool parseFromString(void* address, const sys::lang::String& value, int version = 0) {
		return false;
	}

	static bool parseFromBinaryStream(void* address, ObjectInputStream* value) {
		return false;
	}

	static int compare(T* val1, T* val2) {
		return val1->compareTo(val2);
	}

	static unsigned int hashCode(const T* val) {
#ifdef PLATFORM_64
	return (unsigned int) ((uint64)val ^ ((uint64)val >> 32));
#else
	return (unsigned int) val;
#endif
	}

	static T* nullValue() {
		return nullptr;
	}
};

template<> class TypeInfo<bool> : public TypeInfoAtomicBool<bool> {};
template<> class TypeInfo<char> : public TypeInfoAtomicChar<char> {};
template<> class TypeInfo<uint8> : public TypeInfoAtomicUnsignedChar<uint8> {};
template<> class TypeInfo<uint16> : public TypeInfoAtomicUnsignedShort<uint16> {};
template<> class TypeInfo<uint32> : public TypeInfoAtomicUnsignedInteger<uint32> {};
template<> class TypeInfo<int8> : public TypeInfoAtomicSignedChar<int8> {};
template<> class TypeInfo<int16> : public TypeInfoAtomicSignedShort<int16> {};
template<> class TypeInfo<int32> : public TypeInfoAtomicSignedInteger<int32> {};
template<> class TypeInfo<uint64> : public TypeInfoAtomicUnsignedLong<uint64> {};
template<> class TypeInfo<int64> : public TypeInfoAtomicSignedLong<int64> {};
template<> class TypeInfo<float> : public TypeInfoAtomicFloat<float> {};
template<> class TypeInfo<double> : public TypeInfoAtomicDouble<double> {};
template<> class TypeInfo<const char*> : public TypeInfoAtomicPointer<const char*> {};
template<> class TypeInfo<void*> : public TypeInfoAtomicPointer<void*> {};

namespace SerializationHelpers {
	template <typename T>
	class HasSerializationMethodsSFINAE {
		typedef char success;
		struct failure { char x[2]; };

		template <class C> static success test(decltype(&C::toBinaryStream)) ;
		template <class C> static failure test(...);

		template <class C> static success test2(decltype(&C::parseFromBinaryStream)) ;
		template <class C> static failure test2(...);

	public:
		enum { all = (sizeof(test<T>(0)) == sizeof(char) && sizeof(test2<T>(0)) == sizeof(char))};
		enum { toBinary = sizeof(test<T>(0)) == sizeof(char) };
		enum { fromBinary = sizeof(test2<T>(0)) == sizeof(char) };
	};

	template<class O, std::enable_if_t<HasSerializationMethodsSFINAE<O>::toBinary
	   || std::is_fundamental<O>::value, int> = 0>
	bool toBinary(O* obj, ObjectOutputStream* stream) {
		return TypeInfo<O>::toBinaryStream(obj, stream);
	}

	template<class O, std::enable_if_t<!HasSerializationMethodsSFINAE<O>::toBinary
	   && !std::is_fundamental<O>::value, int> = 0>
	bool toBinary(O* obj, ObjectOutputStream* stream) {
		E3_ABORT("calling toBinaryStream on an element that doesnt have it");
	}

	template<class O, std::enable_if_t<HasSerializationMethodsSFINAE<O>::fromBinary
	   || std::is_fundamental<O>::value, int> = 0>
	bool fromBinary(O* obj, ObjectInputStream* stream) {
		return TypeInfo<O>::parseFromBinaryStream(obj, stream);
	}

	template<class O, std::enable_if_t<!HasSerializationMethodsSFINAE<O>::fromBinary
	   && !std::is_fundamental<O>::value, int> = 0>
	bool fromBinary(O* obj, ObjectInputStream* stream) {
		E3_ABORT("calling parseFromBinaryStream on an element that doesnt have it");
	}
}

