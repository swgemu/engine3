/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef PRIORITYQUEUE_H_
#define PRIORITYQUEUE_H_

#include "system/platform.h"

#include "system/lang/Exception.h"

namespace sys {
  namespace util {

	class PriorityQueue;

	class PriorityQueueEntry {
		PriorityQueueEntry* parentNode;

		PriorityQueueEntry* leftNode;
		PriorityQueueEntry* rightNode;

		int	npl;

		bool enqueued;

	public:
		PriorityQueueEntry(PriorityQueueEntry* parent = nullptr, PriorityQueueEntry* lnode = nullptr,
				PriorityQueueEntry* rnode = nullptr, int np = 0);

		virtual ~PriorityQueueEntry() {
			enqueued = false;
		}

		virtual int compareTo(PriorityQueueEntry* node) = 0;

		inline void clear() {
			parentNode = nullptr;

			leftNode = nullptr;
			rightNode = nullptr;

			enqueued = false;
		}

		inline void setQueued() {
			enqueued = true;
		}

		inline void setUnqueued() {
			enqueued = false;
		}

		inline bool isQueued() {
			return enqueued;
		}

		friend class PriorityQueue;
	};

	class PriorityQueue {
	private:
		PriorityQueueEntry* root;

		int count;

		uint64 pushedEntries;
		uint64 poppedEntries;
		uint64 removedEntries;

	public:
		PriorityQueue();
		PriorityQueue(const PriorityQueue& heap);

		virtual ~PriorityQueue();

		void add(PriorityQueueEntry* node);

		void merge(PriorityQueue& heap);

		const PriorityQueueEntry* peak() const;
		const PriorityQueueEntry* poll();

		bool remove(PriorityQueueEntry* node);

		void clear();
		void clearWithoutTraverse();

		inline bool isEmpty() const {
			return root == nullptr;
		}

		inline bool isFull() const {
			return false;
		}

		inline int size() {
			return count;
		}

		inline uint64 getPushedEntries() const {
			return pushedEntries;
		}

		inline uint64 getPoppedEntries() const {
			return poppedEntries;
		}

		inline uint64 getRemovedEntries() const {
			return removedEntries;
		}

	private:
		PriorityQueueEntry* merge(PriorityQueueEntry* h1, PriorityQueueEntry* h2) const;
		PriorityQueueEntry* merge1(PriorityQueueEntry* h1, PriorityQueueEntry* h2) const;

		void swapChildren(PriorityQueueEntry* node) const;

		void reclaimMemory(PriorityQueueEntry* node) const;
	};

  } // namespace util
} // namespace sys;

using namespace sys::util;

#endif /* PRIORITYQUEUE_H_ */
