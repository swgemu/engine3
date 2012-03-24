/*
 * LuaPanicException.h
 *
 *  Created on: 23/01/2012
 *      Author: victor
 */

#ifndef LUACALLBACKEXCEPTION_H_
#define LUACALLBACKEXCEPTION_H_

class LuaCallbackException : public Exception {
public:
	LuaCallbackException(lua_State *L, const String& msg) : Exception() {
		luaL_where (L, 1);
		String luaMethodName = lua_tostring(L, -1);

		message = msg + " at " + luaMethodName;

	}
};


#endif /* LUACALLBACKEXCEPTION_H_ */
