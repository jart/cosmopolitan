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

textwindows int sys_socketpair_nt(int family, int type, int protocol, int sv[2]) {
    struct sockaddr_storage ss;
    struct sockaddr_in *sa = (struct sockaddr_in *)&ss;
    uint32_t ss_len;
    int spR = -1, spW = -1;
    int opt;
    int rc = -1;

    int listensock = -1;
    sv[0] = -1;     /* INVALID_SOCKET */
    sv[1] = -1;     /* INVALID_SOCKET */

    memset(&ss, 0, sizeof(ss));
    if (family == AF_INET){
        sa->sin_family = family;
        sa->sin_addr.s_addr = htonl(0x7f000001);
        sa->sin_port = 0;
        ss_len = sizeof(struct sockaddr_in);
    } else {
        return -1;
    }

    listensock = sys_socket_nt(family, SOCK_STREAM, IPPROTO_TCP);
    if (listensock == -1){
        goto done;
    }

    if (bind(listensock, (struct sockaddr *)&ss, ss_len) == -1){
        goto done;
    }

    if (listen(listensock, 1) == -1){
        goto done;
    }

    memset(&ss, 0, sizeof(ss));
    ss_len = sizeof(ss);
    if (getsockname(listensock, (struct sockaddr *)&ss, &ss_len) < 0){
        goto done;
    }

    sa->sin_family = family;
    sa->sin_addr.s_addr = htonl(0x7f000001);
    ss_len = sizeof(struct sockaddr_in);

    spR = socket(family, SOCK_STREAM, IPPROTO_TCP);
    if (spR == -1){
        goto done;
    }

    /* Set non-blocking */
    opt = fcntl(spR, F_GETFL, 0);
    if (opt == -1) {
        goto done;
    }
    if(fcntl(spR, F_SETFL, opt | O_NONBLOCK) == -1){
        goto done;
    }

    if (connect(spR, (struct sockaddr *)&ss, ss_len) < 0){
        errno = WSAGetLastError();
        if (errno != EINPROGRESS && errno != EWOULDBLOCK){
            goto done;
        }
    }

    spW = accept(listensock, NULL, 0);
    if(spW == -1){
        errno = WSAGetLastError();
        if (errno != EINPROGRESS && errno != EWOULDBLOCK){
            goto done;
        }
    }
    /* Set non-blocking */
    opt = fcntl(spW, F_GETFL, 0);
    if (opt == -1) {
        goto done;
    }
    if(fcntl(spW, F_SETFL, opt | O_NONBLOCK) == -1){
        goto done;
    }

    rc = 0; /* Success */

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

    /* Both success and failure */
    if (listensock != -1) {
        close(listensock);
    }
    return rc;
}
