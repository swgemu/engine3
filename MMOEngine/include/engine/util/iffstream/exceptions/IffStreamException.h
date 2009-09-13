/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef IFFSTREAMEXCEPTION_H_
#define IFFSTREAMEXCEPTION_H_

#include "system/lang.h"

#include "../IffStream.h"

class IffStreamException : public Exception {
protected:
	IffStream* iffStream;

public:
	IffStreamException(IffStream* stream) : Exception("IffStreamException\n") {
		iffStream = stream;
	}

	IffStream* getIffStream() {
		return iffStream;
	}

};

#endif /*IFFSTREAMEXCEPTION_H_*/
