/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef LUA_H
#define LUA_H

#include "../../system/lang.h"
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
			
		bool runFile(const string& filename);
		bool runString(const string& str);
		
		//static
		static void runFile(const string& filename, lua_State* lState);
		
		static lua_State* callFunction(LuaFunction* func);
		
		// getters
		
		// globals
		bool getGlobalBoolean(const string& name);
		string getGlobalString(const string& name);
		sys::uint32 getGlobalInt(const string& name);
		uint64 getGlobalLong(const string& name);
		sys::uint16 getGlobalShort(const string& name);
		sys::uint8 getGlobalByte(const string& name);
		float getGlobalFloat(const string& name);
		LuaObject getGlobalObject(const string& name);
		
		// static params
		static string getStringParameter(lua_State* lState);
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
		void setGlobalString(const string& name, const string& value);
		void setGlobalInt(const string& name, const int value);
		void setGlobalLong(const string& name, const long value);
		void setGlobalShort(const string& name, const short value);
		void setGlobalByte(const string& name, const char value);
		void setGlobalFloat(const string& name, const float value);
		
		//statics
		inline static bool checkStack(lua_State* lState, int num);
		inline static int checkStack(lua_State* lState);
		
		lua_State* getLuaState();
		inline int checkStack();
		inline bool checkStack(int num);
	
	};
	
	class LuaObject {
		lua_State* L;
		
		string objectName;
		
	public:
		LuaObject(lua_State* lState, const string& name) {
			L = lState;
			objectName = name;
		}
		
		LuaObject(lua_State* lState) {
			L = lState;
		}
		
		void operator=(lua_State* lState) {
			L = lState;
		}
		
		string getStringField(const string& key);
		sys::uint32 getIntField(const string& key);
		sys::uint16 getShortField(const string& key);
		sys::uint8 getByteField(const string& key);
		float getFloatField(const string& key);
		uint64 getLongField(const string& key);
		LuaObject getObjectField(const string& key);
		
		void setField(const string& key, sys::uint32 value);
		void setField(const string& key, const string& value);
		
		bool isValidTable();
		
		void pop();
		
	};
	
	class LuaFunction {
		lua_State* L;

		int numberOfArgs;
		int numberOfArgsToReturn;
		string functionName;

	public:
		LuaFunction(lua_State* l, const string& funcName, int argsToReturn);
		
		LuaFunction(lua_State* l, const string& object, const string& func, int argsToReturn);

		void operator<<(int number);
		void operator<<(sys::uint32 number);
		void operator<<(int64 number);
		void operator<<(uint64 number);
		void operator<<(bool boolean);
		void operator<<(string& str);
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
		
		inline string& getFunctionName() {
			return functionName;
		}

	};

  } // namespace lua
} // namespace engine

using namespace engine::lua;

#endif  // #ifndef LUA_H
