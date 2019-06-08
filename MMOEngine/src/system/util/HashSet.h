/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef HASHSET_H_
#define HASHSET_H_

#include "system/lang/Object.h"

#include "HashTable.h"

namespace sys {
  namespace util {

    template<class E> class HashSetIterator;

	template<class E> class HashSet : public HashTable<E, uint8> {
	public:
		HashSet() : HashTable<E, uint8>() {
		}

		HashSet(int initcap) : HashTable<E, uint8>(initcap) {
		}

		HashSet(const HashSet<E>& h) : HashTable<E, uint8>(h) {
		}

		HashSet(HashSet<E>&& h) : HashTable<E, uint8>(std::move(h)) {
		}

		HashSet<E>& operator=(HashSet<E>&& h) {
			HashTable<E, uint8>::operator=(std::move(h));

			return *this;
		}

		HashSet<E>& operator=(const HashSet<E>& h) {
			HashTable<E, uint8>::operator=(h);

			return *this;
		}

		void add(const E& obj) {
			HashTable<E, uint8>::put(obj, 0);
		}

		bool contains(const E& obj) const {
			return HashTable<E, uint8>::containsKey(obj);
		}

		HashSetIterator<E> iterator() const {
			return HashSetIterator<E>(this);
		}

	};

	template<class E> class HashSetIterator : public HashTableIterator<E, uint8> {
	public:
		HashSetIterator(const HashSetIterator& i)  :  HashTableIterator<E, uint8>(i) {
		}

		HashSetIterator(const HashSet<E>* set)  :  HashTableIterator<E, uint8>(set) {
		}

		E& next() {
			return HashTableIterator<E, uint8>::getNextKey();
		}
	};

  } // namespace util
} // namespace sys

using namespace sys::util;

#endif /*HASHSET_H_*/
