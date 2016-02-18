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

public:
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
	}

	bool drop(const ManagedReference<QuadTreeEntry*>& o) {
		Locker locker(&mutex);

		return SortedVector<ManagedReference<QuadTreeEntry*> >::drop(o);
	}

	void safeCopyTo(Vector<QuadTreeEntry*>& vec) {
		ReadLocker locker(&mutex);

		for (int i = 0; i < size(); ++i) {
			vec.add(get(i).get());
		}
	}

	void safeCopyTo(Vector<ManagedReference<QuadTreeEntry*> >& vec) {
		ReadLocker locker(&mutex);

		vec.addAll(*this);
	}

	SortedVector<ManagedReference<QuadTreeEntry*> > getSafeCopy() {
		ReadLocker locker(&mutex);

		return SortedVector<ManagedReference<QuadTreeEntry*> >(*this);
	}

	int put(const ManagedReference<QuadTreeEntry*>& o) {
		Locker locker(&mutex);

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

	int find(const ManagedReference<QuadTreeEntry*>& o) const {
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
};

#endif /* CLOSEOBJECTSVECTOR_H_ */
