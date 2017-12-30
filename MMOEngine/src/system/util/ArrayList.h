/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ARRAYLIST_H_
#define ARRAYLIST_H_

#include "system/platform.h"

#include "system/lang/String.h"
#include "system/lang/Math.h"
#include "system/lang/ArrayIndexOutOfBoundsException.h"
#include "system/lang/IllegalArgumentException.h"
#include "system/lang/types.h"
#include "system/lang/Integer.h"

#ifdef CXX11_COMPILER
#include <initializer_list>
#endif

namespace sys {
 namespace util {
   template<typename E> class ArrayListReverseIterator;

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

	   typedef E* iterator;
	   typedef const E* const_iterator;

	   typedef ArrayListReverseIterator<E> reverse_iterator;
	   typedef ArrayListReverseIterator<const E> const_reverse_iterator;

#ifdef CXX11_COMPILER
       ArrayList(ArrayList<E>&& array);
	   ArrayList(std::initializer_list<E> v);
#endif

       ArrayList<E>& operator=(const ArrayList<E>& array);

#ifdef CXX11_COMPILER
       ArrayList<E>& operator=(ArrayList<E>&& array);
#endif

       virtual ~ArrayList();

       bool add(const E& element);
       bool add(int index, const E& element);

#ifdef CXX11_COMPILER
       bool add(E&& element);
	   bool add(int index, E&& element);

	   template<class ...A>
	   bool emplace(A&& ...element);
#endif

       void addAll(const ArrayList<E>& array);

#ifdef CXX11_COMPILER
	   void addAll(ArrayList<E>&& array) {
		   moveAll(array);
	   }

	   void moveAll(ArrayList<E>& array);
#endif

       bool contains(const E& element) const;

       void insertElementAt(const E& element, uint32 index);
#ifdef CXX11_COMPILER
	   void insertElementAt(E&& element, uint32 index);
#endif

       E& get(int index) const;
       E& getUnsafe(int index) const;

       E& elementAt(uint32 index) const;

       E& elementAtUnsafe(uint32 index) const;

       virtual E remove(int index);

       bool removeElement(const E& element);

       void removeElementAt(uint32 index);

       virtual void removeRange(int fromIndex, int toIndex);

       virtual void removeAll(int newSize = 10, int newIncrement = 5);

       E set(int index, const E& element);

#ifdef CXX11_COMPILER
	   E set(int index, E&& element);
	   void setElementAt(uint32 index, E&& element);
#endif
       void setElementAt(uint32 index, const E& element);

       void clone(ArrayList<E>& array) const ;

	   iterator begin() {
		   return elementData;
	   }

	   const_iterator begin() const {
		   return elementData;
	   }

	   reverse_iterator rbegin() {
		   return reverse_iterator(elementData + elementCount - 1);
	   }

	   const_reverse_iterator rbegin() const {
		   return const_reverse_iterator(elementData + elementCount - 1);
	   }

	   iterator end() {
		   return elementData + elementCount;
	   }

	   const_iterator end() const {
		   return elementData + elementCount;
	   }

	   reverse_iterator rend() {
		   return reverse_iterator(elementData - 1);
	   }

	   const_reverse_iterator rend() const {
		   return const_reverse_iterator(elementData - 1);
	   }

	   const E& back() const {
		   return get(elementCount - 1);
	   }

	   E& back() {
		   return get(elementCount - 1);
	   }

	   const E& front() const {
		   return get(0);
	   }

	   E& front() {
		   return get(0);
	   }

   protected:
       void init(int initsize, int incr);

       void ensureCapacity(int minCapacity, bool copyContent = true);

       void setSize(int newSize, bool copyContent = true);

       inline void createElementAt(const E& o, int index);

#ifdef CXX11_COMPILER
       inline void createElementAt(E&& o, int index);

	   template<class ...A>
	   inline void emplaceElement(int idx, A&& ...o);
#endif

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

	template<typename E>
	class ArrayListReverseIterator {
	 E* iterator;

	public:
	 ArrayListReverseIterator() : iterator() {

	 }

	 ArrayListReverseIterator(E* address) : iterator(address) {

	 }

	 template<typename T>
	 ArrayListReverseIterator(const ArrayListReverseIterator<T>& it) : iterator(it.getCurrent()) {

	 }

	 ArrayListReverseIterator<E>& operator=(const ArrayListReverseIterator<E>& it) {
		 if (this == &it)
			 return *this;

		 iterator = it.iterator;

		 return *this;
	 }

	 bool operator ==(const ArrayListReverseIterator<E>& b) const {
		 return b.iterator == iterator;
	 }

	 bool operator !=(const ArrayListReverseIterator<E>& b) const {
		 return b.iterator != iterator;
	 }

	 ArrayListReverseIterator<E>& operator++() {
		 --iterator;

		 return *this;
	 }

	 ArrayListReverseIterator<E> operator++(int) {
		 return ArrayListReverseIterator<E>(iterator--);
	 }

	 ArrayListReverseIterator<E>& operator--() {
		 ++iterator;

		 return *this;
	 }

	 ArrayListReverseIterator<E> operator--(int) {
		 return ArrayListReverseIterator<E>(iterator++);
	 }

	 E& operator*() {
		 return *iterator;
	 }

	 const E& operator*() const {
		 return *iterator;
	 }

	 E* operator->() {
		 return &(operator*());
	 }

	 const E* operator->() const {
	    return &(operator*());
	 }

	 E* getCurrent() const {
		 return iterator;
	 }

   };

   template<class E> ArrayList<E>::ArrayList() : elementData(nullptr), elementCapacity(0), capacityIncrement(0), elementCount(0) {
   }

   template<class E> ArrayList<E>::ArrayList(int incr) {
       init(1, incr);
   }

   template<class E> ArrayList<E>::ArrayList(int initsize, int incr) {
       init(initsize, incr);
   }

   template<class E> ArrayList<E>::ArrayList(const ArrayList<E>& array) {
	   init(array.elementCapacity, array.capacityIncrement);

	   elementCount = array.elementCount;

	   if (TypeInfo<E>::needConstructor) {
		   for (int i = 0; i < elementCount; ++i)
			   createElementAt(array.elementData[i], i);
	   } else
		   memcpy((void*)elementData, (void*)array.elementData, elementCount * sizeof(E));
   }

#ifdef CXX11_COMPILER
	template<class E> ArrayList<E>::ArrayList(std::initializer_list<E> v)  {
		init(v.size(), 5);

		elementCount = v.size();

		auto it = v.begin();
		for (int i = 0; i < elementCount; ++i, ++it) {
			createElementAt(*it, i);
		}
	}

   template<class E> ArrayList<E>::ArrayList(ArrayList<E>&& array) {
	   elementData = array.elementData;

	   elementCapacity = array.elementCapacity;

	   capacityIncrement = array.capacityIncrement;

	   elementCount = array.elementCount;

	   array.elementData = nullptr;
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

   	   if (elementData != nullptr) {
   		   destroyElements();

   		   free(elementData);
   	   }

   	   elementData = array.elementData;

   	   elementCapacity = array.elementCapacity;

   	   capacityIncrement = array.capacityIncrement;

   	   elementCount = array.elementCount;

   	   array.elementData = nullptr;
   	   array.elementCount = 0;
   	   array.capacityIncrement = 0;

   	   return *this;
      }
#endif

   template<class E> ArrayList<E>::~ArrayList() {
	   if (elementData != nullptr) {
		   destroyElements();

       	   free(elementData);
	   }
   }

   template<class E> void ArrayList<E>::init(int initsize, int incr) {
	   if (!initsize) {
		   elementCapacity = 0;
		   elementData = nullptr;
	   } else {
		   elementCapacity = Math::max(1, initsize);
		   elementData = (E*) malloc(elementCapacity * sizeof(E));
	   }

       elementCount = 0;
       capacityIncrement = incr;
   }

   template<class E> bool ArrayList<E>::add(const E& element) {
       ensureCapacity(elementCount + 1);

       createElementAt(element, elementCount++);
       return true;
   }

#ifdef CXX11_COMPILER
   template<class E> bool ArrayList<E>::add(E&& element) {
	   ensureCapacity(elementCount + 1);

	   createElementAt(std::move(element), elementCount++);
	   return true;
   }

	template<class E>
	template<class ...A> bool ArrayList<E>::emplace(A&& ...element) {
		ensureCapacity(elementCount + 1);

		emplaceElement(elementCount++, std::forward<A>(element)...);
		return true;
	}

	template<class E> bool ArrayList<E>::add(int index, E&& element) {
		insertElementAt(std::move(element), index);
		return true;
	}
#endif

   template<class E> bool ArrayList<E>::add(int index, const E& element) {
       insertElementAt(element, index);
       return true;
   }

   template<class E> void ArrayList<E>::addAll(const ArrayList<E>& array) {
	   if (TypeInfo<E>::needConstructor) {
		   for (int i = 0; i < array.size(); ++i) {
			   const E& element = array.getUnsafe(i);

			   add(element);
		   }
	   } else {
		   ensureCapacity(elementCount + array.size());

		   memcpy(elementData + elementCount, array.elementData, array.size() * sizeof(E));

		   elementCount += array.size();
	   }
   }
#ifdef CXX11_COMPILER
	template<class E> void ArrayList<E>::moveAll(ArrayList<E>& array) {
		if (TypeInfo<E>::needConstructor) {
			for (int i = 0; i < array.size(); ++i) {
				E& element = array.getUnsafe(i);

				emplace(std::move(element));
			}
		} else {
			ensureCapacity(elementCount + array.size());

			memcpy(elementData + elementCount, array.elementData, array.size() * sizeof(E));
			elementCount += array.size();
		}
	}
#endif

   template<class E> bool ArrayList<E>::contains(const E& element) const {
	   for (int i = 0; i < size(); ++i) {
		   if (element == getUnsafe(i)) {
			   return true;
		   }
	   }

	   return false;
   }

   template<class E> void ArrayList<E>::insertElementAt(const E& element, uint32 index) {
       if (index > (uint32)elementCount)
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
#ifdef CXX11_COMPILER
	template<class E> void ArrayList<E>::insertElementAt(E&& element, uint32 index) {
		if (index > (uint32)elementCount)
			throw ArrayIndexOutOfBoundsException(index);

		ensureCapacity(elementCount + 1);

		int numMoved = elementCount - index;
		if (numMoved > 0) {
			E* indexOffset = elementData + index;
			memmove((void*)(indexOffset + 1), (void*)indexOffset, numMoved * sizeof(E));
		}

		createElementAt(std::move(element), index);
		elementCount++;
	}
#endif

   template<class E> E& ArrayList<E>::get(int index) const {
       return elementAt(index);
   }

   template<class E> E& ArrayList<E>::elementAt(uint32 index) const {
       if (index >= (uint32) elementCount)
           throw ArrayIndexOutOfBoundsException(index);

       return elementData[index];
   }

   template<class E> E& ArrayList<E>::getUnsafe(int index) const {
	   return elementAtUnsafe(index);
   }

   template<class E> E& ArrayList<E>::elementAtUnsafe(uint32 index) const {
	   return elementData[index];
   }

   template<class E> E ArrayList<E>::remove(int index) {
#ifdef CXX11_COMPILER
	   if (std::is_move_constructible<E>::value) {
		   E oldValue(std::move(get(index)));

		   removeElementAt(index);

		   return oldValue;
	   } else {
		   E oldValue = get(index);

		   removeElementAt(index);

		   return oldValue;
	   }
#else
       E oldValue = get(index);

       removeElementAt(index);

       return oldValue;
#endif
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

   template<class E> void ArrayList<E>::removeElementAt(uint32 index) {
       if (index >= (uint32)elementCount)
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
	   if (elementData) {
		   destroyElements();

		   free(elementData);
	   }

       init(newSize, newIncrement);
   }

   template<class E> E ArrayList<E>::set(int index, const E& element) {
       E oldValue = get(index);

       setElementAt(index, element);

       return oldValue;
   }

#ifdef CXX11_COMPILER
	template<class E> E ArrayList<E>::set(int index, E&& element) {
		E oldValue = get(index);

		setElementAt(index, std::move(element));

		return oldValue;
	}

	template<class E> void ArrayList<E>::setElementAt(uint32 index, E&& element) {
		if (index >= (uint32)elementCount)
			throw ArrayIndexOutOfBoundsException(index);

		destroyElementAt(index);
		createElementAt(std::move(element), index);
	}
#endif
   template<class E> void ArrayList<E>::setElementAt(uint32 index, const E& element) {
       if (index >= (uint32)elementCount)
           throw ArrayIndexOutOfBoundsException(index);

       destroyElementAt(index);
       createElementAt(element, index);
   }

   template<class E> void ArrayList<E>::clone(ArrayList<E>& array) const {
       array.removeAll(elementCount, capacityIncrement);

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

           if (copyContent && elementData) {
        	   elementData = (E*) realloc(elementData, (elementCapacity = newCapacity) * sizeof(E));
           } else {
               elementData = (E*) malloc((elementCapacity = newCapacity) * sizeof(E));

			   if (oldData)
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

#ifdef CXX11_COMPILER
   template<class E> void ArrayList<E>::createElementAt(E&& o, int index) {
	   if (TypeInfo<E>::needConstructor)
		   new (&(elementData[index])) E(std::move(o));
	   else
		   elementData[index] = std::move(o);
   }

	template<class E>
	template<class ...A> void ArrayList<E>::emplaceElement(int index, A&& ...o) {
		new (&(elementData[index])) E(std::forward<A>(o)...);
	}
#endif

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
