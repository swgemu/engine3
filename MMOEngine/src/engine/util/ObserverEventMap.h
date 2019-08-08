/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * ObserverEventMap.h
 *
 *  Created on: 20/06/2010
 *      Author: victor
 */

#ifndef OBSERVEREVENTMAP_H_
#define OBSERVEREVENTMAP_H_

namespace engine {
 namespace util {
	 class Observable;
	 class Observer;
 }
}

using namespace engine::util;

#include "system/lang.h"

#include "engine/core/ManagedReference.h"

class ObserverEventMap : public HashTable<uint32, SortedVector<ManagedReference<Observer*> > > {
	mutable Mutex observerMutex;

public:
	ObserverEventMap() {
		//setNoDuplicateInsertPlan();
	}

	ObserverEventMap(const ObserverEventMap& map) : HashTable<uint32, SortedVector<ManagedReference<Observer*> > >(map), observerMutex() {
		//setNoDuplicateInsertPlan();
	}

	void notifyObservers(uint32 eventType, Observable* observable, ManagedObject* arg1 = nullptr, int64 arg2 = 0);

	void registerObserver(uint32 eventType, Observer* observer);

	void dropObserver(uint32 eventType, Observer* observer);

	SortedVector<ManagedReference<Observer*> > getObservers(uint32 eventType) const;

	int getObserverCount(uint32 eventType) const;
};


#endif /* OBSERVEREVENTMAP_H_ */
