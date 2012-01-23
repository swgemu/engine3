/*
 * LuaPanicException.h
 *
 *  Created on: 23/01/2012
 *      Author: victor
 */

#ifndef LUAPANICEXCEPTION_H_
#define LUAPANICEXCEPTION_H_

class LuaPanicException : public Exception {
public:
	LuaPanicException(const String& msg) : Exception(msg) {

	}
};


#endif /* LUAPANICEXCEPTION_H_ */
