/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ITERATOR_H_
#define ITERATOR_H_

namespace sys {
  namespace util {
		
	template<class V> class Iterator {
	public:
		virtual V& next() = 0;
	
		virtual bool hasNext() = 0;

	};

  } // namespace util
} // namespace sys

using namespace sys::util;

#endif /*ITERATOR_H_*/
