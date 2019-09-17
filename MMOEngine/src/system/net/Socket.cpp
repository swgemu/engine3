/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "Socket.h"

#ifndef PLATFORM_WIN
#else
#include <winsock2.h>
#endif

#include "Packet.h"

#include "SocketException.h"

#ifdef PLATFORM_WIN
bool Socket::winsockInitialized = false;
#endif

Socket::Socket() {
}

Socket::Socket(int handle) {
	fileDescriptor = handle;
}

void Socket::initialize() {
	#ifdef PLATFORM_WIN
		if (!winsockInitialized) {
			WSADATA WsaDat;
			if (WSAStartup(MAKEWORD(1, 1), &WsaDat) != 0)
				throw SocketException("WSA Initialization failed.");
			else
				winsockInitialized = true;
		}
	#endif
}

void Socket::bindTo(const SocketAddress* bindpoint) {
	if (bind(fileDescriptor, bindpoint->getAddress(), bindpoint->getAddressSize())) {
		StringBuffer msg;
		msg << "unable to bind to socket " << bindpoint->getPort();

		throw SocketException(msg.toString());
	}
}

void Socket::listen(int maxconn) {
	::listen(fileDescriptor, maxconn);
}

Socket* Socket::accept() {
	int handle = ::accept(fileDescriptor, 0, 0);

	if (handle < 0)
		throw SocketException("failed to accept connection");

	return new Socket(handle);
}

Socket* Socket::accept(SocketAddress* addr) {
	#ifndef PLATFORM_WIN
		socklen_t addr_len = addr->getAddressSize();
	#else
		int addr_len = addr->getAddressSize();
	#endif

	int handle = ::accept(fileDescriptor, addr->getAddress(), &addr_len);

	if (handle < 0)
		throw SocketException("failed to accept connection");

	return new Socket(handle);
}

void Socket::connect(const SocketAddress* address) {
	if (::connect(fileDescriptor, address->getAddress(), address->getAddressSize())) {
		StringBuffer msg;
		msg << "unable to connect to socket " << address->getPort();

		throw SocketException(msg.toString());
	}
}

bool Socket::readAppend(Stream* pack) {
	int offset = pack->getOffset();
	int size = pack->size();

	if (size - offset < Packet::RAW_MAX_SIZE) {
		pack->extendSize(Packet::RAW_MAX_SIZE - offset + 1);
	}

	//char buffer[Packet::RAW_MAX_SIZE];

	int len = recv(fileDescriptor, pack->getBuffer() + offset, Packet::RAW_MAX_SIZE, 0);
	if (len < 0) {
		StringBuffer msg;
		msg << "error reading from socket";

		throw SocketException(msg.toString());
	} else if (len == 0)
		return false;

	pack->setSize(len + size);
	pack->reset();
	//printf("received buffer size:%d\n", len);

	//pack->writeStream(buffer, len);
	//pack->reset();

	return true;
}

bool Socket::read(Packet* pack) {
	int len = recv(fileDescriptor, pack->getBuffer(), Packet::RAW_MAX_SIZE, 0);
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

int Socket::readFrom(Packet* pack, SocketAddress* addr) {
	#ifndef PLATFORM_WIN
		socklen_t addr_len = addr->getAddressSize();
	#else
		int addr_len = addr->getAddressSize();
	#endif

	addr->clear();

	int len = -1;
	do {
		len = recvfrom(fileDescriptor, pack->getBuffer(), Packet::RAW_MAX_SIZE, 0, addr->getAddress(), &addr_len);
	} while (len < 0 && (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK));

	if (len < 0)
		throw SocketException("error reading from socket");

	pack->setSize(len);
	pack->reset();

	return len;
}

int Socket::send(Packet* pack) {
#ifndef PLATFORM_MAC
	int res = ::send(fileDescriptor, pack->getBuffer(), pack->size(), MSG_NOSIGNAL);
#else
	int res = ::send(fileDescriptor, pack->getBuffer(), pack->size(), SO_NOSIGPIPE);
#endif

	if (res < 0/* && errno != EAGAIN*/) {
		StringBuffer msg;
		msg << "unable to send data to socket";

		throw SocketException(msg.toString());
	}

	return res;
}

int Socket::sendTo(Packet* pack, const SocketAddress* addr) {
	int len = -1;

	do {
#ifndef PLATFORM_MAC
		len = sendto(fileDescriptor, pack->getBuffer(), pack->size(), MSG_NOSIGNAL, addr->getAddress(), addr->getAddressSize());
#else
		len = sendto(fileDescriptor, pack->getBuffer(), pack->size(), SO_NOSIGPIPE, addr->getAddress(), addr->getAddressSize());
#endif
	} while (len < 0 && (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK));

	if (len < 0)
		throw SocketException("unable to send data to socket");

	return len;
}

void Socket::close() {
	#ifndef PLATFORM_WIN
		::close(fileDescriptor);
	#else
		closesocket(fileDescriptor);
	#endif
}

int Socket::shutdown(int how) {
#ifndef PLATFORM_WIN
	return ::shutdown(fileDescriptor, how);
#else
	return shutdown(fileDescriptor, how);
#endif
}

void Socket::setLingering(int time) {
	int result;

	struct linger linger;

	if (time > 0)
		linger.l_onoff = 0;
	else
		linger.l_onoff = 1;

	linger.l_linger = time;

	result = setsockopt(fileDescriptor, SOL_SOCKET, SO_LINGER, (char*) &linger, sizeof(linger));

	if (result != 0)
		throw SocketException("unable to set linger options");
}

void Socket::disableLingering() {
	setLingering(0);
}
