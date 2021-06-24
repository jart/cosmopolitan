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

/* SIOCGIFCONF:
 * Takes an struct ifconf object of a given size
 * Modifies the following:
 *  - ifc_len: set it to the number of valid ifreq structures representing
 *    the interfaces
 *  - ifc_ifcu.ifcu_req: sets the name of the interface for each interface
 * The ifc_len is an input/output parameter: set it to the total size of 
 * the ifcu_buf (ifcu_req) buffer on input.
 */
int ioctl_default(int, uint64_t, void *) hidden;
int ioctl_siocgifconf_nt(int, struct ifconf *) hidden;
int ioctl_siocgifaddr_nt(int, struct ifreq *) hidden;
int ioctl_siocgifflags_nt(int, struct ifreq *) hidden;
int ioctl_siocgifnetmask_nt(int, struct ifreq *) hidden;
int ioctl_siocgifbrdaddr_nt(int, struct ifreq *) hidden;

static int ioctl_siocgifconf_sysv(int fd, struct ifconf *ifc) {
  if (IsBsd()) {
    /* BSD uses a slightly different memory structure where:
     * - sizeof(struct ifreq) is 32 bytes instead of 40 bytes
     * - ifc.ifc_len is uint32_t instead of uint64_t
     * - struct ifconf is #pragma pack(4) (instead of the default pack(8))
     */
    int i;
    char *buf;      /* Temporary buffer to store ifreq */
    char *pBsdReq;  /* Scan through buf records */
    char *pBsdEnd;  /* End of the ifreq */

    struct ifconf_bsd ifc_bsd;
    struct ifreq_bsd *bsdReq;
    struct ifreq *req;
    size_t numReq = ifc->ifc_len / sizeof(struct ifreq);

    if (!weaken(malloc)) {
      return enomem();
    } 
    ifc_bsd.ifc_len = numReq * sizeof(struct ifreq_bsd);
    buf = weaken(malloc)(ifc_bsd.ifc_len);
    if (!buf) {
      return enomem();
    }
    ifc_bsd.ifc_buf = buf;

    if ((i = sys_ioctl(fd, SIOCGIFCONF, &ifc_bsd)) < 0) {
      weaken(free)(buf);
      return -1;
    }

    /* On BSD the size of the struct ifreq is different than Linux.
     * On Linux is fixed (40 bytes), but on BSD the struct sockaddr
     * has variable length, making the whole struct ifreq a variable
     * sized record.
     */
    for (pBsdReq = buf, pBsdEnd = buf + ifc_bsd.ifc_len, req = ifc->ifc_req;
        pBsdReq < pBsdEnd;
        ++req) {
      bsdReq = (struct ifreq_bsd *)pBsdReq;
      memcpy(req->ifr_name, bsdReq->ifr_name, IFNAMSIZ);
      memcpy(&req->ifr_addr, &bsdReq->ifr_addr, sizeof(struct sockaddr_bsd));
      sockaddr2linux(&req->ifr_addr);

      pBsdReq += IFNAMSIZ + bsdReq->ifr_addr.sa_len;
    }

    /* Adjust len */
    ifc->ifc_len = (size_t)((char *)req - ifc->ifc_buf);
    weaken(free)(buf);
    return 0;

  } else {
    /* 100% compatible with Linux */
    return sys_ioctl(fd, SIOCGIFCONF, ifc);
  }
}

/* Used for all the ioctl that returns sockaddr structure that
 * requires adjustment between Linux and BSD
 */
static int ioctl_siocgifaddr_sysv(int fd, uint64_t op, struct ifreq *ifr) {
  int i;

  if (IsBsd()) {
    sockaddr2bsd(&ifr->ifr_addr);
  }
  if ((i = sys_ioctl(fd, op, ifr)) < 0) {
    return -1;
  }
  if (IsBsd()) {
    sockaddr2linux(&ifr->ifr_addr);
  }
  return 0;
}

/**
 * Returns information about network interfaces.
 *
 * @see ioctl(fd, SIOCGIFCONF, tio) dispatches here
 */
int ioctl_siocgifconf(int fd, void *ifc) {
  if (!IsWindows()) {
    return ioctl_siocgifconf_sysv(fd, (struct ifconf *)ifc);
  } else {
    return ioctl_siocgifconf_nt(fd, ifc);
  }
}

int ioctl_siocgifaddr(int fd, void *ifr) {
  if (!IsWindows()) {
    return ioctl_siocgifaddr_sysv(fd, SIOCGIFADDR, (struct ifreq *)ifr);
  } else {
    return ioctl_siocgifaddr_nt(fd, (struct ifreq *)ifr);
  }
}

int ioctl_siocgifnetmask(int fd, void *ifr) {
  if (!IsWindows()) {
    return ioctl_siocgifaddr_sysv(fd, SIOCGIFNETMASK, (struct ifreq *)ifr);
  } else {
    return ioctl_siocgifnetmask_nt(fd, (struct ifreq *)ifr);
  }
}

int ioctl_siocgifbrdaddr(int fd, void *ifr) {
  if (!IsWindows()) {
    return ioctl_siocgifaddr_sysv(fd, SIOCGIFBRDADDR, (struct ifreq *)ifr);
  } else {
    return ioctl_siocgifbrdaddr_nt(fd, (struct ifreq *)ifr);
  }
}

int ioctl_siocgifdstaddr(int fd, void *ifr) {
  if (!IsWindows()) {
    return ioctl_siocgifaddr_sysv(fd, SIOCGIFDSTADDR, (struct ifreq *)ifr);
  } else {
    return enotsup();
    /* Not supported - TODO: Find out how to retrieve the destination
     * address of a PPP from the interface list returned by the
     * GetAdaptersAddresses function
     *
    return ioctl_siocgifbrdaddr_nt(fd, ifc);

     */
  }
}

int ioctl_siocgifflags(int fd, void *ifr) {
  if (!IsWindows()) {
    /* Both BSD and Linux are for once compatible here... */
    return ioctl_default(fd, SIOCGIFFLAGS, ifr);
  } else {
    return ioctl_siocgifflags_nt(fd, (struct ifreq *)ifr);
  }
}
