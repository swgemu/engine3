/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef SERIALIZABLE_H_
#define SERIALIZABLE_H_

#include "../platform.h"

#include "../lang/String.h"
#include "../lang/System.h"
#include "../lang/Time.h"

#include "../lang/Object.h"
#include "../util/VectorMap.h"
#include "../thread/ReadWriteLock.h"

namespace sys {
	namespace io {

		class ObjectOutputStream;
		class ObjectInputStream;

		class Serializable : public virtual Object {
			VectorMap<String, Variable*> variables;

			//static VectorMap<uint32, String> variableNames;
			//static ReadWriteLock variableNameMutex;

		public:
			Serializable();

			virtual ~Serializable() {

			}

			virtual void serialize(String& str);
			virtual void serialize(ObjectOutputStream* stream);

			virtual void deSerialize(const String& str);
			virtual void deSerialize(ObjectInputStream* stream);

			void addSerializableVariable(const String& nameAndVersion, Variable* variable);
			Variable* getSerializableVariable(const String& nameAndVersion);

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

		private:
			void deSerializeVariable(const String& nameAndVersion, const String& varData);

		};
	}
}

using namespace sys::io;

#endif /*SERIALIZABLE_H_*/
