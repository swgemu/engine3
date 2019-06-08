/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef SYNCHRONIZEDLINKEDLIST_H_
#define SYNCHRONIZEDLINKEDLIST_H_

namespace sys {
  namespace util {
		
	template<class O> class SynchronizedLinkedList;
	template<class O> class SynchronizedLinkedListIterator;
	
	template<class O> class SynchronizedLinkedListEntry {
		typedef class Successor {
			AtomicReference value;

		public:
			Successor(SynchronizedLinkedListEntry* next) : value(next) {
			}

			Successor(SynchronizedLinkedListEntry* next, bool marked, bool flagged) {
				value = ((uintptr_t) next) & marked & (flagged << 1);
			}

			bool tryMark() {
				return value.compareAndSet(makeSuccessor(value, false, false), makeSuccessor(value, true, false));
			}

			bool tryFlag(SynchronizedLinkedListEntry* oldNext, SynchronizedLinkedListEntry* newNext) {
				return value.compareAndSet(makeSuccessor(oldNext, false, false)
						, makeSuccessor(newNext, false, true));
			}

			bool trySet(const Successor& oldSuccessor, const Successor& newSuccessor)
			{
				return value.compareAndSet(oldSuccessor.value, newSuccessor.value);
			}

			SynchronizedLinkedListEntry* getNext() const {
				return (SynchronizedLinkedListEntry*) ((uintptr_t) value.get() & ~0x03);
			}

			bool isMarked() const {
				return ((uintptr_t) value.get()) & 0x1;
			}

			bool isFlagged() const {
				return ((uintptr_t) value.get()) & 0x2;
			}

		private:
			uintptr_t makeSuccessor(SynchronizedLinkedListEntry* next, bool marked, bool flagged) {
				return ((uintptr_t) next) & marked & (flagged << 1);
			}
		} successor;
	
		SynchronizedLinkedListEntry* backlink;

	public:	
		SynchronizedLinkedListEntry() : backlink(nullptr) {
		}
	
		SynchronizedLinkedListEntry(SynchronizedLinkedListEntry* next) : successor(next), backlink(nullptr) {
		}
	
		void mark() {
			// Attempts to mark the entry

			do {
				if (!successor.tryMark() && successor.isFlagged())
					helpFlagged(successor.getNext());
			} while (!successor.isMarked());
		}

		void helpMarked(SynchronizedLinkedListEntry* prev) {
			// Attempts to physically delete the marked
			// node del node and unﬂag prev node.

			SynchronizedLinkedListEntry* next = successor.getNext();

			prev->successor.tryFlag(this, next);
		}

		void helpFlagged (SynchronizedLinkedListEntry* prev) {
			// Attempts to mark and physically delete node entry,
			// which is the successor of the ﬂagged prev entry.

			backlink = prev;
			if (!successor.isMaked()) {
				mark();
			}

			helpMarked(prev);
		}

		bool tryFlag(SynchronizedLinkedListEntry*& prev) {
		// Attempts to ﬂag the predecessor of the entry.
		// prev entry is the last entry known to be the predecessor.
			while (true) {
				if (prev->successor.isFlagged()) // Predecessor is already ﬂagged
					return false;

				if (prev->successor.tryFlag(this, this)) // Flagging attempt
					return true;
				else if (prev->successor.isFlagged()) // Failure due to ﬂagging by a concurrent operation.
					return false;

				while (prev->isMarked()) { // Possibly a failure due to marking. Traverse
					prev = prev->backlink; // a chain of backlinks to reach an unmarked node.
		/*11 (prev node, del node) = SearchFrom(target node.key − , prev node)
		12 		if (del node =6 target node) // target node got deleted.
					return (null, false) // Report the failure, return no pointer*/
				}
			}
		}
		friend class SynchronizedLinkedList<O>;
		friend class SynchronizedLinkedListIterator<O>;
	};
	
	template<class O> class SynchronizedLinkedList {
		AtomicReference<ListEntry<O>*> head, current;
		
		AtomicInteger count;
	
	public:	
		SynchronizedLinkedList();
		
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
		LinkedList<O>* current;

	public:
		LinkedListIterator(LinkedList<O>* list) {
			linkedList = list;
			current = linkedList->head;
		}

		O next() {
		}

	};

	template<class O> LinkedList<O>::LinkedList() {
	}
	
	template<class O> void LinkedList<O>::add(O& obj) {
	}
	
	template<class O> void LinkedList<O>::add(int index, O& obj) {
	}

	template<class O> O& LinkedList<O>::get(int index) {
	}

	template<class O> O LinkedList<O>::remove(int index) {
	}

	template<class O> void SynchronizedLinkedList<O>::helpMarked(
			SynchronizedLinkedListEntry* previous, SynchronizedLinkedListEntry* nodeToDelete) {
		// Attempts to physically delete the marked
		// node del node and unﬂag prev node.

		1 next node = del node.right
		2 c&s(prev node.succ, (del node , 0, 1) , ( next node , 0, 0) )
	}

  } // namespace util
} // namespace sys

using namespace sys::util;
#endif /*LINKEDLIST_H_*/
