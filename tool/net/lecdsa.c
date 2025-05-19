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
// lecdsa.c - ECDSA key generation, signing, and verification for redbean

#include "libc/log/log.h"
#include "net/https/https.h"
#include "third_party/lua/lauxlib.h"
#include "third_party/mbedtls/ecdsa.h"
#include "third_party/mbedtls/error.h"
#include "third_party/mbedtls/md.h"
#include "third_party/mbedtls/pk.h"

// Supported curves mapping
typedef struct {
  const char *name;
  mbedtls_ecp_group_id id;
} curve_map_t;

static const curve_map_t supported_curves[] = {
    {"secp256r1", MBEDTLS_ECP_DP_SECP256R1},
    {"secp384r1", MBEDTLS_ECP_DP_SECP384R1},
    {"secp521r1", MBEDTLS_ECP_DP_SECP521R1},
    {"secp192r1", MBEDTLS_ECP_DP_SECP192R1},
    {"secp224r1", MBEDTLS_ECP_DP_SECP224R1},
    {"curve25519", MBEDTLS_ECP_DP_CURVE25519},
    {NULL, 0}};

typedef enum { SHA256, SHA384, SHA512 } hash_algorithm_t;

static mbedtls_md_type_t hash_to_md_type(hash_algorithm_t hash_alg) {
  switch (hash_alg) {
    case SHA256:
      return MBEDTLS_MD_SHA256;
    case SHA384:
      return MBEDTLS_MD_SHA384;
    case SHA512:
      return MBEDTLS_MD_SHA512;
    default:
      return MBEDTLS_MD_SHA256;  // Default to SHA-256
  }
}

static size_t get_hash_size(hash_algorithm_t hash_alg) {
  switch (hash_alg) {
    case SHA256:
      return 32;
    case SHA384:
      return 48;
    case SHA512:
      return 64;
    default:
      return 32;  // Default to SHA-256
  }
}

static hash_algorithm_t string_to_hash_alg(const char *hash_name) {
  if (!hash_name || !*hash_name) {
    return SHA256;  // Default to SHA-256 if no name provided
  }

  if (strcasecmp(hash_name, "sha256") == 0 ||
      strcasecmp(hash_name, "sha-256") == 0) {
    return SHA256;
  } else if (strcasecmp(hash_name, "sha384") == 0 ||
             strcasecmp(hash_name, "sha-384") == 0) {
    return SHA384;
  } else if (strcasecmp(hash_name, "sha512") == 0 ||
             strcasecmp(hash_name, "sha-512") == 0) {
    return SHA512;
  } else {
    WARNF("(ecdsa) Unknown hash algorithm '%s', using SHA-256", hash_name);
    return SHA256;
  }
}

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

  return 1;
}
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

static int compute_hash(hash_algorithm_t hash_alg, const unsigned char *input,
                        size_t input_len, unsigned char *output,
                        size_t output_size) {
  mbedtls_md_context_t md_ctx;
  const mbedtls_md_info_t *md_info;
  int ret;

  mbedtls_md_type_t md_type = hash_to_md_type(hash_alg);
  md_info = mbedtls_md_info_from_type(md_type);
  if (md_info == NULL) {
    WARNF("(ecdsa) Unsupported hash algorithm");
    return -1;
  }

  if (output_size < mbedtls_md_get_size(md_info)) {
    WARNF("(ecdsa) Output buffer too small for hash");
    return -1;
  }

  mbedtls_md_init(&md_ctx);

  ret = mbedtls_md_setup(&md_ctx, md_info, 0);  // 0 = non-HMAC
  if (ret != 0) {
    WARNF("(ecdsa) Failed to set up hash context: -0x%04x", -ret);
    goto cleanup;
  }

  ret = mbedtls_md_starts(&md_ctx);
  if (ret != 0) {
    WARNF("(ecdsa) Failed to start hash operation: -0x%04x", -ret);
    goto cleanup;
  }

  ret = mbedtls_md_update(&md_ctx, input, input_len);
  if (ret != 0) {
    WARNF("(ecdsa) Failed to update hash: -0x%04x", -ret);
    goto cleanup;
  }

  ret = mbedtls_md_finish(&md_ctx, output);
  if (ret != 0) {
    WARNF("(ecdsa) Failed to finish hash: -0x%04x", -ret);
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

// Generate an ECDSA key pair and return in PEM format
static int GenerateKeyPair(const char *curve_name, char **priv_key_pem,
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
    VERBOSEF("(ecdsa) No curve specified, using default: secp256r1");
  } else {
    // Find the curve by name
    curve_id = find_curve_by_name(curve_name);
    if (curve_id == MBEDTLS_ECP_DP_NONE) {
      WARNF("(ecdsa) Unknown curve: %s, using default: secp256r1", curve_name);
      curve_id = MBEDTLS_ECP_DP_SECP256R1;
    }
  }

  mbedtls_pk_init(&key);

  // Generate the key with the specified curve
  ret = mbedtls_pk_setup(&key, mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY));
  if (ret != 0) {
    WARNF("(ecdsa) Failed to setup key: -0x%04x", -ret);
    goto cleanup;
  }

  ret =
      mbedtls_ecp_gen_key(curve_id, mbedtls_pk_ec(key), GenerateHardRandom, 0);
  if (ret != 0) {
    WARNF("(ecdsa) Failed to generate key: -0x%04x", -ret);
    goto cleanup;
  }

  // Generate private key PEM
  if (priv_key_pem != NULL) {
    memset(output_buf, 0, sizeof(output_buf));
    ret = mbedtls_pk_write_key_pem(&key, output_buf, sizeof(output_buf));
    if (ret != 0) {
      WARNF("(ecdsa) Failed to write private key: -0x%04x", -ret);
      goto cleanup;
    }
    *priv_key_pem = strdup((char *)output_buf);
    if (*priv_key_pem == NULL) {
      WARNF("(ecdsa) Failed to allocate memory for private key PEM");
      ret = -1;
      goto cleanup;
    }
  }

  // Generate public key PEM
  if (pub_key_pem != NULL) {
    memset(output_buf, 0, sizeof(output_buf));
    ret = mbedtls_pk_write_pubkey_pem(&key, output_buf, sizeof(output_buf));
    if (ret != 0) {
      WARNF("(ecdsa) Failed to write public key: -0x%04x", -ret);
      goto cleanup;
    }
    *pub_key_pem = strdup((char *)output_buf);
    if (*pub_key_pem == NULL) {
      WARNF("(ecdsa) Failed to allocate memory for public key PEM");
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
// Lua binding for generating ECDSA keys
static int LuaGenerateKeyPair(lua_State *L) {
  const char *curve_name = NULL;
  char *priv_key_pem = NULL;
  char *pub_key_pem = NULL;

  // Check if curve name is provided
  if (lua_gettop(L) >= 1 && !lua_isnil(L, 1)) {
    curve_name = luaL_checkstring(L, 1);
  }
  // If not provided, generate_key_pem will use the default

  int ret = GenerateKeyPair(curve_name, &priv_key_pem, &pub_key_pem);

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
static int Sign(const char *priv_key_pem, const char *message,
                hash_algorithm_t hash_alg, unsigned char **signature,
                size_t *sig_len) {
  mbedtls_pk_context key;
  unsigned char hash[64];  // Max hash size (SHA-512)
  size_t hash_size;
  int ret;

  *signature = NULL;
  *sig_len = 0;

  if (!priv_key_pem) {
    WARNF("(ecdsa) Private key is NULL");
    return -1;
  }

  // Get the length of the PEM string (excluding null terminator)
  size_t key_len = strlen(priv_key_pem);
  if (key_len == 0) {
    WARNF("(ecdsa) Private key is empty");
    return -1;
  }

  // Get hash size for the selected algorithm
  hash_size = get_hash_size(hash_alg);

  mbedtls_pk_init(&key);

  // Parse the private key from PEM directly without creating a copy
  ret = mbedtls_pk_parse_key(&key, (const unsigned char *)priv_key_pem,
                             key_len + 1, NULL, 0);

  if (ret != 0) {
    WARNF("(ecdsa) Failed to parse private key: -0x%04x", -ret);
    goto cleanup;
  }

  // Compute hash of the message using the specified algorithm
  ret = compute_hash(hash_alg, (const unsigned char *)message, strlen(message),
                     hash, sizeof(hash));
  if (ret != 0) {
    WARNF("(ecdsa) Failed to compute message hash");
    goto cleanup;
  }

  // Allocate memory for signature (max size for ECDSA)
  *signature = malloc(MBEDTLS_ECDSA_MAX_LEN);
  if (*signature == NULL) {
    WARNF("(ecdsa) Failed to allocate memory for signature");
    ret = -1;
    goto cleanup;
  }

  // Sign the hash using GenerateHardRandom
  ret = mbedtls_pk_sign(&key, hash_to_md_type(hash_alg), hash, hash_size,
                        *signature, sig_len, GenerateHardRandom, 0);

  if (ret != 0) {
    WARNF("(ecdsa) Failed to sign message: -0x%04x", -ret);
    free(*signature);
    *signature = NULL;
    *sig_len = 0;
    goto cleanup;
  }

cleanup:
  mbedtls_pk_free(&key);
  return ret;
}  // Lua binding for signing a message
static int LuaSign(lua_State *L) {
  const char *hash_name = luaL_optstring(L, 1, "sha256");  // Default to SHA-256
  const char *message = luaL_checkstring(L, 2);
  const char *priv_key_pem = luaL_checkstring(L, 3);

  hash_algorithm_t hash_alg = string_to_hash_alg(hash_name);

  unsigned char *signature = NULL;
  size_t sig_len = 0;

  int ret = Sign(priv_key_pem, message, hash_alg, &signature, &sig_len);

  if (ret == 0) {
    lua_pushlstring(L, (const char *)signature, sig_len);
    free(signature);
  } else {
    lua_pushnil(L);
  }

  return 1;
}

// Verify a signature using an ECDSA public key in PEM format
static int Verify(const char *pub_key_pem, const char *message,
                  const unsigned char *signature, size_t sig_len,
                  hash_algorithm_t hash_alg) {
  mbedtls_pk_context key;
  unsigned char hash[64];  // Max hash size (SHA-512)
  size_t hash_size;
  int ret;

  if (!pub_key_pem) {
    WARNF("(ecdsa) Public key is NULL");
    return -1;
  }

  // Get the length of the PEM string (excluding null terminator)
  size_t key_len = strlen(pub_key_pem);
  if (key_len == 0) {
    WARNF("(ecdsa) Public key is empty");
    return -1;
  }

  // Get hash size for the selected algorithm
  hash_size = get_hash_size(hash_alg);

  mbedtls_pk_init(&key);

  // Parse the public key from PEM
  ret = mbedtls_pk_parse_public_key(&key, (const unsigned char *)pub_key_pem,
                                    key_len + 1);
  if (ret != 0) {
    WARNF("(ecdsa) Failed to parse public key: -0x%04x", -ret);
    goto cleanup;
  }

  // Compute hash of the message using the specified algorithm
  ret = compute_hash(hash_alg, (const unsigned char *)message, strlen(message),
                     hash, sizeof(hash));
  if (ret != 0) {
    WARNF("(ecdsa) Failed to compute message hash");
    goto cleanup;
  }

  // Verify the signature
  ret = mbedtls_pk_verify(&key, hash_to_md_type(hash_alg), hash, hash_size,
                          signature, sig_len);
  if (ret != 0) {
    WARNF("(ecdsa) Signature verification failed: -0x%04x", -ret);
    goto cleanup;
  }

cleanup:
  mbedtls_pk_free(&key);
  return ret;
}
static int LuaVerify(lua_State *L) {
  const char *hash_name = luaL_optstring(L, 1, "sha256");  // Default to SHA-256
  const char *message = luaL_checkstring(L, 2);
  const char *pub_key_pem = luaL_checkstring(L, 3);
  size_t sig_len;
  const unsigned char *signature =
      (const unsigned char *)luaL_checklstring(L, 4, &sig_len);

  hash_algorithm_t hash_alg = string_to_hash_alg(hash_name);

  int ret = Verify(pub_key_pem, message, signature, sig_len, hash_alg);

  lua_pushboolean(L, ret == 0);
  return 1;
}

// Register functions
static const luaL_Reg kLuaECDSA[] = {
    {"GenerateKeyPair", LuaGenerateKeyPair},        //
    {"Sign", LuaSign},                              //
    {"Verify", LuaVerify},                          //
    {"ListCurves", LuaListCurves},                  //
    {"ListHashAlgorithms", LuaListHashAlgorithms},  //
    {0}                                             //
};

int LuaECDSA(lua_State *L) {
  luaL_newlib(L, kLuaECDSA);
  return 1;
}
