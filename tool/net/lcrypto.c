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

#include "tool/net/luacheck.h"
// mbedTLS
#include "third_party/mbedtls/aes.h"
#include "third_party/mbedtls/base64.h"
#include "third_party/mbedtls/ctr_drbg.h"
#include "third_party/mbedtls/ecdsa.h"
#include "third_party/mbedtls/entropy.h"
#include "third_party/mbedtls/error.h"
#include "third_party/mbedtls/gcm.h"
#include "third_party/mbedtls/md.h"
#include "third_party/mbedtls/oid.h"
#include "third_party/mbedtls/pk.h"
#include "third_party/mbedtls/rsa.h"
#include "third_party/mbedtls/x509_csr.h"

// Supported curves mapping
typedef struct {
  const char *name;
  mbedtls_ecp_group_id id;
} curve_map_t;

static const curve_map_t supported_curves[] = {
    {"secp256r1", MBEDTLS_ECP_DP_SECP256R1},
    {"P256", MBEDTLS_ECP_DP_SECP256R1},
    {"P-256", MBEDTLS_ECP_DP_SECP256R1},
    {"secp384r1", MBEDTLS_ECP_DP_SECP384R1},
    {"P384", MBEDTLS_ECP_DP_SECP384R1},
    {"P-384", MBEDTLS_ECP_DP_SECP384R1},
    {"secp521r1", MBEDTLS_ECP_DP_SECP521R1},
    {"P521", MBEDTLS_ECP_DP_SECP521R1},
    {"P-521", MBEDTLS_ECP_DP_SECP521R1},
    {"curve25519", MBEDTLS_ECP_DP_CURVE25519},
    {NULL, 0}};

// List available curves
static int LuaListCurves(lua_State *L) {
  const curve_map_t *curve = supported_curves;
  int i = 1;

  lua_newtable(L);

  while (curve->name != NULL) {
    lua_pushstring(L, curve->name);
    lua_rawseti(L, -2, i++);
    curve++;
  }

  return 1;
}

// Remove hash_algorithm_t and hash_to_md_type indirection
static mbedtls_md_type_t string_to_md_type(const char *hash_name) {
  if (!hash_name || !*hash_name) {
    return MBEDTLS_MD_SHA256;  // Default to SHA-256 if no name provided
  }
  if (strcasecmp(hash_name, "sha256") == 0 ||
      strcasecmp(hash_name, "sha-256") == 0) {
    return MBEDTLS_MD_SHA256;
  } else if (strcasecmp(hash_name, "sha384") == 0 ||
             strcasecmp(hash_name, "sha-384") == 0) {
    return MBEDTLS_MD_SHA384;
  } else if (strcasecmp(hash_name, "sha512") == 0 ||
             strcasecmp(hash_name, "sha-512") == 0) {
    return MBEDTLS_MD_SHA512;
  } else {
    WARNF("(crypto) Unknown hash algorithm '%s'", hash_name);
    return -1;
  }
}

// Get the size of the hash output based on the mbedtls_md_type_t
static size_t get_hash_size_from_md_type(mbedtls_md_type_t md_type) {
  switch (md_type) {
    case MBEDTLS_MD_SHA256:
      return 32;
    case MBEDTLS_MD_SHA384:
      return 48;
    case MBEDTLS_MD_SHA512:
      return 64;
    default:
      return 32;
  }
}

// List available hash algorithms
static int LuaListHashAlgorithms(lua_State *L) {
  lua_newtable(L);

  lua_pushstring(L, "SHA256");
  lua_rawseti(L, -2, 1);

  lua_pushstring(L, "SHA384");
  lua_rawseti(L, -2, 2);

  lua_pushstring(L, "SHA512");
  lua_rawseti(L, -2, 3);

  // Add hyphenated versions
  lua_pushstring(L, "SHA-256");
  lua_rawseti(L, -2, 4);

  lua_pushstring(L, "SHA-384");
  lua_rawseti(L, -2, 5);

  lua_pushstring(L, "SHA-512");
  lua_rawseti(L, -2, 6);

  lua_pushstring(L, "SHA1");
  lua_rawseti(L, -2, 7);

  lua_pushstring(L, "MD5");
  lua_rawseti(L, -2, 8);

  return 1;
}

static int compute_hash(mbedtls_md_type_t md_type, const unsigned char *input,
                        size_t input_len, unsigned char *output,
                        size_t output_size) {
  mbedtls_md_context_t md_ctx;
  const mbedtls_md_info_t *md_info;
  int ret;

  md_info = mbedtls_md_info_from_type(md_type);
  if (md_info == NULL) {
    WARNF("(crypto) Unsupported hash algorithm");
    return -1;
  }

  if (output_size < mbedtls_md_get_size(md_info)) {
    WARNF("(crypto) Output buffer too small for hash");
    return -1;
  }

  mbedtls_md_init(&md_ctx);

  ret = mbedtls_md_setup(&md_ctx, md_info, 0);  // 0 = non-HMAC
  if (ret != 0) {
    WARNF("(crypto) Failed to set up hash context: -0x%04x", -ret);
    goto cleanup;
  }

  ret = mbedtls_md_starts(&md_ctx);
  if (ret != 0) {
    WARNF("(crypto) Failed to start hash operation: -0x%04x", -ret);
    goto cleanup;
  }

  ret = mbedtls_md_update(&md_ctx, input, input_len);
  if (ret != 0) {
    WARNF("(crypto) Failed to update hash: -0x%04x", -ret);
    goto cleanup;
  }

  ret = mbedtls_md_finish(&md_ctx, output);
  if (ret != 0) {
    WARNF("(crypto) Failed to finish hash: -0x%04x", -ret);
    goto cleanup;
  }

cleanup:
  mbedtls_md_free(&md_ctx);
  return ret;
}

// Find curve ID by name
static mbedtls_ecp_group_id find_curve_by_name(const char *name) {
  const curve_map_t *curve = supported_curves;

  while (curve->name != NULL) {
    if (strcasecmp(curve->name, name) == 0) {
      return curve->id;
    }
    curve++;
  }

  return MBEDTLS_ECP_DP_NONE;
}

// Strong RNG using mbedtls_entropy_context and mbedtls_ctr_drbg_context
int GenerateRandom(void *ctx, unsigned char *output, size_t len) {
  static mbedtls_entropy_context entropy;
  static mbedtls_ctr_drbg_context ctr_drbg;
  static int initialized = 0;
  int ret;
  const char *pers = "redbean_entropy";

  if (!initialized) {
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);

    ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                (const unsigned char *)pers, strlen(pers));
    if (ret != 0) {
      // Clean up on failure
      mbedtls_ctr_drbg_free(&ctr_drbg);
      mbedtls_entropy_free(&entropy);
      return -1;
    }
    initialized = 1;
  }
  // mbedtls_ctr_drbg_random returns 0 on success
  ret = mbedtls_ctr_drbg_random(&ctr_drbg, output, len);
  if (ret != 0) {
    // If DRBG fails, reinitialize on next call
    initialized = 0;
    return -1;
  }
  return 0;
}

// RSA

// Generate RSA Key Pair
static bool RSAGenerateKeyPair(char **private_key_pem, size_t *private_key_len,
                               char **public_key_pem, size_t *public_key_len,
                               unsigned int key_length) {
  int rc;
  mbedtls_pk_context key;
  mbedtls_pk_init(&key);

  // Initialize as RSA key
  if ((rc = mbedtls_pk_setup(&key,
                             mbedtls_pk_info_from_type(MBEDTLS_PK_RSA))) != 0) {
    WARNF("(crypto) Failed to setup key (grep -0x%04x)", -rc);
    mbedtls_pk_free(&key);
    return false;
  }

  // Generate RSA key
  if ((rc = mbedtls_rsa_gen_key(mbedtls_pk_rsa(key), GenerateRandom, 0,
                                key_length, 65537)) != 0) {
    WARNF("(crypto) Failed to generate key (grep -0x%04x)", -rc);
    mbedtls_pk_free(&key);
    return false;
  }

  // Write private key to PEM
  *private_key_len = 16000;  // Buffer size for private key
  *private_key_pem = calloc(1, *private_key_len);
  if ((rc = mbedtls_pk_write_key_pem(&key, (unsigned char *)*private_key_pem,
                                     *private_key_len)) != 0) {
    WARNF("(crypto) Failed to write private key (grep -0x%04x)", -rc);
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
    WARNF("(crypto) Failed to write public key (grep -0x%04x)", -rc);
    free(*private_key_pem);
    free(*public_key_pem);
    mbedtls_pk_free(&key);
    return false;
  }
  *public_key_len = strlen(*public_key_pem);

  mbedtls_pk_free(&key);
  return true;
}
static int LuaRSAGenerateKeyPair(lua_State *L) {
  int bits = 2048;
  // If no arguments, or first argument is nil, default to 2048
  if (lua_gettop(L) == 0 || lua_isnoneornil(L, 1)) {
    bits = 2048;
  } else if (lua_gettop(L) == 1 && lua_type(L, 1) == LUA_TNUMBER) {
    bits = (int)lua_tointeger(L, 1);
  } else {
    bits = (int)luaL_optinteger(L, 2, 2048);
  }
  // Check if key length is valid (only 2048 or 4096 bits are allowed)
  if (bits != 2048 && bits != 4096) {
    lua_pushnil(L);
    lua_pushfstring(L,
                    "Invalid RSA key length: %d. Only 2048 or 4096 bits key "
                    "lengths are supported",
                    bits);
    return 2;
  }

  char *private_key, *public_key;
  size_t private_len, public_len;

  // Call the C function to generate the key pair
  if (!RSAGenerateKeyPair(&private_key, &private_len, &public_key, &public_len,
                          bits)) {
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

// Helper to get string field from options table for RSA
// static const char *parse_rsa_options(lua_State *L, int options_idx) {
//     const char *padding = "pkcs1"; // default
//     if (lua_istable(L, options_idx)) {
//         lua_getfield(L, options_idx, "padding");
//         if (lua_isstring(L, -1)) {
//             padding = lua_tostring(L, -1);
//         }
//         lua_pop(L, 1);
//     }
//     return padding;
// }

static char *RSAEncrypt(const char *public_key_pem, const unsigned char *data,
                        size_t data_len, size_t *out_len) {
  int rc;

  // Parse public key
  mbedtls_pk_context key;
  mbedtls_pk_init(&key);
  if ((rc = mbedtls_pk_parse_public_key(&key,
                                        (const unsigned char *)public_key_pem,
                                        strlen(public_key_pem) + 1)) != 0) {
    WARNF("(crypto) Failed to parse public key (grep -0x%04x)", -rc);
    mbedtls_pk_free(&key);
    return NULL;
  }

  // Check if key is RSA
  if (mbedtls_pk_get_type(&key) != MBEDTLS_PK_RSA) {
    WARNF("(crypto) Key is not an RSA key");
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
  if ((rc = mbedtls_rsa_pkcs1_encrypt(mbedtls_pk_rsa(key), GenerateRandom, 0,
                                      MBEDTLS_RSA_PUBLIC, data_len, data,
                                      output)) != 0) {
    WARNF("(crypto) Encryption failed (grep -0x%04x)", -rc);
    free(output);
    mbedtls_pk_free(&key);
    return NULL;
  }

  *out_len = key_size;
  mbedtls_pk_free(&key);
  return (char *)output;
}
static int LuaRSAEncrypt(lua_State *L) {
  // Args: key, plaintext, options table
  size_t keylen, ptlen;
  // Ensure key is a string
  if (!lua_isstring(L, 1)) {
    lua_pushnil(L);
    lua_pushstring(L, "Key must be a string");
    return 2;
  }
  const char *key = luaL_checklstring(L, 1, &keylen);
  // Ensure plaintext is a string
  if (!lua_isstring(L, 2)) {
    lua_pushnil(L);
    lua_pushstring(L, "Plaintext must be a string");
    return 2;
  }
  const unsigned char *plaintext =
      (const unsigned char *)luaL_checklstring(L, 2, &ptlen);
  // int options_idx = 3;
  // const char *padding = parse_rsa_options(L, options_idx);
  size_t out_len;

  char *encrypted = RSAEncrypt(key, plaintext, ptlen, &out_len);
  if (!encrypted) {
    lua_pushnil(L);
    lua_pushstring(L, "Encryption failed");
    return 2;
  }

  lua_pushlstring(L, encrypted, out_len);
  free(encrypted);

  return 1;
}
static char *RSADecrypt(const char *private_key_pem,
                        const unsigned char *encrypted_data,
                        size_t encrypted_len, size_t *out_len) {
  int rc;

  // Parse private key
  mbedtls_pk_context key;
  mbedtls_pk_init(&key);
  rc = mbedtls_pk_parse_key(&key, (const unsigned char *)private_key_pem,
                            strlen(private_key_pem) + 1, NULL, 0);
  if (rc != 0) {
    WARNF("(crypto) Failed to parse private key (grep -0x%04x)", -rc);
    mbedtls_pk_free(&key);
    return NULL;
  }

  // Check if key is RSA
  if (mbedtls_pk_get_type(&key) != MBEDTLS_PK_RSA) {
    WARNF("(crypto) Key is not an RSA key");
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
  rc = mbedtls_rsa_pkcs1_decrypt(mbedtls_pk_rsa(key), GenerateRandom, 0,
                                 MBEDTLS_RSA_PRIVATE, &output_len,
                                 encrypted_data, output, key_size);
  if (rc != 0) {
    WARNF("(crypto) Decryption failed (grep -0x%04x)", -rc);
    free(output);
    mbedtls_pk_free(&key);
    return NULL;
  }

  *out_len = output_len;
  mbedtls_pk_free(&key);
  return (char *)output;
}
static int LuaRSADecrypt(lua_State *L) {
  // Args: key, ciphertext, options table
  size_t keylen, ctlen;
  // Ensure key is a string
  if (!lua_isstring(L, 1)) {
    lua_pushnil(L);
    lua_pushstring(L, "Key must be a string");
    return 2;
  }
  const char *key = luaL_checklstring(L, 1, &keylen);
  // Ensure ciphertext is a string
  if (!lua_isstring(L, 2)) {
    lua_pushnil(L);
    lua_pushstring(L, "Ciphertext must be a string");
    return 2;
  }
  const unsigned char *ciphertext =
      (const unsigned char *)luaL_checklstring(L, 2, &ctlen);
  // int options_idx = 3;
  // const char *padding = parse_rsa_options(L, options_idx);
  size_t out_len;

  char *decrypted = RSADecrypt(key, ciphertext, ctlen, &out_len);
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
static char *RSASign(const char *private_key_pem, const unsigned char *data,
                     size_t data_len, const char *hash_algo_str,
                     size_t *sig_len) {
  int rc;
  unsigned char hash[64];  // Large enough for SHA-512
  size_t hash_len = 32;    // Default for SHA-256
  unsigned char *signature;
  mbedtls_md_type_t hash_algo = MBEDTLS_MD_SHA256;  // Default

  // Determine hash algorithm
  if (hash_algo_str) {
    hash_algo = string_to_md_type(hash_algo_str);
    hash_len = get_hash_size_from_md_type(hash_algo);
  }

  // Parse private key
  mbedtls_pk_context key;
  mbedtls_pk_init(&key);
  if ((rc = mbedtls_pk_parse_key(&key, (const unsigned char *)private_key_pem,
                                 strlen(private_key_pem) + 1, NULL, 0)) != 0) {
    WARNF("(crypto) Failed to parse private key (grep -0x%04x)", -rc);
    mbedtls_pk_free(&key);
    return NULL;
  }

  // Check if key is RSA
  if (mbedtls_pk_get_type(&key) != MBEDTLS_PK_RSA) {
    WARNF("(crypto) Key is not an RSA key");
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
                            GenerateRandom, 0)) != 0) {
    free(signature);
    mbedtls_pk_free(&key);
    return NULL;
  }

  // Clean up
  mbedtls_pk_free(&key);

  return (char *)signature;
}
static int LuaRSASign(lua_State *L) {
  size_t msg_len, key_len;
  const char *msg, *key_pem, *hash_algo_str = NULL;
  unsigned char *signature;
  size_t sig_len = 0;

  // Get parameters from Lua
  if (lua_type(L, 1) != LUA_TSTRING) {
    lua_pushnil(L);
    lua_pushstring(L, "Key must be a string");
    return 2;
  }
  if (lua_type(L, 1) == LUA_TTABLE) {
    DEBUGF("(crypto) Detected table type for parameter 1");
    lua_pushnil(L);
    lua_pushstring(L, "Key must be a string, got a table instead");
    return 2;
  }
  // Ensure msg is a string
  if (lua_type(L, 2) != LUA_TSTRING) {
    lua_pushnil(L);
    lua_pushstring(L, "Plaintext must be a string");
    return 2;
  }

  key_pem = luaL_checklstring(L, 1, &key_len);
  msg = luaL_checklstring(L, 2, &msg_len);

  // Optional hash algorithm parameter
  if (!lua_isnoneornil(L, 3)) {
    hash_algo_str = luaL_checkstring(L, 3);
  }

  // Call the C implementation
  signature = (unsigned char *)RSASign(key_pem, (const unsigned char *)msg,
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

// RSA PSS Signing
static char *RSAPSSSign(const char *private_key_pem, const unsigned char *data,
                        size_t data_len, const char *hash_algo_str,
                        size_t *sig_len, int salt_len) {
  int rc;
  unsigned char hash[64];  // Large enough for SHA-512
  size_t hash_len = 32;    // Default for SHA-256
  unsigned char *signature;
  mbedtls_md_type_t hash_algo = MBEDTLS_MD_SHA256;  // Default

  // Determine hash algorithm
  if (hash_algo_str) {
    hash_algo = string_to_md_type(hash_algo_str);
    hash_len = get_hash_size_from_md_type(hash_algo);
  }

  // Parse private key
  mbedtls_pk_context key;
  mbedtls_pk_init(&key);
  rc = mbedtls_pk_parse_key(&key, (const unsigned char *)private_key_pem,
                            strlen(private_key_pem) + 1, NULL, 0);
  if (rc != 0) {
    WARNF("(crypto) Failed to parse private key (grep -0x%04x)", -rc);
    mbedtls_pk_free(&key);
    return NULL;
  }

  // Check if key is RSA
  if (mbedtls_pk_get_type(&key) != MBEDTLS_PK_RSA) {
    WARNF("(crypto) Key is not an RSA key");
    mbedtls_pk_free(&key);
    return NULL;
  }

  // Hash the message
  rc = mbedtls_md(mbedtls_md_info_from_type(hash_algo), data, data_len, hash);
  if (rc != 0) {
    mbedtls_pk_free(&key);
    return NULL;
  }

  // Allocate buffer for signature
  signature = malloc(MBEDTLS_PK_SIGNATURE_MAX_SIZE);
  if (!signature) {
    mbedtls_pk_free(&key);
    return NULL;
  }

  // Get RSA context
  mbedtls_rsa_context *rsa = mbedtls_pk_rsa(key);
  mbedtls_rsa_set_padding(rsa, MBEDTLS_RSA_PKCS_V21, hash_algo);

  // Sign the hash using PSS
  rc = mbedtls_rsa_rsassa_pss_sign(rsa, GenerateRandom, 0, MBEDTLS_RSA_PRIVATE,
                                   hash_algo, (unsigned int)hash_len, hash,
                                   signature);
  if (rc != 0) {
    free(signature);
    mbedtls_pk_free(&key);
    return NULL;
  }

  *sig_len = mbedtls_pk_get_len(&key);

  // Clean up
  mbedtls_pk_free(&key);

  return (char *)signature;
}

static int LuaRSAPSSSign(lua_State *L) {
  size_t key_len, msg_len;
  const char *key_pem, *msg;
  unsigned char *signature;
  size_t sig_len = 0;

  // Get parameters from Lua
  if (lua_type(L, 1) != LUA_TSTRING) {
    lua_pushnil(L);
    lua_pushstring(L, "Key must be a string");
    return 2;
  }
  // Ensure msg is a string
  if (lua_type(L, 2) != LUA_TSTRING) {
    lua_pushnil(L);
    lua_pushstring(L, "Plaintext must be a string");
    return 2;
  }

  // Get parameters from Lua
  key_pem = luaL_checklstring(L, 1, &key_len);
  msg = luaL_checklstring(L, 2, &msg_len);

  // Optional hash algorithm parameter
  const char *hash_name = luaL_optstring(L, 3, "sha256");

  // Optional salt length parameter
  int salt_len = luaL_optinteger(L, 4, -1);

  // Call the C implementation
  signature =
      (unsigned char *)RSAPSSSign(key_pem, (const unsigned char *)msg, msg_len,
                                  hash_name, &sig_len, salt_len);

  if (!signature) {
    return luaL_error(L, "failed to sign message (PSS)");
  }

  // Return the signature as a Lua string
  lua_pushlstring(L, (char *)signature, sig_len);
  free(signature);
  return 1;
}

static int RSAVerify(const char *public_key_pem, const unsigned char *data,
                     size_t data_len, const unsigned char *signature,
                     size_t sig_len, const char *hash_algo_str) {
  int rc;
  unsigned char hash[64];                           // Large enough for SHA-512
  size_t hash_len = 32;                             // Default for SHA-256
  mbedtls_md_type_t hash_algo = MBEDTLS_MD_SHA256;  // Default

  // Determine hash algorithm
  if (hash_algo_str) {
    hash_algo = string_to_md_type(hash_algo_str);
    hash_len = get_hash_size_from_md_type(hash_algo);
  }

  // Parse public key
  mbedtls_pk_context key;
  mbedtls_pk_init(&key);
  if ((rc = mbedtls_pk_parse_public_key(&key,
                                        (const unsigned char *)public_key_pem,
                                        strlen(public_key_pem) + 1)) != 0) {
    WARNF("(crypto) Failed to parse public key (grep -0x%04x)", -rc);
    mbedtls_pk_free(&key);
    return -1;
  }

  // Check if key is RSA
  if (mbedtls_pk_get_type(&key) != MBEDTLS_PK_RSA) {
    WARNF("(crypto) Key is not an RSA key");
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
static int LuaRSAVerify(lua_State *L) {
  size_t msg_len, key_len, sig_len;
  const char *msg, *key_pem, *signature, *hash_algo_str = NULL;
  int result;

  // Get parameters from Lua
  if (!lua_isstring(L, 1)) {
    lua_pushnil(L);
    lua_pushstring(L, "Key must be a string");
    return 2;
  }
  // Ensure msg is a string
  if (!lua_isstring(L, 2)) {
    lua_pushnil(L);
    lua_pushstring(L, "Plaintext must be a string");
    return 2;
  }
  key_pem = luaL_checklstring(L, 1, &key_len);
  msg = luaL_checklstring(L, 2, &msg_len);
  signature = luaL_checklstring(L, 3, &sig_len);

  // Optional hash algorithm parameter
  if (!lua_isnoneornil(L, 4)) {
    hash_algo_str = luaL_checkstring(L, 4);
  }

  // Call the C implementation
  result = RSAVerify(key_pem, (const unsigned char *)msg, msg_len,
                     (const unsigned char *)signature, sig_len, hash_algo_str);

  // Return boolean result (0 means valid signature)
  lua_pushboolean(L, result == 0);

  return 1;
}

// RSA PSS Verification
static int RSAPSSVerify(const char *public_key_pem, const unsigned char *data,
                        size_t data_len, const unsigned char *signature,
                        size_t sig_len, const char *hash_algo_str,
                        int expected_salt_len) {
  int rc;
  unsigned char hash[64];                           // Large enough for SHA-512
  size_t hash_len = 32;                             // Default for SHA-256
  mbedtls_md_type_t hash_algo = MBEDTLS_MD_SHA256;  // Default

  // Determine hash algorithm
  if (hash_algo_str) {
    hash_algo = string_to_md_type(hash_algo_str);
    hash_len = get_hash_size_from_md_type(hash_algo);
  }

  // Parse public key
  mbedtls_pk_context key;
  mbedtls_pk_init(&key);
  if ((rc = mbedtls_pk_parse_public_key(&key,
                                        (const unsigned char *)public_key_pem,
                                        strlen(public_key_pem) + 1)) != 0) {
    WARNF("(crypto) Failed to parse public key (grep -0x%04x)", -rc);
    mbedtls_pk_free(&key);
    return -1;
  }

  // Check if key is RSA
  if (mbedtls_pk_get_type(&key) != MBEDTLS_PK_RSA) {
    WARNF("(crypto) Key is not an RSA key");
    mbedtls_pk_free(&key);
    return -1;
  }

  // Hash the message
  if ((rc = mbedtls_md(mbedtls_md_info_from_type(hash_algo), data, data_len,
                       hash)) != 0) {
    mbedtls_pk_free(&key);
    return -1;
  }

  // Get RSA context
  mbedtls_rsa_context *rsa = mbedtls_pk_rsa(key);

  // Verify the signature using PSS
  rc = mbedtls_rsa_rsassa_pss_verify(rsa, NULL, NULL, MBEDTLS_RSA_PUBLIC,
                                     hash_algo, (unsigned int)hash_len, hash,
                                     signature);

  // Clean up
  mbedtls_pk_free(&key);

  return rc;  // 0 means success (valid signature)
}

static int LuaRSAPSSVerify(lua_State *L) {
  // Args: key, msg, signature, hash_algo (optional), salt_len (optional
  // crypto.verify('rsapss', key, msg, signature, hash_algo, salt_len)
  size_t msg_len, key_len, sig_len;
  const char *msg, *key_pem, *signature;
  const char *hash_algo_str = NULL;  // Default to SHA-256
  int expected_salt_len = -1;
  int result;

  // Get parameters from Lua
  if (lua_type(L, 1) != LUA_TSTRING) {
    lua_pushnil(L);
    lua_pushstring(L, "Key must be a string");
    return 2;
  }
  // Ensure msg is a string
  if (lua_type(L, 2) != LUA_TSTRING) {
    lua_pushnil(L);
    lua_pushstring(L, "Plaintext must be a string");
    return 2;
  }
  // Ensure signature is a string
  if (lua_type(L, 3) != LUA_TSTRING) {
    lua_pushnil(L);
    lua_pushstring(L, "Signature must be a string");
    return 2;
  }
  // Get parameters from Lua
  key_pem = luaL_checklstring(L, 1, &key_len);
  msg = luaL_checklstring(L, 2, &msg_len);
  signature = luaL_checklstring(L, 3, &sig_len);
  // Optional hash algorithm parameter
  if (lua_isstring(L, 4)) {
    hash_algo_str = luaL_checkstring(L, 4);
    // Optional salt length parameter
    expected_salt_len = luaL_optinteger(L, 5, 32);
  } else if (lua_isnumber(L, 4)) {
    // If it's a number, treat it as the expected salt length
    expected_salt_len = (int)lua_tointeger(L, 4);
  }
  DEBUGF("(DEBUG) Key PEM: %s", key_pem);
  DEBUGF("(DEBUG) Message length: %zu", msg_len);
  DEBUGF("(DEBUG) Signature length: %zu", sig_len);
  DEBUGF("(DEBUG) Hash algorithm: %s",
         hash_algo_str ? hash_algo_str : "SHA-256");
  DEBUGF("(DEBUG) Expected salt length: %d", expected_salt_len);
  DEBUGF("(DEBUG) Signature: %.*s", (int)sig_len, signature);
  // Call the C implementation
  result = RSAPSSVerify(key_pem, (const unsigned char *)msg, msg_len,
                        (const unsigned char *)signature, sig_len,
                        hash_algo_str, expected_salt_len);

  // Return boolean result (0 means valid signature)
  lua_pushboolean(L, result == 0);

  return 1;
}

// Elliptic Curve Cryptography Functions

// Generate an ECDSA key pair and return in PEM format
static int ECDSAGenerateKeyPair(const char *curve_name, char **priv_key_pem,
                                char **pub_key_pem) {
  mbedtls_pk_context key;
  unsigned char output_buf[16000];
  int ret;
  mbedtls_ecp_group_id curve_id;

  // Initialize output parameters to NULL in case of early return
  if (priv_key_pem)
    *priv_key_pem = NULL;
  if (pub_key_pem)
    *pub_key_pem = NULL;

  // Use secp256r1 as default if curve_name is NULL or empty
  if (curve_name == NULL || curve_name[0] == '\0') {
    curve_id = MBEDTLS_ECP_DP_SECP256R1;
    VERBOSEF("(crypto) No curve specified, using default: secp256r1");
  } else {
    // Find the curve by name
    curve_id = find_curve_by_name(curve_name);
    if (curve_id == MBEDTLS_ECP_DP_NONE) {
      WARNF("(crypto) Unknown curve: '%s'", curve_name);
      return -1;
    }
  }

  mbedtls_pk_init(&key);

  // Generate the key with the specified curve
  ret = mbedtls_pk_setup(&key, mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY));
  if (ret != 0) {
    WARNF("(crypto) Failed to setup key: -0x%04x", -ret);
    goto cleanup;
  }

  ret = mbedtls_ecp_gen_key(curve_id, mbedtls_pk_ec(key), GenerateRandom, 0);
  if (ret != 0) {
    WARNF("(crypto) Failed to generate key: -0x%04x", -ret);
    goto cleanup;
  }

  // Generate private key PEM
  if (priv_key_pem != NULL) {
    memset(output_buf, 0, sizeof(output_buf));
    ret = mbedtls_pk_write_key_pem(&key, output_buf, sizeof(output_buf));
    if (ret != 0) {
      WARNF("(crypto) Failed to write private key: -0x%04x", -ret);
      goto cleanup;
    }
    *priv_key_pem = strdup((char *)output_buf);
    if (*priv_key_pem == NULL) {
      WARNF("(crypto) Failed to allocate memory for private key PEM");
      ret = -1;
      goto cleanup;
    }
  }

  // Generate public key PEM
  if (pub_key_pem != NULL) {
    memset(output_buf, 0, sizeof(output_buf));
    ret = mbedtls_pk_write_pubkey_pem(&key, output_buf, sizeof(output_buf));
    if (ret != 0) {
      WARNF("(crypto) Failed to write public key: -0x%04x", -ret);
      goto cleanup;
    }
    *pub_key_pem = strdup((char *)output_buf);
    if (*pub_key_pem == NULL) {
      WARNF("(crypto) Failed to allocate memory for public key PEM");
      ret = -1;
      goto cleanup;
    }
  }

cleanup:
  mbedtls_pk_free(&key);
  if (ret != 0) {
    // Clean up on error
    if (priv_key_pem && *priv_key_pem) {
      free(*priv_key_pem);
      *priv_key_pem = NULL;
    }
    if (pub_key_pem && *pub_key_pem) {
      free(*pub_key_pem);
      *pub_key_pem = NULL;
    }
  }
  return ret;
}
static int LuaECDSAGenerateKeyPair(lua_State *L) {
  const char *curve_name = NULL;
  char *priv_key_pem = NULL;
  char *pub_key_pem = NULL;

  // Check if curve name is provided
  if (lua_gettop(L) >= 1 && !lua_isnil(L, 1)) {
    curve_name = luaL_checkstring(L, 1);
  }

  int ret = ECDSAGenerateKeyPair(curve_name, &priv_key_pem, &pub_key_pem);

  if (ret == 0) {
    lua_pushstring(L, priv_key_pem);
    lua_pushstring(L, pub_key_pem);
    free(priv_key_pem);
    free(pub_key_pem);
    return 2;
  } else {
    lua_pushnil(L);
    lua_pushnil(L);
    return 2;
  }
}

// Sign a message using an ECDSA private key in PEM format
static int ECDSASign(const char *priv_key_pem, const char *message,
                     mbedtls_md_type_t hash_alg, unsigned char **signature,
                     size_t *sig_len) {
  mbedtls_pk_context key;
  unsigned char hash[64];  // Max hash size (SHA-512)
  size_t hash_size;
  int ret;

  *signature = NULL;
  *sig_len = 0;

  if (!priv_key_pem) {
    WARNF("(crypto) Private key is NULL");
    return -1;
  }

  // Get the length of the PEM string (excluding null terminator)
  size_t key_len = strlen(priv_key_pem);
  if (key_len == 0) {
    WARNF("(crypto) Private key is empty");
    return -1;
  }

  // Get hash size for the selected algorithm
  hash_size = get_hash_size_from_md_type(hash_alg);

  mbedtls_pk_init(&key);

  // Parse the private key from PEM directly without creating a copy
  ret = mbedtls_pk_parse_key(&key, (const unsigned char *)priv_key_pem,
                             key_len + 1, NULL, 0);

  if (ret != 0) {
    WARNF("(crypto) Failed to parse private key: -0x%04x", -ret);
    goto cleanup;
  }

  // Compute hash of the message using the specified algorithm
  ret = compute_hash(hash_alg, (const unsigned char *)message, strlen(message),
                     hash, sizeof(hash));
  if (ret != 0) {
    WARNF("(crypto) Failed to compute message hash");
    goto cleanup;
  }

  // Allocate memory for signature (max size for ECDSA)
  *signature = malloc(MBEDTLS_ECDSA_MAX_LEN);
  if (*signature == NULL) {
    WARNF("(crypto) Failed to allocate memory for signature");
    ret = -1;
    goto cleanup;
  }

  // Sign the hash using GenerateRandom
  ret = mbedtls_pk_sign(&key, hash_alg, hash, hash_size, *signature, sig_len,
                        GenerateRandom, 0);

  if (ret != 0) {
    WARNF("(crypto) Failed to sign message: -0x%04x", -ret);
    free(*signature);
    *signature = NULL;
    *sig_len = 0;
    goto cleanup;
  }

cleanup:
  mbedtls_pk_free(&key);
  return ret;
}  // Lua binding for signing a message
static int LuaECDSASign(lua_State *L) {
  // Correct order: priv_key, message, hash_name (default sha256)
  const char *priv_key_pem = luaL_checkstring(L, 1);
  const char *message = luaL_checkstring(L, 2);
  const char *hash_name = luaL_optstring(L, 3, "sha256");

  mbedtls_md_type_t hash_alg = string_to_md_type(hash_name);

  unsigned char *signature = NULL;
  size_t sig_len = 0;

  int ret = ECDSASign(priv_key_pem, message, hash_alg, &signature, &sig_len);

  if (ret == 0) {
    lua_pushlstring(L, (const char *)signature, sig_len);
    free(signature);
  } else {
    lua_pushnil(L);
  }

  return 1;
}

// Verify a signature using an ECDSA public key in PEM format
static int ECDSAVerify(const char *pub_key_pem, const char *message,
                       const unsigned char *signature, size_t sig_len,
                       mbedtls_md_type_t hash_alg) {
  mbedtls_pk_context key;
  unsigned char hash[64];  // Max hash size (SHA-512)
  size_t hash_size;
  int ret;

  if (!pub_key_pem) {
    WARNF("(crypto) Public key is NULL");
    return -1;
  }

  // Get the length of the PEM string (excluding null terminator)
  size_t key_len = strlen(pub_key_pem);
  if (key_len == 0) {
    WARNF("(crypto) Public key is empty");
    return -1;
  }

  // Get hash size for the selected algorithm
  hash_size = get_hash_size_from_md_type(hash_alg);

  mbedtls_pk_init(&key);

  // Parse the public key from PEM
  ret = mbedtls_pk_parse_public_key(&key, (const unsigned char *)pub_key_pem,
                                    key_len + 1);
  if (ret != 0) {
    WARNF("(crypto) Failed to parse public key: -0x%04x", -ret);
    goto cleanup;
  }

  // Compute hash of the message using the specified algorithm
  ret = compute_hash(hash_alg, (const unsigned char *)message, strlen(message),
                     hash, sizeof(hash));
  if (ret != 0) {
    WARNF("(crypto) Failed to compute message hash");
    goto cleanup;
  }

  // Verify the signature
  ret = mbedtls_pk_verify(&key, hash_alg, hash, hash_size, signature, sig_len);
  if (ret != 0) {
    WARNF("(crypto) Signature verification failed: -0x%04x", -ret);
    goto cleanup;
  }

cleanup:
  mbedtls_pk_free(&key);
  return ret;
}
static int LuaECDSAVerify(lua_State *L) {
  // Correct order: pub_key, message, signature, hash_name (default sha256)
  const char *pub_key_pem = luaL_checkstring(L, 1);
  const char *message = luaL_checkstring(L, 2);
  size_t sig_len;
  const unsigned char *signature =
      (const unsigned char *)luaL_checklstring(L, 3, &sig_len);
  const char *hash_name = luaL_optstring(L, 4, "sha256");

  mbedtls_md_type_t hash_alg = string_to_md_type(hash_name);

  int ret = ECDSAVerify(pub_key_pem, message, signature, sig_len, hash_alg);

  lua_pushboolean(L, ret == 0);
  return 1;
}

// AES

// AES key generation helper
static int LuaAesGenerateKey(lua_State *L) {
  int keybits = 128;
  if (lua_gettop(L) >= 1 && !lua_isnil(L, 1)) {
    keybits = luaL_checkinteger(L, 1);
  }
  int keylen = keybits / 8;
  if ((keybits != 128 && keybits != 192 && keybits != 256) ||
      (keylen != 16 && keylen != 24 && keylen != 32)) {
    lua_pushnil(L);
    lua_pushstring(L, "AES key length must be 128, 192, or 256 bits");
    return 2;
  }
  unsigned char key[32];
  // Generate random key
  if (GenerateRandom(NULL, key, keylen) != 0) {
    lua_pushnil(L);
    lua_pushstring(L, "Failed to generate random key");
    return 2;
  }
  lua_pushlstring(L, (const char *)key, keylen);
  return 1;
}

// Helper to get string field from options table
typedef struct {
  const char *mode;
  const unsigned char *iv;
  size_t ivlen;
  const unsigned char *tag;
  size_t taglen;
  const unsigned char *aad;
  size_t aadlen;
} aes_options_t;

static void parse_aes_options(lua_State *L, int options_idx,
                              aes_options_t *opts) {
  opts->mode = NULL;
  opts->iv = NULL;
  opts->ivlen = 0;
  opts->tag = NULL;
  opts->taglen = 0;
  opts->aad = NULL;
  opts->aadlen = 0;

  int mode_field_found = 0;

  if (lua_istable(L, options_idx)) {
    // Get mode
    lua_getfield(L, options_idx, "mode");
    if (!lua_isnil(L, -1)) {
      mode_field_found = 1;
      const char *mode = lua_tostring(L, -1);
      if (mode &&
          (strcasecmp(mode, "cbc") == 0 || strcasecmp(mode, "gcm") == 0 ||
           strcasecmp(mode, "ctr") == 0)) {
        opts->mode = mode;
      } else {
        opts->mode = NULL;  // Invalid mode
      }
    }
    lua_pop(L, 1);

    // Get IV
    lua_getfield(L, options_idx, "iv");
    if (lua_isstring(L, -1)) {
      size_t ivlen;
      opts->iv = (const unsigned char *)lua_tolstring(L, -1, &ivlen);
      opts->ivlen = ivlen;
    }
    lua_pop(L, 1);

    // Get tag (for GCM)
    lua_getfield(L, options_idx, "tag");
    if (lua_isstring(L, -1)) {
      size_t taglen;
      opts->tag = (const unsigned char *)lua_tolstring(L, -1, &taglen);
      opts->taglen = taglen;
    }
    lua_pop(L, 1);

    // Get aad (for GCM)
    lua_getfield(L, options_idx, "aad");
    if (lua_isstring(L, -1)) {
      size_t aadlen;
      opts->aad = (const unsigned char *)lua_tolstring(L, -1, &aadlen);
      opts->aadlen = aadlen;
    }
    lua_pop(L, 1);
  }

  // Only default to cbc if mode field was not found at all
  if (!mode_field_found) {
    opts->mode = "cbc";
  }
}
// AES encryption supporting CBC, GCM, and CTR modes
static int LuaAesEncrypt(lua_State *L) {
  // Args: key, plaintext, options table
  size_t keylen, ptlen;

  // Get parameters from Lua
  // Ensure key is a string
  if (lua_type(L, 1) != LUA_TSTRING) {
    lua_pushnil(L);
    lua_pushstring(L, "Key must be a string");
    return 2;
  }
  // Ensure plaintext is a string
  if (lua_type(L, 2) != LUA_TSTRING) {
    lua_pushnil(L);
    lua_pushstring(L, "Plaintext must be a string");
    return 2;
  }
  // Ensure options is a table or nil
  if (!lua_istable(L, 3) && !lua_isnil(L, 3)) {
    lua_pushnil(L);
    lua_pushstring(L, "Options must be a table or nil");
    return 2;
  }

  const unsigned char *key =
      (const unsigned char *)luaL_checklstring(L, 1, &keylen);
  const unsigned char *plaintext =
      (const unsigned char *)luaL_checklstring(L, 2, &ptlen);
  int options_idx = 3;
  aes_options_t opts;
  parse_aes_options(L, options_idx, &opts);
  const char *mode = opts.mode;
  if (!mode) {
    lua_pushnil(L);
    lua_pushstring(L,
                   "Invalid AES mode specified. Use 'cbc', 'gcm', or 'ctr'.");
    return 2;
  }
  const unsigned char *iv = opts.iv;
  size_t ivlen = opts.ivlen;
  unsigned char *gen_iv = NULL;
  int iv_was_generated = 0;
  int ret = 0;
  unsigned char *output = NULL;
  int is_gcm = 0, is_ctr = 0, is_cbc = 0;
  if (strcasecmp(mode, "cbc") == 0) {
    is_cbc = 1;
  } else if (strcasecmp(mode, "gcm") == 0) {
    is_gcm = 1;
  } else if (strcasecmp(mode, "ctr") == 0) {
    is_ctr = 1;
  } else {
    lua_pushnil(L);
    lua_pushstring(L, "Unsupported AES mode. Use 'cbc', 'gcm', or 'ctr'.");
    return 2;
  }

  // If IV is not provided, auto-generate
  if (!iv) {
    if (is_gcm) {
      ivlen = 12;
    } else {
      ivlen = 16;
    }
    gen_iv = malloc(ivlen);
    if (!gen_iv) {
      lua_pushnil(L);
      lua_pushstring(L, "Failed to allocate IV");
      return 2;
    }

    // Generate random IV
    if (GenerateRandom(NULL, gen_iv, ivlen) != 0) {
      free(gen_iv);
      lua_pushnil(L);
      lua_pushstring(L, "Failed to generate random IV");
      return 2;
    }
    iv = gen_iv;
    iv_was_generated = 1;
  }

  // Validate IV/nonce length
  if (is_cbc || is_ctr) {
    if (opts.iv && opts.ivlen != 16) {
      if (iv_was_generated)
        free(gen_iv);
      lua_pushnil(L);
      lua_pushstring(L, "AES IV/nonce must be 16 bytes for CBC/CTR");
      return 2;
    }
  } else if (is_gcm) {
    if (opts.iv && (opts.ivlen < 12 || opts.ivlen > 16)) {
      if (iv_was_generated)
        free(gen_iv);
      lua_pushnil(L);
      lua_pushstring(L, "AES GCM nonce must be 12-16 bytes");
      return 2;
    }
  }

  if (is_cbc) {
    // PKCS7 padding
    size_t block_size = 16;
    size_t padlen = block_size - (ptlen % block_size);
    size_t ctlen = ptlen + padlen;
    unsigned char *input = malloc(ctlen);
    if (!input) {
      lua_pushnil(L);
      lua_pushstring(L, "Memory allocation failed");
      return 2;
    }
    memcpy(input, plaintext, ptlen);
    memset(input + ptlen, (unsigned char)padlen, padlen);
    output = malloc(ctlen);
    if (!output) {
      free(input);
      lua_pushnil(L);
      lua_pushstring(L, "Memory allocation failed");
      return 2;
    }
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    ret = mbedtls_aes_setkey_enc(&aes, key, keylen * 8);
    if (ret != 0) {
      free(input);
      free(output);
      mbedtls_aes_free(&aes);
      lua_pushnil(L);
      lua_pushstring(L, "Failed to set AES encryption key");
      return 2;
    }
    unsigned char iv_copy[16];
    memcpy(iv_copy, iv, 16);
    ret = mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT, ctlen, iv_copy,
                                input, output);
    mbedtls_aes_free(&aes);
    free(input);
    if (ret != 0) {
      free(output);
      lua_pushnil(L);
      lua_pushstring(L, "AES CBC encryption failed");
      return 2;
    }
    lua_pushlstring(L, (const char *)output, ctlen);
    lua_pushlstring(L, (const char *)iv, ivlen);
    free(output);
    if (iv_was_generated)
      free(gen_iv);
    return 2;
  } else if (is_ctr) {
    // CTR mode: no padding
    output = malloc(ptlen);
    if (!output) {
      lua_pushnil(L);
      lua_pushstring(L, "Memory allocation failed");
      return 2;
    }
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    ret = mbedtls_aes_setkey_enc(&aes, key, keylen * 8);
    if (ret != 0) {
      free(output);
      mbedtls_aes_free(&aes);
      lua_pushnil(L);
      lua_pushstring(L, "Failed to set AES encryption key");
      return 2;
    }
    unsigned char nonce_counter[16];
    unsigned char stream_block[16];
    size_t nc_off = 0;
    memcpy(nonce_counter, iv, 16);
    memset(stream_block, 0, 16);
    ret = mbedtls_aes_crypt_ctr(&aes, ptlen, &nc_off, nonce_counter,
                                stream_block, plaintext, output);
    mbedtls_aes_free(&aes);
    if (ret != 0) {
      free(output);
      lua_pushnil(L);
      lua_pushstring(L, "AES CTR encryption failed");
      return 2;
    }
    lua_pushlstring(L, (const char *)output, ptlen);
    lua_pushlstring(L, (const char *)iv, ivlen);
    free(output);
    if (iv_was_generated)
      free(gen_iv);
    return 2;
  } else if (is_gcm) {
    // GCM mode: authenticated encryption
    size_t taglen = 16;
    unsigned char tag[16];
    output = malloc(ptlen);
    if (!output) {
      lua_pushnil(L);
      lua_pushstring(L, "Memory allocation failed");
      return 2;
    }
    mbedtls_gcm_context gcm;
    mbedtls_gcm_init(&gcm);
    ret = mbedtls_gcm_setkey(&gcm, MBEDTLS_CIPHER_ID_AES, key, keylen * 8);
    if (ret != 0) {
      free(output);
      mbedtls_gcm_free(&gcm);
      lua_pushnil(L);
      lua_pushstring(L, "Failed to set AES GCM key");
      return 2;
    }
    ret = mbedtls_gcm_crypt_and_tag(&gcm, MBEDTLS_GCM_ENCRYPT, ptlen, iv, ivlen,
                                    NULL, 0, plaintext, output, taglen, tag);
    mbedtls_gcm_free(&gcm);
    if (ret != 0) {
      free(output);
      lua_pushnil(L);
      lua_pushstring(L, "AES GCM encryption failed");
      return 2;
    }
    lua_pushlstring(L, (const char *)output, ptlen);
    lua_pushlstring(L, (const char *)iv, ivlen);
    lua_pushlstring(L, (const char *)tag, taglen);
    free(output);
    if (iv_was_generated)
      free(gen_iv);
    return 3;
  }
  lua_pushnil(L);
  lua_pushstring(L, "Internal error in AES encrypt");
  return 2;
}

// AES decryption supporting CBC, GCM, and CTR modes
static int LuaAesDecrypt(lua_State *L) {
  // Args: key, ciphertext, options table
  size_t keylen, ctlen;
  // Ensure key is a string
  if (lua_type(L, 1) != LUA_TSTRING) {
    lua_pushnil(L);
    lua_pushstring(L, "Key must be a string");
    return 2;
  }

  // Ensure ciphertext is a string
  if (lua_type(L, 2) != LUA_TSTRING) {
    lua_pushnil(L);
    lua_pushstring(L, "Ciphertext must be a string");
    return 2;
  }
  const unsigned char *key =
      (const unsigned char *)luaL_checklstring(L, 1, &keylen);
  const unsigned char *ciphertext =
      (const unsigned char *)luaL_checklstring(L, 2, &ctlen);
  int options_idx = 3;
  aes_options_t opts;
  parse_aes_options(L, options_idx, &opts);
  const char *mode = opts.mode;
  if (!mode) {
    lua_pushnil(L);
    lua_pushstring(L,
                   "Invalid AES mode specified. Use 'cbc', 'gcm', or 'ctr'.");
    return 2;
  }
  const unsigned char *iv = opts.iv;
  size_t ivlen = opts.ivlen;
  const unsigned char *tag = opts.tag;
  size_t taglen = opts.taglen;
  const unsigned char *aad = opts.aad;
  size_t aadlen = opts.aadlen;
  int is_gcm = 0, is_ctr = 0, is_cbc = 0;
  if (strcasecmp(mode, "cbc") == 0) {
    is_cbc = 1;
  } else if (strcasecmp(mode, "gcm") == 0) {
    is_gcm = 1;
  } else if (strcasecmp(mode, "ctr") == 0) {
    is_ctr = 1;
  } else {
    lua_pushnil(L);
    lua_pushstring(L, "Unsupported AES mode. Use 'cbc', 'gcm', or 'ctr'.");
    return 2;
  }
  // Validate key length (16, 24, 32 bytes)
  if (keylen != 16 && keylen != 24 && keylen != 32) {
    lua_pushnil(L);
    lua_pushstring(L, "AES key must be 16, 24, or 32 bytes");
    return 2;
  }
  // Validate IV/nonce length
  if (is_cbc || is_ctr) {
    if (ivlen != 16) {
      lua_pushnil(L);
      lua_pushstring(L, "AES IV/nonce must be 16 bytes for CBC/CTR");
      return 2;
    }
  } else if (is_gcm) {
    if (ivlen < 12 || ivlen > 16) {
      lua_pushnil(L);
      lua_pushstring(L, "AES GCM nonce must be 12-16 bytes");
      return 2;
    }
  }

  // GCM: require tag and optional AAD
  if (is_gcm) {
    if (!tag || taglen < 12 || taglen > 16) {
      lua_pushnil(L);
      lua_pushstring(L, "AES GCM tag must be 12-16 bytes");
      return 2;
    }
  }

  int ret = 0;
  unsigned char *output = NULL;

  if (is_cbc) {
    // Ciphertext must be a multiple of block size
    if (ctlen == 0 || (ctlen % 16) != 0) {
      lua_pushnil(L);
      lua_pushstring(L, "Ciphertext length must be a multiple of 16");
      return 2;
    }
    output = malloc(ctlen);
    if (!output) {
      lua_pushnil(L);
      lua_pushstring(L, "Memory allocation failed");
      return 2;
    }
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    ret = mbedtls_aes_setkey_dec(&aes, key, keylen * 8);
    if (ret != 0) {
      free(output);
      mbedtls_aes_free(&aes);
      lua_pushnil(L);
      lua_pushstring(L, "Failed to set AES decryption key");
      return 2;
    }
    unsigned char iv_copy[16];
    memcpy(iv_copy, iv, 16);
    ret = mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, ctlen, iv_copy,
                                ciphertext, output);
    mbedtls_aes_free(&aes);
    if (ret != 0) {
      free(output);
      lua_pushnil(L);
      lua_pushstring(L, "AES CBC decryption failed");
      return 2;
    }
    // PKCS7 unpadding
    if (ctlen == 0) {
      free(output);
      lua_pushnil(L);
      lua_pushstring(L, "Decrypted data is empty");
      return 2;
    }
    unsigned char pad = output[ctlen - 1];
    if (pad == 0 || pad > 16) {
      free(output);
      lua_pushnil(L);
      lua_pushstring(L, "Invalid PKCS7 padding");
      return 2;
    }
    for (size_t i = 0; i < pad; ++i) {
      if (output[ctlen - 1 - i] != pad) {
        free(output);
        lua_pushnil(L);
        lua_pushstring(L, "Invalid PKCS7 padding");
        return 2;
      }
    }
    size_t ptlen = ctlen - pad;
    lua_pushlstring(L, (const char *)output, ptlen);
    free(output);
    return 1;
  } else if (is_ctr) {
    // CTR mode: no padding
    output = malloc(ctlen);
    if (!output) {
      lua_pushnil(L);
      lua_pushstring(L, "Memory allocation failed");
      return 2;
    }
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    ret = mbedtls_aes_setkey_enc(&aes, key, keylen * 8);
    if (ret != 0) {
      free(output);
      mbedtls_aes_free(&aes);
      lua_pushnil(L);
      lua_pushstring(L, "Failed to set AES encryption key");
      return 2;
    }
    unsigned char nonce_counter[16];
    unsigned char stream_block[16];
    size_t nc_off = 0;
    memcpy(nonce_counter, iv, 16);
    memset(stream_block, 0, 16);
    ret = mbedtls_aes_crypt_ctr(&aes, ctlen, &nc_off, nonce_counter,
                                stream_block, ciphertext, output);
    mbedtls_aes_free(&aes);
    if (ret != 0) {
      free(output);
      lua_pushnil(L);
      lua_pushstring(L, "AES CTR decryption failed");
      return 2;
    }
    lua_pushlstring(L, (const char *)output, ctlen);
    free(output);
    return 1;
  } else if (is_gcm) {
    // GCM mode: authenticated decryption
    output = malloc(ctlen);
    if (!output) {
      lua_pushnil(L);
      lua_pushstring(L, "Memory allocation failed");
      return 2;
    }
    mbedtls_gcm_context gcm;
    mbedtls_gcm_init(&gcm);
    ret = mbedtls_gcm_setkey(&gcm, MBEDTLS_CIPHER_ID_AES, key, keylen * 8);
    if (ret != 0) {
      free(output);
      mbedtls_gcm_free(&gcm);
      lua_pushnil(L);
      lua_pushstring(L, "Failed to set AES GCM key");
      return 2;
    }
    ret = mbedtls_gcm_auth_decrypt(&gcm, ctlen, iv, ivlen, aad, aadlen, tag,
                                   taglen, ciphertext, output);
    mbedtls_gcm_free(&gcm);
    if (ret != 0) {
      free(output);
      lua_pushnil(L);
      lua_pushstring(L, "AES GCM decryption failed or authentication failed");
      return 2;
    }
    lua_pushlstring(L, (const char *)output, ctlen);
    free(output);
    return 1;
  }
  lua_pushnil(L);
  lua_pushstring(L, "Internal error in AES decrypt");
  return 2;
}

// JWK functions

// Convert JWK (Lua table) to PEM key format
static int LuaConvertJwkToPem(lua_State *L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  const char *kty;

  if (lua_isnoneornil(L, 1) || lua_type(L, 1) != LUA_TTABLE) {
    lua_pushnil(L);
    lua_pushstring(L, "Expected a JWK table, got nil");
    return 2;
  }

  lua_getfield(L, 1, "kty");
  kty = lua_tostring(L, -1);
  if (!kty) {
    lua_pushnil(L);
    lua_pushstring(L, "Missing 'kty' in JWK");
    return 2;
  }

  int ret = -1;
  char *pem = NULL;
  mbedtls_pk_context pk;
  mbedtls_pk_init(&pk);

  if (strcasecmp(kty, "RSA") == 0) {
    // RSA JWK: n, e (base64url), optionally d, p, q, dp, dq, qi
    lua_getfield(L, 1, "n");
    lua_getfield(L, 1, "e");
    const char *n_b64 = lua_tostring(L, -2);
    const char *e_b64 = lua_tostring(L, -1);
    if (!n_b64 || !*n_b64) {
      lua_pushnil(L);
      lua_pushstring(L, "Missing or empty 'n' in JWK");
      return 2;
    }
    if (!e_b64 || !*e_b64) {
      lua_pushnil(L);
      lua_pushstring(L, "Missing or empty 'e' in JWK");
      return 2;
    }
    // Optional private fields
    lua_getfield(L, 1, "d");
    lua_getfield(L, 1, "p");
    lua_getfield(L, 1, "q");
    lua_getfield(L, 1, "dp");
    lua_getfield(L, 1, "dq");
    lua_getfield(L, 1, "qi");
    const char *d_b64 = lua_tostring(L, -6);
    const char *p_b64 = lua_tostring(L, -5);
    const char *q_b64 = lua_tostring(L, -4);
    const char *dp_b64 = lua_tostring(L, -3);
    const char *dq_b64 = lua_tostring(L, -2);
    const char *qi_b64 = lua_tostring(L, -1);
    int has_private = d_b64 && *d_b64;
    // Decode base64url to binary
    size_t n_len, e_len;
    unsigned char n_bin[1024], e_bin[16];
    char *n_b64_std = strdup(n_b64), *e_b64_std = strdup(e_b64);
    for (char *p = n_b64_std; *p; ++p)
      if (*p == '-')
        *p = '+';
      else if (*p == '_')
        *p = '/';
    for (char *p = e_b64_std; *p; ++p)
      if (*p == '-')
        *p = '+';
      else if (*p == '_')
        *p = '/';
    int n_mod = strlen(n_b64_std) % 4;
    int e_mod = strlen(e_b64_std) % 4;
    if (n_mod)
      for (int i = 0; i < 4 - n_mod; ++i)
        strcat(n_b64_std, "=");
    if (e_mod)
      for (int i = 0; i < 4 - e_mod; ++i)
        strcat(e_b64_std, "=");
    if (mbedtls_base64_decode(n_bin, sizeof(n_bin), &n_len,
                              (const unsigned char *)n_b64_std,
                              strlen(n_b64_std)) != 0 ||
        mbedtls_base64_decode(e_bin, sizeof(e_bin), &e_len,
                              (const unsigned char *)e_b64_std,
                              strlen(e_b64_std)) != 0) {
      free(n_b64_std);
      free(e_b64_std);
      lua_pushnil(L);
      lua_pushstring(L, "Base64 decode failed");
      return 2;
    }
    free(n_b64_std);
    free(e_b64_std);
    // Build RSA context in pk
    if ((ret = mbedtls_pk_setup(
             &pk, mbedtls_pk_info_from_type(MBEDTLS_PK_RSA))) != 0) {
      lua_pushnil(L);
      lua_pushstring(L, "mbedtls_pk_setup failed");
      return 2;
    }
    mbedtls_rsa_context *rsa = mbedtls_pk_rsa(pk);
    mbedtls_rsa_init(rsa, MBEDTLS_RSA_PKCS_V15, 0);
    mbedtls_mpi_read_binary(&rsa->N, n_bin, n_len);
    mbedtls_mpi_read_binary(&rsa->E, e_bin, e_len);
    rsa->len = n_len;
    if (has_private) {
      // Decode and set private fields
      size_t d_len, p_len, q_len, dp_len, dq_len, qi_len;
      unsigned char d_bin[1024], p_bin[512], q_bin[512], dp_bin[512],
          dq_bin[512], qi_bin[512];
// Decode all private fields (skip if NULL)
#define DECODE_B64URL(var, b64, bin, binlen)                                \
  if (b64 && *b64) {                                                        \
    char *b64_std = strdup(b64);                                            \
    for (char *p = b64_std; *p; ++p)                                        \
      if (*p == '-')                                                        \
        *p = '+';                                                           \
      else if (*p == '_')                                                   \
        *p = '/';                                                           \
    int mod = strlen(b64_std) % 4;                                          \
    if (mod)                                                                \
      for (int i = 0; i < 4 - mod; ++i)                                     \
        strcat(b64_std, "=");                                               \
    mbedtls_base64_decode(bin, sizeof(bin), &binlen,                        \
                          (const unsigned char *)b64_std, strlen(b64_std)); \
    free(b64_std);                                                          \
  }
      DECODE_B64URL(d, d_b64, d_bin, d_len);
      DECODE_B64URL(p, p_b64, p_bin, p_len);
      DECODE_B64URL(q, q_b64, q_bin, q_len);
      DECODE_B64URL(dp, dp_b64, dp_bin, dp_len);
      DECODE_B64URL(dq, dq_b64, dq_bin, dq_len);
      DECODE_B64URL(qi, qi_b64, qi_bin, qi_len);
      mbedtls_mpi_read_binary(&rsa->D, d_bin, d_len);
      mbedtls_mpi_read_binary(&rsa->P, p_bin, p_len);
      mbedtls_mpi_read_binary(&rsa->Q, q_bin, q_len);
      mbedtls_mpi_read_binary(&rsa->DP, dp_bin, dp_len);
      mbedtls_mpi_read_binary(&rsa->DQ, dq_bin, dq_len);
      mbedtls_mpi_read_binary(&rsa->QP, qi_bin, qi_len);
    }
    // Write PEM
    unsigned char buf[4096];
    if (has_private) {
      ret = mbedtls_pk_write_key_pem(&pk, buf, sizeof(buf));
    } else {
      ret = mbedtls_pk_write_pubkey_pem(&pk, buf, sizeof(buf));
    }
    if (ret != 0) {
      mbedtls_pk_free(&pk);
      lua_pushnil(L);
      lua_pushstring(L, "PEM write failed");
      return 2;
    }
    pem = strdup((char *)buf);
    mbedtls_pk_free(&pk);
    lua_pushstring(L, pem);
    free(pem);
    return 1;
  } else if (strcasecmp(kty, "EC") == 0) {
    // EC JWK: crv, x, y (base64url), optionally d
    lua_getfield(L, 1, "crv");
    lua_getfield(L, 1, "x");
    lua_getfield(L, 1, "y");
    lua_getfield(L, 1, "d");
    const char *crv = lua_tostring(L, -4);
    const char *x_b64 = lua_tostring(L, -3);
    const char *y_b64 = lua_tostring(L, -2);
    const char *d_b64 = lua_tostring(L, -1);
    if (!crv || !*crv) {
      lua_pushnil(L);
      lua_pushstring(L, "Missing or empty 'crv' in JWK");
      return 2;
    }
    if (!x_b64 || !*x_b64) {
      lua_pushnil(L);
      lua_pushstring(L, "Missing or empty 'x' in JWK");
      return 2;
    }
    if (!y_b64 || !*y_b64) {
      lua_pushnil(L);
      lua_pushstring(L, "Missing or empty 'y' in JWK");
      return 2;
    }
    int has_private = d_b64 && *d_b64;
    mbedtls_ecp_group_id gid = find_curve_by_name(crv);
    if (gid == MBEDTLS_ECP_DP_NONE) {
      lua_pushnil(L);
      lua_pushstring(L, "Unknown curve");
      return 2;
    }
    size_t x_len, y_len;
    unsigned char x_bin[72], y_bin[72];
    char *x_b64_std = strdup(x_b64), *y_b64_std = strdup(y_b64);
    for (char *p = x_b64_std; *p; ++p)
      if (*p == '-')
        *p = '+';
      else if (*p == '_')
        *p = '/';
    for (char *p = y_b64_std; *p; ++p)
      if (*p == '-')
        *p = '+';
      else if (*p == '_')
        *p = '/';
    int x_mod = strlen(x_b64_std) % 4;
    int y_mod = strlen(y_b64_std) % 4;
    if (x_mod)
      for (int i = 0; i < 4 - x_mod; ++i)
        strcat(x_b64_std, "=");
    if (y_mod)
      for (int i = 0; i < 4 - y_mod; ++i)
        strcat(y_b64_std, "=");
    if (mbedtls_base64_decode(x_bin, sizeof(x_bin), &x_len,
                              (const unsigned char *)x_b64_std,
                              strlen(x_b64_std)) != 0 ||
        mbedtls_base64_decode(y_bin, sizeof(y_bin), &y_len,
                              (const unsigned char *)y_b64_std,
                              strlen(y_b64_std)) != 0) {
      free(x_b64_std);
      free(y_b64_std);
      lua_pushnil(L);
      lua_pushstring(L, "Base64 decode failed");
      return 2;
    }
    free(x_b64_std);
    free(y_b64_std);
    if ((ret = mbedtls_pk_setup(
             &pk, mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY))) != 0) {
      lua_pushnil(L);
      lua_pushstring(L, "mbedtls_pk_setup failed");
      return 2;
    }
    mbedtls_ecp_keypair *ec = mbedtls_pk_ec(pk);
    mbedtls_ecp_keypair_init(ec);
    mbedtls_ecp_group_load(&ec->grp, gid);
    mbedtls_mpi_read_binary(&ec->Q.X, x_bin, x_len);
    mbedtls_mpi_read_binary(&ec->Q.Y, y_bin, y_len);
    mbedtls_mpi_lset(&ec->Q.Z, 1);
    if (has_private) {
      size_t d_len;
      unsigned char d_bin[72];
      DECODE_B64URL(d, d_b64, d_bin, d_len);
      mbedtls_mpi_read_binary(&ec->d, d_bin, d_len);
    }
    unsigned char buf[4096];
    if (has_private) {
      ret = mbedtls_pk_write_key_pem(&pk, buf, sizeof(buf));
    } else {
      ret = mbedtls_pk_write_pubkey_pem(&pk, buf, sizeof(buf));
    }
    if (ret != 0) {
      mbedtls_pk_free(&pk);
      lua_pushnil(L);
      lua_pushstring(L, "PEM write failed");
      return 2;
    }
    pem = strdup((char *)buf);
    mbedtls_pk_free(&pk);
    lua_pushstring(L, pem);
    free(pem);
    return 1;
  } else {
    lua_pushnil(L);
    lua_pushstring(L, "Unsupported kty");
    return 2;
  }
}

// Convert PEM key to JWK (Lua table) format
static void base64_to_base64url(char *str) {
  if (!str)
    return;
  for (char *p = str; *p; p++) {
    if (*p == '+')
      *p = '-';
    else if (*p == '/')
      *p = '_';
  }
  // Remove padding
  size_t len = strlen(str);
  while (len > 0 && str[len - 1] == '=') {
    str[--len] = '\0';
  }
}

static int LuaConvertPemToJwk(lua_State *L) {
  const char *pem_key = luaL_checkstring(L, 1);
  int has_claims = 0;
  if (!lua_isnoneornil(L, 2) && lua_istable(L, 2)) {
    has_claims = 1;
  }

  mbedtls_pk_context key;
  mbedtls_pk_init(&key);
  int ret;

  // Parse the PEM key
  if ((ret = mbedtls_pk_parse_key(&key, (const unsigned char *)pem_key,
                                  strlen(pem_key) + 1, NULL, 0)) != 0 &&
      (ret = mbedtls_pk_parse_public_key(&key, (const unsigned char *)pem_key,
                                         strlen(pem_key) + 1)) != 0) {
    lua_pushnil(L);
    lua_pushfstring(L, "Failed to parse PEM key: -0x%04x", -ret);
    mbedtls_pk_free(&key);
    return 2;
  }

  lua_newtable(L);

  if (mbedtls_pk_get_type(&key) == MBEDTLS_PK_RSA) {
    lua_pushstring(L, "RSA");
    lua_setfield(L, -2, "kty");
    const mbedtls_rsa_context *rsa = mbedtls_pk_rsa(key);
    size_t n_len = mbedtls_mpi_size(&rsa->N);
    size_t e_len = mbedtls_mpi_size(&rsa->E);
    unsigned char *n = malloc(n_len);
    unsigned char *e = malloc(e_len);
    if (!n || !e) {
      lua_pushnil(L);
      lua_pushstring(L, "Memory allocation failed");
      free(n);
      free(e);
      mbedtls_pk_free(&key);
      return 2;
    }
    mbedtls_mpi_write_binary(&rsa->N, n, n_len);
    mbedtls_mpi_write_binary(&rsa->E, e, e_len);
    char *n_b64 = NULL, *e_b64 = NULL;
    size_t n_b64_len, e_b64_len;
    mbedtls_base64_encode(NULL, 0, &n_b64_len, n, n_len);
    mbedtls_base64_encode(NULL, 0, &e_b64_len, e, e_len);
    n_b64 = malloc(n_b64_len + 1);
    e_b64 = malloc(e_b64_len + 1);
    mbedtls_base64_encode((unsigned char *)n_b64, n_b64_len, &n_b64_len, n,
                          n_len);
    n_b64[n_b64_len] = '\0';
    base64_to_base64url(n_b64);
    mbedtls_base64_encode((unsigned char *)e_b64, e_b64_len, &e_b64_len, e,
                          e_len);
    e_b64[e_b64_len] = '\0';
    base64_to_base64url(e_b64);
    lua_pushstring(L, n_b64);
    lua_setfield(L, -2, "n");
    lua_pushstring(L, e_b64);
    lua_setfield(L, -2, "e");
    // If private key, add private fields
    if (mbedtls_rsa_check_privkey(rsa) == 0 && rsa->D.p) {
      size_t d_len = mbedtls_mpi_size(&rsa->D);
      size_t p_len = mbedtls_mpi_size(&rsa->P);
      size_t q_len = mbedtls_mpi_size(&rsa->Q);
      size_t dp_len = mbedtls_mpi_size(&rsa->DP);
      size_t dq_len = mbedtls_mpi_size(&rsa->DQ);
      size_t qi_len = mbedtls_mpi_size(&rsa->QP);
      unsigned char *d = malloc(d_len), *p = malloc(p_len), *q = malloc(q_len),
                    *dp = malloc(dp_len), *dq = malloc(dq_len),
                    *qi = malloc(qi_len);
      if (!d || !p || !q || !dp || !dq || !qi) {
        free(d);
        free(p);
        free(q);
        free(dp);
        free(dq);
        free(qi);
        lua_pushnil(L);
        lua_pushstring(L, "Memory allocation failed");
        mbedtls_pk_free(&key);
        return 2;
      }
      mbedtls_mpi_write_binary(&rsa->D, d, d_len);
      mbedtls_mpi_write_binary(&rsa->P, p, p_len);
      mbedtls_mpi_write_binary(&rsa->Q, q, q_len);
      mbedtls_mpi_write_binary(&rsa->DP, dp, dp_len);
      mbedtls_mpi_write_binary(&rsa->DQ, dq, dq_len);
      mbedtls_mpi_write_binary(&rsa->QP, qi, qi_len);
      char *d_b64 = NULL, *p_b64 = NULL, *q_b64 = NULL, *dp_b64 = NULL,
           *dq_b64 = NULL, *qi_b64 = NULL;
      size_t d_b64_len, p_b64_len, q_b64_len, dp_b64_len, dq_b64_len,
          qi_b64_len;
      mbedtls_base64_encode(NULL, 0, &d_b64_len, d, d_len);
      mbedtls_base64_encode(NULL, 0, &p_b64_len, p, p_len);
      mbedtls_base64_encode(NULL, 0, &q_b64_len, q, q_len);
      mbedtls_base64_encode(NULL, 0, &dp_b64_len, dp, dp_len);
      mbedtls_base64_encode(NULL, 0, &dq_b64_len, dq, dq_len);
      mbedtls_base64_encode(NULL, 0, &qi_b64_len, qi, qi_len);
      d_b64 = malloc(d_b64_len + 1);
      p_b64 = malloc(p_b64_len + 1);
      q_b64 = malloc(q_b64_len + 1);
      dp_b64 = malloc(dp_b64_len + 1);
      dq_b64 = malloc(dq_b64_len + 1);
      qi_b64 = malloc(qi_b64_len + 1);
      mbedtls_base64_encode((unsigned char *)d_b64, d_b64_len, &d_b64_len, d,
                            d_len);
      mbedtls_base64_encode((unsigned char *)p_b64, p_b64_len, &p_b64_len, p,
                            p_len);
      mbedtls_base64_encode((unsigned char *)q_b64, q_b64_len, &q_b64_len, q,
                            q_len);
      mbedtls_base64_encode((unsigned char *)dp_b64, dp_b64_len, &dp_b64_len,
                            dp, dp_len);
      mbedtls_base64_encode((unsigned char *)dq_b64, dq_b64_len, &dq_b64_len,
                            dq, dq_len);
      mbedtls_base64_encode((unsigned char *)qi_b64, qi_b64_len, &qi_b64_len,
                            qi, qi_len);
      d_b64[d_b64_len] = '\0';
      p_b64[p_b64_len] = '\0';
      q_b64[q_b64_len] = '\0';
      dp_b64[dp_b64_len] = '\0';
      dq_b64[dq_b64_len] = '\0';
      qi_b64[qi_b64_len] = '\0';
      // Convert all private components to base64url
      base64_to_base64url(d_b64);
      base64_to_base64url(p_b64);
      base64_to_base64url(q_b64);
      base64_to_base64url(dp_b64);
      base64_to_base64url(dq_b64);
      base64_to_base64url(qi_b64);
      lua_pushstring(L, d_b64);
      lua_setfield(L, -2, "d");
      lua_pushstring(L, p_b64);
      lua_setfield(L, -2, "p");
      lua_pushstring(L, q_b64);
      lua_setfield(L, -2, "q");
      lua_pushstring(L, dp_b64);
      lua_setfield(L, -2, "dp");
      lua_pushstring(L, dq_b64);
      lua_setfield(L, -2, "dq");
      lua_pushstring(L, qi_b64);
      lua_setfield(L, -2, "qi");
      free(d);
      free(p);
      free(q);
      free(dp);
      free(dq);
      free(qi);
      free(d_b64);
      free(p_b64);
      free(q_b64);
      free(dp_b64);
      free(dq_b64);
      free(qi_b64);
    }
    free(n);
    free(e);
    free(n_b64);
    free(e_b64);
  } else if (mbedtls_pk_get_type(&key) == MBEDTLS_PK_ECKEY) {
    // Handle ECDSA keys
    const mbedtls_ecp_keypair *ec = mbedtls_pk_ec(key);
    const mbedtls_ecp_point *Q = &ec->Q;
    size_t x_len = (ec->grp.pbits + 7) / 8;
    size_t y_len = (ec->grp.pbits + 7) / 8;
    unsigned char *x = malloc(x_len);
    unsigned char *y = malloc(y_len);
    if (!x || !y) {
      lua_pushnil(L);
      lua_pushstring(L, "Memory allocation failed");
      free(x);
      free(y);
      mbedtls_pk_free(&key);
      return 2;
    }
    mbedtls_mpi_write_binary(&Q->X, x, x_len);
    mbedtls_mpi_write_binary(&Q->Y, y, y_len);
    char *x_b64 = NULL, *y_b64 = NULL;
    size_t x_b64_len, y_b64_len;
    mbedtls_base64_encode(NULL, 0, &x_b64_len, x, x_len);
    mbedtls_base64_encode(NULL, 0, &y_b64_len, y, y_len);
    x_b64 = malloc(x_b64_len + 1);
    y_b64 = malloc(y_b64_len + 1);
    mbedtls_base64_encode((unsigned char *)x_b64, x_b64_len, &x_b64_len, x,
                          x_len);
    x_b64[x_b64_len] = '\0';
    base64_to_base64url(x_b64);
    mbedtls_base64_encode((unsigned char *)y_b64, y_b64_len, &y_b64_len, y,
                          y_len);
    y_b64[y_b64_len] = '\0';
    base64_to_base64url(y_b64);
    // Set kty and crv for EC keys
    lua_pushstring(L, "EC");
    lua_setfield(L, -2, "kty");
    const mbedtls_ecp_curve_info *curve_info =
        mbedtls_ecp_curve_info_from_grp_id(ec->grp.id);
    if (curve_info && curve_info->name) {
      lua_pushstring(L, curve_info->name);
      lua_setfield(L, -2, "crv");
    } else {
      lua_pushstring(L, "unknown");
      lua_setfield(L, -2, "crv");
    }
    lua_pushstring(L, x_b64);
    lua_setfield(L, -2, "x");
    lua_pushstring(L, y_b64);
    lua_setfield(L, -2, "y");
    // If private key, add 'd'
    if (mbedtls_ecp_check_privkey(&ec->grp, &ec->d) == 0 && ec->d.p) {
      size_t d_len = mbedtls_mpi_size(&ec->d);
      unsigned char *d = malloc(d_len);
      if (!d) {
        free(x);
        free(y);
        free(x_b64);
        free(y_b64);
        lua_pushnil(L);
        lua_pushstring(L, "Memory allocation failed");
        mbedtls_pk_free(&key);
        return 2;
      }
      mbedtls_mpi_write_binary(&ec->d, d, d_len);
      char *d_b64 = NULL;
      size_t d_b64_len;
      mbedtls_base64_encode(NULL, 0, &d_b64_len, d, d_len);
      d_b64 = malloc(d_b64_len + 1);
      mbedtls_base64_encode((unsigned char *)d_b64, d_b64_len, &d_b64_len, d,
                            d_len);
      d_b64[d_b64_len] = '\0';
      base64_to_base64url(d_b64);
      lua_pushstring(L, d_b64);
      lua_setfield(L, -2, "d");
      free(d);
      free(d_b64);
    }
    free(x);
    free(y);
    free(x_b64);
    free(y_b64);
  } else {
    lua_pushnil(L);
    lua_pushstring(L, "Unsupported key type");
    mbedtls_pk_free(&key);
    return 2;
  }

  mbedtls_pk_free(&key);

  // Merge additional claims if provided and compatible with RFC7517
  if (has_claims) {
    static const char *allowed[] = {"kty", "use",      "sig", "key_ops",
                                    "alg", "kid",      "x5u", "x5c",
                                    "x5t", "x5t#S256", NULL};
    lua_pushnil(L);  // first key
    while (lua_next(L, 2) != 0) {
      const char *k = lua_tostring(L, -2);
      int allowed_key = 0;
      for (int i = 0; allowed[i]; ++i) {
        if (strcmp(k, allowed[i]) == 0) {
          allowed_key = 1;
          break;
        }
      }
      if (allowed_key) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_settable(L, -4);
      } else {
        lua_pop(L, 1);
      }
    }
  }

  return 1;
}

// CSR creation Function
static int LuaGenerateCSR(lua_State *L) {
  const char *key_pem = luaL_checkstring(L, 1);
  const char *subject_name;
  const char *san_list = luaL_optstring(L, 3, NULL);

  if (lua_isnoneornil(L, 2)) {
    subject_name = "";
  } else {
    subject_name = luaL_checkstring(L, 2);
  }

  if (lua_isnoneornil(L, 3) && subject_name[0] == '\0') {
    lua_pushnil(L);
    lua_pushstring(L, "Subject name or SANs are required");
    return 2;
  }
  mbedtls_pk_context key;
  mbedtls_x509write_csr req;
  char buf[4096];
  int ret;

  mbedtls_pk_init(&key);
  mbedtls_x509write_csr_init(&req);

  if ((ret = mbedtls_pk_parse_key(&key, (const unsigned char *)key_pem,
                                  strlen(key_pem) + 1, NULL, 0)) != 0) {
    lua_pushnil(L);
    lua_pushfstring(L, "Failed to parse key: %d", ret);
    return 2;
  }

  mbedtls_x509write_csr_set_subject_name(&req, subject_name);
  mbedtls_x509write_csr_set_key(&req, &key);
  mbedtls_x509write_csr_set_md_alg(&req, MBEDTLS_MD_SHA256);

  if (san_list) {
    if ((ret = mbedtls_x509write_csr_set_extension(
             &req, MBEDTLS_OID_SUBJECT_ALT_NAME,
             MBEDTLS_OID_SIZE(MBEDTLS_OID_SUBJECT_ALT_NAME),
             (const unsigned char *)san_list, strlen(san_list))) != 0) {
      lua_pushnil(L);
      lua_pushfstring(L, "Failed to set SANs: %d", ret);
      return 2;
    }
  }

  if ((ret = mbedtls_x509write_csr_pem(&req, (unsigned char *)buf, sizeof(buf),
                                       NULL, NULL)) < 0) {
    lua_pushnil(L);
    lua_pushfstring(L, "Failed to write CSR: %d", ret);
    return 2;
  }

  lua_pushstring(L, buf);

  mbedtls_pk_free(&key);
  mbedtls_x509write_csr_free(&req);

  return 1;
}

// LuaCrypto compatible API
static int LuaCryptoSign(lua_State *L) {
  // Type of signature (e.g., "rsa", "ecdsa", "rsa-pss")
  const char *dtype = luaL_checkstring(L, 1);
  // Remove the first argument (key type or cipher type) before dispatching
  lua_remove(L, 1);

  if (strcasecmp(dtype, "rsa") == 0) {
    return LuaRSASign(L);
  } else if (strcasecmp(dtype, "rsa-pss") == 0 ||
             strcasecmp(dtype, "rsapss") == 0) {
    return LuaRSAPSSSign(L);
  } else if (strcasecmp(dtype, "ecdsa") == 0) {
    return LuaECDSASign(L);
  } else {
    return luaL_error(L, "Unsupported signature type: %s", dtype);
  }
}

static int LuaCryptoVerify(lua_State *L) {
  // Type of signature (e.g., "rsa", "ecdsa", "rsa-pss")
  const char *dtype = luaL_checkstring(L, 1);
  // Remove the first argument (key type or cipher type) before dispatching
  lua_remove(L, 1);

  if (strcasecmp(dtype, "rsa") == 0) {
    return LuaRSAVerify(L);
  } else if (strcasecmp(dtype, "rsa-pss") == 0 ||
             strcasecmp(dtype, "rsapss") == 0) {
    return LuaRSAPSSVerify(L);
  } else if (strcasecmp(dtype, "ecdsa") == 0) {
    return LuaECDSAVerify(L);
  } else {
    return luaL_error(L, "Unsupported signature type: %s", dtype);
  }
}

static int LuaCryptoEncrypt(lua_State *L) {
  // Args: cipher_type, key, msg, options table
  const char *cipher = luaL_checkstring(L, 1);
  // Remove cipher_type from stack, so key is at 1, msg at 2, options at 3
  lua_remove(L, 1);

  if (strcasecmp(cipher, "rsa") == 0) {
    return LuaRSAEncrypt(L);
  } else if (strcasecmp(cipher, "aes") == 0) {
    return LuaAesEncrypt(L);
  } else {
    return luaL_error(L, "Unsupported cipher type: %s", cipher);
  }
}

static int LuaCryptoDecrypt(lua_State *L) {
  // Args: cipher_type, key, ciphertext, options table
  const char *cipher = luaL_checkstring(L, 1);
  // Remove cipher_type, so key is at 1, ciphertext at 2, options at 3
  lua_remove(L, 1);

  if (strcasecmp(cipher, "rsa") == 0) {
    return LuaRSADecrypt(L);
  } else if (strcasecmp(cipher, "aes") == 0) {
    return LuaAesDecrypt(L);
  } else {
    return luaL_error(L, "Unsupported cipher type: %s", cipher);
  }
}

static int LuaCryptoGenerateKeyPair(lua_State *L) {
  // If the first argument is a number, treat it as RSA key length
  if (lua_gettop(L) >= 1 && lua_type(L, 1) == LUA_TNUMBER) {
    // Call LuaRSAGenerateKeyPair with the number as the key length
    return LuaRSAGenerateKeyPair(L);
  }
  // Otherwise, get the key type from the first argument, default to "rsa"
  const char *type = luaL_optstring(L, 1, "rsa");
  lua_remove(L, 1);

  if (strcasecmp(type, "rsa") == 0) {
    return LuaRSAGenerateKeyPair(L);
  } else if (strcasecmp(type, "ecdsa") == 0) {
    return LuaECDSAGenerateKeyPair(L);
  } else if (strcasecmp(type, "aes") == 0) {
    return LuaAesGenerateKey(L);
  } else {
    return luaL_error(L, "Unsupported key type: %s", type);
  }
}

static const luaL_Reg kLuaCrypto[] = {
    {"sign", LuaCryptoSign},                        //
    {"verify", LuaCryptoVerify},                    //
    {"encrypt", LuaCryptoEncrypt},                  //
    {"decrypt", LuaCryptoDecrypt},                  //
    {"generateKeyPair", LuaCryptoGenerateKeyPair},  //
    {"convertJwkToPem", LuaConvertJwkToPem},        //
    {"convertPemToJwk", LuaConvertPemToJwk},        //
    {"generateCsr", LuaGenerateCSR},                //
    {0},                                            //
};

int LuaCrypto(lua_State *L) {
  luaL_newlib(L, kLuaCrypto);
  return 1;
}
