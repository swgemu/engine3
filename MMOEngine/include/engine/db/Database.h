/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef DATABASE_H_
#define DATABASE_H_

#include "../../system/lang.h"

#include "DatabaseException.h"
#include "ResultSet.h"

namespace engine {
  namespace db {

	class Database {
	public:
		Database() {
		}
		
		virtual ~Database() {
		}
	
		virtual void connect(const string& dbname, const string& user, const string& passw, int port) = 0;
	
		virtual void close() = 0;

		virtual void executeStatement(const char* statement) = 0;

		virtual void executeStatement(const string& statement) = 0;
	
		virtual void executeStatement(const stringstream& statement) = 0;
	
		virtual ResultSet* executeQuery(const char* statement) = 0;

		virtual ResultSet* executeQuery(const string& statement) = 0;
	
		virtual ResultSet* executeQuery(const stringstream& statement) = 0;

		virtual void commit() = 0;
	
		virtual void autocommit(bool doCommit) = 0;
	};

  } // namespace db
} // namespace engine

using namespace engine::db;

#endif /*DATABASE_H_*/
