/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef LUAOBJECT_H_
#define LUAOBJECT_H_

#include "Lua.h"

namespace engine {
namespace lua {

	class LuaObject {
		lua_State* L;

		String objectName;

	public:
		LuaObject(lua_State* lState, const String& name) {
			L = lState;
			objectName = name;
		}

		LuaObject(lua_State* lState) {
			L = lState;
		}

		void operator=(lua_State* lState) {
			L = lState;
		}

		String getStringField(const String& key);
		sys::uint32 getIntField(const String& key);
		sys::uint16 getShortField(const String& key);
		sys::uint8 getByteField(const String& key);
		float getFloatField(const String& key);
		uint64 getLongField(const String& key);
		LuaObject getObjectField(const String& key);

		void setField(const String& key, sys::uint32 value);
		void setField(const String& key, const String& value);

		bool isValidTable();

		void pop();

	};
}
}


#endif /* LUAOBJECT_H_ */
