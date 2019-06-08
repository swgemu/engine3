/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef MANAGEDSORTEDVECTOR_H_
#define MANAGEDSORTEDVECTOR_H_

#include "system/lang.h"

#include "engine/core/ManagedReference.h"

namespace engine {
  namespace core {
    namespace util {

	template<class O> class ManagedSortedVector : public Vector<engine::core::ManagedReference<O> > {
		int insertPlan;

		virtual int compare(O* obj1, O* obj2) {
			return obj1->compareTo(obj2);
		}

	public:
		static const int ALLOW_DUPLICATE = 1;

		static const int NO_DUPLICATE = 2;

	public:
		ManagedSortedVector();
		ManagedSortedVector(int initsize, int incr);

		int put(O* obj);

		bool contains(O* obj);

		int find(O* obj);

		bool drop(O* obj);

		inline void setInsertPlan(int plan) {
			insertPlan = plan;
		}

	};

	template<class O> ManagedSortedVector<O>::ManagedSortedVector() : Vector<ManagedReference<O> >() {
		insertPlan = ALLOW_DUPLICATE;
	}

	template<class O> ManagedSortedVector<O>::ManagedSortedVector(int initsize, int incr)
			: Vector<ManagedReference<O> >(initsize, incr) {
		insertPlan = ALLOW_DUPLICATE;
	}

	template<class O> int ManagedSortedVector<O>::put(O* obj) {
		int m = 0, l = 0;
		int r = Vector<ManagedReference<O> >::elementCount - 1;

    	while (l <= r) {
        	//m = (l + r) / 2;
			m = ((unsigned int)l + (unsigned int)r) >> 1;

        	O* temp = Vector<ManagedReference<O> >::elementData[m];
        	int cmp = compare(temp, obj);

        	if (cmp == 0) {
        		if (insertPlan == ALLOW_DUPLICATE)
        			Vector<ManagedReference<O> >::add(++m, obj);
        		else
					return -1;

            	return m;
        	} else if (cmp > 0)
            	l = m + 1;
        	else
            	r = m - 1;
    	}

	    if (r == m)
   		    m++;

	    Vector<ManagedReference<O> >::add(m, obj);

    	return m;
	}

	template<class O> bool ManagedSortedVector<O>::contains(O* obj) {
		return find(obj) != -1;
	}

	template<class O> int ManagedSortedVector<O>::find(O* obj) {
	    int l = 0, r = Vector<ManagedReference<O> >::elementCount - 1;
	    int m = 0, cmp = 0;

	    while (l <= r) {
        	//m = (l + r) / 2;
			m = ((unsigned int)l + (unsigned int)r) >> 1;

        	O* temp = Vector<ManagedReference<O> >::elementData[m];
        	int cmp = compare(temp, obj);

        	if (cmp == 0)
            	return m;
        	else if (cmp > 0)
	            l = m + 1;
        	else
	            r = m - 1;
	    }

    	return -1;
	}

	template<class O> bool ManagedSortedVector<O>::drop(O* obj) {
		int index = find(obj);
		if (index == -1)
			return false;

		O* oldValue = Vector<ManagedReference<O> >::elementData[index];

		Vector<ManagedReference<O> >::remove(index);
		return true;
	}

    } // namespace util
  } // namespace core
} // namespace engine

using namespace engine::core::util;

#endif /*MANAGEDSORTEDVECTOR_H_*/
