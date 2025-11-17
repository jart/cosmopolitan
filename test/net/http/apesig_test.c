/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2025 Justine Alexandra Roberts Tunney                              │
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
#include "net/http/apesig.h"
#include "libc/cosmo.h"
#include "libc/elf/def.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/testlib/benchmark.h"
#include "libc/testlib/testlib.h"
#include "net/http/apesig.h"

TEST(apesig, test) {

  // mock signature
  struct ApeSig sig = {
      .path = "o//examples/hello",
      .issued = 123456789,
      .expires = 987654321,
      .identity = "Justine Tunney <jtunney@gmail.com>",
      .github = "jart",
      .key.algorithm = APESIG_ALGORITHM_ED25519,
      .key.pub.size = 32,
      .key.pub.data = (uint8_t[32]){1},
      .elfs_count = 1,
      .elfs = (struct ApeSigElf[]){{
          .machine = EM_NEXGEN32E,
          .sig.algorithm = APESIG_ALGORITHM_ED25519,
          .sig.sig.size = 64,
          .sig.sig.data = (uint8_t[64]){2},
          .phdrs_count = 3,
          .phdrs =
              (struct ApeSigElfPhdr[]){
                  {
                      .type = PT_LOAD,
                      .flags = PF_X | PF_R,
                      .sig.algorithm = APESIG_ALGORITHM_ED25519,
                      .sig.sig.size = 10,
                      .sig.sig.data = (uint8_t[10]){3},
                  },
                  {
                      .type = PT_LOAD,
                      .flags = PF_R,
                      .sig.algorithm = APESIG_ALGORITHM_ED25519,
                      .sig.sig.size = 13,
                      .sig.sig.data = (uint8_t[13]){4},
                  },
                  {
                      .type = PT_LOAD,
                      .flags = PF_R | PF_W,
                      .sig.algorithm = APESIG_ALGORITHM_ED25519,
                      .sig.sig.size = 15,
                      .sig.sig.data = (uint8_t[15]){5},
                  },
              },
      }},
  };

  // turn signature to bytes
  struct ApeBytes bytes;
  ASSERT_EQ(0, SerializeApeSig(&sig, &bytes));

  // wipe signature
  memset(&sig, 0, sizeof(sig));

  // turn bytes back into signature
  ASSERT_EQ(0, ParseApeSig(bytes.data, bytes.size, &sig));

  // verify it has the same stuff
  EXPECT_STREQ("o//examples/hello", sig.path);
  EXPECT_EQ(123456789, sig.issued);
  EXPECT_EQ(987654321, sig.expires);
  EXPECT_STREQ("Justine Tunney <jtunney@gmail.com>", sig.identity);
  EXPECT_STREQ("jart", sig.github);
  EXPECT_EQ(APESIG_ALGORITHM_ED25519, sig.key.algorithm);
  EXPECT_EQ(32, sig.key.pub.size);
  EXPECT_EQ(1, sig.key.pub.data[0]);
  EXPECT_EQ(1, sig.elfs_count);
  EXPECT_EQ(EM_NEXGEN32E, sig.elfs[0].machine);
  EXPECT_EQ(64, sig.elfs[0].sig.sig.size);
  EXPECT_EQ(2, sig.elfs[0].sig.sig.data[0]);
  EXPECT_EQ(3, sig.elfs[0].phdrs_count);
  EXPECT_EQ(PT_LOAD, sig.elfs[0].phdrs[0].type);
  EXPECT_EQ(PF_X | PF_R, sig.elfs[0].phdrs[0].flags);
  EXPECT_EQ(10, sig.elfs[0].phdrs[0].sig.sig.size);
  EXPECT_EQ(3, sig.elfs[0].phdrs[0].sig.sig.data[0]);
  EXPECT_EQ(PT_LOAD, sig.elfs[0].phdrs[1].type);
  EXPECT_EQ(PF_R, sig.elfs[0].phdrs[1].flags);
  EXPECT_EQ(13, sig.elfs[0].phdrs[1].sig.sig.size);
  EXPECT_EQ(4, sig.elfs[0].phdrs[1].sig.sig.data[0]);
  EXPECT_EQ(PT_LOAD, sig.elfs[0].phdrs[2].type);
  EXPECT_EQ(PF_R | PF_W, sig.elfs[0].phdrs[2].flags);
  EXPECT_EQ(15, sig.elfs[0].phdrs[2].sig.sig.size);
  EXPECT_EQ(5, sig.elfs[0].phdrs[2].sig.sig.data[0]);

  // clean up
  DestroyApeSig(&sig);
  free(bytes.data);
}

TEST(apetrust, test) {

  // mock data
  struct ApeTrusts crl = {
      .magic = APETRUST_MAGIC,
      .trusts_count = 1,
      .trusts = (struct ApeTrust[]){{
          .issued = 123456789,
          .expires = 987654321,
          .comment = "too evil",
          .github = "beelzebub",
          .key.algorithm = APESIG_ALGORITHM_ED25519,
          .key.pub.size = 32,
          .key.pub.data = (uint8_t[32]){7},
      }},
  };

  // turn to bytes
  struct ApeBytes bytes;
  ASSERT_EQ(0, SerializeApeTrusts(&crl, &bytes));

  // wipe data
  memset(&crl, 0, sizeof(crl));

  // turn bytes back into data
  ASSERT_EQ(0, ParseApeTrusts(bytes.data, bytes.size, &crl));

  // verify it has the same stuff
  EXPECT_EQ(APETRUST_MAGIC, crl.magic);
  EXPECT_EQ(1, crl.trusts_count);
  EXPECT_EQ(123456789, crl.trusts[0].issued);
  EXPECT_EQ(987654321, crl.trusts[0].expires);
  EXPECT_STREQ("too evil", crl.trusts[0].comment);
  EXPECT_STREQ("beelzebub", crl.trusts[0].github);
  EXPECT_EQ(APESIG_ALGORITHM_ED25519, crl.trusts[0].key.algorithm);
  EXPECT_EQ(32, crl.trusts[0].key.pub.size);
  EXPECT_EQ(7, crl.trusts[0].key.pub.data[0]);

  // clean up
  DestroyApeTrusts(&crl);
  free(bytes.data);
}

void BenchSerializeApeSig(const struct ApeSig *sig) {
  struct ApeBytes bytes;
  ASSERT_EQ(0, SerializeApeSig(sig, &bytes));
  free(bytes.data);
}

void BenchParseApeSig(struct ApeBytes bytes) {
  struct ApeSig sig;
  ASSERT_EQ(0, ParseApeSig(bytes.data, bytes.size, &sig));
  DestroyApeSig(&sig);
}

BENCH(apesig, bench) {
  struct ApeSig sig = {
      .path = "o//examples/hello",
      .issued = 123456789,
      .expires = 987654321,
      .identity = "Justine Tunney <jtunney@gmail.com>",
      .github = "jart",
      .key.algorithm = APESIG_ALGORITHM_ED25519,
      .key.pub.size = 32,
      .key.pub.data = (uint8_t[32]){1},
      .elfs_count = 1,
      .elfs = (struct ApeSigElf[]){{
          .machine = EM_NEXGEN32E,
          .sig.sig.size = 64,
          .sig.sig.data = (uint8_t[64]){2},
          .phdrs_count = 3,
          .phdrs =
              (struct ApeSigElfPhdr[]){
                  {
                      .type = PT_LOAD,
                      .flags = PF_X | PF_R,
                      .sig.sig.size = 10,
                      .sig.sig.data = (uint8_t[10]){3},
                  },
                  {
                      .type = PT_LOAD,
                      .flags = PF_R,
                      .sig.sig.size = 13,
                      .sig.sig.data = (uint8_t[13]){4},
                  },
                  {
                      .type = PT_LOAD,
                      .flags = PF_R | PF_W,
                      .sig.sig.size = 15,
                      .sig.sig.data = (uint8_t[15]){5},
                  },
              },
      }},
  };
  BENCHMARK(100, 1, BenchSerializeApeSig(&sig));
  struct ApeBytes bytes;
  ASSERT_EQ(0, SerializeApeSig(&sig, &bytes));
  BENCHMARK(100, 1, BenchParseApeSig(bytes));
  free(bytes.data);
}
