/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * SynchronizedSynchronizedSortedVector.h
 *
 *  Created on: 20/12/2013
 *      Author: victor
 */

#ifndef SYNCHRONIZEDSORTEDVECTOR_H_
#define SYNCHRONIZEDSORTEDVECTOR_H_

#include "SynchronizedVector.h"

namespace sys {
 namespace util {

 	 template<class E>
 	 class SynchronizedSortedVector : public SynchronizedVector<E> {
 	 protected:
 		 int insertPlan;

 		 virtual int compare(const E& o1, const E& o2) const {
 			 return TypeInfo<E>::compare(o1, o2);
 		 }

 	 public:
 		 static const int ALLOW_DUPLICATE = 1;

 		 static const int NO_DUPLICATE = 2;

 		 static const int ALLOW_OVERWRITE = 3;

 	 public:
 		 SynchronizedSortedVector();
 		 SynchronizedSortedVector(int initsize, int incr);
 		 SynchronizedSortedVector(const SynchronizedSortedVector<E>& vector);

 		 SynchronizedSortedVector<E>& operator=(const SynchronizedSortedVector<E>& vector);

 		 virtual int put(const E& o);

 		 virtual int find(const E& o) const;

 		 bool contains(const E& o) const;

 		 int lowerBound(const E& o) const;
 		 int upperBound(const E& o) const;

 		 virtual bool drop(const E& o);

 		 Object* clone();

 		 inline void setInsertPlan(int plan) {
 			 insertPlan = plan;
 		 }

 		 inline void setNoDuplicateInsertPlan() {
 			 insertPlan = NO_DUPLICATE;
 		 }

 		 inline void setAllowDuplicateInsertPlan() {
 			 insertPlan = ALLOW_DUPLICATE;
 		 }

 		 inline void setAllowOverwriteInsertPlan() {
 			 insertPlan = ALLOW_OVERWRITE;
 		 }

 		 inline int getInsertPlan() const {
 			 return insertPlan;
 		 }

 	 };

 	 template<class E> SynchronizedSortedVector<E>::SynchronizedSortedVector() : SynchronizedVector<E>() {
 		 insertPlan = ALLOW_DUPLICATE;
 	 }

 	 template<class E> SynchronizedSortedVector<E>::SynchronizedSortedVector(int initsize, int incr) : SynchronizedVector<E>(initsize, incr) {
 		 insertPlan = ALLOW_DUPLICATE;
 	 }

 	 template<class E> SynchronizedSortedVector<E>::SynchronizedSortedVector(const SynchronizedSortedVector<E>& vector) : SynchronizedVector<E>(vector) {
 		 insertPlan = vector.insertPlan;
 	 }

 	 template<class E> SynchronizedSortedVector<E>& SynchronizedSortedVector<E>::operator=(const SynchronizedSortedVector<E>& vector) {
 		 Locker locker(&this->guard);

 		 if (this == &vector)
 			 return *this;

 		 insertPlan = vector.getInsertPlan();

 		 SynchronizedVector<E>::operator=(vector);

#ifdef ATOMIC_SYNC_VECTOR_COUNT
		 SynchronizedVector<E>::count = this->vector.size();
#endif

 		 return *this;
 	 }

 	 template<class E> int SynchronizedSortedVector<E>::lowerBound(const E& o) const {
 		 ReadLocker locker(&this->guard);

 		 if (SynchronizedVector<E>::vector.size() == 0)
 			 return -1;

 		 int l = 0, r = SynchronizedVector<E>::vector.elementCount - 1;
 		 int m = 0, cmp = 0;

 		 while (l <= r) {
 			 //m = (l + r) / 2;
			 m = ((unsigned int)l + (unsigned int)r) >> 1;

 			 E& obj = SynchronizedVector<E>::vector.elementData[m];
 			 cmp = compare(obj, o);

 			 if (cmp > 0) {
 				 l = m + 1;

 				 if (r < l)
 					 return m < SynchronizedVector<E>::vector.size() - 1 ? m + 1 : -1;
 			 } else {
 				 r = m - 1;

 				 if (r < l)
 					 return m;
 			 }
 		 }

 		 return -1;
 	 }

 	 template<class E> int SynchronizedSortedVector<E>::upperBound(const E& o) const {
 		 ReadLocker locker(&this->guard);

 		 if (SynchronizedVector<E>::vector.size() == 0)
 			 return -1;

 		 int l = 0, r = SynchronizedVector<E>::vector.elementCount - 1;
 		 int m = 0, cmp = 0;

 		 while (l <= r) {
 			 //m = (l + r) / 2;
			 m = ((unsigned int)l + (unsigned int)r) >> 1;

 			 E& obj = SynchronizedVector<E>::vector.elementData[m];
 			 cmp = compare(obj, o);

 			 if (cmp == 0 || cmp > 0) {
 				 l = m + 1;

 				 if (r < l)
 					 return m < SynchronizedVector<E>::size() - 1 ? m + 1 : -1;
 			 } else {
 				 r = m - 1;

 				 if (r < l)
 					 return m;
 			 }
 		 }

 		 return -1;
 	 }

 	 template<class E> int SynchronizedSortedVector<E>::put(const E& o) {
 		 Locker locker(&this->guard);

 		 int m = 0, l = 0;
 		 int r = SynchronizedVector<E>::vector.size() - 1;

 		 while (l <= r) {
 			 //m = (l + r) / 2;
			 m = ((unsigned int)l + (unsigned int)r) >> 1;

 			 E& obj = SynchronizedVector<E>::vector.elementData[m];
 			 int cmp = compare(obj, o);

 			 if (cmp == 0) {
 				 switch (insertPlan) {
 				 case ALLOW_DUPLICATE:
 					 SynchronizedVector<E>::vector.add(++m, o);
 					 break;
 				 case ALLOW_OVERWRITE:
 					 SynchronizedVector<E>::vector.set(m, o);
 					 break;
 				 default:
 					 return -1;
 				 }
#ifdef ATOMIC_SYNC_VECTOR_COUNT
				 SynchronizedVector<E>::count = this->vector.size();
#endif

 				 return m;
 			 } else if (cmp > 0)
 				 l = m + 1;
 			 else
 				 r = m - 1;
 		 }

 		 if (r == m)
 			 m++;

 		 SynchronizedVector<E>::vector.add(m, o);

#ifdef ATOMIC_SYNC_VECTOR_COUNT
		 SynchronizedVector<E>::count = this->vector.size();
#endif

 		 return m;
 	 }

 	 template<class E> bool SynchronizedSortedVector<E>::contains(const E& o) const {
 		 return find(o) != -1;
 	 }

 	 template<class E> int SynchronizedSortedVector<E>::find(const E& o) const {
 		 ReadLocker locker(&this->guard);

 		 if (SynchronizedVector<E>::vector.size() == 0)
 			 return -1;

 		 int l = 0, r = SynchronizedVector<E>::vector.elementCount - 1;
 		 int m = 0, cmp = 0;

 		 while (l <= r) {
 			 //m = (l + r) / 2;
			 m = ((unsigned int)l + (unsigned int)r) >> 1;

 			 E& obj = SynchronizedVector<E>::vector.elementData[m];
 			 cmp = compare(obj, o);

 			 if (cmp == 0)
 				 return m;
 			 else if (cmp > 0)
 				 l = m + 1;
 			 else
 				 r = m - 1;
 		 }

 		 return -1;
 	 }

 	 template<class E> Object* SynchronizedSortedVector<E>::clone() {
 		 return new SynchronizedSortedVector<E>(*this);
 	 }

 	 template<class E> bool SynchronizedSortedVector<E>::drop(const E& o) {
 		 Locker locker(&this->guard);

 		 int index = find(o);
 		 if (index == -1)
 			 return false;

 		 //E& oldValue = Vector<E>::elementData[index];

 		 SynchronizedVector<E>::vector.remove(index);

#ifdef ATOMIC_SYNC_VECTOR_COUNT
		 SynchronizedVector<E>::count = this->vector.size();
#endif

 		 return true;
 	 }
 }
}

using namespace sys::util;

#endif /* SYNCHRONIZEDSORTEDVECTOR_H_ */
