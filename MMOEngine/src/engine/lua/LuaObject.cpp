/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "Lua.h"
#include "LuaObject.h"

LuaObject::LuaObject(Lua* lState, const String& name) : L(lState->getLuaState()), objectName(name) {

}

LuaObject::LuaObject(Lua* lState) : L(lState->getLuaState()) {

}

LuaObject::LuaObject(lua_State* lState, const String& name) : L(lState), objectName(name) {

}

LuaObject::LuaObject(lua_State* lState) : L(lState) {

}

void LuaObject::setField(const String& key, uint64 value) {
	lua_pushstring(L, key.toCharArray());
	lua_pushinteger(L, value);
	lua_settable(L, -3);
}

void LuaObject::setField(const String& key, const String& value) {
	lua_pushstring(L, key.toCharArray());
	lua_pushstring(L, value.toCharArray());
	lua_settable(L, -3);
}

String LuaObject::getStringField(const String& key, const char* defaultValue) {
	const char* result = nullptr;

	lua_pushstring(L, key.toCharArray());
	lua_gettable(L, -2);
	std::size_t size = 0;

	if (lua_isnil(L, -1))
		result = defaultValue;
	else {
		result = lua_tostring(L, -1);
		size = lua_rawlen(L, -1);
	}

	String val;

	if (result != nullptr)
		val = String(result, size);
	else
		val = String(defaultValue);

	lua_pop(L, 1);

	return val;
}

bool LuaObject::getBooleanField(const String& key, bool defaultValue) {
	lua_pushstring(L, key.toCharArray());
	lua_gettable(L, -2);

	bool res = defaultValue;

	if (!lua_isnil(L, -1))
		res = lua_toboolean(L, -1);

	lua_pop(L, 1);

	return res;
}

uint32 LuaObject::getIntField(const String& key, uint32 defaultValue) {
	uint32 result = defaultValue;

	lua_pushstring(L, key.toCharArray());
	lua_gettable(L, -2);

	if (!lua_isnil(L, -1))
		result = (uint32)lua_tointeger(L, -1);

	lua_pop(L, 1);

	return result;
}

int32 LuaObject::getSignedIntField(const String& key, int32 defaultValue) {
	int32 result = defaultValue;

	lua_pushstring(L, key.toCharArray());
	lua_gettable(L, -2);

	if (!lua_isnil(L, -1))
		result = (int32)lua_tointeger(L, -1);

	lua_pop(L, 1);

	return result;
}

uint16 LuaObject::getShortField(const String& key, uint16 defaultValue) {
	uint16 result = defaultValue;

	lua_pushstring(L, key.toCharArray());
	lua_gettable(L, -2);

	if (!lua_isnil(L, -1))
		result = (uint16)lua_tointeger(L, -1);

	lua_pop(L, 1);

	return result;
}

uint8 LuaObject::getByteField(const String& key, uint8 defaultValue) {
	uint8 result = defaultValue;

	lua_pushstring(L, key.toCharArray());
	lua_gettable(L, -2);

	if (!lua_isnil(L, -1))
		result = (uint8)lua_tointeger(L, -1);

	lua_pop(L, 1);

	return result;
}

float LuaObject::getFloatField(const String& key, float defaultValue) {
	float result = defaultValue;

	lua_pushstring(L, key.toCharArray());
	lua_gettable(L, -2);

	if (!lua_isnil(L, -1))
		result = (float)lua_tonumber(L, -1);

	lua_pop(L, 1);

	return result;
}

double LuaObject::getDoubleField(const String& key, double defaultValue) {
	double result = defaultValue;

	lua_pushstring(L, key.toCharArray());
	lua_gettable(L, -2);

	if (!lua_isnil(L, -1))
		result = (double)lua_tonumber(L, -1);

	lua_pop(L, 1);

	return result;
}

uint64 LuaObject::getLongField(const String& key, uint64 defaultValue) {
	uint64 result = defaultValue;

	lua_pushstring(L, key.toCharArray());
	lua_gettable(L, -2);

	if (!lua_isnil(L, -1))
		result = (uint64)lua_tointeger(L, -1);

	lua_pop(L, 1);

	return result;
}

LuaObject LuaObject::getObjectField(const String& key) {
	lua_pushstring(L, key.toCharArray());
	lua_gettable(L, -2);

	LuaObject obj(L);

	return obj;
}

LuaObject LuaObject::getObjectAt(int idx) {
	lua_rawgeti(L, -1, idx);

	LuaObject obj(L);

	return obj;
}

sys::uint32 LuaObject::getIntAt(int idx) {
	uint32 result = 0;

	if (idx > (int)getTableSize() || idx < 1)
		throw ArrayIndexOutOfBoundsException(idx);

	lua_rawgeti(L, -1, idx);
	result = (uint32)lua_tointeger(L, -1);
	lua_pop(L, 1);

	return result;
}

sys::int32 LuaObject::getSignedIntAt(int idx) {
	int32 result = 0;

	if (idx > (int)getTableSize() || idx < 1)
		throw ArrayIndexOutOfBoundsException(idx);

	lua_rawgeti(L, -1, idx);
	result = (int32)lua_tointeger(L, -1);
	lua_pop(L, 1);

	return result;
}

sys::uint64 LuaObject::getLongAt(int idx) {
	uint64 result = 0;

	if (idx > (int)getTableSize() || idx < 1)
		throw ArrayIndexOutOfBoundsException(idx);

	lua_rawgeti(L, -1, idx);
	result = (uint64)lua_tointeger(L, -1);
	lua_pop(L, 1);

	return result;
}

bool LuaObject::getBooleanAt(int idx) {
	bool result = 0;

	if (idx > (int)getTableSize() || idx < 1)
		throw ArrayIndexOutOfBoundsException(idx);

	lua_rawgeti(L, -1, idx);
	result = (bool)lua_toboolean(L, -1);
	lua_pop(L, 1);

	return result;
}

String LuaObject::getStringAt(int idx) {
	const char* result = nullptr;

	if (idx > getTableSize() || idx < 1)
		throw ArrayIndexOutOfBoundsException(idx);

	lua_rawgeti(L, -1, idx);
	result = lua_tostring(L, -1);
	auto size = lua_rawlen(L, -1);

	String val(result, size);

	lua_pop(L, 1);

	return val;
}

float LuaObject::getFloatAt(int idx) {
	float result = 0;

	if (idx > getTableSize() || idx < 1)
		throw ArrayIndexOutOfBoundsException(idx);

	lua_rawgeti(L, -1, idx);
	result = (float)lua_tonumber(L, -1);
	lua_pop(L, 1);

	return result;
}

double LuaObject::getDoubleAt(int idx) {
	double result = 0;

	if (idx > getTableSize() || idx < 1)
		throw ArrayIndexOutOfBoundsException(idx);

	lua_rawgeti(L, -1, idx);
	result = (double)lua_tonumber(L, -1);
	lua_pop(L, 1);

	return result;
}

int LuaObject::getTableSize() {
#if LUA_VERSION_NUM > 501
	return (int) lua_rawlen(L, -1);
#else
	return luaL_getn(L, -1);
#endif
}

bool LuaObject::isValidTable() {
	return lua_istable(L, -1);
}

void LuaObject::pop() {
	lua_pop(L, 1);
}
