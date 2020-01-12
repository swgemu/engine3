/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "Crypto.h"

#include <openssl/rand.h>
#include <openssl/err.h>

#include <openssl/sha.h>
#include <openssl/md5.h>

#include "engine/log/Logger.h"
#include "system/thread/Mutex.h"

namespace CryptoDetail {
	static const engine::log::Logger logger("Crypto");
	static Mutex cryptoMutex;
}

using namespace CryptoDetail;

String Crypto::hashToString(uint8* val, std::size_t size) {
	StringBuffer sb;
	char hex[3];

	for (std::size_t i = 0; i < size; i++) {
		snprintf(hex, sizeof(hex), "%02x", val[i]);
		sb << hex;
	}

	return sb.toString();
}

String Crypto::MD5Hash(const String& str) {
	unsigned char res[MD5_DIGEST_LENGTH];
	MD5(reinterpret_cast<const unsigned char*>(str.toCharArray()), str.length(), res);

	return hashToString(res, MD5_DIGEST_LENGTH);
}

String Crypto::SHA1Hash(const String& str) {
	unsigned char res[SHA_DIGEST_LENGTH];

	SHA1(reinterpret_cast<const unsigned char*>(str.toCharArray()), str.length(), res);

	return hashToString(res, SHA_DIGEST_LENGTH);
}

String Crypto::SHA256Hash(const String& str) {
	unsigned char res[SHA256_DIGEST_LENGTH];

	SHA256(reinterpret_cast<const unsigned char*>(str.toCharArray()), str.length(), res);

	return hashToString(res, SHA256_DIGEST_LENGTH);
}

String Crypto::SHA512Hash(const String& str) {
	unsigned char res[SHA512_DIGEST_LENGTH];

	SHA512(reinterpret_cast<const unsigned char*>(str.toCharArray()), str.length(), res);

	return hashToString(res, SHA512_DIGEST_LENGTH);
}

uint64 Crypto::randomOpenSSLBytes(uint8* bytes, std::size_t size) {
	Locker guard(&cryptoMutex);

	int rc = RAND_bytes(bytes, size);
	auto err = ERR_get_error();

	if (rc != 1) {
		/* RAND_bytes failed */
		/* `err` is valid    */

		onOpenSSLRandomFail(err);

		return err;
	} else {
		return 0;
	}
}

void Crypto::onOpenSSLRandomFail(uint64 errorCode) {
	logger.warning() << "OpenSSL could not generate random bytes with code: " << errorCode;
}