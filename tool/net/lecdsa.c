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
#include "third_party/mbedtls/pk.h"
#include "third_party/mbedtls/sha256.h"

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
                unsigned char **signature, size_t *sig_len) {
  mbedtls_pk_context key;
  unsigned char hash[32];  // SHA-256 hash
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

  mbedtls_pk_init(&key);

  // Parse the private key from PEM directly without creating a copy
  ret = mbedtls_pk_parse_key(&key, (const unsigned char *)priv_key_pem,
                             key_len + 1, NULL, 0);

  if (ret != 0) {
    WARNF("(ecdsa) Failed to parse private key: -0x%04x", -ret);
    goto cleanup;
  }

  // Compute SHA-256 hash of the message
  ret = mbedtls_sha256_ret((const unsigned char *)message, strlen(message),
                           hash, 0);
  if (ret != 0) {
    WARNF("(ecdsa) Failed to compute message hash: -0x%04x", -ret);
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
  ret = mbedtls_pk_sign(&key, MBEDTLS_MD_SHA256, hash, sizeof(hash), *signature,
                        sig_len, GenerateHardRandom, 0);

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
}
// Lua binding for signing a message
static int LuaSign(lua_State *L) {
  const char *priv_key_pem = luaL_checkstring(L, 1);
  const char *message = luaL_checkstring(L, 2);

  unsigned char *signature = NULL;
  size_t sig_len = 0;

  int ret = Sign(priv_key_pem, message, &signature, &sig_len);

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
                  const unsigned char *signature, size_t sig_len) {
  mbedtls_pk_context key;
  unsigned char hash[32];  // SHA-256 hash
  int ret;

  mbedtls_pk_init(&key);

  // Parse the public key from PEM
  ret = mbedtls_pk_parse_public_key(&key, (const unsigned char *)pub_key_pem,
                                    strlen(pub_key_pem) + 1);
  if (ret != 0) {
    WARNF("(ecdsa) Failed to parse public key: -0x%04x", -ret);
    goto cleanup;
  }

  // Compute SHA-256 hash of the message
  ret = mbedtls_sha256_ret((const unsigned char *)message, strlen(message),
                           hash, 0);
  if (ret != 0) {
    WARNF("(ecdsa) Failed to compute message hash: -0x%04x", -ret);
    goto cleanup;
  }

  // Verify the signature
  ret = mbedtls_pk_verify(&key, MBEDTLS_MD_SHA256, hash, sizeof(hash),
                          signature, sig_len);
  if (ret != 0) {
    WARNF("(ecdsa) Signature verification failed: -0x%04x", -ret);
    goto cleanup;
  }

cleanup:
  mbedtls_pk_free(&key);
  return ret;
}
// Lua binding for verifying a signature
static int LuaVerify(lua_State *L) {
  const char *pub_key_pem = luaL_checkstring(L, 1);
  const char *message = luaL_checkstring(L, 2);
  size_t sig_len;
  const unsigned char *signature =
      (const unsigned char *)luaL_checklstring(L, 3, &sig_len);

  int ret = Verify(pub_key_pem, message, signature, sig_len);

  lua_pushboolean(L, ret == 0);
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

// Register functions
static const luaL_Reg kLuaECDSA[] = {
    {"GenerateKeyPair", LuaGenerateKeyPair},  //
    {"Sign", LuaSign},                        //
    {"Verify", LuaVerify},                    //
    {"ListCurves", LuaListCurves},            //
    {0}                                       //
};

int LuaECDSA(lua_State *L) {
  luaL_newlib(L, kLuaECDSA);
  return 1;
}
