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
		bool deinitOnDestruction;

	public:
		Lua();
		Lua(lua_State* L);

		virtual ~Lua();

		virtual void init();

		void deinit();

		virtual bool runFile(const String& filename);
		virtual bool runString(const String& str);

		//static
		static bool runFile(const String& filename, lua_State* lState);

		static int atPanic(lua_State* L);

		// getters

		// globals
		virtual bool getGlobalBoolean(const String& name);
		virtual String getGlobalString(const String& name);
		virtual sys::uint32 getGlobalInt(const String& name);
		virtual uint64 getGlobalLong(const String& name);
		virtual sys::uint16 getGlobalShort(const String& name);
		virtual sys::uint8 getGlobalByte(const String& name);
		virtual float getGlobalFloat(const String& name);
		virtual LuaObject getGlobalObject(const String& name);

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
		virtual void setGlobalString(const String& name, const String& value);
		virtual void setGlobalInt(const String& name, const int value);
		virtual void setGlobalLong(const String& name, const uint64 value);
		virtual void setGlobalShort(const String& name, const short value);
		virtual void setGlobalByte(const String& name, const char value);
		virtual void setGlobalFloat(const String& name, const float value);
		virtual void setGlobalBoolean(const String& name, const bool value);

		//statics
		static bool checkStack(lua_State* lState, int num);
		static int checkStack(lua_State* lState);

		virtual lua_State* getLuaState();
		virtual int checkStack();
		virtual bool checkStack(int num);

		virtual LuaFunction* createFunction(const String& funcname, int argsThatWillReturn);
		virtual LuaFunction* createFunction(const String& object, const String& func, int argsThatWillReturn);

		void setDeinitOnDestruction(bool val) {
			deinitOnDestruction = val;
		}

	};

  } // namespace lua
} // namespace engine

using namespace engine::lua;

#endif  // #ifndef LUA_H
