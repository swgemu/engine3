/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "MySqlDatabase.h"

MySqlDatabase::MySqlDatabase(const String& s) : Mutex("MYSQL DB"), Logger(s) {
}

MySqlDatabase::MySqlDatabase(const String& s, const String& host) : Mutex("MYSQL DB"), Logger(s) {
	MySqlDatabase::host = host;

	setLockTracing(false);
}

MySqlDatabase::~MySqlDatabase() {
	close();
}

void MySqlDatabase::connect(const String& dbname, const String& user, const String& passw, int port) {
	lock();

	if (!mysql_init(&mysql))
		error();

	if (!mysql_real_connect(&mysql, host.toCharArray(), user.toCharArray(), passw.toCharArray(), dbname.toCharArray(), port, NULL, 0))
		error();

	StringBuffer msg;
	msg << "connected to " << host;
	info(msg);

	unlock();
}

void MySqlDatabase::executeStatement(const char* statement) {
	lock();

	if (mysql_query(&mysql, statement))
		error(statement);

	unlock();
}

void MySqlDatabase::executeStatement(const String& statement) {
	executeStatement(statement.toCharArray());
}

void MySqlDatabase::executeStatement(const StringBuffer& statement) {
	executeStatement(statement.toString().toCharArray());
}

ResultSet* MySqlDatabase::executeQuery(const char* statement) {
	lock();

	if (mysql_query(&mysql, statement))
		error(statement);

	MYSQL_RES* result = mysql_store_result(&mysql);

	if (result == NULL) {
		//TA: insert statements return NULL result
		//but if the table has auto_increment you can still call mysql_insert_id(mysql) to get the last affected row
		if (mysql_field_count(&mysql) != 0) {
			error(statement);
		}
	}

	ResultSet* res = new ResultSet(&mysql, result);

	unlock();
	return res;
}

ResultSet* MySqlDatabase::executeQuery(const String& statement) {
	return executeQuery(statement.toCharArray());
}

ResultSet* MySqlDatabase::executeQuery(const StringBuffer& statement) {
	return executeQuery(statement.toString().toCharArray());
}

void MySqlDatabase::commit() {
	lock();

	mysql_commit(&mysql);

	unlock();
}

void MySqlDatabase::autocommit(bool doCommit) {
	lock();

	mysql_autocommit(&mysql, doCommit);

	unlock();
}

void MySqlDatabase::close() {
	mysql_close(&mysql);

	info("disconnected");
}


void MySqlDatabase::error() {
	StringBuffer msg;
	msg << mysql_errno(&mysql) << ": " << mysql_error(&mysql);
	Logger::error(msg);

	unlock();

	throw DatabaseException(msg.toString());
}

void MySqlDatabase::error(const char* query) {
	StringBuffer msg;
	msg << "DatabaseException caused by query: " << query << "\n" << mysql_errno(&mysql) << ": " << mysql_error(&mysql);
	//Logger::error(msg);

	unlock();

	throw DatabaseException(msg.toString());
}

void MySqlDatabase::escapeString(String& s) {
	if (s.isEmpty())
		return;

	StringBuffer buf;

	for (int i = 0; i < s.length(); i++) {
		switch (s[i]) {
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
			continue;
		}

		++i;
	}

	s = buf.toString();
}

void MySqlDatabase::finalizeLibrary() {
	mysql_library_end();
}
