/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#pragma once

namespace sys {
  namespace lang {

	class Runnable {
	public:
		virtual ~Runnable() {
		}

		virtual void run() = 0;

	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

