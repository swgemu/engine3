/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "SocketException.h"

#include "SocketAddress.h"

SocketAddress::SocketAddress() {
	clear();
}

SocketAddress::SocketAddress(int port) {
	addr.sin_family = AF_INET;	
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
}

SocketAddress::SocketAddress(const string& host, int port) {
#ifndef PLATFORM_WIN
	struct hostent *hp = gethostbyname(host.c_str());
	
	if (hp == 0)
		throw SocketException("unknown host " + host);

	bcopy(hp->h_addr, &addr.sin_addr, hp->h_length);
#else
	HOSTENT *hp = gethostbyname(host.c_str());
	
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
	
string SocketAddress::getIPAddress() {
	return string(inet_ntoa(addr.sin_addr));
}

uint16 SocketAddress::getPort() {
	return htons(addr.sin_port);
}
    
string SocketAddress::getFullIPAddress() {
	stringstream ip;
	ip << inet_ntoa(addr.sin_addr) << ":" << htons(addr.sin_port);
        
	return ip.str();
}

string SocketAddress::getFullPrintableIPAddress() {
	stringstream ip;
	ip << inet_ntoa(addr.sin_addr) << "_" << htons(addr.sin_port);
        
	return ip.str();
}

uint64 SocketAddress::getNetworkID() {
	return ((uint64) (addr.sin_addr.s_addr) << 32) | addr.sin_port;
}

uint32 SocketAddress::getIPID() {
	return addr.sin_addr.s_addr;
}
