/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef REFERENCE_H_
#define REFERENCE_H_

#include "../platform.h"

#include "../thread/Atomic.h"

namespace sys {
  namespace lang {

	template<class O> class Reference {
	protected:
		O* _object;
	
		sys::uint32* _references;
	
	public:
		Reference() {
			_object = NULL;
			_references = NULL;
		}
	
		Reference(Reference<O>& ref) {
			ref.increaseReference();
	
			copyContent(ref);
		}
	
		Reference(O* obj) {
			initializeReference(obj);
		}
	
		~Reference() {
			if (_references != NULL) {
				delete _references;
				_references = NULL;
			}
		}

		void operator=(const Reference<O>& ref) {
			ref.increaseReference();
	
			copyContent(ref);
		}
	
		O* operator->() {
			return _object;
		}
	
		inline void acquire() {
			increaseReference();
		}
		
		inline void release() {
			decreaseReference();
		}
		
	protected:
		inline void initializeReference(O* obj) {
			_object = obj;
	
			_references = new sys::uint32();
			*_references = 1;
		}
	
	private:
		inline void copyContent(const Reference<O>& ref) {
			_object = ref._object;
			_references = ref._references;
		}
	
		inline void increaseReference() {
			Atomic::incrementInt(_references);
		}
	
		inline void decreaseReference() {
			if (!Atomic::decrementInt(_references)) {
				//cout << "[Reference] destroying object\n";
				delete _object;
			}
		}
	
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*REFERENCE_H_*/

