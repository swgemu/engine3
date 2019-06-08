/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * databasetests.h
 *
 *  Created on: 26/02/2013
 *      Author: victor
 */

#ifndef DATABASETESTS_H_
#define DATABASETESTS_H_

#include "engine/engine.h"

void databaseTests() {
	ObjectDatabaseManager::instance()->loadDatabases();
	LocalDatabase* localDatabase = ObjectDatabaseManager::instance()->loadLocalDatabase("testcompression", true, 0xFFFF, false);

	localDatabase->truncate();

	String testData = "Lorem ipsum dolor sit amet";

	printf("adding values to database\n");

	for (int i = 0; i < 100; ++i) {
		ObjectOutputStream* key = new ObjectOutputStream();

		TypeInfo<int>::toBinaryStream(&i, key);

		ObjectOutputStream* data = new ObjectOutputStream();

		testData = String::valueOf(System::random()) + String::valueOf(System::random()) + String::valueOf(System::random()) + String::valueOf(System::random());
		testData.toBinaryStream(data);

		localDatabase->putData(key, data);
	}

	ObjectDatabaseManager::instance()->commitLocalTransaction();

	printf("compressing database\n");

	engine::db::berkley::Transaction* transaction = ObjectDatabaseManager::instance()->startTransaction();

	ObjectDatabaseManager::instance()->compressDatabase("testcompression", transaction);

	ObjectDatabaseManager::instance()->commitLocalTransaction(transaction);
	ObjectDatabaseManager::instance()->commitTransaction(transaction);

	printf("database compressed\n");

	printf("running tests\n");

	LocalDatabaseIterator iterator(localDatabase);

	ObjectInputStream keyStream;
	ObjectInputStream data;

	int values = 0;

	while (iterator.getNextKeyAndValue(&keyStream, &data)) {
		String value;
		value.parseFromBinaryStream(&data);

		//assert(value == testData);

		printf("%s\n", value.toCharArray());

		keyStream.clear();
		data.clear();

		++values;
	}

	assert(values == 100);

	printf("tests success\n");
}


#endif /* DATABASETESTS_H_ */
