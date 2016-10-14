/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "engine/core/Core.h"

#include "engine/orb/DistributedObjectBroker.h"

#include "engine/orb/messages/RemoteObjectBroker.h"

#include "engine/orb/messages/InvokeMethodMessage.h"

DistributedMethod::DistributedMethod(const DistributedObject* obj, uint32 methid, bool async) {
	object = obj;
	methodID = methid;

	orb = NULL;

	invocationMessgage = new InvokeMethodMessage(object->_getObjectID(), methid, 0, async);
	response = NULL;
}

DistributedMethod::DistributedMethod(DistributedObjectBroker* broker, InvokeMethodMessage* invmsg) {
	orb = broker;

	invocationMessgage = invmsg;
	response = invmsg;
}

DistributedMethod::~DistributedMethod() {
	if (invocationMessgage != NULL)
		delete invocationMessgage;

	/*if (response != NULL)
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
		return NULL;
}

void DistributedMethod::execute(bool asyncMethod) {
	RemoteObjectBroker* broker = dynamic_cast<RemoteObjectBroker*>(object->_getObjectBroker());
	assert(broker != NULL);

	broker->invokeMethod(*this, asyncMethod);

	if (!asyncMethod) {
		response = invocationMessgage->getResponseMessage();
	}
}

void DistributedMethod::addBooleanParameter(bool val) {
	invocationMessgage->insertBoolean(val);
}

void DistributedMethod::addByteParameter(byte val) {
	invocationMessgage->insertByte(val);
}

void DistributedMethod::addSignedCharParameter(char val) {
	invocationMessgage->insertSignedByte(val);
}

void DistributedMethod::addUnsignedCharParameter(unsigned char val) {
	invocationMessgage->insertByte(val);
}

void DistributedMethod::addSignedIntParameter(int val) {
	invocationMessgage->insertSignedInt(val);
}

void DistributedMethod::addUnsignedIntParameter(unsigned int val) {
	invocationMessgage->insertInt(val);
}

void DistributedMethod::addSignedLongParameter(long long val) {
	invocationMessgage->insertSignedLong(val);
}

void DistributedMethod::addUnsignedLongParameter(unsigned long long val) {
	invocationMessgage->insertLong(val);
}

void DistributedMethod::addSignedShortParameter(short val) {
	invocationMessgage->insertSignedShort(val);
}

void DistributedMethod::addUnsignedShortParameter(unsigned short val) {
	invocationMessgage->insertShort(val);
}

void DistributedMethod::addFloatParameter(float val) {
	invocationMessgage->insertFloat(val);
}

void DistributedMethod::addDoubleParameter(double val) {
	invocationMessgage->insertDouble(val);
}

void DistributedMethod::addAsciiParameter(const String& ascii) {
	invocationMessgage->insertAscii(ascii);
}

void DistributedMethod::addUnicodeParameter(const UnicodeString& str) {
	invocationMessgage->insertUnicode(str);
}

void DistributedMethod::addObjectParameter(DistributedObject* obj) {
	if (obj != NULL)
		invocationMessgage->insertLong(obj->_getObjectID());
	else
		invocationMessgage->insertLong(0);
}

bool DistributedMethod::getBooleanParameter() {
	return invocationMessgage->getBooleanParameter();
}

byte DistributedMethod::getByteParameter() {
	return invocationMessgage->getByteParameter();
}

char DistributedMethod::getSignedCharParameter() {
	return invocationMessgage->getSignedCharParameter();
}

unsigned char DistributedMethod::getUnsignedCharParameter() {
	return invocationMessgage->getUnsignedCharParameter();
}

int DistributedMethod::getSignedIntParameter() {
	return invocationMessgage->getSignedIntParameter();
}

unsigned int DistributedMethod::getUnsignedIntParameter() {
	return invocationMessgage->getUnsignedIntParameter();
}

long long DistributedMethod::getSignedLongParameter() {
	return invocationMessgage->getSignedLongParameter();
}

unsigned long long DistributedMethod::getUnsignedLongParameter() {
	return invocationMessgage->getUnsignedLongParameter();
}

short DistributedMethod::getSignedShortParameter() {
	return invocationMessgage->getSignedShortParameter();
}

unsigned short DistributedMethod::getUnsignedShortParameter() {
	return invocationMessgage->getUnsignedShortParameter();
}

float DistributedMethod::getFloatParameter() {
	return invocationMessgage->getFloatParameter();
}

double DistributedMethod::getDoubleParameter() {
	return invocationMessgage->getDoubleParameter();
}

String& DistributedMethod::getAsciiParameter(String& ascii) {
	invocationMessgage->getAsciiParameter(ascii);

	return ascii;
}

UnicodeString& DistributedMethod::getUnicodeParameter(UnicodeString& str) {
	invocationMessgage->getUnicodeParameter(str);

	return str;
}

DistributedObject* DistributedMethod::getObjectParameter() {
	uint64 objid = invocationMessgage->parseLong();

	if (objid != 0)
		return orb->lookUp(objid);
	else
		return NULL;
}
