/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * referencetest.h
 *
 *  Created on: 14/12/2011
 *      Author: victor
 */

#ifndef REFERENCETEST_H_
#define REFERENCETEST_H_

#include "engine/engine.h"


void referenceTest() {
	SortedVector<Reference<Object*> > container;
	container.setNoDuplicateInsertPlan();

	Vector<Object*> nonReferenceContainer;

	for (int i = 0; i < 1000; ++i) {
		Reference<Object*> ref = new Object();

		container.put(ref);
		container.put(ref);
		container.drop(ref);
		container.put(ref);


		nonReferenceContainer.add(ref.get());
		ref->acquire();

		if (i > 0)
			ref = container.get(i - 1);
	}

	container.removeAll();

	for (int i = 0; i < nonReferenceContainer.size(); ++i) {
#ifdef	TRACE_REFERENCES
		nonReferenceContainer.get(i)->printReferenceHolders();
#endif
		printf("%d ref count left\n", nonReferenceContainer.get(i)->getReferenceCount());
	}

}


#endif /* REFERENCETEST_H_ */
