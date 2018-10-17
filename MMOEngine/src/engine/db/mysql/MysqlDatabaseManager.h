/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef MYSQLDATABASEMANAGER_H_
#define MYSQLDATABASEMANAGER_H_

#include "system/lang.h"

#include "engine/util/Singleton.h"

#include "MySqlDatabase.h"

namespace engine {
  namespace db {
    namespace mysql {

    class MysqlDatabaseManager : public Singleton<MysqlDatabaseManager>, public Object {
    	ThreadLocal<VectorSet<MySqlDatabase*>* > modifiedDatabases;

    public:
    	void commitModifiedDatabases();
    	void rollbackModifiedDatabases();

    	void addModifiedDatabase(MySqlDatabase* database);

    protected:
    	VectorSet<MySqlDatabase*>* getLocalModifiedDatabases();

    };

    } // namespace mysql
  } // namespace db
} // namespace engine

using namespace engine::db::mysql;

#endif /* MYSQLDATABASEMANAGER_H_ */
