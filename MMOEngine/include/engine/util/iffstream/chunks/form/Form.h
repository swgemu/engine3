/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef FORM_H_
#define FORM_H_

#include "system/lang.h"

#include "../Chunk.h"

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
