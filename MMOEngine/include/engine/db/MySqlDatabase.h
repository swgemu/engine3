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
		string host;
		
	public:
		MySqlDatabase(const string& s);
		MySqlDatabase(const string& s, const string& host);
		
		virtual ~MySqlDatabase();
		
		void connect(const string& dbname, const string& user, const string& passw, int port);
		
		void executeStatement(const char* statement);
		void executeStatement(const string& statement);
		void executeStatement(const stringstream& statement);

		ResultSet* executeQuery(const char* statement);
		ResultSet* executeQuery(const string& statement);
		ResultSet* executeQuery(const stringstream& statement);

		void commit();
	
		void autocommit(bool doCommit);
	
		void close();
	    
	    void error();
	    void error(const char* query);

		static void escapeString(string& s);
		
		static void finalizeLibrary();
		
	};

  } // namespace db
} // namespace engine

using namespace engine::db;

#endif /*MYSQLDATABASE_H_*/
