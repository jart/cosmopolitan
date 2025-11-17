// Copyright 2025 Justine Alexandra Roberts Tunney
//
// Permission to use, copy, modify, and/or distribute this software for
// any purpose with or without fee is hereby granted, provided that the
// above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
  int sock1, sock2, sockpair[2];

  sock1 = socket(AF_INET, SOCK_STREAM, 0);
  if (sock1 < 0)
    return 1;

  sock2 = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock2 < 0)
    return 2;

  close(sock1);
  close(sock2);

  if (socket(-1, SOCK_STREAM, 0) != -1)
    return 3;

  if (errno != EAFNOSUPPORT)
    return 4;

#if 0
  // this is hairy across platforms
  if (socket(AF_INET, -1, 0) != -1)
    return 5;
  if (errno != EPROTONOSUPPORT)
    return 6;
#endif

  if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockpair) != 0)
    return 7;

  close(sockpair[0]);
  close(sockpair[1]);

  if (socketpair(-1, SOCK_STREAM, 0, sockpair) != -1)
    return 8;

  if (errno != EAFNOSUPPORT)
    return 9;

  return 0;
}
