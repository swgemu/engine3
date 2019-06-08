/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * TransactionBaseClientManager.cpp
 *
 *  Created on: 08/06/2011
 *      Author: victor
 */

#include "system/platform.h"

#include "engine/service/proto/BasePacket.h"
#include "engine/service/proto/BaseClient.h"

#include "TransactionalBaseClientManager.h"


TransactionalBaseClientManager::TransactionalBaseClientManager() : Logger("TransactionalBaseClientManager") {
	setLogging(false);
	setGlobalLogging(false);
}

void TransactionalBaseClientManager::sendPacket(BasePacket* packet, BaseClient* baseClient) {
	//BaseClient* baseClient = packet->getClient()
	packet->setClient(baseClient);

	//VectorMap<BaseClient*, Vector<BasePacket*>*>* packetBuffer = getLocalBufferedPackets();

	Vector<BasePacket*>* buffer =  getLocalBufferedPackets();//packetBuffer->get(baseClient);

	/*if (buffer == nullptr) {
		buffer = new Vector<BasePacket*>();
		packetBuffer->put(baseClient, buffer);
	}*/

	buffer->add(packet);
}

Vector<BasePacket*>* TransactionalBaseClientManager::getLocalBufferedPackets() {
	Vector<BasePacket*>* queue = bufferedPackets.get();

	if (queue == nullptr) {
		queue = new Vector<BasePacket*>();
		//queue->setNoDuplicateInsertPlan();

		//info("message queue created");
		//queue->setNullValue(nullptr);

		bufferedPackets.set(queue);
	}

	return queue;
}

void TransactionalBaseClientManager::execute() {
	//VectorMap<BaseClient*, Vector<BasePacket*>*>* queue = getLocalBufferedPackets();
	Vector<BasePacket*>* packetQueue = getLocalBufferedPackets();

	/*if (queue->size() > 0)
		info("sending " + String::valueOf(queue->size()) + " messages", true);*/

	for (int j = 0; j < packetQueue->size(); ++j) {
		//BasePacket* pack = queue->get(i);
		BasePacket* pack = packetQueue->get(j);

		BaseClient* baseClient = dynamic_cast<BaseClient*>(pack->getClient());

		pack->setClient(nullptr);

		Locker locker(baseClient);

		if (!baseClient->isAvailable()) {
			//info("baseClient is not available", true);

			delete pack;

			continue;
		}

		try {
			if (pack->doSequencing()) {
				if (pack->size() >= 490) {
					if (baseClient->getRawBufferedPacket() != nullptr) {
						baseClient->sendSequenced(baseClient->getRawBufferedPacket()->getPacket());
						baseClient->setNullBufferedPacket();
						//bufferedPacket = nullptr;
					}

					baseClient->sendFragmented(pack);
				} else
					baseClient->bufferMultiPacket(pack);
			} else {
				baseClient->sendSequenceLess(pack);
			}
		} catch (Exception& e) {
			error(e.getMessage());
		} catch (...) {
			error("unreported exception on sendPacket()");
			//baseClient->disconnect("unreported exception on sendPacket()", false);
		}
	}

	packetQueue->removeAll(1000, 100);
}

void TransactionalBaseClientManager::undo() {
	Vector<BasePacket*>* packetQueue = getLocalBufferedPackets();

	for (int j = 0; j < packetQueue->size(); ++j) {
		Message* message = packetQueue->get(j);

		delete message;
	}

	packetQueue->removeAll(1000, 100);
}
