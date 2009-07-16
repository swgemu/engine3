/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef RESULTSET_H_
#define RESULTSET_H_

#ifdef PLATFORM_MAC
#include "mysql5/mysql/mysql.h"
#else
#include <mysql.h>
#endif

#include "system/lang.h"

#include "system/lang/Long.h"

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

		sys::int64 getLong(int index) {
			return Long::valueOf(row[index]);
		}

		sys::uint64 getUnsignedLong(int index) {
			return Long::unsignedvalueOf(row[index]);
		}

		float getFloat(int index) {
			return atof(row[index]);
		}

		char* getString(int index) {
			return row[index];
		}

		sys::uint64 getLastAffectedRow() {
			return mysql_insert_id(mysql);
		}

		inline sys::uint64 size() {
			return mysql_num_rows(result);
		}
	};

  } // namespace db
} // namespace engine

using namespace engine::db;

#endif /*RESULTSET_H_*/
