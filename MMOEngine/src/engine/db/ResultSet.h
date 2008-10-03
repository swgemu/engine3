/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef RESULTSET_H_
#define RESULTSET_H_

#include <mysql.h>

#include "../../system/lang.h"

namespace engine {
  namespace db {

	class ResultSet {
		MYSQL *mysql;
	
		MYSQL_RES *result;
		MYSQL_ROW row;
		
		int lastAffectedRow;
		
	public:
		ResultSet(MYSQL* db, MYSQL_RES *res) {
			mysql = db;
			result = res;
		}
		
		virtual ~ResultSet() {
			mysql_free_result(result);
		}
		
		bool next() {
			return (row = mysql_fetch_row(result)) != NULL;
		}
		
		bool getBoolean(int index) {
			return atoi(row[index]);
		}
		
		int getInt(int index) {
			return atoi(row[index]);
		}
	
		sys::uint32 getUnsignedInt(int index) {
			return (sys::uint32) strtoul(row[index], NULL, 0);
		}
		
		uint64 getUnsignedLong(int index) {
			return String::toUnsignedLong(row[index]);
		}
		
		long getLong(int index) {
			return (long)strtoul(row[index], NULL, 0);
		}
	
		float getFloat(int index) {
			return atof(row[index]);
		}
	
		char* getString(int index) {
			return row[index];
		}
		
		uint64 getLastAffectedRow() {
			return mysql_insert_id(mysql);
		}
		
		inline uint64 size() {
			return mysql_num_rows(result);
		}
	};

  } // namespace db
} // namespace engine

using namespace engine::db;

#endif /*RESULTSET_H_*/
