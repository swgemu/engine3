/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef SERIALIZABLEVECTOR_H_
#define SERIALIZABLEVECTOR_H_

#include "../lang/String.h"
#include "../lang/StringBuffer.h"

#include "../io/ObjectOutputStream.h"
#include "../io/ObjectInputStream.h"

#include "../io/StringTokenizer.h"
#include "../io/Serializable.h"

namespace sys {
	namespace util {

		template<class E> class SerializableVector : public Vector<E>, public Variable {
		public:
			SerializableVector();

			SerializableVector(int incr);

			SerializableVector(int initsize, int incr);

			bool toString(String& str);
			bool parseFromString(const String& str, int version = 0);

			bool toBinaryStream(ObjectOutputStream* stream);

			bool parseFromBinaryStream(ObjectInputStream* stream);
		};

		template<class E> SerializableVector<E>::SerializableVector() : Vector<E>(), Variable() {

		}

		template<class E> SerializableVector<E>::SerializableVector(int incr) : Vector<E>(incr), Variable() {

		}

		template<class E> SerializableVector<E>::SerializableVector(int initsize, int incr) : Vector<E>(initsize, incr), Variable() {

		}

		template<class E> bool SerializableVector<E>::toString(String& str) {
			StringBuffer buffer;
			buffer << "{" << Vector<E>::size();

			for (int i = 0; i < Vector<E>::size(); ++i) {
				String data;
				Vector<E>::get(i).toString(data);
				buffer << "," << data;
			}

			buffer << "}";

			buffer.toString(str);

			return true;
		}

		template<class E> bool SerializableVector<E>::parseFromString(const String& str, int version) {
			Vector<E>::removeAll();

			String data;

			if (!Serializable::getObjectData(str, data))
				return true;

			int comma = data.indexOf(",");

			int variableSize = Integer::valueOf(data.subString(1, comma));

			for (int i = 0; i < variableSize; ++i) {
				data = data.subString(comma + 1);

				String variableData;

				if (data.subString(0, 1).indexOf("{") != -1) {
					int lastSemiColon = Serializable::getObjectData(data, variableData);

					comma = lastSemiColon;
				} else {
					comma = data.indexOf(",");
					variableData = data.subString(0, comma);
				}

				E object;

				if (object.parseFromString(variableData))
					add(object);
			}

			return true;
		}

		template<class E> bool SerializableVector<E>::toBinaryStream(ObjectOutputStream* stream) {
			stream->writeInt(Vector<E>::size());

			for (int i = 0; i < Vector<E>::size(); ++i) {
				Vector<E>::get(i).toBinaryStream(stream);
			}

			return true;
		}

		template<class E> bool SerializableVector<E>::parseFromBinaryStream(ObjectInputStream* stream) {
			Vector<E>::removeAll();

			int size = stream->readInt();

			for (int i = 0; i < size; ++i) {
				E object;

				if (object.parseFromBinaryStream(stream))
					add(object);
			}

			return true;
		}

	}
}

using namespace sys::util;

#endif /* SERIALIZABLEVECTOR_H_ */
