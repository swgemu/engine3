/**
 * @author      : theanswer (theanswer@hyperv)
 * @file        : Function
 * @created     : Tuesday Sep 17, 2019 14:26:44 UTC
 */

#pragma once

#include <type_traits>
#include <functional>

namespace sys {
	namespace lang {
		template<typename F>
		using Function = std::function<F>;
	}
}

