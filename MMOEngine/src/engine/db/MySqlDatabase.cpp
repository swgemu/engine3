/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "MySqlDatabase.h"

MySqlDatabase::MySqlDatabase(const string& s) : Mutex("MYSQL DB"), Logger(s) {
}

MySqlDatabase::MySqlDatabase(const string& s, const string& host) : Mutex("MYSQL DB"), Logger(s) {
	MySqlDatabase::host = host;

	setLockTracing(false);
}

MySqlDatabase::~MySqlDatabase() {
	close();
}

void MySqlDatabase::connect(const string& dbname, const string& user, const string& passw, int port) {
	lock();

	if (!mysql_init(&mysql))
		error();

	if(!mysql_real_connect(&mysql, host.c_str(), user.c_str(), passw.c_str(), dbname.c_str(), port, NULL, 0))
		error();

	stringstream msg;
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

void MySqlDatabase::executeStatement(const string& statement) {
	executeStatement(statement.c_str());
}

void MySqlDatabase::executeStatement(const stringstream& statement) {
	executeStatement(statement.str().c_str());
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

ResultSet* MySqlDatabase::executeQuery(const string& statement) {
	return executeQuery(statement.c_str());
}

ResultSet* MySqlDatabase::executeQuery(const stringstream& statement) {
	return executeQuery(statement.str().c_str());
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
	stringstream msg;
	msg << mysql_errno(&mysql) << ": " << mysql_error(&mysql);
	Logger::error(msg);

	unlock();

	throw DatabaseException(msg.str());
}

void MySqlDatabase::error(const char* query) {
	stringstream msg;
	msg << "DatabaseException caused by query: " << query << "\n" << mysql_errno(&mysql) << ": " << mysql_error(&mysql);
	//Logger::error(msg);

	unlock();

	throw DatabaseException(msg.str());
}

void MySqlDatabase::escapeString(string& s) {
	if (s.size() == 0)
		return;

	for (unsigned int i = 0; i < s.size(); i++) {
		switch (s[i]) {
		case '\0': // Must be escaped for "mysql"
			s[i] = '\\';
			s.insert(++i, "0", 1);
			break;
		case '\n': // Must be escaped for logs
			s[i] = '\\';
			s.insert(++i, "n", 1);
			break;
		case '\r':
			s[i] = '\\';
			s.insert(++i, "r", 1);
			break;
		case '\\':
			s[i] = '\\';
			s.insert(++i, "\\", 1);
			break;
		case '\"':
			s[i] = '\\';
			s.insert(++i, "\"", 1);
			break;
		case '\'': // Better safe than sorry
			s[i] = '\\';
			s.insert(++i, "\'", 1);
			break;
		/*case '\032': // This gives problems on Win32
			s[i] = '\\';
			s.insert(++i, "Z", 1);
			break;*/
		default:
			break;
		}
	}
}

void MySqlDatabase::finalizeLibrary() {
	mysql_library_end();
}
