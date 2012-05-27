/*
 * ObserverEventType.cpp
 *
 *  Created on: 20/06/2010
 *      Author: victor
 */

#include "ObserverEventMap.h"
#include "Observer.h"

void ObserverEventMap::notifyObservers(uint32 eventType, Observable* observable, ManagedObject* arg1, int64 arg2) {
	Locker locker(&observerMutex);

	if (!containsKey(eventType))
		return;

	SortedVector<ManagedReference<Observer*> >* observers = &get(eventType);

	SortedVector<ManagedReference<Observer*> > observersCopy(*observers);

	locker.release();

	for (int i = 0; i < observersCopy.size(); ++i) {
		ManagedReference<Observer*> observer = observersCopy.get(i);

		int result = observer->notifyObserverEvent(eventType, observable, arg1, arg2);

		if (result == 1) {
			dropObserver(eventType, observer);
		}

	}
}

void ObserverEventMap::registerObserver(uint32 eventType, Observer* observer) {
	if (observer == NULL) {
		System::out << "registering NULL OBSERVER" << endl;
		StackTrace::printStackTrace();

		return;
	}

	Locker locker(&observerMutex);

	if (!observer->isDeplyoed())
		observer->deploy();

	if (!containsKey(eventType)) {
		SortedVector<ManagedReference<Observer*> > observers;
		observers.put(observer);

		this->put(eventType, observers);
	} else {
		SortedVector<ManagedReference<Observer*> >* observers = &get(eventType);
		observers->setNoDuplicateInsertPlan();

		if (observers->put(observer) == -1) {
			Logger::console.warning("duplicate observer when registering");
		}
	}
}

void ObserverEventMap::dropObserver(uint32 eventType, Observer* observer) {
	Locker locker(&observerMutex);

	if (!containsKey(eventType))
		return;

	SortedVector<ManagedReference<Observer*> >* observers = &get(eventType);

	observers->drop(observer);

	if (observers->size() == 0)
		remove(eventType);
}

SortedVector<ManagedReference<Observer*> >* ObserverEventMap::getObservers(uint32 eventType) {
	Locker locker(&observerMutex);

	if (!containsKey(eventType))
		return NULL;

	SortedVector<ManagedReference<Observer*> >* observers = &get(eventType);

	return observers;
}

int ObserverEventMap::getObserverCount(uint32 eventType) {
	Locker locker(&observerMutex);

	if (!containsKey(eventType))
		return NULL;

	SortedVector<ManagedReference<Observer*> >* observers = &get(eventType);//&elementAt(index).getValue();

	return observers->size();
}
