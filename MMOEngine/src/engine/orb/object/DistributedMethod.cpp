/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "engine/core/Core.h"

#include "engine/orb/DistributedObjectBroker.h"

#include "engine/orb/messages/RemoteObjectBroker.h"

#include "engine/orb/messages/InvokeMethodMessage.h"

DistributedMethod::DistributedMethod(const DistributedObject* obj, uint32 methid, bool async) {
	object = obj;
	methodID = methid;

	invocationMessage = new InvokeMethodMessage(object->_getObjectID(), methid, 0, async);
}

DistributedMethod::DistributedMethod(DistributedObjectBroker* broker, InvokeMethodMessage* invmsg) {
	orb = broker;

	invocationMessage = invmsg;
	response = invmsg;
}

DistributedMethod::~DistributedMethod() {
	if (invocationMessage != nullptr)
		delete invocationMessage;

	/*if (response != nullptr)
		delete response;*/
}

void DistributedMethod::executeWithVoidReturn(bool async) {
	execute(async);
}

bool DistributedMethod::executeWithBooleanReturn() {
	execute();

	return response->parseBoolean();
}

byte DistributedMethod::executeWithByteReturn() {
	execute();

	return response->parseByte();
}

char DistributedMethod::executeWithSignedCharReturn() {
	execute();

	return response->parseSignedByte();
}

unsigned char DistributedMethod::executeWithUnsignedCharReturn() {
	execute();

	return response->parseByte();
}

int DistributedMethod::executeWithSignedIntReturn() {
	execute();

	return response->parseSignedInt();
}

unsigned int DistributedMethod::executeWithUnsignedIntReturn() {
	execute();

	return response->parseInt();
}

long long DistributedMethod::executeWithSignedLongReturn() {
	execute();

	return response->parseSignedLong();
}

unsigned long long DistributedMethod::executeWithUnsignedLongReturn() {
	execute();

	return response->parseLong();
}

float DistributedMethod::executeWithFloatReturn() {
	execute();

	return response->parseFloat();
}

double DistributedMethod::executeWithDoubleReturn() {
	execute();

	return response->parseDouble();
}

void DistributedMethod::executeWithAsciiReturn(String& value) {
	execute();

	response->parseAscii(value);
}

void DistributedMethod::executeWithUnicodeReturn(UnicodeString& value) {
	execute();

	response->parseUnicode(value);
}

short DistributedMethod::executeWithSignedShortReturn() {
	execute();

	return response->parseSignedShort();
}

unsigned short DistributedMethod::executeWithUnsignedShortReturn() {
	execute();

	return response->parseShort();
}

DistributedObject* DistributedMethod::executeWithObjectReturn() {
	execute();

	uint64 objid = response->parseLong();
	if (objid != 0)
		return Core::getObjectBroker()->lookUp(objid);
	else
		return nullptr;
}

void DistributedMethod::execute(bool asyncMethod) {
	RemoteObjectBroker* broker = dynamic_cast<RemoteObjectBroker*>(object->_getObjectBroker());
	if (broker == nullptr) {
		throw Exception("cannot do RPC beacuase remote agent died");
	}

	Locker locker(broker);

	broker->invokeMethod(*this, asyncMethod);

	if (!asyncMethod) {
		response = invocationMessage->getResponseMessage();
	}
}

void DistributedMethod::addBooleanParameter(bool val) {
	invocationMessage->insertBoolean(val);
}

void DistributedMethod::addByteParameter(byte val) {
	invocationMessage->insertByte(val);
}

void DistributedMethod::addSignedCharParameter(char val) {
	invocationMessage->insertSignedByte(val);
}

void DistributedMethod::addUnsignedCharParameter(unsigned char val) {
	invocationMessage->insertByte(val);
}

void DistributedMethod::addSignedIntParameter(int val) {
	invocationMessage->insertSignedInt(val);
}

void DistributedMethod::addUnsignedIntParameter(unsigned int val) {
	invocationMessage->insertInt(val);
}

void DistributedMethod::addSignedLongParameter(long long val) {
	invocationMessage->insertSignedLong(val);
}

void DistributedMethod::addUnsignedLongParameter(unsigned long long val) {
	invocationMessage->insertLong(val);
}

void DistributedMethod::addSignedShortParameter(short val) {
	invocationMessage->insertSignedShort(val);
}

void DistributedMethod::addUnsignedShortParameter(unsigned short val) {
	invocationMessage->insertShort(val);
}

void DistributedMethod::addFloatParameter(float val) {
	invocationMessage->insertFloat(val);
}

void DistributedMethod::addDoubleParameter(double val) {
	invocationMessage->insertDouble(val);
}

void DistributedMethod::addAsciiParameter(const String& ascii) {
	invocationMessage->insertAscii(ascii);
}

void DistributedMethod::addUnicodeParameter(const UnicodeString& str) {
	invocationMessage->insertUnicode(str);
}

void DistributedMethod::addObjectParameter(DistributedObject* obj) {
	if (obj != nullptr)
		invocationMessage->insertLong(obj->_getObjectID());
	else
		invocationMessage->insertLong(0);
}

bool DistributedMethod::getBooleanParameter() {
	return invocationMessage->getBooleanParameter();
}

byte DistributedMethod::getByteParameter() {
	return invocationMessage->getByteParameter();
}

char DistributedMethod::getSignedCharParameter() {
	return invocationMessage->getSignedCharParameter();
}

unsigned char DistributedMethod::getUnsignedCharParameter() {
	return invocationMessage->getUnsignedCharParameter();
}

int DistributedMethod::getSignedIntParameter() {
	return invocationMessage->getSignedIntParameter();
}

unsigned int DistributedMethod::getUnsignedIntParameter() {
	return invocationMessage->getUnsignedIntParameter();
}

long long DistributedMethod::getSignedLongParameter() {
	return invocationMessage->getSignedLongParameter();
}

unsigned long long DistributedMethod::getUnsignedLongParameter() {
	return invocationMessage->getUnsignedLongParameter();
}

short DistributedMethod::getSignedShortParameter() {
	return invocationMessage->getSignedShortParameter();
}

unsigned short DistributedMethod::getUnsignedShortParameter() {
	return invocationMessage->getUnsignedShortParameter();
}

float DistributedMethod::getFloatParameter() {
	return invocationMessage->getFloatParameter();
}

double DistributedMethod::getDoubleParameter() {
	return invocationMessage->getDoubleParameter();
}

String& DistributedMethod::getAsciiParameter(String& ascii) {
	invocationMessage->getAsciiParameter(ascii);

	return ascii;
}

UnicodeString& DistributedMethod::getUnicodeParameter(UnicodeString& str) {
	invocationMessage->getUnicodeParameter(str);

	return str;
}

DistributedObject* DistributedMethod::getObjectParameter() {
	uint64 objid = invocationMessage->getIncomingPacket()->parseLong();

	//printf("receiving oid:%d\n", (int)objid);

	if (objid != 0) {
		return DistributedObjectBroker::instance()->lookUp(objid);
	} else {
		return nullptr;
	}
}
