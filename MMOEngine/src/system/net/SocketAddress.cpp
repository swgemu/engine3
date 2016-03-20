/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "SocketException.h"

#include "SocketAddress.h"

#include "engine/log/Logger.h"

SocketAddress::SocketAddress() {
	clear();
}

SocketAddress::SocketAddress(int port) {
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
}

SocketAddress::SocketAddress(const String& host, int port) {
#ifndef PLATFORM_WIN
	/*struct hostent* hp = 0;

	do
	{
		hp = gethostbyname(host.toCharArray());
	} while (!hp && h_errno == TRY_AGAIN);

	if (!hp)
		throw SocketException("unknown host " + host + " (herrno " + h_errno  + ") ");

	bcopy(hp->h_addr, &addr.sin_addr, hp->h_length);*/

	struct addrinfo *result = NULL;

	int error = getaddrinfo(host.toCharArray(), NULL, NULL, &result);

	if (error != 0 || !result) {
		Logger::console.error("getaddrinfo failed");

		addr.sin_family = 0;

		if (result)
			freeaddrinfo(result);

		throw SocketException("unknown host " + host + " (error: " + String::valueOf(error)  + ") ");
	}

	//TODO fix this shit cause IPv6 ai_addrlen is 28 bytes vs sizeof(addr) that is 16
	uint32 moveSize = MIN(result->ai_addrlen, sizeof(addr));

	memmove(&addr, result->ai_addr, moveSize);

	freeaddrinfo(result);
#else
	HOSTENT *hp = gethostbyname(host.toCharArray());

	if (hp == NULL)
		throw SocketException("unknown host " + host);

	addr.sin_addr.S_un.S_un_b.s_b1 = (unsigned char)hp->h_addr_list[0][0];
	addr.sin_addr.S_un.S_un_b.s_b2 = (unsigned char)hp->h_addr_list[0][1];
	addr.sin_addr.S_un.S_un_b.s_b3 = (unsigned char)hp->h_addr_list[0][2];
	addr.sin_addr.S_un.S_un_b.s_b4 = (unsigned char)hp->h_addr_list[0][3];
#endif

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
}

SocketAddress::SocketAddress(const SocketAddress& add) {
	memcpy(&addr, &add.addr, sizeof(addr));
}

SocketAddress& SocketAddress::operator=(const SocketAddress& add) {
	if (this == &add)
		return *this;

	memcpy(&addr, &add.addr, sizeof(addr));

	return *this;
}

void SocketAddress::clear() {
	memset(&addr, 0, sizeof(addr));
}

int SocketAddress::compareTo(SocketAddress* socka) {
	if (addr.sin_addr.s_addr < socka->addr.sin_addr.s_addr) {
		return 1;
	} else if (addr.sin_addr.s_addr > socka->addr.sin_addr.s_addr) {
		return -1;
	} else {
		if (addr.sin_port < socka->addr.sin_port)
			return 1;
		else if (addr.sin_port > socka->addr.sin_port)
			return -1;
		else
			return 0;
	}
}

struct sockaddr* SocketAddress::getAddress() {
	return (struct sockaddr*) &addr;
}

String SocketAddress::getIPAddress() {
	return String(inet_ntoa(addr.sin_addr));
}

uint16 SocketAddress::getPort() {
	return htons(addr.sin_port);
}

String SocketAddress::getFullIPAddress() {
	StringBuffer ip;
	ip << inet_ntoa(addr.sin_addr) << ":" << htons(addr.sin_port);

	return ip.toString();
}

String SocketAddress::getFullPrintableIPAddress() {
	StringBuffer ip;
	ip << inet_ntoa(addr.sin_addr) << "_" << htons(addr.sin_port);

	return ip.toString();
}

uint64 SocketAddress::getNetworkID() {
	return ((uint64) (addr.sin_addr.s_addr) << 32) | addr.sin_port;
}

uint32 SocketAddress::getIPID() {
	return addr.sin_addr.s_addr;
}
