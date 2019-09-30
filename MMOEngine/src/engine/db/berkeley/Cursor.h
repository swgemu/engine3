/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * Cursor.h
 *
 *  Created on: 15/02/2010
 *      Author: victor
 */

#pragma once

#include <db.h>
#include "CursorConfig.h"
#include "DatabaseConfig.h"

namespace engine {
  namespace db {
    namespace berkeley {

	class BerkeleyDatabase;
	class DatabaseEntry;

	class Cursor {
	protected:
		CursorConfig config;
		BerkeleyDatabase* database;

		DBC* dbc;

	protected:
		Cursor(BerkeleyDatabase* database, DBC* dbcursor, const CursorConfig& config = CursorConfig::DEFAULT);

	public:
		~Cursor();

		int close();

		/**
		 * Delete the key/data pair to which the cursor refers
		 */
		int del(uint32 flags = 0);

		/**
		 * Return a new cursor with the same transaction and locker ID as the original cursor.
		 */
		Cursor* dup(bool samePosition);

		/**
		 * Returns the key/data pair to which the cursor refers.
		 */
		inline int getCurrent(DatabaseEntry* key, DatabaseEntry* data, uint32 lockMode = LockMode::DEFAULT) {
			return get(key, data, DB_CURRENT | lockMode);
		}

		/**
		 * Move the cursor to the first key/data pair of the database, and return that pair.
		 */
		inline int getFirst(DatabaseEntry* key, DatabaseEntry* data, uint32 lockMode = LockMode::DEFAULT) {
			return get(key, data, DB_FIRST | lockMode);
		}

		/**
	         * Move the cursor to the last key/data pair of the database, and return that pair.
		 */
		inline int getLast(DatabaseEntry* key, DatabaseEntry* data, uint32 lockMode = LockMode::DEFAULT) {
			return get(key, data, DB_LAST | lockMode);
		}

		/**
		 * Move the cursor to the next key/data pair and return that pair.
		 */
		inline int getNext(DatabaseEntry* key, DatabaseEntry* data, uint32 lockMode = LockMode::DEFAULT) {
			return get(key, data, DB_NEXT | lockMode);
		}

		inline int pGetSet(DatabaseEntry* key, DatabaseEntry* primaryKey, DatabaseEntry* data, uint32 lockMode = LockMode::DEFAULT) {
			return pget(key, primaryKey, data, DB_SET | lockMode);
		}

		inline int pGetNext(DatabaseEntry* key, DatabaseEntry* primaryKey, DatabaseEntry* data, uint32 lockMode = LockMode::DEFAULT) {
			return pget(key, primaryKey, data, DB_NEXT | lockMode);
		}

		/**
		 * Move the cursor to the next key/data pair and return that pair.
		 */
		inline int getNextMultiple(DatabaseEntry* key, DatabaseEntry* data, uint32 lockMode = LockMode::DEFAULT) {
			return get(key, data, DB_MULTIPLE_KEY | DB_NEXT | lockMode);
		}

		/**
		 * If the next key/data pair of the database is a duplicate data record for the current key/data pair, move the cursor to the next key/data pair of the database and return that pair.
		 */
		inline int getNextDup(DatabaseEntry* key, DatabaseEntry* data, uint32 lockMode = LockMode::DEFAULT) {
			return get(key, data, DB_NEXT_DUP | lockMode);
		}

		inline int pGetNextDup(DatabaseEntry* key, DatabaseEntry* primaryKey, DatabaseEntry* data, uint32 lockMode = LockMode::DEFAULT) {
			return pget(key, primaryKey, data, DB_NEXT_DUP | lockMode);
		}

		/**
		 * Move the cursor to the next non-duplicate key/data pair and return that pair.
		 */

		inline int getNextNoDup(DatabaseEntry* key, DatabaseEntry* data, uint32 lockMode = LockMode::DEFAULT) {
			return get(key, data, DB_NEXT_NODUP | lockMode);
		}


		/**
		 * Move the cursor to the previous key/data pair and return that pair.
		 */
		inline int getPrev(DatabaseEntry* key, DatabaseEntry* data, uint32 lockMode = LockMode::DEFAULT) {
			return get(key, data, DB_PREV | lockMode);
		}

		/**
		 * If the previous key/data pair of the database is a duplicate data record for the current key/data pair, move the cursor to the previous key/data pair of the database and return that pair.
		 */
		inline int getPrevDup(DatabaseEntry* key, DatabaseEntry* data, uint32 lockMode = LockMode::DEFAULT) {
			return get(key, data, DB_PREV_DUP | lockMode);
		}

		/**
		 * Move the cursor to the previous non-duplicate key/data pair and return that pair.
		 */
		inline int getPrevNoDup(DatabaseEntry* key, DatabaseEntry* data, uint32 lockMode = LockMode::DEFAULT) {
			return get(key, data, DB_PREV_NODUP | lockMode);
		}

		/**
		 * Return the record number associated with the cursor.
		 */
		int getRecordNumber(DatabaseEntry* data, uint32 lockMode = LockMode::DEFAULT);

		/**
		 * Move the cursor to the specified key/data pair, where both the key and data items must match.
		 */
		inline int getSearchBoth(DatabaseEntry* key, DatabaseEntry* data, uint32 lockMode = LockMode::DEFAULT) {
			return get(key, data, DB_GET_BOTH | lockMode);
		}

		/**
		 * Move the cursor to the specified key and closest matching data item of the database.
		 */
		inline int getSearchBothRange(DatabaseEntry* key, DatabaseEntry* data, uint32 lockMode = LockMode::DEFAULT) {
			return get(key, data, DB_GET_BOTH_RANGE | lockMode);
		}

		/**
		 * Move the cursor to the given key of the database, and return the datum associated with the given key.
		 */
		inline int getSearchKey(DatabaseEntry* key, DatabaseEntry* data, uint32 lockMode = LockMode::DEFAULT) {
			return get(key, data, DB_SET | lockMode);
		}

		/**
		 * Move the cursor to the closest matching key of the database, and return the data item associated with the matching key.
		 */
		inline int getSearchKeyRange(DatabaseEntry* key, DatabaseEntry* data, uint32 lockMode = LockMode::DEFAULT) {
			return get(key, data, DB_SET_RANGE | lockMode);
		}

		/**
		 * Move the cursor to the specific numbered record of the database, and return the associated key/data pair.
		 */
		inline int getSearchRecordNumber(DatabaseEntry* key, DatabaseEntry* data, uint32 lockMode = LockMode::DEFAULT) {
			return get(key, data, DB_SET_RECNO | lockMode);
		}

		/**
		 * Store a key/data pair into the database.
		 */
		int put(DatabaseEntry* key, DatabaseEntry* data, uint32 flags);

		/**
		 * Store a key/data pair into the database.
		 */
		inline int putAfter(DatabaseEntry* key, DatabaseEntry* data) {
			return put(key, data, DB_AFTER);
		}

		/**
		 * Store a key/data pair into the database.
		 */
		inline int putBefore(DatabaseEntry* key, DatabaseEntry* data) {
			return put(key, data, DB_BEFORE);
		}

		/**
		 * Replaces the data in the key/data pair at the current cursor position.
		 */
		int putCurrent(DatabaseEntry* data);

		/**
		 * Store a key/data pair into the database.
		 */
		inline int putKeyFirst(DatabaseEntry* key, DatabaseEntry* data) {
			return put(key, data, DB_KEYFIRST);
		}

		/**
		 * Store a key/data pair into the database.
		 */
		inline int putKeyLast(DatabaseEntry* key, DatabaseEntry* data) {
			return put(key, data, DB_KEYLAST);
		}

		/**
		 * Store a key/data pair into the database.
		 */
		inline int putNoDupData(DatabaseEntry* key, DatabaseEntry* data) {
			return put(key, data, DB_NODUPDATA);
		}

		/**
		 * Store a key/data pair into the database.
		 */
		/*inline int putNoOverwrite(DatabaseEntry* key, DatabaseEntry* data) {

		}*/

		/**
		 * Return this cursor's configuration.
		 */
		inline CursorConfig getConfig() {
			return config;
		}

		/**
		 * Return the Database handle associated with this Cursor.
		 */
		inline BerkeleyDatabase* getDatabase() {
			return database;
		}

		int get(DatabaseEntry* key, DatabaseEntry* data, uint32 flags);
		int pget(DatabaseEntry* key, DatabaseEntry* primaryKey, DatabaseEntry* data, uint32 flags);

		friend class BerkeleyDatabase;

	};

    } // namespace berkeley
  } // namespace db
} // namespace engine

