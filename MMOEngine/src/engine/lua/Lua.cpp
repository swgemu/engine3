/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "../log/Logger.h"

#include "Lua.h"

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

Lua::Lua() : Logger("Lua") {
	L = NULL;
}

Lua::~Lua() {
	deinit();
}

void Lua::init() {
	L = lua_open();
	luaL_openlibs(L);
}

void Lua::deinit() {
	if (L)
		lua_close(L);

	L = NULL;
}

bool Lua::runFile(const String& filename) {
	if (filename.isEmpty())
		return false;

	/*StringBuffer msg;
	msg << "Loading lua file: " << filename;
	info(msg);*/

	if (!L)
		init();

	if (luaL_loadfile(L, filename.toCharArray()) || lua_pcall(L, 0, 0, 0)) {
		const char* err = lua_tostring(L, -1);
		lua_pop(L,1);

		StringBuffer msg;
		msg << "ERROR " << err;
		info(msg);

		return false;
	}

	return true;
}

void Lua::runFile(const String& filename, lua_State* lState) {
	if (!filename.isEmpty()) {
		//System::out << "Loading lua file: " << filename << "\n";

		if (luaL_loadfile(lState, filename.toCharArray()) || lua_pcall(lState, 0, 0, 0)) {
			const char* err = lua_tostring(lState, -1);
			lua_pop(lState,1);

			System::out << "ERROR " << err << "\n";
		}
	}
}

bool Lua::runString(const String& str) {
	if (str.isEmpty())
		return false;

	StringBuffer msg;
	msg << "Loading lua String: " << str << "\n";
	info(msg);

	if (!L)
		init();

	if (luaL_loadbuffer(L, str.toCharArray(), str.length(), "command") || lua_pcall(L, 0, 0, 0)) {
		const char* err = lua_tostring(L, -1);
		lua_pop(L,1);

		StringBuffer msg;
		msg << "ERROR " << err;
		info(msg);

		return false;
	}

	return true;
}

lua_State* Lua::callFunction(LuaFunction* func) {
	if (lua_pcall(func->getLuaState(), func->getNumberOfArgs(), func->getNumberOfReturnArgs(), 0) != 0) {
		System::out << "Error running function " << func->getFunctionName() << " " << lua_tostring(func->getLuaState(), -1);
		return NULL;
	}

	return func->getLuaState();
}

// getters
bool Lua::getGlobalBoolean(const String& name) {
	String result = getGlobalString(name);

	if (result == "true")
		return true;
	else
		return false;
}

String Lua::getGlobalString(const String& name) {
	const char* result = NULL;

	lua_getglobal(L, name.toCharArray());

	if (!lua_isstring(L, -1)) {
		lua_pop(L, 1);

		StringBuffer msg;
		msg << "invalid String value for: " << name;
		info(msg);

		return String("");
	}

	result = lua_tostring(L, -1);
	lua_pop(L, 1);

	return result;
}

uint32 Lua::getGlobalInt(const String& name) {
	uint32 result = 0;

	lua_getglobal(L, name.toCharArray());

	if (!lua_isnumber(L, -1)) {
		lua_pop(L, 1);

		StringBuffer msg;
		msg << "invalid value for: " << name << " expected NUMBER";
		info(msg);

		return 0;
	}

	result = (uint32)(lua_tonumber(L, -1));
	lua_pop(L, 1);

	return result;
}

uint64 Lua::getGlobalLong(const String& name) {
	uint64 result = 0;

	lua_getglobal(L, name.toCharArray());

	if (!lua_isnumber(L, -1)) {
		lua_pop(L, 1);

		StringBuffer msg;
		msg << "invalid value for: " << name << " expected NUMBER";
		info(msg);

		return 0;
	}

	result = (uint64) (lua_tonumber(L, -1));
	lua_pop(L, 1);

	return result;
}

uint16 Lua::getGlobalShort(const String& name) {
	uint16 result = 0;

	lua_getglobal(L, name.toCharArray());

	if (!lua_isnumber(L, -1)) {
		lua_pop(L, 1);

		StringBuffer msg;
		msg << "invalid value for: " << name << " expected NUMBER";
		info(msg);

		return 0;
	}

	result = (uint16)(lua_tonumber(L, -1));
	lua_pop(L, 1);

	return result;
}

uint8 Lua::getGlobalByte(const String& name) {
	uint8 result = 0;

	lua_getglobal(L, name.toCharArray());

	if (!lua_isnumber(L, -1)) {
		lua_pop(L, 1);

		StringBuffer msg;
		msg << "invalid value for: " << name << " expected NUMBER";
		info(msg);
		return 0;
	}

	result = (uint8)(lua_tonumber(L, -1));
	lua_pop(L, 1);

	return result;
}

float Lua::getGlobalFloat(const String& name) {
	float result = 0;

	lua_getglobal(L, name.toCharArray());

	if (!lua_isnumber(L, -1)) {
		lua_pop(L, 1);

		StringBuffer msg;
		msg << "invalid value for: " << name << " expected NUMBER";
		info(msg);
		return 0;
	}

	result = (float)(lua_tonumber(L, -1));
	lua_pop(L, 1);

	return result;
}

LuaObject Lua::getGlobalObject(const String& name) {
	lua_getglobal(L, name.toCharArray());

	LuaObject obj(L, name.toCharArray());

	return obj;
}

//params passed to funcs
String Lua::getStringParameter(lua_State* lState) {
	const char* result = NULL;
	if (!lua_isstring(lState, -1)) {
		lua_pop(lState, 1);
		/*
		StringBuffer msg;
		msg << "ERROR expected STRING";
		info(msg);
		System::out << "error reading STRING value while loading lua\n";
		*/
		return result;
	}
	result = lua_tostring(lState, -1);
	lua_pop(lState, 1);

	return result;
}

int32 Lua::getIntParameter(lua_State* lState) {
	int32 result = 0;
	if (!lua_isnumber(lState, -1)) {
		lua_pop(lState, 1);

		/*StringBuffer msg;
		msg << "ERROR expected NUMBER";
		info(msg);*/

		return 0;
	}

	result = (int32)lua_tonumber(lState, -1);
	lua_pop(lState, 1);

	return result;
}

uint32 Lua::getUnsignedIntParameter(lua_State* lState) {
	uint32 result = 0;
	if (!lua_isnumber(lState, -1)) {
		lua_pop(lState, 1);

		/*StringBuffer msg;
		msg << "ERROR expected NUMBER";
		info(msg);*/

		return 0;
	}

	result = (uint32)lua_tonumber(lState, -1);
	lua_pop(lState, 1);

	return result;
}

int64 Lua::getLongParameter(lua_State* lState) {
	int64 result = 0;
	if (!lua_isnumber(lState, -1)) {
		lua_pop(lState, 1);

		/*StringBuffer msg;
		msg << "ERROR expected NUMBER";
		info(msg);*/

		return 0;
	}

	result = (int64)lua_tonumber(lState, -1);
	lua_pop(lState, 1);

	return result;
}

uint64 Lua::getUnsignedLongParameter(lua_State* lState) {
	uint64 result = 0;
	if (!lua_isnumber(lState, -1)) {
		lua_pop(lState, 1);

		/*StringBuffer msg;
		msg << "ERROR expected NUMBER";
		info(msg);*/

		return 0;
	}

	result = (uint64)lua_tonumber(lState, -1);
	lua_pop(lState, 1);

	return result;
}

int16 Lua::getShortParameter(lua_State* lState) {
	int16 result = 0;
	if (!lua_isnumber(lState, -1)) {
		lua_pop(lState, 1);

		/*StringBuffer msg;
		msg << "ERROR expected NUMBER";
		info(msg);*/

		return 0;
	}

	result = (int16)lua_tonumber(lState, -1);
	lua_pop(lState, 1);

	return result;
}

uint16 Lua::getUnsignedShortParameter(lua_State* lState) {
	uint16 result = 0;
	if (!lua_isnumber(lState, -1)) {
		lua_pop(lState, 1);

		/*StringBuffer msg;
		msg << "ERROR expected NUMBER";
		info(msg);*/

		return 0;
	}

	result = (uint16)lua_tonumber(lState, -1);
	lua_pop(lState, 1);

	return result;
}

int8 Lua::getByteParameter(lua_State* lState) {
	int8 result = 0;
	if (!lua_isnumber(lState, -1)) {
		lua_pop(lState, 1);

		/*StringBuffer msg;
		msg << "ERROR expected NUMBER";
		info(msg);*/

		return 0;
	}

	result = (int8)lua_tonumber(lState, -1);
	lua_pop(lState, 1);

	return result;
}

uint8 Lua::getUnsignedByteParameter(lua_State* lState) {
	uint8 result = 0;
	if (!lua_isnumber(lState, -1)) {
		lua_pop(lState, 1);

		/*StringBuffer msg;
		msg << "ERROR expected NUMBER";
		info(msg);*/

		return 0;
	}

	result = (uint8)lua_tonumber(lState, -1);
	lua_pop(lState, 1);

	return result;
}

float Lua::getFloatParameter(lua_State* lState) {
	float result = 0;

	if (!lua_isnumber(lState, -1)) {
		lua_pop(lState, 1);

		/*StringBuffer msg;
		msg << "ERROR expected NUMBER";
		info(msg);*/

		return 0;
	}

	result = (float)lua_tonumber(lState, -1);
	lua_pop(lState, 1);

	return result;
}

// setters

void Lua::setGlobalString(const String& name, const String& value) {
	lua_pushstring(L, value.toCharArray());
	lua_setglobal(L, name.toCharArray());
}

void Lua::setGlobalInt(const String& name, const int value) {
	lua_pushnumber(L, value);
	lua_setglobal(L, name.toCharArray());
}

void Lua::setGlobalLong(const String& name, const long value) {
	lua_pushnumber(L, value);
	lua_setglobal(L, name.toCharArray());
}

void Lua::setGlobalShort(const String& name, const short value) {
	lua_pushnumber(L, value);
	lua_setglobal(L, name.toCharArray());
}

void Lua::setGlobalByte(const String& name, const char value) {
	lua_pushnumber(L, value);
	lua_setglobal(L, name.toCharArray());
}

void Lua::setGlobalFloat(const String& name, const float value) {
	lua_pushnumber(L, value);
	lua_setglobal(L, name.toCharArray());
}

bool Lua::checkStack(lua_State* lState, int num) {
	return (lua_gettop(lState) == num);
}

int Lua::checkStack(lua_State* lState) {
	return (lua_gettop(lState));
}

lua_State* Lua::getLuaState() {
	return L;
}

int Lua::checkStack() {
	return (lua_gettop(L));
}

bool Lua::checkStack(int num) {
	return (lua_gettop(L) == num);
}

//Field

void LuaObject::setField(const String& key, uint32 value) {
	lua_pushstring(L, key.toCharArray());
	lua_pushnumber(L, (double)value);
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

	return String(result);
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

bool LuaObject::isValidTable() {
	return lua_istable(L, -1);
}

void LuaObject::pop() {
	lua_pop(L, 1);
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
	lua_pushnumber(L, number);
}

void LuaFunction::operator<<(uint64 number) {
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
