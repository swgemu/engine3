/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/**
 * @author      : theanswer (theanswer@Victors-MacBook-Pro.local)
 * @file        : JSONSerializationType
 * @created     : Saturday Nov 10, 2018 16:12:29 CET
 */

#ifndef JSONSERIALIZATIONTYPE_H

#define JSONSERIALIZATIONTYPE_H

#include "json.hpp"

namespace engine {

	namespace util {
		typedef nlohmann::json JSONSerializationType;
	}
}

using namespace engine::util;

#endif /* end of include guard JSONSERIALIZATIONTYPE_H */

