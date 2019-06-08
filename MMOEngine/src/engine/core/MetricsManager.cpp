/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
//
// Created by Victor Popovici on 8/4/17.
//

#include "MetricsManager.h"

using namespace engine::core;

MetricsManager::MetricsManager() : Logger("MetricsManager"), client(nullptr) {
}

void MetricsManager::initializeStatsDConnection(const char* hostname, int port) {
	DatagramServiceClient* newClient = new DatagramServiceClient(hostname, port);

	client = newClient;
}

MetricsManager::Result MetricsManager::publish(const Metrics& metrics) {
	return publish(metrics.name, metrics.value, metrics.type, metrics.sampleTime);
}

MetricsManager::Result MetricsManager::publish(const char* name, const char* value, const char* type, const char* sampleTime) {
	if (client == nullptr) {
		return NO_CONNECTION;
	}

	try {
		Packet pack(64);

		pack.insertCString(globalPrefix.toCharArray());
		pack.insertCString(name);
		pack.insertCString(":");
		pack.insertCString(value);
		pack.insertCString("|");
		pack.insertCString(type);

		if (sampleTime) {
			pack.insertCString("|@");
			pack.insertCString(sampleTime);
		}

		client->send(&pack);
	} catch (const SocketException& exc) {
		error(exc.getMessage());

		return SOCKET_EXCEPTION;
	}

	return SUCCESS;
}
