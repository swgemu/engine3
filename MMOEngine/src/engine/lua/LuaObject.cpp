/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "LuaObject.h"

void LuaObject::setField(const String& key, uint64 value) {
	lua_pushstring(L, key.toCharArray());
	lua_pushinteger(L, (double)value);
	lua_settable(L, -3);
}

void LuaObject::setField(const String& key, const String& value) {
	lua_pushstring(L, key.toCharArray());
	lua_pushstring(L, value.toCharArray());
	lua_settable(L, -3);
}

String LuaObject::getStringField(const String& key) {
	const char* result = NULL;

	lua_pushstring(L, key.toCharArray());
	lua_gettable(L, -2);

	result = lua_tostring(L, -1);
	lua_pop(L, 1);

	if (result != NULL)
		return String(result);
	else
		return String("");
}

uint32 LuaObject::getIntField(const String& key) {
	uint32 result = 0;

	lua_pushstring(L, key.toCharArray());
	lua_gettable(L, -2);

	result = (uint32)lua_tonumber(L, -1);
	lua_pop(L, 1);

	return result;
}

uint16 LuaObject::getShortField(const String& key) {
	uint16 result = 0;

	lua_pushstring(L, key.toCharArray());
	lua_gettable(L, -2);

	result = (uint16)lua_tonumber(L, -1);
	lua_pop(L, 1);

	return result;
}

uint8 LuaObject::getByteField(const String& key) {
	uint8 result = 0;

	lua_pushstring(L, key.toCharArray());
	lua_gettable(L, -2);

	result = (uint8)lua_tonumber(L, -1);
	lua_pop(L, 1);

	return result;
}

float LuaObject::getFloatField(const String& key) {
	float result = 0;

	lua_pushstring(L, key.toCharArray());
	lua_gettable(L, -2);

	result = (float)lua_tonumber(L, -1);
	lua_pop(L, 1);

	return result;
}

uint64 LuaObject::getLongField(const String& key) {
	uint64 result = 0;

	lua_pushstring(L, key.toCharArray());
	lua_gettable(L, -2);

	result = (uint64)lua_tonumber(L, -1);
	lua_pop(L, 1);

	return result;
}

LuaObject LuaObject::getObjectField(const String& key) {
	lua_pushstring(L, key.toCharArray());
	lua_gettable(L, -2);

	LuaObject obj(L);

	return obj;
}

sys::uint32 LuaObject::getIntAt(int idx) {
	uint32 result = 0;

	if (idx > (int)getTableSize() || idx < 1)
		throw ArrayIndexOutOfBoundsException(idx);

	lua_rawgeti(L, -1, idx);
	result = (uint32)lua_tonumber(L, -1);
	lua_pop(L, 1);

	return result;
}

String LuaObject::getStringAt(int idx) {
	const char* result = NULL;

	if (idx > getTableSize() || idx < 1)
		throw ArrayIndexOutOfBoundsException(idx);

	lua_rawgeti(L, -1, idx);
	result = lua_tostring(L, -1);
	lua_pop(L, 1);

	return String(result);
}

float LuaObject::getFloatAt(int idx) {
	float result = 0.f;

	if (idx > getTableSize() || idx < 1)
		throw ArrayIndexOutOfBoundsException(idx);

	lua_rawgeti(L, -1, idx);
	result = (float)lua_tonumber(L, -1);
	lua_pop(L, 1);

	return result;
}

int LuaObject::getTableSize() {
	return luaL_getn(L, -1);
}

bool LuaObject::isValidTable() {
	return lua_istable(L, -1);
}

void LuaObject::pop() {
	lua_pop(L, 1);
}
