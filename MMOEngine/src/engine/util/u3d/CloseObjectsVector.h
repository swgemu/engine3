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

class CloseObjectsVector : public Object {
	mutable ReadWriteLock mutex;
	SortedVector<Reference<QuadTreeEntry*> > objects;

	VectorMap<uint32, SortedVector<QuadTreeEntry*> > messageReceivers;

#ifdef CXX11_COMPILER
	static_assert(sizeof(QuadTreeEntry*) == sizeof(Reference<QuadTreeEntry*>), "Reference<> size is not the size of a pointer");
#endif

protected:
	void dropReceiver(QuadTreeEntry* entry);
	void putReceiver(QuadTreeEntry* entry, uint32 receiverTypes);

public:
	CloseObjectsVector();

	Reference<QuadTreeEntry*> remove(int index);

	bool contains(const Reference<QuadTreeEntry*>& o) const;

	void removeAll(int newSize = 10, int newIncrement = 5);

	bool drop(const Reference<QuadTreeEntry*>& o);

	void safeCopyTo(Vector<QuadTreeEntry*>& vec) const;

	void safeCopyReceiversTo(Vector<QuadTreeEntry*>& vec, uint32 receiverType) const;
	void safeCopyReceiversTo(Vector<ManagedReference<QuadTreeEntry*> >& vec, uint32 receiverType) const;

	void safeCopyTo(Vector<ManagedReference<QuadTreeEntry*> >& vec) const;

	SortedVector<ManagedReference<QuadTreeEntry*> > getSafeCopy() const;

	Reference<QuadTreeEntry*> get(int idx) const;

	int put(const Reference<QuadTreeEntry*>& o);
#ifdef CXX11_COMPILER
	int put(Reference<QuadTreeEntry*>&& o);
#endif

	inline int size() const {
		return objects.size();
	}

	void setNoDuplicateInsertPlan() {
		objects.setNoDuplicateInsertPlan();
	}
};

#endif /* CLOSEOBJECTSVECTOR_H_ */
