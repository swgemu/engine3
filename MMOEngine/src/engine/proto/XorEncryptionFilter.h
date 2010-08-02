/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef XORENCRYPTIONFILTER_H_
#define XORENCRYPTIONFILTER_H_

#include "engine/service/ServiceFilter.h"

namespace engine {
  namespace proto {

	class XorEncryptionFilter : public ServiceFilter {
	protected:
		unsigned int crcSeed;

	public:
		XorEncryptionFilter() {
			crcSeed = 0;
		}

		void messageSent(ServiceSession* session, Packet* message) {
			encrypt(message);
		}

		void messageReceived(ServiceSession* session, Packet* message) {
			decrypt(message);
		}

	protected:
		void encrypt(Packet* message) {
			char* pData = message->getBuffer();
			int nLength = message->size();

			unsigned nCrcSeed = crcSeed;

			unsigned int *data;

			if (pData[0] == 0x00) {
				nLength -= 4;
				data = (unsigned int*) (pData+2);
			} else {
				nLength -= 3;
				data = (unsigned int*) (pData+1);
			}

			short block_count = (nLength / 4);
			short byte_count = (nLength % 4);
			unsigned int itemp;

			for (short count = 0; count< block_count; count++) {
				*data ^= nCrcSeed;
				nCrcSeed = *data;
				data++;
			}

			pData = (char*) data;
			for (short count = 0; count < byte_count; count++) {
				*pData ^= (char) nCrcSeed;
				pData++;
			}
		}

		void decrypt(Packet* message) {
			char* pData = message->getBuffer();
			int nLength = message->size();

			unsigned int nCrcSeed = crcSeed;
		    unsigned int *data;

		    if (pData[0] == 0x00) {
				nLength -= 4;
		    	data = (unsigned int*) (pData+2);
		    } else {
		     	nLength -= 3;
		     	data = (unsigned int*) (pData+1);
		    }

		    short block_count = (nLength / 4);
		    short byte_count = (nLength % 4);
		    unsigned int itemp;

		    for (short count = 0; count < block_count; count++) {
		    	itemp = *data;
		    	*data ^= nCrcSeed;
		    	nCrcSeed = itemp;
		    	data++;
		    }

		    pData = (char*) data;
		    for (short count = 0; count < byte_count; count++) {
		      	*pData ^= nCrcSeed;
		       	pData++;
		    }
		}


		void setSeed(sys::uint32 seed) {
			crcSeed = seed;
		}
	};

  } // namespace proto
} // namespace engine

using namespace engine::proto;

#endif /* XORENCRYPTIONFILTER_H_ */
