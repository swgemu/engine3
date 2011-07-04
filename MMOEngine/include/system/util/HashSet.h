/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef HASHSET_H_
#define HASHSET_H_

#include "system/lang/Object.h"

#include "HashTable.h"

namespace sys {
  namespace util {

    template<class E> class HashSetIterator;

	template<class E> class HashSet : public HashTable<E, uint8> {
		uint8 PRESENT;
		
	public:
		HashSet() : HashTable<E, uint8>() {
			PRESENT = 0;
		}

		HashSet(int initcap) : HashTable<E, uint8>(initcap) {
			PRESENT = 0;//new Object();
		}
		
		virtual ~HashSet() {
			//delete PRESENT;
		}
		
		void add(const E& obj) {
			put(obj, PRESENT);
		}
		
		bool contains(const E& obj) {
			return containsKey(obj);
		}
		
		HashSetIterator<E> iterator() {
			return HashSetIterator<E>(this);
		}

	};

	template<class E> class HashSetIterator : public HashTableIterator<E, uint8> {
	public:
		HashSetIterator(const HashSetIterator& i)  :  HashTableIterator<E, uint8>(i) {
		}

		HashSetIterator(HashSet<E>* set)  :  HashTableIterator<E, uint8>(set) {
		}
	};

  } // namespace util
} // namespace sys

using namespace sys::util;

#endif /*HASHSET_H_*/
