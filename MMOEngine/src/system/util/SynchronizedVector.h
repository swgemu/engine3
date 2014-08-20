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

   template<class E> class SynchronizedVector : public Object {
   protected:
	   mutable ReadWriteLock guard;
	   Vector<E> vector;
   public:
	   SynchronizedVector();

	   SynchronizedVector(const SynchronizedVector<E>& array);

	   SynchronizedVector(const Vector<E>& array);

	   SynchronizedVector<E>& operator=(const SynchronizedVector<E>& array);

	   bool add(const E& element);
	   bool add(int index, const E& element);

	   void addAll(const ArrayList<E>& array);

	   bool contains(const E& element);

	   void insertElementAt(const E& element, int index);

	   E get(int index) const;

	   E elementAt(int index) const;

	   E remove(int index);

	   bool removeElement(const E& element);

	   void removeElementAt(int index);

	   void removeRange(int fromIndex, int toIndex);

	   void removeAll(int newSize = 10, int newIncrement = 5);

	   E set(int index, const E& element);
	   void setElementAt(int index, const E& element);

	   bool toBinaryStream(ObjectOutputStream* stream) {
		   return vector.toBinaryStream(stream);
	   }

	   bool parseFromBinaryStream(ObjectInputStream* stream) {
		   return vector.parseFromBinaryStream(stream);
	   }

	   int size() const {
		   return vector.size();
	   }

	   inline bool isEmpty() const {
		   return vector.size() == 0;
	   }
   };
   template<class E>
   SynchronizedVector<E>::SynchronizedVector() : vector() {

   }

   template<class E>
   SynchronizedVector<E>::SynchronizedVector(const SynchronizedVector<E>& array) : Object(), vector(array.vector) {

   }

   template<class E>
   SynchronizedVector<E>::SynchronizedVector(const Vector<E>& array) : vector(array) {

   }

   template<class E>
   SynchronizedVector<E>& SynchronizedVector<E>::operator=(const SynchronizedVector<E>& array) {
	   Locker locker(&guard);

	   vector.operator=(array);

	   return *this;
   }

   template<class E>
   bool SynchronizedVector<E>::add(const E& element) {
	   Locker locker(&guard);

	   bool ret = vector.add(element);

	   return ret;
   }

   template<class E>
   bool SynchronizedVector<E>::add(int index, const E& element) {
	   Locker locker(&guard);

	   bool ret = vector.add(index, element);

	   return ret;
   }

   template<class E>
   void SynchronizedVector<E>::addAll(const ArrayList<E>& array) {
	   Locker locker(&guard);

	   vector.addAll(array);
   }

   template<class E>
   bool SynchronizedVector<E>::contains(const E& element) {
	   ReadLocker locker(&guard);

	   bool ret = vector.contains(element);

	   return ret;
   }

   template<class E>
   void SynchronizedVector<E>::insertElementAt(const E& element, int index) {
	   Locker locker(&guard);

	   vector.insertElementAt(element, index);
   }

   template<class E>
   E SynchronizedVector<E>::get(int index) const {
	   ReadLocker locker(&guard);

	   E ret = vector.get(index);

	   return ret;
   }

   template<class E>
   E SynchronizedVector<E>::elementAt(int index) const {
	   ReadLocker locker(&guard);

	   E ret = vector.elementAt(index);

	   return ret;
   }

   template<class E>
   E SynchronizedVector<E>::remove(int index) {
	   Locker locker(&guard);

	   E obj = vector.remove(index);

	   return obj;
   }

   template<class E>
   bool SynchronizedVector<E>::removeElement(const E& element) {
	   Locker locker(&guard);

	   bool obj = vector.removeElement(element);

	   return obj;
   }

   template<class E>
   void SynchronizedVector<E>::removeElementAt(int index) {
	   Locker locker(&guard);

	   vector.removeElementAt(index);
   }

   template<class E>
   void SynchronizedVector<E>::removeRange(int fromIndex, int toIndex) {
	   Locker locker(&guard);

	   vector.removeRange(fromIndex, toIndex);
   }

   template<class E>
   void SynchronizedVector<E>::removeAll(int newSize, int newIncrement) {
	   Locker locker(&guard);

	   vector.removeAll(newSize, newIncrement);
   }

   template<class E>
   E SynchronizedVector<E>::set(int index, const E& element) {
	   Locker locker(&guard);

	   E obj = vector.set(index, element);

	   return obj;
   }

   template<class E>
   void SynchronizedVector<E>::setElementAt(int index, const E& element) {
	   Locker locker(&guard);

	   vector.setElementAt(index, element);
   }

 }
}

using namespace sys::util;

#endif /* SYNCHRONIZEDVECTOR_H_ */
