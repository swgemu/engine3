/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef NOCHUNKSEXCEPTION_H_
#define NOCHUNKSEXCEPTION_H_

#include "system/lang.h"

#include "IffStreamException.h"

class NoChunksException : public IffStreamException {
public:
	NoChunksException(IffStream* iffstream) : IffStreamException(iffstream) {
		StringBuffer sstr;
		sstr << "NoChunksException on file:[" << iffstream->getFileName() << "]\n";
		message = sstr.toString();
	}
};

#endif /*NOCHUNKSEXCEPTION_H_*/
