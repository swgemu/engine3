/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef IOEVENT_H_
#define IOEVENT_H_

#include <sys/epoll.h>

namespace sys {
  namespace io {

  	class IOEvent {
  		unsigned int events;

  	public:
  		IOEvent(unsigned int evnts) {
  			events = events;
  		}

  		bool hasInEvent() const {
  			return events && EPOLLIN;
  		}

  		bool hasOutEvent() const {
  			return events && EPOLLOUT;
  		}

  		bool hasHangupEvent() const {
  			return events && EPOLLHUP;
  		}

  		bool hasErrorEvent() const {
  			return events && EPOLLERR;
  		}

  	};

  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /* IOEVENT_H_ */
