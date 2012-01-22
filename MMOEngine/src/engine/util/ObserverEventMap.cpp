/*
 * ObserverEventType.cpp
 *
 *  Created on: 20/06/2010
 *      Author: victor
 */

#include "ObserverEventMap.h"
#include "Observer.h"

void ObserverEventMap::notifyObservers(uint32 eventType, Observable* observable, ManagedObject* arg1, int64 arg2) {
	/*int index = find(eventType);

	if (index == -1) {
		return;
	}*/
	if (!containsKey(eventType))
		return;

	SortedVector<ManagedReference<Observer*> >* observers = &get(eventType);

	SortedVector<ManagedReference<Observer*> > observersCopy(*observers);

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

	/*int index = find(eventType);

	if (index == -1) {*/
	if (!containsKey(eventType)) {
		SortedVector<ManagedReference<Observer*> > observers;
		observers.put(observer);

		this->put(eventType, observers);
	} else {
		SortedVector<ManagedReference<Observer*> >* observers = &get(eventType);
		observers->setNoDuplicateInsertPlan();

		observers->put(observer);
	}
}

void ObserverEventMap::dropObserver(uint32 eventType, Observer* observer) {
	/*int index = find(eventType);

	if (index == -1)
		return;*/

	if (!containsKey(eventType))
		return;

	SortedVector<ManagedReference<Observer*> >* observers = &get(eventType);

	observers->drop(observer);

	/*if (observers->size() == 0)
		this->remove(index);*/

	if (observers->size() == 0)
		remove(eventType);
}

SortedVector<ManagedReference<Observer*> >* ObserverEventMap::getObservers(uint32 eventType) {
	/*int index = find(eventType);

	if (index == -1)
		return NULL;*/

	if (!containsKey(eventType))
		return NULL;

	SortedVector<ManagedReference<Observer*> >* observers = &get(eventType);

	return observers;
}

int ObserverEventMap::getObserverCount(uint32 eventType) {
	/*int index = find(eventType);

	if (index == -1)
		return 0;*/

	if (!containsKey(eventType))
		return NULL;

	SortedVector<ManagedReference<Observer*> >* observers = &get(eventType);//&elementAt(index).getValue();

	return observers->size();
}
