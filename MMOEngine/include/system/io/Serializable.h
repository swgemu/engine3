/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef SERIALIZABLE_H_
#define SERIALIZABLE_H_

#include "../platform.h"

#include "../lang/String.h"
#include "../lang/Object.h"

#include "../util/VectorMap.h"

#include "ObjectInputStream.h"

namespace sys {
	namespace io {

		class ObjectOutputStream;
		class ObjectInputStream;

		class VariableName;

		class Serializable : public Object {
			String _className;
			VectorMap<VariableName, void*> variables;

		public:
			Serializable();

			virtual ~Serializable() {

			}

			virtual void writeObject(String& str);
			virtual void writeObject(ObjectOutputStream* stream);

			virtual void readObject(const String& str);
			virtual void readObject(ObjectInputStream* stream);

			void addSerializableVariable(const char* name, uint8* variable, int version = 0);
			void addSerializableVariable(const char* name, int8* variable, int version = 0);

			void addSerializableVariable(const char* name, uint16* variable, int version = 0);
			void addSerializableVariable(const char* name, int16* variable, int version = 0);

			void addSerializableVariable(const char* name, uint32* variable, int version = 0);
			void addSerializableVariable(const char* name, int32* variable, int version = 0);

			void addSerializableVariable(const char* name, uint64* variable, int version = 0);
			void addSerializableVariable(const char* name, int64* variable, int version = 0);

			void addSerializableVariable(const char* name, float* variable, int version = 0);
			void addSerializableVariable(const char* name, double* variable, int version = 0);

			void addSerializableVariable(const char* name, bool* variable, int version = 0);

			void addSerializableVariable(const char* name, Variable* variable, int version = 0);
			Variable* getSerializableVariable(const char* name);

			bool toString(String& str) {
				writeObject(str);

				return true;
			}

			bool parseFromString(const String& str, int version = 0) {
				readObject(str);

				return true;
			}

			bool toBinaryStream(ObjectOutputStream* stream) {
				writeObject(stream);

				return true;
			}

			bool parseFromBinaryStream(ObjectInputStream* stream) {
				readObject(stream);

				return true;
			}

			inline void _setClassName(const String& name) {
				_className = name;
			}

			static int getObjectData(const String& str, String& obj);

			static int serializeAtomicType(void* address, int type, String& value);
			static int serializeAtomicType(void* address, int type, ObjectOutputStream* stream);

			static int deSerializeAtomicType(void* address, int type, const String& value);
			static int deSerializeAtomicType(void* address, int type, ObjectInputStream* stream);

			static int getVariableDataMap(const String& serializedData, VectorMap<String, String>& map);
			static int getVariableDataOffset(const String& variableName, ObjectInputStream* stream);

			template<typename ClassType> static bool getVariable(const String& variableName, ClassType* address, ObjectInputStream* serializedObject) {
				int offset = getVariableDataOffset(variableName, serializedObject);

				if (offset == -1)
					return false;

				serializedObject->setOffset(offset);

				TypeInfo<ClassType>::parseFromBinaryStream(address, serializedObject);

				serializedObject->reset();

				return true;
			}

		private:
			void deSerializeVariable(const String& nameAndVersion, const String& varData);

		};

		class VariableName : public Variable {
			const char* name;
			uint8 version;

			uint8 type;

		public:
			VariableName() : Variable() {
				version = 0;
				type = 0;
			}

			VariableName(const char* name, int version) : Variable() {
				VariableName::name = name;
				VariableName::version = version;

				type = 0;
			}

			~VariableName() {

			}

			int compareTo(const VariableName& str) const {
				return strcmp(name, str.name);
			}

			inline const char* getName() {
				return name;
			}

			inline void setName(const char* str) {
				name = str;
			}

			inline void setVersion(int ver) {
				version = (uint8) ver;
			}

			inline void setType(int typ) {
				type = (uint8) typ;
			}

			inline int getVersion() {
				return (int) version;
			}

			inline int getType() {
				return (int) type;
			}

			bool toString(String& str) {
				return false;
			}

			bool parseFromString(const String& str, int version = 0) {
				return false;
			}

			bool toBinaryStream(ObjectOutputStream* stream) {
				return false;
			}

			bool parseFromBinaryStream(ObjectInputStream* stream) {
				return false;
			}

		};
	}
}

using namespace sys::io;

#endif /*SERIALIZABLE_H_*/
