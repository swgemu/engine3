/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * CursorConfig.h
 *
 *  Created on: 15/02/2010
 *      Author: victor
 */

#ifndef CURSORCONFIG_H_
#define CURSORCONFIG_H_

#include "system/platform.h"

#include <db.h>

namespace engine {
  namespace db {
    namespace berkeley {

	class CursorConfig {
		uint32 flags;

	public:
		static CursorConfig DEFAULT;

	public:
		CursorConfig() {
			flags = 0;
		}

		CursorConfig(uint32 flag) {
			flags = flag;
		}

		/**
		 * Specify that the cursor will be used to do bulk operations on the underlying database.
		 */

		/*void setBulkCursor(bool bulkCursor) { DB_BULK not declared?
			if (bulkCursor)
				flags |= DB_BULK;
			else
				flags &= ~DB_BULK;
		}*/

		/**
		 * Configure the cursor for read committed isolation.
		 */
		void setReadCommitted(bool readCommitted) {
			if (readCommitted)
				flags |= DB_READ_COMMITTED;
			else
				flags &= ~DB_READ_COMMITTED;
		}

		/**
		 * Configure read operations performed by the cursor to return modified but not yet committed data.
		 */
		void setReadUncommitted(bool readUncommitted) {
			if (readUncommitted)
				flags |= DB_READ_UNCOMMITTED;
			else
				flags &= ~DB_READ_UNCOMMITTED;
		}

		/**
		 * Configure read operations performed by the cursor to return data as it was when the cursor opened without locking, if DatabaseConfig.setMultiversion(boolean) was configured.
		 */
		void setSnapshot(bool snapshot) {
			if (snapshot)
				flags |= DB_TXN_SNAPSHOT;
			else
				flags &= ~DB_TXN_SNAPSHOT;
		}

		/**
		 * Specify the Concurrent Data Store environment cursor will be used to update the database.
		 */
		void setWriteCursor(bool writeCursor) {
			if (writeCursor)
				flags |= DB_WRITECURSOR;
			else
				flags &= ~DB_WRITECURSOR;
		}

		inline uint32 getFlags() const {
			return flags;
		}

	};

    } // namespace berkeley
  } // namespace db
} // namespace engine

#endif /* CURSORCONFIG_H_ */
