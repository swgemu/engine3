/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef SERVICECLIENT_H_
#define SERVICECLIENT_H_

#include "../../system/lang.h"

namespace engine {
  namespace service {

	class ServiceClient : public Object {
	protected:
		SocketAddress addr;
		Socket* socket;

		bool hasError, disconnected;

		int packetLossChance;
		
	public:
		ServiceClient(Socket* sock);
		ServiceClient(Socket* sock, SocketAddress& addr);
		ServiceClient(const String& host, int port);

		virtual ~ServiceClient();

		void close();
		
		bool isAvailable();

		inline bool isDisconnected() {
			return disconnected;
		}

		virtual void finalize();
		
		virtual void acquire();
		
		virtual void release();

		// setters
		inline void setAddress(const String& host, int port) {
			addr = SocketAddress(host, port);
		}

		inline void setError() {
			hasError = true;
		}

		inline void setPacketLoss(int ratio) {
			packetLossChance = ratio;
		}

		// getters
		inline uint64 getNetworkID() {
			return addr.getNetworkID();
		}
		
		inline SocketAddress& getAddress()
		{
			return addr;
		}
	};

  } // namespace service
} // namespace engine

using namespace engine::service;

#endif /*SERVICECLIENT_H_*/
