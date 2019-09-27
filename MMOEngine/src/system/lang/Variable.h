/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#pragma once

#include "system/platform.h"

namespace sys {
	namespace io {
		class ObjectOutputStream;
		class ObjectInputStream;
	}
}

namespace sys {
	namespace lang {

	class String;

	using namespace sys::io;

	class Variable {
	public:
		inline Variable() {
		}

		virtual ~Variable() {
		}

		virtual bool toBinaryStream(ObjectOutputStream* stream) = 0;

		virtual bool parseFromBinaryStream(ObjectInputStream* stream) = 0;

	};

	} // namespace lang
} // namespace sys

using namespace sys::lang;

