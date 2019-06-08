/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef NOOPENEDCHUNKEXCEPTION_H_
#define NOOPENEDCHUNKEXCEPTION_H_

#include "system/lang.h"

#include "IffStreamException.h"

class NoOpenedChunkException : public IffStreamException {

public:
	NoOpenedChunkException(IffStream* iffstream) : IffStreamException(iffstream) {
		StringBuffer sstr;
		sstr << "NoOpenedChunkException on file:[" << iffstream->getFileName() << "]\n";
		message = sstr.toString();
	}
};

#endif /*NOOPENEDCHUNKEXCEPTION_H_*/
