/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2025 Miguel Angel Terron                                           │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/

// lrsa.c - RSA key generation, encryption, decryption, signing, and
// verification for redbean

#include "libc/log/log.h"
#include "net/https/https.h"
#include "third_party/lua/lauxlib.h"
#include "third_party/mbedtls/error.h"
#include "third_party/mbedtls/pk.h"
#include "third_party/mbedtls/rsa.h"

static bool GenerateKeyPair(char **private_key_pem, size_t *private_key_len,
                            char **public_key_pem, size_t *public_key_len,
                            unsigned int key_length) {
  int rc;
  mbedtls_pk_context key;
  mbedtls_pk_init(&key);

  // Initialize as RSA key
  if ((rc = mbedtls_pk_setup(&key,
                             mbedtls_pk_info_from_type(MBEDTLS_PK_RSA))) != 0) {
    WARNF("Failed to setup key (grep -0x%04x)", -rc);
    mbedtls_pk_free(&key);
    return false;
  }

  // Generate RSA key
  if ((rc = mbedtls_rsa_gen_key(mbedtls_pk_rsa(key), GenerateHardRandom, 0,
                                key_length, 65537)) != 0) {
    WARNF("Failed to generate key (grep -0x%04x)", -rc);
    mbedtls_pk_free(&key);
    return false;
  }

  // Write private key to PEM
  *private_key_len = 16000;  // Buffer size for private key
  *private_key_pem = calloc(1, *private_key_len);
  if ((rc = mbedtls_pk_write_key_pem(&key, (unsigned char *)*private_key_pem,
                                     *private_key_len)) != 0) {
    WARNF("Failed to write private key (grep -0x%04x)", -rc);
    free(*private_key_pem);
    mbedtls_pk_free(&key);
    return false;
  }
  *private_key_len = strlen(*private_key_pem);

  // Write public key to PEM
  *public_key_len = 8000;  // Buffer size for public key
  *public_key_pem = calloc(1, *public_key_len);
  if ((rc = mbedtls_pk_write_pubkey_pem(&key, (unsigned char *)*public_key_pem,
                                        *public_key_len)) != 0) {
    WARNF("Failed to write public key (grep -0x%04x)", -rc);
    free(*private_key_pem);
    free(*public_key_pem);
    mbedtls_pk_free(&key);
    return false;
  }
  *public_key_len = strlen(*public_key_pem);

  mbedtls_pk_free(&key);
  return true;
}
/**
 * Lua wrapper for RSA key pair generation
 *
 * Lua function signature: GenerateKeyPair([key_length])
 * @param L Lua state
 * @return 2 on success (private_key, public_key), 2 on failure (nil,
 * error_message)
 */
static int LuaGenerateKeyPair(lua_State *L) {
  char *private_key, *public_key;
  size_t private_len, public_len;
  int key_length = 2048;  // Default RSA key length

  // Get key length from Lua (optional parameter)
  if (lua_gettop(L) >= 1 && !lua_isnil(L, 1)) {
    key_length = luaL_checkinteger(L, 1);
    // Validate key length (common RSA key lengths are 1024, 2048, 3072, 4096)
    if (key_length != 1024 && key_length != 2048 && key_length != 3072 &&
        key_length != 4096) {
      lua_pushnil(L);
      lua_pushstring(L,
                     "Invalid RSA key length. Use 1024, 2048, 3072, or 4096.");
      return 2;
    }
  }

  // Call the C function to generate the key pair
  if (!GenerateKeyPair(&private_key, &private_len, &public_key, &public_len,
                       key_length)) {
    lua_pushnil(L);
    lua_pushstring(L, "Failed to generate RSA key pair");
    return 2;
  }

  // Push results to Lua
  lua_pushstring(L, private_key);
  lua_pushstring(L, public_key);

  // Clean up
  free(private_key);
  free(public_key);

  return 2;
}

// RSA
static char *Encrypt(const char *public_key_pem, const unsigned char *data,
                     size_t data_len, size_t *out_len) {
  int rc;

  // Parse public key
  mbedtls_pk_context key;
  mbedtls_pk_init(&key);
  if ((rc = mbedtls_pk_parse_public_key(&key,
                                        (const unsigned char *)public_key_pem,
                                        strlen(public_key_pem) + 1)) != 0) {
    WARNF("Failed to parse public key (grep -0x%04x)", -rc);
    mbedtls_pk_free(&key);
    return NULL;
  }

  // Check if key is RSA
  if (mbedtls_pk_get_type(&key) != MBEDTLS_PK_RSA) {
    WARNF("Key is not an RSA key");
    mbedtls_pk_free(&key);
    return NULL;
  }

  // Allocate output buffer
  size_t key_size = mbedtls_pk_get_len(&key);
  unsigned char *output = calloc(1, key_size);
  if (!output) {
    mbedtls_pk_free(&key);
    return NULL;
  }

  // Encrypt data
  if ((rc = mbedtls_rsa_pkcs1_encrypt(mbedtls_pk_rsa(key), GenerateHardRandom,
                                      0, MBEDTLS_RSA_PUBLIC, data_len, data,
                                      output)) != 0) {
    WARNF("Encryption failed (grep -0x%04x)", -rc);
    free(output);
    mbedtls_pk_free(&key);
    return NULL;
  }

  *out_len = key_size;
  mbedtls_pk_free(&key);
  return (char *)output;
}
static int LuaEncrypt(lua_State *L) {
  const char *public_key = luaL_checkstring(L, 1);
  size_t data_len;
  const unsigned char *data =
      (const unsigned char *)luaL_checklstring(L, 2, &data_len);
  size_t out_len;

  char *encrypted = Encrypt(public_key, data, data_len, &out_len);
  if (!encrypted) {
    lua_pushnil(L);
    lua_pushstring(L, "Encryption failed");
    return 2;
  }

  lua_pushlstring(L, encrypted, out_len);
  free(encrypted);

  return 1;
}

static char *Decrypt(const char *private_key_pem,
                     const unsigned char *encrypted_data, size_t encrypted_len,
                     size_t *out_len) {
  int rc;

  // Parse private key
  mbedtls_pk_context key;
  mbedtls_pk_init(&key);
  if ((rc = mbedtls_pk_parse_key(&key, (const unsigned char *)private_key_pem,
                                 strlen(private_key_pem) + 1, NULL, 0)) != 0) {
    WARNF("Failed to parse private key (grep -0x%04x)", -rc);
    mbedtls_pk_free(&key);
    return NULL;
  }

  // Check if key is RSA
  if (mbedtls_pk_get_type(&key) != MBEDTLS_PK_RSA) {
    WARNF("Key is not an RSA key");
    mbedtls_pk_free(&key);
    return NULL;
  }

  // Allocate output buffer
  size_t key_size = mbedtls_pk_get_len(&key);
  unsigned char *output = calloc(1, key_size);
  if (!output) {
    mbedtls_pk_free(&key);
    return NULL;
  }

  // Decrypt data
  size_t output_len = 0;
  if ((rc = mbedtls_rsa_pkcs1_decrypt(mbedtls_pk_rsa(key), GenerateHardRandom,
                                      0, MBEDTLS_RSA_PRIVATE, &output_len,
                                      encrypted_data, output, key_size)) != 0) {
    WARNF("Decryption failed (grep -0x%04x)", -rc);
    free(output);
    mbedtls_pk_free(&key);
    return NULL;
  }

  *out_len = output_len;
  mbedtls_pk_free(&key);
  return (char *)output;
}
static int LuaDecrypt(lua_State *L) {
  const char *private_key = luaL_checkstring(L, 1);
  size_t encrypted_len;
  const unsigned char *encrypted_data =
      (const unsigned char *)luaL_checklstring(L, 2, &encrypted_len);
  size_t out_len;

  char *decrypted =
      Decrypt(private_key, encrypted_data, encrypted_len, &out_len);
  if (!decrypted) {
    lua_pushnil(L);
    lua_pushstring(L, "Decryption failed");
    return 2;
  }

  lua_pushlstring(L, decrypted, out_len);
  free(decrypted);

  return 1;
}

// RSA Signing
static char *Sign(const char *private_key_pem, const unsigned char *data,
                  size_t data_len, const char *hash_algo_str, size_t *sig_len) {
  int rc;
  unsigned char hash[64];  // Large enough for SHA-512
  size_t hash_len = 32;    // Default for SHA-256
  unsigned char *signature;
  mbedtls_md_type_t hash_algo = MBEDTLS_MD_SHA256;  // Default

  // Determine hash algorithm
  if (hash_algo_str) {
    if (strcasecmp(hash_algo_str, "sha256") == 0) {
      hash_algo = MBEDTLS_MD_SHA256;
      hash_len = 32;
    } else if (strcasecmp(hash_algo_str, "sha384") == 0) {
      hash_algo = MBEDTLS_MD_SHA384;
      hash_len = 48;
    } else if (strcasecmp(hash_algo_str, "sha512") == 0) {
      hash_algo = MBEDTLS_MD_SHA512;
      hash_len = 64;
    } else {
      return NULL;  // Unsupported hash algorithm
    }
  }

  // Parse private key
  mbedtls_pk_context key;
  mbedtls_pk_init(&key);
  if ((rc = mbedtls_pk_parse_key(&key, (const unsigned char *)private_key_pem,
                                 strlen(private_key_pem) + 1, NULL, 0)) != 0) {
    WARNF("Failed to parse private key (grep -0x%04x)", -rc);
    mbedtls_pk_free(&key);
    return NULL;
  }

  // Check if key is RSA
  if (mbedtls_pk_get_type(&key) != MBEDTLS_PK_RSA) {
    WARNF("Key is not an RSA key");
    mbedtls_pk_free(&key);
    return NULL;
  }

  // Hash the message
  if ((rc = mbedtls_md(mbedtls_md_info_from_type(hash_algo), data, data_len,
                       hash)) != 0) {
    mbedtls_pk_free(&key);
    return NULL;
  }

  // Allocate buffer for signature
  signature = malloc(MBEDTLS_PK_SIGNATURE_MAX_SIZE);
  if (!signature) {
    mbedtls_pk_free(&key);
    return NULL;
  }

  // Sign the hash
  if ((rc = mbedtls_pk_sign(&key, hash_algo, hash, hash_len, signature, sig_len,
                            GenerateHardRandom, 0)) != 0) {
    free(signature);
    mbedtls_pk_free(&key);
    return NULL;
  }

  // Clean up
  mbedtls_pk_free(&key);

  return (char *)signature;
}
static int LuaSign(lua_State *L) {
  size_t msg_len, key_len;
  const char *msg, *key_pem, *hash_algo_str = NULL;
  unsigned char *signature;
  size_t sig_len = 0;

  // Get parameters from Lua
  key_pem = luaL_checklstring(L, 1, &key_len);
  msg = luaL_checklstring(L, 2, &msg_len);

  // Optional hash algorithm parameter
  if (!lua_isnoneornil(L, 3)) {
    hash_algo_str = luaL_checkstring(L, 3);
  }

  // Call the C implementation
  signature = (unsigned char *)Sign(key_pem, (const unsigned char *)msg,
                                    msg_len, hash_algo_str, &sig_len);

  if (!signature) {
    return luaL_error(L, "failed to sign message");
  }

  // Return the signature as a Lua string
  lua_pushlstring(L, (char *)signature, sig_len);

  // Clean up
  free(signature);

  return 1;
}

static int Verify(const char *public_key_pem, const unsigned char *data,
                  size_t data_len, const unsigned char *signature,
                  size_t sig_len, const char *hash_algo_str) {
  int rc;
  unsigned char hash[64];                           // Large enough for SHA-512
  size_t hash_len = 32;                             // Default for SHA-256
  mbedtls_md_type_t hash_algo = MBEDTLS_MD_SHA256;  // Default

  // Determine hash algorithm
  if (hash_algo_str) {
    if (strcasecmp(hash_algo_str, "sha256") == 0) {
      hash_algo = MBEDTLS_MD_SHA256;
      hash_len = 32;
    } else if (strcasecmp(hash_algo_str, "sha384") == 0) {
      hash_algo = MBEDTLS_MD_SHA384;
      hash_len = 48;
    } else if (strcasecmp(hash_algo_str, "sha512") == 0) {
      hash_algo = MBEDTLS_MD_SHA512;
      hash_len = 64;
    } else {
      return -1;  // Unsupported hash algorithm
    }
  }

  // Parse public key
  mbedtls_pk_context key;
  mbedtls_pk_init(&key);
  if ((rc = mbedtls_pk_parse_public_key(&key,
                                        (const unsigned char *)public_key_pem,
                                        strlen(public_key_pem) + 1)) != 0) {
    WARNF("Failed to parse public key (grep -0x%04x)", -rc);
    mbedtls_pk_free(&key);
    return -1;
  }

  // Check if key is RSA
  if (mbedtls_pk_get_type(&key) != MBEDTLS_PK_RSA) {
    WARNF("Key is not an RSA key");
    mbedtls_pk_free(&key);
    return -1;
  }

  // Hash the message
  if ((rc = mbedtls_md(mbedtls_md_info_from_type(hash_algo), data, data_len,
                       hash)) != 0) {
    mbedtls_pk_free(&key);
    return -1;
  }

  // Verify the signature
  rc = mbedtls_pk_verify(&key, hash_algo, hash, hash_len, signature, sig_len);

  // Clean up
  mbedtls_pk_free(&key);

  return rc;  // 0 means success (valid signature)
}
static int LuaVerify(lua_State *L) {
  size_t msg_len, key_len, sig_len;
  const char *msg, *key_pem, *signature, *hash_algo_str = NULL;
  int result;

  // Get parameters from Lua
  key_pem = luaL_checklstring(L, 1, &key_len);
  msg = luaL_checklstring(L, 2, &msg_len);
  signature = luaL_checklstring(L, 3, &sig_len);

  // Optional hash algorithm parameter
  if (!lua_isnoneornil(L, 4)) {
    hash_algo_str = luaL_checkstring(L, 4);
  }

  // Call the C implementation
  result = Verify(key_pem, (const unsigned char *)msg, msg_len,
                  (const unsigned char *)signature, sig_len, hash_algo_str);

  // Return boolean result (0 means valid signature)
  lua_pushboolean(L, result == 0);

  return 1;
}

static const luaL_Reg kLuaRSA[] = {
    {"GenerateKeyPair", LuaGenerateKeyPair},  //
    {"Sign", LuaSign},                        //
    {"Verify", LuaVerify},                    //
    {"Encrypt", LuaEncrypt},                  //
    {"Decrypt", LuaDecrypt},                  //
    {0},                                      //
};

int LuaRSA(lua_State *L) {
  luaL_newlib(L, kLuaRSA);
  return 1;
}
