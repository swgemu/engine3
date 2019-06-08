/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef DATABASEEXCEPTION_H_
#define DATABASEEXCEPTION_H_

#include "system/lang.h"

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
