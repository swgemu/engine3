/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

namespace sys {
  namespace util {
		
	template<class O> class LinkedList;
	
	template<class O> class ListEntry {
		O obj;  
		ListEntry* next;
	
	public:	
		ListEntry() {
		}
	
		ListEntry(O obj, ListEntry* e) {
			ListEntry::obj = obj; next = e;
		}
	
		friend class LinkedList<O>;
	};
	
	template<class O> class LinkedList {
		ListEntry<O> head, *current;
		
		int count;
	
	public:	
		LinkedList();
		
		void add(O& obj);
		
		O& get(int index);
		
		O remove(int index);

		inline bool isEmpty() {
			return count == 0;
		}

		inline int size() {
			return count;
		}
		
	};

	template<class O> LinkedList<O>::LinkedList() {
		current = NULL;
		count = 0;
	}
	
	template<class O> void LinkedList<O>::add(O& obj) {
		ListEntry<O>* e = new ListEntry<O>(obj, NULL);
		
		if (count == 0) 
			head.next = e;
		else 
			current->next = e;
		
		current = e;
		++count;
	}
	
	template<class O> O& LinkedList<O>::get(int index) {
		if (count < index + 1 || index < 0) 
			throw ArrayIndexOutOfBoundsException(index);
		
		ListEntry<O>* e = &head;
		
		for (int i = 0; i < index + 1; ++i)
			e = e->next;
		
		return e->obj;
	}

	template<class O> O LinkedList<O>::remove(int index) {
		if (count < index + 1 || index < 0) 
			throw ArrayIndexOutOfBoundsException(index);
		
		ListEntry<O>* e = &head;
		
		for (int i = 0; i < index; ++i)
			e = e->next;
		
		ListEntry<O>* o = e->next;
		
		if (o != NULL) 
			e->next = o->next;
		else 
			e->next = NULL;
		
		O obj = o->obj;
		delete o;
		
		--count;
		return obj;
	}

  } // namespace util
} // namespace sys

using namespace sys::util;
#endif /*LINKEDLIST_H_*/
