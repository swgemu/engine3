/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef LUAFUNCTION_H_
#define LUAFUNCTION_H_

#include "Lua.h"

namespace engine {
namespace lua {

	class LuaFunction : public Object {
		lua_State* L;

		int numberOfArgs;
		int numberOfArgsToReturn;

		String functionName;
		String object;

	public:
		//Empty constructor, only to use in testing
		LuaFunction();

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

#endif /* LUAFUNCTION_H_ */
