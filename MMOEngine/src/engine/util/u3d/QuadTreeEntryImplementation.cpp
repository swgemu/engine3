/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "QuadTreeEntry.h"

QuadTreeEntryImplementation::QuadTreeEntryImplementation(QuadTreeNode* n) : Coordinate() {
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
	return positionX >= node->minX && positionX < node->dividerX &&
			positionY >= node->minY && positionY < node->dividerY;
}


bool QuadTreeEntryImplementation::isInSEArea(QuadTreeNode* node) {
	return positionX >= node->dividerX && positionX < node->maxX &&
			positionY >= node->minY && positionY < node->dividerY;
}


bool QuadTreeEntryImplementation::isInNWArea(QuadTreeNode* node) {
	return positionX >= node->minX && positionX < node->dividerX &&
			positionY >= node->dividerY && positionY < node->maxY;
}


bool QuadTreeEntryImplementation::isInArea(QuadTreeNode* node) {
	return (positionX + radius > node->dividerX && positionX - radius < node->dividerX) ||
			(positionY + radius > node->dividerY && positionY - radius < node->dividerY);
}

