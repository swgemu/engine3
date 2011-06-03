/*
 Copyright (C) 2007 <SWGEmu>. All rights reserved.
 Distribution of this file for usage outside of Core3 is prohibited.
 */

#ifndef DATABASE_H_
#define DATABASE_H_

#include "system/lang.h"

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

	virtual void connect(const String& dbname, const String& user,
			const String& passw, int port) {

	}

	virtual void close() {

	}

	virtual void executeStatement(const char* statement) {

	}

	virtual void executeStatement(const String& statement) {

	}

	virtual void executeStatement(const StringBuffer& statement) {

	}

	virtual ResultSet* executeQuery(const char* statement) {
		return NULL;
	}

	virtual ResultSet* executeQuery(const String& statement) {
		return NULL;
	}

	virtual ResultSet* executeQuery(const StringBuffer& statement) {
		return NULL;
	}

	virtual void commit() {

	}

	virtual void rollback() {

	}

	virtual void autocommit(bool doCommit) {

	}

	static void escapeString(String& s) {
		if (s.isEmpty())
			return;

		StringBuffer buf;

		for (int i = 0; i < s.length(); ++i) {
			char ch = s.charAt(i);

			switch (ch) {
			case '\0': // Must be escaped for "mysql"
				buf << "\\0";
				break;
			case '\n': // Must be escaped for logs
				buf << "\\n";
				break;
			case '\r':
				buf << "\\r";
				break;
			case '\\':
				buf << "\\\\";
				break;
			case '\"':
				buf << "\\\"";
				break;
			case '\'': // Better safe than sorry
				buf << "\\\'";
				break;
				/*case '\032': // This gives problems on Win32
				 buf << "\\Z";
				 break;*/
			default:
				buf << ch;
				break;
			}
		}

		s = buf.toString();
	}
};

} // namespace db
} // namespace engine

using namespace engine::db;

#endif /*DATABASE_H_*/
