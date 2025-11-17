#ifndef COSMOPOLITAN_THIRD_PARTY_HACLSTAR_HACLSTAR_H_
#define COSMOPOLITAN_THIRD_PARTY_HACLSTAR_HACLSTAR_H_
COSMOPOLITAN_C_START_
/* clang-format off */

typedef unsigned __int128 FStar_UInt128_uint128;

typedef struct {
  uint32_t size;
  const void *data;
} Hacl_Slice;

typedef struct {
  uint8_t ctx_key[32];
  uint8_t ctx_nonce[12];
  uint64_t ctx_seq;
  uint8_t ctx_exporter[32];
} Hacl_Impl_HPKE_context_s;

uint32_t Hacl_HPKE_Curve25519_CP128_SHA256_setupBaseR(Hacl_Impl_HPKE_context_s*, const uint8_t[32], const uint8_t[32], uint32_t, const void *);
uint32_t Hacl_HPKE_Curve25519_CP128_SHA256_setupBaseS(uint8_t[32], Hacl_Impl_HPKE_context_s*, const uint8_t[32], const uint8_t[32], uint32_t, const void *);
bool Hacl_HPKE_getNonce(Hacl_Impl_HPKE_context_s*, uint8_t[12]);

uint32_t Hacl_AEAD_Chacha20Poly1305_decrypt(uint8_t*, const uint8_t*, uint32_t, const uint8_t*, uint32_t, const uint8_t[32], const uint8_t[12], const uint8_t[16]);
void Hacl_AEAD_Chacha20Poly1305_encrypt(uint8_t*, uint8_t[16], const uint8_t*, uint32_t, const uint8_t*, uint32_t, const uint8_t[32], const uint8_t[12]);

void Hacl_Ed25519_secret_to_public(uint8_t[32], const uint8_t[32]);
void Hacl_Ed25519_expand_keys(uint8_t[96], const uint8_t[32]);
void Hacl_Ed25519_sign_expanded(uint8_t[64], const uint8_t[96], uint32_t, const Hacl_Slice*);
void Hacl_Ed25519_sign(uint8_t[64], const uint8_t[32], uint32_t, const Hacl_Slice*);
bool Hacl_Ed25519_verify(const uint8_t[32], uint32_t, const Hacl_Slice*, const uint8_t[64]);

void Hacl_Curve25519_secret_to_public(uint8_t[32], const uint8_t[32]);
bool Hacl_Curve25519_ecdh(uint8_t[32], const uint8_t[32], const uint8_t[32]);
void Hacl_Curve25519_scalarmult(uint8_t[32], const uint8_t[32], const uint8_t[32]);
void Hacl_Curve25519_fsquare_times(uint64_t[5], const uint64_t[5], FStar_UInt128_uint128*, uint32_t);
void Hacl_Curve25519_finv(uint64_t[5], const uint64_t[5], FStar_UInt128_uint128*);

#define Hacl_Streaming_Types_Success 0
#define Hacl_Streaming_Types_InvalidAlgorithm 1
#define Hacl_Streaming_Types_InvalidLength 2
#define Hacl_Streaming_Types_MaximumLengthExceeded 3
#define Hacl_Streaming_Types_OutOfMemory 4
typedef uint8_t Hacl_Streaming_Types_error_code;

typedef struct Hacl_Streaming_MD_state_32_s Hacl_Streaming_MD_state_32;
void Hacl_Hash_SHA1_hash(uint8_t[20], const void*, uint32_t);
Hacl_Streaming_MD_state_32* Hacl_Hash_SHA1_malloc(void);
Hacl_Streaming_MD_state_32* Hacl_Hash_SHA1_copy(Hacl_Streaming_MD_state_32*);
Hacl_Streaming_Types_error_code Hacl_Hash_SHA1_update(Hacl_Streaming_MD_state_32*, const void*, uint32_t);
void Hacl_Hash_SHA1_digest(const Hacl_Streaming_MD_state_32*, uint8_t[20]);
void Hacl_Hash_SHA1_reset(Hacl_Streaming_MD_state_32*);
void Hacl_Hash_SHA1_free(Hacl_Streaming_MD_state_32*);
void Hacl_Hash_SHA1_init(uint32_t[5]);
void Hacl_Hash_SHA1_update_multi(uint32_t[5], const uint8_t*, uint32_t);
void Hacl_Hash_SHA1_update_last(uint32_t[5], uint64_t, const uint8_t*, uint32_t);
void Hacl_Hash_SHA1_finish(const uint32_t[5], uint8_t[20]);

typedef struct Hacl_Streaming_MD_state_64_s Hacl_Streaming_MD_state_64;
void Hacl_Hash_SHA2_hash_224(uint8_t[28], const void*, uint32_t);
void Hacl_Hash_SHA2_hash_256(uint8_t[32], const void*, uint32_t);
void Hacl_Hash_SHA2_hash_384(uint8_t[48], const void*, uint32_t);
void Hacl_Hash_SHA2_hash_512(uint8_t[64], const void*, uint32_t);
Hacl_Streaming_MD_state_32* Hacl_Hash_SHA2_malloc_224(void);
Hacl_Streaming_MD_state_32* Hacl_Hash_SHA2_malloc_256(void);
Hacl_Streaming_MD_state_64* Hacl_Hash_SHA2_malloc_384(void);
Hacl_Streaming_MD_state_64* Hacl_Hash_SHA2_malloc_512(void);
Hacl_Streaming_MD_state_32* Hacl_Hash_SHA2_copy_256(const Hacl_Streaming_MD_state_32*);
Hacl_Streaming_MD_state_64* Hacl_Hash_SHA2_copy_512(const Hacl_Streaming_MD_state_64*);
Hacl_Streaming_Types_error_code Hacl_Hash_SHA2_update_224(Hacl_Streaming_MD_state_32*, const void*, uint32_t);
Hacl_Streaming_Types_error_code Hacl_Hash_SHA2_update_256(Hacl_Streaming_MD_state_32*, const void*, uint32_t);
Hacl_Streaming_Types_error_code Hacl_Hash_SHA2_update_384(Hacl_Streaming_MD_state_64*, const void*, uint32_t);
Hacl_Streaming_Types_error_code Hacl_Hash_SHA2_update_512(Hacl_Streaming_MD_state_64*, const void*, uint32_t);
void Hacl_Hash_SHA2_reset_224(Hacl_Streaming_MD_state_32*);
void Hacl_Hash_SHA2_reset_256(Hacl_Streaming_MD_state_32*);
void Hacl_Hash_SHA2_reset_384(Hacl_Streaming_MD_state_64*);
void Hacl_Hash_SHA2_reset_512(Hacl_Streaming_MD_state_64*);
void Hacl_Hash_SHA2_digest_224(Hacl_Streaming_MD_state_32*, uint8_t[28]);
void Hacl_Hash_SHA2_digest_256(Hacl_Streaming_MD_state_32*, uint8_t[32]);
void Hacl_Hash_SHA2_digest_384(Hacl_Streaming_MD_state_64*, uint8_t[48]);
void Hacl_Hash_SHA2_digest_512(Hacl_Streaming_MD_state_64*, uint8_t[64]);
void Hacl_Hash_SHA2_free_224(Hacl_Streaming_MD_state_32*);
void Hacl_Hash_SHA2_free_256(Hacl_Streaming_MD_state_32*);
void Hacl_Hash_SHA2_free_384(Hacl_Streaming_MD_state_64*);
void Hacl_Hash_SHA2_free_512(Hacl_Streaming_MD_state_64*);
void Hacl_Hash_SHA2_sha224_init(uint32_t[8]);
void Hacl_Hash_SHA2_sha256_init(uint32_t[8]);
void Hacl_Hash_SHA2_sha384_init(uint64_t[8]);
void Hacl_Hash_SHA2_sha512_init(uint64_t[8]);
void Hacl_Hash_SHA2_sha224_update_nblocks(uint32_t, const uint8_t*, uint32_t[8]);
void Hacl_Hash_SHA2_sha256_update_nblocks(uint32_t, const uint8_t*, uint32_t[8]);
void Hacl_Hash_SHA2_sha384_update_nblocks(uint32_t, const uint8_t*, uint64_t[8]);
void Hacl_Hash_SHA2_sha512_update_nblocks(uint32_t, const uint8_t*, uint64_t[8]);
void Hacl_Hash_SHA2_sha224_update_last(uint64_t, uint32_t, const uint8_t*, uint32_t[8]);
void Hacl_Hash_SHA2_sha256_update_last(uint64_t, uint32_t, const uint8_t*, uint32_t[8]);
void Hacl_Hash_SHA2_sha384_update_last(FStar_UInt128_uint128, uint32_t, const uint8_t*, uint64_t[8]);
void Hacl_Hash_SHA2_sha512_update_last(FStar_UInt128_uint128, uint32_t, const uint8_t*, uint64_t[8]);
void Hacl_Hash_SHA2_sha224_finish(const uint32_t[8], uint8_t[28]);
void Hacl_Hash_SHA2_sha256_finish(const uint32_t[8], uint8_t[32]);
void Hacl_Hash_SHA2_sha384_finish(const uint64_t[8], uint8_t[48]);
void Hacl_Hash_SHA2_sha512_finish(const uint64_t[8], uint8_t[64]);

typedef struct Hacl_Hash_Blake2b_blake2_params_s {
  uint8_t digest_length;
  uint8_t key_length;
  uint8_t fanout;
  uint8_t depth;
  uint32_t leaf_length;
  uint64_t node_offset;
  uint8_t node_depth;
  uint8_t inner_length;
  uint8_t *salt;
  uint8_t *personal;
} Hacl_Hash_Blake2b_blake2_params;

typedef struct Hacl_Hash_Blake2b_index_s {
  uint8_t key_length;
  uint8_t digest_length;
  bool last_node;
} Hacl_Hash_Blake2b_index;

typedef struct Hacl_Hash_Blake2b_block_state_t_s Hacl_Hash_Blake2b_block_state_t;
typedef struct Hacl_Hash_Blake2b_state_t_s Hacl_Hash_Blake2b_state_t;
void Hacl_Hash_Blake2b_hash_with_key(uint8_t*, uint32_t, const void*, uint32_t, const uint8_t*, uint32_t);
void Hacl_Hash_Blake2b_hash_with_key_and_params(uint8_t*, const void*, uint32_t, Hacl_Hash_Blake2b_blake2_params, uint8_t*);
Hacl_Hash_Blake2b_state_t* Hacl_Hash_Blake2b_malloc(void);
Hacl_Hash_Blake2b_state_t* Hacl_Hash_Blake2b_malloc_256(void);
Hacl_Hash_Blake2b_state_t* Hacl_Hash_Blake2b_malloc_with_key(uint8_t*, uint8_t);
Hacl_Hash_Blake2b_state_t* Hacl_Hash_Blake2b_malloc_with_params_and_key(Hacl_Hash_Blake2b_blake2_params*, bool, uint8_t*);
Hacl_Hash_Blake2b_state_t* Hacl_Hash_Blake2b_copy(Hacl_Hash_Blake2b_state_t*);
Hacl_Streaming_Types_error_code Hacl_Hash_Blake2b_update(Hacl_Hash_Blake2b_state_t*, const void*, uint32_t);
Hacl_Hash_Blake2b_index Hacl_Hash_Blake2b_info(const Hacl_Hash_Blake2b_state_t*);
uint8_t Hacl_Hash_Blake2b_digest(const Hacl_Hash_Blake2b_state_t*, uint8_t*);
void Hacl_Hash_Blake2b_reset(Hacl_Hash_Blake2b_state_t*);
void Hacl_Hash_Blake2b_reset_with_key(Hacl_Hash_Blake2b_state_t*, uint8_t*);
void Hacl_Hash_Blake2b_reset_with_key_and_params(Hacl_Hash_Blake2b_state_t*, Hacl_Hash_Blake2b_blake2_params*, uint8_t*);
void Hacl_Hash_Blake2b_free(Hacl_Hash_Blake2b_state_t*);
void Hacl_Hash_Blake2b_init(uint64_t[16], uint32_t, uint32_t);
void Hacl_Hash_Blake2b_update_multi(uint32_t, uint64_t[16], uint64_t[16], FStar_UInt128_uint128, const uint8_t*, uint32_t);
void Hacl_Hash_Blake2b_update_last(uint32_t, uint64_t[16], uint64_t[16], bool, FStar_UInt128_uint128, uint32_t, const uint8_t*);
void Hacl_Hash_Blake2b_finish(uint32_t, uint8_t*, const uint64_t[16]);

void Hacl_Chacha20_chacha20_encrypt(uint32_t, uint8_t*, const uint8_t*, const uint8_t[32], const uint8_t[12], uint32_t);
void Hacl_Chacha20_chacha20_decrypt(uint32_t, uint8_t*, const uint8_t*, const uint8_t[32], const uint8_t[12], uint32_t);
void Hacl_Impl_Chacha20_chacha20_init(uint32_t[16], const uint8_t[32], const uint8_t[12], uint32_t);
void Hacl_Impl_Chacha20_chacha20_update(uint32_t[16], uint32_t, uint8_t*, const uint8_t*);

typedef struct Hacl_MAC_Poly1305_state_t_s Hacl_MAC_Poly1305_state_t;
Hacl_MAC_Poly1305_state_t* Hacl_MAC_Poly1305_malloc(const uint8_t[32]);
void Hacl_MAC_Poly1305_reset(Hacl_MAC_Poly1305_state_t*, const uint8_t[32]);
Hacl_Streaming_Types_error_code Hacl_MAC_Poly1305_update(Hacl_MAC_Poly1305_state_t*, uint8_t*, uint32_t);
void Hacl_MAC_Poly1305_digest(Hacl_MAC_Poly1305_state_t*, uint8_t[16]);
void Hacl_MAC_Poly1305_free(Hacl_MAC_Poly1305_state_t*);
void Hacl_MAC_Poly1305_mac(uint8_t[16], uint8_t*, uint32_t, uint8_t[32]);
void Hacl_MAC_Poly1305_poly1305_init(uint64_t[25], const uint8_t[32]);
void Hacl_MAC_Poly1305_poly1305_finish(uint8_t[16], const uint8_t[32], uint64_t[25]);

void Hacl_HKDF_expand_sha2_256(uint8_t*, uint8_t*, uint32_t, uint8_t*, uint32_t, uint32_t);
void Hacl_HKDF_expand_sha2_384(uint8_t*, uint8_t*, uint32_t, uint8_t*, uint32_t, uint32_t);
void Hacl_HKDF_expand_sha2_512(uint8_t*, uint8_t*, uint32_t, uint8_t*, uint32_t, uint32_t);
void Hacl_HKDF_extract_sha2_256(uint8_t*, uint8_t*, uint32_t, uint8_t*, uint32_t);
void Hacl_HKDF_extract_sha2_384(uint8_t*, uint8_t*, uint32_t, uint8_t*, uint32_t);
void Hacl_HKDF_extract_sha2_512(uint8_t*, uint8_t*, uint32_t, uint8_t*, uint32_t);

void Hacl_HMAC_compute_sha1(uint8_t[20], const uint8_t*, uint32_t, const void*, uint32_t);
void Hacl_HMAC_compute_sha2_224(uint8_t[28], const uint8_t*, uint32_t, const void*, uint32_t);
void Hacl_HMAC_compute_sha2_256(uint8_t[32], const uint8_t*, uint32_t, const void*, uint32_t);
void Hacl_HMAC_compute_sha2_384(uint8_t[48], const uint8_t*, uint32_t, const void*, uint32_t);
void Hacl_HMAC_compute_sha2_512(uint8_t[64], const uint8_t*, uint32_t, const void*, uint32_t);
void Hacl_HMAC_compute_blake2b_32(uint8_t[64], const uint8_t*, uint32_t, const void*, uint32_t);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_HACLSTAR_HACLSTAR_H_ */
