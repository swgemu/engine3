/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ARRAYLIST_H_
#define ARRAYLIST_H_

#include "system/platform.h"

#include "system/lang/String.h"

#include "system/lang/ArrayIndexOutOfBoundsException.h"
#include "system/lang/IllegalArgumentException.h"

#include "system/lang/types.h"

#include "system/lang/Integer.h"

namespace sys {
 namespace util {

   class ArrayCloneCounter {
   public:
	   static volatile int totalCount;
   };

   template<class E> class ArrayList {
   protected:
       E* elementData;

       int elementCapacity, capacityIncrement;
       int elementCount;

   public:
       ArrayList();
       ArrayList(int incr);
       ArrayList(int initsize, int incr);
       ArrayList(const ArrayList<E>& array);

#ifdef CXX11_COMPILER
       ArrayList(ArrayList<E>&& array);
#endif

       ArrayList<E>& operator=(const ArrayList<E>& array);

#ifdef CXX11_COMPILER
       ArrayList<E>& operator=(ArrayList<E>&& array);
#endif

       virtual ~ArrayList();

       bool add(const E& element);
       bool add(int index, const E& element);

       void addAll(const ArrayList<E>& array);

       bool contains(const E& element);

       void insertElementAt(const E& element, int index);

       virtual E& get(int index) const;

       E& elementAt(int index) const;

       virtual E remove(int index);

       bool removeElement(const E& element);

       void removeElementAt(int index);

       virtual void removeRange(int fromIndex, int toIndex);

       virtual void removeAll(int newSize = 10, int newIncrement = 5);

       E set(int index, const E& element);
       void setElementAt(int index, const E& element);

       void clone(ArrayList<E>& array) const ;

   protected:
       void init(int initsize, int incr);

       void ensureCapacity(int minCapacity, bool copyContent = true);

       void setSize(int newSize, bool copyContent = true);

       inline void createElementAt(const E& o, int index);

       inline void destroyElementAt(int index);

       inline void destroyElementRange(int fromIndex, int toIndex);

       inline void destroyElements();

   public:
       inline int size() const {
           return elementCount;
       }

       int capacity() const {
           return elementCapacity;
       }

       inline bool isEmpty() const {
           return elementCount == 0;
       }

   };

   template<class E> ArrayList<E>::ArrayList() {
       init(1, 0);
   }

   template<class E> ArrayList<E>::ArrayList(int incr) {
       init(1, incr);
   }

   template<class E> ArrayList<E>::ArrayList(int initsize, int incr) {
       init(initsize, incr);
   }

   template<class E> ArrayList<E>::ArrayList(const ArrayList<E>& array) {
	   //__sync_add_and_fetch(&ArrayCloneCounter::totalCount, 1);

	   init(array.elementCapacity, array.capacityIncrement);

	   elementCount = array.elementCount;

	   if (TypeInfo<E>::needConstructor) {
		   for (int i = 0; i < elementCount; ++i)
			   createElementAt(array.elementData[i], i);
	   } else
		   memcpy((void*)elementData, (void*)array.elementData, elementCount * sizeof(E));
   }

#ifdef CXX11_COMPILER
   template<class E> ArrayList<E>::ArrayList(ArrayList<E>&& array) {
	   elementData = array.elementData;

	   elementCapacity = array.elementCapacity;

	   capacityIncrement = array.capacityIncrement;

	   elementCount = array.elementCount;

	   array.elementData = NULL;
	   array.elementCount = 0;
	   array.capacityIncrement = 0;
   }
#endif

   template<class E> ArrayList<E>& ArrayList<E>::operator=(const ArrayList<E>& array) {
	   if (this == &array)
		   return *this;

	   array.clone(*this);

	   return *this;
   }

#ifdef CXX11_COMPILER
   template<class E> ArrayList<E>& ArrayList<E>::operator=(ArrayList<E>&& array) {
   	   if (this == &array)
   		   return *this;

   	   if (elementData != NULL) {
   		   destroyElements();

   		   free(elementData);
   	   }

   	   elementData = array.elementData;

   	   elementCapacity = array.elementCapacity;

   	   capacityIncrement = array.capacityIncrement;

   	   elementCount = array.elementCount;

   	   array.elementData = NULL;
   	   array.elementCount = 0;
   	   array.capacityIncrement = 0;

   	   return *this;
      }
#endif

   template<class E> ArrayList<E>::~ArrayList() {
	   if (elementData != NULL) {
		   destroyElements();

       	   free(elementData);
	   }
   }

   template<class E> void ArrayList<E>::init(int initsize, int incr) {
       elementCapacity = MAX(1, initsize);
       elementData = (E*) malloc(elementCapacity * sizeof(E));

       elementCount = 0;
       capacityIncrement = incr;
   }

   template<class E> bool ArrayList<E>::add(const E& element) {
       ensureCapacity(elementCount + 1);

       createElementAt(element, elementCount++);
       return true;
   }

   template<class E> bool ArrayList<E>::add(int index, const E& element) {
       insertElementAt(element, index);
       return true;
   }

   template<class E> void ArrayList<E>::addAll(const ArrayList<E>& array) {
	   for (int i= 0; i < array.size(); ++i) {
		   const E& element = array.get(i);

		   add(element);
	   }
   }

   template<class E> bool ArrayList<E>::contains(const E& element) {
	   for (int i= 0; i < size(); ++i) {
		   if (element == get(i))
			   return true;
	   }

	   return false;
   }

   template<class E> void ArrayList<E>::insertElementAt(const E& element, int index) {
       if (index > elementCount || index < 0)
           throw ArrayIndexOutOfBoundsException(index);

       ensureCapacity(elementCount + 1);

       int numMoved = elementCount - index;
       if (numMoved > 0) {
           E* indexOffset = elementData + index;
           memmove((void*)(indexOffset + 1), (void*)indexOffset, numMoved * sizeof(E));
       }

       createElementAt(element, index);
       elementCount++;
   }

   template<class E> E& ArrayList<E>::get(int index) const {
       return elementAt(index);
   }

   template<class E> E& ArrayList<E>::elementAt(int index) const {
       if (index >= elementCount || index < 0)
           throw ArrayIndexOutOfBoundsException(index);

       return elementData[index];
   }

   template<class E> E ArrayList<E>::remove(int index) {
       E oldValue = get(index);

       removeElementAt(index);

       return oldValue;
   }

   template<class E> bool ArrayList<E>::removeElement(const E& element) {
       for (int i = 0; i < elementCount; ++i) {
           if (elementData[i] == element) {
               remove(i);
               return true;
           }
       }

       return false;
   }

   template<class E> void ArrayList<E>::removeElementAt(int index) {
       if (index >= elementCount || index < 0)
           throw ArrayIndexOutOfBoundsException(index);

       destroyElementAt(index);

       int numMoved = elementCount - index - 1;
       if (numMoved > 0) {
           E* indexOffset = elementData + index;
           memmove((void*)indexOffset, (void*)(indexOffset + 1), numMoved * sizeof(E));
       }

       --elementCount;
   }

   template<class E> void ArrayList<E>::removeAll(int newSize, int newIncrement) {
       destroyElements();

       free(elementData);

       init(newSize, newIncrement);
   }

   template<class E> E ArrayList<E>::set(int index, const E& element) {
       E oldValue = get(index);

       setElementAt(index, element);

       return oldValue;
   }

   template<class E> void ArrayList<E>::setElementAt(int index, const E& element) {
       if (index >= elementCount || index < 0)
           throw ArrayIndexOutOfBoundsException(index);

       destroyElementAt(index);
       createElementAt(element, index);
   }

   template<class E> void ArrayList<E>::clone(ArrayList<E>& array) const {
       array.removeAll(elementCapacity, capacityIncrement);

       array.elementCount = elementCount;

       if (TypeInfo<E>::needConstructor) {
           for (int i = 0; i < elementCount; ++i)
               array.createElementAt(elementData[i], i);
       } else
           memcpy((void*)array.elementData, (void*)elementData, elementCount * sizeof(E));
   }

   template<class E> void ArrayList<E>::ensureCapacity(int minCapacity, bool copyContent) {
       int oldCapacity = elementCapacity;

       if (minCapacity > oldCapacity) {
    	   //assert(capacityIncrement != 0);

           E* oldData = elementData;

           int newCapacity = (capacityIncrement > 0) ?
               (oldCapacity + capacityIncrement) : (oldCapacity * 2);

           capacityIncrement *= 2;

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

   template<class E> void ArrayList<E>::removeRange(int fromIndex, int toIndex) {
       if (fromIndex < 0)
           throw ArrayIndexOutOfBoundsException(fromIndex);
       else if (toIndex > elementCount)
           throw ArrayIndexOutOfBoundsException(toIndex);
       else if (fromIndex > toIndex)
    	   throw IllegalArgumentException();

       destroyElementRange(fromIndex, toIndex);

       int numMoved = elementCount - toIndex ;
       if (numMoved > 0) {
           E* indexOffset = elementData + fromIndex;

           memmove((void*)indexOffset, (void*)(indexOffset + toIndex - fromIndex), numMoved * sizeof(E));
       }

       elementCount -= toIndex - fromIndex;
   }

   template<class E> void ArrayList<E>::setSize(int newSize, bool copyContent) {
       if (newSize > elementCount)
           ensureCapacity(newSize, copyContent);

       elementCount = newSize;
   }

   template<class E> void ArrayList<E>::createElementAt(const E& o, int index) {
       if (TypeInfo<E>::needConstructor)
           new (&(elementData[index])) E(o);
       else
    	   elementData[index] = o;
   }

   template<class E> void ArrayList<E>::destroyElementAt(int index) {
       if (TypeInfo<E>::needConstructor)
           (&(elementData[index]))->~E();
   }

   template<class E> void ArrayList<E>::destroyElementRange(int fromIndex, int toIndex) {
       if (TypeInfo<E>::needConstructor) {
    	   for (int i = fromIndex; i < toIndex; ++i)
    		   destroyElementAt(i);
       }
   }

   template<class E> void ArrayList<E>::destroyElements() {
	   destroyElementRange(0, elementCount);
   }

  } // namespace util
} // namespace sys

using namespace sys::util;

#endif /* ARRAYLIST_H_ */
