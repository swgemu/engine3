/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ZIPCOMPRESSIONFILTER_H_
#define ZIPCOMPRESSIONFILTER_H_

#include "system/lang.h"

#include "engine/service/ServiceFilter.h"

namespace engine {
  namespace proto {

	class ZipCompressionFilter : public ServiceFilter {
		static const int COMPRESSION_BUFFER_MAX = 800;

	public:
		ZipCompressionFilter() {
		}

		void messageSent(ServiceSession* session, Packet* message) {
			compress(message);
		}

		void messageReceived(ServiceSession* session, Packet* message) {
			decompress(message);
		}

	protected:
		void decompress(Packet* pack);

		bool compress(Packet* pack);

	};

  } // namespace proto
} // namespace engine

using namespace engine::proto;

#endif /* ZIPCOMPRESSIONFILTER_H_ */
