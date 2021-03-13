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
#include "libc/calls/internal.h"
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"
#include "libc/sock/yoink.inc"
#include "libc/sock/sock.h"
#include "libc/sysv/consts/fio.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/inaddr.h"

/* Polyfill for socketpair() on Windows with family=AF_INET and 
 * type=SOCK_STREAM or SOCK_SEQPACKET
 */
textwindows int sys_socketpair_nt_stream(int family, int type, int proto, int sv[2]) {
  struct sockaddr_storage ss;
  struct sockaddr_in *sa = (struct sockaddr_in *)&ss;
  uint32_t ss_len;
  int spR = -1, spW = -1;
  int opt;
  int rc = -1;
  int listensock = -1;

  /* Avoid warnings for unused parameters */
  (void)family;
  (void)proto;

  sv[0] = -1;     /* INVALID_SOCKET */
  sv[1] = -1;     /* INVALID_SOCKET */

  memset(&ss, 0, sizeof(ss));
  sa->sin_family = AF_INET;
  sa->sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  sa->sin_port = 0;
  ss_len = sizeof(struct sockaddr_in);

  listensock = sys_socket_nt(AF_INET, type, IPPROTO_TCP);
  if (listensock == -1) {
    errno = WSAGetLastError();
    goto done;
  }

  if (bind(listensock, (struct sockaddr *)&ss, ss_len) == -1) {
    goto done;
  }

  if (listen(listensock, 1) == -1) {
    goto done;
  }

  memset(&ss, 0, sizeof(ss));
  ss_len = sizeof(ss);
  if (getsockname(listensock, (struct sockaddr *)&ss, &ss_len) < 0) {
    goto done;
  }

  sa->sin_family = AF_INET;
  sa->sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  ss_len = sizeof(struct sockaddr_in);

  spR = socket(AF_INET, type, IPPROTO_TCP);
  if (spR == -1) {
    goto done;
  }

  /* Set non-blocking */
  opt = fcntl(spR, F_GETFL, 0);
  if (opt == -1) {
    goto done;
  }
  if(fcntl(spR, F_SETFL, opt | O_NONBLOCK) == -1) {
    goto done;
  }

  if (connect(spR, (struct sockaddr *)&ss, ss_len) < 0) {
    errno = WSAGetLastError();
    if (errno != EINPROGRESS && errno != EWOULDBLOCK) {
      goto done;
    }
  }

  spW = accept(listensock, NULL, 0);
  if(spW == -1) {
    errno = WSAGetLastError();
    if (errno != EINPROGRESS && errno != EWOULDBLOCK) {
      goto done;
    }
  }
  /* Set non-blocking */
  opt = fcntl(spW, F_GETFL, 0);
  if (opt == -1) {
    goto done;
  }
  if(fcntl(spW, F_SETFL, opt | O_NONBLOCK) == -1) {
    goto done;
  }

  rc = 0; /* Success */

done:
  if (rc == -1) {
    if (spR != -1) {
      close(spR);
    }
    if (spW != -1) {
      close(spW);
    }
  } else {
    sv[0] = spR;
    sv[1] = spW;
  }

  /* Both success and failure */
  if (listensock != -1) {
    close(listensock);
  }
  return rc;
}


int sys_socketpair_nt_dgram(int family, int type, int proto, int sv[2]) {
  struct sockaddr_in sa;
  uint32_t sa_len;
  int opt;
  int spR = -1, spW = -1;
  int rc = -1;

  /* Avoid warnings for unused parameters */
  (void)family;
  (void)proto;

  sv[0] = -1;     /* INVALID_SOCKET */
  sv[1] = -1;     /* INVALID_SOCKET */
  memset(&sa, 0, sizeof(sa));
  sa.sin_family = AF_INET;
  sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  sa.sin_port = 0;
  sa_len = sizeof(struct sockaddr_in);

  /* Creates a read socket */
  spR = sys_socket_nt(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (spR == -1) {
    errno = WSAGetLastError();
    goto done;
  }

  /* ... and bind it to whatever port is available on localhost */
  if (bind(spR, (struct sockaddr *)&sa, sa_len) < 0) {
    goto done;
  }

  /* Read the port # that bind obtained */
  memset(&sa, 0, sizeof(sa));
  sa_len = sizeof(sa);
  if (getsockname(spR, (struct sockaddr *)&sa, &sa_len) < 0) {
    goto done;
  }
  // The port # is on: sa.sin_port
  // printf("RD Bound: port=%d, addr=%08x\n", sa.sin_port, sa.sin_addr.s_addr);

  /* Creates a write socket */
  spW = sys_socket_nt(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (spW == -1) {
    errno = WSAGetLastError();
    goto done;
  }
  /* ... and connect it to the read socket port */
  if (connect(spW, (struct sockaddr *)&sa, sa_len) < 0) {
    goto done;
  }

  /* So far we got connection only in one way:
   *      WR ----> RD
   * To ensure symmetric behavior, read back the port bound of the 
   * write socket */
  memset(&sa, 0, sizeof(sa));
  sa_len = sizeof(sa);
  if (getsockname(spW, (struct sockaddr *)&sa, &sa_len) < 0) {
    goto done;
  }

  // The write socket is bound at: sa.sin_port
  //printf("WR Bound: port=%d, addr=%08x\n", sa.sin_port, sa.sin_addr.s_addr);

  // ... finally connect the READ socket to the WRITE socket
  if (connect(spR, (struct sockaddr *)&sa, sa_len) < 0) {
    goto done;
  }

  /* Now we finally got both bi-directional sockets: RD <---> WR */

  /* Finally set both sockets as non-blocking */
  opt = fcntl(spW, F_GETFL, 0);
  if (opt == -1) {
    goto done;
  }
  if(fcntl(spW, F_SETFL, opt | O_NONBLOCK) == -1) {
    goto done;
  }

  opt = fcntl(spR, F_GETFL, 0);
  if (opt == -1) {
    goto done;
  }
  if(fcntl(spR, F_SETFL, opt | O_NONBLOCK) == -1) {
    goto done;
  }

  rc = 0;

done:
  if (rc == -1) {
    errno = WSAGetLastError();
    if (spR != -1) {
      close(spR);
    }
    if (spW != -1) {
      close(spW);
    }
  } else {
    sv[0] = spR;
    sv[1] = spW;
  }

  return rc;
}





