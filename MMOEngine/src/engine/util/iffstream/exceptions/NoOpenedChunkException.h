/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
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
