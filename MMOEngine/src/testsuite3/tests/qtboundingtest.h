/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * qtboundingtest.h
 *
 *  Created on: 31/12/2011
 *      Author: victor
 */

#ifndef QTBOUNDINGTEST_H_
#define QTBOUNDINGTEST_H_

#include "engine/util/u3d/QuadTree.h"
#include "engine/util/u3d/QuadTreeEntry.h"

void testQTBounding() {
	uint64 ids = 1;
	Reference<QuadTree*> quadTree = new QuadTree(-8192, -8192, 8192, 8192);

	SortedVector<ManagedReference<QuadTreeEntry*> > objects;

	Reference<QuadTreeEntry*> entry = new QuadTreeEntry();
	entry->_setObjectID(++ids);
	entry->initializePosition(0, 0, 0);
	entry->setRadius(128);

	quadTree->insert(entry);

	entry = new QuadTreeEntry();
	entry->_setObjectID(++ids);
	entry->initializePosition(0, 0, 0);
	entry->setRadius(16384);

	quadTree->insert(entry);

	entry = new QuadTreeEntry();
	entry->_setObjectID(++ids);
	entry->initializePosition(512, 512, 512);
	entry->setRadius(1024);

	quadTree->insert(entry);

	quadTree->inRange(0, 0, objects);

	printf("%d in range entries\n", objects.size());

}


#endif /* QTBOUNDINGTEST_H_ */
