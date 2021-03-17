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
#include "libc/sock/internal.h"
#include "libc/sock/yoink.inc"
#include "libc/sysv/consts/fio.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/consts/af.h"

#include "libc/alg/reverse.internal.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/filesharemode.h"
#include "libc/nt/ipc.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"


// {{{ sys_socketpair_nt
int sys_socketpair_nt(int family, int type, int proto, int sv[2]) {
    int64_t hpipe, h1, h2;
    int reader, writer;
    char16_t pipename[64];
    uint32_t mode;

    // Supports only AF_UNIX
    if (family != AF_UNIX) {
        errno = EAFNOSUPPORT;
        return -1;
    }

    mode = kNtPipeWait;
    if (type == SOCK_STREAM) {
        mode |= kNtPipeReadmodeByte | kNtPipeTypeByte;
    } else if ((type == SOCK_DGRAM) || (type == SOCK_SEQPACKET)) {
        mode |= kNtPipeReadmodeMessage | kNtPipeTypeMessage;
    } else {
        errno = EOPNOTSUPP;
        return -1;
    }

    CreatePipeName(pipename);
    if ((reader = __reservefd()) == -1) return -1;
    if ((writer = __reservefd()) == -1) {
        __releasefd(reader);
        return -1;
    }
    if ((hpipe = CreateNamedPipe(pipename, 
            kNtPipeAccessDuplex,
            mode, 
            1, 
            65536, 
            65536,
            0, 
            &kNtIsInheritable)) == -1) {
        __winerr();
        __releasefd(writer);
        __releasefd(reader);
        return -1;
    }

    h1 = CreateFile(pipename, 
            kNtGenericWrite | kNtGenericRead, 
            0,  // Not shared
            &kNtIsInheritable, 
            kNtOpenExisting, 0, 0);
    if (h1 == -1) {
        CloseHandle(hpipe);
        __winerr();
        __releasefd(writer);
        __releasefd(reader);
        return -1;
    }

    g_fds.p[reader].kind = kFdFile;
    g_fds.p[reader].flags = 0;      // TODO
    g_fds.p[reader].handle = hpipe;

    g_fds.p[writer].kind = kFdFile;
    g_fds.p[writer].flags = 0;      // TODO
    g_fds.p[writer].handle = h1;

    sv[0] = reader;
    sv[1] = writer;
    return 0;
}
// }}}




