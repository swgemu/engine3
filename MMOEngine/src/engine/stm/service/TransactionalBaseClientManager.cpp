/*
 * TransactionBaseClientManager.cpp
 *
 *  Created on: 08/06/2011
 *      Author: victor
 */

#include "engine/engine.h"
#include "TransactionalBaseClientManager.h"

#include "engine/service/proto/BasePacket.h"
#include "engine/service/proto/BaseClient.h"

TransactionalBaseClientManager::TransactionalBaseClientManager() : Logger("TransactionalBaseClientManager") {
	setLogging(false);
	setGlobalLogging(false);
}

void TransactionalBaseClientManager::sendPacket(BasePacket* packet, BaseClient* baseClient) {
	//BaseClient* baseClient = packet->getClient()
	packet->setClient(baseClient);

	Vector<BasePacket*>* packetBuffer = getLocalBufferedPackets();

	packetBuffer->add(packet);
}

Vector<BasePacket*>* TransactionalBaseClientManager::getLocalBufferedPackets() {
	Vector<BasePacket*>* queue = bufferedPackets.get();

	if (queue == NULL) {
		queue = new Vector<BasePacket*>();

		//info("message queue created");

		bufferedPackets.set(queue);
	}

	return queue;
}

void TransactionalBaseClientManager::execute() {
	Vector<BasePacket*>* queue = getLocalBufferedPackets();

	/*if (queue->size() > 0)
		info("sending " + String::valueOf(queue->size()) + " messages", true);*/

	for (int i = 0; i < queue->size(); ++i) {
		BasePacket* pack = queue->get(i);
		BaseClient* baseClient = dynamic_cast<BaseClient*>(pack->getClient());

		pack->setClient(NULL);

		Locker locker(baseClient);

		if (!baseClient->isAvailable()) {
			//info("baseClient is not available", true);

			delete pack;

			continue;
		}

		try {
			if (pack->doSequencing()) {
				if (pack->size() >= 490) {
					if (baseClient->getRawBufferedPacket() != NULL) {
						baseClient->sendSequenced(baseClient->getRawBufferedPacket()->getPacket());
						baseClient->setNullBufferedPacket();
						//bufferedPacket = NULL;
					}

					baseClient->sendFragmented(pack);
				} else
					baseClient->bufferMultiPacket(pack);
			} else {
				baseClient->sendSequenceLess(pack);
			}
		} catch (...) {
			error("HUI CATCH (...)");
			baseClient->disconnect("unreported exception on sendPacket()", false);
		}

	}

	queue->removeAll(1000, 100);
}

void TransactionalBaseClientManager::undo() {
	Vector<BasePacket*>* queue = getLocalBufferedPackets();

	for (int i = 0; i < queue->size(); ++i) {
		Message* message = queue->get(i);

		delete message;
	}

	queue->removeAll(1000, 100);
}
