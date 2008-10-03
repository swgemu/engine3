/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef SORTEDVECTOR_H_
#define SORTEDVECTOR_H_

#include "Vector.h"

namespace sys {
  namespace util {

	template<class E> class SortedVector : public Vector<E> {
		int insertPlan;
	
		virtual int compare(E& o1, E& o2) {
			return o1->compareTo(o2);
		}
		
	public:
		static const int ALLOW_DUPLICATE = 1;
	
		static const int NO_DUPLICATE = 2;
	
	public:
		SortedVector();
		SortedVector(int initsize, int incr);
	
		int put(E& o);
		
		bool contains(E& o);
		
		int find(E& o);
		
		bool drop(E& o);

		inline void setInsertPlan(int plan) {
			insertPlan = plan;
		}

	};

	template<class E> SortedVector<E>::SortedVector() : Vector<E>() {
		insertPlan = ALLOW_DUPLICATE;
	}

	template<class E> SortedVector<E>::SortedVector(int initsize, int incr) : Vector<E>(initsize, incr) {
		insertPlan = ALLOW_DUPLICATE;
	}

	template<class E> int SortedVector<E>::put(E& o) {
		int m = 0, l = 0; 
		int r = Vector<E>::elementCount - 1;

    	while (l <= r) {
        	m = (l + r) / 2;
        	
        	E& obj = Vector<E>::elementData[m];
        	int cmp = compare(obj, o);

        	if (cmp == 0) {
        		if (insertPlan == ALLOW_DUPLICATE)
        			Vector<E>::add(++m, o);
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

		Vector<E>::add(m, o);

    	return m;
	}
	
	template<class E> bool SortedVector<E>::contains(E& o) {
		return find(o) != -1;
	}
	
	template<class E> int SortedVector<E>::find(E& o) {
	    int l = 0, r = Vector<E>::elementCount - 1;
	    int m = 0, cmp = 0;

	    while (l <= r) {
        	m = (l + r) / 2;

        	E& obj = Vector<E>::elementData[m];
        	int cmp = compare(obj, o);

        	if (cmp == 0)
            	return m;
        	else if (cmp > 0)
	            l = m + 1;
        	else
	            r = m - 1;
	    }

    	return -1;
	}
	
	template<class E> bool SortedVector<E>::drop(E& o) {
		int index = find(o);
		if (index == -1)
			return false;
		
		E& oldValue = Vector<E>::elementData[index];
		
		Vector<E>::remove(index);
		return true;
	}

  } // namespace util
} // namespace sys

#endif /*SORTEDVECTOR_H_*/
