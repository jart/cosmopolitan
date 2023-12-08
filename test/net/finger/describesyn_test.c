/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/log/check.h"
#include "libc/stdio/rand.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"
#include "net/finger/finger.h"

#define DESCRIBESYN(s) MyDescribeSyn(s, sizeof(s) - 1)

char o[128];

char *MyDescribeSyn(const char *p, size_t n) {
  char *q;
  rngset(o, sizeof(o), lemur64, -1);
  q = DescribeSyn(o, sizeof(o), p, n);
  CHECK_NOTNULL(q);
  CHECK_EQ(strlen(o), q - o);
  return o;
}

TEST(DescribeSyn, badPacket) {
  ASSERT_EQ(NULL, DescribeSyn(o, sizeof(o), "123", 3));
}

TEST(DescribeSyn, testWindows7) {
  // version        = 4
  // ihl            = 20
  // dscp           = 0
  // ecn            = 0
  // lengthtotal    = 52
  // identification = 5538
  // flags          = 2
  // fragmentoffset = 0
  // ttl            = 128
  // protocol       = 6
  // ipchecksum     = 48124
  // srcip          = 127.10.10.150
  // dstip          = 127.10.10.124
  //
  // srcport     = 50078
  // dstport     = 7000
  // sequence    = 305725139
  // acknumber   = 0
  // dataoffset  = 8
  // ns          = 0
  // cwr         = 0
  // ece         = 0
  // urg         = 0
  // ack         = 0
  // psh         = 0
  // rst         = 0
  // syn         = 1
  // fin         = 0
  // wsize       = 8192
  // tcpchecksum = 13806
  // urgpointer  = 0
  //
  // option 2 4 5 180 (Maximum Segment Size)
  // option 1 (No-Operation)
  // option 3 3 2 (Window Scale)
  // option 1 (No-Operation)
  // option 1 (No-Operation)
  // option 4 2 (SACK Permitted)
  static char s[] = "\
\x45\x00\x00\x34\x15\xa2\x40\x00\x80\x06\xbb\xfc\x7f\x0a\x0a\x96\
\x7f\x0a\x0a\x7c\xc3\x9e\x1b\x58\x12\x38\xfe\xd3\x00\x00\x00\x00\
\x80\x02\x20\x00\x35\xee\x00\x00\x02\x04\x05\xb4\x01\x03\x03\x02\
\x01\x01\x04\x02";
  EXPECT_STREQ("128:MNKNNO:2710:52", DESCRIBESYN(s));
}

TEST(DescribeSyn, testWindows10) {
  // version        = 4
  // ihl            = 20
  // dscp           = 0
  // ecn            = 0
  // lengthtotal    = 52
  // identification = 24981
  // flags          = 2
  // fragmentoffset = 0
  // ttl            = 128
  // protocol       = 6
  // ipchecksum     = 28818
  // srcip          = 127.10.10.13
  // dstip          = 127.10.10.124
  //
  // srcport     = 60914
  // dstport     = 7000
  // sequence    = 3953567664
  // acknumber   = 0
  // dataoffset  = 8
  // ns          = 0
  // cwr         = 0
  // ece         = 0
  // urg         = 0
  // ack         = 0
  // psh         = 0
  // rst         = 0
  // syn         = 1
  // fin         = 0
  // wsize       = 64240
  // tcpchecksum = 44768
  // urgpointer  = 0
  //
  // option 2 4 5 180 (Maximum Segment Size)
  // option 1 (No-Operation)
  // option 3 3 8 (Window Scale)
  // option 1 (No-Operation)
  // option 1 (No-Operation)
  // option 4 2 (SACK Permitted)
  static char s[] = "\
\x45\x00\x00\x34\x61\x95\x40\x00\x80\x06\x70\x92\x7f\x0a\x0a\x0d\
\x7f\x0a\x0a\x7c\xed\xf2\x1b\x58\xeb\xa6\xa7\xb0\x00\x00\x00\x00\
\x80\x02\xfa\xf0\xae\xe0\x00\x00\x02\x04\x05\xb4\x01\x03\x03\x08\
\x01\x01\x04\x02";
  EXPECT_STREQ("128:MNKNNO:2573:52", DESCRIBESYN(s));
}

TEST(FingerprintSyn, testLinuxAlpine_isLinux) {
  // version        = 4
  // ihl            = 20
  // dscp           = 0
  // ecn            = 0
  // lengthtotal    = 60
  // identification = 16488
  // flags          = 2
  // fragmentoffset = 0
  // ttl            = 64
  // protocol       = 6
  // ipchecksum     = 53576
  // srcip          = 127.10.10.124
  // dstip          = 127.10.10.124
  //
  // srcport     = 60978
  // dstport     = 7000
  // sequence    = 4272530606
  // acknumber   = 0
  // dataoffset  = 10
  // ns          = 0
  // cwr         = 0
  // ece         = 0
  // urg         = 0
  // ack         = 0
  // psh         = 0
  // rst         = 0
  // syn         = 1
  // fin         = 0
  // wsize       = 65495
  // tcpchecksum = 10554
  // urgpointer  = 0
  //
  // option 2 4 255 215 (Maximum Segment Size)
  // option 4 2 (SACK Permitted)
  // option 8 10 67 61 171 124 0 0 0 0 (Timestamps)
  // option 1 (No-Operation)
  // option 3 3 7 (Window Scale)
  static char s[] = "\
\x45\x00\x00\x3c\x40\x68\x40\x00\x40\x06\xd1\x48\x7f\x0a\x0a\x7c\
\x7f\x0a\x0a\x7c\xee\x32\x1b\x58\xfe\xa9\xa4\xae\x00\x00\x00\x00\
\xa0\x02\xff\xd7\x29\x3a\x00\x00\x02\x04\xff\xd7\x04\x02\x08\x0a\
\x43\x3d\xab\x7c\x00\x00\x00\x00\x01\x03\x03\x07";
  EXPECT_STREQ("64:MOTNK:2684:60", DESCRIBESYN(s));
}

TEST(FingerprintSyn, testLinuxDebian_isLinux) {
  // version        = 4
  // ihl            = 20
  // dscp           = 0
  // ecn            = 0
  // lengthtotal    = 60
  // identification = 51388
  // flags          = 2
  // fragmentoffset = 0
  // ttl            = 64
  // protocol       = 6
  // ipchecksum     = 18678
  // srcip          = 127.10.10.122
  // dstip          = 127.10.10.124
  //
  // srcport     = 57892
  // dstport     = 7000
  // sequence    = 4142527109
  // acknumber   = 0
  // dataoffset  = 10
  // ns          = 0
  // cwr         = 0
  // ece         = 0
  // urg         = 0
  // ack         = 0
  // psh         = 0
  // rst         = 0
  // syn         = 1
  // fin         = 0
  // wsize       = 64240
  // tcpchecksum = 24492
  // urgpointer  = 0
  //
  // option 2 4 5 180 (Maximum Segment Size)
  // option 4 2 (SACK Permitted)
  // option 8 10 74 67 147 41 0 0 0 0 (Timestamps)
  // option 1 (No-Operation)
  // option 3 3 7 (Window Scale)
  static char s[] = "\
\x45\x00\x00\x3c\xc8\xbc\x40\x00\x40\x06\x48\xf6\x7f\x0a\x0a\x7a\
\x7f\x0a\x0a\x7c\xe2\x24\x1b\x58\xf6\xe9\xf2\x85\x00\x00\x00\x00\
\xa0\x02\xfa\xf0\x5f\xac\x00\x00\x02\x04\x05\xb4\x04\x02\x08\x0a\
\x4a\x43\x93\x29\x00\x00\x00\x00\x01\x03\x03\x07";
  EXPECT_STREQ("64:MOTNK:2682:60", DESCRIBESYN(s));
}

TEST(FingerprintSyn, testLinuxFastOpen_isLinux) {
  static char s[] = "\
\x45\x00\x00\x99\x6f\x97\x40\x00\x40\x06\xa1\xbc\x7f\x0a\x0a\x7c\
\x7f\x0a\x0a\x7c\xee\x3a\x1b\x58\xe1\xbd\xd2\x8d\x00\x00\x00\x00\
\xd0\x02\xff\xd7\x29\x97\x00\x00\x02\x04\xff\xd7\x04\x02\x08\x0a\
\x43\x3f\x21\xf8\x00\x00\x00\x00\x01\x03\x03\x07\x22\x0a\xd4\x75\
\x4f\x06\xc0\x97\x25\x6c\x01\x01";
  EXPECT_STREQ("64:MOTNK:2684:153", DESCRIBESYN(s));
}

TEST(FingerprintSyn, testLinuxGceVm_isLinux) {
  static char s[] = "\
\x45\x20\x00\x3c\x59\x9a\x40\x00\x3b\x06\xb0\xe4\x23\xe2\xfc\xb5\
\x7f\x0a\x0a\x7c\xd2\xfe\x1b\x58\xf5\xb2\xf6\xca\x00\x00\x00\x00\
\xa0\x02\xff\x28\x16\xe7\x00\x00\x02\x04\x05\x8c\x04\x02\x08\x0a\
\xb6\x4d\x6b\xd8\x00\x00\x00\x00\x01\x03\x03\x07";
  EXPECT_STREQ("59:MOTNK:64693:60", DESCRIBESYN(s));
}

TEST(FingerprintSyn, testOpenbsd) {
  static char s[] = "\
\x45\x00\x00\x40\x7c\x71\x40\x00\x40\x06\x95\x32\x7f\x0a\x0a\x85\
\x7f\x0a\x0a\x7c\xa2\x91\x1b\x58\x53\x54\xbc\xc5\x00\x00\x00\x00\
\xb0\x02\x40\x00\x70\xf7\x00\x00\x02\x04\x05\xb4\x01\x01\x04\x02\
\x01\x03\x03\x06\x01\x01\x08\x0a\xa2\x70\xeb\x7a\x00\x00\x00\x00";
  EXPECT_STREQ("64:MNNONKNNT:2693:64", DESCRIBESYN(s));
}

TEST(FingerprintSyn, testXnu) {
  static char s[] = "\
\x45\x00\x00\x40\x00\x00\x40\x00\x40\x06\x11\xb1\x7f\x0a\x0a\x78\
\x7f\x0a\x0a\x7c\xd3\x63\x1b\x58\xe6\xd5\xf8\x7e\x00\x00\x00\x00\
\xb0\x02\xff\xff\x70\x36\x00\x00\x02\x04\x05\xb4\x01\x03\x03\x06\
\x01\x01\x08\x0a\x25\x85\xaa\x28\x00\x00\x00\x00\x04\x02\x00\x00";
  EXPECT_STREQ("64:MNKNNTOE:2680:64", DESCRIBESYN(s));
}

TEST(FingerprintSyn, testFreebsd12) {
  static char s[] = "\
\x45\x00\x00\x3c\x00\x00\x40\x00\x40\x06\x11\xae\x7f\x0a\x0a\x7f\
\x7f\x0a\x0a\x7c\xde\x81\x1b\x58\x23\x39\xfc\x6b\x00\x00\x00\x00\
\xa0\x02\xff\xff\x8c\x8c\x00\x00\x02\x04\x05\xb4\x01\x03\x03\x06\
\x04\x02\x08\x0a\x1f\xa1\x59\x46\x00\x00\x00\x00";
  EXPECT_STREQ("64:MNKOT:2687:60", DESCRIBESYN(s));
}

TEST(FingerprintSyn, testRedbeanGceVm_isFreebsd) {
  static char s[] = "\
\x45\x20\x00\x3c\x00\x00\x40\x00\x3b\x06\x92\xe4\x23\xee\x74\x44\
\x7f\x0a\x0a\x7c\x5a\x6e\x1b\x58\x0a\x15\xba\x1a\x00\x00\x00\x00\
\xa0\x02\xff\xff\xe4\x60\x00\x00\x02\x04\x05\x8c\x01\x03\x03\x06\
\x04\x02\x08\x0a\xc9\x06\xb4\x13\x00\x00\x00\x00";
  EXPECT_STREQ("59:MNKOT:29764:60", DESCRIBESYN(s));
}

TEST(FingerprintSyn, testNetbsd_isFreebsdSadly) {
  static char s[] = "\
\x45\x00\x00\x3c\x00\x00\x40\x00\x40\x06\x11\x98\x7f\x0a\x0a\x95\
\x7f\x0a\x0a\x7c\xfc\x18\x1b\x58\x8e\xb8\x1b\xfe\x00\x00\x00\x00\
\xa0\x02\x80\x00\xdc\xb6\x00\x00\x02\x04\x05\xb4\x01\x03\x03\x03\
\x04\x02\x08\x0a\x00\x00\x00\x01\x00\x00\x00\x00";
  EXPECT_STREQ("64:MNKOT:2709:60", DESCRIBESYN(s));
}
