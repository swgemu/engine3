/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef DATABASEEXCEPTION_H_
#define DATABASEEXCEPTION_H_

#include "../../system/lang.h"

namespace engine {
  namespace db {

	class DatabaseException : public Exception {
	public:
		DatabaseException(const String& msg) : Exception(msg) {
		}
		
	};

  } // namespace db
} // namespace engine

using namespace engine::db;

#endif /*DATABASEEXCEPTION_H_*/
