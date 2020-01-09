/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#pragma once

#ifdef CXX11_COMPILER
#include <type_traits>
#endif

#include "system/lang/Object.h"

#include "ArrayList.h"

namespace sys {
 namespace util {

   template<class E> class SynchronizedVector;
   template<class E> class SynchronizedSortedVector;

   template<class E, bool RawCopyAndRealloc = ARRAYLIST_DEFAULT_RAW_REALLOC> class Vector : public ArrayList<E, RawCopyAndRealloc>, public Object {
   public:
       Vector();
       Vector(int incr);
       Vector(int initsize, int incr);
       Vector(const Vector<E, RawCopyAndRealloc>& vector);

#ifdef CXX11_COMPILER
       Vector(std::initializer_list<E> l);
       Vector(Vector<E, RawCopyAndRealloc>&& vector);
#endif

       Vector<E, RawCopyAndRealloc>& operator=(const Vector<E, RawCopyAndRealloc>& vector);

#ifdef CXX11_COMPILER
       Vector<E, RawCopyAndRealloc>& operator=(Vector<E, RawCopyAndRealloc>&& vector);
#endif

       void addAll(const Vector<E, RawCopyAndRealloc>& array) {
	       ArrayList<E, RawCopyAndRealloc>::addAll(array);
       }

#ifdef CXX11_COMPILER
       void addAll(Vector<E, RawCopyAndRealloc>&& array) {
	       ArrayList<E, RawCopyAndRealloc>::moveAll(array);
       }
#endif

       ~Vector();

       void clone(Vector<E, RawCopyAndRealloc>& vector) const ;

       bool toBinaryStream(ObjectOutputStream* stream);

       bool parseFromBinaryStream(ObjectInputStream* stream);

       bool operator==(const Vector<E, RawCopyAndRealloc>& v) const;

       Object* clone();

       static int getObjectData(const String& str, String& obj);

       friend class SynchronizedVector<E>;
       friend class SynchronizedSortedVector<E>;

   };

   template<class E, bool RawCopyAndRealloc> Vector<E, RawCopyAndRealloc>::Vector() : ArrayList<E, RawCopyAndRealloc>() {
   }

   template<class E, bool RawCopyAndRealloc> Vector<E, RawCopyAndRealloc>::Vector(int incr) : ArrayList<E, RawCopyAndRealloc>(10, incr) {
   }

   template<class E, bool RawCopyAndRealloc> Vector<E, RawCopyAndRealloc>::Vector(int initsize, int incr) : ArrayList<E, RawCopyAndRealloc>(initsize, incr) {
   }

   template<class E, bool RawCopyAndRealloc> Vector<E, RawCopyAndRealloc>::Vector(const Vector<E, RawCopyAndRealloc>& vector) : ArrayList<E, RawCopyAndRealloc>(vector), Object() {
   }

#ifdef CXX11_COMPILER
   template<class E, bool RawCopyAndRealloc> Vector<E, RawCopyAndRealloc>::Vector(std::initializer_list<E> l) : ArrayList<E, RawCopyAndRealloc>(l), Object() {
   }

   template<class E, bool RawCopyAndRealloc> Vector<E, RawCopyAndRealloc>::Vector(Vector<E, RawCopyAndRealloc>&& vector) : ArrayList<E, RawCopyAndRealloc>(std::move(vector)), Object() {
   }
#endif

   template<class E, bool RawCopyAndRealloc> Vector<E, RawCopyAndRealloc>& Vector<E, RawCopyAndRealloc>::operator=(const Vector<E, RawCopyAndRealloc>& vector) {
	   if (this == &vector)
		   return *this;

	   ArrayList<E, RawCopyAndRealloc>::operator=(vector);

	   return *this;
   }

#ifdef CXX11_COMPILER
   template<class E, bool RawCopyAndRealloc> Vector<E, RawCopyAndRealloc>& Vector<E, RawCopyAndRealloc>::operator=(Vector<E, RawCopyAndRealloc>&& vector) {
	   if (this == &vector)
		   return *this;

	   ArrayList<E, RawCopyAndRealloc>::operator=(std::move(vector));

	   return *this;
   }
#endif

   template<class E, bool RawCopyAndRealloc> Vector<E, RawCopyAndRealloc>::~Vector() {
   }

   template<class E, bool RawCopyAndRealloc> void Vector<E, RawCopyAndRealloc>::clone(Vector<E, RawCopyAndRealloc>& array) const {
	   ArrayList<E, RawCopyAndRealloc>::clone(array);
   }

   template<class E, bool RawCopyAndRealloc> bool Vector<E, RawCopyAndRealloc>::toBinaryStream(ObjectOutputStream* stream) {
	   return ArrayList<E, RawCopyAndRealloc>::toBinaryStream(stream);
   }

   template<class E, bool RawCopyAndRealloc> bool Vector<E, RawCopyAndRealloc>::parseFromBinaryStream(ObjectInputStream* stream) {
	   return ArrayList<E, RawCopyAndRealloc>::parseFromBinaryStream(stream);
   }

   template<class E, bool RawCopyAndRealloc> Object* Vector<E, RawCopyAndRealloc>::clone() {
	   return new Vector<E, RawCopyAndRealloc>(*this);
   }

   template<class E, bool RawCopyAndRealloc>  bool Vector<E, RawCopyAndRealloc>::operator==(const Vector<E, RawCopyAndRealloc>& v) const {
	   if (this == &v)
		   return true;

	   return false;
   }

   template<class E, bool RawCopyAndRealloc> int Vector<E, RawCopyAndRealloc>::getObjectData(const String& str, String& obj) {
	   int opening = str.indexOf("{");

	   if (opening == -1)
		   return 0;

	   int i, subObjects = 0;
	   bool subString = false;


	   for (i = opening + 1; i < str.length(); ++i) {
		   char openingCharacter = str.charAt(i);

		   if (subString && openingCharacter == '\\') {
			   ++i;
			   continue;
		   }

		   if (openingCharacter == '"') {
			   subString = !subString;
			   continue;
		   }

		   if (!subString) {
			   if (openingCharacter == '{') {
				   ++subObjects;
			   } else if (openingCharacter == '}') {
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
	   } else {
		  // System::out << "WARNING expecting \"}\" in " << str << " at " << i;
	   }

	   return 0;
   }

  } // namespace util
} // namespace sys

using namespace sys::util;

