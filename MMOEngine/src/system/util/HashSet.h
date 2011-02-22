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

	template<class E> class HashSet : public HashTable<E, Object*> {
		Object* PRESENT;
		
	public:
		HashSet() : HashTable<E, Object*>() {
			PRESENT = new Object();
		}

		HashSet(int initcap) : HashTable<E, Object*>(initcap) {
			PRESENT = new Object();
		}
		
		virtual ~HashSet() {
			delete PRESENT;
		}
		
		void add(const E& obj) {
			put(obj, PRESENT);
		}
		
		bool contains(const E& obj) {
			return containsKey(obj);
		}
		
	};

	/*template<class E> class HashSetIterator : public class HashTableIterator<E, Object*> {
		HashSetIterator(HashSet<E, Object*>* set)  :  HashTableIterator<E, Object*>(set) {
		}
	};*/

  } // namespace util
} // namespace sys

using namespace sys::util;

#endif /*HASHSET_H_*/
