/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef VECTORMAP_H_
#define VECTORMAP_H_

#include "system/platform.h"

#include "SortedVector.h"

namespace sys {
  namespace util {

	template<class K, class V> class VectorMap;

	template<class K, class V> class VectorMapEntry : public Variable {
		K key;
		V value;

	public:
		VectorMapEntry() : Variable() {
		}

		VectorMapEntry(K key) : Variable() {
			VectorMapEntry::key = key;
		}

		VectorMapEntry(K key, V value) : Variable() {
			VectorMapEntry::key = key;
			VectorMapEntry::value = value;
		}

		VectorMapEntry(const VectorMapEntry& entry) : Variable() {
			key = entry.key;
			value = entry.value;
		}

		VectorMapEntry& operator=(const VectorMapEntry& entry) {
			if (this == &entry)
				return *this;

			key = entry.key;
			value = entry.value;

			return *this;
		}

		~VectorMapEntry() {

		}

		int compareTo(const VectorMapEntry& e) const {
			return TypeInfo<K>::compare(key, e.key);
		}

		K& getKey() {
			return key;
		}

		V& getValue() {
			return value;
		}

		/*bool toString(String& str) {
			// structure key=value
			String stringKey, stringValue;

			TypeInfo<K>::toString(&key, stringKey);
			TypeInfo<V>::toString(&value, stringValue);

			StringBuffer buffer;
			buffer << "{{" << stringKey << "},{" << stringValue << "}}";

			buffer.toString(str);

			return true;
		}

		bool parseFromString(const String& str, int version = 0) {
			String data;

			if (!Vector<int>::getObjectData(str, data))
				return true;

			String stringKey, stringValue;

			int size = Vector<int>::getObjectData(data.subString(1), stringKey);
			Vector<int>::getObjectData(data.subString(size + 1), stringValue);

			if (!TypeInfo<K>::parseFromString(&key, stringKey.subString(1, stringKey.length() - 1))
					|| !TypeInfo<V>::parseFromString(&value, stringValue.subString(1, stringValue.length() - 1)))
				return false;

			return true;
		}*/

		bool toBinaryStream(ObjectOutputStream* stream) {
			TypeInfo<K>::toBinaryStream(&key, stream);
			TypeInfo<V>::toBinaryStream(&value, stream);

			return true;
		}

		bool parseFromBinaryStream(ObjectInputStream* stream) {
			if (!TypeInfo<K>::parseFromBinaryStream(&key, stream) || !TypeInfo<V>::parseFromBinaryStream(&value, stream))
				return false;

			return true;
		}

		friend class VectorMap<K,V>;
	};

	template<class K, class V> class VectorMap : public SortedVector<VectorMapEntry<K, V> > {
		V nullValue;

	public:
		VectorMap();
		VectorMap(int initsize, int incr);
		VectorMap(const VectorMap<K, V>& vector);

		VectorMap<K, V>& operator=(const VectorMap<K, V>& vector);

		virtual ~VectorMap();

		int put(const K& key, const V& value);

		V& get(int index);
		V& get(const K& key);

		int find(const K& key);

		Object* clone();

		bool contains(const K& key);

		bool drop(const K& key);

		inline void setNullValue(V val) {
			nullValue = val;
		}

	protected:
		virtual int compare(VectorMapEntry<K, V>& e1, VectorMapEntry<K, V>& e2);

	};

	template<class K, class V> VectorMap<K, V>::VectorMap()
			: SortedVector<VectorMapEntry<K, V> >() {
		setNullValue(TypeInfo<V>::nullValue());

		SortedVector<VectorMapEntry<K, V> >::setInsertPlan(SortedVector<VectorMapEntry<K, V> >::ALLOW_OVERWRITE);
	}

	template<class K, class V> VectorMap<K, V>::VectorMap(int initsize, int incr)
			: SortedVector<VectorMapEntry<K, V> >(initsize, incr) {
		setNullValue(TypeInfo<V>::nullValue());

		SortedVector<VectorMapEntry<K, V> >::setInsertPlan(SortedVector<VectorMapEntry<K, V> >::ALLOW_OVERWRITE);
	}

	template<class K, class V> VectorMap<K, V>::VectorMap(const VectorMap<K, V>& vector)
			: SortedVector<VectorMapEntry<K, V> >(vector) {
		nullValue = vector.nullValue;
	}

	template<class K, class V> VectorMap<K, V>& VectorMap<K, V>::operator=(const VectorMap<K, V>& vector) {
		if (this == &vector)
			return *this;

		nullValue = vector.nullValue;

		SortedVector<VectorMapEntry<K, V> >::operator=(vector);

		return *this;
	}

	template<class K, class V> VectorMap<K, V>::~VectorMap() {

	}

	template<class K, class V> Object* VectorMap<K, V>::clone() {
		return new VectorMap<K, V>(*this);
	}

	template<class K, class V> int VectorMap<K, V>::put(const K& key, const V& value) {
	 	VectorMapEntry<K, V> e(key, value);

	 	if ((SortedVector<VectorMapEntry<K, V> >::getInsertPlan() == (SortedVector<VectorMapEntry<K, V> >::ALLOW_OVERWRITE)) && contains(key)) {
			drop(key);
	 	}

	 	int res = SortedVector<VectorMapEntry<K, V> >::put(e);

	 	return res;
	}

	template<class K, class V> V& VectorMap<K, V>::get(int index) {
		VectorMapEntry<K, V>* entry = &SortedVector<VectorMapEntry<K, V> >::get(index);
	 	return entry->value;
	}

	template<class K, class V> V& VectorMap<K, V>::get(const K& key) {
		if (ArrayList<VectorMapEntry<K, V> >::size() == 0)
			return nullValue;

	 	VectorMapEntry<K, V> e(key);

	 	int pos = SortedVector<VectorMapEntry<K, V> >::find(e);
		if (pos == -1)
	 		return nullValue;

	 	VectorMapEntry<K, V>* entry = &SortedVector<VectorMapEntry<K, V> >::get(pos);
	 	return entry->value;
	}

	template<class K, class V> int VectorMap<K, V>::find(const K& key) {
		if (ArrayList<VectorMapEntry<K, V> >::size() == 0)
			return -1;

		VectorMapEntry<K, V> e(key);

	 	return SortedVector<VectorMapEntry<K, V> >::find(e);
	}

	template<class K, class V> bool VectorMap<K, V>::contains(const K& key) {
		if (ArrayList<VectorMapEntry<K, V> >::size() == 0)
			return false;

		VectorMapEntry<K, V> e(key);

	 	int idx = SortedVector<VectorMapEntry<K, V> >::find(e);

	 	if (idx == -1)
	 		return false;
	 	else
	 		return true;
	}

	template<class K, class V> bool VectorMap<K, V>::drop(const K& key) {
		int pos = find(key);
		if (pos == -1)
			return false;

		SortedVector<VectorMapEntry<K, V> >::remove(pos);

	 	return true;
	}

	template<class K, class V> int VectorMap<K, V>::compare(VectorMapEntry<K, V>& e1, VectorMapEntry<K, V>& e2) {
		return TypeInfo<K>::compare(e1.key, e2.key);
	}

  } // namespace util
} // namespace sys

using namespace sys::util;

#endif /*VECTORMAP_H_*/
