/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * hashtableiterator.h
 *
 *  Created on: 05/10/2012
 *      Author: victor
 */

#ifndef HASHTABLEITERATOR_H_
#define HASHTABLEITERATOR_H_

#include "engine/engine.h"

#define OBJECTS_TO_CREATE 1000000

void testHashTableIterator() {
	printf("initializing %d objecs\n", OBJECTS_TO_CREATE);

	HashTable<uint64, Object*> table;

	uint64 id = 0;

	for (int i = 0; i < OBJECTS_TO_CREATE; ++i) {
		Object* obj = new ManagedObject();

		table.put(++id, obj);

		printf("\r[%d]", i);
	}

	printf("\ndone initializing\n");

	printf("iterating table\n");

	Time start;

	HashTableIterator<uint64, Object*> iterator = table.iterator();

	Vector<Object*> objects;

	objects.removeAll(table.size(), 1);

	while (iterator.hasNext()) {
		Object* obj = iterator.getNextValue();

		ManagedObject* dist = dynamic_cast<ManagedObject*>(obj);

		assert(dist != nullptr);

		objects.add(dist);
	}

	printf("done iterating in %lld\n", start.miliDifference());
}


#endif /* HASHTABLEITERATOR_H_ */
