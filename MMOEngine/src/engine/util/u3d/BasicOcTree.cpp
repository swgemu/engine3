/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * LightweightOcTree.cpp
 */

#include "engine/log/Logger.h"

#include "engine/util/u3d/BasicOcTree.h"

namespace OTNode {
static Logger logger("BasicOcTreeNode", Logger::WARNING);
}

using namespace OTNode;

BasicOcTreeNode::BasicOcTreeNode() {
	objects.setNoDuplicateInsertPlan();

	parentNode = nullptr;
	nwNode = neNode = swNode = seNode = nullptr;

	minX = 0;
	minY = 0;
	minZ = 0;
	maxX = 0;
	maxY = 0;
	maxZ = 0;

	dividerX = 0;
	dividerY = 0;
	dividerZ = 0;
}

BasicOcTreeNode::BasicOcTreeNode(float minx, float miny, float minz, float maxx, float maxy, float maxz, BasicOcTreeNode* parent) {
	objects.setNoDuplicateInsertPlan();

	parentNode = parent;
	nwNode = neNode = swNode = seNode = nullptr;
	nwNode2 = neNode2 = swNode2 = seNode2 = nullptr;

	minX = minx;
	minY = miny;
	minZ = minz;
	maxX = maxx;
	maxY = maxy;
	maxZ = maxz;

	if (!validateNode() || minX > maxX || minY > maxY || minZ > maxZ) {
		logger.error() << "[BasicOcTree] invalid node in create - " << *this;
	}

	dividerX = (minX + maxX) / 2;
	dividerY = (minY + maxY) / 2;
	dividerZ = (minZ + maxZ) / 2;
}

BasicOcTreeNode::~BasicOcTreeNode() {
	if (nwNode != nullptr)
		delete nwNode;

	if (neNode != nullptr)
		delete neNode;

	if (swNode != nullptr)
		delete swNode;

	if (seNode != nullptr)
		delete seNode;

	if (nwNode2 != nullptr)
		delete nwNode2;

	if (neNode2 != nullptr)
		delete neNode2;

	if (swNode2 != nullptr)
		delete swNode2;

	if (seNode2 != nullptr)
		delete seNode2;
}

void BasicOcTreeNode::addObject(OcTreeEntryInterface* obj) {
	if (BasicOcTree::doLog())
		logger.info(true) << hex << "object [" << obj->getObjectID() << "] added to BasicOcTree" << *this;

	if (!validateNode())
		logger.error() << "[BasicOcTree] invalid node in addObject() - " << *this;

	objects.put(obj);

	E3_ASSERT(obj->getNode() == nullptr);

	obj->setNode(this);
}

void BasicOcTreeNode::removeObject(OcTreeEntryInterface* obj) {
	if (!objects.drop(obj)) {
		logger.error() << hex << "object [" << obj->getObjectID() << "] not found on BasicOcTree" << *this;
	} else {
		obj->setNode(nullptr);

		if (BasicOcTree::doLog())
			logger.info(true) << hex << "object [" << obj->getObjectID() << "] removed BasicOcTree" << *this;
	}
}

void BasicOcTreeNode::removeObject(int index) {
	OcTreeEntryInterface* obj = objects.remove(index);
	obj->setNode(nullptr);
}

bool BasicOcTreeNode::testInside(OcTreeEntryInterface* obj) const {
	float x = obj->getPositionX();
	float y = obj->getPositionY();
	float z = obj->getPositionZ();

	return x >= minX && x < maxX && y >= minY && y < maxY && z >= minZ && z < maxZ;
}

bool BasicOcTreeNode::testInRange(float x, float y, float z, float range) const {
	bool insideX = (minX <= x) && (x < maxX);
	bool insideY = (minY <= y) && (y < maxY);
	bool insideZ = (minZ <= z) && (z < maxZ);

	if (insideX && insideY && insideZ)
		return true;

	bool closeenoughX = (fabs(minX - x) <= range || fabs(maxX - x) <= range);
	bool closeenoughY = (fabs(minY - y) <= range || fabs(maxY - y) <= range);
	bool closeenoughZ = (fabs(minZ - z) <= range || fabs(maxZ - z) <= range);

	if ((insideX || closeenoughX) && (insideY || closeenoughY) && (insideZ || closeenoughZ))
		return true;
	else
		return false;
}

void BasicOcTreeNode::check() {
	if (isEmpty() && !hasSubNodes() && parentNode != nullptr) {
		if (parentNode->nwNode == this)
			parentNode->nwNode = nullptr;
		else if (parentNode->neNode == this)
			parentNode->neNode = nullptr;
		else if (parentNode->swNode == this)
			parentNode->swNode = nullptr;
		else if (parentNode->seNode == this)
			parentNode->seNode = nullptr;
		else if (parentNode->nwNode2 == this)
			parentNode->nwNode2 = nullptr;
		else if (parentNode->neNode2 == this)
			parentNode->neNode2 = nullptr;
		else if (parentNode->swNode2 == this)
			parentNode->swNode2 = nullptr;
		else if (parentNode->seNode2 == this)
			parentNode->seNode2 = nullptr;

		if (BasicOcTree::doLog())
			logger.info(true) << "deleteing node (" << *this << ")";

		delete this;
	}
}

String BasicOcTreeNode::toStringData() const {
	StringBuffer s;
	s << "Node " << this << " (" << (int)minX << ", " << (int)minY << ", " << (int)minZ << ", " << (int)maxX << ", " << (int)maxY << ", " << maxZ << ") [" << objects.size() << "]";

	return s.toString();
}

int BasicOcTreeNode::_getSubNodeCount(const BasicOcTreeNode* s) {
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

	if (s->neNode2 != nullptr) {
		count += _getSubNodeCount(s->neNode2);
	}

	if (s->nwNode2 != nullptr) {
		count += _getSubNodeCount(s->nwNode2);
	}

	if (s->seNode2 != nullptr) {
		count += _getSubNodeCount(s->seNode2);
	}

	if (s->swNode2 != nullptr) {
		count += _getSubNodeCount(s->swNode2);
	}

	return count;
}

int BasicOcTreeNode::getNodeCount() const {
	return _getSubNodeCount(this);
}

//---------------------------------------------------------------------------//

bool BasicOcTree::logTree = false;

BasicOcTree::BasicOcTree() : root(nullptr), minSquareSize(8) {
}

BasicOcTree::BasicOcTree(float minx, float miny, float minz, float maxx, float maxy, float maxz, float minSquareSize) : root(new BasicOcTreeNode(minx, miny, minz, maxx, maxy, maxz, nullptr)), minSquareSize(minSquareSize) {
}

BasicOcTree::~BasicOcTree() {
	delete root;

	root = nullptr;
}

void BasicOcTree::setSize(float minx, float miny, float minz, float maxx, float maxy, float maxz) {
	delete root;

	root = new BasicOcTreeNode(minx, miny, minz, maxx, maxy, maxz, nullptr);
}

void BasicOcTree::insert(OcTreeEntryInterface* obj) {
	/*if (!isLocked()) {
		System::out << "inserting to unlocked quad tree\n";
		StackTrace::printStackTrace();
		raise(SIGSEGV);
	}*/

	try {
		if (BasicOcTree::doLog()) {
			logger.info(true) << hex << "object [" << obj->getObjectID() << "] inserting" << "(" << obj->getPositionX() << ", " << obj->getPositionY() << ", " << obj->getPositionZ() << ")";
		}

		if (obj->getNode() != nullptr)
			remove(obj);

		_insert(root, obj);

		if (BasicOcTree::doLog())
			logger.info(true) << hex << "object [" << obj->getObjectID() << "] finished inserting";
	} catch (const Exception& e) {
		logger.error() << "[BasicOcTree] error - " << e.getMessage();
		e.printStackTrace();
	}
}

int BasicOcTree::inRange(float x, float y, float z, float range, Vector<OcTreeEntryInterface*>& objects) {
	try {
		return _inRange(root, x, y, z, range, objects);
	} catch (const Exception& e) {
		logger.error() << "[BasicOcTree] " << e.getMessage();
		e.printStackTrace();
	}

	return 0;
}

void BasicOcTree::remove(BasicOcTreeNode* node) {
	if (!node->validateNode()) {
		logger.error() << "[BasicOcTree] "
					   << " error on remove(BasicOcTreeNode) - invalid Node" << *node;

		return;
	}

	if (node->parentNode == nullptr) {
		logger.error() << "[BasicOcTree] "
					   << " error on remove(BasicOcTreeNode) - trying to remove root Node" << *node;

		return;
	}

	BasicOcTreeNode* parent = node->parentNode;

	if (parent->nwNode == node) {
		parent->nwNode = nullptr;
	}

	if (parent->neNode == node) {
		parent->neNode = nullptr;
	}

	if (parent->swNode == node) {
		parent->swNode = nullptr;
	}

	if (parent->seNode == node) {
		parent->seNode = nullptr;
	}
}

int BasicOcTree::getNodeCount() {
	return root->getNodeCount();
}

void BasicOcTree::remove(OcTreeEntryInterface* obj) {
	/*if (!isLocked()) {
		System::out << "remove on unlocked quad tree\n";
		StackTrace::printStackTrace();
		raise(SIGSEGV);
	}*/

	if (BasicOcTree::doLog())
		logger.info(true) << hex << "object [" << obj->getObjectID() << "] removing";

	BasicOcTreeNode* node = obj->getNode();

	if (node != nullptr) {
		if (!node->validateNode()) {
			logger.error() << "[BasicOcTree] " << obj->getObjectID() << " error on remove() - invalid Node" << *node;
		}

		node->removeObject(obj);

		node->check();
		obj->setNode(nullptr);
	} else {
		logger.error() << hex << "object [" << obj->getObjectID() << "] ERROR - removing the node";
		StackTrace::printStackTrace();
	}

	if (BasicOcTree::doLog())
		logger.info(true) << hex << "object [" << obj->getObjectID() << "] finished removing";
}

void BasicOcTree::removeAll() {
	if (root != nullptr) {
		root = nullptr;
		delete root;
	}
}

/*
 * Every Node can have data and children. Every data must be completely
 * contained inside the Node, so boundary sphere is checked.
 */
void BasicOcTree::_insert(BasicOcTreeNode* node, OcTreeEntryInterface* obj) {
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
		if ((node->maxX - node->minX <= minSquareSize) && (node->maxY - node->minY <= minSquareSize) && (node->maxZ - node->minZ <= minSquareSize)) {
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
			OcTreeEntryInterface* existing = node->getObject(i);

			// We remove the Object from the Node if its not locked
			// for crossing boundaries to add it to another Node
			if (existing->isBounding())
				continue;

			// Increment the refcount on the Object since if this is the
			// last reference ever, the object will be destroyed by Delete()
			// existing->IncRef ();
			node->removeObject(i);

			// First find out which square it needs, then Insert it into it
			// We divide the Node area into 4 squares, reusing existing children

			if (existing->isInSWArea(node)) {
				if (node->swNode == nullptr)
					node->swNode = new BasicOcTreeNode(node->minX, node->minY, node->dividerX, node->dividerY, node);

				_insert(node->swNode, existing);
			} else if (existing->isInSEArea(node)) {
				if (node->seNode == nullptr)
					node->seNode = new BasicOcTreeNode(node->dividerX, node->minY, node->maxX, node->dividerY, node);

				_insert(node->seNode, existing);
			} else if (existing->isInNWArea(node)) {
				if (node->nwNode == nullptr)
					node->nwNode = new BasicOcTreeNode(node->minX, node->dividerY, node->dividerX, node->maxY, node);

				_insert(node->nwNode, existing);
			} else if (existing->isInNEArea(node)) {
				if (node->neNode == nullptr)
					node->neNode = new BasicOcTreeNode(node->dividerX, node->dividerY, node->maxX, node->maxY, node);

				_insert(node->neNode, existing);
			} else if (existing->isInSW2Area(node)) {
				if (node->swNode2 == nullptr)
					node->swNode2 = new BasicOcTreeNode(node->minX, node->minY, node->dividerX, node->dividerY, node);

				_insert(node->swNode2, existing);
			} else if (existing->isInSE2Area(node)) {
				if (node->seNode2 == nullptr)
					node->seNode2 = new BasicOcTreeNode(node->dividerX, node->minY, node->maxX, node->dividerY, node);

				_insert(node->seNode2, existing);
			} else if (existing->isInNW2Area(node)) {
				if (node->nwNode2 == nullptr)
					node->nwNode2 = new BasicOcTreeNode(node->minX, node->dividerY, node->dividerX, node->maxY, node);

				_insert(node->nwNode2, existing);
			} else {
				if (node->neNode2 == nullptr)
					node->neNode2 = new BasicOcTreeNode(node->dividerX, node->dividerY, node->maxX, node->maxY, node);

				_insert(node->neNode2, existing);
			}

			// Okay, we don't need this reference anymore
			// existing->DecRef ();
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
				node->swNode = new BasicOcTreeNode(node->minX, node->minY, node->dividerX, node->dividerY, node);

			_insert(node->swNode, obj);
		} else if (obj->isInSEArea(node)) {
			if (node->seNode == nullptr)
				node->seNode = new BasicOcTreeNode(node->dividerX, node->minY, node->maxX, node->dividerY, node);

			_insert(node->seNode, obj);
		} else if (obj->isInNWArea(node)) {
			if (node->nwNode == nullptr)
				node->nwNode = new BasicOcTreeNode(node->minX, node->dividerY, node->dividerX, node->maxY, node);

			_insert(node->nwNode, obj);
		} else if (obj->isInNEArea(node)) {
			if (node->neNode == nullptr)
				node->neNode = new BasicOcTreeNode(node->dividerX, node->dividerY, node->maxX, node->maxY, node);

			_insert(node->neNode, obj);
		} else if (obj->isInSW2Area(node)) {
			if (node->swNode2 == nullptr)
				node->swNode2 = new BasicOcTreeNode(node->minX, node->minY, node->dividerX, node->dividerY, node);

			_insert(node->swNode2, obj);
		} else if (obj->isInSE2Area(node)) {
			if (node->seNode2 == nullptr)
				node->seNode2 = new BasicOcTreeNode(node->dividerX, node->minY, node->maxX, node->dividerY, node);

			_insert(node->seNode2, obj);
		} else if (obj->isInNW2Area(node)) {
			if (node->nwNode2 == nullptr)
				node->nwNode2 = new BasicOcTreeNode(node->minX, node->dividerY, node->dividerX, node->maxY, node);

			_insert(node->nwNode2, obj);
		} else {
			if (node->neNode2 == nullptr)
				node->neNode2 = new BasicOcTreeNode(node->dividerX, node->dividerY, node->maxX, node->maxY, node);

			_insert(node->neNode2, obj);
		}

		return;
	}

	// Node is not squared, and we have only one data entry, so it can stay
	// this way. Data can be Inserted, and the recursion is over.
	node->addObject(obj);
}

int BasicOcTree::_inRange(BasicOcTreeNode* node, float x, float y, float z, float range, Vector<OcTreeEntryInterface*>& objects) {
	int count = 0;

	for (int i = 0; i < node->objects.size(); i++) {
		OcTreeEntryInterface* o = node->objects.get(i);

		if (o->isInRange(x, y, z, range)) {
			++count;
			objects.add(o);
		}
	}

	if (node->hasSubNodes()) {
		if (node->nwNode != nullptr && node->nwNode->testInRange(x, y, range))
			count += _inRange(node->nwNode, x, y, z, range, objects);
		if (node->neNode != nullptr && node->neNode->testInRange(x, y, range))
			count += _inRange(node->nwNode, x, y, z, range, objects);
		if (node->swNode != nullptr && node->swNode->testInRange(x, y, range))
			count += _inRange(node->nwNode, x, y, z, range, objects);
		if (node->seNode != nullptr && node->seNode->testInRange(x, y, range))
			count += _inRange(node->nwNode, x, y, z, range, objects);

		if (node->nwNode2 != nullptr && node->nwNode2->testInRange(x, y, range))
			count += _inRange(node->nwNode2, x, y, z, range, objects);
		if (node->neNode2 != nullptr && node->neNode2->testInRange(x, y, range))
			count += _inRange(node->nwNode2, x, y, z, range, objects);
		if (node->swNode2 != nullptr && node->swNode2->testInRange(x, y, range))
			count += _inRange(node->nwNode, x, y, z, range, objects);
		if (node->seNode2 != nullptr && node->seNode2->testInRange(x, y, range))
			count += _inRange(node->nwNode2, x, y, z, range, objects);
	}

	return count;
}
