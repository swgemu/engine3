/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef SERVICECLIENT_H_
#define SERVICECLIENT_H_

#include "system/lang.h"

namespace engine {
  namespace service {

  	class ServiceHandler;

	class ServiceClient : public Object {
	protected:
		ServiceHandler* serviceHandler;

		SocketAddress addr;
		Socket* socket;

		bool errored, disconnected;

		int packetLossChance;

	public:
		ServiceClient();
		ServiceClient(Socket* sock);
		ServiceClient(Socket* sock, SocketAddress& addr);
		ServiceClient(const String& host, int port);

		virtual ~ServiceClient();

		void close();

		virtual int send(Packet* packet) = 0;

		bool isAvailable();

		inline bool isDisconnected() {
			return disconnected;
		}

		inline bool hasError() {
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
		inline uint64 getNetworkID() {
			return addr.getNetworkID();
		}

		inline SocketAddress& getAddress() {
			return addr;
		}

		inline Socket* getSocket() {
			return socket;
		}
	};

  } // namespace service
} // namespace engine

using namespace engine::service;

#endif /*SERVICECLIENT_H_*/
