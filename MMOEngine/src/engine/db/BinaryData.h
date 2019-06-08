/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef BINARYDATA_H_
#define BINARYDATA_H_

#include "system/lang.h"

namespace engine {
  namespace db {

	static const char *base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	
	class BinaryData {
		const char* iData;
		int iDataSize;
	
	public:
		BinaryData(String& data);
	
		void encode(String& stream);
		
		bool decode(String& stream);		
	};

  } // namespace db
} // namespace engine

using namespace engine::db;

#endif /*BINARYDATA_H_*/
