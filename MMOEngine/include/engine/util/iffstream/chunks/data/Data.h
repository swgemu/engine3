/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef DATA_H_
#define DATA_H_

#include "../Chunk.h"

namespace engine {
 namespace util {

	class Data : public Chunk {
	protected:

	public:
		Data(Chunk* parent, uint32 id, uint32 size,  char* buffer) : Chunk(parent, id, size, buffer) {
		}

	};

 }
}

using namespace engine::util;

#endif /*DATA_H_*/
