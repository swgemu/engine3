/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "SocketImplementation.h"

#ifndef PLATFORM_WIN
#include <sys/socket.h>
#include <sys/ioctl.h>
#else
#include <winsock2.h>
#endif

#include "Packet.h"

#include "SocketException.h"

SocketImplementationInitializer SocketImplementation::socketInitializer;

SocketImplementation::SocketImplementation(int type) {
	switch (type) {
	case STREAM_SOCKET:
		createStreamSocket();
		break;
	case DATAGRAM_SOCKET:
		createDatagramSocket();
		break;
	}

	setTimeOut(DEFAULT_TIMEOUT);
}

void SocketImplementation::createStreamSocket() {
	int sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (sock < 0)
		throw SocketException("unable to create socket");
}

void SocketImplementation::createDatagramSocket() {
	int sock = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (sock < 0)
		throw SocketException("unable to create socket");
}

SocketImplementation* SocketImplementation::accept() {
	int handle = ::accept(sock, 0, 0);

	if (handle < 0)
		throw SocketException("failed to accept connection");

	return new SocketImplementation(handle);
}

SocketImplementation* SocketImplementation::accept(const SocketAddress& addr) {
	#ifndef PLATFORM_WIN
		socklen_t addr_len = addr.getAddressSize();
	#else
		int addr_len = addr.getAddressSize();
	#endif

	int handle = ::accept(sock, addr.getAddress(), &addr_len);

	if (handle < 0)
		throw SocketException("failed to accept connection");

	return new SocketImplementation(handle);
}

void SocketImplementation::bindTo(const SocketAddress& bindpoint) {
	if (bind(sock, bindpoint.getAddress(), bindpoint.getAddressSize())) {
		StringBuffer msg;
		msg << "unable to bind to socket " << bindpoint.getPort();

		throw SocketException(msg.toString());
	}
}

void SocketImplementation::connect(const SocketAddress& address) {
	if (::connect(sock, address.getAddress(), address.getAddressSize())) {
		StringBuffer msg;
		msg << "unable to connect to socket " << address.getPort();

		throw SocketException(msg.toString());
	}
}

void SocketImplementation::listen(int maxconn) {
	::listen(sock, maxconn);
}

bool SocketImplementation::read(Packet* pack) {
	int len = recv(sock, pack->getBuffer(), Packet::RAW_MAX_SIZE, 0);
	if (len < 0) {
		StringBuffer msg;
		msg << "error reading from socket";

		throw SocketException(msg.toString());
	} else if (len == 0)
		return false;

	pack->setSize(len);
	pack->reset();

	return true;
}

void SocketImplementation::readFrom(Packet* pack, SocketAddress* addr) {
	#ifndef PLATFORM_WIN
		socklen_t addr_len = addr->getAddressSize();
	#else
		int addr_len = addr->getAddressSize();
	#endif

	addr->clear();

	int len = recvfrom(sock, pack->getBuffer(), Packet::RAW_MAX_SIZE, 0, addr->getAddress(), &addr_len);
	if (len < 0) {
		StringBuffer msg;
		msg << "error reading from socket";

		throw SocketException(msg.toString());
	}

	pack->setSize(len);
	pack->reset();
}

void SocketImplementation::send(Packet* pack) {
	int res = ::send(sock, pack->getBuffer(), pack->size(), 0);

	if (res < 0/* && errno != EAGAIN*/) {
		StringBuffer msg;
		msg << "unable to send data to socket";

		throw SocketException(msg.toString());
	}
}

void SocketImplementation::sendTo(Packet* pack, const SocketAddress& addr) {
	int res = sendto(sock, pack->getBuffer(), pack->size(), 0, addr.getAddress(), addr.getAddressSize());

	if (res < 0/* && errno != EAGAIN*/) {
		StringBuffer msg;
		msg << "unable to send data to socket";

		throw SocketException(msg.toString());
	}
}

void SocketImplementation::close() {
	#ifndef PLATFORM_WIN
		::close(sock);
	#else
		closesocket(sock);
	#endif
}

bool SocketImplementation::hasData() {
	fd_set readSet;

	FD_ZERO(&readSet);
	FD_SET(sock, &readSet);

	updateTimeOut();

	if (select(sock + 1, &readSet, NULL, NULL, &tv) < 0) {
		StringBuffer msg;
		msg << "select error";

		throw SocketException(msg.toString());
	}

	return FD_ISSET(sock, &readSet) != 0;
}

void SocketImplementation::setBlocking(bool status) {
	#ifndef PLATFORM_WIN
		unsigned long mode = !status;

		int ret = ioctl(sock, FIONBIO, &mode );
		if (ret < 0)
			throw SocketException("unable to set blocking mode");
	#endif
}

void SocketImplementation::setLingering(int time) {
	int result;

	struct linger linger;

	if (time > 0)
		linger.l_onoff = 0;
	else
		linger.l_onoff = 1;

	linger.l_linger = time;

	result = setsockopt(sock, SOL_SOCKET, SO_LINGER, (char*) &linger, sizeof(linger));

	if (result != 0)
		throw SocketException("unable to set linger options");
}

void SocketImplementation::disableLingering() {
	setLingering(0);
}

void SocketImplementation::updateTimeOut() {
	tv.tv_sec = (uint32) timeout / 1000;
	tv.tv_usec = (uint32) (timeout % 1000) * 1000;
}

SocketImplementationInitializer::SocketImplementationInitializer() {
	initialize();
}

void SocketImplementationInitializer::initialize() {
	#ifdef PLATFORM_WIN
		if (!initialized) {
			WSADATA WsaDat;
			if (WSAStartup(MAKEWORD(1, 1), &WsaDat) != 0)
				throw SocketException("WSA Initialization failed.");
			else
				initialized = true;
		}
	#endif
}
