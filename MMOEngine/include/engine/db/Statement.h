/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef STATEMENT_H_
#define STATEMENT_H_

#include "../../system/lang.h"

namespace engine {
  namespace db {

	#define quote '\''

  	class Statement {
  		char* data;
  		
  	public:
  		Statement();
  		
	};

  } // namespace db
} // namespace engine

using namespace engine::db;

#endif /*STATEMENT_H_*/
