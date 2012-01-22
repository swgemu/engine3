#ifndef CRYPTO_H_
#define CRYPTO_H_

#include "../platform.h"

#include <openssl/sha.h>
#include <openssl/md5.h>


namespace sys {
	namespace security {
		class Crypto {
		public:

			static String hashToString(String hash) {
				StringBuffer sb;
				char hex[3];

				for(int i = 0; i < hash.length(); i++) {
					sprintf(hex, "%02x", (unsigned char) hash[i]);
					sb << hex;
				}

				return sb.toString();
			}

			static String MD5Hash(const String& str) {
				String hash = (char *) MD5(reinterpret_cast<const unsigned char *>(str.toCharArray()), str.length(), NULL);

				return hashToString(hash);
			}

			static String SHA1Hash(const String& str) {
				String hash = (char *) SHA1(reinterpret_cast<const unsigned char *>(str.toCharArray()), str.length(), NULL);

				return hashToString(hash);
			}

			static String SHA256Hash(const String& str) {
				String hash = (char *) SHA256(reinterpret_cast<const unsigned char *>(str.toCharArray()), str.length(), NULL);

				return hashToString(hash);
			}

			static String SHA512Hash(const String& str) {
				String hash = (char *) SHA512(reinterpret_cast<const unsigned char *>(str.toCharArray()), str.length(), NULL);

				return hashToString(hash);
			}

			static String randomSalt(uint8 length = 16) {
				static char buffer[256];

				for(int i = 0; i < length; i++) {
					buffer[i] = System::random();
				}

				buffer[length] = 0;

				return hashToString(String(buffer, length));
			}


		};
	}
}

using namespace sys::security;

#endif /* CRYPTO_H_ */
