/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/**
 * @author      : theanswer (theanswer@Victors-MacBook-Pro.local)
 * @file        : Optional
 * @created     : Monday Apr 01, 2019 19:39:52 CEST
 */

#ifndef OPTIONAL_H

#define OPTIONAL_H

#include "optional.hpp"

namespace sys {
namespace util {
	template<typename T>
	using Optional = std::experimental::optional<T>;
 }
}

using namespace sys::util;

#endif /* end of include guard OPTIONAL_H */

