/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef FORM_H_
#define FORM_H_

#include "system/lang.h"

#include "engine/util/iffstream/chunks/Chunk.h"

namespace engine {
 namespace util {

	class Form : public Chunk {
	protected:
		uint32 formType;

	public:
		Form(Chunk* parent, uint32 type, uint32 size, char* data);
		~Form();

		void parseData();
		void parseSubObjects();

		inline uint32 getFormType() {
			return formType;
		}

	};

 }
}

using namespace engine::util;

#endif /*FORM_H_*/
