/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef SERIALIZABLE_H_
#define SERIALIZABLE_H_

#include "system/platform.h"

#include "system/lang/String.h"
#include "system/lang/Object.h"

#include "system/util/VectorMap.h"

#include "ObjectInputStream.h"

namespace sys {
	namespace io {

		class ObjectOutputStream;
		class ObjectInputStream;

		class VariableName;

		class Serializable : public virtual Object {
		protected:
			SerializableString _className;
			VectorMap<VariableName, uint64> _variables;

		public:
			const static bool is_virtual_object = true;

			Serializable();

			virtual ~Serializable() {

			}

			//virtual void writeObject(String& str);
			virtual void writeObject(ObjectOutputStream* stream);

			//virtual void readObject(const String& str);
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

			/*bool toString(String& str) {
				writeObject(str);

				return true;
			}

			bool parseFromString(const String& str, int version = 0) {
				readObject(str);

				return true;
			}*/

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

			inline String& _getClassName() {
				return _className;
			}

			inline const String& _getClassName() const {
				return _className;
			}

			static int getObjectData(const String& str, String& obj);

			static int serializeAtomicType(void* address, int type, String& value);
			static int serializeAtomicType(void* address, int type, ObjectOutputStream* stream);

			static int deSerializeAtomicType(void* address, int type, const String& value);
			static int deSerializeAtomicType(void* address, int type, ObjectInputStream* stream);

			static int getVariableDataMap(const String& serializedData, VectorMap<String, String>& map);
			static int getVariableDataOffset(const String& variableName, ObjectInputStream* stream);
			static int getVariableDataOffset(const uint32& variableHashCode, ObjectInputStream* stream);

			static int getVariableNames(Vector<String>& variableNames, ObjectInputStream* stream);

			/**
			 * Returns a new ObjectOutputStream with the modified variable data
			 * Returns nullptr on error
			 */
			static ObjectOutputStream* convertToHashCodeNameMembers(ObjectInputStream* object);
			static ObjectOutputStream* changeVariableData(const String& variableName, ObjectInputStream* object, Stream* newVariableData);
			static ObjectOutputStream* changeVariableData(const uint32& variableHashCode, ObjectInputStream* object, Stream* newVariableData);
			static ObjectOutputStream* deleteVariable(const String& variableName, ObjectInputStream* object);
			static ObjectOutputStream* changeVariableName(const String& variableName, const String& newVariableName, ObjectInputStream* object);
			static ObjectOutputStream* addVariable(const String& variableName, ObjectInputStream* object, Stream* newVariableData);

			template<typename ClassType> static bool getVariable(const String& variableName, ClassType* address, ObjectInputStream* serializedObject) {
				int offset = getVariableDataOffset(variableName, serializedObject);

				if (offset == -1)
					return false;

				serializedObject->setOffset(offset);

				TypeInfo<ClassType>::parseFromBinaryStream(address, serializedObject);

				serializedObject->reset();

				return true;
			}

			template<typename ClassType> static bool getVariable(const uint32& variableHashCode, ClassType* address, ObjectInputStream* serializedObject) {
				int offset = getVariableDataOffset(variableHashCode, serializedObject);

				if (offset == -1)
					return false;

				serializedObject->setOffset(offset);

				TypeInfo<ClassType>::parseFromBinaryStream(address, serializedObject);

				serializedObject->reset();

				return true;
			}

			template<typename ClassType> static bool getVariable(const String& variableName, ClassType* address, const String& serializedObject) {
				VectorMap<String, String> variableDataMap;

				int variableSize = getVariableDataMap(serializedObject, variableDataMap);

				if (!variableDataMap.contains(variableName))
					return false;

				String variableData = variableDataMap.get(variableName);

				TypeInfo<ClassType>::parseFromString(address, variableData);

				return true;
			}

		private:
			void deSerializeVariable(const String& nameAndVersion, const String& varData);

		};

		class VariableName {
			const char* name;
			uint8 version;

			uint8 type;

		public:
			VariableName() {
				version = 0;
				type = 0;
				name = nullptr;
			}

			VariableName(const char* name, int version) {
				VariableName::name = name;
				VariableName::version = version;

				type = 0;
			}

			VariableName(const VariableName& v) {
				name = v.name;
				version = v.version;
				type = v.type;
			}

			VariableName& operator=(const VariableName& v) {
				if (this == &v)
					return *this;

				name = v.name;
				version = v.version;
				type = v.type;

				return *this;
			}

			~VariableName() {

			}

			int compareTo(const VariableName& str) const {
				return strcmp(name, str.name);
			}

			inline const char* getName() const {
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

			inline int getVersion() const {
				return (int) version;
			}

			inline int getType() const {
				return (int) type;
			}

			/*bool toString(String& str) {
				return false;
			}

			bool parseFromString(const String& str, int version = 0) {
				return false;
			}*/

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
