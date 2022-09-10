// clang-format off
/* Copyright 2016 Google Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License. */

#include "libc/thread/headers.h"

NSYNC_CPP_START_

/* Write the nul-terminated string s[] to file descriptor fd. */
static void writestr (int fd, const char *s) {
        int len = strlen (s);
        int n = 0;
        while (len != 0 && n >= 0) {
                n = write (fd, s, len);
                if (n >= 0) {
                        len -= n;
                        s += n;
                } else if (n == -1 && errno == EINTR) {
                        n = 0;
                }
        }
}

/* Abort after printing the nul-terminated string s[]. */
void nsync_panic_ (const char *s) {
        writestr (2, "panic: ");
        writestr (2, s);
        abort ();
}

NSYNC_CPP_END_
