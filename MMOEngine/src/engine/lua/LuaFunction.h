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

	class LuaFunction : public Object {
		lua_State* L;

		int numberOfArgs;
		int numberOfArgsToReturn;

		String functionName;
		String object;

	protected:
		void init(lua_State* l, const String& funcName, int argsToReturn);
		void init(lua_State* l, const String& object, const String& func, int argsToReturn);

	public:
		//Empty constructor, only to use in testing
		LuaFunction();

		LuaFunction(Lua* lua, const String& funcName, int argsToReturn);
		LuaFunction(Lua* lua, const String& object, const String& func, int argsToReturn);

		LuaFunction(lua_State* l, const String& funcName, int argsToReturn);
		LuaFunction(lua_State* l, const String& object, const String& func, int argsToReturn);

		LuaFunction(const LuaFunction& func);

		virtual ~LuaFunction();

		LuaFunction& operator=(const LuaFunction& func);

		virtual void operator<<(int number);
		virtual void operator<<(sys::uint32 number);
		virtual void operator<<(float number);
		virtual void operator<<(double number);
		virtual void operator<<(int64 number);
		virtual void operator<<(uint64 number);
		virtual void operator<<(bool boolean);
		virtual void operator<<(const String& str);
		virtual void operator<<(const char* str);
		virtual void operator<<(void* pointer);

		virtual lua_State* callFunction();

		inline lua_State* getLuaState() const {
			return L;
		}

		inline int getNumberOfArgs() const {
			return numberOfArgs;
		}

		inline int getNumberOfReturnArgs() const {
			return numberOfArgsToReturn;
		}

		inline String& getFunctionName() {
			return functionName;
		}

		inline const String& getFunctionName() const {
			return functionName;
		}

		inline String& getObject() {
			return object;
		}

		inline const String& getObject() const {
			return object;
		}

	};
}
}

using namespace engine::lua;

