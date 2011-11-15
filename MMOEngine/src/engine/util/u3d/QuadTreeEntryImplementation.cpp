/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "QuadTreeEntry.h"

QuadTreeEntryImplementation::QuadTreeEntryImplementation(QuadTreeNode* n) {
	node = n;
	bounding = false;

	//visibilityRange = 128;

	closeobjects.setInsertPlan(SortedVector<QuadTreeEntry*>::NO_DUPLICATE);

	radius = 0.5f;
}

void QuadTreeEntryImplementation::setNode(QuadTreeNode* n) {
	node = n;
}

bool QuadTreeEntryImplementation::isInSWArea(QuadTreeNode* node) {
	return coordinates.getPositionX() >= node->minX && coordinates.getPositionX() < node->dividerX &&
			coordinates.getPositionY() >= node->minY && coordinates.getPositionY() < node->dividerY;
}


bool QuadTreeEntryImplementation::isInSEArea(QuadTreeNode* node) {
	return coordinates.getPositionX() >= node->dividerX && coordinates.getPositionX() < node->maxX &&
			coordinates.getPositionY() >= node->minY && coordinates.getPositionY() < node->dividerY;
}


bool QuadTreeEntryImplementation::isInNWArea(QuadTreeNode* node) {
	return coordinates.getPositionX() >= node->minX && coordinates.getPositionX() < node->dividerX &&
			coordinates.getPositionY() >= node->dividerY && coordinates.getPositionY() < node->maxY;
}


bool QuadTreeEntryImplementation::isInArea(QuadTreeNode* node) {
	return (coordinates.getPositionX() + radius > node->dividerX && coordinates.getPositionX() - radius < node->dividerX) ||
			(coordinates.getPositionY() + radius > node->dividerY && coordinates.getPositionY() - radius < node->dividerY);
}

uint64 QuadTreeEntryImplementation::getObjectID() {
	return _this->_getObjectID();
}

QuadTreeEntry* QuadTreeEntryImplementation::getRootParent() {
	/*if (parent == NULL)
		return NULL;

	QuadTreeEntry* grandParent = parent;

	while (grandParent->getParent() != NULL)
		grandParent = grandParent->getParent();

	return grandParent;*/

	return NULL;
}

int QuadTreeEntry::compareTo(QuadTreeEntry* obj) {
	if (getDirtyObjectID() < obj->getDirtyObjectID())
		return 1;
	else if (getDirtyObjectID() > obj->getDirtyObjectID())
		return -1;
	else
		return 0;
}

int QuadTreeEntryImplementation::compareTo(QuadTreeEntry* obj) {
	if (getObjectID() < obj->getObjectID())
		return 1;
	else if (getObjectID() > obj->getObjectID())
		return -1;
	else
		return 0;
}

uint64 QuadTreeEntry::getDirtyObjectID() {
	return _getObjectID();
}

uint64 QuadTreeEntryImplementation::getDirtyObjectID() {
	return _this->_getObjectID();
}
