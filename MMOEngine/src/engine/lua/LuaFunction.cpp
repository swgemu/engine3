/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "engine/core/Task.h"
#include "engine/core/TaskWorkerThread.h"

#include "LuaFunction.h"
#include "Lua.h"
#include "LuaPanicException.h"

#ifdef CXX11_COMPILER
#include <chrono>
#endif

LuaFunction::LuaFunction() {
	L = nullptr;
	numberOfArgs = 0;
	numberOfArgsToReturn = 0;
}

void LuaFunction::init(lua_State* l, const String& funcName, int argsToReturn) {
	L = l;
	numberOfArgs = 0;
	numberOfArgsToReturn = argsToReturn;

	lua_getglobal(L, functionName.toCharArray());
}

void LuaFunction::init(lua_State* l, const String& object, const String& func, int argsToReturn) {
	L = l;
	numberOfArgs = 1;
	numberOfArgsToReturn = argsToReturn;

	lua_getglobal(L, object.toCharArray());

	/* get func key */
	lua_pushstring(L, func.toCharArray());
	lua_gettable(L, -2);  /* funx resides at stack index -2 */
	lua_insert(L, -2);  /* and swap with func... */
}

LuaFunction::LuaFunction(lua_State* l, const String& funcName, int argsToReturn) : functionName(funcName) {
	init(l, funcName, argsToReturn);
}

LuaFunction::LuaFunction(lua_State* l, const String& object, const String& func, int argsToReturn) : functionName(func), object(object) {
	init(l, object, func, argsToReturn);
}

LuaFunction::LuaFunction(Lua* l, const String& funcName, int argsToReturn) : functionName(funcName) {
	init(l->getLuaState(), funcName, argsToReturn);
}

LuaFunction::LuaFunction(Lua* l, const String& object, const String& func, int argsToReturn) : functionName(func), object(object) {
	init(l->getLuaState(), object, func, argsToReturn);
}

LuaFunction::LuaFunction(const LuaFunction& func) : Object(), functionName(func.functionName), object(func.object) {
	L = func.L;
	numberOfArgs = func.numberOfArgs;
	numberOfArgsToReturn = func.numberOfArgsToReturn;
}

LuaFunction& LuaFunction::operator=(const LuaFunction& func) {
	if (this == &func)
		return *this;

	L = func.L;
	numberOfArgs = func.numberOfArgs;
	numberOfArgsToReturn = func.numberOfArgsToReturn;
	functionName = func.functionName;
	object = func.object;

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

void LuaFunction::operator<<(const String& str) {
	numberOfArgs++;
	lua_pushstring(L, str.toCharArray());
}

void LuaFunction::operator<<(const char* str) {
	numberOfArgs++;
	lua_pushstring(L, str);
}

void LuaFunction::operator<<(void* ptr) {
	numberOfArgs++;

	if (ptr != nullptr)
		lua_pushlightuserdata(L, ptr);
	else
		lua_pushnil(L);
}

lua_State* LuaFunction::callFunction() {
	try {
#ifdef COLLECT_TASKSTATISTICS
#ifdef CXX11_COMPILER
		auto start = std::chrono::high_resolution_clock::now();
#else
		Timer executionTimer;

		executionTimer.start();
#endif
#endif
		int result = lua_pcall(getLuaState(), getNumberOfArgs(), getNumberOfReturnArgs(), 0);

#ifdef COLLECT_TASKSTATISTICS
#ifdef CXX11_COMPILER
		auto end = std::chrono::high_resolution_clock::now();

		uint64 elapsedTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
#else
		uint64 elapsedTime = executionTimer.stop();
#endif

		Thread* thread = Thread::getCurrentThread();
		TaskWorkerThread* worker = thread ? thread->asTaskWorkerThread() : nullptr;

		if (worker) {
			if (object.length()) {
				String fullName = object + ":" + functionName;

#ifdef CXX11_COMPILER
				worker->addLuaTaskStats(std::move(fullName), elapsedTime);
#else
				worker->addLuaTaskStats(fullName, elapsedTime);
#endif
			} else {
				worker->addLuaTaskStats(functionName, elapsedTime);
			}
		}
#endif

		if (result != 0) {
			Logger::console.error("Error running function " + getFunctionName() + " " + String(lua_tostring(getLuaState(), -1)));
			return nullptr;
		}
	} catch (LuaPanicException& e) {
		Logger::console.error("LuaPanicException running function " + getFunctionName() + " " + String(lua_tostring(getLuaState(), -1)));

		return nullptr;
	}

	return getLuaState();
}

