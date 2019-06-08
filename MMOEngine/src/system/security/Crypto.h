/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#ifndef CRYPTO_H_
#define CRYPTO_H_

#include "system/platform.h"

#include <openssl/sha.h>
#include <openssl/md5.h>

namespace sys {
	namespace security {
		class Crypto {
		public:

			static String hashToString(unsigned char* val, int size) {
				StringBuffer sb;
				char hex[3];

				for(int i = 0; i < size; i++) {
					sprintf(hex, "%02x", val[i]);
					sb << hex;
				}

				return sb.toString();
			}

			static String MD5Hash(const String& str) {
				unsigned char res[MD5_DIGEST_LENGTH];
				MD5(reinterpret_cast<const unsigned char *>(str.toCharArray()), str.length(), res);

				return hashToString(res, MD5_DIGEST_LENGTH);
			}

			static String SHA1Hash(const String& str) {
				unsigned char res[SHA_DIGEST_LENGTH];

				SHA1(reinterpret_cast<const unsigned char *>(str.toCharArray()), str.length(), res);

				return hashToString(res, SHA_DIGEST_LENGTH);
			}

			static String SHA256Hash(const String& str) {
				unsigned char res[SHA256_DIGEST_LENGTH];

				SHA256(reinterpret_cast<const unsigned char *>(str.toCharArray()), str.length(), res);

				return hashToString(res, SHA256_DIGEST_LENGTH);
			}

			static String SHA512Hash(const String& str) {
				unsigned char res[SHA512_DIGEST_LENGTH];

				SHA512(reinterpret_cast<const unsigned char *>(str.toCharArray()), str.length(), res);

				return hashToString(res, SHA512_DIGEST_LENGTH);
			}

			static String randomSalt(uint8 length = 16) {
				unsigned char buffer[256];

				for(int i = 0; i < length && i < 256; i++) {
					buffer[i] = System::random();
				}

				return hashToString(buffer, length);
			}


		};
	}
}

using namespace sys::security;

#endif /* CRYPTO_H_ */
