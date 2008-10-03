/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef OBJECT_H_
#define OBJECT_H_

#include "ref/ReferenceCounter.h"

namespace sys {
  namespace lang {

	class Object : public ReferenceCounter {
		bool finalized;
		
	public:
		Object() : ReferenceCounter() {
			initializeCount();
			
			finalized = false;
		}
		
		virtual ~Object() {
		}

		virtual void finalize() {
			if (finalized)
				return;
			
			finalized = true;
			
			release();
		}

		void revoke() {
			if (!finalized)
				return;
			
			finalized = false;
			
			acquire();
		}

	protected:
		inline void acquire() {
			increaseCount();
		}
		
		inline void release() {
			if (decreaseCount())
				destroy();
		}
		
		virtual bool destroy() {
			if (finalized) {
				delete this;
				
				return true;
			} else
				return false;
		}
		
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*OBJECT_H_*/
