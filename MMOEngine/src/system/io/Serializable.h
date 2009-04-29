/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef SERIALIZABLE_H_
#define SERIALIZABLE_H_

#include "../platform.h"

#include "../lang/String.h"
#include "../util/VectorMap.h"


namespace sys {
	namespace io {

		class ObjectOutputStream;
		class ObjectInputStream;

		class Serializable {
			VectorMap<String, Variable*> variables;

		public:
			Serializable();

			virtual ~Serializable() {

			}

			virtual void serialize(String& str);
			virtual void serialize(ObjectOutputStream* stream);

			virtual void deSerialize(const String& str);
			virtual void deSerialize(ObjectInputStream* stream);


			void addSerializableVariable(const String& nameAndVersion, Variable* variable) {
				variables.put(nameAndVersion, variable);
			}

			Variable* getSerilizableVariable(const String& nameAndVersion) {
				return variables.get(nameAndVersion);
			}

		private:
			void deSerializeVariable(const String& var);
		};
	}
}

using namespace sys::io;

#endif /*SERIALIZABLE_H_*/
