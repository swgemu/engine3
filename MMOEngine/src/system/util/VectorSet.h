/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef VECTORSET_H_
#define VECTORSET_H_

#include "system/lang/Object.h"

#include "VectorMap.h"

namespace sys {
  namespace util {

	template<class E> class VectorSet : public VectorMap<E, E> {
	public:
		VectorSet() : VectorMap<E, E>() {
		}

		virtual ~VectorSet() {
		}

		void add(const E& obj) {
			VectorMap<E, E>::put(obj, obj);
		}

	};

  } // namespace util
} // namespace sys

using namespace sys::util;

#endif /*VECTORSET_H_*/
