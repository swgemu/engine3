/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef SERVICETHREAD_H_
#define SERVICETHREAD_H_

#include "system/lang.h"

#include "engine/log/Logger.h"

#include "ServiceException.h"

namespace engine {
  namespace service {

	class ServiceThread : public Thread, public Mutex, public Logger, public virtual Object {
	protected:
		AtomicBoolean doRun, serviceReady;

	public:
		constexpr const static bool is_virtual_object = true;

		ServiceThread(const String& s);

		virtual ~ServiceThread();

		virtual void init();

		void start(bool waitForStartup = true);

		virtual void run();

		virtual void stop(bool doJoin = true);

		// setters and getters
		inline void setRunning(bool val) {
			doRun = val;
		}

		inline bool isRunning() {
			return doRun;
		}

		inline void setReady(bool val) {
			serviceReady = val;
		}

		inline bool isReady() {
			return serviceReady;
		}
	};

  } // namespace service
} // namespace engine

using namespace engine::service;

#endif /*SERVICETHREAD_H_*/
