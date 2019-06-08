/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * convertdatabases.h
 *
 *  Created on: 03/03/2013
 *      Author: victor
 */

#ifndef CONVERTDATABASES_H_
#define CONVERTDATABASES_H_

#include "engine/engine.h"

#include "engine/db/berkley/BerkeleyDatabase.h"
using namespace engine::db::berkley;

#define COMPRESS

void convertDatabase(const String& dbName, const String& targetDirectory = "");

void convertAllDatabases(const String& targetDirectory = "") {
	ObjectDatabaseManager::CONVERT_DATABASES = false;
	ObjectDatabaseManager::instance()->loadDatabases();

	int count = ObjectDatabaseManager::instance()->getTotalDatabaseCount();

	for (int i = 0; i < count; ++i) {
		LocalDatabase* db = ObjectDatabaseManager::instance()->getDatabase(i);

		if (!db->isObjectDatabase())
			continue;

		ObjectDatabase* objectDatabase = dynamic_cast<ObjectDatabase*>(db);

		String name;
		objectDatabase->getDatabaseName(name);
		convertDatabase(name, targetDirectory);
	}
}

void convertDatabase(const String& dbName, const String& targetDirectory) {
	DatabaseConfig config;
	config.setType(DatabaseType::HASH);
	//config.setThreaded(true);
		//config.setTransactional(true);
	config.setAllowCreate(true);

	String dir = targetDirectory;

	if (!dir.isEmpty() && !dir.endsWith("/"))
		dir += "/";

	//config.setReadUncommited(true);

	ObjectDatabaseManager::CONVERT_DATABASES = false;
	ObjectDatabaseManager::instance()->loadDatabases();

	ObjectDatabase* originalDatabase = ObjectDatabaseManager::instance()->loadObjectDatabase(dbName, false);

	printf("converting %s\n", dbName.toCharArray());

	LocalDatabaseIterator iterator(originalDatabase);

	ObjectInputStream key;
	ObjectInputStream data;

	ObjectDatabaseManager::instance()->commitLocalTransaction();

	engine::db::berkley::Transaction* tx = ObjectDatabaseManager::instance()->startTransaction();

	uint64 totalDbItems = originalDatabase->getDatabaseHandle()->count(false, tx);

	ObjectDatabaseManager::instance()->commitTransaction(tx);

	uint64 totalItemsConverted = 0;
#ifdef COMPRESS
	BerkeleyDatabase* db = new BerkeleyDatabase(dir + dbName + "_converted_compressed.db", dbName, config);
#else
	BerkeleyDatabase* db = new BerkeleyDatabase(dir + dbName + "_converted.db", dbName, config);
#endif

	int lastRatio = 0;

	if (totalDbItems) {
		printf("converting %llu items\n", totalDbItems);
		printf("\rConverted [%d%%]", lastRatio);
	} else {
		printf("\rConverted [0]");
	}

	while (iterator.getNextKeyAndValue(&key, &data)) {
		Stream* newData = nullptr;

		try {
			//data.readShort();

			//uint32 test = data.readInt();

			//if (test != 0x62b0f0cf) {
				//data.reset();

				newData = Serializable::convertToHashCodeNameMembers(&data);
			//}

#ifdef COMPRESS
			Stream* compressed = LocalDatabase::compress(newData);
			delete newData;
			newData = compressed;
#endif

		} catch (Exception& e) {
			if (newData)
				delete newData;

			printf("could not convert object\n");
			printf("%s\n", data.toStringData().toCharArray());

			newData = nullptr;
		}

		if (newData) {
			engine::db::berkley::DatabaseEntry keyEntry, dataEntry;
			keyEntry.setData(key.getBuffer(), key.size());

			dataEntry.setData(newData->getBuffer(), newData->size());

			int ret = db->put(nullptr, &keyEntry, &dataEntry);

			delete newData;

			assert(ret == 0);

			++totalItemsConverted;

			if (totalDbItems) {
				int currRatio = ((float)totalItemsConverted / (float)totalDbItems) * 100;

				if (currRatio != lastRatio) {
					lastRatio = currRatio;

					printf("\rConverted [%d%%]", lastRatio);
				}
			} else if ((totalItemsConverted % 10000) == 0) {
				printf("\rConverted [%llu] items", totalItemsConverted);
			}
		}

		key.clear();
		data.clear();
	}

	db->sync();
	db->close(false);

	printf("\nconverted %llu items\n", totalItemsConverted);
}


#endif /* CONVERTDATABASES_H_ */
