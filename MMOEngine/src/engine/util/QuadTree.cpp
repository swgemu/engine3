/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

/*
   Quad tree implementation
   Copyright (C) 2003 by The Arianne Team
   Copyright (C) 2005 Team OpenWoW
   Copyright (C) 2006 Team SWGEmu

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <math.h>

#include "QuadTree.h"

QuadTreeNode::QuadTreeNode(float minx, float miny, float maxx, float maxy, QuadTreeNode *parent) {
	objects.setInsertPlan(SortedVector<QuadTreeEntry*>::NO_DUPLICATE);

   	parentNode = parent;
   	nwNode = neNode = swNode = seNode = NULL;

   	minX = minx;
   	minY = miny;
   	maxX = maxx;
   	maxY = maxy;

   	if (!validateNode() || minX > maxX || minY > maxY) {
		StringBuffer msg;
		msg << "[QuadTree] invalid node in create - " << toString();
		Logger::console.info(msg, true);
   	}
}

QuadTreeNode::~QuadTreeNode() {
	if (nwNode != NULL)
		delete nwNode;

	if (neNode != NULL)
		delete neNode;

	if (swNode != NULL)
 		delete swNode;

	if (seNode != NULL)
		delete seNode;
}

void QuadTreeNode::addObject(QuadTreeEntry *obj) {
	if (QuadTree::doLog())
		System::out << hex << "object [" << obj->getObjectID() <<  "] added to QuadTree"
					<< toString() << "\n";

	if (!validateNode())
		System::out << "[QuadTree] invalid node in addObject() - " << toString() << "\n";

	objects.put(obj);
  	obj->node = this;
}

void QuadTreeNode::removeObject(QuadTreeEntry *obj) {
	if (!objects.drop(obj)) {
		System::out << hex << "object [" << obj->getObjectID() <<  "] not found on QuadTree"
					<< toString() << "\n";
	} else {
    	obj->node = NULL;

		if (QuadTree::doLog())
			System::out <<  hex << "object [" << obj->getObjectID() <<  "] removed QuadTree"
						<< toString() << "\n";
	}
}

bool QuadTreeNode::testInRange(float x, float y, float range) {
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

void QuadTreeNode::check () {
    if (isEmpty() && !hasSubNodes() && parentNode != NULL) {
        if (parentNode->nwNode == this)
            parentNode->nwNode = NULL;
        else if (parentNode->neNode == this)
            parentNode->neNode = NULL;
        else if (parentNode->swNode == this)
            parentNode->swNode = NULL;
        else if (parentNode->seNode == this)
            parentNode->seNode = NULL;

		if (QuadTree::doLog())
			System::out << "deleteing node (" << (uint64) this << ")\n";

        delete this;
    }
}

String QuadTreeNode::toString() {
	StringBuffer s;
	s << "Node " << (uint64) this << " (" << (int) minX << ","
	  << (int) minY << "," << (int) maxX << "," << (int) maxY
	  << ") [" << objects.size() << "]";

	return s.toString();
}

//---------------------------------------------------------------------------//

bool QuadTree::logTree = false;

QuadTree::QuadTree() : Mutex("QuadTree") {
    root = NULL;
}

QuadTree::QuadTree(float minx, float miny, float maxx, float maxy) : Mutex("QuadTree") {
    root = new QuadTreeNode(minx, miny, maxx, maxy, NULL);
}

QuadTree::~QuadTree() {
    delete root;

   	root = NULL;
}

void QuadTree::setSize(float minx, float miny, float maxx, float maxy) {
    delete root;

    root = new QuadTreeNode(minx, miny, maxx, maxy, NULL);
}

void QuadTree::insert(QuadTreeEntry *obj) {
	try {
		if (QuadTree::doLog()) {
			System::out << hex << "object [" << obj->getObjectID() <<  "] inserting\n";
			System::out << "(" << obj->getPositionX() << ", " << obj->getPositionY() << ")\n";
		}

	    if (obj->node != NULL)
    	    remove(obj);

    	_insert(root, obj);

		if (QuadTree::doLog())
			System::out << hex << "object [" << obj->getObjectID() <<  "] finished inserting\n";
	} catch (...) {
		System::out << "[QuadTree] error on insert object\n";
	}
}

bool QuadTree::update(QuadTreeEntry *obj) {
	try {
		if (QuadTree::doLog()) {
			System::out << hex << "object [" << obj->getObjectID() <<  "] updating on node "
						<< obj->node->toString() << " \n" << "(" << obj->getPositionX()
						<< ", " << obj->getPositionY() << ")\n";
		}

		if (obj->node == NULL) {
			System::out << hex << "object [" << obj->getObjectID() <<  "] updating error\n";
			return false;
		}

		bool res = _update(obj->node, obj);

		if (QuadTree::doLog())
			System::out << hex << "object [" << obj->getObjectID() <<  "] finished updating\n";

		return res;
	} catch (...) {
		System::out << "[QuadTree] error on update object\n";
		return false;
	}
}

void QuadTree::inRange(QuadTreeEntry *obj, float range) {
	try {
		_inRange(root, obj, range);

		if (QuadTree::doLog()) {
			System::out << hex << "object [" << obj->getObjectID() <<  "] in range (";

			for (int i = 0; i < obj->inRangeObjectCount(); ++i) {
				System::out << hex << obj->getInRangeObject(i)->getObjectID() << ", ";
			}

			System::out << "\n";
		}

	} catch (Exception& e) {
		System::out << "[QuadTree] " << e.getMessage() << "\n";
	} catch (...) {
		System::out << "[QuadTree] error on inRange\n";
	}
}

int QuadTree::inRange(float x, float y, float range) {
	try {
		return _inRange(root, x, y, range);
	} catch (Exception& e) {
		System::out << "[QuadTree] " << e.getMessage() << "\n";
	} catch (...) {
		System::out << "[QuadTree] error on inRange\n";
	}

	return 0;
}

void QuadTree::remove(QuadTreeEntry *obj) {
	if (QuadTree::doLog())
		System::out << hex << "object [" << obj->getObjectID() <<  "] removing\n";

    if (obj->node != NULL) {
        QuadTreeNode* node = obj->node;

        if (!node->validateNode()) {
			System::out << "[QuadTree] " << obj->getObjectID() << " error on remove() - invalid Node"
						<< node->toString() << "\n";
        }

       	node->removeObject(obj);

       	node->check();
       	obj->node = NULL;
    } else {
		System::out << hex << "object [" << obj->getObjectID() <<  "] ERROR - removing the node\n";
		StackTrace::printStackTrace();
    }

	if (QuadTree::doLog())
		System::out << hex << "object [" << obj->getObjectID() <<  "] finished removing\n";
}

void QuadTree::removeAll() {
	if (root != NULL)
    	delete root;
}

/*
 * Every Node can have data and children. Every data must be completely
 * contained inside the Node, so boundary sphere is checked.
 */
void QuadTree::_insert(QuadTreeNode *node, QuadTreeEntry *obj) {
    float dividerX = (node->minX + node->maxX) / 2;
    float dividerY = (node->minY + node->maxY) / 2;

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
    obj->bounding = false;

    if (!node->isEmpty() && !node->hasSubNodes()) {
        /*
         * We want to Insert another object, so we square this Node up and move
         * all the Objects that arent locked (cause they cross a boundary) down.
         */
        if ((node->maxX - node->minX <= 8) && (node->maxY - node->minY <= 8)) {
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
            QuadTreeEntry* existing = node->getObject(i);

            // We remove the Object from the Node if its not locked
            // for crossing boundaries to add it to another Node
            if (existing->bounding)
                continue;

            // Increment the refcount on the Object since if this is the
            // last reference ever, the object will be destroyed by Delete()
            //existing->IncRef ();
            node->removeObject(i);

            // First find out which square it needs, then Insert it into it
            // We divide the Node area into 4 squares, reusing existing children

            if (existing->positionX >= node->minX && existing->positionX < dividerX &&
                	existing->positionY >= node->minY && existing->positionY < dividerY) {
                if (node->swNode == NULL)
                    node->swNode = new QuadTreeNode(node->minX, node->minY, dividerX, dividerY, node);

                _insert(node->swNode, existing);
            } else if (existing->positionX >= dividerX && existing->positionX < node->maxX &&
          			existing->positionY >= node->minY && existing->positionY < dividerY) {
                if (node->seNode == NULL)
                    node->seNode = new QuadTreeNode(dividerX, node->minY, node->maxX, dividerY, node);

                _insert(node->seNode, existing);
            } else if (existing->positionX >= node->minX && existing->positionX < dividerX &&
                 	existing->positionY >= dividerY && existing->positionY < node->maxY) {
                if (node->nwNode == NULL)
                    node->nwNode = new QuadTreeNode(node->minX, dividerY, dividerX, node->maxY, node);

                _insert(node->nwNode, existing);
            } else {
                if (node->neNode == NULL)
                    node->neNode = new QuadTreeNode(dividerX, dividerY, node->maxX, node->maxY, node);

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

    if ((obj->positionX + obj->radius > dividerX && obj->positionX - obj->radius < dividerX) ||
            (obj->positionY + obj->radius > dividerY && obj->positionY - obj->radius < dividerY)) {
        obj->bounding = true;
        node->addObject(obj);

        return;
    }

    /*
     * The Node has been squared because there was already an object inside.
     * Also, the new object is contained in one of those new squares.
     * So we search for the right one and insert the object there.
     */
    if (node->hasSubNodes()) {
        if (obj->positionX >= node->minX && obj->positionX < dividerX &&
                obj->positionY >= node->minY && obj->positionY < dividerY) {
            if (node->swNode == NULL)
                node->swNode = new QuadTreeNode(node->minX, node->minY, dividerX, dividerY, node);

            _insert(node->swNode, obj);
        } else if (obj->positionX >= dividerX && obj->positionX < node->maxX &&
                obj->positionY >= node->minY && obj->positionY < dividerY) {
            if (node->seNode == NULL)
                node->seNode = new QuadTreeNode(dividerX, node->minY, node->maxX, dividerY, node);

            _insert(node->seNode, obj);
        } else if (obj->positionX >= node->minX && obj->positionX < dividerX &&
                obj->positionY >= dividerY && obj->positionY < node->maxY) {
            if (node->nwNode == NULL)
                node->nwNode = new QuadTreeNode(node->minX, dividerY, dividerX, node->maxY, node);

            _insert(node->nwNode, obj);
        } else {
            if (node->neNode == NULL)
                node->neNode = new QuadTreeNode(dividerX, dividerY, node->maxX, node->maxY, node);

            _insert(node->neNode, obj);
        }

        return;
    }

    // Node is not squared, and we have only one data entry, so it can stay
    // this way. Data can be Inserted, and the recursion is over.
    node->addObject(obj);
}

/* The difference to the Insert is that it starts at the current node
 * and tries to find the right place to be now that the position changed.
 */
bool QuadTree::_update(QuadTreeNode *node, QuadTreeEntry *obj) {
    // Whew, still in the same square. Lucky bastards we are.
    //System::out << "(" << obj->positionX << "," << obj->positionY << ")\n";

    if (node->testInside(obj->positionX, obj->positionY))
        return true;

    // Since we'll have to temporarily remove the object from the Quad Tree,
    // make sure it won't be deleted as we do that
    //data->IncRef ();

    // Go upwards til the object is inside the square.
    QuadTreeNode *cur = node->parentNode;
    while (cur != NULL && !cur->testInside(obj->positionX, obj->positionY))
        cur = cur->parentNode;

    remove(obj);

    // Here is the right spot for the object, so lets drop it in.
    // May result in another squaring frenzy.
    if (cur != NULL)
        _insert(cur, obj);
	else
		System::out << "[QuadTree] error on update() - invalid Node\n";

    // We don't need the reference anymore. If the object goes out of the
    // quadtree and there aren't any references left... well... goodbye
    //data->DecRef ();
    return cur != NULL;
}

void QuadTree::_inRange(QuadTreeNode *node, QuadTreeEntry *obj, float range) {
 	float rangesq = range * range;

	float x = obj->positionX;
	float y = obj->positionY;

	float oldx = obj->previousPositionX;
	float oldy = obj->previousPositionY;

	for (int i = 0; i < node->objects.size(); i++) {
		QuadTreeEntry *o = node->objects.get(i);

		if (o != obj) {
			float deltaX = x - o->positionX;
			float deltaY = y - o->positionY;

			if (deltaX * deltaX + deltaY * deltaY <= rangesq) {
				obj->addInRangeObject(o, false);
				o->addInRangeObject(obj);
 			} else {
 				float oldDeltaX = oldx - o->positionX;
 				float oldDeltaY = oldy - o->positionY;

 				if (oldDeltaX * oldDeltaX + oldDeltaY * oldDeltaY <= rangesq) {
 	 				obj->removeInRangeObject(o);
 					o->removeInRangeObject(obj);

 					/*if (obj->closeobjects.contains(o)) {
 						QuadTreeEntry* obj2 = NULL;
 						obj2->getPositionX();
 					}

 					if (o->closeobjects.contains(obj)) {
 						QuadTreeEntry* obj2 = NULL;
 						obj2->getPositionX();
 					}*/

 				}
 			}
		} else
			obj->addInRangeObject(obj, false);
	}

	if (node->hasSubNodes()) {
		if (node->nwNode != NULL && node->nwNode->testInRange(x, y, range))
 			_inRange(node->nwNode, obj, range);
		if (node->neNode != NULL && node->neNode->testInRange(x, y, range))
			_inRange(node->neNode, obj, range);
		if (node->swNode != NULL && node->swNode->testInRange(x, y, range))
			_inRange(node->swNode, obj, range);
		if (node->seNode != NULL && node->seNode->testInRange(x, y, range))
			_inRange(node->seNode, obj, range);
	}
}

int QuadTree::_inRange(QuadTreeNode *node, float x, float y, float range) {
	int count = 0;

 	float rangesq = range * range;

	for (int i = 0; i < node->objects.size(); i++) {
		QuadTreeEntry *o = node->objects.get(i);

		float deltaX = x - o->positionX;
		float deltaY = y - o->positionY;

		if (deltaX * deltaX + deltaY * deltaY <= rangesq)
			++count;
	}

	if (node->hasSubNodes()) {
		if (node->nwNode != NULL && node->nwNode->testInRange(x, y, range))
 			count += _inRange(node->nwNode, x, y, range);
		if (node->neNode != NULL && node->neNode->testInRange(x, y, range))
			count += _inRange(node->neNode, x, y, range);
		if (node->swNode != NULL && node->swNode->testInRange(x, y, range))
			count += _inRange(node->swNode, x, y, range);
		if (node->seNode != NULL && node->seNode->testInRange(x, y, range))
			count += _inRange(node->seNode, x, y, range);
	}

	return count;
}
