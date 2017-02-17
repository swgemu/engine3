//
// Created by Victor Popovici on 7/2/17.
//

#include "engine/core/ManagedReference.h"

#include "CloseObjectsVector.h"
#include "QuadTreeEntry.h"

#define MAX_COV_RECEIVER_TYPES 4

CloseObjectsVector::CloseObjectsVector() : SortedVector<ManagedReference<QuadTreeEntry*> >(), messageReceivers() {
	setNoDuplicateInsertPlan();

	messageReceivers.setNoDuplicateInsertPlan();
}

ManagedReference<QuadTreeEntry*> CloseObjectsVector::remove(int index) {
	Locker locker(&mutex);

	const auto& ref = SortedVector<ManagedReference<QuadTreeEntry*> >::get(index);

	uint32 receiverTypes = ref->registerToCloseObjectsReceivers();

	if (receiverTypes) {
		for (int i = 0; i < MAX_COV_RECEIVER_TYPES; ++i) {
			uint32 type = 1 << i;

			if (receiverTypes & type) {
				int idx = messageReceivers.find(type);

				if (idx != -1) {
					auto& receivers = messageReceivers.elementAt(idx).getValue();

					receivers.drop(ref.get());
				}
			}
		}
	}

	return SortedVector<ManagedReference<QuadTreeEntry*> >::remove(index);
}

bool CloseObjectsVector::drop(const ManagedReference<QuadTreeEntry*>& o) {
	uint32 receiverTypes = o->registerToCloseObjectsReceivers();

	Locker locker(&mutex);

	if (receiverTypes) {
		for (int i = 0; i < MAX_COV_RECEIVER_TYPES; ++i) {
			uint32 type = 1 << i;

			if (receiverTypes & type) {
				int idx = messageReceivers.find(type);

				if (idx != -1) {
					auto& receivers = messageReceivers.elementAt(idx).getValue();

					receivers.drop(o.get());
				}
			}
		}
	}

	return SortedVector<ManagedReference<QuadTreeEntry*> >::drop(o);
}

void CloseObjectsVector::safeCopyReceiversTo(Vector<QuadTreeEntry*>& vec, uint32 receiverType) const {
	ReadLocker locker(&mutex);

	int i = messageReceivers.find(receiverType);

	if (i != -1) {
		const auto& receivers = messageReceivers.elementAt(i).getValue();

		vec.removeAll(receivers.size(), receivers.size() / 2);

		vec.addAll(receivers);
	}
}

void CloseObjectsVector::safeCopyReceiversTo(Vector<ManagedReference<QuadTreeEntry*> >& vec, uint32 receiverType) const {
	ReadLocker locker(&mutex);

	int i = messageReceivers.find(receiverType);

	if (i != -1) {
		const auto& receivers = messageReceivers.elementAt(i).getValue();

		vec.removeAll(receivers.size(), receivers.size() / 2);

		for (int i = 0; i < receivers.size(); ++i)
			vec.add(receivers.get(i));
	}
}

int CloseObjectsVector::put(const ManagedReference<QuadTreeEntry*>& o) {
	uint32 receiverTypes = o->registerToCloseObjectsReceivers();

	Locker locker(&mutex);

	if (receiverTypes) {
		for (int i = 0; i < MAX_COV_RECEIVER_TYPES; ++i) {
			uint32 type = 1 << i;

			if (receiverTypes & type) {
				int idx = messageReceivers.find(type);

				if (idx != -1) {
					auto& receivers = messageReceivers.elementAt(idx).getValue();

					receivers.put(o.get());
				} else {
					SortedVector<QuadTreeEntry*> vec;
					vec.setNoDuplicateInsertPlan();

					vec.put(o);

					messageReceivers.put(type, std::move(vec));
				}
			}
		}
	}

	int m = 0, l = 0;
	int r = Vector<ManagedReference<QuadTreeEntry*> >::elementCount - 1;

	while (l <= r) {
		//m = (l + r) / 2;
		m = ((unsigned int)l + (unsigned int)r) >> 1;

		/*E& obj = Vector<E>::elementData[m];
		int cmp = compare(obj, o);*/

		int cmp = Vector<ManagedReference<QuadTreeEntry*> >::elementData[m].compareTo(o);

		if (cmp == 0) {
			switch (insertPlan) {
				case ALLOW_DUPLICATE:
					Vector<ManagedReference<QuadTreeEntry*> >::add(++m, o);
					break;
				case ALLOW_OVERWRITE:
					Vector<ManagedReference<QuadTreeEntry*> >::set(m, o);
					break;
				default:
					return -1;
			}

			return m;
		} else if (cmp > 0)
			l = m + 1;
		else
			r = m - 1;
	}

	if (r == m)
		m++;

	Vector<ManagedReference<QuadTreeEntry*> >::add(m, o);

	return m;
}

int CloseObjectsVector::find(const ManagedReference<QuadTreeEntry*>& o) const {
	if (ArrayList<ManagedReference<QuadTreeEntry*> >::size() == 0)
		return -1;

	int l = 0, r = Vector<ManagedReference<QuadTreeEntry*> >::elementCount - 1;
	int m = 0, cmp = 0;

	while (l <= r) {
		//m = (l + r) / 2;
		m = ((unsigned int)l + (unsigned int)r) >> 1;

		cmp = Vector<ManagedReference<QuadTreeEntry*> >::elementData[m].compareTo(o);

		if (cmp == 0)
			return m;
		else if (cmp > 0)
			l = m + 1;
		else
			r = m - 1;
	}

	return -1;
}