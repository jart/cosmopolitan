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
#include "libc/dns/servicestxt.h"
#include "libc/str/str.h"
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
  char name[8];        /* service names are of length 3 */
  char eitherproto[8]; /* protocol names are of length 3 */
  char proto1[] = "tcp";
  char proto2[] = "udp";
  char* localproto;
  strcpy(eitherproto, "");
  strcpy(name, "");

  localproto = eitherproto;
  ASSERT_EQ(-1, /* non existent port */
            LookupServicesByPort(965, localproto, sizeof(eitherproto), name,
                                 sizeof(name), "services"));
  ASSERT_EQ('\0', localproto[0]);

  localproto = eitherproto;
  ASSERT_EQ(-1, /* port in network byte order */
            LookupServicesByPort(htons(22), localproto, sizeof(eitherproto),
                                 name, sizeof(name), "services"));
  ASSERT_EQ('\0', localproto[0]);

  localproto = proto2;
  ASSERT_EQ(-1, /* port ok but wrong protocol */
            LookupServicesByPort(22, localproto, sizeof(proto2), name,
                                 sizeof(name), "services"));
  ASSERT_STREQ(proto2, "udp");

  localproto = proto1;
  ASSERT_EQ(
      -1, /* protocol is non-NULL/length must be nonzero */
      LookupServicesByPort(22, localproto, 0, name, sizeof(name), "services"));
  ASSERT_STREQ(proto1, "tcp");

  localproto = proto1;
  ASSERT_EQ(-1, /* sizeof(name) insufficient, memccpy failure */
            LookupServicesByPort(22, localproto, sizeof(proto1), name, 1,
                                 "services"));
  ASSERT_STREQ(proto1, "tcp");
  ASSERT_STREQ(name, ""); /* cleaned up after memccpy failed */

  localproto = eitherproto;
  ASSERT_EQ(
      -1, /* sizeof(proto) insufficient, memccpy failure */
      LookupServicesByPort(22, localproto, 1, name, sizeof(name), "services"));
  ASSERT_STREQ(eitherproto, ""); /* cleaned up after memccpy failed */

  localproto = proto1;
  ASSERT_EQ(0, LookupServicesByPort(22, localproto, sizeof(proto1), name,
                                    sizeof(name), "services"));
  ASSERT_STREQ(name, "ssh");
  ASSERT_STREQ(proto1, "tcp");

  localproto = proto2;
  ASSERT_EQ(0, LookupServicesByPort(19, localproto, sizeof(proto2), name,
                                    sizeof(name), "services"));
  ASSERT_STREQ(name, "chargen");
  ASSERT_STREQ(proto2, "udp");

  localproto = eitherproto;
  ASSERT_EQ(0, /* pick first matching protocol */
            LookupServicesByPort(19, localproto, sizeof(eitherproto), name,
                                 sizeof(name), "services"));
  ASSERT_STREQ(name, "chargen");
  ASSERT_NE('\0', localproto[0]); /* buffer filled during the call */
  ASSERT_STREQ(eitherproto, "tcp");
}

TEST(LookupServicesByName, GetPortWhenNameOrAlias) {
  char name[8];        /* service names are of length 3 */
  char eitherproto[8]; /* protocol names are of length 3 */
  char proto1[] = "tcp";
  char proto2[] = "udp";
  char* localproto;
  strcpy(eitherproto, "");
  strcpy(name, "");

  localproto = eitherproto;
  ASSERT_EQ(-1, /* non-existent name */
            LookupServicesByName("http", localproto, sizeof(eitherproto), name,
                                 sizeof(name), "services"));
  ASSERT_EQ('\0', localproto[0]);

  localproto = proto2;
  ASSERT_EQ(-1, /* name exists but wrong protocol */
            LookupServicesByName("ssh", localproto, sizeof(proto2), name,
                                 sizeof(name), "services"));
  ASSERT_STREQ(proto2, "udp");

  localproto = proto2;
  ASSERT_EQ(-1, /* protocol is non-NULL/length must be nonzero */
            LookupServicesByName("ssh", localproto, 0, name, sizeof(name),
                                 "services"));
  ASSERT_STREQ(proto2, "udp");

  localproto = proto1;
  ASSERT_EQ(-1, /* sizeof(name) insufficient, memccpy failure */
            LookupServicesByName("ssh", localproto, sizeof(proto1), name, 1,
                                 "services"));
  ASSERT_STREQ(proto1, "tcp");
  ASSERT_STREQ(name, ""); /* cleaned up after memccpy failed */

  localproto = eitherproto;
  ASSERT_EQ(-1, /* sizeof(proto) insufficient, memccpy failure */
            LookupServicesByName("ssh", localproto, 1, name, sizeof(name),
                                 "services"));
  ASSERT_STREQ(eitherproto, ""); /* cleaned up after memccpy failed */

  localproto = proto1;
  ASSERT_EQ(22, LookupServicesByName("ssh", localproto, sizeof(proto1), name,
                                     sizeof(name), "services"));
  ASSERT_STREQ(name, "ssh"); /* official name written to buffer */
  ASSERT_STREQ(proto1, "tcp");

  localproto = proto2;
  ASSERT_EQ(19, /* works if alias provided */
            LookupServicesByName("ttytst", localproto, sizeof(proto2), name,
                                 sizeof(name), "services"));
  ASSERT_STREQ(name, "chargen"); /* official name written to buffer */

  localproto = proto2;
  ASSERT_EQ(19, /* can get port returned without official name */
            LookupServicesByName("ttytst", localproto, sizeof(proto2), NULL, 0,
                                 "services"));

  localproto = eitherproto;
  ASSERT_EQ(19, /* pick first matching protocol */
            LookupServicesByName("source", localproto, sizeof(eitherproto),
                                 name, sizeof(name), "services"));
  ASSERT_STREQ(name, "chargen");
  ASSERT_STREQ(localproto, "tcp");
}
