/*
 * CloseObjectsVector.h
 *
 *  Created on: 11/06/2012
 *      Author: victor
 */

#ifndef CLOSEOBJECTSVECTOR_H_
#define CLOSEOBJECTSVECTOR_H_

#include "system/util/SortedVector.h"

namespace engine {
 namespace util {
  namespace u3d {
   class QuadTreeEntry;
  }
 }
}

using namespace engine::util::u3d;

class CloseObjectsVector : public SortedVector<ManagedReference<QuadTreeEntry*> > {
	mutable ReadWriteLock mutex;
	VectorMap<uint32, SortedVector<QuadTreeEntry*>> messageReceivers;

public:
	CloseObjectsVector();

	ManagedReference<QuadTreeEntry*> remove(int index) {
		Locker locker(&mutex);

		return SortedVector<ManagedReference<QuadTreeEntry*> >::remove(index);
	}

	bool contains(const ManagedReference<QuadTreeEntry*>& o) const {
		ReadLocker locker(&mutex);

		bool ret = find(o) != -1;

		return ret;
	}

	void removeAll(int newSize = 10, int newIncrement = 5) {
		Locker locker(&mutex);

		SortedVector<ManagedReference<QuadTreeEntry*> >::removeAll(newSize, newIncrement);

		messageReceivers.removeAll(newSize, newIncrement);
	}

	bool drop(const ManagedReference<QuadTreeEntry*>& o);

	void safeCopyTo(Vector<QuadTreeEntry*>& vec) const {
		vec.removeAll(size(), size() / 2);

		ReadLocker locker(&mutex);

		for (int i = 0; i < size(); ++i) {
			vec.add(get(i).get());
		}
	}

	void safeCopyReceiversTo(Vector<QuadTreeEntry*>& vec, uint32 receiverType) const;

	void safeCopyTo(Vector<ManagedReference<QuadTreeEntry*> >& vec) const {
		vec.removeAll(size(), size() / 2);

		ReadLocker locker(&mutex);

		vec.addAll(*this);
	}

	SortedVector<ManagedReference<QuadTreeEntry*> > getSafeCopy() const {
		ReadLocker locker(&mutex);

		return SortedVector<ManagedReference<QuadTreeEntry*> >(*this);
	}

	int put(const ManagedReference<QuadTreeEntry*>& o);
	int find(const ManagedReference<QuadTreeEntry*>& o) const;
};

#endif /* CLOSEOBJECTSVECTOR_H_ */
