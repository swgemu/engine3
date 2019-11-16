/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef SERVICECLIENT_H_
#define SERVICECLIENT_H_

#include "system/lang.h"

namespace engine {
  namespace service {

  	class ServiceHandler;

	class ServiceClient : public Object {
	protected:
		ServiceHandler* serviceHandler = nullptr;

		SocketAddress addr;
		Socket* socket = nullptr;

		bool errored = false, disconnected = false;

		int packetLossChance = 0;

	public:
		ServiceClient();
		ServiceClient(Socket* sock);
		ServiceClient(Socket* sock, const SocketAddress& addr);
		ServiceClient(const String& host, int port);

		virtual ~ServiceClient();

		void close();

		virtual int send(Packet* packet) = 0;

		bool isAvailable();

		inline bool isDisconnected() const {
			return disconnected;
		}

		inline bool hasError() const {
			return errored;
		}

		virtual void finalize();

		virtual void acquire();

		virtual void release();

		// setters
		void setHandler(ServiceHandler* handler) {
			serviceHandler = handler;
		}

		inline void setAddress(const String& host, int port) {
			addr = SocketAddress(host, port);
		}

		inline void setError() {
			errored = true;
		}

		inline void setPacketLoss(int ratio) {
			packetLossChance = ratio;
		}

		// getters
		inline uint64 getNetworkID() const {
			return addr.getNetworkID();
		}

		inline SocketAddress& getAddress() {
			return addr;
		}

		inline const SocketAddress& getAddress() const {
			return addr;
		}

		inline Socket* getSocket() {
			return socket;
		}

		inline const Socket* getSocket() const {
			return socket;
		}
	};

  } // namespace service
} // namespace engine

using namespace engine::service;

#endif /*SERVICECLIENT_H_*/
