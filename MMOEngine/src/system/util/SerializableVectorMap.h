/*
 * SerializableVectorMap.h
 *
 *  Created on: May 1, 2009
 *      Author: theanswer
 */

#ifndef SERIALIZABLEVECTORMAP_H_
#define SERIALIZABLEVECTORMAP_H_

#include "SerializableSortedVector.h"

namespace sys {
	namespace util {
		template<class K, class V> class SerializableVectorMap;

		template<class K, class V> class SerializableVectorMapEntry : public virtual Variable {
			K key;
			V value;

		public:
			SerializableVectorMapEntry() : Variable() {
			}

			SerializableVectorMapEntry(K key) : Variable() {
				SerializableVectorMapEntry::key = key;
			}

			SerializableVectorMapEntry(K key, V value) : Variable() {
				SerializableVectorMapEntry::key = key;
				SerializableVectorMapEntry::value = value;
			}

			SerializableVectorMapEntry(const SerializableVectorMapEntry& val) : Variable() {
				SerializableVectorMapEntry::key = val.key;
				SerializableVectorMapEntry::value = val.value;
			}

			SerializableVectorMapEntry(SerializableVectorMapEntry& val) : Variable() {
				SerializableVectorMapEntry::key = val.key;
				SerializableVectorMapEntry::value = val.value;
			}


			int compareTo(const SerializableVectorMapEntry& e) {
				return (key.compareTo(e.key));
			}

			K getKey() {
				return key;
			}

			V getValue() {
				return value;
			}

			bool toString(String& str) {
				// structure key=value
				String stringKey, stringValue;

				key.toString(stringKey);
				value.toString(stringValue);

				StringBuffer buffer;
				buffer << "{{" << stringKey << "},{" << stringValue << "}}";

				buffer.toString(str);

				return true;
			}

			bool parseFromString(const String& str, int version = 0) {
				String data;

				if (!Serializable::getObjectData(str, data))
					return true;

				String stringKey, stringValue;

				int size = Serializable::getObjectData(data.subString(1), stringKey);
				Serializable::getObjectData(data.subString(size + 1), stringValue);

				if (!key.parseFromString(stringKey.subString(1, stringKey.length() - 1))
						|| !value.parseFromString(stringValue.subString(1, stringValue.length() - 1)))
					return false;

				return true;
			}

			bool toBinaryStream(ObjectOutputStream* stream) {
				key.toBinaryStream(stream);
				value.toBinaryStream(stream);

				return true;
			}

			bool parseFromBinaryStream(ObjectInputStream* stream) {
				if (!key.parseFromBinaryStream(stream) || value.parseFromBinaryStream(stream))
					return false;

				return true;
			}


			friend class SerializableVectorMap<K, V>;
		};

		template<class K, class V> class SerializableVectorMap : public SerializableSortedVector<SerializableVectorMapEntry<K, V> > {
			V nullValue;

		public:
			SerializableVectorMap();
			SerializableVectorMap(int initsize, int incr);

			virtual ~SerializableVectorMap();

			int put(const K& key, const V& value);

			V get(int index);
			V get(const K& key);
			SerializableVectorMapEntry<K, V> getEntry(int index);

			int find(const K& key);

			bool contains(const K& key);

			bool drop(const K& key);

			inline void setNullValue(V val) {
				nullValue = val;
			}

		protected:
			virtual int compare(SerializableVectorMapEntry<K, V>& e1, SerializableVectorMapEntry<K, V>& e2);

		};

		template<class K, class V> SerializableVectorMap<K, V>::SerializableVectorMap()
		: SerializableSortedVector<SerializableVectorMapEntry<K, V> >() {
			setInsertPlan(SortedVector<SerializableVectorMapEntry<K, V> >::ALLOW_OVERWRITE);
		}

		template<class K, class V> SerializableVectorMap<K, V>::SerializableVectorMap(int initsize, int incr)
		: SerializableSortedVector<SerializableVectorMapEntry<K, V> >(initsize, incr) {
			setInsertPlan(SortedVector<SerializableVectorMapEntry<K, V> >::ALLOW_OVERWRITE);
		}

		template<class K, class V> SerializableVectorMap<K, V>::~SerializableVectorMap() {
		}

		template<class K, class V> int SerializableVectorMap<K, V>::put(const K& key, const V& value) {
			SerializableVectorMapEntry<K, V> e(key, value);

			int res = SerializableSortedVector<SerializableVectorMapEntry<K, V> >::put(e);

			return res;
		}

		template<class K, class V> V SerializableVectorMap<K, V>::get(int index) {
			SerializableVectorMapEntry<K, V> entry = SerializableSortedVector<SerializableVectorMapEntry<K, V> >::get(index);
			return entry.value;
		}

		template<class K, class V> V SerializableVectorMap<K, V>::get(const K& key) {
			SerializableVectorMapEntry<K, V> e(key);

			int pos = SerializableSortedVector<SerializableVectorMapEntry<K, V> >::find(e);
			if (pos == -1)
				return nullValue;

			SerializableVectorMapEntry<K, V> entry = SerializableSortedVector<SerializableVectorMapEntry<K, V> >::get(pos);
			return entry.value;
		}

		template<class K, class V> SerializableVectorMapEntry<K, V> SerializableVectorMap<K, V>::getEntry(int index) {
			SerializableVectorMapEntry<K, V> entry = SerializableSortedVector<SerializableVectorMapEntry<K, V> >::get(index);
			return entry;
		}

		template<class K, class V> int SerializableVectorMap<K, V>::find(const K& key) {
			SerializableVectorMapEntry<K, V> e(key);

			return SerializableSortedVector<SerializableVectorMapEntry<K, V> >::find(e);
		}

		template<class K, class V> bool SerializableVectorMap<K, V>::contains(const K& key) {
			SerializableVectorMapEntry<K, V> e(key);

			int idx = SerializableSortedVector<SerializableVectorMapEntry<K, V> >::find(e);

			if (idx == -1)
				return false;
			else
				return true;
		}

		template<class K, class V> bool SerializableVectorMap<K, V>::drop(const K& key) {
			int pos = find(key);
			if (pos == -1)
				return false;

			SerializableSortedVector<SerializableVectorMapEntry<K, V> >::remove(pos);

			return true;
		}

		template<class K, class V> int SerializableVectorMap<K, V>::compare(SerializableVectorMapEntry<K, V>& e1, SerializableVectorMapEntry<K, V>& e2) {
			/*if (e1.key < e2.key)
				return 1;
			else if (e1.key > e2.key)
				return -1;
			else
				return 0;*/
			return e1.compareTo(e2);
		}
	}
}


#endif /* SERIALIZABLEVECTORMAP_H_ */
