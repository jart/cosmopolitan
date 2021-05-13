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
#include "libc/bits/weaken.h"
#include "libc/sysv/errfuns.h"
#include "libc/calls/internal.h"
#include "libc/sock/sock.h"
#include "libc/sock/internal.h"
#include "libc/sysv/consts/sio.h"


#include "libc/stdio/stdio.h"
#define PRINTF  weaken(printf)

/* SIOCGIFCONF:
 * Takes an struct ifconf object of a given size
 * Modifies the following:
 *  - ifc_len: set it to the number of valid ifreq structures representing
 *    the interfaces
 *  - ifc_ifcu.ifcu_req: sets the name of the interface for each interface
 * The ifc_len is an input/output parameter: set it to the total size of 
 * the ifcu_buf (ifcu_req) buffer on input.
 */
int ioctl_siocgifconf_nt(int, struct ifconf *) hidden;

static int ioctl_siocgifconf_sysv(int fd, struct ifconf *ifc) {
  /* Same as the default for now... */
  if (IsBsd()) {
    if (!weaken(malloc)) {
      return enomem();
    } else {
      /* On BSD the size of the struct ifreq is smaller (16 bytes
       * instead of 24 bytes), so buffers need to be adjusted accordingly
       *
       * TODO: Since BSD requires a SMALLER buffer we don't need to 
       *       malloc a temp buffer, insted reuse the same buffer and
       *       safely move overlapping ifrn_name chunks
       */
      int i;
      struct ifconf ifc_bsd;
      size_t num_ifreq = ifc->ifc_len / sizeof(struct ifreq);

      PRINTF("Mac version!\n");
      ifc_bsd.ifc_len = (num_ifreq * sizeof(struct ifreq_bsd));    /* Adjust max buffer */
      ifc_bsd.ifc_buf = weaken(malloc)(ifc_bsd.ifc_len);
      PRINTF("numInterf Linux=%lu\n", num_ifreq);
      PRINTF("BSD   size=%lu\n", ifc_bsd.ifc_len);
      PRINTF("Linux size=%lu\n", ifc->ifc_len);
      if (!ifc_bsd.ifc_buf) {
        PRINTF("Malloc failed\n");
        return enomem();
      }
      PRINTF("Calling ioctl()\n");
      i = sys_ioctl(fd, SIOCGIFCONF, &ifc_bsd);
      PRINTF("rc=%d\n", i);
      if (i < 0) {
        weaken(free)(ifc_bsd.ifc_buf);
        return -1;
      }

      /* Number of interfaces returned */
      num_ifreq = ifc_bsd.ifc_len / sizeof(struct ifreq_bsd);
      for (i = 1; i < num_ifreq; ++i) {
        /* The first interface always match the same position */
        memcpy(ifc->ifc_req[i].ifr_name, ifc_bsd.ifc_req[i].ifr_name, IFNAMSIZ);
      }
      ifc->ifc_len = num_ifreq * sizeof(struct ifreq);
      weaken(free)(ifc_bsd.ifc_buf);
      return 0;
    }
  } else {
    // 100% compatible with Linux
    return sys_ioctl(fd, SIOCGIFCONF, ifc);
  }
}

/**
 * Returns information about network interfaces.
 *
 * @see ioctl(fd, SIOCGIFCONF, tio) dispatches here
 */
int ioctl_siocgifconf(int fd, struct ifconf *ifc) {
  if (!IsWindows()) {
    return ioctl_siocgifconf_sysv(fd, ifc);
  } else {
    return ioctl_siocgifconf_nt(fd, ifc);
  }
}

