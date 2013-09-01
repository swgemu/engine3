/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef LUA_H
#define LUA_H

#include "system/lang.h"

#include "lua.hpp"

#include "LuaObject.h"
#include "LuaFunction.h"
#include "Luna.h"

#include "LuaCallbackException.h"

#include "engine/log/Logger.h"

namespace engine {
  namespace lua {

  	class LuaFunction;
  	class LuaObject;

	class Lua : public Logger, public Object {
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
		static bool runFile(const String& filename, lua_State* lState);

		static int atPanic(lua_State* L);

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
		static bool getBooleanParameter(lua_State* lState);

		// setters
		void setGlobalString(const String& name, const String& value);
		void setGlobalInt(const String& name, const int value);
		void setGlobalLong(const String& name, const uint64 value);
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

  } // namespace lua
} // namespace engine

using namespace engine::lua;

#endif  // #ifndef LUA_H
