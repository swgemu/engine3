/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef LUA_H
#define LUA_H

#include "system/lang.h"

#include "../engine.h"

#include "lua.hpp"

namespace engine {
  namespace lua {

  	class LuaFunction;
  	class LuaObject;

	class Lua : public Logger {
	protected:
		lua_State* L;

	public:
		Lua();

		virtual ~Lua();

		void init();

		void deinit();

		bool runFile(const String& filename);
		bool runString(const String& str);

		//static
		static void runFile(const String& filename, lua_State* lState);

		static lua_State* callFunction(LuaFunction* func);

		// getters

		// globals
		bool getGlobalBoolean(const String& name);
		String getGlobalString(const String& name);
		sys::uint32 getGlobalInt(const String& name);
		uint64 getGlobalLong(const String& name);
		sys::uint16 getGlobalShort(const String& name);
		sys::uint8 getGlobalByte(const String& name);
		float getGlobalFloat(const String& name);
		LuaObject getGlobalObject(const String& name);

		// static params
		static String getStringParameter(lua_State* lState);
		static int32 getIntParameter(lua_State* lState);
		static sys::uint32 getUnsignedIntParameter(lua_State* lState);
		static int64 getLongParameter(lua_State* lState);
		static uint64 getUnsignedLongParameter(lua_State* lState);
		static int16 getShortParameter(lua_State* lState);
		static sys::uint16 getUnsignedShortParameter(lua_State* lState);
		static int8 getByteParameter(lua_State* lState);
		static sys::uint8 getUnsignedByteParameter(lua_State* lState);
		static float getFloatParameter(lua_State* lState);

		// setters
		void setGlobalString(const String& name, const String& value);
		void setGlobalInt(const String& name, const int value);
		void setGlobalLong(const String& name, const long value);
		void setGlobalShort(const String& name, const short value);
		void setGlobalByte(const String& name, const char value);
		void setGlobalFloat(const String& name, const float value);

		//statics
		inline static bool checkStack(lua_State* lState, int num);
		inline static int checkStack(lua_State* lState);

		lua_State* getLuaState();
		inline int checkStack();
		inline bool checkStack(int num);

	};

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

  } // namespace lua
} // namespace engine

using namespace engine::lua;

#endif  // #ifndef LUA_H
