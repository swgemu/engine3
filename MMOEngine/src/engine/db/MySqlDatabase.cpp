/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "MySqlDatabase.h"

MySqlDatabase::MySqlDatabase(const String& s) : Mutex("MYSQL DB"), Logger(s) {
	queryTimeout = 600000;
}

MySqlDatabase::MySqlDatabase(const String& s, const String& host) : Mutex("MYSQL DB"), Logger(s) {
	MySqlDatabase::host = host;

	queryTimeout = 600000;

	setLockTracing(false);
}

MySqlDatabase::~MySqlDatabase() {
	close();
}

void MySqlDatabase::connect(const String& dbname, const String& user, const String& passw, int port) {
	Locker locker(this);

	if (!mysql_init(&mysql))
		error();

	mysql_options(&mysql, MYSQL_OPT_READ_TIMEOUT, (char*)&queryTimeout);
	mysql_options(&mysql, MYSQL_OPT_WRITE_TIMEOUT, (char*)&queryTimeout);

	if (!mysql_real_connect(&mysql, host.toCharArray(), user.toCharArray(), passw.toCharArray(), dbname.toCharArray(), port, NULL, 0))
		error();

	StringBuffer msg;
	msg << "connected to " << host;
	info(msg);
}

void MySqlDatabase::executeStatement(const char* statement) {
	Locker locker(this);

	/*if (mysql_query(&mysql, statement))
		error(statement);*/

	while (mysql_query(&mysql, statement)) {
		unsigned int errorNumber = mysql_errno(&mysql);

		if (errorNumber != 1205/*ER_LOCK_WAIT_TIMEOUT*/) {
			error(statement);
			break;
		} else
			info(String("Warning mysql lock wait timeout on statement: ") + statement);
	}
}

void MySqlDatabase::executeStatement(const String& statement) {
	executeStatement(statement.toCharArray());
}

void MySqlDatabase::executeStatement(const StringBuffer& statement) {
	executeStatement(statement.toString().toCharArray());
}

ResultSet* MySqlDatabase::executeQuery(const char* statement) {
	Locker locker(this);

	/*if (mysql_query(&mysql, statement))
		error(statement);*/

	while (mysql_query(&mysql, statement)) {
		unsigned int errorNumber = mysql_errno(&mysql);

		if (errorNumber != 1205/* ER_LOCK_WAIT_TIMEOUT*/) {
			error(statement);
			break;
		} else
			info(String("Warning mysql lock wait timeout on statement: ") + statement);
	}

	MYSQL_RES* result = mysql_store_result(&mysql);

	if (result == NULL) {
		//TA: insert statements return NULL result
		//but if the table has auto_increment you can still call mysql_insert_id(mysql) to get the last affected row
		if (mysql_field_count(&mysql) != 0) {
			error(statement);
		}
	}

	ResultSet* res = new ResultSet(&mysql, result);

	return res;
}

ResultSet* MySqlDatabase::executeQuery(const String& statement) {
	return executeQuery(statement.toCharArray());
}

ResultSet* MySqlDatabase::executeQuery(const StringBuffer& statement) {
	return executeQuery(statement.toString().toCharArray());
}

void MySqlDatabase::commit() {
	Locker locker(this);

	mysql_commit(&mysql);
}

void MySqlDatabase::autocommit(bool doCommit) {
	Locker locker(this);

	mysql_autocommit(&mysql, doCommit);
}

void MySqlDatabase::close() {
	mysql_close(&mysql);

	info("disconnected");
}


void MySqlDatabase::error() {
	StringBuffer msg;
	msg << mysql_errno(&mysql) << ": " << mysql_error(&mysql);
	Logger::error(msg);

	throw DatabaseException(msg.toString());
}

void MySqlDatabase::error(const char* query) {
	StringBuffer msg;
	msg << "DatabaseException caused by query: " << query << "\n" << mysql_errno(&mysql) << ": " << mysql_error(&mysql);
	//Logger::error(msg);

	throw DatabaseException(msg.toString());
}

void MySqlDatabase::escapeString(String& s) {
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

void MySqlDatabase::finalizeLibrary() {
	mysql_library_end();
}
