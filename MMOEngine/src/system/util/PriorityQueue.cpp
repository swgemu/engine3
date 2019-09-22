/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "PriorityQueue.h"

PriorityQueueEntry::PriorityQueueEntry(PriorityQueueEntry* parent,
		PriorityQueueEntry* lnode, PriorityQueueEntry* rnode, int np) {
	parentNode = parent;
	leftNode = lnode;
	rightNode = rnode;

	npl = np;

	enqueued = false;
}

PriorityQueue::PriorityQueue() {
	root = nullptr;
	count = 0;

	pushedEntries = 0;
	poppedEntries = 0;
	removedEntries = 0;
}

PriorityQueue::PriorityQueue(const PriorityQueue& heap) {
	root = nullptr;

	*this = heap;
}

PriorityQueue::~PriorityQueue() {
	clear();
}

void PriorityQueue::add(PriorityQueueEntry* node) {
	root = merge(node, root);

	node->setQueued();

	++count;

	++pushedEntries;
}

const PriorityQueueEntry* PriorityQueue::peak() const {
	return root;
}

PriorityQueueEntry* PriorityQueue::poll() {
	if (isEmpty())
		return nullptr;

	PriorityQueueEntry* oldRoot = root;
	root = merge(root->leftNode, root->rightNode);

	if (root != nullptr)
		root->parentNode = nullptr;

	oldRoot->parentNode = nullptr;
	oldRoot->leftNode = nullptr;
	oldRoot->rightNode = nullptr;

	oldRoot->setUnqueued();

	auto val = --count >= 0;
	E3_ASSERT (val);

	++poppedEntries;

	return oldRoot;
}

bool PriorityQueue::remove(PriorityQueueEntry* node) {
	if (isEmpty())
		return false;

	PriorityQueueEntry* oldNode = node;
	PriorityQueueEntry* parent = oldNode->parentNode;

	node = merge(node->leftNode, node->rightNode);

	if (parent != nullptr) {
		if (parent->leftNode == oldNode)
			parent->leftNode = node;
		else
			parent->rightNode = node;

		if (node != nullptr)
			node->parentNode = parent;
	} else {
		root = node;

		if (node != nullptr)
			node->parentNode = nullptr;
	}

	oldNode->parentNode = nullptr;
	oldNode->leftNode = nullptr;
	oldNode->rightNode = nullptr;

	oldNode->setUnqueued();
	auto val = --count >= 0;

	E3_ASSERT(val);

	++removedEntries;

	return true;
}

void PriorityQueue::clear() {
	reclaimMemory(root);

	root = nullptr;
	count = 0;
}

void PriorityQueue::clearWithoutTraverse() {
	root = nullptr;
	count = 0;
}

void PriorityQueue::merge(PriorityQueue& heap) {
	if (this == &heap)
		return;

	root = merge(root, heap.root);

	count += heap.count;

	heap.count = 0;
	heap.root = nullptr;
}

PriorityQueueEntry* PriorityQueue::merge(PriorityQueueEntry* h1, PriorityQueueEntry* h2) const {
	if (h1 == nullptr)
		return h2;

	if (h2 == nullptr)
		return h1;

	E3_ASSERT(h1 != h2);

	if (h1->compareTo(h2) > 0)
		return merge1(h1, h2);
	else
		return merge1(h2, h1);
}

PriorityQueueEntry* PriorityQueue::merge1(PriorityQueueEntry* h1, PriorityQueueEntry* h2) const {
	if (h1->leftNode == nullptr) {
		h1->leftNode = h2;
		h2->parentNode = h1;
	} else {
		E3_ASSERT(h1->rightNode != h2);

		h1->rightNode = merge(h1->rightNode, h2);
		h1->rightNode->parentNode = h1;

		if (h1->leftNode->npl < h1->rightNode->npl)
			swapChildren(h1);

		h1->npl = h1->rightNode->npl + 1;
	}

	return h1;
}

void PriorityQueue::swapChildren(PriorityQueueEntry* node) const {
	PriorityQueueEntry* tmp = node->leftNode;

	node->leftNode = node->rightNode;
	node->rightNode = tmp;
}

void PriorityQueue::reclaimMemory(PriorityQueueEntry* node) const {
	if (node != nullptr) {
		reclaimMemory(node->leftNode);
		reclaimMemory(node->rightNode);

		//delete node;
		node->clear();
	}
}
