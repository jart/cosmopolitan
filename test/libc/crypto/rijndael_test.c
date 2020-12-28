/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/crypto/rijndael.h"
#include "libc/dce.h"
#include "libc/fmt/bing.internal.h"
#include "libc/runtime/internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/prot.h"
#include "libc/testlib/testlib.h"

/**
 * Test vectors published by:
 *
 *   Morris Dworkin
 *   National Institute of Standards and Technology
 *   Recommendation for Block Cipher Modes of Operation: Methods and Techniques
 *   SP 800-38A (DOI)
 *   December 2001
 */

FIXTURE(rijndael, disableHardwareExtensions) {
  memset((/*unconst*/ void *)kCpuids, 0, sizeof(kCpuids));
}

/**
 * F.1.1: ECB-AES128.Encrypt
 *
 * Key          2b7e151628aed2a6abf7158809cf4f3c
 *
 * Block No. 1
 * Plaintext    6bc1bee22e409f96e93d7e117393172a
 * Input Block  6bc1bee22e409f96e93d7e117393172a
 * Output Block 3ad77bb40d7a3660a89ecaf32466ef97
 * Ciphertext   3ad77bb40d7a3660a89ecaf32466ef97
 *
 * Block No. 2
 * Plaintext    ae2d8a571e03ac9c9eb76fac45af8e51
 * Input Block  ae2d8a571e03ac9c9eb76fac45af8e51
 * Output Block f5d3d58503b9699de785895a96fdbaaf
 * Ciphertext   f5d3d58503b9699de785895a96fdbaaf
 *
 * Block No. 3
 * Plaintext    30c81c46a35ce411e5fbc1191a0a52ef
 * Input Block  30c81c46a35ce411e5fbc1191a0a52ef
 * Output Block 43b1cd7f598ece23881b00e3ed030688
 * Ciphertext   43b1cd7f598ece23881b00e3ed030688
 *
 * Block No. 4
 * Plaintext    f69f2445df4f9b17ad2b417be66c3710
 * Input Block  f69f2445df4f9b17ad2b417be66c3710
 * Output Block 7b0c785e27e8ad3f8223207104725dd4
 * Ciphertext   7b0c785e27e8ad3f8223207104725dd4
 */
TEST(aes128, testNistEcbRijndael) {
  struct Rijndael ctx;
  aes_block_t k1, block;
  unhexbuf(&k1, 16, "2b7e151628aed2a6abf7158809cf4f3c");
  rijndaelinit(&ctx, 10, k1, k1);
  unhexbuf(&block, 16, "6bc1bee22e409f96e93d7e117393172a");
  block = rijndael(10, block, &ctx);
  EXPECT_BINEQ("3ad77bb40d7a3660a89ecaf32466ef97", &block);
  unhexbuf(&block, 16, "ae2d8a571e03ac9c9eb76fac45af8e51");
  block = rijndael(10, block, &ctx);
  EXPECT_BINEQ("f5d3d58503b9699de785895a96fdbaaf", &block);
  unhexbuf(&block, 16, "30c81c46a35ce411e5fbc1191a0a52ef");
  block = rijndael(10, block, &ctx);
  EXPECT_BINEQ("43b1cd7f598ece23881b00e3ed030688", &block);
  unhexbuf(&block, 16, "f69f2445df4f9b17ad2b417be66c3710");
  block = rijndael(10, block, &ctx);
  EXPECT_BINEQ("7b0c785e27e8ad3f8223207104725dd4", &block);
}

/**
 * F.1.2: ECB-AES128.Decrypt
 *
 * Key          2b7e151628aed2a6abf7158809cf4f3c
 *
 * Block No. 1
 * Plaintext    3ad77bb40d7a3660a89ecaf32466ef97
 * Input Block  3ad77bb40d7a3660a89ecaf32466ef97
 * Output Block 6bc1bee22e409f96e93d7e117393172a
 * Ciphertext   6bc1bee22e409f96e93d7e117393172a
 *
 * Block No. 2
 * Plaintext    f5d3d58503b9699de785895a96fdbaaf
 * Input Block  f5d3d58503b9699de785895a96fdbaaf
 * Output Block ae2d8a571e03ac9c9eb76fac45af8e51
 * Ciphertext   ae2d8a571e03ac9c9eb76fac45af8e51
 *
 * Block No. 3
 * Plaintext    43b1cd7f598ece23881b00e3ed030688
 * Input Block  43b1cd7f598ece23881b00e3ed030688
 * Output Block 30c81c46a35ce411e5fbc1191a0a52ef
 * Ciphertext   30c81c46a35ce411e5fbc1191a0a52ef
 *
 * Block No. 4
 * Plaintext    7b0c785e27e8ad3f8223207104725dd4
 * Input Block  7b0c785e27e8ad3f8223207104725dd4
 * Output Block f69f2445df4f9b17ad2b417be66c3710
 * Ciphertext   f69f2445df4f9b17ad2b417be66c3710
 */
TEST(aes128, testNistEcbUnrijndael) {
  struct Rijndael ctx;
  aes_block_t k1, block;
  unhexbuf(&k1, 16, "2b7e151628aed2a6abf7158809cf4f3c");
  unrijndaelinit(&ctx, 10, k1, k1);
  unhexbuf(&block, 16, "3ad77bb40d7a3660a89ecaf32466ef97");
  block = unrijndael(10, block, &ctx);
  EXPECT_BINEQ("6bc1bee22e409f96e93d7e117393172a", &block);
  unhexbuf(&block, 16, "f5d3d58503b9699de785895a96fdbaaf");
  block = unrijndael(10, block, &ctx);
  EXPECT_BINEQ("ae2d8a571e03ac9c9eb76fac45af8e51", &block);
  unhexbuf(&block, 16, "43b1cd7f598ece23881b00e3ed030688");
  block = unrijndael(10, block, &ctx);
  EXPECT_BINEQ("30c81c46a35ce411e5fbc1191a0a52ef", &block);
  unhexbuf(&block, 16, "7b0c785e27e8ad3f8223207104725dd4");
  block = unrijndael(10, block, &ctx);
  EXPECT_BINEQ("f69f2445df4f9b17ad2b417be66c3710", &block);
}

/**
 * F.1.3: ECB-AES192.Encrypt
 *
 * Key          8e73b0f7da0e6452c810f32b809079e5
 *              62f8ead2522c6b7b
 *
 * Block No. 1
 * Plaintext    6bc1bee22e409f96e93d7e117393172a
 * Input Block  6bc1bee22e409f96e93d7e117393172a
 * Output Block bd334f1d6e45f25ff712a214571fa5cc
 * Ciphertext   bd334f1d6e45f25ff712a214571fa5cc
 *
 * Block No. 2
 * Plaintext    ae2d8a571e03ac9c9eb76fac45af8e51
 * Input Block  ae2d8a571e03ac9c9eb76fac45af8e51
 * Output Block 974104846d0ad3ad7734ecb3ecee4eef
 * Ciphertext   974104846d0ad3ad7734ecb3ecee4eef
 *
 * Block No. 3
 * Plaintext    30c81c46a35ce411e5fbc1191a0a52ef
 * Input Block  30c81c46a35ce411e5fbc1191a0a52ef
 * Output Block ef7afd2270e2e60adce0ba2face6444e
 * Ciphertext   ef7afd2270e2e60adce0ba2face6444e
 *
 * Block No. 4
 * Plaintext    f69f2445df4f9b17ad2b417be66c3710
 * Input Block  f69f2445df4f9b17ad2b417be66c3710
 * Output Block 9a4b41ba738d6c72fb16691603c18e0e
 * Ciphertext   9a4b41ba738d6c72fb16691603c18e0e
 */
TEST(aes192, testNistEcbRijndael) {
  struct Rijndael ctx;
  aes_block_t k1, k2, block;
  unhexbuf(&k1, 16, "8e73b0f7da0e6452c810f32b809079e5");
  unhexbuf(&k2, 16, "62f8ead2522c6b7bDEADBEEFFEEDFACE");
  rijndaelinit(&ctx, 12, k1, k2);
  unhexbuf(&block, 16, "6bc1bee22e409f96e93d7e117393172a");
  block = rijndael(12, block, &ctx);
  EXPECT_BINEQ("bd334f1d6e45f25ff712a214571fa5cc", &block);
  unhexbuf(&block, 16, "ae2d8a571e03ac9c9eb76fac45af8e51");
  block = rijndael(12, block, &ctx);
  EXPECT_BINEQ("974104846d0ad3ad7734ecb3ecee4eef", &block);
  unhexbuf(&block, 16, "30c81c46a35ce411e5fbc1191a0a52ef");
  block = rijndael(12, block, &ctx);
  EXPECT_BINEQ("ef7afd2270e2e60adce0ba2face6444e", &block);
  unhexbuf(&block, 16, "f69f2445df4f9b17ad2b417be66c3710");
  block = rijndael(12, block, &ctx);
  EXPECT_BINEQ("9a4b41ba738d6c72fb16691603c18e0e", &block);
}

/**
 * F.1.4: ECB-AES192.Decrypt
 *
 * Key          8e73b0f7da0e6452c810f32b809079e5
 *              62f8ead2522c6b7b
 *
 * Block No. 1
 * Plaintext    bd334f1d6e45f25ff712a214571fa5cc
 * Input Block  bd334f1d6e45f25ff712a214571fa5cc
 * Output Block 6bc1bee22e409f96e93d7e117393172a
 * Ciphertext   6bc1bee22e409f96e93d7e117393172a
 *
 * Block No. 2
 * Plaintext    974104846d0ad3ad7734ecb3ecee4eef
 * Input Block  974104846d0ad3ad7734ecb3ecee4eef
 * Output Block ae2d8a571e03ac9c9eb76fac45af8e51
 * Ciphertext   ae2d8a571e03ac9c9eb76fac45af8e51
 *
 * Block No. 3
 * Plaintext    ef7afd2270e2e60adce0ba2face6444e
 * Input Block  ef7afd2270e2e60adce0ba2face6444e
 * Output Block 30c81c46a35ce411e5fbc1191a0a52ef
 * Ciphertext   30c81c46a35ce411e5fbc1191a0a52ef
 *
 * Block No. 4
 * Plaintext    9a4b41ba738d6c72fb16691603c18e0e
 * Input Block  9a4b41ba738d6c72fb16691603c18e0e
 * Output Block f69f2445df4f9b17ad2b417be66c3710
 * Ciphertext   f69f2445df4f9b17ad2b417be66c3710
 */
TEST(aes192, testNistEcbUnrijndael) {
  struct Rijndael ctx;
  aes_block_t k1, k2, block;
  unhexbuf(&k1, 16, "8e73b0f7da0e6452c810f32b809079e5");
  unhexbuf(&k2, 16, "62f8ead2522c6b7bDEADBEEFFEEDFACE");
  unrijndaelinit(&ctx, 12, k1, k2);
  unhexbuf(&block, 16, "bd334f1d6e45f25ff712a214571fa5cc");
  block = unrijndael(12, block, &ctx);
  EXPECT_BINEQ("6bc1bee22e409f96e93d7e117393172a", &block);
  unhexbuf(&block, 16, "974104846d0ad3ad7734ecb3ecee4eef");
  block = unrijndael(12, block, &ctx);
  EXPECT_BINEQ("ae2d8a571e03ac9c9eb76fac45af8e51", &block);
  unhexbuf(&block, 16, "ef7afd2270e2e60adce0ba2face6444e");
  block = unrijndael(12, block, &ctx);
  EXPECT_BINEQ("30c81c46a35ce411e5fbc1191a0a52ef", &block);
  unhexbuf(&block, 16, "9a4b41ba738d6c72fb16691603c18e0e");
  block = unrijndael(12, block, &ctx);
  EXPECT_BINEQ("f69f2445df4f9b17ad2b417be66c3710", &block);
}

/**
 * F.1.5: ECB-AES256.Encrypt
 *
 * Key          603deb1015ca71be2b73aef0857d7781
 *              1f352c073b6108d72d9810a30914dff4
 *
 * Block No. 1
 * Plaintext    6bc1bee22e409f96e93d7e117393172a
 * Input Block  6bc1bee22e409f96e93d7e117393172a
 * Output Block f3eed1bdb5d2a03c064b5a7e3db181f8
 * Ciphertext   f3eed1bdb5d2a03c064b5a7e3db181f8
 *
 * Block No. 2
 * Plaintext    ae2d8a571e03ac9c9eb76fac45af8e51
 * Input Block  ae2d8a571e03ac9c9eb76fac45af8e51
 * Output Block 591ccb10d410ed26dc5ba74a31362870
 * Ciphertext   591ccb10d410ed26dc5ba74a31362870
 *
 * Block No. 3
 * Plaintext    30c81c46a35ce411e5fbc1191a0a52ef
 * Input Block  30c81c46a35ce411e5fbc1191a0a52ef
 * Output Block b6ed21b99ca6f4f9f153e7b1beafed1d
 * Ciphertext   b6ed21b99ca6f4f9f153e7b1beafed1d
 *
 * Block No. 4
 * Plaintext    f69f2445df4f9b17ad2b417be66c3710
 * Input Block  f69f2445df4f9b17ad2b417be66c3710
 * Output Block 23304b7a39f9f3ff067d8d8f9e24ecc7
 * Ciphertext   23304b7a39f9f3ff067d8d8f9e24ecc7
 */
TEST(aes256, testNistEcbRijndael) {
  struct Rijndael ctx;
  aes_block_t k1, k2, block;
  unhexbuf(&k1, 16, "603deb1015ca71be2b73aef0857d7781");
  unhexbuf(&k2, 16, "1f352c073b6108d72d9810a30914dff4");
  rijndaelinit(&ctx, 14, k1, k2);
  unhexbuf(&block, 16, "6bc1bee22e409f96e93d7e117393172a");
  block = rijndael(14, block, &ctx);
  EXPECT_BINEQ("f3eed1bdb5d2a03c064b5a7e3db181f8", &block);
  unhexbuf(&block, 16, "ae2d8a571e03ac9c9eb76fac45af8e51");
  block = rijndael(14, block, &ctx);
  EXPECT_BINEQ("591ccb10d410ed26dc5ba74a31362870", &block);
  unhexbuf(&block, 16, "30c81c46a35ce411e5fbc1191a0a52ef");
  block = rijndael(14, block, &ctx);
  EXPECT_BINEQ("b6ed21b99ca6f4f9f153e7b1beafed1d", &block);
  unhexbuf(&block, 16, "f69f2445df4f9b17ad2b417be66c3710");
  block = rijndael(14, block, &ctx);
  EXPECT_BINEQ("23304b7a39f9f3ff067d8d8f9e24ecc7", &block);
}

/**
 * F.1.6: ECB-AES256.Decrypt
 *
 * Key          603deb1015ca71be2b73aef0857d7781
 *              1f352c073b6108d72d9810a30914dff4
 *
 * Block No. 1
 * Input Block  f3eed1bdb5d2a03c064b5a7e3db181f8
 * Plaintext    f3eed1bdb5d2a03c064b5a7e3db181f8
 * Ciphertext   6bc1bee22e409f96e93d7e117393172a
 * Output Block 6bc1bee22e409f96e93d7e117393172a
 *
 * Block No. 2
 * Input Block  591ccb10d410ed26dc5ba74a31362870
 * Plaintext    591ccb10d410ed26dc5ba74a31362870
 * Ciphertext   ae2d8a571e03ac9c9eb76fac45af8e51
 * Output Block ae2d8a571e03ac9c9eb76fac45af8e51
 *
 * Block No. 3
 * Input Block  b6ed21b99ca6f4f9f153e7b1beafed1d
 * Plaintext    b6ed21b99ca6f4f9f153e7b1beafed1d
 * Ciphertext   30c81c46a35ce411e5fbc1191a0a52ef
 * Output Block 30c81c46a35ce411e5fbc1191a0a52ef
 *
 * Block No. 4
 * Input Block  23304b7a39f9f3ff067d8d8f9e24ecc7
 * Plaintext    23304b7a39f9f3ff067d8d8f9e24ecc7
 * Ciphertext   f69f2445df4f9b17ad2b417be66c3710
 * Output Block f69f2445df4f9b17ad2b417be66c3710
 */
TEST(aes256, testNistEcbUnrijndael) {
  struct Rijndael ctx;
  aes_block_t k1, k2, block;
  unhexbuf(&k1, 16, "603deb1015ca71be2b73aef0857d7781");
  unhexbuf(&k2, 16, "1f352c073b6108d72d9810a30914dff4");
  unrijndaelinit(&ctx, 14, k1, k2);
  unhexbuf(&block, 16, "f3eed1bdb5d2a03c064b5a7e3db181f8");
  block = unrijndael(14, block, &ctx);
  EXPECT_BINEQ("6bc1bee22e409f96e93d7e117393172a", &block);
  unhexbuf(&block, 16, "591ccb10d410ed26dc5ba74a31362870");
  block = unrijndael(14, block, &ctx);
  EXPECT_BINEQ("ae2d8a571e03ac9c9eb76fac45af8e51", &block);
  unhexbuf(&block, 16, "b6ed21b99ca6f4f9f153e7b1beafed1d");
  block = unrijndael(14, block, &ctx);
  EXPECT_BINEQ("30c81c46a35ce411e5fbc1191a0a52ef", &block);
  unhexbuf(&block, 16, "23304b7a39f9f3ff067d8d8f9e24ecc7");
  block = unrijndael(14, block, &ctx);
  EXPECT_BINEQ("f69f2445df4f9b17ad2b417be66c3710", &block);
}
