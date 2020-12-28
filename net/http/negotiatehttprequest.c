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
#include "libc/alg/alg.h"
#include "libc/calls/calls.h"
#include "libc/macros.h"
#include "libc/math.h"
#include "libc/sock/sock.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/shut.h"
#include "libc/sysv/errfuns.h"
#include "libc/time/time.h"
#include "net/http/http.h"

static pureconst long AsMilliseconds(long double ts) {
  return ts * 1e3 + .5;
}

/**
 * Negotiates HTTP request.
 *
 * This function blocks until a response header is consumed. We assume
 * the response header is smaller than *inout_respsize. We're agnostic
 * to the various different i/o paradigms. Goal is really good latency.
 *
 * @param singleshot should be true w/ connection: close
 * @return 0 on success, or -1 w/ errno
 */
int NegotiateHttpRequest(int sock, const char *req, uint32_t *inout_reqsize,
                         char *resp, uint32_t *inout_respsize,
                         uint32_t *out_resphdrsize, bool singleshot,
                         long double timeout) {
  ssize_t rc;
  struct pollfd fd;
  const char *body;
  long double start, now, deadline;
  uint32_t transmitted, received, exchanged, remaining;
  fd.fd = sock;
  received = 0;
  transmitted = 0;
  fd.events = POLLOUT | POLLIN;
  deadline = (start = now = nowl()) + timeout;
  do {
    if ((rc = poll(&fd, 1, MAX(0, AsMilliseconds(deadline - now)))) == 1) {
      if (fd.revents & POLLHUP) {
        econnreset();
        break;
      }
      if (fd.revents & (POLLIN | POLLERR)) {
        remaining = *inout_respsize - received - 1;
        if ((rc = recv(fd.fd, resp + received, remaining, 0)) != -1) {
          exchanged = rc;
          body = memmem(resp + (received >= 4 ? received - 4 : 0), exchanged,
                        "\r\n\r\n", 4);
          received += exchanged;
          if (body) {
            resp[received] = '\0';
            *inout_respsize = received;
            *inout_reqsize = transmitted;
            *out_resphdrsize = body - resp;
            return 0;
          }
          if (exchanged == remaining) {
            emsgsize();
            break;
          }
        } else {
          break;
        }
      }
      if (fd.revents & POLLOUT) {
        remaining = *inout_reqsize - transmitted;
        if ((rc = send(fd.fd, req + transmitted, remaining, 0)) != -1) {
          exchanged = rc;
          transmitted += exchanged;
          if (exchanged == remaining) {
            if (singleshot) shutdown(fd.fd, SHUT_WR);
            fd.events &= ~POLLOUT;
          }
        } else {
          break;
        }
      }
    } else {
      if (!rc) etimedout();
      break;
    }
  } while ((now = nowl()) < deadline);
  close(fd.fd);
  return -1;
}
