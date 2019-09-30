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

   template<class E> class Vector : public ArrayList<E>, public Object {
   public:
       Vector();
       Vector(int incr);
       Vector(int initsize, int incr);
       Vector(const Vector<E>& vector);

#ifdef CXX11_COMPILER
       Vector(std::initializer_list<E> l);
       Vector(Vector<E>&& vector);
#endif

       Vector<E>& operator=(const Vector<E>& vector);

#ifdef CXX11_COMPILER
       Vector<E>& operator=(Vector<E>&& vector);
#endif

       void addAll(const Vector<E>& array) {
	       ArrayList<E>::addAll(array);
       }

#ifdef CXX11_COMPILER
       void addAll(Vector<E>&& array) {
	       ArrayList<E>::moveAll(array);
       }
#endif

       ~Vector();

       void clone(Vector<E>& vector) const ;

       bool toBinaryStream(ObjectOutputStream* stream);

       bool parseFromBinaryStream(ObjectInputStream* stream);

       bool operator==(const Vector<E>& v) const;

       Object* clone();

       static int getObjectData(const String& str, String& obj);

       friend class SynchronizedVector<E>;
       friend class SynchronizedSortedVector<E>;

   };

   template<class E> Vector<E>::Vector() : ArrayList<E>() {
   }

   template<class E> Vector<E>::Vector(int incr) : ArrayList<E>(10, incr) {
   }

   template<class E> Vector<E>::Vector(int initsize, int incr) : ArrayList<E>(initsize, incr) {
   }

   template<class E> Vector<E>::Vector(const Vector<E>& vector) : ArrayList<E>(vector), Object() {
   }

#ifdef CXX11_COMPILER
   template<class E> Vector<E>::Vector(std::initializer_list<E> l) : ArrayList<E>(l), Object() {
   }

   template<class E> Vector<E>::Vector(Vector<E>&& vector) : ArrayList<E>(std::move(vector)), Object() {
   }
#endif

   template<class E> Vector<E>& Vector<E>::operator=(const Vector<E>& vector) {
	   if (this == &vector)
		   return *this;

	   ArrayList<E>::operator=(vector);

	   return *this;
   }

#ifdef CXX11_COMPILER
   template<class E> Vector<E>& Vector<E>::operator=(Vector<E>&& vector) {
	   if (this == &vector)
		   return *this;

	   ArrayList<E>::operator=(std::move(vector));

	   return *this;
   }
#endif

   template<class E> Vector<E>::~Vector() {
   }

   template<class E> void Vector<E>::clone(Vector<E>& array) const {
	   ArrayList<E>::clone(array);
   }

   template<class E> bool Vector<E>::toBinaryStream(ObjectOutputStream* stream) {
	   return ArrayList<E>::toBinaryStream(stream);
   }

   template<class E> bool Vector<E>::parseFromBinaryStream(ObjectInputStream* stream) {
	   return ArrayList<E>::parseFromBinaryStream(stream);
   }

   template<class E> Object* Vector<E>::clone() {
	   return new Vector<E>(*this);
   }

   template<class E>  bool Vector<E>::operator==(const Vector<E>& v) const {
	   if (this == &v)
		   return true;

	   return false;
   }

   template<class E> int Vector<E>::getObjectData(const String& str, String& obj) {
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

