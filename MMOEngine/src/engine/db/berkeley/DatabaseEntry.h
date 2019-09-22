/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * DatabaseEntry.h
 *
 *  Created on: 14/02/2010
 *      Author: victor
 */

#ifndef DATABASEENTRY_H_
#define DATABASEENTRY_H_

#include <db.h>

#include "system/platform.h"
#include "system/io/Stream.h"

namespace engine {
  namespace db {
    namespace berkeley {

	class DatabaseEntry {
	protected:
		DBT databaseEntry; //BDB

	public:
		/**
		 *  Construct a DatabaseEntry with null data.
		 */
		DatabaseEntry();

		/**
		 * deletes from memory databaseEntyr.data if setUserBuffer(false)
		 */
		~DatabaseEntry();

		/**
		 * Construct a DatabaseEntry with a given byte array
		 */
		DatabaseEntry(uint8* data, int size);

		/**
		 * Constructs a DatabaseEntry with a given byte array, offset and size
		 */
		//DatabaseEntry(byte* data, int offset, int size);

		/**
		 * Construct a DatabaseEntry with a given native I/O buffer
		 */
		DatabaseEntry(Stream* data);

		/**
		 * Configures the entry with an application-owned buffer.
		 */
		void setUserBuffer(int length, bool usermem) {
			databaseEntry.ulen = length;

			if (usermem) {
				databaseEntry.flags |= DB_DBT_USERMEM;
				databaseEntry.flags &= ~DB_DBT_MALLOC;
				databaseEntry.flags &= ~DB_DBT_REALLOC;
			} else {
				databaseEntry.flags &= ~DB_DBT_USERMEM;
				databaseEntry.flags &= ~DB_DBT_REALLOC;
				databaseEntry.flags |= DB_DBT_MALLOC;
			}
		}

		/*
		 * Configures the entry to try to reuse the buffer before allocating a new one.
		 */
		void setReuseBuffer(bool reuse) {
			if (reuse) {
				databaseEntry.flags &= ~DB_DBT_MALLOC;
				databaseEntry.flags &= ~DB_DBT_USERMEM;
				databaseEntry.flags |= DB_DBT_REALLOC;
			} else {
				databaseEntry.flags &= ~DB_DBT_REALLOC;
				databaseEntry.flags &= ~DB_DBT_USERMEM;
				databaseEntry.flags |= DB_DBT_MALLOC;
			}
		}

		/**
		 * Sets the user buffer and size
		 */
		void setData(void* addr, int size);

		inline void* getData() {
			return databaseEntry.data;
		}

		inline int getSize() {
			return databaseEntry.size;
		}

		inline DBT* getDBT() {
			return &databaseEntry;
		}
	};

    } // namespace berkeley
  } // namespace db
} // namespace engine

#endif /* DATABASEENTRY_H_ */
