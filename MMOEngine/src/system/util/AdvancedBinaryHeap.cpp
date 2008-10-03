/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "../lang/Exception.h"

#include "AdvancedBinaryHeap.h"

AdvancedBinaryHeapNode::AdvancedBinaryHeapNode(AdvancedBinaryHeapNode* parent, 
		AdvancedBinaryHeapNode* lnode, AdvancedBinaryHeapNode* rnode, int np) {
	parentNode = parent;
	leftNode = lnode;
	rightNode = rnode;
		
	npl = np; 
}

AdvancedBinaryHeap::AdvancedBinaryHeap() {
	root = NULL;
	count = 0;
}
	
AdvancedBinaryHeap::AdvancedBinaryHeap(const AdvancedBinaryHeap& heap) {
	root = NULL;
	*this = heap;
}
	
AdvancedBinaryHeap::~AdvancedBinaryHeap() {
	removeAll();
}

void AdvancedBinaryHeap::add(AdvancedBinaryHeapNode* node) {
	root = merge(node, root);
		
	++count;
}
	
const AdvancedBinaryHeapNode* AdvancedBinaryHeap::getMin() const {
	if (isEmpty())
		throw Exception("object not found in AdvancedBinaryHeap");
			
	return root;
}

const AdvancedBinaryHeapNode* AdvancedBinaryHeap::remove(AdvancedBinaryHeapNode* node) {
	if (isEmpty())
		throw Exception("object not found in AdvancedBinaryHeap");

	AdvancedBinaryHeapNode* oldNode = node;
	AdvancedBinaryHeapNode* parent = oldNode->parentNode;

	node = merge(node->leftNode, node->rightNode);

	if (parent != NULL) {
		if (parent->leftNode == oldNode)
			parent->leftNode = node;
		else
			parent->rightNode = node;
			
		if (node != NULL)
			node->parentNode = parent;
	} else {
		root = node;

		if (node != NULL)
			node->parentNode = NULL;
	}

	oldNode->parentNode = NULL;
	oldNode->leftNode = NULL;
	oldNode->rightNode = NULL;

	--count;
	return oldNode;
}
	
const AdvancedBinaryHeapNode* AdvancedBinaryHeap::removeMin() {
	if (isEmpty())
		throw Exception("object not found in AdvancedBinaryHeap");

	AdvancedBinaryHeapNode* oldRoot = root;
	root = merge(root->leftNode, root->rightNode);

	if (root != NULL)
		root->parentNode = NULL;

	oldRoot->parentNode = NULL;
	oldRoot->leftNode = NULL;
	oldRoot->rightNode = NULL;

	--count;
	return oldRoot;
}
	
void AdvancedBinaryHeap::removeAll() {
	reclaimMemory(root);

	root = NULL;
	count = 0;
}

void AdvancedBinaryHeap::merge(AdvancedBinaryHeap& heap) {
	if (this == &heap)
		return;
    
	root = merge(root, heap.root);
	heap.root = NULL;
}

AdvancedBinaryHeapNode* AdvancedBinaryHeap::merge(AdvancedBinaryHeapNode* h1, AdvancedBinaryHeapNode* h2) const {
	if (h1 == NULL)
		return h2;
			
	if (h2 == NULL)
		return h1;
		
	if (h1 == h2) {
		int i = 2 * 2;
	}
		
	if (h1->compareTo(h2) > 0)
		return merge1(h1, h2);
	else
		return merge1(h2, h1);
}
                                             
AdvancedBinaryHeapNode* AdvancedBinaryHeap::merge1(AdvancedBinaryHeapNode* h1, AdvancedBinaryHeapNode* h2) const {
	if (h1->leftNode == NULL) {
		h1->leftNode = h2;
		h2->parentNode = h1;
	} else {
		h1->rightNode = merge(h1->rightNode, h2);
		h1->rightNode->parentNode = h1;

		if (h1->leftNode->npl < h1->rightNode->npl)
			swapChildren(h1);
				
		h1->npl = h1->rightNode->npl + 1;
	}
		
	return h1;
}
	
void AdvancedBinaryHeap::swapChildren(AdvancedBinaryHeapNode* node) const {
	AdvancedBinaryHeapNode* tmp = node->leftNode;
		
	node->leftNode = node->rightNode;
	node->rightNode = tmp;
}
	
void AdvancedBinaryHeap::reclaimMemory(AdvancedBinaryHeapNode* node) const {
	if (node != NULL) {
		reclaimMemory(node->leftNode);
		reclaimMemory(node->rightNode);
			
		delete node;
	}
}
