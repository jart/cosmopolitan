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
#include "libc/assert.h"
#include "libc/bits/weaken.h"
#include "libc/calls/internal.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/sio.h"
#include "libc/sysv/errfuns.h"

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
  /*
   * We're 100% compatible with Linux.
   * BSD ABIs mainly differ by having sockaddr::sa_len
   * XNU uses a 32-bit length in a struct that's packed!
   */
  int i, rc, fam;
  char *b, *p, *e;
  char ifcBsd[16];
  struct ifreq *req;
  struct ifreq *end;
  uint32_t bufLen, ip;
  size_t numReq, bufMax;
  if (IsLinux()) return sys_ioctl(fd, SIOCGIFCONF, ifc);
  if (!weaken(malloc)) return enomem();
  bufMax = 15000; /* conservative guesstimate */
  if (!(b = weaken(malloc)(bufMax))) return enomem();
  memcpy(ifcBsd, &bufMax, 8);                /* ifc_len */
  memcpy(ifcBsd + (IsXnu() ? 4 : 8), &b, 8); /* ifc_buf */
  if ((rc = sys_ioctl(fd, SIOCGIFCONF, &ifcBsd)) != -1) {
    /*
     * On XNU the size of the struct ifreq is different than Linux.
     * On Linux is fixed (40 bytes), but on XNU the struct sockaddr
     * has variable length, making the whole struct ifreq a variable
     * sized record.
     */
    memcpy(&bufLen, b, 4);
    req = ifc->ifc_req;
    end = req + ifc->ifc_len / sizeof(*req);
    for (p = b, e = p + MIN(bufMax, READ32LE(ifcBsd)); p + 16 + 16 <= e;
         p += IsBsd() ? 16 + MAX(16, p[16] & 255) : 40) {
      fam = p[IsBsd() ? 17 : 16] & 255;
      if (fam != AF_INET) continue;
      ip = READ32BE(p + 20);
      memset(req, 0, sizeof(*req));
      memcpy(req->ifr_name, p, 16);
      memcpy(&req->ifr_addr, p + 16, 16);
      req->ifr_addr.sa_family = fam;
      ((struct sockaddr_in *)&req->ifr_addr)->sin_addr.s_addr = htonl(ip);
      ++req;
    }
    ifc->ifc_len = (char *)req - ifc->ifc_buf; /* Adjust len */
  }
  if (weaken(free)) weaken(free)(b);
  return rc;
}

/* Used for all the ioctl that returns sockaddr structure that
 * requires adjustment between Linux and XNU
 */
static int ioctl_siocgifaddr_sysv(int fd, uint64_t op, struct ifreq *ifr) {
  if (sys_ioctl(fd, op, ifr) == -1) return -1;
  if (IsBsd()) sockaddr2linux(&ifr->ifr_addr);
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
    /* Not supported - Unknown how to find out how to retrieve the destination
     * address of a PPP from the interface list returned by the
     * GetAdaptersAddresses function
     *
    return ioctl_siocgifdstaddr_nt(fd, ifc);
     */
  }
}

int ioctl_siocgifflags(int fd, void *ifr) {
  if (!IsWindows()) {
    /* Both XNU and Linux are for once compatible here... */
    return ioctl_default(fd, SIOCGIFFLAGS, ifr);
  } else {
    return ioctl_siocgifflags_nt(fd, (struct ifreq *)ifr);
  }
}
