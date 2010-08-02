/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef SERVICECLIENT_H_
#define SERVICECLIENT_H_

#include "system/lang.h"

#include "ServiceHandler.h"

namespace engine {
  namespace service {

	class ServiceFilterChain;

	class ServiceSession : public virtual Object {
	protected:
		SocketImplementation* socket;

		SocketAddress address;

		ServiceFilterChain* filterChain;

		ServiceHandler* handler;

		bool hasError, disconnected;

		int packetLossChance;

	public:
		ServiceSession();

		virtual ~ServiceSession();

		void bind(SocketImplementation* sock, const SocketAddress& addr);

		virtual bool send(Packet* packet) = 0;

		bool isAvailable();

		inline bool isDisconnected() {
			return disconnected;
		}

		virtual void finalize();

		virtual void acquire();

		virtual void release();

		// getters
		SocketImplementation* getSocket() {
			return socket;
		}

		virtual uint64 getNetworkID() {
			return address.getNetworkID();
		}

		const SocketAddress& getAddress() {
			return address;
		}

	    String getIPAddress() {
	    	return address.getIPAddress();
	    }

	    ServiceFilterChain* getFilterChain() {
	    	return filterChain;
	    }

	    ServiceHandler* getHandler() {
	    	return handler;
	    }

		// setters
	    inline void setFilterChain(ServiceFilterChain* chain) {
	    	filterChain = chain;
	    }

		inline void setHandler(ServiceHandler* hand) {
			handler = hand;
		}

		inline void setError() {
			hasError = true;
		}

		inline void setPacketLoss(int ratio) {
			packetLossChance = ratio;
		}
	};

  } // namespace service
} // namespace engine

using namespace engine::service;

#endif /*SERVICECLIENT_H_*/
