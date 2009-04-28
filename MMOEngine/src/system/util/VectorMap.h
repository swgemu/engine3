/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef VECTORMAP_H_
#define VECTORMAP_H_

#include "SortedVector.h"

namespace sys {
  namespace util {

	template<class K, class V> class VectorMap;

	template<class K, class V> class VectorMapEntry {
		K key;
		V value;

	public:
		VectorMapEntry() {
		}

		VectorMapEntry(K key) {
			VectorMapEntry::key = key;
		}

		VectorMapEntry(K key, V value) {
			VectorMapEntry::key = key;
			VectorMapEntry::value = value;
		}

		int compareTo(VectorMapEntry*& e) {
			if (key < e->key)
				return 1;
			else if (key > e->key)
				return -1;
			else
				return 0;
		}

		K& getKey() {
			return key;
		}

		V& getValue() {
			return value;
		}

		friend class VectorMap<K,V>;
	};

	template<class K, class V> class VectorMap : public SortedVector<VectorMapEntry<K, V>*> {
		V nullValue;

	public:
		VectorMap();
		VectorMap(int initsize, int incr);

		virtual ~VectorMap();

		int put(const K& key, const V& value);

		V& get(int index);
		V& get(const K& key);

		int find(const K& key);

		bool contains(const K& key);

		bool drop(const K& key);

		void removeAll();

		inline void setNullValue(V val) {
			nullValue = val;
		}

	protected:
		virtual int compare(VectorMapEntry<K, V>*& e1, VectorMapEntry<K, V>*& e2);

	};

	template<class K, class V> VectorMap<K, V>::VectorMap()
			: SortedVector<VectorMapEntry<K, V>*>() {
		setInsertPlan(SortedVector<VectorMapEntry<K, V>*>::ALLOW_OVERWRITE);
	}

	template<class K, class V> VectorMap<K, V>::VectorMap(int initsize, int incr)
			: SortedVector<VectorMapEntry<K, V>*>(initsize, incr) {
		setInsertPlan(SortedVector<VectorMapEntry<K, V>*>::ALLOW_OVERWRITE);
	}

	template<class K, class V> VectorMap<K, V>::~VectorMap() {
		removeAll();
	}

	template<class K, class V> int VectorMap<K, V>::put(const K& key, const V& value) {
	 	VectorMapEntry<K, V>* e = new VectorMapEntry<K, V>(key, value);

	 	if ((SortedVector<VectorMapEntry<K, V>*>::getInsertPlan() == (SortedVector<VectorMapEntry<K, V>*>::ALLOW_OVERWRITE)) && contains(key)) {
			drop(key);
	 	}

	 	int res = SortedVector<VectorMapEntry<K, V>*>::put(e);

	 	if (res == -1)
	 		delete e;

	 	return res;
	}

	template<class K, class V> V& VectorMap<K, V>::get(int index) {
		VectorMapEntry<K, V>* entry = SortedVector<VectorMapEntry<K, V>*>::get(index);
	 	return entry->value;
	}

	template<class K, class V> V& VectorMap<K, V>::get(const K& key) {
	 	VectorMapEntry<K, V> e(key);
	 	VectorMapEntry<K, V>* eptr = &e;

	 	int pos = SortedVector<VectorMapEntry<K, V>*>::find(eptr);
		if (pos == -1)
	 		return nullValue;

	 	VectorMapEntry<K, V>* entry = SortedVector<VectorMapEntry<K, V>*>::get(pos);
	 	return entry->value;
	}

	template<class K, class V> int VectorMap<K, V>::find(const K& key) {
		VectorMapEntry<K, V> e(key);
		VectorMapEntry<K, V>* eptr = &e;

	 	return SortedVector<VectorMapEntry<K, V>*>::find(eptr);
	}

	template<class K, class V> bool VectorMap<K, V>::contains(const K& key) {
		VectorMapEntry<K, V> e(key);
	 	VectorMapEntry<K, V>* eptr = &e;

	 	int idx = SortedVector<VectorMapEntry<K, V>*>::find(eptr);

	 	if (idx == -1)
	 		return false;
	 	else
	 		return true;
	}

	template<class K, class V> bool VectorMap<K, V>::drop(const K& key) {
		int pos = find(key);
		if (pos == -1)
			return false;

		VectorMapEntry<K, V>* entry = SortedVector<VectorMapEntry<K, V>*>::remove(pos);
		delete entry;

	 	return true;
	}

	template<class K, class V> void VectorMap<K, V>::removeAll() {
		for (int i = SortedVector<VectorMapEntry<K, V>*>::size() - 1; i >= 0; --i) {
			VectorMapEntry<K, V>* entry = SortedVector<VectorMapEntry<K, V>*>::remove(i);
			delete entry;
		}
	}

	template<class K, class V> int VectorMap<K, V>::compare(VectorMapEntry<K, V>*& e1, VectorMapEntry<K, V>*& e2) {
		if (e1->key < e2->key)
			return 1;
		else if (e1->key > e2->key)
			return -1;
		else
			return 0;
	}

  } // namespace util
} // namespace sys

using namespace sys::util;

#endif /*VECTORMAP_H_*/
