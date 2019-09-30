/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#pragma once

#include "system/lang.h"

#include <lua.hpp>

#include "LuaObject.h"
#include "LuaFunction.h"
#include "Luna.h"

#include "LuaCallbackException.h"

#include "engine/log/Logger.h"

namespace engine {
  namespace lua {

	class Lua : public Logger, public Object {
	protected:
		lua_State* L;
		bool deinitOnDestruction;

	public:
		Lua();
		Lua(lua_State* L);
		Lua(Lua&& lua);

		virtual ~Lua();

		virtual void init();

		void deinit();

		virtual bool runFile(const String& filename);
		virtual bool runString(const String& str);

		//static
		static bool runFile(const String& filename, lua_State* lState);
		static int atPanic(lua_State* L);

		virtual void registerFunction(const char* name, int(*functionPointer)(lua_State*));

		// getters
		virtual String getString(int index = -1, bool pop = true) {
			return getStringParameter(L, index, pop);
		}

		virtual int32 getInt(int index = -1, bool pop = true) {
			return getIntParameter(L, index, pop);
		}

		virtual sys::uint32 getUnsignedInt(int index = -1, bool pop = true) {
			return getUnsignedIntParameter(L, index, pop);
		}

		virtual int64 getLong(int index = -1, bool pop = true) {
			return getLongParameter(L, index, pop);
		}

		virtual uint64 getUnsignedLong(int index = -1, bool pop = true) {
			return getUnsignedLongParameter(L, index, pop);
		}

		virtual int16 getShort(int index = -1, bool pop = true) {
			return getShortParameter(L, index, pop);
		}

		virtual sys::uint16 getUnsignedShort(int index = -1, bool pop = true) {
			return getUnsignedShortParameter(L, index, pop);
		}

		virtual int8 getByte(int index = -1, bool pop = true) {
			return getByteParameter(L, index, pop);
		}

		virtual sys::uint8 getUnsignedByte(int index = -1, bool pop = true) {
			return getUnsignedByteParameter(L, index, pop);
		}

		virtual float getFloat(int index = -1, bool pop = true) {
			return getFloatParameter(L, index, pop);
		}

		virtual bool getBoolean(int index = -1, bool pop = true) {
			return getBooleanParameter(L, index, pop);
		}

		virtual void pop(int n);

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
		static String getStringParameter(lua_State* lState, int index = -1, bool pop = true);
		static int32 getIntParameter(lua_State* lState, int index = -1, bool pop = true);
		static sys::uint32 getUnsignedIntParameter(lua_State* lState, int index = -1, bool pop = true);
		static int64 getLongParameter(lua_State* lState, int index = -1, bool pop = true);
		static uint64 getUnsignedLongParameter(lua_State* lState, int index = -1, bool pop = true);
		static int16 getShortParameter(lua_State* lState, int index = -1, bool pop = true);
		static sys::uint16 getUnsignedShortParameter(lua_State* lState, int index = -1, bool pop = true);
		static int8 getByteParameter(lua_State* lState, int index = -1, bool pop = true);
		static sys::uint8 getUnsignedByteParameter(lua_State* lState, int index = -1, bool pop = true);
		static float getFloatParameter(lua_State* lState, int index = -1, bool pop = true);
		static bool getBooleanParameter(lua_State* lState, int index = -1, bool pop = true);

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

