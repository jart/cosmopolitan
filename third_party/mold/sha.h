// clang-format off
#pragma once

#include "third_party/libcxx/cstdint"

typedef uint8_t u8;
static constexpr int64_t SHA256_SIZE = 32;

#ifdef _WIN32
// On Windows, we use Microsoft CNG.

// MISSING #include <Windows.h>
// MISSING #include <bcrypt.h>
// MISSING #include <ntstatus.h>

inline static BCRYPT_ALG_HANDLE get_sha256_handle() {
  static std::once_flag once;
  static BCRYPT_ALG_HANDLE alg;

  std::call_once(once, [&] {
    BCryptOpenAlgorithmProvider(&alg, BCRYPT_SHA256_ALGORITHM, nullptr, 0);
  });
  return alg;
}

inline void sha256_hash(u8 *in, size_t len, u8 *out) {
  BCryptHash(get_sha256_handle(), nullptr, 0, in, len, out, SHA256_SIZE);
}

class SHA256Hash {
public:
  SHA256Hash() {
    BCryptCreateHash(get_sha256_handle(), &handle, nullptr, 0, nullptr, 0, 0);
  }

  void update(u8 *data, size_t len) {
    BCryptHashData(handle, data, len, 0);
  }

  void finish(u8 *out) {
    BCryptFinishHash(handle, out, SHA256_SIZE, 0);
  }

private:
  BCRYPT_HASH_HANDLE handle;
};

#else
// On Unix, we use OpenSSL or the Apple's OpenSSL-compatible API.

#ifdef __APPLE__
#  define COMMON_DIGEST_FOR_OPENSSL
// MISSING #include <CommonCrypto/CommonDigest.h>
#  define SHA256(data, len, md) CC_SHA256(data, len, md)
#else
#  define OPENSSL_SUPPRESS_DEPRECATED 1
// MISSING #include <openssl/sha.h>
#endif

inline void sha256_hash(u8 *in, size_t len, u8 *out) {
  SHA256(in, len, out);
}

class SHA256Hash {
public:
  SHA256Hash() {
    SHA256_Init(&ctx);
  }

  void update(u8 *data, size_t len) {
    SHA256_Update(&ctx, data, len);
  }

  void finish(u8 *out) {
    SHA256_Final(out, &ctx);
  }

private:
  SHA256_CTX ctx;
};

#endif
