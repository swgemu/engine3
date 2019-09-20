/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "engine/log/Logger.h"

#include "Lua.h"

extern "C" {
}

#include "LuaPanicException.h"

Lua::Lua() : Logger("Lua") {
	L = nullptr;

	deinitOnDestruction = true;
}

Lua::Lua(Lua&& lua) : Logger(std::move(lua)), L(lua.L), deinitOnDestruction(lua.deinitOnDestruction) {
	lua.L = nullptr;
}

Lua::Lua(lua_State* L) : Logger("Lua") {
	this->L = L;

	deinitOnDestruction = false;
}

Lua::~Lua() {
	if (deinitOnDestruction)
		deinit();
}

void Lua::init() {
	L = luaL_newstate();
	luaL_openlibs(L);

	lua_atpanic(L, Lua::atPanic);
}

void Lua::deinit() {
	if (L)
		lua_close(L);

	L = nullptr;
}

void Lua::registerFunction(const char* name, int(*functionPointer)(lua_State*)) {
	lua_register(L, name, functionPointer);
}

bool Lua::runFile(const String& filename) {
	if (filename.isEmpty())
		return false;

	/*StringBuffer msg;
	msg << "Loading lua file: " << filename;
	info(msg);*/

	if (!L)
		init();

	try {

		if (luaL_loadfile(L, filename.toCharArray()) || lua_pcall(L, 0, 0, 0)) {
			const char* err = lua_tostring(L, -1);

			info() << "file:" << filename << " ERROR " << err;

			lua_pop(L, 1);

			return false;
		}
	} catch (LuaPanicException& e) {
		error() << "LuaPanicException while running " << filename;
		error(e.getMessage());

		return false;
	}

	return true;
}

bool Lua::runFile(const String& filename, lua_State* lState) {
	try {
		if (!filename.isEmpty()) {
			//System::out << "Loading lua file: " << filename << "\n";

			if (luaL_loadfile(lState, filename.toCharArray()) || lua_pcall(lState, 0, 0, 0)) {
				const char* err = lua_tostring(lState, -1);

				Logger::console.error() << "file:" << filename << " ERROR " << err;

				lua_pop(lState, 1);

				return false;
			}
		}
	} catch (const LuaPanicException& e) {
		Logger::console.error("LuaPanicException while running " + filename);
		Logger::console.error(e.getMessage());

		return false;
	}

	return true;
}

bool Lua::runString(const String& str) {
	if (str.isEmpty())
		return false;

	info() << "Loading lua String: " << str << endl;

	if (!L)
		init();

	try {
		if (luaL_loadbuffer(L, str.toCharArray(), str.length(), "command") || lua_pcall(L, 0, 0, 0)) {
			const char* err = lua_tostring(L, -1);

			info() << "ERROR " << err;

			lua_pop(L, 1);

			return false;
		}
	} catch (const LuaPanicException& e) {
		error("LuaPanicException while " + str);

		return false;
	}

	return true;
}

int Lua::atPanic(lua_State* L) {
	const char* msg = lua_tostring(L, -1);

	String errorMessage;

	if (msg != nullptr) {
		errorMessage = msg;
	}

	throw LuaPanicException("Lua panic: " + errorMessage);
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
	const char* result = nullptr;

	lua_getglobal(L, name.toCharArray());

	if (!lua_isstring(L, -1)) {
		lua_pop(L, 1);

		info() << "invalid String value for: " << name;

		return String("");
	}

	result = lua_tostring(L, -1);
	auto size = lua_rawlen(L, -1);

	String val(result, size);

	lua_pop(L, 1);

	return val;
}

uint32 Lua::getGlobalInt(const String& name) {
	uint32 result = 0;

	lua_getglobal(L, name.toCharArray());

	if (!lua_isnumber(L, -1)) {
		lua_pop(L, 1);

		info() << "invalid value for: " << name << " expected NUMBER";

		return 0;
	}

	result = (uint32)(lua_tointeger(L, -1));
	lua_pop(L, 1);

	return result;
}

uint64 Lua::getGlobalLong(const String& name) {
	uint64 result = 0;

	lua_getglobal(L, name.toCharArray());

	if (!lua_isnumber(L, -1)) {
		lua_pop(L, 1);

		info() << "invalid value for: " << name << " expected NUMBER";

		return 0;
	}

	result = (uint64) (lua_tointeger(L, -1));
	lua_pop(L, 1);

	return result;
}

uint16 Lua::getGlobalShort(const String& name) {
	uint16 result = 0;

	lua_getglobal(L, name.toCharArray());

	if (!lua_isnumber(L, -1)) {
		lua_pop(L, 1);

		info() << "invalid value for: " << name << " expected NUMBER";

		return 0;
	}

	result = (uint16)(lua_tointeger(L, -1));
	lua_pop(L, 1);

	return result;
}

uint8 Lua::getGlobalByte(const String& name) {
	uint8 result = 0;

	lua_getglobal(L, name.toCharArray());

	if (!lua_isnumber(L, -1)) {
		lua_pop(L, 1);

		info() << "invalid value for: " << name << " expected NUMBER";

		return 0;
	}

	result = (uint8)(lua_tointeger(L, -1));
	lua_pop(L, 1);

	return result;
}

float Lua::getGlobalFloat(const String& name) {
	float result = 0;

	lua_getglobal(L, name.toCharArray());

	if (!lua_isnumber(L, -1)) {
		lua_pop(L, 1);

		info() << "invalid value for: " << name << " expected NUMBER";

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
String Lua::getStringParameter(lua_State* lState, int index, bool pop) {
	const char* result = nullptr;
	if (!lua_isstring(lState, index)) {
		if (pop) {
			lua_pop(lState, 1);
		}
		/*
		StringBuffer msg;
		msg << "ERROR expected STRING";
		info(msg);
		System::out << "error reading STRING value while loading lua\n";
		 */
		return result;
	}

	result = lua_tostring(lState, index);
	auto size = lua_rawlen(lState, index);

	String val(result, size);

	if (pop) {
		lua_pop(lState, 1);
	}

	return val;
}

int32 Lua::getIntParameter(lua_State* lState, int index, bool pop) {
	int32 result = 0;
	if (!lua_isnumber(lState, index)) {
		if (pop) {
			lua_pop(lState, 1);
		}

		/*StringBuffer msg;
		msg << "ERROR expected NUMBER";
		info(msg);*/

		return 0;
	}

	result = (int32)lua_tointeger(lState, index);

	if (pop) {
		lua_pop(lState, 1);
	}

	return result;
}

uint32 Lua::getUnsignedIntParameter(lua_State* lState, int index, bool pop) {
	uint32 result = 0;
	if (!lua_isnumber(lState, index)) {
		if (pop) {
			lua_pop(lState, 1);
		}

		/*StringBuffer msg;
		msg << "ERROR expected NUMBER";
		info(msg);*/

		return 0;
	}

	result = (uint32)lua_tointeger(lState, index);

	if (pop) {
		lua_pop(lState, 1);
	}

	return result;
}

int64 Lua::getLongParameter(lua_State* lState, int index, bool pop) {
	int64 result = 0;
	if (!lua_isnumber(lState, index)) {
		if (pop) {
			lua_pop(lState, 1);
		}

		/*StringBuffer msg;
		msg << "ERROR expected NUMBER";
		info(msg);*/

		return 0;
	}

	result = (int64)lua_tointeger(lState, index);

	if (pop) {
		lua_pop(lState, 1);
	}

	return result;
}

uint64 Lua::getUnsignedLongParameter(lua_State* lState, int index, bool pop) {
	uint64 result = 0;
	if (!lua_isnumber(lState, index)) {
		if (pop) {
			lua_pop(lState, 1);
		}

		/*StringBuffer msg;
		msg << "ERROR expected NUMBER";
		info(msg);*/

		return 0;
	}

	result = (uint64)lua_tointeger(lState, index);

	if (pop) {
		lua_pop(lState, 1);
	}

	return result;
}

int16 Lua::getShortParameter(lua_State* lState, int index, bool pop) {
	int16 result = 0;
	if (!lua_isnumber(lState, index)) {
		if (pop) {
			lua_pop(lState, 1);
		}

		/*StringBuffer msg;
		msg << "ERROR expected NUMBER";
		info(msg);*/

		return 0;
	}

	result = (int16)lua_tointeger(lState, index);

	if (pop) {
		lua_pop(lState, 1);
	}

	return result;
}

uint16 Lua::getUnsignedShortParameter(lua_State* lState, int index, bool pop) {
	uint16 result = 0;
	if (!lua_isnumber(lState, index)) {
		if (pop) {
			lua_pop(lState, 1);
		}

		/*StringBuffer msg;
		msg << "ERROR expected NUMBER";
		info(msg);*/

		return 0;
	}

	result = (uint16)lua_tointeger(lState, index);

	if (pop) {
		lua_pop(lState, 1);
	}

	return result;
}

int8 Lua::getByteParameter(lua_State* lState, int index, bool pop) {
	int8 result = 0;
	if (!lua_isnumber(lState, index)) {
		if (pop) {
			lua_pop(lState, 1);
		}

		/*StringBuffer msg;
		msg << "ERROR expected NUMBER";
		info(msg);*/

		return 0;
	}

	result = (int8)lua_tointeger(lState, index);

	if (pop) {
		lua_pop(lState, 1);
	}

	return result;
}

uint8 Lua::getUnsignedByteParameter(lua_State* lState, int index, bool pop) {
	uint8 result = 0;
	if (!lua_isnumber(lState, index)) {
		if (pop) {
			lua_pop(lState, 1);
		}

		/*StringBuffer msg;
		msg << "ERROR expected NUMBER";
		info(msg);*/

		return 0;
	}

	result = (uint8)lua_tointeger(lState, index);

	if (pop) {
		lua_pop(lState, 1);
	}

	return result;
}

bool Lua::getBooleanParameter(lua_State* lState, int index, bool pop) {
	bool result = false;

	if (!lua_isboolean(lState, index)) {
		if (pop) {
			lua_pop(lState, 1);
		}

		return false;
	}

	result = lua_toboolean(lState, index);

	if (pop) {
		lua_pop(lState, 1);
	}

	return result;
}

float Lua::getFloatParameter(lua_State* lState, int index, bool pop) {
	float result = 0;

	if (!lua_isnumber(lState, index)) {
		if (pop) {
			lua_pop(lState, 1);
		}

		/*StringBuffer msg;
		msg << "ERROR expected NUMBER";
		info(msg);*/

		return 0;
	}

	result = (float)lua_tonumber(lState, index);

	if (pop) {
		lua_pop(lState, 1);
	}

	return result;
}

// setters

void Lua::setGlobalString(const String& name, const String& value) {
	lua_pushstring(L, value.toCharArray());
	lua_setglobal(L, name.toCharArray());
}

void Lua::setGlobalInt(const String& name, const int value) {
	lua_pushinteger(L, value);
	lua_setglobal(L, name.toCharArray());
}

void Lua::setGlobalLong(const String& name, const uint64 value) {
	lua_pushinteger(L, value);
	lua_setglobal(L, name.toCharArray());
}

void Lua::setGlobalShort(const String& name, const short value) {
	lua_pushinteger(L, value);
	lua_setglobal(L, name.toCharArray());
}

void Lua::setGlobalByte(const String& name, const char value) {
	lua_pushinteger(L, value);
	lua_setglobal(L, name.toCharArray());
}

void Lua::setGlobalFloat(const String& name, const float value) {
	lua_pushnumber(L, value);
	lua_setglobal(L, name.toCharArray());
}

void Lua::setGlobalBoolean(const String& name, const bool value) {
	lua_pushboolean(L, value);
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

void Lua::pop(int n) {
	lua_pop(L, n);
}

LuaFunction* Lua::createFunction(const String& funcname, int argsThatWillReturn) {
	LuaFunction* function = new LuaFunction(L, funcname, argsThatWillReturn);

	return function;
}

LuaFunction* Lua::createFunction(const String& object, const String& func, int argsThatWillReturn) {
	LuaFunction* function = new LuaFunction(L, object, func, argsThatWillReturn);

	return function;
}

//Field
