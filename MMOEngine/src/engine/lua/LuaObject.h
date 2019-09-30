/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#pragma once

#include <lua.hpp>

#include "system/lang.h"

namespace engine {
namespace lua {
	class Lua;

	class LuaObject {
		lua_State* L;

		String objectName;

	public:
		LuaObject(Lua* lState, const String& name);
		LuaObject(Lua* lState);

		LuaObject(lua_State* lState, const String& name);
		LuaObject(lua_State* lState);

#ifdef CXX11_COMPILER
		LuaObject(const LuaObject& obj) : L(obj.L), objectName(obj.objectName) {
		}

		LuaObject(LuaObject&& obj) : L(obj.L), objectName(std::move(obj.objectName)) {
			obj.L = nullptr;
		}

		LuaObject& operator=(const LuaObject& obj) {
			if (this == &obj)
				return *this;

			L = obj.L;
			objectName = obj.objectName;

			return *this;
		}

		LuaObject& operator=(LuaObject&& obj) {
			if (this == &obj)
				return *this;

			L = obj.L;
			objectName = std::move(obj.objectName);

			obj.L = nullptr;

			return *this;
		}
#endif

		virtual ~LuaObject() {

		}

		virtual String getStringField(const String& key, const char* defaultValue = "");
		virtual sys::uint32 getIntField(const String& key, sys::uint32 defaultValue = 0);
		virtual sys::int32 getSignedIntField(const String& key, sys::int32 defaultValue = 0);
		virtual sys::uint16 getShortField(const String& key, sys::uint16 defaultValue = 0);
		virtual sys::uint8 getByteField(const String& key, sys::uint8 defaultValue = 0);
		virtual float getFloatField(const String& key, float defaultValue = 0);
		virtual double getDoubleField(const String& key, double defaultValue = 0);
		virtual sys::uint64 getLongField(const String& key, sys::uint64 defaultValue = 0);
		virtual LuaObject getObjectField(const String& key);
		virtual bool getBooleanField(const String& key, bool defaultValue = false);

		virtual void setField(const String& key, sys::uint64 value);
		virtual void setField(const String& key, const String& value);

		virtual String getStringAt(int idx);
		virtual sys::uint32 getIntAt(int idx);
		virtual sys::int32 getSignedIntAt(int idx);
		virtual float getFloatAt(int idx);
		virtual double getDoubleAt(int idx);
		virtual LuaObject getObjectAt(int idx);
		virtual sys::uint64 getLongAt(int idx);
		virtual bool getBooleanAt(int idx);


		virtual int getTableSize();

		virtual bool isValidTable();

		virtual void pop();

		inline lua_State* getLuaState() {
			return L;
		}

		inline void operator=(lua_State* lState) {
			L = lState;
		}

	};
}
}

using namespace engine::lua;

