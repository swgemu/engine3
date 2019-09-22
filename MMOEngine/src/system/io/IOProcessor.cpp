/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "system/platform.h"

#ifdef PLATFORM_LINUX
#include <sys/epoll.h>
#endif

#include "IOHandler.h"
#include "IOException.h"

#include "IOProcessor.h"

IOProcessor::IOProcessor() {
#ifdef PLATFORM_LINUX
	epollFileDescritptor = -1;

	epollQueueLength = 0;
#endif
}

IOProcessor::~IOProcessor() {
	shutdown();
}

void IOProcessor::initialize(int queueLength) {
#ifdef PLATFORM_LINUX
	E3_ASSERT(epollFileDescritptor == -1);

	epollQueueLength = queueLength;

	epollFileDescritptor = epoll_create(epollQueueLength);
	if (epollFileDescritptor < 0)
		throw IOException("epoll create failed");
#endif
}

void IOProcessor::shutdown() {
#ifdef PLATFORM_LINUX
	if (epollFileDescritptor != -1)
		close(epollFileDescritptor);
#endif
}

int IOProcessor::pollEvents(int timeout) {
#ifdef PLATFORM_LINUX
	if (epollFileDescritptor < 0)
		throw IOException("epoll not initialized for processing events");

	struct epoll_event events[epollQueueLength];

	int count = -1;
	do {
		count = epoll_wait(epollFileDescritptor, events, epollQueueLength, timeout);
	} while (count < 0 && errno == EINTR);

	if (count < 0)
		throw IOException("epoll_wait failed");

	for (int i = 0; i < count; i++) {
		FileDescriptor* descriptor = (FileDescriptor*) (events[i].data.ptr);

		IOHandler* handler = descriptor->getHandler();
		if (!handler)
			throw IOException("file descriptor does not have io handler");

		uint eventSet = events[i].events;
		if (eventSet & EPOLLIN)
			handler->handleInput(descriptor);

		if (eventSet & EPOLLOUT)
			handler->handleOutput(descriptor);

  		if (eventSet & EPOLLHUP)
  			handler->handleHangup(descriptor);

  		if (eventSet & EPOLLERR)
  			handler->handleError(descriptor);
	}

	return count;

#else
	fd_set readSet;

	FD_ZERO(&readSet);
	int maxfd = 0;

	for (int i = 0; i < descriptors.size(); ++i) {
		FileDescriptor* descriptor = descriptors.get(i);

		int fd = descriptor->getFileDescriptor();
		FD_SET(fd, &readSet);

		if (fd > maxfd)
			maxfd = fd;
	}

	struct timeval tv;
	tv.tv_sec = (uint32) timeout / 1000;
	tv.tv_usec = (uint32) (timeout % 1000) * 1000;

	if (select(maxfd + 1, &readSet, nullptr, nullptr, &tv) < 0) {
		StringBuffer msg;
		msg << "select error";

		throw IOException(msg.toString());
	}

	int count = 0;

	for (int i = 0; i < descriptors.size(); i++) {
		FileDescriptor* descriptor = descriptors.get(i);

		IOHandler* handler = descriptor->getHandler();
		if (!handler)
			throw IOException("file descriptor does not have io handler");

		int fd = descriptor->getFileDescriptor();
		if (FD_ISSET(fd, &readSet) != 0) {
			handler->handleInput(descriptor);

			count++;
		}
	}

	return count;
#endif
}

void IOProcessor::addFileDescriptor(FileDescriptor* descriptor, bool edgeTriggered) {
#ifdef PLATFORM_LINUX
	int fileDescriptor = descriptor->getFileDescriptor();

	struct epoll_event ev;
	if (edgeTriggered)
		ev.events = EPOLLIN | /*EPOLLOUT |*/ EPOLLERR | EPOLLHUP | EPOLLET;
	else
		ev.events = EPOLLIN | /*EPOLLOUT |*/ EPOLLERR | EPOLLHUP;
	ev.data.fd = fileDescriptor;
	ev.data.ptr = descriptor;

	int res = epoll_ctl(epollFileDescritptor, EPOLL_CTL_ADD, fileDescriptor, &ev);
	if (res < 0)
		throw IOException("epoll file descriptor \'" + String::valueOf(fileDescriptor)
						+ "\' add failed");
#else
	descriptors.add(descriptor);
#endif
}

void IOProcessor::removeFileDescriptor(FileDescriptor* descriptor) {
#ifdef PLATFORM_LINUX
	int res = epoll_ctl(epollFileDescritptor, EPOLL_CTL_ADD, descriptor->getFileDescriptor(), nullptr);
	if (res < 0)
		throw IOException("epoll file descriptor removal failed");
#else
	descriptors.removeElement(descriptor);
#endif
}
