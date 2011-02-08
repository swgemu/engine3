/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "engine/core/Core.h"

#include "engine/orb/DistributedObjectBroker.h"
#include "engine/orb/DistributedObjectBrokerClient.h"

#include "DistributedObject.h"

#include "DistributedMethod.h"

#include "engine/orb/packets/InvokeMethodMessage.h"

DistributedMethod::DistributedMethod(DistributedObject* obj, uint32 methid) {
	object = obj;
	methodID = methid;

	orb = NULL;

	invmsg = new InvokeMethodMessage(object->_getObjectID(), methid, 0);
}

DistributedMethod::DistributedMethod(DistributedObjectBroker* broker, Packet* pack) {
	orb = broker;

	invmsg = pack;
}

void DistributedMethod::executeWithVoidReturn() {
	Packet resp;

	execute(&resp);
}

bool DistributedMethod::executeWithBooleanReturn() {
	Packet resp;

	execute(&resp);

	return resp.parseByte(8);
}

byte DistributedMethod::executeWithByteReturn() {
	Packet resp;

	execute(&resp);

	return resp.parseByte(8);
}

char DistributedMethod::executeWithSignedCharReturn() {
	Packet resp;

	execute(&resp);

	return resp.parseSignedByte(8);
}

unsigned char DistributedMethod::executeWithUnsignedCharReturn() {
	Packet resp;

	execute(&resp);

	return resp.parseByte(8);
}

int DistributedMethod::executeWithSignedIntReturn() {
	Packet resp;

	execute(&resp);

	return resp.parseSignedInt(8);
}

unsigned int DistributedMethod::executeWithUnsignedIntReturn() {
	Packet resp;

	execute(&resp);

	return resp.parseInt(8);
}

long long DistributedMethod::executeWithSignedLongReturn() {
	Packet resp;

	execute(&resp);

	return resp.parseSignedLong(8);
}

unsigned long long DistributedMethod::executeWithUnsignedLongReturn() {
	Packet resp;

	execute(&resp);

	return resp.parseLong(8);
}

float DistributedMethod::executeWithFloatReturn() {
	Packet resp;

	execute(&resp);

	return resp.parseFloat(8);
}

void DistributedMethod::executeWithAsciiReturn(String& value) {
	Packet resp;

	execute(&resp);

	resp.parseAscii(8, value);
}

void DistributedMethod::executeWithUnicodeReturn(UnicodeString& value) {
	Packet resp;

	execute(&resp);

	resp.parseUnicode(8, value);
}

DistributedObject* DistributedMethod::executeWithObjectReturn() {
	Packet resp;

	execute(&resp);

	uint64 objid = resp.parseLong(8);
	if (objid != 0)
		return Core::getObjectBroker()->lookUp(objid);
	else
		return NULL;
}

void DistributedMethod::execute(Packet* response) {
	DistributedObjectBrokerClient* client = object->_getNamingDirectory()->getClient();
	client->send(invmsg);

	client->read(response);
}

void DistributedMethod::addBooleanParameter(bool val) {
	invmsg->insertByte(val);
}

void DistributedMethod::addByteParameter(byte val) {
	invmsg->insertByte(val);
}

void DistributedMethod::addSignedCharParameter(char val) {
	invmsg->insertSignedByte(val);
}

void DistributedMethod::addUnsignedCharParameter(unsigned char val) {
	invmsg->insertByte(val);
}

void DistributedMethod::addSignedIntParameter(int val) {
	invmsg->insertSignedInt(val);
}

void DistributedMethod::addUnsignedIntParameter(unsigned int val) {
	invmsg->insertInt(val);
}

void DistributedMethod::addSignedLongParameter(long long val) {
	invmsg->insertSignedLong(val);
}

void DistributedMethod::addUnsignedLongParameter(unsigned long long val) {
	invmsg->insertLong(val);
}

void DistributedMethod::addFloatParameter(float val) {
	invmsg->insertFloat(val);
}

void DistributedMethod::addAsciiParameter(const String& ascii) {
	invmsg->insertAscii(ascii);
}

void DistributedMethod::addUnicodeParameter(const UnicodeString& str) {
	invmsg->insertUnicode(str);
}

void DistributedMethod::addObjectParameter(DistributedObject* obj) {
	if (obj != NULL)
		invmsg->insertLong(obj->_getObjectID());
	else
		invmsg->insertLong(0);
}

bool DistributedMethod::getBooleanParameter() {
	return invmsg->parseByte();
}

byte DistributedMethod::getByteParameter() {
	return invmsg->parseByte();
}

char DistributedMethod::getSignedCharParameter() {
	return invmsg->parseSignedByte();
}

unsigned char DistributedMethod::getUnsignedCharParameter() {
	return invmsg->parseByte();
}

int DistributedMethod::getSignedIntParameter() {
	return invmsg->parseSignedInt();
}

unsigned int DistributedMethod::getUnsignedIntParameter() {
	return invmsg->parseInt();
}

long long DistributedMethod::getSignedLongParameter() {
	return invmsg->parseSignedLong();
}

unsigned long long DistributedMethod::getUnsignedLongParameter() {
	return invmsg->parseLong();
}

float DistributedMethod::getFloatParameter() {
	return invmsg->parseFloat();
}

String& DistributedMethod::getAsciiParameter(String& ascii) {
	invmsg->parseAscii(ascii);

	return ascii;
}

UnicodeString& DistributedMethod::getUnicodeParameter(UnicodeString& str) {
	invmsg->parseUnicode(str);

	return str;
}

DistributedObject* DistributedMethod::getObjectParameter() {
	uint64 objid = invmsg->parseLong();

	if (objid != 0)
		return orb->lookUp(objid);
	else
		return NULL;
}
