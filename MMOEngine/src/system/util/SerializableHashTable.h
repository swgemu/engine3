#ifndef SERIALIZABLEHASHTABLE_H_
#define SERIALIZABLEHASHTABLE_H_

#include "../lang.h"

#include "HashTable.h"
#include "../lang/StringBuffer.h"
#include "../lang/Variable.h"
#include "../io/Serializable.h"
#include "../io/ObjectOutputStream.h"
#include "../io/ObjectInputStream.h"

namespace sys {
	namespace util {

	template<class K, class V> class SerializableHashTable;

	template<class K, class V> class SerializableHashTable : public HashTable<K, V>, public Variable {

	public:
		SerializableHashTable();
		SerializableHashTable(int initialCapacity);
		SerializableHashTable(int initialCapacity, float loadFactor);

		~SerializableHashTable();

		bool toString(String& str);
		bool parseFromString(const String& str, int version = 0);

		bool toBinaryStream(ObjectOutputStream* stream);
		bool parseFromBinaryStream(ObjectInputStream* stream);

		friend class HashTableIterator<K,V>;

	};


	template<class K, class V> SerializableHashTable<K,V>::SerializableHashTable(int initialCapacity, float loadFactor)
		: HashTable<K, V>(initialCapacity, loadFactor), Variable() {

	}

	template<class K, class V> SerializableHashTable<K,V>::SerializableHashTable(int initialCapacity)
		: HashTable<K, V>(initialCapacity), Variable() {

	}

	template<class K, class V> SerializableHashTable<K,V>::SerializableHashTable() : HashTable<K, V>(), Variable() {

	}

	template<class K, class V> SerializableHashTable<K,V>::~SerializableHashTable() {

	}

	template<class K, class V> bool SerializableHashTable<K,V>::toString(String& str) {
		StringBuffer buffer;
		buffer << "{" << HashTable<K, V>::size() << ",";
		HashTableIterator<K, V> iterator(this);

		while (iterator.hasNext()) {
			K key;
			V value;

			iterator.getNextKeyAndValue(key, value);

			String keyData, valueData;
			key.toString(keyData);
			value.toString(valueData);

			buffer << "{" << keyData << "},{" << valueData << "},";
		}

		buffer << "}";

		buffer.toString(str);

		return true;
	}

	template<class K, class V> bool SerializableHashTable<K,V>::parseFromString(const String& str, int version) {
		HashTable<K, V>::removeAll();

		String data;

		if (!Serializable::getObjectData(str, data))
			return true;

		int comma = data.indexOf(",");

		int variableSize = Integer::valueOf(data.subString(1, comma));

		for (int i = 0; i < variableSize; ++i) {
			data = data.subString(comma + 1);

			String keyData, valueData;

			int dataSize = Serializable::getObjectData(data, keyData);
			int equal = data.subString(dataSize).indexOf(",");
			int valueSize = Serializable::getObjectData(data.subString(equal + 1), valueData);

			comma = equal + 1 + valueSize;

			K key;
			V value;

			if (key.parseFromString(keyData.subString(1, keyData.length() - 1)) && value.parseFromString(valueData.subString(1, valueData.length() - 1)))
				HashTable<K, V>::put(key, value);
		}

		return true;
	}

	template<class K, class V>  bool SerializableHashTable<K,V>::toBinaryStream(ObjectOutputStream* stream) {
		HashTableIterator<K, V> iterator(this);
		stream->writeInt(HashTable<K, V>::size());

		while (iterator.hasNext()) {
			K key;
			V value;

			iterator.getNextKeyAndValue(key, value);

			key.toBinaryStream(stream);
			value.toBinaryStream(stream);
		}

		return true;
	}

	template<class K, class V>  bool SerializableHashTable<K,V>::parseFromBinaryStream(ObjectInputStream* stream) {
		HashTable<K, V>::removeAll();

		int size = stream->readInt();

		for (int i = 0; i < size; ++i) {
			K key;
			V value;

			if (key.parseFromBinaryStream(stream) && value.parseFromBinaryStream(stream))
				HashTable<K, V>::put(key, value);
		}

		return true;
	}

	}
}


#endif /* SERIALIZABLEHASHTABLE_H_ */
