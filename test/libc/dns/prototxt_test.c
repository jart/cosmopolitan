/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ This is free and unencumbered software released into the public domain.      │
│                                                                              │
│ Anyone is free to copy, modify, publish, use, compile, sell, or              │
│ distribute this software, either in source code form or as a compiled        │
│ binary, for any purpose, commercial or non-commercial, and by any            │
│ means.                                                                       │
│                                                                              │
│ In jurisdictions that recognize copyright laws, the author or authors        │
│ of this software dedicate any and all copyright interest in the              │
│ software to the public domain. We make this dedication for the benefit       │
│ of the public at large and to the detriment of our heirs and                 │
│ successors. We intend this dedication to be an overt act of                  │
│ relinquishment in perpetuity of all present and future rights to this        │
│ software under copyright law.                                                │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,              │
│ EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF           │
│ MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.       │
│ IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR            │
│ OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        │
│ ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR        │
│ OTHER DEALINGS IN THE SOFTWARE.                                              │
│                                                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/calls.h"
#include "libc/dns/dns.h"
#include "libc/dns/ent.h"
#include "libc/dns/prototxt.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

char testlib_enable_tmp_setup_teardown;

void SetUp() {
  int fd;
  const char *sample = "\
# skip comment string\n\
rspf	73	RSPF CPHB	\n\
ggp	    3	GGP		    ";
  ASSERT_NE(-1, (fd = creat("protocols", 0755)));
  ASSERT_NE(-1, write(fd, sample, strlen(sample)));
  ASSERT_NE(-1, close(fd));
}

TEST(LookupProtoByNumber, GetNameWhenNumberCorrect) {
  char name[16]; /* sample has only names of length 3-4 */
  strcpy(name, "");
  ASSERT_EQ(-1, /*non-existent number */
            LookupProtoByNumber(24, name, sizeof(name), "protocols"));
  ASSERT_EQ(-1, /* sizeof(name) insufficient, memccpy failure */
            LookupProtoByNumber(73, name, 1, "protocols"));
  ASSERT_STREQ(name, ""); /* cleaned up after memccpy failed */
  ASSERT_EQ(0,            /* works with valid number */
            LookupProtoByNumber(73, name, sizeof(name), "protocols"));
  ASSERT_STREQ(name, "rspf"); /* official name written */
}

TEST(LookupProtoByName, GetNumberWhenNameOrAlias) {
  char name[16]; /* sample has only names of length 3-4 */
  strcpy(name, "");
  ASSERT_EQ(-1, /* non-existent name or alias */
            LookupProtoByName("tcp", name, sizeof(name), "protocols"));
  ASSERT_EQ(-1, /* sizeof(name) insufficient, memccpy failure */
            LookupProtoByName("ggp", name, 1, "protocols"));
  ASSERT_STREQ(name, ""); /* cleaned up after memccpy failed */
  ASSERT_EQ(3,            /* works with valid name */
            LookupProtoByName("ggp", name, sizeof(name), "protocols"));
  ASSERT_STREQ(name, "ggp");
  ASSERT_EQ(73, /* works with valid alias */
            LookupProtoByName("CPHB", name, sizeof(name), "protocols"));
  ASSERT_STREQ(name, "rspf"); /* official name written */
}
