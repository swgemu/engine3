/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#ifndef ARRAYQUEUE_H_
#define ARRAYQUEUE_H_

#include "Vector.h"

namespace sys {
  namespace util {

	template<class E> class ArrayQueue : public Vector<E> {
	public:
		ArrayQueue();
		ArrayQueue(const ArrayQueue& queue);

#ifdef CXX11_COMPILER
		ArrayQueue(ArrayQueue&& queue);
#endif

		ArrayQueue& operator=(const ArrayQueue& queue);

#ifdef CXX11_COMPILER
		ArrayQueue& operator=(ArrayQueue&& queue);
#endif
	
		bool add(E& o);
		
		void clear();
		
		E& element();
		
		E remove();
	};

	template<class E> ArrayQueue<E>::ArrayQueue() : Vector<E>() {
	}

	template<class E> ArrayQueue<E>::ArrayQueue(const ArrayQueue& queue) : Vector<E>(queue) {
	}

#ifdef CXX11_COMPILER
	template<class E> ArrayQueue<E>::ArrayQueue(ArrayQueue&& queue) : Vector<E>(std::move(queue)) {
	}
#endif

	template<class E> ArrayQueue<E>& ArrayQueue<E>::operator=(const ArrayQueue<E>& queue) {
		if (this == &queue)
			return *this;

		Vector<E>::operator=(queue);

		return *this;
	}

#ifdef CXX11_COMPILER
	template<class E> ArrayQueue<E>& ArrayQueue<E>::operator=(ArrayQueue<E>&& queue) {
		if (this == &queue)
			return *this;

		Vector<E>::operator=(std::move(queue));

		return *this;
	}
#endif

	template<class E> bool ArrayQueue<E>::add(E& o) {
		Vector<E>::add(o);
		
		return true;
	}

	template<class E> void ArrayQueue<E>::clear() {
		Vector<E>::removeAll();
	}

	template<class E> E& ArrayQueue<E>::element() {
    	return Vector<E>::get(0);
	}
	
	template<class E> E ArrayQueue<E>::remove() {
		return Vector<E>::remove(0);
	}

  } // namespace util
} // namespace sys


#endif /*ARRAYQUEUE_H_*/
