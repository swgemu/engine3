/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef VECTORMAP_H_
#define VECTORMAP_H_

#ifdef CXX11_COMPILER
#include <type_traits>
#endif

#include "system/platform.h"

#include "SortedVector.h"

namespace sys {
  namespace util {

	template<class K, class V>
	class VectorMapEntry {
	public:
		K key;
		V value;

	public:
		VectorMapEntry() : key{}, value{} {
		}

		VectorMapEntry(const K& key) : key(key) {
		}

		VectorMapEntry(const K& key, const V& value) : key(key), value(value) {
		}

		VectorMapEntry(K&& key, V&& value) : key(std::move(key)), value(std::move(value)) {
		}

		VectorMapEntry(const VectorMapEntry& entry) : key(entry.key), value(entry.value) {
		}

		VectorMapEntry(VectorMapEntry&& entry) : key(std::move(entry.key)), value(std::move(entry.value)) {
		}

		VectorMapEntry& operator=(const VectorMapEntry& entry) {
			if (this == &entry)
				return *this;

			key = entry.key;
			value = entry.value;

			return *this;
		}

		VectorMapEntry& operator=(VectorMapEntry&& entry) {
			if (this == &entry)
				return *this;

			key = std::move(entry.key);
			value = std::move(entry.value);

			return *this;
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

		const K& getKey() const {
			return key;
		}

		const V& getValue() const {
			return value;
		}

		bool toBinaryStream(ObjectOutputStream* stream) {
			SerializationHelpers::toBinary<K>(&this->key, stream);
			SerializationHelpers::toBinary<V>(&this->value, stream);

			return true;
		}

		bool parseFromBinaryStream(ObjectInputStream* stream) {
			if (SerializationHelpers::fromBinary<K>(&this->key, stream) &&
				SerializationHelpers::fromBinary<V>(&this->value, stream)) {

				return true;
			} else {
				return false;
			}
		}

	};

	template<class K, class V, bool RawCopyAndRealloc = ArrayListReallocTrait<V>::value>
	class VectorMap : public SortedVector<VectorMapEntry<K, V>, RawCopyAndRealloc> {
		V nullValue;

	public:
		VectorMap();
		VectorMap(int initsize, int incr);
		VectorMap(const VectorMap<K, V, RawCopyAndRealloc>& vector);

#ifdef CXX11_COMPILER
		VectorMap(VectorMap<K, V, RawCopyAndRealloc>&& vector);
#endif

		VectorMap<K, V, RawCopyAndRealloc>& operator=(const VectorMap<K, V, RawCopyAndRealloc>& vector);

#ifdef CXX11_COMPILER
		VectorMap<K, V, RawCopyAndRealloc>& operator=(VectorMap<K, V, RawCopyAndRealloc>&& vector);
#endif

		virtual ~VectorMap();

		int put(const K& key, const V& value);

#ifdef CXX11_COMPILER
		int put(K&& key, V&& value);
#endif

		V& get(int index);
		V& get(const K& key);

		const V& get(int index) const;
		const V& get(const K& key) const;

		int find(const K& key) const;

		Object* clone();

		bool contains(const K& key) const;

		bool drop(const K& key);

		inline void setNullValue(const V& val) {
			nullValue = val;
		}

	protected:
		virtual int compare(const VectorMapEntry<K, V>& e1, const VectorMapEntry<K, V>& e2) const;

	};

	template<class K, class V, bool RawCopyAndRealloc> VectorMap<K, V, RawCopyAndRealloc>::VectorMap()
			: SortedVector<VectorMapEntry<K, V>, RawCopyAndRealloc>() {
		setNullValue(TypeInfo<V>::nullValue());

		SortedVector<VectorMapEntry<K, V>, RawCopyAndRealloc>::setInsertPlan(SortedVector<VectorMapEntry<K, V>, RawCopyAndRealloc>::ALLOW_OVERWRITE);
	}

	template<class K, class V, bool RawCopyAndRealloc> VectorMap<K, V, RawCopyAndRealloc>::VectorMap(int initsize, int incr)
			: SortedVector<VectorMapEntry<K, V>, RawCopyAndRealloc>(initsize, incr) {
		setNullValue(TypeInfo<V>::nullValue());

		SortedVector<VectorMapEntry<K, V>, RawCopyAndRealloc>::setInsertPlan(SortedVector<VectorMapEntry<K, V>, RawCopyAndRealloc>::ALLOW_OVERWRITE);
	}

	template<class K, class V, bool RawCopyAndRealloc> VectorMap<K, V, RawCopyAndRealloc>::VectorMap(const VectorMap<K, V, RawCopyAndRealloc>& vector)
			: SortedVector<VectorMapEntry<K, V>, RawCopyAndRealloc>(vector) {
		nullValue = vector.nullValue;
	}

#ifdef CXX11_COMPILER
	template<class K, class V, bool RawCopyAndRealloc> VectorMap<K, V, RawCopyAndRealloc>::VectorMap(VectorMap<K, V, RawCopyAndRealloc>&& vector)
						: SortedVector<VectorMapEntry<K, V>, RawCopyAndRealloc>(std::move(vector)) {
		nullValue = vector.nullValue;
	}
#endif

	template<class K, class V, bool RawCopyAndRealloc> VectorMap<K, V, RawCopyAndRealloc>& VectorMap<K, V, RawCopyAndRealloc>::operator=(const VectorMap<K, V, RawCopyAndRealloc>& vector) {
		if (this == &vector)
			return *this;

		nullValue = vector.nullValue;

		SortedVector<VectorMapEntry<K, V>, RawCopyAndRealloc>::operator=(vector);

		return *this;
	}

#ifdef CXX11_COMPILER
	template<class K, class V, bool RawCopyAndRealloc> VectorMap<K, V, RawCopyAndRealloc>& VectorMap<K, V, RawCopyAndRealloc>::operator=(VectorMap<K, V, RawCopyAndRealloc>&& vector) {
		if (this == &vector)
			return *this;

		nullValue = vector.nullValue;

		SortedVector<VectorMapEntry<K, V>, RawCopyAndRealloc>::operator=(std::move(vector));

		return *this;
	}
#endif

	template<class K, class V, bool RawCopyAndRealloc> VectorMap<K, V, RawCopyAndRealloc>::~VectorMap() {

	}

	template<class K, class V, bool RawCopyAndRealloc> Object* VectorMap<K, V, RawCopyAndRealloc>::clone() {
		return new VectorMap<K, V, RawCopyAndRealloc>(*this);
	}

	template<class K, class V, bool RawCopyAndRealloc> int VectorMap<K, V, RawCopyAndRealloc>::put(const K& key, const V& value) {
		int res = SortedVector<VectorMapEntry<K, V>, RawCopyAndRealloc>::put(VectorMapEntry<K, V>(key, value));

	 	return res;
	}
#ifdef CXX11_COMPILER
	template<class K, class V, bool RawCopyAndRealloc> int VectorMap<K, V, RawCopyAndRealloc>::put(K&& key, V&& value) {
		VectorMapEntry<K, V> e(std::move(key), std::move(value));

		int res = SortedVector<VectorMapEntry<K, V>, RawCopyAndRealloc>::put(std::move(e));

		return res;
	}
#endif

	template<class K, class V, bool RawCopyAndRealloc> V& VectorMap<K, V, RawCopyAndRealloc>::get(int index) {
		VectorMapEntry<K, V>* entry = &SortedVector<VectorMapEntry<K, V>, RawCopyAndRealloc>::get(index);
	 	return entry->value;
	}

	template<class K, class V, bool RawCopyAndRealloc> const V& VectorMap<K, V, RawCopyAndRealloc>::get(int index) const {
		VectorMapEntry<K, V>* entry = &SortedVector<VectorMapEntry<K, V>, RawCopyAndRealloc>::get(index);
		return entry->value;
	}

	template<class K, class V, bool RawCopyAndRealloc> V& VectorMap<K, V, RawCopyAndRealloc>::get(const K& key) {
	 	int pos = find(key);

		if (pos == this->npos)
	 		return nullValue;

	 	VectorMapEntry<K, V>* entry = &SortedVector<VectorMapEntry<K, V>, RawCopyAndRealloc>::getUnsafe(pos);
	 	return entry->value;
	}

	template<class K, class V, bool RawCopyAndRealloc> const V& VectorMap<K, V, RawCopyAndRealloc>::get(const K& key) const {
		int pos = find(key);

		if (pos == this->npos)
			return nullValue;

		VectorMapEntry<K, V>* entry = &SortedVector<VectorMapEntry<K, V>, RawCopyAndRealloc>::getUnsafe(pos);
		return entry->value;
	}

	template<class K, class V, bool RawCopyAndRealloc> int VectorMap<K, V, RawCopyAndRealloc>::find(const K& key) const {
		if (ArrayList<VectorMapEntry<K, V>, RawCopyAndRealloc>::elementCount == 0)
			return this->npos;

		int l = 0, r = ArrayList<VectorMapEntry<K, V>, RawCopyAndRealloc>::elementCount - 1;
		int m = 0, cmp = 0;

		while (l <= r) {
			m = ((uint32)l + (uint32)r) >> 1;

			const VectorMapEntry<K, V>& obj = ArrayList<VectorMapEntry<K, V>, RawCopyAndRealloc>::elementData[m];
			cmp = TypeInfo<K>::compare(obj.getKey(), key);

			if (cmp == 0)
				return m;
			else if (cmp > 0)
				l = m + 1;
			else
				r = m - 1;
		}

		return this->npos;
	}

	template<class K, class V, bool RawCopyAndRealloc> bool VectorMap<K, V, RawCopyAndRealloc>::contains(const K& key) const {
	 	int idx = find(key);

	 	return idx != this->npos;
	}

	template<class K, class V, bool RawCopyAndRealloc> bool VectorMap<K, V, RawCopyAndRealloc>::drop(const K& key) {
		int pos = find(key);

		if (pos == this->npos)
			return false;

		SortedVector<VectorMapEntry<K, V>, RawCopyAndRealloc>::remove(pos);

	 	return true;
	}

	template<class K, class V, bool RawCopyAndRealloc> int VectorMap<K, V, RawCopyAndRealloc>::compare(const VectorMapEntry<K, V>& e1, const VectorMapEntry<K, V>& e2) const {
		return TypeInfo<K>::compare(e1.key, e2.key);
	}

  } // namespace util
} // namespace sys

using namespace sys::util;

#endif /*VECTORMAP_H_*/
