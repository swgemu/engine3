/*
 * SynchronizedVector.h
 *
 *  Created on: 20/12/2013
 *      Author: victor
 */

#ifndef SYNCHRONIZEDVECTOR_H_
#define SYNCHRONIZEDVECTOR_H_

#include "Vector.h"

namespace sys {
 namespace util {

   template<class E> class SynchronizedVector : public Vector<E> {
   protected:
	   mutable ReadWriteLock guard;
   public:
	   SynchronizedVector<E>& operator=(const SynchronizedVector<E>& array);

	   bool add(const E& element);
	   bool add(int index, const E& element);

	   void addAll(const ArrayList<E>& array);

	   bool contains(const E& element);

	   void insertElementAt(const E& element, int index);

	   E& get(int index) const;

	   E& elementAt(int index) const;

	   E remove(int index);

	   bool removeElement(const E& element);

	   void removeElementAt(int index);

	   void removeRange(int fromIndex, int toIndex);

	   void removeAll(int newSize = 10, int newIncrement = 5);

	   E set(int index, const E& element);
	   void setElementAt(int index, const E& element);
   };

   template<class E>
   SynchronizedVector<E>& SynchronizedVector<E>::operator=(const SynchronizedVector<E>& array) {
	   Locker locker(&guard);

	   Vector<E>::operator=(array);

	   return *this;
   }

   template<class E>
   bool SynchronizedVector<E>::add(const E& element) {
	   Locker locker(&guard);

	   return Vector<E>::add(element);
   }

   template<class E>
   bool SynchronizedVector<E>::add(int index, const E& element) {
	   Locker locker(&guard);

	   return Vector<E>::add(index, element);
   }

   template<class E>
   void SynchronizedVector<E>::addAll(const ArrayList<E>& array) {
	   Locker locker(&guard);

	   Vector<E>::addAll(array);
   }

   template<class E>
   bool SynchronizedVector<E>::contains(const E& element) {
	   ReadLocker locker(&guard);

	   return Vector<E>::contains(element);
   }

   template<class E>
   void SynchronizedVector<E>::insertElementAt(const E& element, int index) {
	   Locker locker(&guard);

	   Vector<E>::insertElementAt(element, index);
   }

   template<class E>
   E& SynchronizedVector<E>::get(int index) const {
	   ReadLocker locker(&guard);

	   return Vector<E>::get(index);
   }

   template<class E>
   E& SynchronizedVector<E>::elementAt(int index) const {
	   ReadLocker locker(&guard);

	   return Vector<E>::elementAt(index);
   }

   template<class E>
   E SynchronizedVector<E>::remove(int index) {
	   Locker locker(&guard);

	   return Vector<E>::remove(index);
   }

   template<class E>
   bool SynchronizedVector<E>::removeElement(const E& element) {
	   Locker locker(&guard);

	   return Vector<E>::removeElement(element);
   }

   template<class E>
   void SynchronizedVector<E>::removeElementAt(int index) {
	   Locker locker(&guard);

	   Vector<E>::removeElementAt(index);
   }

   template<class E>
   void SynchronizedVector<E>::removeRange(int fromIndex, int toIndex) {
	   Locker locker(&guard);

	   Vector<E>::removeRange(fromIndex, toIndex);
   }

   template<class E>
   void SynchronizedVector<E>::removeAll(int newSize, int newIncrement) {
	   Locker locker(&guard);

	   Vector<E>::removeAll(newSize, newIncrement);
   }

   template<class E>
   E SynchronizedVector<E>::set(int index, const E& element) {
	   Locker locker(&guard);

	   return Vector<E>::set(index, element);
   }

   template<class E>
   void SynchronizedVector<E>::setElementAt(int index, const E& element) {
	   Locker locker(&guard);

	   Vector<E>::setElementAt(index, element);
   }

 }
}

using namespace sys::util;

#endif /* SYNCHRONIZEDVECTOR_H_ */
