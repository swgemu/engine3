/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "LuaFunction.h"

#include "LuaPanicException.h"

LuaFunction::LuaFunction() {
	L = NULL;
	numberOfArgs = 0;
	numberOfArgsToReturn = 0;
}

LuaFunction::LuaFunction(lua_State* l, const String& funcName, int argsToReturn) {
	L = l;
	functionName = funcName;
	numberOfArgs = 0;
	numberOfArgsToReturn = argsToReturn;

	lua_getglobal(L, functionName.toCharArray());
}

LuaFunction::LuaFunction(lua_State* l, const String& object, const String& func, int argsToReturn) {
	L = l;
	functionName = func;
	numberOfArgs = 1;
	numberOfArgsToReturn = argsToReturn;

	lua_getglobal(L, object.toCharArray());

	/* get func key */
	lua_pushstring(L, func.toCharArray());
	lua_gettable(L, -2);  /* funx resides at stack index -2 */
	lua_insert(L, -2);  /* and swap with func... */
}

LuaFunction::LuaFunction(const LuaFunction& func) : Object() {
	L = func.L;
	numberOfArgs = func.numberOfArgs;
	numberOfArgsToReturn = func.numberOfArgsToReturn;
	functionName = func.functionName;
}

LuaFunction& LuaFunction::operator=(const LuaFunction& func) {
	if (this == &func)
		return *this;

	L = func.L;
	numberOfArgs = func.numberOfArgs;
	numberOfArgsToReturn = func.numberOfArgsToReturn;
	functionName = func.functionName;

	return *this;
}


LuaFunction::~LuaFunction() {

}

void LuaFunction::operator<<(int number) {
	numberOfArgs++;
	lua_pushnumber(L, number);
}

void LuaFunction::operator<<(sys::uint32 number) {
	numberOfArgs++;
	lua_pushnumber(L, number);
}

void LuaFunction::operator<<(int64 number) {
	numberOfArgs++;
	lua_pushinteger(L, number);
}

void LuaFunction::operator<<(uint64 number) {
	numberOfArgs++;
	lua_pushinteger(L, number);
}

void LuaFunction::operator<<(float number) {
	numberOfArgs++;
	lua_pushnumber(L, number);
}

void LuaFunction::operator<<(double number) {
	numberOfArgs++;
	lua_pushnumber(L, number);
}

void LuaFunction::operator<<(bool boolean) {
	numberOfArgs++;
	lua_pushboolean(L, boolean);
}

void LuaFunction::operator<<(String& str) {
	numberOfArgs++;
	lua_pushstring(L, str.toCharArray());
}

void LuaFunction::operator<<(const char* str) {
	numberOfArgs++;
	lua_pushstring(L, str);
}

void LuaFunction::operator<<(void* ptr) {
	numberOfArgs++;

	if (ptr != NULL)
		lua_pushlightuserdata(L, ptr);
	else
		lua_pushnil(L);
}

lua_State* LuaFunction::callFunction() {
	try {
		if (lua_pcall(getLuaState(), getNumberOfArgs(), getNumberOfReturnArgs(), 0) != 0) {
			Logger::console.error("Error running function " + getFunctionName() + " " + String(lua_tostring(getLuaState(), -1)));
			return NULL;
		}
	} catch (LuaPanicException& e) {
		Logger::console.error("LuaPanicException running function " + getFunctionName() + " " + String(lua_tostring(getLuaState(), -1)));

		return NULL;
	}

	return getLuaState();
}

