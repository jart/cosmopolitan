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
#include "libc/dns/servicestxt.h"

#include "libc/dns/dns.h"
#include "libc/dns/ent.h"
#include "libc/calls/calls.h"
#include "libc/testlib/testlib.h"

char testlib_enable_tmp_setup_teardown;

void SetUp() {
  int fd;
  const char* sample = "\
# skip comment string\n\
chargen		19/tcp		ttytst source\n\
chargen		19/udp		ttytst source\n\
ssh		    22/tcp		# SSH Remote Login Protocol";

  ASSERT_NE(-1, (fd = creat("services", 0755)));
  ASSERT_NE(-1, write(fd, sample, strlen(sample)));
  ASSERT_NE(-1, close(fd));
}

TEST(LookupServicesByPort, GetNameWhenPortCorrect) {
  char name[16]; /* sample has only names of length 3 */
  char proto1[] = "tcp";
  char proto2[] = "udp";
  char* localproto[1];

  localproto[0] = NULL;
  ASSERT_EQ(
      -1, /* non existent port */
      LookupServicesByPort(965, localproto, name, sizeof(name), "services"));
  ASSERT_EQ(NULL, localproto[0]);

  ASSERT_EQ(
      -1, /* port not in network byte order */
      LookupServicesByPort(22, localproto, name, sizeof(name), "services"));
  ASSERT_EQ(NULL, localproto[0]);

  localproto[0] = proto2;
  ASSERT_EQ(-1, /* port ok but wrong protocol */
            LookupServicesByPort(htons(22), localproto, name, sizeof(name),
                                 "services"));
  ASSERT_EQ(localproto[0], proto2);

  localproto[0] = proto1;
  ASSERT_EQ(0, LookupServicesByPort(htons(22), localproto, name, sizeof(name),
                                    "services"));
  ASSERT_STREQ(name, "ssh");
  ASSERT_EQ(localproto[0], proto1);

  localproto[0] = proto2;
  ASSERT_EQ(0, LookupServicesByPort(htons(19), localproto, name,
                                    sizeof(name), "services"));
  ASSERT_STREQ(name, "chargen");
  ASSERT_EQ(localproto[0], proto2);

  localproto[0] = NULL;
  ASSERT_EQ(0, /* pick first matching protocol */
            LookupServicesByPort(htons(19), localproto, name, sizeof(name),
                                 "services"));
  ASSERT_STREQ(name, "chargen");
  ASSERT_NE(NULL, localproto[0]); /* got alloc'd during the call */
  ASSERT_STREQ(localproto[0], "tcp");
  free(localproto[0]);
}

TEST(LookupServicesByName, GetPortWhenNameOrAlias) {
  char name[16]; /* sample has only names of length 3 */
  char proto1[] = "tcp";
  char proto2[] = "udp";
  char* localproto[1];

  localproto[0] = NULL;
  ASSERT_EQ(
      -1, /* non-existent name */
      LookupServicesByName("http", localproto, name, sizeof(name), "services"));
  ASSERT_EQ(NULL, localproto[0]);

  localproto[0] = proto2;
  ASSERT_EQ(
      -1, /* name exists but wrong protocol */
      LookupServicesByName("ssh", localproto, name, sizeof(name), "services"));
  ASSERT_EQ(localproto[0], proto2);

  localproto[0] = proto1;
  ASSERT_EQ(
      htons(22), /* in network byte order */
      LookupServicesByName("ssh", localproto, name, sizeof(name), "services"));
  ASSERT_STREQ(name, "ssh"); /* official name written to buffer */
  ASSERT_EQ(localproto[0], proto1);

  localproto[0] = proto2;
  ASSERT_EQ(htons(19), /* works if alias provided */
            LookupServicesByName("ttytst", localproto, name, sizeof(name),
                                 "services"));
  ASSERT_STREQ(name, "chargen"); /* official name written to buffer */
  ASSERT_EQ(localproto[0], proto2);

  localproto[0] = NULL;
  ASSERT_EQ(htons(19), /* pick first matching protocol */
            LookupServicesByName("source", localproto, name, sizeof(name),
                                 "services"));
  ASSERT_STREQ(name, "chargen");
  ASSERT_NE(NULL, localproto[0]); /* got alloc'd during the call */
  ASSERT_STREQ(localproto[0], "tcp");
  free(localproto[0]);
}
