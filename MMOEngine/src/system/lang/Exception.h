/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include "../platform.h"

#include "StackTrace.h"

namespace sys {
  namespace lang {

	class Exception {
	protected:	
		string message;
	
		StackTrace trace;
	
	public:
		Exception() {
		}
	
		Exception(const string& msg) {
			message = msg;
		}
	
		virtual ~Exception() {
		}
		
		void printStackTrace() {
			cout << message << "\n";
			trace.print();
		}

		// setters and getters
		inline void setMessage(const string& msg) {
			message = msg;
		}
		
	 	inline string& getMessage() {
			return message;
		}
	 	
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*EXCEPTION_H_*/
