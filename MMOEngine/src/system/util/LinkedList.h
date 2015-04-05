/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

namespace sys {
  namespace util {
		
	template<class O> class LinkedList;
	template<class O> class LinkedListIterator;
	
	template<class O> class ListEntry {
		O obj;  
		AtomicReference<ListEntry*> next;
	
	public:	
		ListEntry() {
		}
	
		ListEntry(O obj, ListEntry* e) {
			ListEntry::obj = obj; next = e;
		}
	
		friend class LinkedList<O>;
		friend class LinkedListIterator<O>;
	};
	
	template<class O> class LinkedList {
		AtomicReference<ListEntry<O>*> head, current;
		
		AtomicInteger count;
	
	public:	
		LinkedList();
		virtual ~LinkedList();
		
		virtual void add(O& obj);
		virtual void add(int index, O& obj);
		
		O& get(int index);
		
		O remove(int index);

		LinkedListIterator<O> getIterator() {
			return LinkedListIterator<O>(this);
		}

		inline bool isEmpty() {
			return count == 0;
		}

		inline int size() {
			return count;
		}
		
		friend class LinkedListIterator<O>;

	};

	template<class O> class LinkedListIterator {
		LinkedList<O>* linkedList;
		ListEntry<O>* current;

	public:
		LinkedListIterator(LinkedList<O>* list) {
			linkedList = list;
			current = linkedList->head;
		}

		O next() {
			O cur = current;

			if (current != NULL)
				current = current->next;

			return cur;
		}

	};

	template<class O> LinkedList<O>::LinkedList() {
		current = NULL;
		count = 0;

		head = new ListEntry<O>();
	}
	
	template<class O> LinkedList<O>::~LinkedList() {
		ListEntry<O>* currentObject = head;

		while (currentObject->next != NULL) {
			ListEntry<O>* old = currentObject;
			currentObject = currentObject->next;

			delete old;
		}

		delete currentObject;
	}

	template<class O> void LinkedList<O>::add(O& obj) {
		ListEntry<O>* e = new ListEntry<O>(obj, NULL);
		
		if (count == 0)
			head->next = e;
		else
			current->next = e;

		current = e;

		count.increment();
	}
	
	template<class O> void LinkedList<O>::add(int index, O& obj) {
		if ((int) count < index + 1 || index < 0)
			throw ArrayIndexOutOfBoundsException(index);

		ListEntry<O>* newEntry = new ListEntry<O>(obj, NULL);

		//if (count =)

		ListEntry<O>* e = head;

		for (int i = 0; i < index; ++i)
			e = e->next;

		ListEntry<O>* o = e->next; //obj at index

		if (o != NULL) {
			newEntry->next = o;
		} else {
			current = newEntry;
		}

		e->next = newEntry;

		/*O obj = o->obj;
		delete o;*/

		count.increment();
	}

	template<class O> O& LinkedList<O>::get(int index) {
		if (count < index + 1 || index < 0) 
			throw ArrayIndexOutOfBoundsException(index);
		
		ListEntry<O>* e = head;
		
		for (int i = 0; i < index + 1; ++i)
			e = e->next;
		
		return e->obj;
	}

	template<class O> O LinkedList<O>::remove(int index) {
		if ((int) count < index + 1 || index < 0)
			throw ArrayIndexOutOfBoundsException(index);
		
		ListEntry<O>* e = head;
		
		for (int i = 0; i < index; ++i)
			e = e->next;
		
		ListEntry<O>* o = e->next;
		
		if (o != NULL) {
			e->next = o->next;

			O obj = o->obj;
			delete o;

			count.decrement();

			return obj;
		}
		
		return NULL;
	}

  } // namespace util
} // namespace sys

using namespace sys::util;
#endif /*LINKEDLIST_H_*/
