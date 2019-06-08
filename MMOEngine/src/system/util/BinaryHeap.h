/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef BINARYHEAP_H_
#define BINARYHEAP_H_

#include "system/platform.h"

namespace sys {
  namespace util {

	template <class O> class BinaryHeap;
	
	template <class O> class BinaryHeapNode {
		O object;
	
		BinaryHeapNode* leftNode;
		BinaryHeapNode* rightNode;
	
		int	npl;
	
	public:
		BinaryHeapNode(const O& obj, BinaryHeapNode* lnode = nullptr, BinaryHeapNode* rnode = nullptr, int np = 0) {
			object = obj;
	
			leftNode = lnode;
			rightNode = rnode;
			
			npl = np; 
		}
	
		friend class BinaryHeap<O>;
	
	};
	
	template <class O> class BinaryHeap {
	private:
		BinaryHeapNode<O>* root;
	
		int count;
	
	public:
		BinaryHeap() {
			root = nullptr;
			count = 0;
		}
		
		BinaryHeap(const BinaryHeap& heap) {
			root = nullptr;
			*this = heap;
		}
		
		~BinaryHeap() {
			removeAll();
		}
	
		void add(const O& obj) {
			root = merge(new BinaryHeapNode<O>(obj), root);
			
			++count;
		}
		
		const O& getMin() const {
			if (isEmpty())
				throw Exception("object not found in BinaryHeap");
				
			return root->object;
		}
		
		const O removeMin() {
			if (isEmpty())
				throw Exception("object not found in BinaryHeap");
	
			O minItem = getMin();
	
			BinaryHeapNode<O>* oldRoot = root;
			root = merge(root->leftNode, root->rightNode);
	
			delete oldRoot;
			--count;
	
			return minItem;
		}
		
		void removeAll() {
			reclaimMemory(root);
	
			root = nullptr;
			count = 0;
		}
	
		void merge(BinaryHeap& heap) {
			if (this == heap)
				return;
	    
			root = merge(root, heap.root);
			heap.root = nullptr;
		}
	
		BinaryHeap& operator=(const BinaryHeap& heap) {
			if (this != &heap) {
				removeAll();
				root = clone(heap.root);
			}
			
			return *this;
		}
	
		bool isEmpty() const {
			return root == nullptr;
		}
		
		bool isFull() const {
			return false;
		}
	
		int size() {
			return count;
		}
	
	private:
		BinaryHeapNode<O>* merge(BinaryHeapNode<O>* h1, BinaryHeapNode<O>* h2 ) const {
			if (h1 == nullptr)
				return h2;
				
			if (h2 == nullptr)
				return h1;
			
			if (h1->object->compareTo(h2->object) > 0)
				return merge1(h1, h2);
			else
				return merge1(h2, h1);
		}
	                                             
		BinaryHeapNode<O>* merge1(BinaryHeapNode<O>* h1, BinaryHeapNode<O>* h2) const {
			if (h1->leftNode == nullptr) {
				h1->leftNode = h2;
			} else {
				h1->rightNode = merge(h1->rightNode, h2);
	
				if (h1->leftNode->npl < h1->rightNode->npl)
					swapChildren(h1);
					
				h1->npl = h1->rightNode->npl + 1;
			}
			
			return h1;
		}
		
		void swapChildren(BinaryHeapNode<O>* node) const {
			BinaryHeapNode<O>* tmp = node->leftNode;
			
			node->leftNode = node->rightNode;
			node->rightNode = tmp;
		}
		
		void reclaimMemory(BinaryHeapNode<O>* node) const {
			if (node != nullptr) {
				reclaimMemory(node->leftNode);
				reclaimMemory(node->rightNode);
				
				delete node;
			}
		}
		
		BinaryHeapNode<O>* clone(BinaryHeapNode<O>* node) const {
			if (node == nullptr)
				return nullptr;
			else
				return new BinaryHeapNode<O>(node->object, clone(node->leftNode), 
					clone(node->rightNode), node->npl);
		}
	
	};

  } // namespace util
} // namespace sys

using namespace sys::util;

#endif /*BINARYHEAP_H_*/
