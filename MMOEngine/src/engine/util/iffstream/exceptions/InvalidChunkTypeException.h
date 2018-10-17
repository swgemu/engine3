/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef INVALIDCHUNKTYPEEXCEPTION_H_
#define INVALIDCHUNKTYPEEXCEPTION_H_

#include "system/lang.h"

#include "IffStreamException.h"

class InvalidChunkTypeException : public IffStreamException {
public:
	InvalidChunkTypeException(IffStream* iffstream) : IffStreamException(iffstream) {
		StringBuffer sstr;
		sstr << "InvalidChunkTypeException on file:[" << iffstream->getFileName() << "]\n";
		message = sstr.toString();
	}
};

#endif /*INVALIDCHUNKTYPEEXCEPTION_H_*/
