/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef MYSQLDATABASE_H_
#define MYSQLDATABASE_H_

#include "../../system/lang.h"

#include <mysql.h>

#include "../log/Logger.h"

#include "Database.h"
#include "Statement.h"
#include "ResultSet.h"

#include "BinaryData.h"

namespace engine {
  namespace db {

	class MySqlDatabase : public Database, public Mutex,  public Logger {
		MYSQL mysql;
		String host;
		
	public:
		MySqlDatabase(const String& s);
		MySqlDatabase(const String& s, const String& host);
		
		virtual ~MySqlDatabase();
		
		void connect(const String& dbname, const String& user, const String& passw, int port);
		
		void executeStatement(const char* statement);
		void executeStatement(const String& statement);
		void executeStatement(const StringBuffer& statement);

		ResultSet* executeQuery(const char* statement);
		ResultSet* executeQuery(const String& statement);
		ResultSet* executeQuery(const StringBuffer& statement);

		void commit();
	
		void autocommit(bool doCommit);
	
		void close();
	    
	    void error();
	    void error(const char* query);

		static void escapeString(String& s);
		
		static void finalizeLibrary();
		
	};

  } // namespace db
} // namespace engine

using namespace engine::db;

#endif /*MYSQLDATABASE_H_*/
