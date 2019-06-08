/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#ifndef RESULTSET_H_
#define RESULTSET_H_

#include "system/lang.h"

namespace engine {
  namespace db {

	class ResultSet : public Object {
	public:
		ResultSet() {
		}

		virtual ~ResultSet() {
		}

		virtual bool next() = 0;

		virtual bool getBoolean(int index) = 0;

		virtual int getInt(int index) = 0;

		virtual sys::uint32 getUnsignedInt(int index) = 0;

		virtual sys::int64 getLong(int index) = 0;

		virtual sys::uint64 getUnsignedLong(int index) = 0;

		virtual float getFloat(int index) = 0;

		virtual char* getString(int index) = 0;

		virtual sys::uint64 getRowsAffected() = 0;

		virtual sys::uint64 getLastAffectedRow() = 0;

		virtual sys::uint64 size() = 0;
	};

  } // namespace db
} // namespace engine

using namespace engine::db;

#endif /*RESULTSET_H_*/
