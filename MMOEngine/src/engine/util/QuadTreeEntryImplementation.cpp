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
