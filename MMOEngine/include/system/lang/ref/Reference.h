/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef REFERENCE_H_
#define REFERENCE_H_

#include "ReferenceCounter.h"

namespace sys {
  namespace lang {

	template<class O> class Reference : public ReferenceCounter {
	protected:
		O* _object;

	public:
		Reference() : ReferenceCounter() {
			_object = NULL;
		}
		
		Reference(Reference<O>& ref) : ReferenceCounter(ref) {
			ref.increaseCount();

			copyContent(ref);
		}

		Reference(O* obj) : ReferenceCounter() {
			initializeReference(obj);
		}

		virtual ~Reference() {
			decreaseReference();
		}
		
		void operator=(const Reference<O>& ref) {
			ref.increaseCount();

			copyContent(ref);
		}

		O* operator->() {
			return _object;
		}

	protected:
		inline void initializeReference(O* obj) {
			_object = obj;

			initializeCount();
		}

	private:
		inline void copyContent(const Reference<O>& ref) {
			_object = ref._object;

			_references = ref._references;
		}

		inline void increaseReference() {
			increaseCount();
		}

		inline void decreaseReference() {
			if (!decreaseCount()) {
				delete _object;
			}
		}

	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*REFERENCE_H_*/
