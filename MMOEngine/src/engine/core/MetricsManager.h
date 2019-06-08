/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
//
// Created by Victor Popovici on 8/4/17.
//

#ifndef ENGINE3_METRICSMANAGER_H
#define ENGINE3_METRICSMANAGER_H

#include "engine/engine.h"

#include "RunStatistics.h"

namespace engine {
	namespace service {
		class DatagramServiceClient;
	}

	struct Metrics {
		const char* name;
		const char* value;
		const char* type;
		const char* sampleTime;

		Metrics() : name(nullptr), value(nullptr), type(nullptr), sampleTime(nullptr) {

		}

		Metrics(const char* name, const char* value, const char* type, const char* sampleTime = nullptr)
					: name(name), value(value), type(type), sampleTime(sampleTime) {

		}
	};

	namespace core {
		class MetricsManager : public Singleton<MetricsManager>, public Object, public Logger {
		protected:
			Reference<engine::service::DatagramServiceClient*> client;
			String globalPrefix;

		public:
			enum Result {SUCCESS = 0, NO_CONNECTION, SOCKET_EXCEPTION, GENERAL_ERROR};

			MetricsManager();

			void initializeStatsDConnection(const char* hostname, int port);

			Result publish(const char* name, const char* value, const char* type, const char* sampleTime = nullptr);
			Result publish(const Metrics& metrics);

			void setGlobalPrefix(const String& prefix) {
				globalPrefix = prefix;
			}
		};

	}
}

#endif //ENGINE3_METRICSMANAGER_H
