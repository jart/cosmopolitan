/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╚──────────────────────────────────────────────────────────────────────────────╝
│                                                                              │
│  HACL*                                                                       │
│  Copyright (c) 2016-2022 INRIA, CMU and Microsoft Corporation                │
│  Copyright (c) 2022-2023 HACL* Contributors                                  │
│                                                                              │
│  Permission is hereby granted, free of charge, to any person obtaining       │
│  a copy of this software and associated documentation files (the             │
│  "Software"), to deal in the Software without restriction, including         │
│  without limitation the rights to use, copy, modify, merge, publish,         │
│  distribute, sublicense, and/or sell copies of the Software, and to          │
│  permit persons to whom the Software is furnished to do so, subject to       │
│  the following conditions:                                                   │
│                                                                              │
│  The above copyright notice and this permission notice shall be              │
│  included in all copies or substantial portions of the Software.             │
│                                                                              │
│  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,             │
│  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF          │
│  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.      │
│  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY        │
│  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,        │
│  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE           │
│  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                      │
│                                                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/serialize.h"
#include "libc/str/str.h"
#include "third_party/haclstar/haclstar.h"
#include "third_party/haclstar/int128.h"
#include "third_party/haclstar/loop.h"
#include "third_party/haclstar/macros.h"
#include "third_party/haclstar/sha2.h"
__static_yoink("hacl_star_notice");

typedef struct K___uint32_t_uint32_t_s
{
    uint32_t fst;
    uint32_t snd;
} K___uint32_t_uint32_t;

/**
  Write the HMAC-SHA-1 MAC of a message (`data`) by using a key (`key`)
  into `dst`.

  The key can be any length and will be hashed if it is longer and
  padded if it is shorter than 64 byte. `dst` must point to 20 bytes of
  memory.
*/
void
Hacl_HMAC_compute_sha1(uint8_t dst[20],
                       const uint8_t* key,
                       uint32_t key_len,
                       const void* data,
                       uint32_t data_len)
{
    uint8_t key_block[64U];
    memset(key_block, 0U, 64U * sizeof(uint8_t));
    uint8_t* nkey = key_block;
    uint32_t ite;
    if (key_len <= 64U) {
        ite = key_len;
    } else {
        ite = 20U;
    }
    uint8_t* zeroes = key_block + ite;
    (void)(zeroes);
    if (key_len <= 64U) {
        memcpy(nkey, key, key_len * sizeof(uint8_t));
    } else {
        Hacl_Hash_SHA1_hash(nkey, key, key_len);
    }
    uint8_t ipad[64U];
    memset(ipad, 0x36U, 64U * sizeof(uint8_t));
    for (uint32_t i = 0U; i < 64U; i++) {
        uint8_t xi = ipad[i];
        uint8_t yi = key_block[i];
        ipad[i] = (uint32_t)xi ^ (uint32_t)yi;
    }
    uint8_t opad[64U];
    memset(opad, 0x5cU, 64U * sizeof(uint8_t));
    for (uint32_t i = 0U; i < 64U; i++) {
        uint8_t xi = opad[i];
        uint8_t yi = key_block[i];
        opad[i] = (uint32_t)xi ^ (uint32_t)yi;
    }
    uint32_t s[5U] = {
        0x67452301U, 0xefcdab89U, 0x98badcfeU, 0x10325476U, 0xc3d2e1f0U
    };
    uint8_t* dst1 = ipad;
    if (data_len == 0U) {
        Hacl_Hash_SHA1_update_last(s, 0ULL, ipad, 64U);
    } else {
        uint32_t block_len = 64U;
        uint32_t n_blocks0 = data_len / block_len;
        uint32_t rem0 = data_len % block_len;
        K___uint32_t_uint32_t scrut;
        if (n_blocks0 > 0U && rem0 == 0U) {
            uint32_t n_blocks_ = n_blocks0 - 1U;
            scrut = ((K___uint32_t_uint32_t){
              .fst = n_blocks_, .snd = data_len - n_blocks_ * block_len });
        } else {
            scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks0, .snd = rem0 });
        }
        uint32_t n_blocks = scrut.fst;
        uint32_t rem_len = scrut.snd;
        uint32_t full_blocks_len = n_blocks * block_len;
        const uint8_t* full_blocks = data;
        const uint8_t* rem = (const uint8_t*)data + full_blocks_len;
        Hacl_Hash_SHA1_update_multi(s, ipad, 1U);
        Hacl_Hash_SHA1_update_multi(s, full_blocks, n_blocks);
        Hacl_Hash_SHA1_update_last(
          s, (uint64_t)64U + (uint64_t)full_blocks_len, rem, rem_len);
    }
    Hacl_Hash_SHA1_finish(s, dst1);
    uint8_t* hash1 = ipad;
    Hacl_Hash_SHA1_init(s);
    uint32_t block_len = 64U;
    uint32_t n_blocks0 = 20U / block_len;
    uint32_t rem0 = 20U % block_len;
    K___uint32_t_uint32_t scrut;
    if (n_blocks0 > 0U && rem0 == 0U) {
        uint32_t n_blocks_ = n_blocks0 - 1U;
        scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks_,
                                          .snd = 20U - n_blocks_ * block_len });
    } else {
        scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks0, .snd = rem0 });
    }
    uint32_t n_blocks = scrut.fst;
    uint32_t rem_len = scrut.snd;
    uint32_t full_blocks_len = n_blocks * block_len;
    uint8_t* full_blocks = hash1;
    uint8_t* rem = hash1 + full_blocks_len;
    Hacl_Hash_SHA1_update_multi(s, opad, 1U);
    Hacl_Hash_SHA1_update_multi(s, full_blocks, n_blocks);
    Hacl_Hash_SHA1_update_last(
      s, (uint64_t)64U + (uint64_t)full_blocks_len, rem, rem_len);
    Hacl_Hash_SHA1_finish(s, dst);
}

/**
  Write the HMAC-SHA-2-224 MAC of a message (`data`) by using a key
  (`key`) into `dst`.

  The key can be any length and will be hashed if it is longer and
  padded if it is shorter than 64 bytes. `dst` must point to 28 bytes of
  memory.
*/
void
Hacl_HMAC_compute_sha2_224(uint8_t dst[28],
                           const uint8_t* key,
                           uint32_t key_len,
                           const void* data,
                           uint32_t data_len)
{
    uint8_t key_block[64U];
    memset(key_block, 0U, 64U * sizeof(uint8_t));
    uint8_t* nkey = key_block;
    uint32_t ite;
    if (key_len <= 64U) {
        ite = key_len;
    } else {
        ite = 28U;
    }
    uint8_t* zeroes = key_block + ite;
    (void)(zeroes);
    if (key_len <= 64U) {
        memcpy(nkey, key, key_len * sizeof(uint8_t));
    } else {
        Hacl_Hash_SHA2_hash_224(nkey, key, key_len);
    }
    uint8_t ipad[64U];
    memset(ipad, 0x36U, 64U * sizeof(uint8_t));
    for (uint32_t i = 0U; i < 64U; i++) {
        uint8_t xi = ipad[i];
        uint8_t yi = key_block[i];
        ipad[i] = (uint32_t)xi ^ (uint32_t)yi;
    }
    uint8_t opad[64U];
    memset(opad, 0x5cU, 64U * sizeof(uint8_t));
    for (uint32_t i = 0U; i < 64U; i++) {
        uint8_t xi = opad[i];
        uint8_t yi = key_block[i];
        opad[i] = (uint32_t)xi ^ (uint32_t)yi;
    }
    uint32_t st[8U] = { 0U };
    KRML_MAYBE_FOR8(i, 0U, 8U, 1U, uint32_t* os = st;
                    uint32_t x = Hacl_Hash_SHA2_h224[i];
                    os[i] = x;);
    uint32_t* s = st;
    uint8_t* dst1 = ipad;
    if (data_len == 0U) {
        Hacl_Hash_SHA2_sha224_update_last(0ULL + (uint64_t)64U, 64U, ipad, s);
    } else {
        uint32_t block_len = 64U;
        uint32_t n_blocks0 = data_len / block_len;
        uint32_t rem0 = data_len % block_len;
        K___uint32_t_uint32_t scrut;
        if (n_blocks0 > 0U && rem0 == 0U) {
            uint32_t n_blocks_ = n_blocks0 - 1U;
            scrut = ((K___uint32_t_uint32_t){
              .fst = n_blocks_, .snd = data_len - n_blocks_ * block_len });
        } else {
            scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks0, .snd = rem0 });
        }
        uint32_t n_blocks = scrut.fst;
        uint32_t rem_len = scrut.snd;
        uint32_t full_blocks_len = n_blocks * block_len;
        const uint8_t* full_blocks = data;
        const uint8_t* rem = (const uint8_t*)data + full_blocks_len;
        Hacl_Hash_SHA2_sha224_update_nblocks(64U, ipad, s);
        Hacl_Hash_SHA2_sha224_update_nblocks(n_blocks * 64U, full_blocks, s);
        Hacl_Hash_SHA2_sha224_update_last(
          (uint64_t)64U + (uint64_t)full_blocks_len + (uint64_t)rem_len,
          rem_len,
          rem,
          s);
    }
    Hacl_Hash_SHA2_sha224_finish(s, dst1);
    uint8_t* hash1 = ipad;
    Hacl_Hash_SHA2_sha224_init(s);
    uint32_t block_len = 64U;
    uint32_t n_blocks0 = 28U / block_len;
    uint32_t rem0 = 28U % block_len;
    K___uint32_t_uint32_t scrut;
    if (n_blocks0 > 0U && rem0 == 0U) {
        uint32_t n_blocks_ = n_blocks0 - 1U;
        scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks_,
                                          .snd = 28U - n_blocks_ * block_len });
    } else {
        scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks0, .snd = rem0 });
    }
    uint32_t n_blocks = scrut.fst;
    uint32_t rem_len = scrut.snd;
    uint32_t full_blocks_len = n_blocks * block_len;
    uint8_t* full_blocks = hash1;
    uint8_t* rem = hash1 + full_blocks_len;
    Hacl_Hash_SHA2_sha224_update_nblocks(64U, opad, s);
    Hacl_Hash_SHA2_sha224_update_nblocks(n_blocks * 64U, full_blocks, s);
    Hacl_Hash_SHA2_sha224_update_last(
      (uint64_t)64U + (uint64_t)full_blocks_len + (uint64_t)rem_len,
      rem_len,
      rem,
      s);
    Hacl_Hash_SHA2_sha224_finish(s, dst);
}

/**
  Write the HMAC-SHA-2-256 MAC of a message (`data`) by using a key
  (`key`) into `dst`.

  The key can be any length and will be hashed if it is longer and
  padded if it is shorter than 64 bytes. `dst` must point to 32 bytes of
  memory.
*/
void
Hacl_HMAC_compute_sha2_256(uint8_t dst[32],
                           const uint8_t* key,
                           uint32_t key_len,
                           const void* data,
                           uint32_t data_len)
{
    uint8_t key_block[64U];
    memset(key_block, 0U, 64U * sizeof(uint8_t));
    uint8_t* nkey = key_block;
    uint32_t ite;
    if (key_len <= 64U) {
        ite = key_len;
    } else {
        ite = 32U;
    }
    uint8_t* zeroes = key_block + ite;
    (void)(zeroes);
    if (key_len <= 64U) {
        memcpy(nkey, key, key_len * sizeof(uint8_t));
    } else {
        Hacl_Hash_SHA2_hash_256(nkey, key, key_len);
    }
    uint8_t ipad[64U];
    memset(ipad, 0x36U, 64U * sizeof(uint8_t));
    for (uint32_t i = 0U; i < 64U; i++) {
        uint8_t xi = ipad[i];
        uint8_t yi = key_block[i];
        ipad[i] = (uint32_t)xi ^ (uint32_t)yi;
    }
    uint8_t opad[64U];
    memset(opad, 0x5cU, 64U * sizeof(uint8_t));
    for (uint32_t i = 0U; i < 64U; i++) {
        uint8_t xi = opad[i];
        uint8_t yi = key_block[i];
        opad[i] = (uint32_t)xi ^ (uint32_t)yi;
    }
    uint32_t st[8U] = { 0U };
    KRML_MAYBE_FOR8(i, 0U, 8U, 1U, uint32_t* os = st;
                    uint32_t x = Hacl_Hash_SHA2_h256[i];
                    os[i] = x;);
    uint32_t* s = st;
    uint8_t* dst1 = ipad;
    if (data_len == 0U) {
        Hacl_Hash_SHA2_sha256_update_last(0ULL + (uint64_t)64U, 64U, ipad, s);
    } else {
        uint32_t block_len = 64U;
        uint32_t n_blocks0 = data_len / block_len;
        uint32_t rem0 = data_len % block_len;
        K___uint32_t_uint32_t scrut;
        if (n_blocks0 > 0U && rem0 == 0U) {
            uint32_t n_blocks_ = n_blocks0 - 1U;
            scrut = ((K___uint32_t_uint32_t){
              .fst = n_blocks_, .snd = data_len - n_blocks_ * block_len });
        } else {
            scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks0, .snd = rem0 });
        }
        uint32_t n_blocks = scrut.fst;
        uint32_t rem_len = scrut.snd;
        uint32_t full_blocks_len = n_blocks * block_len;
        const uint8_t* full_blocks = data;
        const uint8_t* rem = (const uint8_t*)data + full_blocks_len;
        Hacl_Hash_SHA2_sha256_update_nblocks(64U, ipad, s);
        Hacl_Hash_SHA2_sha256_update_nblocks(n_blocks * 64U, full_blocks, s);
        Hacl_Hash_SHA2_sha256_update_last(
          (uint64_t)64U + (uint64_t)full_blocks_len + (uint64_t)rem_len,
          rem_len,
          rem,
          s);
    }
    Hacl_Hash_SHA2_sha256_finish(s, dst1);
    uint8_t* hash1 = ipad;
    Hacl_Hash_SHA2_sha256_init(s);
    uint32_t block_len = 64U;
    uint32_t n_blocks0 = 32U / block_len;
    uint32_t rem0 = 32U % block_len;
    K___uint32_t_uint32_t scrut;
    if (n_blocks0 > 0U && rem0 == 0U) {
        uint32_t n_blocks_ = n_blocks0 - 1U;
        scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks_,
                                          .snd = 32U - n_blocks_ * block_len });
    } else {
        scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks0, .snd = rem0 });
    }
    uint32_t n_blocks = scrut.fst;
    uint32_t rem_len = scrut.snd;
    uint32_t full_blocks_len = n_blocks * block_len;
    uint8_t* full_blocks = hash1;
    uint8_t* rem = hash1 + full_blocks_len;
    Hacl_Hash_SHA2_sha256_update_nblocks(64U, opad, s);
    Hacl_Hash_SHA2_sha256_update_nblocks(n_blocks * 64U, full_blocks, s);
    Hacl_Hash_SHA2_sha256_update_last(
      (uint64_t)64U + (uint64_t)full_blocks_len + (uint64_t)rem_len,
      rem_len,
      rem,
      s);
    Hacl_Hash_SHA2_sha256_finish(s, dst);
}

/**
  Write the HMAC-SHA-2-384 MAC of a message (`data`) by using a key
  (`key`) into `dst`.

  The key can be any length and will be hashed if it is longer and
  padded if it is shorter than 128 bytes. `dst` must point to 48 bytes
  of memory.
*/
void
Hacl_HMAC_compute_sha2_384(uint8_t dst[48],
                           const uint8_t* key,
                           uint32_t key_len,
                           const void* data,
                           uint32_t data_len)
{
    uint8_t key_block[128U];
    memset(key_block, 0U, 128U * sizeof(uint8_t));
    uint8_t* nkey = key_block;
    uint32_t ite;
    if (key_len <= 128U) {
        ite = key_len;
    } else {
        ite = 48U;
    }
    uint8_t* zeroes = key_block + ite;
    (void)(zeroes);
    if (key_len <= 128U) {
        memcpy(nkey, key, key_len * sizeof(uint8_t));
    } else {
        Hacl_Hash_SHA2_hash_384(nkey, key, key_len);
    }
    uint8_t ipad[128U];
    memset(ipad, 0x36U, 128U * sizeof(uint8_t));
    for (uint32_t i = 0U; i < 128U; i++) {
        uint8_t xi = ipad[i];
        uint8_t yi = key_block[i];
        ipad[i] = (uint32_t)xi ^ (uint32_t)yi;
    }
    uint8_t opad[128U];
    memset(opad, 0x5cU, 128U * sizeof(uint8_t));
    for (uint32_t i = 0U; i < 128U; i++) {
        uint8_t xi = opad[i];
        uint8_t yi = key_block[i];
        opad[i] = (uint32_t)xi ^ (uint32_t)yi;
    }
    uint64_t st[8U] = { 0U };
    KRML_MAYBE_FOR8(i, 0U, 8U, 1U, uint64_t* os = st;
                    uint64_t x = Hacl_Hash_SHA2_h384[i];
                    os[i] = x;);
    uint64_t* s = st;
    uint8_t* dst1 = ipad;
    if (data_len == 0U) {
        Hacl_Hash_SHA2_sha384_update_last(
          FStar_UInt128_add(FStar_UInt128_uint64_to_uint128(0ULL),
                            FStar_UInt128_uint64_to_uint128((uint64_t)128U)),
          128U,
          ipad,
          s);
    } else {
        uint32_t block_len = 128U;
        uint32_t n_blocks0 = data_len / block_len;
        uint32_t rem0 = data_len % block_len;
        K___uint32_t_uint32_t scrut;
        if (n_blocks0 > 0U && rem0 == 0U) {
            uint32_t n_blocks_ = n_blocks0 - 1U;
            scrut = ((K___uint32_t_uint32_t){
              .fst = n_blocks_, .snd = data_len - n_blocks_ * block_len });
        } else {
            scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks0, .snd = rem0 });
        }
        uint32_t n_blocks = scrut.fst;
        uint32_t rem_len = scrut.snd;
        uint32_t full_blocks_len = n_blocks * block_len;
        const uint8_t* full_blocks = data;
        const uint8_t* rem = (const uint8_t*)data + full_blocks_len;
        Hacl_Hash_SHA2_sha384_update_nblocks(128U, ipad, s);
        Hacl_Hash_SHA2_sha384_update_nblocks(n_blocks * 128U, full_blocks, s);
        Hacl_Hash_SHA2_sha384_update_last(
          FStar_UInt128_add(
            FStar_UInt128_add(
              FStar_UInt128_uint64_to_uint128((uint64_t)128U),
              FStar_UInt128_uint64_to_uint128((uint64_t)full_blocks_len)),
            FStar_UInt128_uint64_to_uint128((uint64_t)rem_len)),
          rem_len,
          rem,
          s);
    }
    Hacl_Hash_SHA2_sha384_finish(s, dst1);
    uint8_t* hash1 = ipad;
    Hacl_Hash_SHA2_sha384_init(s);
    uint32_t block_len = 128U;
    uint32_t n_blocks0 = 48U / block_len;
    uint32_t rem0 = 48U % block_len;
    K___uint32_t_uint32_t scrut;
    if (n_blocks0 > 0U && rem0 == 0U) {
        uint32_t n_blocks_ = n_blocks0 - 1U;
        scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks_,
                                          .snd = 48U - n_blocks_ * block_len });
    } else {
        scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks0, .snd = rem0 });
    }
    uint32_t n_blocks = scrut.fst;
    uint32_t rem_len = scrut.snd;
    uint32_t full_blocks_len = n_blocks * block_len;
    uint8_t* full_blocks = hash1;
    uint8_t* rem = hash1 + full_blocks_len;
    Hacl_Hash_SHA2_sha384_update_nblocks(128U, opad, s);
    Hacl_Hash_SHA2_sha384_update_nblocks(n_blocks * 128U, full_blocks, s);
    Hacl_Hash_SHA2_sha384_update_last(
      FStar_UInt128_add(
        FStar_UInt128_add(
          FStar_UInt128_uint64_to_uint128((uint64_t)128U),
          FStar_UInt128_uint64_to_uint128((uint64_t)full_blocks_len)),
        FStar_UInt128_uint64_to_uint128((uint64_t)rem_len)),
      rem_len,
      rem,
      s);
    Hacl_Hash_SHA2_sha384_finish(s, dst);
}

/**
  Write the HMAC-SHA-2-512 MAC of a message (`data`) by using a key
  (`key`) into `dst`.

  The key can be any length and will be hashed if it is longer and
  padded if it is shorter than 128 bytes. `dst` must point to 64 bytes
  of memory.
*/
void
Hacl_HMAC_compute_sha2_512(uint8_t dst[64],
                           const uint8_t* key,
                           uint32_t key_len,
                           const void* data,
                           uint32_t data_len)
{
    uint8_t key_block[128U];
    memset(key_block, 0U, 128U * sizeof(uint8_t));
    uint8_t* nkey = key_block;
    uint32_t ite;
    if (key_len <= 128U) {
        ite = key_len;
    } else {
        ite = 64U;
    }
    uint8_t* zeroes = key_block + ite;
    (void)(zeroes);
    if (key_len <= 128U) {
        memcpy(nkey, key, key_len * sizeof(uint8_t));
    } else {
        Hacl_Hash_SHA2_hash_512(nkey, key, key_len);
    }
    uint8_t ipad[128U];
    memset(ipad, 0x36U, 128U * sizeof(uint8_t));
    for (uint32_t i = 0U; i < 128U; i++) {
        uint8_t xi = ipad[i];
        uint8_t yi = key_block[i];
        ipad[i] = (uint32_t)xi ^ (uint32_t)yi;
    }
    uint8_t opad[128U];
    memset(opad, 0x5cU, 128U * sizeof(uint8_t));
    for (uint32_t i = 0U; i < 128U; i++) {
        uint8_t xi = opad[i];
        uint8_t yi = key_block[i];
        opad[i] = (uint32_t)xi ^ (uint32_t)yi;
    }
    uint64_t st[8U] = { 0U };
    KRML_MAYBE_FOR8(i, 0U, 8U, 1U, uint64_t* os = st;
                    uint64_t x = Hacl_Hash_SHA2_h512[i];
                    os[i] = x;);
    uint64_t* s = st;
    uint8_t* dst1 = ipad;
    if (data_len == 0U) {
        Hacl_Hash_SHA2_sha512_update_last(
          FStar_UInt128_add(FStar_UInt128_uint64_to_uint128(0ULL),
                            FStar_UInt128_uint64_to_uint128((uint64_t)128U)),
          128U,
          ipad,
          s);
    } else {
        uint32_t block_len = 128U;
        uint32_t n_blocks0 = data_len / block_len;
        uint32_t rem0 = data_len % block_len;
        K___uint32_t_uint32_t scrut;
        if (n_blocks0 > 0U && rem0 == 0U) {
            uint32_t n_blocks_ = n_blocks0 - 1U;
            scrut = ((K___uint32_t_uint32_t){
              .fst = n_blocks_, .snd = data_len - n_blocks_ * block_len });
        } else {
            scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks0, .snd = rem0 });
        }
        uint32_t n_blocks = scrut.fst;
        uint32_t rem_len = scrut.snd;
        uint32_t full_blocks_len = n_blocks * block_len;
        const uint8_t* full_blocks = data;
        const uint8_t* rem = (const uint8_t*)data + full_blocks_len;
        Hacl_Hash_SHA2_sha512_update_nblocks(128U, ipad, s);
        Hacl_Hash_SHA2_sha512_update_nblocks(n_blocks * 128U, full_blocks, s);
        Hacl_Hash_SHA2_sha512_update_last(
          FStar_UInt128_add(
            FStar_UInt128_add(
              FStar_UInt128_uint64_to_uint128((uint64_t)128U),
              FStar_UInt128_uint64_to_uint128((uint64_t)full_blocks_len)),
            FStar_UInt128_uint64_to_uint128((uint64_t)rem_len)),
          rem_len,
          rem,
          s);
    }
    Hacl_Hash_SHA2_sha512_finish(s, dst1);
    uint8_t* hash1 = ipad;
    Hacl_Hash_SHA2_sha512_init(s);
    uint32_t block_len = 128U;
    uint32_t n_blocks0 = 64U / block_len;
    uint32_t rem0 = 64U % block_len;
    K___uint32_t_uint32_t scrut;
    if (n_blocks0 > 0U && rem0 == 0U) {
        uint32_t n_blocks_ = n_blocks0 - 1U;
        scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks_,
                                          .snd = 64U - n_blocks_ * block_len });
    } else {
        scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks0, .snd = rem0 });
    }
    uint32_t n_blocks = scrut.fst;
    uint32_t rem_len = scrut.snd;
    uint32_t full_blocks_len = n_blocks * block_len;
    uint8_t* full_blocks = hash1;
    uint8_t* rem = hash1 + full_blocks_len;
    Hacl_Hash_SHA2_sha512_update_nblocks(128U, opad, s);
    Hacl_Hash_SHA2_sha512_update_nblocks(n_blocks * 128U, full_blocks, s);
    Hacl_Hash_SHA2_sha512_update_last(
      FStar_UInt128_add(
        FStar_UInt128_add(
          FStar_UInt128_uint64_to_uint128((uint64_t)128U),
          FStar_UInt128_uint64_to_uint128((uint64_t)full_blocks_len)),
        FStar_UInt128_uint64_to_uint128((uint64_t)rem_len)),
      rem_len,
      rem,
      s);
    Hacl_Hash_SHA2_sha512_finish(s, dst);
}

/**
  Write the HMAC-BLAKE2b MAC of a message (`data`) by using a key
  (`key`) into `dst`.

  The key can be any length and will be hashed if it is longer and
  padded if it is shorter than 128 bytes. `dst` must point to 64 bytes
  of memory.
*/
void
Hacl_HMAC_compute_blake2b_32(uint8_t dst[64],
                             const uint8_t* key,
                             uint32_t key_len,
                             const void* data,
                             uint32_t data_len)
{
    uint8_t key_block[128U];
    memset(key_block, 0U, 128U * sizeof(uint8_t));
    uint8_t* nkey = key_block;
    uint32_t ite;
    if (key_len <= 128U) {
        ite = key_len;
    } else {
        ite = 64U;
    }
    uint8_t* zeroes = key_block + ite;
    (void)(zeroes);
    if (key_len <= 128U) {
        memcpy(nkey, key, key_len * sizeof(uint8_t));
    } else {
        Hacl_Hash_Blake2b_hash_with_key(nkey, 64U, key, key_len, NULL, 0U);
    }
    uint8_t ipad[128U];
    memset(ipad, 0x36U, 128U * sizeof(uint8_t));
    for (uint32_t i = 0U; i < 128U; i++) {
        uint8_t xi = ipad[i];
        uint8_t yi = key_block[i];
        ipad[i] = (uint32_t)xi ^ (uint32_t)yi;
    }
    uint8_t opad[128U];
    memset(opad, 0x5cU, 128U * sizeof(uint8_t));
    for (uint32_t i = 0U; i < 128U; i++) {
        uint8_t xi = opad[i];
        uint8_t yi = key_block[i];
        opad[i] = (uint32_t)xi ^ (uint32_t)yi;
    }
    uint64_t s[16U] = { 0U };
    Hacl_Hash_Blake2b_init(s, 0U, 64U);
    uint64_t* s0 = s;
    uint8_t* dst1 = ipad;
    if (data_len == 0U) {
        uint64_t wv[16U] = { 0U };
        Hacl_Hash_Blake2b_update_last(128U,
                                      wv,
                                      s0,
                                      false,
                                      FStar_UInt128_uint64_to_uint128(0ULL),
                                      128U,
                                      ipad);
    } else {
        uint32_t block_len = 128U;
        uint32_t n_blocks0 = data_len / block_len;
        uint32_t rem0 = data_len % block_len;
        K___uint32_t_uint32_t scrut;
        if (n_blocks0 > 0U && rem0 == 0U) {
            uint32_t n_blocks_ = n_blocks0 - 1U;
            scrut = ((K___uint32_t_uint32_t){
              .fst = n_blocks_, .snd = data_len - n_blocks_ * block_len });
        } else {
            scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks0, .snd = rem0 });
        }
        uint32_t n_blocks = scrut.fst;
        uint32_t rem_len = scrut.snd;
        uint32_t full_blocks_len = n_blocks * block_len;
        const uint8_t* full_blocks = data;
        const uint8_t* rem = (const uint8_t*)data + full_blocks_len;
        uint64_t wv[16U] = { 0U };
        Hacl_Hash_Blake2b_update_multi(
          128U, wv, s0, FStar_UInt128_uint64_to_uint128(0ULL), ipad, 1U);
        uint64_t wv0[16U] = { 0U };
        Hacl_Hash_Blake2b_update_multi(
          n_blocks * 128U,
          wv0,
          s0,
          FStar_UInt128_uint64_to_uint128((uint64_t)block_len),
          full_blocks,
          n_blocks);
        uint64_t wv1[16U] = { 0U };
        Hacl_Hash_Blake2b_update_last(
          rem_len,
          wv1,
          s0,
          false,
          FStar_UInt128_add(
            FStar_UInt128_uint64_to_uint128((uint64_t)128U),
            FStar_UInt128_uint64_to_uint128((uint64_t)full_blocks_len)),
          rem_len,
          rem);
    }
    Hacl_Hash_Blake2b_finish(64U, dst1, s0);
    uint8_t* hash1 = ipad;
    Hacl_Hash_Blake2b_init(s0, 0U, 64U);
    uint32_t block_len = 128U;
    uint32_t n_blocks0 = 64U / block_len;
    uint32_t rem0 = 64U % block_len;
    K___uint32_t_uint32_t scrut;
    if (n_blocks0 > 0U && rem0 == 0U) {
        uint32_t n_blocks_ = n_blocks0 - 1U;
        scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks_,
                                          .snd = 64U - n_blocks_ * block_len });
    } else {
        scrut = ((K___uint32_t_uint32_t){ .fst = n_blocks0, .snd = rem0 });
    }
    uint32_t n_blocks = scrut.fst;
    uint32_t rem_len = scrut.snd;
    uint32_t full_blocks_len = n_blocks * block_len;
    uint8_t* full_blocks = hash1;
    uint8_t* rem = hash1 + full_blocks_len;
    uint64_t wv[16U] = { 0U };
    Hacl_Hash_Blake2b_update_multi(
      128U, wv, s0, FStar_UInt128_uint64_to_uint128(0ULL), opad, 1U);
    uint64_t wv0[16U] = { 0U };
    Hacl_Hash_Blake2b_update_multi(
      n_blocks * 128U,
      wv0,
      s0,
      FStar_UInt128_uint64_to_uint128((uint64_t)block_len),
      full_blocks,
      n_blocks);
    uint64_t wv1[16U] = { 0U };
    Hacl_Hash_Blake2b_update_last(
      rem_len,
      wv1,
      s0,
      false,
      FStar_UInt128_add(
        FStar_UInt128_uint64_to_uint128((uint64_t)128U),
        FStar_UInt128_uint64_to_uint128((uint64_t)full_blocks_len)),
      rem_len,
      rem);
    Hacl_Hash_Blake2b_finish(64U, dst, s0);
}
