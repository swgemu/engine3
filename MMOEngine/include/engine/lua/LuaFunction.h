/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef LUAFUNCTION_H_
#define LUAFUNCTION_H_

#include "Lua.h"

namespace engine {
namespace lua {

	class LuaFunction {
		lua_State* L;

		int numberOfArgs;
		int numberOfArgsToReturn;
		String functionName;

	public:
		LuaFunction(lua_State* l, const String& funcName, int argsToReturn);

		LuaFunction(lua_State* l, const String& object, const String& func, int argsToReturn);

		void operator<<(int number);
		void operator<<(sys::uint32 number);
		void operator<<(int64 number);
		void operator<<(uint64 number);
		void operator<<(bool boolean);
		void operator<<(String& str);
		void operator<<(const char* str);

		inline lua_State* getLuaState() {
			return L;
		}

		inline int getNumberOfArgs() {
			return numberOfArgs;
		}

		inline int getNumberOfReturnArgs() {
			return numberOfArgsToReturn;
		}

		inline String& getFunctionName() {
			return functionName;
		}

	};
}
}

#endif /* LUAFUNCTION_H_ */
