/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * ObjectDatabaseManager.h
 *
 *  Created on: 11/11/2010
 *      Author: victor
 */

#ifndef OBJECTDATABASEMANAGER_H_
#define OBJECTDATABASEMANAGER_H_

#include "DatabaseManager.h"

namespace engine {
 namespace db {

 class ObjectDatabaseManager : public Singleton<ObjectDatabaseManager>, public DatabaseManager {
 public:
	 /**
	  * Loads or creates a database in this environment
	  * @param name name of the database
	  * @param create create if no database exists with specified uniqueID/name pair
	  * @param uniqueID if not specified, uniqueID will be set to name.hashCode()
	  */
	 ObjectDatabase* loadObjectDatabase(const String& name, bool create, uint16 uniqueID = 0xFFFF, bool compression = true);
 };


 }
}


#endif /* OBJECTDATABASEMANAGER_H_ */
