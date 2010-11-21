/*
 * ObjectDatabaseManager.cpp
 *
 *  Created on: 12/11/2010
 *      Author: victor
 */

#include "ObjectDatabaseManager.h"

using namespace engine::db;
using namespace engine::db::berkley;

ObjectDatabase* ObjectDatabaseManager::loadObjectDatabase(const String& name, bool create, uint16 uniqueID) {
	return dynamic_cast<ObjectDatabase*>(instantiateDatabase(name, create, uniqueID, true));
}
