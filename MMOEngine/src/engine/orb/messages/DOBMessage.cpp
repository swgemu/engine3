/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
//
// Created by Victor Popovici on 17/10/16.
//

#include "DOBMessage.h"

DOBMessage::DOBMessage(uint32 messageType) : Packet(20), client(nullptr), sequence(0), async(false) {
	insertInt(0); //size
	insertInt(messageType);
	insertInt(0); // sequence
}

DOBMessage::DOBMessage(uint32 messageType, uint32 initialBufferSize) : Packet(initialBufferSize), client(nullptr), sequence(0), async(false) {
	insertInt(0); //size
	insertInt(messageType);
	insertInt(0); // sequence
}

DOBMessage::DOBMessage(Packet* packet) : Packet(40), client(nullptr), async(false) {
	sequence = packet->parseInt();

	insertInt(0); //size
	insertInt(REPLYMESSAGE); // messageType
	insertInt(sequence); // sequence
}

DOBMessage::~DOBMessage() {
}

void DOBMessage::signalReply() {
	replyCondition.signal();
}

bool DOBMessage::waitForReply(uint32 timeout) {
	Time time;
	time.addMiliTime(timeout);

	return replyCondition.timedWait(&time) == 0;
}
