/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
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
