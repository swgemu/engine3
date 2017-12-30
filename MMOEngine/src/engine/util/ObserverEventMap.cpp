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

	sys::util::Entry<uint32, SortedVector<ManagedReference<Observer*> > >* entry = getEntry(eventType);

	if (entry == nullptr)
		return;

	SortedVector<ManagedReference<Observer*> >* observers = &entry->getValue();

	//SortedVector<ManagedReference<Observer*> > observersCopy(*observers);
	
	SortedVector<Observer*> observersCopy(observers->size(), 50);
	
	for (int i = 0; i < observers->size(); ++i) {
		observersCopy.add(observers->getUnsafe(i).get());
	}
	
	locker.release();

	for (int i = 0; i < observersCopy.size(); ++i) {
		ManagedReference<Observer*> observer = observersCopy.getUnsafe(i);

		int result = observer->notifyObserverEvent(eventType, observable, arg1, arg2);

		if (result == 1) {
			dropObserver(eventType, observer);
		}

	}
}

void ObserverEventMap::registerObserver(uint32 eventType, Observer* observer) {
	if (observer == nullptr) {
		System::out << "registering nullptr OBSERVER" << endl;
		StackTrace::printStackTrace();

		return;
	}

	Locker locker(&observerMutex);

	if (!observer->isDeplyoed())
		observer->deploy();

	sys::util::Entry<uint32, SortedVector<ManagedReference<Observer*> > >* entry = getEntry(eventType);

	if (entry == nullptr) {
		SortedVector<ManagedReference<Observer*> > observers;
		observers.put(observer);

		this->put(eventType, std::move(observers));
	} else {
		SortedVector<ManagedReference<Observer*> >* observers = &entry->getValue();
		observers->setNoDuplicateInsertPlan();

		if (observers->put(observer) == -1) {
//			StackTrace::printStackTrace();
//			Logger::console.warning("duplicate observer when registering");
		}
	}
}

void ObserverEventMap::dropObserver(uint32 eventType, Observer* observer) {
	if (observer == nullptr)
		return;
		
	Locker locker(&observerMutex);

	/*if (!containsKey(eventType))
		return;*/

	sys::util::Entry<uint32, SortedVector<ManagedReference<Observer*> > >* entry = getEntry(eventType);

	if (entry == nullptr)
		return;

	SortedVector<ManagedReference<Observer*> >* observers = &entry->getValue();

	observers->drop(observer);

	if (observers->size() == 0)
		remove(eventType);
}

SortedVector<ManagedReference<Observer*> > ObserverEventMap::getObservers(uint32 eventType) {
	Locker locker(&observerMutex);

	/*if (!containsKey(eventType))
		return nullptr;

	SortedVector<ManagedReference<Observer*> >* observers = &get(eventType); */

	sys::util::Entry<uint32, SortedVector<ManagedReference<Observer*> > >* entry = getEntry(eventType);

	if (entry == nullptr)
		return SortedVector<ManagedReference<Observer*> >();

	SortedVector<ManagedReference<Observer*> > observers(entry->getValue());

	return observers;
}

int ObserverEventMap::getObserverCount(uint32 eventType) const {
	Locker locker(&observerMutex);

	/*if (!containsKey(eventType))
		return nullptr;

	SortedVector<ManagedReference<Observer*> >* observers = &get(eventType);//&elementAt(index).getValue();*/

	sys::util::Entry<uint32, SortedVector<ManagedReference<Observer*> > >* entry = getEntry(eventType);

	if (entry == nullptr)
		return 0;

	SortedVector<ManagedReference<Observer*> >* observers = &entry->getValue();

	return observers->size();
}
