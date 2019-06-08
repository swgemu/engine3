/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include "system/lang/ArrayIndexOutOfBoundsException.h"
#include <atomic>

namespace sys {
  namespace util {

	template<class O> class LinkedList;
	template<class O> class LinkedListIterator;

	template<class O> class ListEntry {
		O obj;
		ListEntry* next;

	public:
		ListEntry() : next(nullptr) {
		}

		ListEntry(O obj, ListEntry* e) {
			ListEntry::obj = obj; next = e;
		}

		friend class LinkedList<O>;
		friend class LinkedListIterator<O>;
	};

	template<class O> class LinkedList {
		ListEntry<O>* head;
		ListEntry<O>* current;

		std::atomic<int> count;

	public:
		LinkedList();
		virtual ~LinkedList();

		virtual void add(O& obj);
		virtual void add(int index, O& obj);

		O& get(int index) const;

		O remove(int index);

		LinkedListIterator<O> getIterator() {
			return LinkedListIterator<O>(this);
		}

		inline bool isEmpty() const {
			return count.load(std::memory_order_relaxed) == 0;
		}

		inline int size() const {
			return count.load(std::memory_order_relaxed);
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

			if (current != nullptr)
				current = current->next;

			return cur;
		}

	};

	template<class O> LinkedList<O>::LinkedList() {
		current = nullptr;
		count = 0;

		head = new ListEntry<O>();
	}

	template<class O> LinkedList<O>::~LinkedList() {
		ListEntry<O>* currentObject = head;

		while (currentObject->next != nullptr) {
			ListEntry<O>* old = currentObject;
			currentObject = currentObject->next;

			delete old;
		}

		delete currentObject;
	}

	template<class O> void LinkedList<O>::add(O& obj) {
		ListEntry<O>* e = new ListEntry<O>(obj, nullptr);

		if (count.load(std::memory_order_relaxed) == 0)
			head->next = e;
		else
			current->next = e;

		current = e;

		count++;
	}

	template<class O> void LinkedList<O>::add(int index, O& obj) {
		if ((int) count.load(std::memory_order_relaxed) < index + 1 || index < 0)
			throw ArrayIndexOutOfBoundsException(index);

		ListEntry<O>* newEntry = new ListEntry<O>(obj, nullptr);

		//if (count =)

		ListEntry<O>* e = head;

		for (int i = 0; i < index; ++i)
			e = e->next;

		ListEntry<O>* o = e->next; //obj at index

		if (o != nullptr) {
			newEntry->next = o;
		} else {
			current = newEntry;
		}

		e->next = newEntry;

		/*O obj = o->obj;
		delete o;*/

		count++;
	}

	template<class O> O& LinkedList<O>::get(int index) const {
		if (count.load(std::memory_order_relaxed) < index + 1 || index < 0)
			throw ArrayIndexOutOfBoundsException(index);

		const ListEntry<O>* e = head;

		for (int i = 0; i < index + 1; ++i)
			e = e->next;

		return e->obj;
	}

	template<class O> O LinkedList<O>::remove(int index) {
		if ((int) count.load(std::memory_order_relaxed) < index + 1 || index < 0)
			throw ArrayIndexOutOfBoundsException(index);

		ListEntry<O>* e = head;

		for (int i = 0; i < index; ++i)
			e = e->next;

		ListEntry<O>* o = e->next;

		if (o != nullptr) {
			e->next = o->next;

			O obj = o->obj;
			delete o;

			--count;

			return obj;
		}

		return nullptr;
	}

  } // namespace util
} // namespace sys

using namespace sys::util;
#endif /*LINKEDLIST_H_*/
