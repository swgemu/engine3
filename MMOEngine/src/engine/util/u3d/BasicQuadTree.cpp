/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * LightweightQuadTree.cpp
 *
 *  Created on: 15 de ago. de 2015
 *      Author: victor
 */

#include "engine/log/Logger.h"

#include "engine/util/u3d/BasicQuadTree.h"

namespace QTNode {
	static Logger logger("BasicQuadTreeNode", Logger::WARNING);
}

using namespace QTNode;

BasicQuadTreeNode::BasicQuadTreeNode() {
	objects.setNoDuplicateInsertPlan();

	parentNode = nullptr;
	nwNode = neNode = swNode = seNode = nullptr;

	minX = 0;
	minY = 0;
	maxX = 0;
	maxY = 0;

	dividerX = 0;
	dividerY = 0;
}

BasicQuadTreeNode::BasicQuadTreeNode(float minx, float miny, float maxx, float maxy, BasicQuadTreeNode *parent) {
	objects.setNoDuplicateInsertPlan();

	parentNode = parent;
	nwNode = neNode = swNode = seNode = nullptr;

	minX = minx;
	minY = miny;
	maxX = maxx;
	maxY = maxy;

	if (!validateNode() || minX > maxX || minY > maxY) {
		logger.error() << "[BasicQuadTree] invalid node in create - " << *this;
	}

	dividerX = (minX + maxX) / 2;
	dividerY = (minY + maxY) / 2;
}

BasicQuadTreeNode::~BasicQuadTreeNode() {
	if (nwNode != nullptr)
		delete nwNode;

	if (neNode != nullptr)
		delete neNode;

	if (swNode != nullptr)
		delete swNode;

	if (seNode != nullptr)
		delete seNode;
}

void BasicQuadTreeNode::addObject(QuadTreeEntryInterface *obj) {
	if (BasicQuadTree::doLog())
		logger.info(true) << hex << "object [" << obj->getObjectID() <<  "] added to BasicQuadTree"
		<< *this;

	if (!validateNode())
		logger.error() << "[BasicQuadTree] invalid node in addObject() - " << *this;

	objects.put(obj);

	E3_ASSERT(obj->getNode() == nullptr);

	obj->setNode(this);
}

void BasicQuadTreeNode::removeObject(QuadTreeEntryInterface *obj) {
	if (!objects.drop(obj)) {
		logger.error() << hex << "object [" << obj->getObjectID() <<  "] not found on BasicQuadTree"
				<< *this;
	} else {
		obj->setNode(nullptr);

		if (BasicQuadTree::doLog())
			logger.info(true) << hex << "object [" << obj->getObjectID() <<  "] removed BasicQuadTree"
			<< *this;
	}
}

void BasicQuadTreeNode::removeObject(int index) {
	QuadTreeEntryInterface* obj = objects.remove(index);
	obj->setNode(nullptr);
}

bool BasicQuadTreeNode::testInside(QuadTreeEntryInterface* obj) const {
	float x = obj->getPositionX();
	float y = obj->getPositionY();

	return x >= minX && x < maxX && y >= minY && y < maxY;
}

bool BasicQuadTreeNode::testInRange(float x, float y, float range) const {
	bool insideX = (minX <= x) && (x < maxX);
	bool insideY = (minY <= y) && (y < maxY);

	if (insideX && insideY)
		return true;

	bool closeenoughX = (fabs(minX - x) <= range || fabs(maxX - x) <= range);
	bool closeenoughY = (fabs(minY - y) <= range || fabs(maxY - y) <= range);

	if ((insideX || closeenoughX) && (insideY || closeenoughY))
		return true;
	else
		return false;
}

void BasicQuadTreeNode::check () {
	if (isEmpty() && !hasSubNodes() && parentNode != nullptr) {
		if (parentNode->nwNode == this)
			parentNode->nwNode = nullptr;
		else if (parentNode->neNode == this)
			parentNode->neNode = nullptr;
		else if (parentNode->swNode == this)
			parentNode->swNode = nullptr;
		else if (parentNode->seNode == this)
			parentNode->seNode = nullptr;

		if (BasicQuadTree::doLog())
			logger.info(true) << "deleteing node (" << *this << ")";

		delete this;
	}
}

String BasicQuadTreeNode::toStringData() const {
	StringBuffer s;
	s << "Node " << this << " (" << (int) minX << ","
			<< (int) minY << "," << (int) maxX << "," << (int) maxY
			<< ") [" << objects.size() << "]";

	return s.toString();
}

int BasicQuadTreeNode::_getSubNodeCount(const BasicQuadTreeNode* s) {
	int count = 1;

	if (s->neNode != nullptr) {
		count += _getSubNodeCount(s->neNode);
	}

	if (s->nwNode != nullptr) {
		count += _getSubNodeCount(s->nwNode);
	}

	if (s->seNode != nullptr) {
		count += _getSubNodeCount(s->seNode);
	}

	if (s->swNode != nullptr) {
		count += _getSubNodeCount(s->swNode);
	}

	return count;
}


int BasicQuadTreeNode::getNodeCount() const {
	return _getSubNodeCount(this);
}

//---------------------------------------------------------------------------//

bool BasicQuadTree::logTree = false;

BasicQuadTree::BasicQuadTree() : root(nullptr), minSquareSize(8) {
}

BasicQuadTree::BasicQuadTree(float minx, float miny, float maxx,
		float maxy, float minSquareSize) : root(new BasicQuadTreeNode(minx, miny, maxx, maxy, nullptr)),
				minSquareSize(minSquareSize) {
}

BasicQuadTree::~BasicQuadTree() {
	delete root;

	root = nullptr;
}

void BasicQuadTree::setSize(float minx, float miny, float maxx, float maxy) {
	delete root;

	root = new BasicQuadTreeNode(minx, miny, maxx, maxy, nullptr);
}

void BasicQuadTree::insert(QuadTreeEntryInterface *obj) {
	/*if (!isLocked()) {
		System::out << "inserting to unlocked quad tree\n";
		StackTrace::printStackTrace();
		raise(SIGSEGV);
	}*/

	try {
		if (BasicQuadTree::doLog()) {
			logger.info(true) << hex << "object [" << obj->getObjectID() <<  "] inserting"
				<< "(" << obj->getPositionX() << ", " << obj->getPositionY() << ")";
		}

		if (obj->getNode() != nullptr)
			remove(obj);

		_insert(root, obj);

		if (BasicQuadTree::doLog())
			logger.info(true) << hex << "object [" << obj->getObjectID() <<  "] finished inserting";
	} catch (const Exception& e) {
		logger.error() << "[BasicQuadTree] error - " << e.getMessage();
		e.printStackTrace();
	}
}

int BasicQuadTree::inRange(float x, float y, float range,
		Vector<QuadTreeEntryInterface*>& objects) {

	try {
		return _inRange(root, x, y, range, objects);
	} catch (const Exception& e) {
		logger.error() << "[BasicQuadTree] " << e.getMessage();
		e.printStackTrace();
	}

	return 0;
}

void BasicQuadTree::remove(BasicQuadTreeNode* node) {
	if (!node->validateNode()) {
		logger.error() << "[BasicQuadTree] " << " error on remove(BasicQuadTreeNode) - invalid Node"
				<< *node;

		return;
	}

	if (node->parentNode == nullptr) {
		logger.error() << "[BasicQuadTree] " << " error on remove(BasicQuadTreeNode) - trying to remove root Node"
				<< *node;
	}

	BasicQuadTreeNode* parent = node->parentNode;

	if (parent->nwNode == node) {
		parent->nwNode = nullptr;
	}

	if (parent->neNode == node) {
		parent->neNode = nullptr;
	}

	if (parent->swNode == node) {
		parent->swNode = nullptr;
	}

	if (parent->seNode == node){
		parent->seNode = nullptr;
	}
}

int BasicQuadTree::getNodeCount() {
	return root->getNodeCount();
}

void BasicQuadTree::remove(QuadTreeEntryInterface *obj) {
	/*if (!isLocked()) {
		System::out << "remove on unlocked quad tree\n";
		StackTrace::printStackTrace();
		raise(SIGSEGV);
	}*/

	if (BasicQuadTree::doLog())
		logger.info(true) << hex << "object [" << obj->getObjectID() <<  "] removing";

	BasicQuadTreeNode* node = obj->getNode();

	if (node != nullptr) {
		if (!node->validateNode()) {
			logger.error() << "[BasicQuadTree] " << obj->getObjectID() << " error on remove() - invalid Node"
					<< *node;
		}

		node->removeObject(obj);

		node->check();
		obj->setNode(nullptr);
	} else {
		logger.error() << hex << "object [" << obj->getObjectID() <<  "] ERROR - removing the node";
		StackTrace::printStackTrace();
	}

	if (BasicQuadTree::doLog())
		logger.info(true) << hex << "object [" << obj->getObjectID() <<  "] finished removing";
}

void BasicQuadTree::removeAll() {
	if (root != nullptr) {
		root = nullptr;
		delete root;
	}
}

/*
 * Every Node can have data and children. Every data must be completely
 * contained inside the Node, so boundary sphere is checked.
 */
void BasicQuadTree::_insert(BasicQuadTreeNode* node, QuadTreeEntryInterface *obj) {
	/*
	 * Logic:
	 *
	 * 1) We have Objects in this Node. The Node has no children.
	 *    - Squaring was not yet needed, so we do it now and move
	 *      all the objects down into the squares that arent locked.
	 *
	 * 2) We have Objects in this Node. The Node has children.
	 *    - All those objects must be locked, else they would have been
	 *    moved down on squaring. So we dont worry about moving them.
	 *
	 * 3) The Node is empty. We add the object.
	 */

	// Initially assume the object is not crossing any boundaries
	obj->clearBounding();

	if (!node->isEmpty() && !node->hasSubNodes()) {
		/*
		 * We want to Insert another object, so we square this Node up and move
		 * all the Objects that arent locked (cause they cross a boundary) down.
		 */
		if ((node->maxX - node->minX <= minSquareSize) && (node->maxY - node->minY <= minSquareSize)) {
			/*
			 * This protects from killing the stack. If something is messed up it may
			 * blow the stack because the recursion runs forever. Stop squaring when
			 * it doesnt make sense anymore. If the two objects have the same coordinate
			 * we add the new one to the map. The search is linear for objects inside
			 * .1 Unit. So what.
			 */
			node->addObject(obj);
			return;
		}

		/**
		 * Proceed objects in node in reverse direction since this
		 * makes handling deletions from the vector easier.
		 */
		for (int i = node->objects.size() - 1; i >= 0; i--) {
			QuadTreeEntryInterface* existing = node->getObject(i);

			// We remove the Object from the Node if its not locked
			// for crossing boundaries to add it to another Node
			if (existing->isBounding())
				continue;

			// Increment the refcount on the Object since if this is the
			// last reference ever, the object will be destroyed by Delete()
			//existing->IncRef ();
			node->removeObject(i);

			// First find out which square it needs, then Insert it into it
			// We divide the Node area into 4 squares, reusing existing children

			if (existing->isInSWArea(node)) {
				if (node->swNode == nullptr)
					node->swNode = new BasicQuadTreeNode(node->minX, node->minY, node->dividerX, node->dividerY, node);

				_insert(node->swNode, existing);
			} else if (existing->isInSEArea(node)) {
				if (node->seNode == nullptr)
					node->seNode = new BasicQuadTreeNode(node->dividerX, node->minY, node->maxX, node->dividerY, node);

				_insert(node->seNode, existing);
			} else if (existing->isInNWArea(node)) {
				if (node->nwNode == nullptr)
					node->nwNode = new BasicQuadTreeNode(node->minX, node->dividerY, node->dividerX, node->maxY, node);

				_insert(node->nwNode, existing);
			} else {
				if (node->neNode == nullptr)
					node->neNode = new BasicQuadTreeNode(node->dividerX, node->dividerY, node->maxX, node->maxY, node);

				_insert(node->neNode, existing);
			}

			// Okay, we don't need this reference anymore
			//existing->DecRef ();
		}
	}

	/*
	 * Maybe we squared, maybe we didnt. Anyway, this object extends beyond one
	 * of the boundaries, so we cannot put it into a lower node. It will be
	 * placed in this one regardless and locked.
	 */

	if (obj->isInArea(node)) {
		obj->setBounding();
		node->addObject(obj);

		return;
	}

	/*
	 * The Node has been squared because there was already an object inside.
	 * Also, the new object is contained in one of those new squares.
	 * So we search for the right one and insert the object there.
	 */
	if (node->hasSubNodes()) {
		if (obj->isInSWArea(node)) {
			if (node->swNode == nullptr)
				node->swNode = new BasicQuadTreeNode(node->minX, node->minY, node->dividerX, node->dividerY, node);

			_insert(node->swNode, obj);
		} else if (obj->isInSEArea(node)) {
			if (node->seNode == nullptr)
				node->seNode = new BasicQuadTreeNode(node->dividerX, node->minY, node->maxX, node->dividerY, node);

			_insert(node->seNode, obj);
		} else if (obj->isInNWArea(node)) {
			if (node->nwNode == nullptr)
				node->nwNode = new BasicQuadTreeNode(node->minX, node->dividerY, node->dividerX, node->maxY, node);

			_insert(node->nwNode, obj);
		} else {
			if (node->neNode == nullptr)
				node->neNode = new BasicQuadTreeNode(node->dividerX, node->dividerY, node->maxX, node->maxY, node);

			_insert(node->neNode, obj);
		}

		return;
	}

	// Node is not squared, and we have only one data entry, so it can stay
	// this way. Data can be Inserted, and the recursion is over.
	node->addObject(obj);
}

int BasicQuadTree::_inRange(BasicQuadTreeNode* node, float x, float y, float range,
		Vector<QuadTreeEntryInterface* >& objects) {
	int count = 0;

	for (int i = 0; i < node->objects.size(); i++) {
		QuadTreeEntryInterface *o = node->objects.get(i);

		if (o->isInRange(x, y, range)) {
			++count;
			objects.add(o);
		}
	}

	if (node->hasSubNodes()) {
		if (node->nwNode != nullptr && node->nwNode->testInRange(x, y, range))
			count += _inRange(node->nwNode, x, y, range, objects);
		if (node->neNode != nullptr && node->neNode->testInRange(x, y, range))
			count += _inRange(node->neNode, x, y, range, objects);
		if (node->swNode != nullptr && node->swNode->testInRange(x, y, range))
			count += _inRange(node->swNode, x, y, range, objects);
		if (node->seNode != nullptr && node->seNode->testInRange(x, y, range))
			count += _inRange(node->seNode, x, y, range, objects);
	}

	return count;
}



