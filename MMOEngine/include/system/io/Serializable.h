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

namespace sys {
	namespace io {

		class ObjectOutputStream;
		class ObjectInputStream;

		class VariableName : public Variable {
			String name;
			uint8 version;

			uint8 type;

		public:
			VariableName() : Variable() {
				version = 0;
				type = 0;
			}

			VariableName(const String& name, int version) : Variable() {
				VariableName::name = name;
				VariableName::version = version;

				type = 0;
			}

			~VariableName() {

			}

			int compareTo(const VariableName& str) const {
				return name.compareTo(str.name);
			}

			inline String getName() {
				return name;
			}

			inline void setName(const String& str) {
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

		class Serializable : public virtual Object {
			VectorMap<VariableName, void*> variables;

		public:
			Serializable();

			virtual ~Serializable() {

			}

			virtual void serialize(String& str);
			virtual void serialize(ObjectOutputStream* stream);

			virtual void deSerialize(const String& str);
			virtual void deSerialize(ObjectInputStream* stream);

			void addSerializableVariable(const String& name, uint8* variable, int version = 0);
			void addSerializableVariable(const String& name, int8* variable, int version = 0);

			void addSerializableVariable(const String& name, uint16* variable, int version = 0);
			void addSerializableVariable(const String& name, int16* variable, int version = 0);

			void addSerializableVariable(const String& name, uint32* variable, int version = 0);
			void addSerializableVariable(const String& name, int32* variable, int version = 0);

			void addSerializableVariable(const String& name, uint64* variable, int version = 0);
			void addSerializableVariable(const String& name, int64* variable, int version = 0);

			void addSerializableVariable(const String& name, float* variable, int version = 0);
			void addSerializableVariable(const String& name, double* variable, int version = 0);

			void addSerializableVariable(const String& name, bool* variable, int version = 0);

			void addSerializableVariable(const String& name, Variable* variable, int version = 0);
			Variable* getSerializableVariable(const String& name);

			bool toString(String& str) {
				serialize(str);

				return true;
			}

			bool parseFromString(const String& str, int version = 0) {
				deSerialize(str);

				return true;
			}

			bool toBinaryStream(ObjectOutputStream* stream) {
				serialize(stream);

				return true;
			}

			bool parseFromBinaryStream(ObjectInputStream* stream) {
				deSerialize(stream);

				return true;
			}

			static int getObjectData(const String& str, String& obj);

			static int serializeAtomicType(void* address, int type, String& value);
			static int serializeAtomicType(void* address, int type, ObjectOutputStream* stream);

			static int deSerializeAtomicType(void* address, int type, const String& value);
			static int deSerializeAtomicType(void* address, int type, ObjectInputStream* stream);

		private:
			void deSerializeVariable(const String& nameAndVersion, const String& varData);

		};
	}
}

using namespace sys::io;

#endif /*SERIALIZABLE_H_*/
