/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include <sys/epoll.h>
#include <errno.h>

#include "IOException.h"

#include "IOProcessor.h"

IOProcessor::IOProcessor() {
	epollFileDescritptor = -1;

	epollQueueLength = 0;
}

IOProcessor::~IOProcessor() {
	shutdown();
}

void IOProcessor::initialize(int queueLength) {
	assert(epollFileDescritptor == -1);

	epollQueueLength = queueLength;

	epollFileDescritptor = epoll_create(epollQueueLength);
	if (epollFileDescritptor < 0)
		throw IOException("epoll create failed");

	epollQueueLength = queueLength;
}

void IOProcessor::shutdown() {
	if (epollFileDescritptor != -1)
		close(epollFileDescritptor);
}

void IOProcessor::pollEvents(int timeout) {
	if (epollFileDescritptor < 0)
		throw IOException("epoll not initialized for processing events");

	struct epoll_event events[epollQueueLength];

	int fileDescriptorCount = -1;
	do {
		fileDescriptorCount = epoll_wait(epollFileDescritptor, events, epollQueueLength, timeout);
	} while (fileDescriptorCount < 0 && errno == EINTR);

	if (fileDescriptorCount < 0)
		throw IOException("epoll_wait failed");

	for (int i = 0; i < fileDescriptorCount; i++) {
		FileDescriptor* fileDescriptor = (FileDescriptor*) (events[i].data.ptr);
		IOEvent ioEvent(events[i].events);

		fileDescriptor->handleEvent(ioEvent);
	}
}

IOEvent IOProcessor::getEvents(FileDescriptor* descriptor, int timeout) {
	if (epollFileDescritptor < 0)
		throw IOException("epoll not initialized for processing events");

	struct epoll_event events[epollQueueLength];

	int fileDescriptorCount = -1;
	do {
		fileDescriptorCount = epoll_wait(epollFileDescritptor, events, epollQueueLength, timeout);
	} while (fileDescriptorCount < 0 && errno == EINTR);

	if (fileDescriptorCount < 0)
		throw IOException("epoll_wait failed");

	for (int i = 0; i < fileDescriptorCount; i++) {
		FileDescriptor* fileDescriptor = (FileDescriptor*) (events[i].data.ptr);
		if (descriptor == fileDescriptor) {
			return IOEvent(events[i].events);
		}
	}

	return IOEvent();
}

void IOProcessor::addFileDescriptor(FileDescriptor* descriptor, bool edgeTriggered) {
	int fileDescriptor = descriptor->getFileDescriptor();

	struct epoll_event ev;
	if (edgeTriggered)
		ev.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP | EPOLLET;
	else
		ev.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP;
	ev.data.fd = fileDescriptor;
	ev.data.ptr = descriptor;

	int res = epoll_ctl(epollFileDescritptor, EPOLL_CTL_ADD, fileDescriptor, &ev);
	if (res < 0)
		throw IOException("epoll file descriptor \'" + String::valueOf(fileDescriptor)
						+ "\' add failed");
}

void IOProcessor::removeFileDescriptor(FileDescriptor* descriptor) {
	int res = epoll_ctl(epollFileDescritptor, EPOLL_CTL_ADD, descriptor->getFileDescriptor(), NULL);
	if (res < 0)
		throw IOException("epoll file descriptor removal failed");
}
