/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include "system/platform.h"

#include "system/lang/StringBuffer.h"
#include "system/lang/Variable.h"

#include "system/io/Serializable.h"
#include "system/io/ObjectOutputStream.h"
#include "system/io/ObjectInputStream.h"

namespace sys {
  namespace util {

	template<class K, class V> class HashTable;
	template<class K, class V> class HashTableIterator;

	template<class K, class V> class Entry {
		int hash;
		K key;
		V value;
		Entry* next;

	public:
		Entry() {
			next = NULL;
			hash = 0;
		}

		Entry(const Entry<K, V>& e) {
			hash = e.hash;
			key = e.key;
			value = e.value;
			next = e.next;
		}

		Entry(int hash, const K& key, const V& value, Entry* e) : hash(hash), key(key),
				value(value), next(e) {
		}

#ifdef CXX11_COMPILER
		Entry(Entry&& e) {
			Entry::hash = e.hash;
			Entry::key = std::move(e.key);
			Entry::value = std::move(e.value);

			next = e.next;

			e.next = NULL;
			e.hash = 0;
		}
#endif

		V& getValue() {
			return value;
		}

		K& getKey() {
			return key;
		}

		friend class HashTable<K,V>;
		friend class HashTableIterator<K,V>;
	};

	template<class K, class V> class HashTable : public Variable {
	    Entry<K,V>** table;
		int tableLength;

	    int count;

	    int initialCapacity;
	    int threshold;
	    float loadFactor;

	    int modCount;

	    virtual int hash(const K& key) const {
	    	return TypeInfo<K>::hashCode(key);
	    }

	protected:
		V nullValue;

	public:
	    HashTable();
		HashTable(int initialCapacity);
	    HashTable(int initialCapacity, float loadFactor);

	    HashTable(const HashTable<K, V>& table);

#ifdef CXX11_COMPILER
	    HashTable(HashTable<K, V>&& table);
#endif

		virtual ~HashTable();

	    V& put(const K& key, const V& value);

	    V& get(const K& key);

	    Entry<K, V>* getEntry(const K& key) const;

	    bool containsKey(const K& key);

	    void copyFrom(HashTable<K, V>* htable);

	    HashTable<K, V>& operator=(const HashTable<K, V>& htable);

#ifdef CXX11_COMPILER
	    HashTable<K, V>& operator=(HashTable<K, V>&& htable);
#endif

	    HashTableIterator<K, V> iterator();

	    V remove(const K& key);

	    void removeAll();

	    /*bool toString(String& str);
	    bool parseFromString(const String& str, int version = 0);*/

	    bool toBinaryStream(ObjectOutputStream* stream);
	    bool parseFromBinaryStream(ObjectInputStream* stream);

	protected:
	    void init(int initialCapacity, float loadFactor);

	   	void rehash();

	public:
	 	inline int size() {
			return count;
	    }

	    inline bool isEmpty() {
			return count == 0;
	    }

	    inline void setNullValue(const V& val) {
	    	nullValue = val;
	    }

	    inline V& getNullValue() {
	    	return nullValue;
	    }

	    friend class HashTableIterator<K,V>;

	};

	template<class K, class V> class HashTableIterator {
		HashTable<K,V>* htable;
		Entry<K,V>* e;

		int position;
		int eIndex;

	public:
		HashTableIterator();
		HashTableIterator(const HashTableIterator<K, V>& iter);
		HashTableIterator(HashTable<K,V>* Table);

		V& getNextValue();

		K& getNextKey();

		void getNextKeyAndValue(K& key, V& value);

		V& next();

		bool hasNext();

		void resetIterator();

		void toConsole();

		inline void setTable(HashTable<K,V>* table) {
			htable = table;
		}
	};

	template<class K, class V> HashTable<K,V>::HashTable(int initialCapacity, float loadFactor)
		: Variable() {
		init(initialCapacity, loadFactor);
	}

	template<class K, class V> HashTable<K,V>::HashTable(int initialCapacity) : Variable() {
		init(initialCapacity, 0.75f);
	}

	template<class K, class V> HashTable<K,V>::HashTable() : Variable() {
		init(11, 0.75f);
	}

	template<class K, class V> HashTable<K,V>::HashTable(const HashTable<K,V>& htable) : Variable() {
		init(11, 0.75f);

		copyFrom(const_cast<HashTable<K,V>* >(&htable));
	}

#ifdef CXX11_COMPILER
	template<class K, class V> HashTable<K,V>::HashTable(HashTable<K,V>&& htable) : Variable(),
			table(htable.table), tableLength(htable.tableLength), count(htable.count),
			initialCapacity(htable.initialCapacity), threshold(htable.threshold),
			loadFactor(htable.loadFactor), modCount(htable.modCount), nullValue(htable.nullValue) {

		htable.table = NULL;
	}
#endif

	template<class K, class V> HashTable<K,V>& HashTable<K,V>::operator=(const HashTable<K,V>& htable) {
		if (this == &htable)
			return *this;

		removeAll();

		copyFrom(const_cast<HashTable<K,V>* >(&htable));

		return *this;
	}

#ifdef CXX11_COMPILER
	template<class K, class V> HashTable<K,V>& HashTable<K,V>::operator=(HashTable<K,V>&& htable) {
		if (this == &htable)
			return *this;

		removeAll();

		free(table);

		table = htable.table;
		htable.table = NULL;

		tableLength = htable.tableLength;

		count = htable.count;

		htable.count = 0;

		initialCapacity = htable.initialCapacity;
		threshold = htable.threshold;
		loadFactor = htable.loadFactor;

		modCount = htable.modCount;

		nullValue = htable.nullValue;

		return *this;
	}
#endif

	template<class K, class V> void HashTable<K,V>::copyFrom(HashTable<K,V>* htable) {
		nullValue = htable->nullValue;

		HashTableIterator<K, V> iterator(htable);

		while (iterator.hasNext()) {
			K key;
			V value;

			iterator.getNextKeyAndValue(key, value);

			put(key, value);
		}
	}

	template<class K, class V> HashTable<K,V>::~HashTable() {
		if (table != NULL) {
			removeAll();

			free(table);
		}
	}

	template<class K, class V> void HashTable<K,V>::init(int initialCapacity, float loadFactor) {
		setNullValue(TypeInfo<V>::nullValue());

		if (initialCapacity == 0)
			initialCapacity = 1;

		HashTable::initialCapacity = initialCapacity;

		table = (Entry<K, V>**) malloc(initialCapacity * sizeof(Entry<K, V>*));
		memset(table, 0, initialCapacity * sizeof(Entry<K, V>*));
		tableLength = initialCapacity;

		HashTable::loadFactor = loadFactor;
		threshold = (int) (initialCapacity * loadFactor);

	   	count = 0;
	   	modCount = 0;
	}

	template<class K, class V> void HashTable<K,V>::rehash() {
		int oldCapacity = tableLength;
		Entry<K,V>** oldMap = table;

		int newCapacity = oldCapacity * 2 + 1;

		Entry<K,V>** newMap = (Entry<K, V>**) malloc(newCapacity * sizeof(Entry<K, V>*));
		memset(newMap, 0, newCapacity * sizeof(Entry<K, V>*));

		modCount++;
		threshold = (int) (newCapacity * loadFactor);
		table = newMap;

		for (int i = oldCapacity; i-- > 0 ;) {
	    	for (Entry<K,V>* old = oldMap[i]; old != NULL ;) {
				Entry<K,V>* e = old;
				old = old->next;

				int index = (e->hash & 0x7FFFFFFF) % newCapacity;
				e->next = newMap[index];
				newMap[index] = e;
	    	}
		}

		tableLength = newCapacity;

		free(oldMap);
	}

	template<class K, class V> V& HashTable<K,V>::put(const K& key, const V& value) {
		int hashCode = hash(key);
		int index = (hashCode & 0x7FFFFFFF) % tableLength;

		for (Entry<K,V>* e = table[index]; e != NULL; e = e->next) {
	    	if ((e->hash == hashCode) && e->key == key) {
				V& old = e->value;
				e->value = value;
				return old;
	    	}
		}

		modCount++;
		if (count >= threshold) {
	    	rehash();

			index = (hashCode & 0x7FFFFFFF) % tableLength;
		}

		Entry<K,V>* e = table[index];
		table[index] = new Entry<K,V>(hashCode, key, value, e);
		count++;

		return nullValue;
	}

	template<class K, class V> V& HashTable<K,V>::get(const K& key) {
		int hashCode = hash(key);
		int index = (hashCode & 0x7FFFFFFF) % tableLength;

		for (Entry<K,V>* e = table[index]; e != NULL; e = e->next) {
	    	if ((e->hash == hashCode) && e->key == key) {
				return e->value;
	    	}
		}

		return nullValue;
	}

	template<class K, class V> bool HashTable<K,V>::containsKey(const K& key) {
		int hashCode = hash(key);
		int index = (hashCode & 0x7FFFFFFF) % tableLength;

		for (Entry<K,V>* e = table[index]; e != NULL; e = e->next) {
		    if ((e->hash == hashCode) && e->key == key) {
				return true;
	    	}
		}

		return false;
	}

	template<class K, class V> Entry<K,V>* HashTable<K,V>::getEntry(const K& key) const {
		int hashCode = hash(key);
		int index = (hashCode & 0x7FFFFFFF) % tableLength;

		for (Entry<K,V>* e = table[index]; e != NULL; e = e->next) {
			if ((e->hash == hashCode) && e->key == key) {
				return e;
			}
		}

		return NULL;
	}

	template<class K, class V> HashTableIterator<K, V> HashTable<K,V>::iterator() {
		return HashTableIterator<K, V>(this);
	}

	template<class K, class V> V HashTable<K,V>::remove(const K& key) {
		int hashCode = hash(key);
		int index = (hashCode & 0x7FFFFFFF) % tableLength;

		for (Entry<K,V> *e = table[index], *prev = NULL; e != NULL; prev = e, e = e->next) {
	    	if ((e->hash == hashCode) && e->key == key) {
				if (prev != NULL)
			    	prev->next = e->next;
				else
		    		table[index] = e->next;

				count--;
				V oldValue = e->value;
				e->value = nullValue;

				delete e;

				return oldValue;
	    	}
		}

		return nullValue;
	}

	template<class K, class V> void HashTable<K,V>::removeAll() {
		if (count == 0)
			return;

		for (int i = 0; i < tableLength; ++i) {
			for (Entry<K,V>* e = table[i]; e != NULL;) {
				Entry<K,V>* next = e->next;

				delete e;
				e = next;
			}

			table[i] = NULL;
		}

		count = 0;
	}

	/*template<class K, class V> bool HashTable<K,V>::toString(String& str) {
		StringBuffer buffer;
		buffer << "{" << HashTable<K, V>::size() << ",";
		HashTableIterator<K, V> iterator(this);

		while (iterator.hasNext()) {
			K key;
			V value;

			iterator.getNextKeyAndValue(key, value);

			String keyData, valueData;
			TypeInfo<K>::toString(&key, keyData);
			TypeInfo<V>::toString(&value, valueData);

			buffer << "{" << keyData << "},{" << valueData << "},";
		}

		buffer << "}";

		buffer.toString(str);

		return true;
	}

	template<class K, class V> bool HashTable<K,V>::parseFromString(const String& str, int version) {
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

			if (TypeInfo<K>::parseFromString(&key, keyData.subString(1, keyData.length() - 1))
					&& TypeInfo<V>::parseFromString(&value, valueData.subString(1, valueData.length() - 1)))

				HashTable<K, V>::put(key, value);
		}

		return true;
	}*/

	template<class K, class V>  bool HashTable<K,V>::toBinaryStream(ObjectOutputStream* stream) {
		HashTableIterator<K, V> iterator(this);
		stream->writeInt(HashTable<K, V>::size());

		while (iterator.hasNext()) {
			K key;
			V value;

			iterator.getNextKeyAndValue(key, value);

			TypeInfo<K>::toBinaryStream(&key, stream);
			TypeInfo<V>::toBinaryStream(&value, stream);
		}

		return true;
	}

	template<class K, class V>  bool HashTable<K,V>::parseFromBinaryStream(ObjectInputStream* stream) {
		HashTable<K, V>::removeAll();

		int size = stream->readInt();

		for (int i = 0; i < size; ++i) {
			K key;
			V value;

			if (TypeInfo<K>::parseFromBinaryStream(&key, stream) && TypeInfo<V>::parseFromBinaryStream(&value, stream))
				HashTable<K, V>::put(key, value);
		}

		return true;
	}

	template<class K, class V> HashTableIterator<K,V>::HashTableIterator() {
		htable = NULL;

		position = 0;
		eIndex = -1;

		e = NULL;
	}

	template<class K, class V> HashTableIterator<K,V>::HashTableIterator(const HashTableIterator<K, V>& iter) {
		htable = iter.htable;
		e = iter.e;

		position = iter.position;
		eIndex = iter.eIndex;
	}

	template<class K, class V> HashTableIterator<K,V>::HashTableIterator(HashTable<K,V>* Table) {
		htable = Table;

		position = 0;
		eIndex = -1;

		e = NULL;
	}

	template<class K, class V> V& HashTableIterator<K,V>::getNextValue() {
		while (e == NULL)
			e = htable->table[++eIndex];

		V& result = e->value;

		e = e->next;
		position++;

		return result;
	}

	template<class K, class V> K& HashTableIterator<K,V>::getNextKey() {
		while (e == NULL)
			e = htable->table[++eIndex];

		K& result = e->key;

		e = e->next;
		position++;

		return result;
	}

	template<class K, class V> void HashTableIterator<K,V>::getNextKeyAndValue(K& key, V& value) {
		while (e == NULL)
			e = htable->table[++eIndex];

		key = e->key;
		value = e->value;

		e = e->next;
		position++;
	}

	template<class K, class V> V& HashTableIterator<K,V>::next() {
		return getNextValue();
	}

	template<class K, class V> bool HashTableIterator<K,V>::hasNext() {
	 	return position < htable->size();
	}

	template<class K, class V> void HashTableIterator<K,V>::resetIterator() {
		position = 0;
		eIndex = -1;

		e = NULL;
	}

	template<class K, class V> void HashTableIterator<K,V>::toConsole() {
		while (hasNext()) {
			while (e == NULL)
				e = htable->table[++eIndex];

			System::out << e->key << " = " << e->value << "\n";

			e = e->next;
			position++;
		}
	}

  } // namespace util
} // namespace sys

using namespace sys::util;

#endif /*HASHTABLE_H_*/
