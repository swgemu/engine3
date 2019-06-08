/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef NETWORKINTERFACE_H_
#define NETWORKINTERFACE_H_

#include "system/util/Vector.h"

#include "InetAddress.h"

namespace sys {
  namespace net {

	class NetworkInterface {
		Vector<InetAddress*>* interfaces;

		static NetworkInterface* sinterface;

	public:
		NetworkInterface();
		
		virtual ~NetworkInterface();

		static void finalize();
		
		void initialize();
		
		void close();
		
		static void setPrimaryAddress(InetAddress* address);

		static InetAddress* getPrimaryAddress();

	private:
		void sortInterfaces();

	public:
		//getters
		Vector<InetAddress*>* getInterfaces() {
			return interfaces;
		}

	};

  } // namespace net
} // namespace sys

using namespace sys::net;

#endif /*NETWORKINTERFACE_H_*/
