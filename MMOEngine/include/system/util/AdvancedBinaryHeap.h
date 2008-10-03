/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ADVANCEDBINARYHEAP_H_
#define ADVANCEDBINARYHEAP_H_

#include "../platform.h"

namespace sys {
  namespace util {
		
	class AdvancedBinaryHeap;
	
	class AdvancedBinaryHeapNode {
		AdvancedBinaryHeapNode* parentNode;
	
		AdvancedBinaryHeapNode* leftNode;
		AdvancedBinaryHeapNode* rightNode;
	
		int	npl;
	
	public:
		AdvancedBinaryHeapNode(AdvancedBinaryHeapNode* parent = NULL, AdvancedBinaryHeapNode* lnode = NULL, 
				AdvancedBinaryHeapNode* rnode = NULL, int np = 0);
				
		virtual ~AdvancedBinaryHeapNode() {
		}
	
		virtual int compareTo(AdvancedBinaryHeapNode* node) = 0;
	
		friend class AdvancedBinaryHeap;
	
	};
	
	class AdvancedBinaryHeap {
	private:
		AdvancedBinaryHeapNode* root;
	
		int count;
	
	public:
		AdvancedBinaryHeap();
		AdvancedBinaryHeap(const AdvancedBinaryHeap& heap);
		
		virtual ~AdvancedBinaryHeap();
	
		void add(AdvancedBinaryHeapNode* node);
		
		const AdvancedBinaryHeapNode* getMin() const;
	
		const AdvancedBinaryHeapNode* remove(AdvancedBinaryHeapNode* node);
		const AdvancedBinaryHeapNode* removeMin();
		
		void removeAll();
	
		void merge(AdvancedBinaryHeap& heap);
	
		inline bool isEmpty() const {
			return root == NULL;
		}
		
		inline bool isFull() const {
			return false;
		}
	
		inline int size() {
			return count;
		}
	
	private:
		AdvancedBinaryHeapNode* merge(AdvancedBinaryHeapNode* h1, AdvancedBinaryHeapNode* h2) const;
		AdvancedBinaryHeapNode* merge1(AdvancedBinaryHeapNode* h1, AdvancedBinaryHeapNode* h2) const;
		
		void swapChildren(AdvancedBinaryHeapNode* node) const;
		
		void reclaimMemory(AdvancedBinaryHeapNode* node) const;
	};

  } // namespace util
} // namespace sys;

using namespace sys::util;

#endif /*ADVANCEDBINARYHEAP_H_*/
