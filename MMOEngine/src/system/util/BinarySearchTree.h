/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef BINARYSEARCHTREE_H_
#define BINARYSEARCHTREE_H_

#include "system/platform.h"

#include "system/lang/System.h"

template <class O> class BinarySearchTree;

template <class O> class BinaryNode {
	O object;
	BinaryNode* leftNode;
	BinaryNode* rightNode;

public:
	BinaryNode(const O& obj, BinaryNode* left, BinaryNode* right) {
		object = obj;

		leftNode = left;
		rightNode = right;
	}

	friend class BinarySearchTree<O>;

};

template <class O> class BinarySearchTree {
private:
	BinaryNode<O>* root;
	int count;

	O nullValue;

public:
	BinarySearchTree(const O& nullval) {
		root = nullptr;
		count = 0;

		nullValue = nullval;
	}

	BinarySearchTree(const BinarySearchTree& tree) {
		root = nullptr;
		count = 0;

		nullValue = tree.nullValue;

		*this = tree;
	}

	~BinarySearchTree() {
		removeAll();
	}

	void add(const O& obj) {
		add(obj, root);
	}

	bool contains(const O& obj) const {
		return elementAt(find(obj, root));
	}

	const O& first() const {
		return elementAt(findMin(root));
	}

	const O& last() const {
		return elementAt(findMax(root));
	}

	void remove(const O& obj) {
		remove(obj, root);
	}

	void removeAll() {
		removeAll(root);
	}

	BinarySearchTree& operator=(const BinarySearchTree& tree) {
		if (this != &tree) {
			removeAll();
			root = clone(tree.root);
		}

		return *this;
	}

	bool isEmpty() const {
		return root == nullptr;
	}

	int size() const {
		return 0;
	}

	void printTree() const {
		if (isEmpty())
			System::out << "Empty tree\n";
		else
			printTree(root);
	}

private:
	const O& elementAt(BinaryNode<O>* node) const {
		if (node == nullptr)
			return nullValue;
		else
			return node->object;
	}

	void add(const O& obj, BinaryNode<O>*& node) const {
		if (node == nullptr)
			node = new BinaryNode<O>(obj, nullptr, nullptr);
		else if (obj->compareTo(node->object) > 0)
			add(obj, node->leftNode);
		else if (obj->compareTo(node->object) < 0)
			add(obj, node->rightNode);
	}

	void remove(const O& obj, BinaryNode<O>*& node) const {
		if (node == nullptr)
			return;

		if (obj->compareTo(node->object) > 0) {
			remove(obj, node->leftNode);
		} else if (obj->compareTo(node->object) < 0) {
			remove(obj, node->rightNode);
		} else if (node->leftNode != nullptr && node->rightNode != nullptr) {
			node->object = findMin(node->rightNode)->object;
			remove(node->object, node->rightNode);
		} else {
			BinaryNode<O>* oldNode = node;
			node = (node->leftNode != nullptr) ? node->leftNode : node->rightNode;

			delete oldNode;
		}
	}

	BinaryNode<O>* findMin(BinaryNode<O>* node) const {
		if (node == nullptr)
			return nullptr;
		else if (node->leftNode == nullptr)
			return node;
		else
			return findMin(node->leftNode);
	}

	BinaryNode<O>* findMax(BinaryNode<O>* node) const {
		if (node != nullptr) {
			while (node->rightNode != nullptr)
				node = node->rightNode;
		}

		return node;
	}

	BinaryNode<O>* find(const O& obj, BinaryNode<O>* node) const {
		if (node == nullptr)
			return nullptr;
		else if (obj->compareTo(node->object) > 0)
			return find(obj, node->leftNode);
		else if (obj->compareTo(node->object) < 0)
			return find(obj, node->rightNode);
		else
			return node;
	}

	void removeAll(BinaryNode<O>*& node) const {
		if (node != nullptr) {
			removeAll(node->leftNode);
			removeAll(node->rightNode);

			delete node;
		}

		node = nullptr;
	}

	void printTree(BinaryNode<O>* node) const {
		if (node != nullptr) {
			printTree(node->leftNode);

			System::out << node->object << "\n";

			printTree(node->rightNode);
		}
	}

	BinaryNode<O>* clone(BinaryNode<O>* node) const {
		if (node == nullptr)
			return nullptr;
		else
			return new BinaryNode<O>(node->object, clone(node->leftNode), clone(node->rightNode));
	}

};

#endif /*BINARYSEARCHTREE_H_*/
