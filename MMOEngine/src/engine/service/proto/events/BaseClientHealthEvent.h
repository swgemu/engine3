/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

/**
 * @author      : lordkator (lordkator@swgemu.com)
 * @file        : BaseClientHealthEvent.h
 * @created     : Mon Jan 24 12:22:23 UTC 2022
 */

#pragma once

#include "engine/service/proto/BaseClient.h"

namespace engine {
 namespace service {
  namespace proto {

	class BaseClientHealthEvent : public Task {
		WeakReference<BaseClient*> client;

	public:
		BaseClientHealthEvent(BaseClient* client) {
			BaseClientHealthEvent::client = client;

#if defined(BASECLIENT_DISABLE_STATSD) and defined(COLLECT_TASKSTATISTICS)
			setStatsSample(0);
#endif
		}

		void shutdown() {
			cancel();
			client = nullptr;
		}

		void run() {
			auto strong = client.get();

			if (strong == nullptr)
				return;

			strong->runHealthCheck();
		}

	};

    } // namespace proto
  } // namespace service
} // namespace engine
