/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef SERVICEFILTER_H_
#define SERVICEFILTER_H_

namespace engine {
  namespace service {

	class ServiceFilter {
	public:
		virtual ~ServiceFilter() {
		}

		virtual void messageReceived(ServiceClient* client, Packet* message) = 0;

		virtual void messageSent(ServiceClient* client, Packet* message) = 0;
};

  } // namespace service
} // namespace engine

using namespace engine::service;

#endif /* SERVICEFILTER_H_ */
