/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ROW_H_
#define ROW_H_

#include <mysql.h>

namespace engine {
  namespace db {

	class Row {
		MYSQL_ROW row;

	public:
		Row() {
		}
	
	};

  } // namespace db
} // namespace engine

using namespace engine::db;

#endif /*ROW_H_*/
