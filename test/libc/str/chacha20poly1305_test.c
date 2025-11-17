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
#include "libc/calls/calls.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/weirdtypes.h"
#include "libc/cosmotime.h"
#include "libc/inttypes.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/stdlib.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/exit.h"
#include "libc/testlib/testlib.h"
#include "third_party/haclstar/haclstar.h"
__static_yoink("hacl_star_notice");

#define ROUNDS 3000
#define SIZE   16384

typedef struct {
  uint8_t* input;
  size_t input_len;
  uint8_t key[32];
  uint8_t nonce[12];
  uint8_t* aad;
  size_t aad_len;
  uint8_t tag[16];
  uint8_t* cipher;
} chacha20poly1305_test_vector;

static uint8_t input1[114] = {
    0x4c, 0x61, 0x64, 0x69, 0x65, 0x73, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x47,
    0x65, 0x6e, 0x74, 0x6c, 0x65, 0x6d, 0x65, 0x6e, 0x20, 0x6f, 0x66, 0x20,
    0x74, 0x68, 0x65, 0x20, 0x63, 0x6c, 0x61, 0x73, 0x73, 0x20, 0x6f, 0x66,
    0x20, 0x27, 0x39, 0x39, 0x3a, 0x20, 0x49, 0x66, 0x20, 0x49, 0x20, 0x63,
    0x6f, 0x75, 0x6c, 0x64, 0x20, 0x6f, 0x66, 0x66, 0x65, 0x72, 0x20, 0x79,
    0x6f, 0x75, 0x20, 0x6f, 0x6e, 0x6c, 0x79, 0x20, 0x6f, 0x6e, 0x65, 0x20,
    0x74, 0x69, 0x70, 0x20, 0x66, 0x6f, 0x72, 0x20, 0x74, 0x68, 0x65, 0x20,
    0x66, 0x75, 0x74, 0x75, 0x72, 0x65, 0x2c, 0x20, 0x73, 0x75, 0x6e, 0x73,
    0x63, 0x72, 0x65, 0x65, 0x6e, 0x20, 0x77, 0x6f, 0x75, 0x6c, 0x64, 0x20,
    0x62, 0x65, 0x20, 0x69, 0x74, 0x2e};

static uint8_t cipher1[114] = {
    0xd3, 0x1a, 0x8d, 0x34, 0x64, 0x8e, 0x60, 0xdb, 0x7b, 0x86, 0xaf, 0xbc,
    0x53, 0xef, 0x7e, 0xc2, 0xa4, 0xad, 0xed, 0x51, 0x29, 0x6e, 0x08, 0xfe,
    0xa9, 0xe2, 0xb5, 0xa7, 0x36, 0xee, 0x62, 0xd6, 0x3d, 0xbe, 0xa4, 0x5e,
    0x8c, 0xa9, 0x67, 0x12, 0x82, 0xfa, 0xfb, 0x69, 0xda, 0x92, 0x72, 0x8b,
    0x1a, 0x71, 0xde, 0x0a, 0x9e, 0x06, 0x0b, 0x29, 0x05, 0xd6, 0xa5, 0xb6,
    0x7e, 0xcd, 0x3b, 0x36, 0x92, 0xdd, 0xbd, 0x7f, 0x2d, 0x77, 0x8b, 0x8c,
    0x98, 0x03, 0xae, 0xe3, 0x28, 0x09, 0x1b, 0x58, 0xfa, 0xb3, 0x24, 0xe4,
    0xfa, 0xd6, 0x75, 0x94, 0x55, 0x85, 0x80, 0x8b, 0x48, 0x31, 0xd7, 0xbc,
    0x3f, 0xf4, 0xde, 0xf0, 0x8e, 0x4b, 0x7a, 0x9d, 0xe5, 0x76, 0xd2, 0x65,
    0x86, 0xce, 0xc6, 0x4b, 0x61, 0x16};

static uint8_t aad1[12] = {0x50, 0x51, 0x52, 0x53, 0xc0, 0xc1,
                           0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7};

static chacha20poly1305_test_vector vectors[] = {
    {.input = input1,
     .input_len = sizeof(input1) / sizeof(uint8_t),
     .key = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a,
             0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95,
             0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f},
     .nonce = {0x07, 0x00, 0x00, 0x00, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46,
               0x47},
     .aad = aad1,
     .aad_len = sizeof(aad1) / sizeof(uint8_t),
     .tag = {0x1a, 0xe1, 0x0b, 0x59, 0x4f, 0x09, 0xe2, 0x6a, 0x7e, 0x90, 0x2e,
             0xcb, 0xd0, 0x60, 0x06, 0x91},
     .cipher = cipher1}};

static inline bool compare_and_print(size_t len, uint8_t* comp, uint8_t* exp) {
  printf("computed:");
  for (size_t i = 0; i < len; i++)
    printf("%02x", comp[i]);
  printf("\n");
  printf("expected:");
  for (size_t i = 0; i < len; i++)
    printf("%02x", exp[i]);
  printf("\n");
  bool ok = true;
  for (size_t i = 0; i < len; i++)
    ok = ok & (exp[i] == comp[i]);
  if (ok)
    printf("Success!\n");
  else
    printf("**FAILED**\n");
  return ok;
}

static inline void print_time(uint64_t count, clock_t tdiff,
                              struct timespec cdiff) {
  printf("wall time for %" PRIu64 " bytes: %" PRIu64 " (%.2fns/byte)\n", count,
         (uint64_t)timespec_tonanos(cdiff),
         (double)timespec_tonanos(cdiff) / count);
  printf("cpu time for %" PRIu64 " bytes: %" PRIu64 " (%.2fns/byte)\n", count,
         (uint64_t)tdiff,
         (((double)tdiff / CLOCKS_PER_SEC) * 100000000) / count);
  printf("bw %8.2f MB/s\n",
         (double)count / (((double)tdiff / CLOCKS_PER_SEC) * 1000000));
}

bool print_result(int in_len, uint8_t* comp, uint8_t* exp) {
  return compare_and_print(in_len, comp, exp);
}

bool print_test(int in_len, uint8_t* in, uint8_t* key, uint8_t* nonce,
                int aad_len, uint8_t* aad, uint8_t* exp_mac,
                uint8_t* exp_cipher) {
  uint8_t plaintext[in_len];
  memset(plaintext, 0, in_len * sizeof plaintext[0]);
  uint8_t ciphertext[in_len];
  memset(ciphertext, 0, in_len * sizeof ciphertext[0]);
  uint8_t mac[16] = {0};

  Hacl_AEAD_Chacha20Poly1305_encrypt(ciphertext, mac, in, in_len, aad, aad_len,
                                     key, nonce);
  printf("Chacha20Poly1305 (32-bit) Result (chacha20):\n");
  bool ok = print_result(in_len, ciphertext, exp_cipher);
  printf("(poly1305):\n");
  ok = ok && print_result(16, mac, exp_mac);

  int res = Hacl_AEAD_Chacha20Poly1305_decrypt(
      plaintext, exp_cipher, in_len, aad, aad_len, key, nonce, exp_mac);
  if (res != 0)
    printf("AEAD Decrypt (Chacha20/Poly1305) failed \n.");
  ok = ok && (res == 0);
  ok = ok && print_result(in_len, plaintext, in);

  return ok;
}

TEST(chacha20poly1305, test) {
  for (size_t i = 0; i < sizeof(vectors) / sizeof(chacha20poly1305_test_vector);
       ++i) {
    ASSERT_TRUE(print_test(vectors[i].input_len, vectors[i].input,
                           vectors[i].key, vectors[i].nonce, vectors[i].aad_len,
                           vectors[i].aad, vectors[i].tag, vectors[i].cipher));
  }
}

BENCH(chacha20poly1305, bench) {
  uint8_t plain[SIZE];
  uint8_t cipher[SIZE];
  uint8_t aead_key[32];
  uint8_t aead_nonce[12];
  int aad_len = 12;
  uint8_t aead_aad[aad_len];

  int res = 0;
  uint8_t tag[16];
  struct timespec a, b;
  clock_t t1, t2;

  getentropy(aead_nonce, sizeof(aead_nonce));

  memset(plain, 'P', SIZE);
  memset(aead_key, 'K', 32);
  for (int j = 0; j < ROUNDS; j++) {
    Hacl_AEAD_Chacha20Poly1305_encrypt(cipher, tag, plain, SIZE, aead_aad,
                                       aad_len, aead_key, aead_nonce);
  }

  t1 = clock();
  a = timespec_mono();
  for (int j = 0; j < ROUNDS; j++) {
    Hacl_AEAD_Chacha20Poly1305_encrypt(cipher, tag, plain, SIZE, aead_aad,
                                       aad_len, aead_key, aead_nonce);

    res ^= tag[0] ^ tag[15];
  }
  b = timespec_mono();
  t2 = clock();
  clock_t tdiff1 = t2 - t1;
  struct timespec cdiff1 = timespec_sub(b, a);

  for (int j = 0; j < ROUNDS; j++)
    ASSERT_EQ(0, Hacl_AEAD_Chacha20Poly1305_decrypt(plain, cipher, SIZE,
                                                    aead_aad, aad_len, aead_key,
                                                    aead_nonce, tag));

  t1 = clock();
  a = timespec_mono();
  for (int j = 0; j < ROUNDS; j++)
    ASSERT_EQ(0, Hacl_AEAD_Chacha20Poly1305_decrypt(plain, cipher, SIZE,
                                                    aead_aad, aad_len, aead_key,
                                                    aead_nonce, tag));
  b = timespec_mono();
  t2 = clock();
  clock_t tdiff4 = t2 - t1;
  struct timespec cdiff4 = timespec_sub(b, a);

  uint64_t count = ROUNDS * SIZE;
  printf("Chacha20Poly1305 Encrypt (32-bit) PERF:\n");
  print_time(count, tdiff1, cdiff1);
  printf("Chacha20Poly1305 Decrypt (32-bit) PERF:\n");
  print_time(count, tdiff4, cdiff4);
}
