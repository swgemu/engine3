/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "Socket.h"

#ifndef PLATFORM_WIN
#include <sys/socket.h>
#include <sys/ioctl.h>
#else
#include <winsock2.h>
#endif

#include "Packet.h"

#include "SocketException.h"

#ifdef PLATFORM_WIN
bool Socket::winsockInitialized = false;
#endif

Socket::Socket() {
	setTimeOut(10);
}

Socket::Socket(int handle) {
	fileDescriptor = handle;
		
	setTimeOut(10);
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

void Socket::bindTo(SocketAddress* bindpoint) {
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

void Socket::connect(SocketAddress* address) {
	if (::connect(fileDescriptor, address->getAddress(), address->getAddressSize())) {
		StringBuffer msg;
		msg << "unable to connect to socket " << address->getPort();

		throw SocketException(msg.toString());
	}
}

bool Socket::recieve(Packet* pack) {
	if (hasData()) {
		read(pack);

		return true;
	}

	return false;		
}

bool Socket::recieveFrom(Packet* pack, SocketAddress* addr) {
	if (hasData()) {
		readFrom(pack, addr);
			
		return true;
	}

	return false;		
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

	int len = recvfrom(fileDescriptor, pack->getBuffer(), Packet::RAW_MAX_SIZE, 0, addr->getAddress(), &addr_len);
	if (len < 0) {
		if (errno == EAGAIN)
			return 0;

		StringBuffer msg;
		msg << "error reading from socket";
			
		throw SocketException(msg.toString());
	}
			
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

int Socket::sendTo(Packet* pack, SocketAddress* addr) {
#ifndef PLATFORM_MAC
	int res = sendto(fileDescriptor, pack->getBuffer(), pack->size(), MSG_NOSIGNAL, addr->getAddress(), addr->getAddressSize());
#else
	int res = sendto(fileDescriptor, pack->getBuffer(), pack->size(), SO_NOSIGPIPE, addr->getAddress(), addr->getAddressSize());
#endif
		
	if (res < 0/* && errno != EAGAIN*/) {
		StringBuffer msg;
		msg << "unable to send data to socket";

		throw SocketException(msg.toString());
	}

	return res;
}
	
void Socket::close() {
	#ifndef PLATFORM_WIN
		::close(fileDescriptor);
	#else
		closesocket(fileDescriptor);
	#endif
}

bool Socket::hasData() {
	fd_set readSet;

	FD_ZERO(&readSet);
	FD_SET(fileDescriptor, &readSet);

	updateTimeOut();
	
	if (select(fileDescriptor + 1, &readSet, NULL, NULL, &tv) < 0) {
		StringBuffer msg;
		msg << "select error";

		throw SocketException(msg.toString());
	}

	return FD_ISSET(fileDescriptor, &readSet) != 0;
}

void Socket::setBlocking(bool b) {
	#ifndef PLATFORM_WIN
		unsigned long mode = !b;

		int ret = ioctl(fileDescriptor, FIONBIO, &mode );
		if (ret < 0)
			throw SocketException("unable to set blocking mode");					
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

void Socket::updateTimeOut() {
	tv.tv_sec = (uint32) timeout / 1000;
	tv.tv_usec = (uint32) (timeout % 1000) * 1000;
}
