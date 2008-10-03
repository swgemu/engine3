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

bool Lua::runFile(const string& filename) {
	if (filename.size() == 0)
		return false;
	
	/*stringstream msg;
	msg << "Loading lua file: " << filename;
	info(msg);*/

	if (!L) 
		init();

	if (luaL_loadfile(L, filename.c_str()) || lua_pcall(L, 0, 0, 0)) {
		const char* err = lua_tostring(L, -1);
		lua_pop(L,1);

		stringstream msg;
		msg << "ERROR " << err;
		info(msg); 

		return false;
	}

	return true;
}

void Lua::runFile(const string& filename, lua_State* lState) {
	if (filename.size() != 0) {
		//cout << "Loading lua file: " << filename << "\n";
	
		if (luaL_loadfile(lState, filename.c_str()) || lua_pcall(lState, 0, 0, 0)) {
			const char* err = lua_tostring(lState, -1);
			lua_pop(lState,1);
			
			cout << "ERROR " << err << "\n";
		}
	}
}

bool Lua::runString(const string& str) {
	if (str.size() == 0)
		return false;
	
	stringstream msg;
	msg << "Loading lua string: " << str << "\n";
	info(msg); 

	if (!L) 
		init();

	if (luaL_loadbuffer(L, str.c_str(), str.length(), "command") || lua_pcall(L, 0, 0, 0)) {
		const char* err = lua_tostring(L, -1);
		lua_pop(L,1);

		stringstream msg;
		msg << "ERROR " << err;
		info(msg); 

		return false;
	}
	
	return true;
}

lua_State* Lua::callFunction(LuaFunction* func) {
	if (lua_pcall(func->getLuaState(), func->getNumberOfArgs(), func->getNumberOfReturnArgs(), 0) != 0) {
		cout << "Error running function " << func->getFunctionName() << " " << lua_tostring(func->getLuaState(), -1);
		return NULL;
	} 
	
	return func->getLuaState();
}

// getters
bool Lua::getGlobalBoolean(const string& name) {
	string result = getGlobalString(name);

	if (result == "true")
		return true;
	else
		return false;
}

string Lua::getGlobalString(const string& name) {
	const char* result = NULL;
	
	lua_getglobal(L, name.c_str());
	
	if (!lua_isstring(L, -1)) {
		lua_pop(L, 1);
		
		stringstream msg;
		msg << "invalid string value for: " << name;
		info(msg); 

		return string("");
	}
	
	result = lua_tostring(L, -1);
	lua_pop(L, 1);
	
	return result;
}

uint32 Lua::getGlobalInt(const string& name) {
	uint32 result = 0;
	
	lua_getglobal(L, name.c_str());
	
	if (!lua_isnumber(L, -1)) {
		lua_pop(L, 1);
		
		stringstream msg;
		msg << "invalid value for: " << name << " expected NUMBER";
		info(msg);
		 
		return 0;
	}
	
	result = (uint32)(lua_tonumber(L, -1));
	lua_pop(L, 1);
	
	return result;
}

uint64 Lua::getGlobalLong(const string& name) {
	uint64 result = 0;
	
	lua_getglobal(L, name.c_str());
	
	if (!lua_isnumber(L, -1)) {
		lua_pop(L, 1);
		
		stringstream msg;
		msg << "invalid value for: " << name << " expected NUMBER";
		info(msg);
		
		return 0;
	}
	
	result = (uint64) (lua_tonumber(L, -1));
	lua_pop(L, 1);
	
	return result;
}

uint16 Lua::getGlobalShort(const string& name) {
	uint16 result = 0;
	
	lua_getglobal(L, name.c_str());
	
	if (!lua_isnumber(L, -1)) {
		lua_pop(L, 1);
		
		stringstream msg;
		msg << "invalid value for: " << name << " expected NUMBER";
		info(msg);
		
		return 0;
	}
	
	result = (uint16)(lua_tonumber(L, -1));
	lua_pop(L, 1);
	
	return result;
}

uint8 Lua::getGlobalByte(const string& name) {
	uint8 result = 0;
	
	lua_getglobal(L, name.c_str());
	
	if(!lua_isnumber(L, -1)) {
		lua_pop(L, 1);
		
		stringstream msg;
		msg << "invalid value for: " << name << " expected NUMBER";
		info(msg);
		return 0;
	}
	
	result = (uint8)(lua_tonumber(L, -1));
	lua_pop(L, 1);
	
	return result;
}

float Lua::getGlobalFloat(const string& name) {
	float result = 0;
	
	lua_getglobal(L, name.c_str());
	
	if(!lua_isnumber(L, -1)) {
		lua_pop(L, 1);
		
		stringstream msg;
		msg << "invalid value for: " << name << " expected NUMBER";
		info(msg);
		return 0;
	}
	
	result = (float)(lua_tonumber(L, -1));
	lua_pop(L, 1);
	
	return result;
}

LuaObject Lua::getGlobalObject(const string& name) {
	lua_getglobal(L, name.c_str());

	LuaObject obj(L, name.c_str());
	
	return obj;
}

//params passed to funcs
string Lua::getStringParameter(lua_State* lState) {
	const char* result = NULL;
	if (!lua_isstring(lState, -1)) {
		lua_pop(lState, 1);
		/*
		stringstream msg;
		msg << "ERROR expected STRING";
		info(msg); 
		cout << "error reading STRING value while loading lua\n";
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
		
		/*stringstream msg;
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
		
		/*stringstream msg;
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
		
		/*stringstream msg;
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
		
		/*stringstream msg;
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
	if(!lua_isnumber(lState, -1)) {
		lua_pop(lState, 1);
		
		/*stringstream msg;
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
	if(!lua_isnumber(lState, -1)) {
		lua_pop(lState, 1);
		
		/*stringstream msg;
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
	if(!lua_isnumber(lState, -1)) {
		lua_pop(lState, 1);
		
		/*stringstream msg;
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
	if(!lua_isnumber(lState, -1)) {
		lua_pop(lState, 1);
		
		/*stringstream msg;
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
	
	if(!lua_isnumber(lState, -1)) {
		lua_pop(lState, 1);
		
		/*stringstream msg;
		msg << "ERROR expected NUMBER";
		info(msg);*/
		
		return 0;
	}
	
	result = (float)lua_tonumber(lState, -1);
	lua_pop(lState, 1);
	
	return result;
}

// setters

void Lua::setGlobalString(const string& name, const string& value) {
	lua_pushstring(L, value.c_str());
	lua_setglobal(L, name.c_str());
}

void Lua::setGlobalInt(const string& name, const int value) {
	lua_pushnumber(L, value);
	lua_setglobal(L, name.c_str());
}

void Lua::setGlobalLong(const string& name, const long value) {
	lua_pushnumber(L, value);
	lua_setglobal(L, name.c_str());
}

void Lua::setGlobalShort(const string& name, const short value) {
	lua_pushnumber(L, value);
	lua_setglobal(L, name.c_str());
}

void Lua::setGlobalByte(const string& name, const char value) {
	lua_pushnumber(L, value);
	lua_setglobal(L, name.c_str());
}

void Lua::setGlobalFloat(const string& name, const float value) {
	lua_pushnumber(L, value);
	lua_setglobal(L, name.c_str());
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

void LuaObject::setField(const string& key, uint32 value) {
	lua_pushstring(L, key.c_str());
	lua_pushnumber(L, (double)value);
	lua_settable(L, -3);
}

void LuaObject::setField(const string& key, const string& value) {
	lua_pushstring(L, key.c_str());
	lua_pushstring(L, value.c_str());
	lua_settable(L, -3);
}

string LuaObject::getStringField(const string& key) {
	const char* result = NULL;
	
	lua_pushstring(L, key.c_str());
	lua_gettable(L, -2);
	
	result = lua_tostring(L, -1);
	lua_pop(L, 1);
	
	return result;
}

uint32 LuaObject::getIntField(const string& key) {
	uint32 result = 0;
	
	lua_pushstring(L, key.c_str());
	lua_gettable(L, -2);
	
	result = (uint32)lua_tonumber(L, -1);
	lua_pop(L, 1);
	
	return result;
}

uint16 LuaObject::getShortField(const string& key) {
	uint16 result = 0;
	
	lua_pushstring(L, key.c_str());
	lua_gettable(L, -2);
	
	result = (uint16)lua_tonumber(L, -1);
	lua_pop(L, 1);
	
	return result;
}

uint8 LuaObject::getByteField(const string& key) {
	uint8 result = 0;
	
	lua_pushstring(L, key.c_str());
	lua_gettable(L, -2);
	
	result = (uint8)lua_tonumber(L, -1);
	lua_pop(L, 1);
	
	return result;
}

float LuaObject::getFloatField(const string& key) {
	float result = 0;
	
	lua_pushstring(L, key.c_str());
	lua_gettable(L, -2);
	
	result = (float)lua_tonumber(L, -1);
	lua_pop(L, 1);
	
	return result;
}

uint64 LuaObject::getLongField(const string& key) {
	uint64 result = 0;
	
	lua_pushstring(L, key.c_str());
	lua_gettable(L, -2);
	
	result = (uint64)lua_tonumber(L, -1);
	lua_pop(L, 1);
	
	return result;
}

LuaObject LuaObject::getObjectField(const string& key) {
	lua_pushstring(L, key.c_str());
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

LuaFunction::LuaFunction(lua_State* l, const string& funcName, int argsToReturn) {
	L = l;
	functionName = funcName;
	numberOfArgs = 0;
	numberOfArgsToReturn = argsToReturn;

	lua_getglobal(L, functionName.c_str());
}

LuaFunction::LuaFunction(lua_State* l, const string& object, const string& func, int argsToReturn) {
	L = l;
	functionName = func;
	numberOfArgs = 1;
	numberOfArgsToReturn = argsToReturn;

	lua_getglobal(L, object.c_str());

	/* get func key */
	lua_pushstring(L, func.c_str());
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

void LuaFunction::operator<<(string& str) {
	numberOfArgs++;
	lua_pushstring(L, str.c_str());
}

void LuaFunction::operator<<(const char* str) {
	numberOfArgs++;
	lua_pushstring(L, str);
}
