/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef VECTOR_H_
#define VECTOR_H_

#include "../platform.h"

#include "../lang/String.h"

#include "../lang/ArrayIndexOutOfBoundsException.h"
#include "../lang/IllegalArgumentException.h"

#include "../lang/types.h"

namespace sys {
 namespace util {

   template<class E> class Vector : public Variable {
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

       E& get(int index) const;

       E& elementAt(int index) const;

       E remove(int index);

       bool removeElement(const E& element);

       void removeElementAt(int index);

       void removeRange(int fromIndex, int toIndex);

       void removeAll();

       E set(int index, const E& element);
       void setElementAt(int index, const E& element);

       void clone(Vector<E>& vector) const ;

       Vector<E>& operator=(Vector<E>& vector);

       bool toString(String& str);
       bool parseFromString(const String& str, int version = 0);

       bool toBinaryStream(ObjectOutputStream* stream);

       bool parseFromBinaryStream(ObjectInputStream* stream);

       static int getObjectData(const String& str, String& obj);

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

   template<class E> Vector<E>::Vector() : Variable() {
       init(10, 5);
   }

   template<class E> Vector<E>::Vector(int incr) : Variable() {
       init(10, incr);
   }

   template<class E> Vector<E>::Vector(int initsize, int incr) : Variable() {
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

   template<class E> E& Vector<E>::get(int index) const {
       return elementAt(index);
   }

   template<class E> E& Vector<E>::elementAt(int index) const {
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

   template<class E> void Vector<E>::clone(Vector<E>& vector) const {
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
       vector.clone(*this);

       return *this;
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

   template<class E> void Vector<E>::removeRange(int fromIndex, int toIndex) {
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
           memcpy(indexOffset, indexOffset + toIndex - fromIndex, numMoved * sizeof(E));
       }

       elementCount -= toIndex - fromIndex;
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

   template<class E> void Vector<E>::destroyElementRange(int fromIndex, int toIndex) {
       if (TypeInfo<E>::needConstructor) {
    	   for (int i = fromIndex; i < toIndex; ++i)
    		   destroyElementAt(i);
       }
   }

   template<class E> void Vector<E>::destroyElements() {
	   destroyElementRange(0, elementCount);
   }

   template<class E> bool Vector<E>::toString(String& str) {
	   str = "{";
	   str += String::valueOf(size());

	   for (int i = 0; i < size(); ++i) {
		   String data;

		   E obj = Vector<E>::get(i);

		   TypeInfo<E>::toString(&obj, data);

		   str += ",";
		   str += data;
	   }

	   str += "}";

	   return true;
   }

   template<class E> bool Vector<E>::parseFromString(const String& str, int version) {
	   Vector<E>::removeAll();

	   String data;

	   if (!getObjectData(str, data))
		   return true;

	   int comma = data.indexOf(",");

	   int variableSize;
	   TypeInfo<int>::parseFromString(&variableSize, data.subString(1, comma));

	   for (int i = 0; i < variableSize; ++i) {
		   data = data.subString(comma + 1);

		   String variableData;

		   if (data.subString(0, 1).indexOf("{") != -1) {
			   int lastSemiColon = getObjectData(data, variableData);

			   comma = lastSemiColon;
		   } else {
			   comma = data.indexOf(",");
			   if (comma != -1)
				   variableData = data.subString(0, comma);
			   else
				   variableData = data.subString(0, data.length() - 1);
		   }

		   E object;

		   if (TypeInfo<E>::parseFromString(&object, variableData))
			   add(object);
	   }

	   return true;
   }

   template<class E> bool Vector<E>::toBinaryStream(ObjectOutputStream* stream) {
	   int size = Vector<E>::size();

	   TypeInfo<int>::toBinaryStream(&size, stream);

	   for (int i = 0; i < Vector<E>::size(); ++i) {
		   E obj = Vector<E>::get(i);

		   TypeInfo<E>::toBinaryStream(&obj, stream);
	   }

	   return true;
   }

   template<class E> bool Vector<E>::parseFromBinaryStream(ObjectInputStream* stream) {
	   Vector<E>::removeAll();

	   int size;

	   TypeInfo<int>::parseFromBinaryStream(&size, stream);

	   for (int i = 0; i < size; ++i) {
		   E object;

		   if (TypeInfo<E>::parseFromBinaryStream(&object, stream))
			   add(object);
	   }

	   return true;
   }

   template<class E> int Vector<E>::getObjectData(const String& str, String& obj) {
	   int opening = str.indexOf("{");

	   if (opening == -1)
		   return 0;

	   int i, subObjects = 0;
	   bool subString = false;


	   for (i = opening + 1; i < str.length(); ++i) {
		   char opening = str.charAt(i);

		   if (subString && opening == '\\') {
			   ++i;
			   continue;
		   }

		   if (opening == '"') {
			   subString = !subString;
			   continue;
		   }

		   if (!subString) {
			   if (opening == '{') {
				   ++subObjects;
			   } else if (opening == '}') {
				   --subObjects;
			   }
		   }

		   if (subObjects < 0) {
			   break;
		   }
	   }

	   if (subObjects < 0) {
		   obj = str.subString(opening, i + 1);

		   //System::out << obj << "\n";

		   return i + 1;
	   } else
		  // System::out << "WARNING expecting \"}\" in " << str << " at " << i;

	   return 0;
   }
 }

} // namespace util
 // namespace sys

using namespace sys::util;

#endif /*VECTOR_H_*/
