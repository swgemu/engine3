#ifndef ARRAYQUEUE_H_
#define ARRAYQUEUE_H_

#include "Vector.h"

namespace sys {
  namespace util {

	template<class E> class ArrayQueue : public Vector<E> {
	public:
		ArrayQueue();
	
		bool add(E& o);
		
		void clear();
		
		E& element();
		
		E remove();
	};

	template<class E> ArrayQueue<E>::ArrayQueue() : Vector<E>() {
	}

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
