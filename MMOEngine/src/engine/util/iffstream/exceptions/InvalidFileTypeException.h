/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef INVALIDFILETYPEEXCEPTION_H_
#define INVALIDFILETYPEEXCEPTION_H_

#include "system/lang.h"

#include "IffStreamException.h"

class InvalidFileTypeException : public IffStreamException {
public:
	InvalidFileTypeException(IffStream* iffstream) : IffStreamException(iffstream) {
		StringBuffer sstr;
		sstr << "InvalidFileTypeException on file:[" << iffstream->getFileName() << "]\n";
		message = sstr.toString();
	}
};

#endif /*INVALIDFILETYPEEXCEPTION_H_*/
