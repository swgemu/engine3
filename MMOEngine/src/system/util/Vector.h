/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef VECTOR_H_
#define VECTOR_H_

#include "../platform.h"

#include "../lang/types.h"

#include "../lang/ArrayIndexOutOfBoundsException.h"

namespace sys {
 namespace util {

   template<class E> class Vector {
   protected:
       E* elementData;

       int elementCapacity, capacityIncrement;
       int elementCount;

   public:
       Vector();
       Vector(int incr);
       Vector(int initsize, int incr);

       virtual ~Vector();

       bool add(const E& element);
       bool add(int index, const E& element);

       void insertElementAt(const E& element, int index);

       E& get(int index);

       E& elementAt(int index);

       E remove(int index);

       bool removeElement(const E& element);

       void removeElementAt(int index);

       void removeAll();

       E set(int index, const E& element);
       void setElementAt(int index, const E& element);

       void clone(Vector<E>& vector);

       Vector<E>& operator=(Vector<E>& vector);

   protected:
       void init(int initsize, int incr);

       void ensureCapacity(int minCapacity, bool copyContent = true);

       void setSize(int newSize, bool copyContent = true);

       inline void createElementAt(const E& o, int index);

       inline void destroyElementAt(int index);

       inline void destroyElements();

   public:
       inline int size() {
           return elementCount;
       }

       int capacity() {
           return elementCapacity;
       }

       inline bool isEmpty() {
           return elementCount == 0;
       }

   };

   template<class E> Vector<E>::Vector() {
       init(10, 5);
   }

   template<class E> Vector<E>::Vector(int incr) {
       init(10, incr);
   }

   template<class E> Vector<E>::Vector(int initsize, int incr) {
       init(initsize, incr);
   }

   template<class E> Vector<E>::~Vector() {
       destroyElements();

       free(elementData);
   }

   template<class E> void Vector<E>::init(int initsize, int incr) {
       elementCapacity = initsize;
       elementData = (E*) malloc(elementCapacity * sizeof(E));

       elementCount = 0;
       capacityIncrement = incr;
   }

   template<class E> bool Vector<E>::add(const E& element) {
       ensureCapacity(elementCount + 1);

       createElementAt(element, elementCount++);
       return true;
   }

   template<class E> bool Vector<E>::add(int index, const E& element) {
       insertElementAt(element, index);
       return true;
   }

   template<class E> void Vector<E>::insertElementAt(const E& element, int index) {
       if (index > elementCount || index < 0)
           throw ArrayIndexOutOfBoundsException(index);

       ensureCapacity(elementCount + 1);

       int numMoved = elementCount - index;
       if (numMoved > 0) {
           E* indexOffset = elementData + index;
           memmove(indexOffset + 1, indexOffset, numMoved * sizeof(E));
       }

       createElementAt(element, index);
       elementCount++;
   }

   template<class E> E& Vector<E>::get(int index) {
       return elementAt(index);
   }

   template<class E> E& Vector<E>::elementAt(int index) {
       if (index >= elementCount || index < 0)
           throw ArrayIndexOutOfBoundsException(index);

       return elementData[index];
   }

   template<class E> E Vector<E>::remove(int index) {
       E oldValue = get(index);

       removeElementAt(index);

       return oldValue;
   }

   template<class E> bool Vector<E>::removeElement(const E& element) {
       for (int i = 0; i < elementCount; ++i) {
           if (elementData[i] == element) {
               remove(i);
               return true;
           }
       }

       return false;
   }

   template<class E> void Vector<E>::removeElementAt(int index) {
       if (index >= elementCount || index < 0)
           throw ArrayIndexOutOfBoundsException(index);

       destroyElementAt(index);

       int numMoved = elementCount - index - 1;
       if (numMoved > 0) {
           E* indexOffset = elementData + index;
           memcpy(indexOffset, indexOffset + 1, numMoved * sizeof(E));
       }

       --elementCount;
   }

   template<class E> void Vector<E>::removeAll() {
       destroyElements();

       free(elementData);

       init(10, 5);
   }

   template<class E> E Vector<E>::set(int index, const E& element) {
       E oldValue = get(index);

       setElementAt(index, element);

       return oldValue;
   }

   template<class E> void Vector<E>::setElementAt(int index, const E& element) {
       if (index >= elementCount || index < 0)
           throw ArrayIndexOutOfBoundsException(index);

       destroyElementAt(index);
       createElementAt(element, index);
   }

   template<class E> void Vector<E>::clone(Vector<E>& vector) {
       vector.removeAll();
       vector.init(elementCapacity, capacityIncrement);

       vector.elementCount = elementCount;

       if (TypeInfo<E>::needConstructor) {
           for (int i = 0; i < elementCount; ++i)
               vector.createElementAt(elementData[i], i);
       } else
           memcpy(vector.elementData, elementData, elementCount * sizeof(E));
   }

   template<class E> Vector<E>& Vector<E>::operator=(Vector<E>& vector) {
       clone(vector);

       return vector;
   }

   template<class E> void Vector<E>::ensureCapacity(int minCapacity, bool copyContent) {
       int oldCapacity = elementCapacity;

       if (minCapacity > oldCapacity) {
           E* oldData = elementData;

           int newCapacity = (capacityIncrement > 0) ?
               (oldCapacity + capacityIncrement) : (oldCapacity * 2);

           if (newCapacity < minCapacity)
               newCapacity = minCapacity;

           if (copyContent) {
        	   elementData = (E*) realloc(elementData, (elementCapacity = newCapacity) * sizeof(E));
           } else {
               elementData = (E*) malloc((elementCapacity = newCapacity) * sizeof(E));
               free(oldData);
           }
       }
   }

   template<class E> void Vector<E>::setSize(int newSize, bool copyContent) {
       if (newSize > elementCount)
           ensureCapacity(newSize, copyContent);

       elementCount = newSize;
   }

  template<class E> void Vector<E>::createElementAt(const E& o, int index) {
       if (TypeInfo<E>::needConstructor)
           new (&(elementData[index])) E(o);
       else
    	   elementData[index] = o;
   }

   template<class E> void Vector<E>::destroyElementAt(int index) {
       if (TypeInfo<E>::needConstructor)
           (&(elementData[index]))->~E();
   }

   template<class E> void Vector<E>::destroyElements() {
       if (TypeInfo<E>::needConstructor) {
           for (int i = 0; i < elementCount; ++i)
               destroyElementAt(i);
       }
   }

 } // namespace util
} // namespace sys

using namespace sys::util;

#endif /*VECTOR_H_*/
