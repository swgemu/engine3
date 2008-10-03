/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef MANAGEDVECTORIMPLEMENTATION_H_
#define MANAGEDVECTORIMPLEMENTATION_H_

#include "../../../system/lang.h"

#include "../ManagedReference.h"

namespace engine {
  namespace core {
    namespace util {

	template<class O> class ManagedVectorImplementation {
	protected:
		ManagedReference<O>* elementData;
	
		int elementCapacity, capacityIncrement;
		int elementCount;
	
	public:
		ManagedVectorImplementation();
		ManagedVectorImplementation(int incr);
		ManagedVectorImplementation(int initsize, int incr);
	
		virtual ~ManagedVectorImplementation();
	
		bool add(const O* element);
		bool add(int index, const O* element);
	
		void insertElementAt(const O* element, int index);
	
		O* get(int index);
	
		O* elementAt(int index);
	
		O* remove(int index);
	
		bool removeElement(const O* element);
	
		void removeElementAt(int index);
	
		void removeAll();
	
		O* set(int index, const O* element);
		void setElementAt(int index, const O* element);
	
	protected:
		void init(int initsize, int incr);
	
		void ensureCapacity(int minCapacity, bool copyContent = true);
	
		void setSize(int newSize, bool copyContent = true);
	
		inline void createElementAt(const O* element, int index);
	
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
	
	/*template<class O> ManagedVectorImplementation<O>::ManagedVectorImplementation() {
		init(10, 5);
	}
	
	template<class O> ManagedVectorImplementation<O>::ManagedVectorImplementation(int incr) {
		init(10, incr);
	}
	
	template<class O> ManagedVectorImplementation<O>::ManagedVectorImplementation(int initsize, int incr) {
		init(initsize, incr);
	}
	
	template<class O> ManagedVectorImplementation<O>::~ManagedVectorImplementation() {
		destroyElements();
	
		free(elementData);
	}
	
	template<class O> void ManagedVectorImplementation<O>::init(int initsize, int incr) {
		elementCapacity = initsize;
		elementData = (ManagedReference<O>*) malloc(elementCapacity * sizeof(ManagedReference<O>));
	
		elementCount = 0;
		capacityIncrement = incr;
	}
	
	template<class O> bool ManagedVectorImplementation<O>::add(const O* element) {
		ensureCapacity(elementCount + 1);
	
		createElementAt(element, elementCount++);
		return true;
	}
	
	template<class O> bool ManagedVectorImplementation<O>::add(int index, const O* element) {
		insertElementAt(element, index);
		return true;
	}
	
	template<class O> void ManagedVectorImplementation<O>::insertElementAt(const O* element, int index) {
		if (index > elementCount || index < 0)
			throw ArrayIndexOutOfBoundsException(index);
	
		ensureCapacity(elementCount + 1);
	
		int numMoved = elementCount - index;
		if (numMoved > 0) {
			ManagedReference<O>* indexOffset = elementData + index;
			memmove(indexOffset + 1, indexOffset, numMoved * sizeof(ManagedReference<O>));
		}
	
		createElementAt(element, index);
		elementCount++;
	}
	
	template<class O> O* ManagedVectorImplementation<O>::get(int index) {
		return elementAt(index);
	}
	
	template<class O> O* ManagedVectorImplementation<O>::elementAt(int index) {
		if (index >= elementCount || index < 0)
			throw ArrayIndexOutOfBoundsException(index);
	
		return elementData[index];
	}
	
	template<class O> O* ManagedVectorImplementation<O>::remove(int index) {
		O* oldValue = get(index);
	
		removeElementAt(index);
	
		return oldValue;
	}
	
	template<class O> bool ManagedVectorImplementation<O>::removeElement(const O* element) {
		for (int i = 0; i < elementCount; ++i) {
			if (elementData[i].get() == element) {
				remove(i);
	               
				return true;
			}
		}
	
		return false;
	}
	
	template<class O> void ManagedVectorImplementation<O>::removeElementAt(int index) {
		if (index >= elementCount || index < 0)
			throw ArrayIndexOutOfBoundsException(index);
	
		destroyElementAt(index);
	
		int numMoved = elementCount - index - 1;
		if (numMoved > 0) {
			ManagedReference<O>* indexOffset = elementData + index;
			memcpy(indexOffset, indexOffset + 1, numMoved * sizeof(ManagedReference<O>));
		}
	
		--elementCount;
	}
	
	template<class O> void ManagedVectorImplementation<O>::removeAll() {
		destroyElements();
	
		free(elementData);
	
		init(10, 5);
	}
	
	template<class O> O* ManagedVectorImplementation<O>::set(int index, const O* element) {
		O* oldValue = get(index);
	
		setElementAt(index, element);
	
		return oldValue;
	}
	
	template<class O> void ManagedVectorImplementation<O>::setElementAt(int index, const O* element) {
		if (index >= elementCount || index < 0)
			throw ArrayIndexOutOfBoundsException(index);
	
		destroyElementAt(index);
		createElementAt(element, index);
	}
	
	template<class O> void ManagedVectorImplementation<O>::ensureCapacity(int minCapacity, bool copyContent) {
		int oldCapacity = elementCapacity;
	
		if (minCapacity > oldCapacity) {
			ManagedReference<O>* oldData = elementData;
	
			int newCapacity = (capacityIncrement > 0) ? (oldCapacity + capacityIncrement) : (oldCapacity * 2);
	
			if (newCapacity < minCapacity)
				newCapacity = minCapacity;
	
			elementData = (ManagedReference<O>*) malloc((elementCapacity = newCapacity) * sizeof(ManagedReference<O>));
	
			if (copyContent)
				memcpy(elementData, oldData, elementCount * sizeof(ManagedReference<O>));
	
			free(oldData);
		}
	}
	
	template<class O> void ManagedVectorImplementation<O>::setSize(int newSize, bool copyContent) {
		if (newSize > elementCount)
			ensureCapacity(newSize, copyContent);
	
		elementCount = newSize;
	}
	
	template<class O> void ManagedVectorImplementation<O>::createElementAt(const O* element, int index) {
		new (&(elementData[index])) ManagedReference<O>(element);
	}
	
	template<class O> void ManagedVectorImplementation<O>::destroyElementAt(int index) {
		(&(elementData[index]))->~ManagedReference<O>();
	}
	
	template<class O> void ManagedVectorImplementation<O>::destroyElements() {
		for (int i = 0; i < elementCount; ++i)
			destroyElementAt(i);
	}*/

    } // namespace util
  } // namespace core
} // namespace engine

using namespace engine::core::util;

#endif /*MANAGEDVECTORIMPLEMENTATION_H_*/
