#include "libc/log/log.h"
#include "net/https/https.h"
#include "third_party/lua/lauxlib.h"
#include "third_party/mbedtls/error.h"
#include "third_party/mbedtls/pk.h"
#include "third_party/mbedtls/rsa.h"
#include "third_party/mbedtls/ecdsa.h"
#include "third_party/mbedtls/x509_csr.h"
#include "third_party/mbedtls/oid.h"
#include "third_party/mbedtls/md.h"
#include "third_party/mbedtls/base64.h"
#include "third_party/mbedtls/aes.h"
#include "third_party/mbedtls/ctr_drbg.h"
#include "third_party/mbedtls/entropy.h"
#include "third_party/mbedtls/gcm.h"

// Standard C library and redbean utilities
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "tool/net/luacheck.h"

// Parse PEM keys and convert them into JWK format
static int LuaConvertPemToJwk(lua_State *L) {
    const char *pem_key = luaL_checkstring(L, 1);

    mbedtls_pk_context key;
    mbedtls_pk_init(&key);
    int ret;

    // Parse the PEM key
    if ((ret = mbedtls_pk_parse_key(&key, (const unsigned char *)pem_key, strlen(pem_key) + 1, NULL, 0)) != 0 &&
        (ret = mbedtls_pk_parse_public_key(&key, (const unsigned char *)pem_key, strlen(pem_key) + 1)) != 0) {
        lua_pushnil(L);
        lua_pushfstring(L, "Failed to parse PEM key: -0x%04x", -ret);
        mbedtls_pk_free(&key);
        return 2;
    }

    lua_newtable(L); // Create a new Lua table

    if (mbedtls_pk_get_type(&key) == MBEDTLS_PK_RSA) {
        // Handle RSA keys
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

        if (!n_b64 || !e_b64) {
            lua_pushnil(L);
            lua_pushstring(L, "Memory allocation failed");
            free(n);
            free(e);
            free(n_b64);
            free(e_b64);
            mbedtls_pk_free(&key);
            return 2;
        }

        mbedtls_base64_encode((unsigned char *)n_b64, n_b64_len, &n_b64_len, n, n_len);
        mbedtls_base64_encode((unsigned char *)e_b64, e_b64_len, &e_b64_len, e, e_len);

        n_b64[n_b64_len] = '\0';
        e_b64[e_b64_len] = '\0';

        lua_pushstring(L, "RSA");
        lua_setfield(L, -2, "kty");
        lua_pushstring(L, n_b64);
        lua_setfield(L, -2, "n");
        lua_pushstring(L, e_b64);
        lua_setfield(L, -2, "e");

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

        if (!x_b64 || !y_b64) {
            lua_pushnil(L);
            lua_pushstring(L, "Memory allocation failed");
            free(x);
            free(y);
            free(x_b64);
            free(y_b64);
            mbedtls_pk_free(&key);
            return 2;
        }

        mbedtls_base64_encode((unsigned char *)x_b64, x_b64_len, &x_b64_len, x, x_len);
        mbedtls_base64_encode((unsigned char *)y_b64, y_b64_len, &y_b64_len, y, y_len);

        x_b64[x_b64_len] = '\0';
        y_b64[y_b64_len] = '\0';

        lua_pushstring(L, "EC");
        lua_setfield(L, -2, "kty");
        lua_pushstring(L, mbedtls_ecp_curve_info_from_grp_id(ec->grp.id)->name);
        lua_setfield(L, -2, "crv");
        lua_pushstring(L, x_b64);
        lua_setfield(L, -2, "x");
        lua_pushstring(L, y_b64);
        lua_setfield(L, -2, "y");

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
    return 1;
}

// CSR Creation Function
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

    if ((ret = mbedtls_pk_parse_key(&key, (const unsigned char *)key_pem, strlen(key_pem) + 1, NULL, 0)) != 0) {
        lua_pushnil(L);
        lua_pushfstring(L, "Failed to parse key: %d", ret);
        return 2;
    }

    mbedtls_x509write_csr_set_subject_name(&req, subject_name);
    mbedtls_x509write_csr_set_key(&req, &key);
    mbedtls_x509write_csr_set_md_alg(&req, MBEDTLS_MD_SHA256);

    if (san_list) {
        if ((ret = mbedtls_x509write_csr_set_extension(&req, MBEDTLS_OID_SUBJECT_ALT_NAME, MBEDTLS_OID_SIZE(MBEDTLS_OID_SUBJECT_ALT_NAME), (const unsigned char *)san_list, strlen(san_list))) != 0) {
            lua_pushnil(L);
            lua_pushfstring(L, "Failed to set SANs: %d", ret);
            return 2;
        }
    }

    if ((ret = mbedtls_x509write_csr_pem(&req, (unsigned char *)buf, sizeof(buf), NULL, NULL)) < 0) {
        lua_pushnil(L);
        lua_pushfstring(L, "Failed to write CSR: %d", ret);
        return 2;
    }

    lua_pushstring(L, buf);

    mbedtls_pk_free(&key);
    mbedtls_x509write_csr_free(&req);

    return 1;
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
 * Lua function signature: RSAGenerateKeyPair([key_length])
 * @param L Lua state
 * @return 2 on success (private_key, public_key), 2 on failure (nil,
 * error_message)
 */
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

    char *private_key, *public_key;
    size_t private_len, public_len;

    // Call the C function to generate the key pair
    if (!RSAGenerateKeyPair(&private_key, &private_len, &public_key, &public_len, bits)) {
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


static char *RSAEncrypt(const char *public_key_pem, const unsigned char *data,
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
static int LuaRSAEncrypt(lua_State *L) {
  const char *public_key = luaL_checkstring(L, 1);
  size_t data_len;
  const unsigned char *data =
      (const unsigned char *)luaL_checklstring(L, 2, &data_len);
  size_t out_len;

  char *encrypted = RSAEncrypt(public_key, data, data_len, &out_len);
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
static int LuaRSADecrypt(lua_State *L) {
  const char *private_key = luaL_checkstring(L, 1);
  size_t encrypted_len;
  const unsigned char *encrypted_data =
      (const unsigned char *)luaL_checklstring(L, 2, &encrypted_len);
  size_t out_len;

  char *decrypted =
      RSADecrypt(private_key, encrypted_data, encrypted_len, &out_len);
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
static int LuaRSASign(lua_State *L) {
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

static int RSAVerify(const char *public_key_pem, const unsigned char *data,
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
static int LuaRSAVerify(lua_State *L) {
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
  result = RSAVerify(key_pem, (const unsigned char *)msg, msg_len,
                  (const unsigned char *)signature, sig_len, hash_algo_str);

  // Return boolean result (0 means valid signature)
  lua_pushboolean(L, result == 0);

  return 1;
}

// Elliptic Curve Cryptography Functions
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
static int LuaECDSAGenerateKeyPair(lua_State *L) {
  const char *curve_name = NULL;
  char *priv_key_pem = NULL;
  char *pub_key_pem = NULL;

  // Check if curve name is provided
  if (lua_gettop(L) >= 1 && !lua_isnil(L, 1)) {
    curve_name = luaL_checkstring(L, 1);
  }
  // If not provided, generate_key_pem will use the default

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
static int LuaECDSASign(lua_State *L) {
  // Correct order: priv_key, message, hash_name (default sha256)
  const char *priv_key_pem = luaL_checkstring(L, 1);
  const char *message = luaL_checkstring(L, 2);
  const char *hash_name = luaL_optstring(L, 3, "sha256");

  hash_algorithm_t hash_alg = string_to_hash_alg(hash_name);

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
static int LuaECDSAVerify(lua_State *L) {
  // Correct order: pub_key, message, signature, hash_name (default sha256)
  const char *pub_key_pem = luaL_checkstring(L, 1);
  const char *message = luaL_checkstring(L, 2);
  size_t sig_len;
  const unsigned char *signature = (const unsigned char *)luaL_checklstring(L, 3, &sig_len);
  const char *hash_name = luaL_optstring(L, 4, "sha256");

  hash_algorithm_t hash_alg = string_to_hash_alg(hash_name);

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
    if ((keybits != 128 && keybits != 192 && keybits != 256) || (keylen != 16 && keylen != 24 && keylen != 32)) {
        lua_pushnil(L);
        lua_pushstring(L, "AES key length must be 128, 192, or 256 bits");
        return 2;
    }
    unsigned char key[32];
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    const char *pers = "aes_keygen";
    int ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char *)pers, strlen(pers));
    if (ret != 0) {
        lua_pushnil(L);
        lua_pushstring(L, "Failed to initialize RNG for AES key");
        mbedtls_ctr_drbg_free(&ctr_drbg);
        mbedtls_entropy_free(&entropy);
        return 2;
    }
    ret = mbedtls_ctr_drbg_random(&ctr_drbg, key, keylen);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
    if (ret != 0) {
        lua_pushnil(L);
        lua_pushstring(L, "Failed to generate random AES key");
        return 2;
    }
    lua_pushlstring(L, (const char *)key, keylen);
    return 1;
}

// AES encryption supporting CBC, GCM, and CTR modes
static int LuaAesEncrypt(lua_State *L) {
    // Accept IV as the 3rd argument (after key, plaintext)
    size_t keylen, ivlen = 0, ptlen;
    const unsigned char *key = (const unsigned char *)luaL_checklstring(L, 1, &keylen);
    const unsigned char *plaintext = (const unsigned char *)luaL_checklstring(L, 2, &ptlen);
    const unsigned char *iv = NULL;
    unsigned char *gen_iv = NULL;
    int iv_was_generated = 0;

    const char *mode = luaL_optstring(L, 4, "cbc"); // Default to CBC if not provided
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

    // If IV is not provided (arg3 is nil or missing), auto-generate
    if (lua_isnoneornil(L, 3)) {
        // For GCM, standard is 12 bytes, but allow 12-16
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
        mbedtls_entropy_context entropy;
        mbedtls_ctr_drbg_context ctr_drbg;
        mbedtls_entropy_init(&entropy);
        mbedtls_ctr_drbg_init(&ctr_drbg);
        mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, NULL, 0);
        mbedtls_ctr_drbg_random(&ctr_drbg, gen_iv, ivlen);
        mbedtls_ctr_drbg_free(&ctr_drbg);
        mbedtls_entropy_free(&entropy);
        iv = gen_iv;
        iv_was_generated = 1;
    } else {
        // IV provided
        iv = (const unsigned char *)luaL_checklstring(L, 3, &ivlen);
        // Do not force ivlen to 16 here! Accept actual length for GCM (12-16)
        if (is_cbc || is_ctr) {
            if (ivlen != 16) {
                lua_pushnil(L);
                lua_pushstring(L, "AES IV must be 16 bytes for CBC/CTR");
                return 2;
            }
        } else if (is_gcm) {
            if (ivlen < 12 || ivlen > 16) {
                lua_pushnil(L);
                lua_pushstring(L, "AES GCM IV/nonce must be 12-16 bytes");
                return 2;
            }
        }
        iv_was_generated = 0;
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
        ret = mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT, ctlen, iv_copy, input, output);
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
        if (iv_was_generated) free(gen_iv);
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
        ret = mbedtls_aes_crypt_ctr(&aes, ptlen, &nc_off, nonce_counter, stream_block, plaintext, output);
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
        if (iv_was_generated) free(gen_iv);
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
        // Use actual ivlen, not hardcoded 16
        ret = mbedtls_gcm_crypt_and_tag(&gcm, MBEDTLS_GCM_ENCRYPT, ptlen, iv, ivlen, NULL, 0, plaintext, output, taglen, tag);
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
        if (iv_was_generated) free(gen_iv);
        return 3;
    }
    lua_pushnil(L);
    lua_pushstring(L, "Internal error in AES encrypt");
    return 2;
}

// AES decryption supporting CBC, GCM, and CTR modes
static int LuaAesDecrypt(lua_State *L) {
    size_t keylen, ctlen, ivlen;
    const unsigned char *key = (const unsigned char *)luaL_checklstring(L, 1, &keylen);
    const unsigned char *ciphertext = (const unsigned char *)luaL_checklstring(L, 2, &ctlen);
    const unsigned char *iv = (const unsigned char *)luaL_checklstring(L, 3, &ivlen);
    const char *mode = luaL_optstring(L, 4, "cbc"); // Default to CBC if not provided
    const unsigned char *aad = NULL;
    const unsigned char *tag = NULL;
    size_t aadlen = 0, taglen = 0;
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
        if (!lua_isnoneornil(L, 5)) {
            aad = (const unsigned char *)luaL_checklstring(L, 5, &aadlen);
        }
        if (!lua_isnoneornil(L, 6)) {
            tag = (const unsigned char *)luaL_checklstring(L, 6, &taglen);
            if (taglen < 12 || taglen > 16) {
                lua_pushnil(L);
                lua_pushstring(L, "AES GCM tag must be 12-16 bytes");
                return 2;
            }
        } else {
            lua_pushnil(L);
            lua_pushstring(L, "AES GCM tag required as 6th argument");
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
        ret = mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, ctlen, iv_copy, ciphertext, output);
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
        ret = mbedtls_aes_crypt_ctr(&aes, ctlen, &nc_off, nonce_counter, stream_block, ciphertext, output);
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
        ret = mbedtls_gcm_auth_decrypt(&gcm, ctlen, iv, ivlen, aad, aadlen, tag, taglen, ciphertext, output);
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

// LuaCrypto compatible API
static int LuaCryptoSign(lua_State *L) {
    const char *dtype = luaL_checkstring(L, 1); // Type of signature (e.g., "rsa", "ecdsa")
    lua_remove(L, 1); // Remove the first argument (key type or cipher type) before dispatching

    if (strcasecmp(dtype, "rsa") == 0) {
        return LuaRSASign(L);
    } else if (strcasecmp(dtype, "ecdsa") == 0) {
        return LuaECDSASign(L);
    } else {
        return luaL_error(L, "Unsupported signature type: %s", dtype);
    }
}

static int LuaCryptoVerify(lua_State *L) {
    const char *dtype = luaL_checkstring(L, 1); // Type of signature (e.g., "rsa", "ecdsa")
    lua_remove(L, 1); // Remove the first argument (key type or cipher type) before dispatching

    if (strcasecmp(dtype, "rsa") == 0) {
        return LuaRSAVerify(L);
    } else if (strcasecmp(dtype, "ecdsa") == 0) {
        return LuaECDSAVerify(L);
    } else {
        return luaL_error(L, "Unsupported signature type: %s", dtype);
    }
}

static int LuaCryptoEncrypt(lua_State *L) {
    const char *cipher = luaL_checkstring(L, 1); // Cipher type (e.g., "rsa", "aes")
    lua_remove(L, 1); // Remove the first argument (key type or cipher type) before dispatching

    if (strcasecmp(cipher, "rsa") == 0) {
        return LuaRSAEncrypt(L);
    } else if (strcasecmp(cipher, "aes") == 0) {
        return LuaAesEncrypt(L);
    } else {
        return luaL_error(L, "Unsupported cipher type: %s", cipher);
    }
}

static int LuaCryptoDecrypt(lua_State *L) {
    const char *cipher = luaL_checkstring(L, 1); // Cipher type (e.g., "rsa", "aes")
    lua_remove(L, 1); // Remove the first argument (key type or cipher type) before dispatching

    if (strcasecmp(cipher, "rsa") == 0) {
        return LuaRSADecrypt(L);
    } else if (strcasecmp(cipher, "aes") == 0) {
        return LuaAesDecrypt(L);
    } else {
        return luaL_error(L, "Unsupported cipher type: %s", cipher);
    }
}

static int LuaCryptoGenerateKeyPair(lua_State *L) {
    // If the first argument is a number, treat as RSA key length
    if (lua_gettop(L) >= 1 && lua_type(L, 1) == LUA_TNUMBER) {
        // Call LuaRSAGenerateKeyPair with the number as the key length
        return LuaRSAGenerateKeyPair(L);
    }
    // Otherwise, get the key type from the first argument, default to "rsa" if not provided
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
  {"sign", LuaCryptoSign},                       //
  {"verify", LuaCryptoVerify},                   //
  {"encrypt", LuaCryptoEncrypt},                 //
  {"decrypt", LuaCryptoDecrypt},                 //
  {"generatekeypair", LuaCryptoGenerateKeyPair}, //
  {"convertPemToJwk", LuaConvertPemToJwk},       // 
  {"generateCsr", LuaGenerateCSR},               //
  {0},                                           //
};

int LuaCrypto(lua_State *L) {
  luaL_newlib(L, kLuaCrypto);
  return 1;
}
