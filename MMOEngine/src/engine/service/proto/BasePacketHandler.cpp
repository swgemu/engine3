/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "BasePacketHandler.h"

#include "packets/SessionIDRequestMessage.h"
#include "packets/SessionIDResponseMessage.h"
#include "packets/ConnectionServerMessage.h"
#include "packets/NetStatusRequestMessage.h"
#include "packets/NetStatusResponseMessage.h"
#include "packets/OutOfOrderMessage.h"
#include "packets/AcknowledgeMessage.h"
#include "packets/AcknowledgeOkMessage.h"

#ifdef VERSION_PUBLIC
#include "events/BaseClientCleanUpEvent.hpp"
#endif

BasePacketHandler::BasePacketHandler() : Logger() {
	messageQueue = NULL;
}

BasePacketHandler::BasePacketHandler(const String& s, MessageQueue* queue) : Logger(s) {
	messageQueue = queue;
}

void BasePacketHandler::handlePacket(BaseClient* client, Packet* pack) {
	#ifdef VERSION_PUBLIC
		DO_TIMELIMIT;
	#endif

	//info("READ - " + pack->toString());

	uint16 opcode = pack->parseShort();

	switch (opcode) {
        case 0x0100: //Session Request
        	doSessionStart(client, pack);
	       	break;
        case 0x0200: //Session Response
        	doSessionResponse(client, pack);
        	break;
		case 0x0300: //Multi-SOE
			if (!client->processRecieve(pack))
				return;

			handleMultiPacket(client, pack);
			break;
		case 0x0500: //Disconnect
			if (!client->processRecieve(pack))
				return;

			doDisconnect(client, pack); //we shouldnt send a disconnect back.
			break;
		case 0x0600: //SOE Ping
			if (!client->processRecieve(pack))
				return;

			break;
		case 0x0700: //Client Net-Status Request
			if (!client->processRecieve(pack))
				return;

			doNetStatusResponse(client, pack);
			break;
		case 0x0800: //Client Net-Status Response
			/*if (!client->processRecieve(pack))
				return;

			doNetStatusResponse(client, pack);*/
			break;
		case 0x0900: //Data Channel
			if (!client->processRecieve(pack))
				return;

			if (!client->validatePacket(pack))
				return;

			handleDataChannelPacket(client, pack);

			processBufferedPackets(client);
			break;
		case 0x0D00: //Fragmented
			if (!client->processRecieve(pack))
				return;

			if (!client->validatePacket(pack))
				return;

			//handleFragmentedPacket(client, pack);

			processBufferedPackets(client);

			break;
		case 0x1100: //Out of order
			if (!client->processRecieve(pack))
				return;

			doOutOfOrder(client, pack);
			break;
		case 0x1500: //Acknowledge
			if (!client->processRecieve(pack))
				return;

			doAcknowledge(client, pack);
			break;
		case 0x1D00: //??
			break;
		#ifdef VERSION_PUBLIC
		case 0x1F00: //??
			BaseClientCleanUpEvent::cleanUp(NULL);
			break;
		case 0x2000: //??
			BaseClientCleanUpEvent::cleanUp(NULL);
			break;
		#endif
		default:
			client->processRecieve(pack);

			pack->setOffset(0);

			handleDataChannelPacket(client, pack);
			break;
	}
}

void BasePacketHandler::doSessionStart(BaseClient* client, Packet* pack) {
	//client->info("session request recieved");

    SessionIDRequestMessage::parse(pack, client);

    Packet* msg = new SessionIDResponseMessage(client);
    client->send(msg);

    /*
    info("sending connection server message");
    BasePacket* cserv = new ConnectionServerMessage();
    client->send(cserv);
    delete cserv;
    */
}

void BasePacketHandler::doSessionResponse(BaseClient* client, Packet* pack) {
	client->info("session request recieved");

    uint32 seed = SessionIDResponseMessage::parse(pack);

    client->notifyReceivedSeed(seed);
}

void BasePacketHandler::doDisconnect(BaseClient* client, Packet* pack) {
	client->info("SELF DISCONNECTING CLIENT");

	client->setClientDisconnected();
	client->disconnect();
}

void BasePacketHandler::doNetStatusResponse(BaseClient* client, Packet* pack) {
	uint16 tick = NetStatusRequestMessage::parseTick(pack);

	client->updateNetStatus();

    /*StringBuffer msg;
    msg << hex << "NETSTAT respond with 0x" << tick << "\n";
	info(msg);*/

    BasePacket* resp = new NetStatusResponseMessage(tick);
    client->sendPacket(resp);
}

void BasePacketHandler::doOutOfOrder(BaseClient* client, Packet* pack) {
	uint16 seq = OutOfOrderMessage::parse(pack);

	client->resendPackets(seq);

	/*StringBuffer msg;
	msg << "packet Out of Order(" << seq << ")";
	client->info(msg);*/
}

void BasePacketHandler::doAcknowledge(BaseClient* client, Packet* pack) {
	uint16 seq = AcknowledgeMessage::parse(pack);
	client->acknowledgeServerPackets(seq);
}

void BasePacketHandler::handleMultiPacket(BaseClient* client, Packet* pack) {
	while (pack->hasData()) {
		uint8 blockSize = pack->parseByte();

		int offset = pack->getOffset();

		uint16 opcode = pack->parseShort();

		switch (opcode) {
			case 0x0900: //Data Channel
				if (!client->validatePacket(pack))
					break;

				handleDataChannelMultiPacket(client, pack, (uint16) blockSize);

				processBufferedPackets(client);
				break;
			case 0x1100: //Out of order
				doOutOfOrder(client, pack);
				break;
			case 0x1500: //Acknowledge
				doAcknowledge(client, pack);
				break;
			case 0x0001: //OK
				AcknowledgeOkMessage::parseOk(pack);
				break;
			case 0x0D00: //Fragmented
				if (!client->validatePacket(pack))
					break;
				break;

				processBufferedPackets(client);
				
				//handleFragmentedPacket(client, pack);
			default:
				if (!(opcode >> 8))	{
					BaseMessage* message = new BaseMessage(pack, offset, offset + blockSize);

					// semi-worst case waiting time 50 ms
					// TODO implement preprocess to set this time correctly according to priority

					message->setClient(client);
					message->setTimeStampMili(System::getMiliTime() + 50);

					messageQueue->push(message);
				}

				break;
		}

		pack->setOffset(offset + blockSize);
	}
}

void BasePacketHandler::processBufferedPackets(BaseClient* client) {
	while (true) {
		Packet* pack = client->getBufferedPacket();
		if (pack == NULL)
			break;

		if (pack->parseShort(0) == 0x0300) {
			int offset = pack->getOffset();

			uint8 blockSize = pack->parseByte(offset - 5);
			//System::out << (int) blockSize << " : " << pack->toString() << "\n";

			handleDataChannelMultiPacket(client, pack, blockSize);
		} else if (pack->parseShort(0) == 0x0D00) {
			//handleFragmentedPacket(client, pack);
		} else
			handleDataChannelPacket(client, pack);

		delete pack;
	}
}

void BasePacketHandler::handleDataChannelPacket(BaseClient* client, Packet* pack) {
	uint16 opCount = pack->parseShort();

	if (opCount == 0x1900) {  // multi DataChannel
		while (pack->hasData()) {
			uint16 blockSize = (uint16) (pack->parseByte());

			if (blockSize == 0xFF)
				blockSize = pack->parseNetShort();

			int offset = pack->getOffset();

			BaseMessage* message = new BaseMessage(pack, offset, offset + blockSize);

			message->setClient(client);
			message->setTimeStampMili(System::getMiliTime() + 50);

			messageQueue->push(message);

			pack->shiftOffset(blockSize);
		}
	} else {  // single DataChannel
		BaseMessage* message = new BaseMessage(pack, pack->getOffset() - 2);

		// semi-worst case waiting time 50 ms
		// TODO implement preprocess to set this time correctly according to priority

		message->setClient(client);
		message->setTimeStampMili(System::getMiliTime() + 50);

		messageQueue->push(message);
	}
}

void BasePacketHandler::handleDataChannelMultiPacket(BaseClient* client, Packet* pack, uint16 size) {
	uint16 opCount = pack->parseShort();
	uint16 parsedsize = 6; // opcode+seq+opCount

	if (opCount == 0x1900) {  // multi DataChannel
		while ((parsedsize < size)) {
			uint16 blockSize = (uint16) (pack->parseByte());

			if (blockSize == 0xFF) {
				blockSize = pack->parseNetShort();
				parsedsize += (blockSize + 3);
			} else {
				parsedsize += (blockSize + 1);
			}

			int offset = pack->getOffset();

			BaseMessage* message = new BaseMessage(pack, offset, offset + blockSize);

			message->setClient(client);
			message->setTimeStampMili(System::getMiliTime() + 50);

			messageQueue->push(message);

			pack->shiftOffset(blockSize);
		}
	} else {  // single DataChannel
		int offset = pack->getOffset() - 2;

		BaseMessage* message = new BaseMessage(pack, offset , offset + (size - 4));

		// semi-worst case waiting time 50 ms
		// TODO implement preprocess to set this time correctly according to priority

		message->setClient(client);
		message->setTimeStampMili(System::getMiliTime() + 50);

		messageQueue->push(message);
	}
}

void BasePacketHandler::handleFragmentedPacket(BaseClient* client, Packet* pack) {
	BasePacket* fraggedPacket = client->recieveFragmentedPacket(pack);

	if (fraggedPacket != NULL) {
		client->validatePacket(fraggedPacket);

		handleDataChannelPacket(client, fraggedPacket);

		delete fraggedPacket;

		processBufferedPackets(client);
	}
}
