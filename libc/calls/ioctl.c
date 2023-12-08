/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/calls/termios.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/serialize.h"
#include "libc/intrin/cmpxchg.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/macros.internal.h"
#include "libc/mem/alloca.h"
#include "libc/mem/mem.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/filetype.h"
#include "libc/nt/errors.h"
#include "libc/nt/files.h"
#include "libc/nt/ipc.h"
#include "libc/nt/iphlpapi.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/ipadapteraddresses.h"
#include "libc/nt/winsock.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/sock/internal.h"
#include "libc/sock/struct/ifconf.h"
#include "libc/sock/struct/ifreq.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/fio.h"
#include "libc/sysv/consts/iff.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sio.h"
#include "libc/sysv/consts/termios.h"
#include "libc/sysv/errfuns.h"

/* Maximum number of unicast addresses handled for each interface */
#define MAX_UNICAST_ADDR 32
#define MAX_NAME_CLASH   ((int)('z' - 'a')) /* Allow a..z */

static struct HostAdapterInfoNode {
  struct HostAdapterInfoNode *next;
  char name[IFNAMSIZ]; /* Obtained from FriendlyName */
  struct sockaddr unicast;
  struct sockaddr netmask;
  struct sockaddr broadcast;
  short flags;
} *__hostInfo;

static int ioctl_default(int fd, unsigned long request, void *arg) {
  int rc;
  int64_t handle;
  if (!IsWindows()) {
    return sys_ioctl(fd, request, arg);
  } else if (__isfdopen(fd)) {
    if (g_fds.p[fd].kind == kFdSocket) {
      handle = g_fds.p[fd].handle;
      if ((rc = _weaken(__sys_ioctlsocket_nt)(handle, request, arg)) != -1) {
        return rc;
      } else {
        return _weaken(__winsockerr)();
      }
    } else {
      return eopnotsupp();
    }
  } else {
    return ebadf();
  }
}

static int ioctl_fionread(int fd, uint32_t *arg) {
  int rc;
  int64_t handle;
  if (!IsWindows()) {
    return sys_ioctl(fd, FIONREAD, arg);
  } else if (__isfdopen(fd)) {
    handle = g_fds.p[fd].handle;
    if (g_fds.p[fd].kind == kFdSocket) {
      if ((rc = _weaken(__sys_ioctlsocket_nt)(handle, FIONREAD, arg)) != -1) {
        return rc;
      } else {
        return _weaken(__winsockerr)();
      }
    } else if (g_fds.p[fd].kind == kFdConsole) {
      int bytes = CountConsoleInputBytes();
      *arg = MAX(0, bytes);
      return 0;
    } else if (g_fds.p[fd].kind == kFdDevNull) {
      *arg = 1;
      return 0;
    } else if (GetFileType(handle) == kNtFileTypePipe) {
      uint32_t avail;
      if (PeekNamedPipe(handle, 0, 0, 0, &avail, 0)) {
        *arg = avail;
        return 0;
      } else if (GetLastError() == kNtErrorBrokenPipe) {
        *arg = 0;  // win32 can give epipe on reader end
        return 0;
      } else {
        return __winerr();
      }
    } else {
      return eopnotsupp();
    }
  } else {
    return ebadf();
  }
}

/* Frees all the nodes of the _hostInfo */
static textwindows void freeHostInfo(void) {
  struct HostAdapterInfoNode *next, *node = __hostInfo;
  if (_weaken(free)) {
    while (node) {
      next = node->next;
      _weaken(free)(node);
      node = next;
    }
  }
  __hostInfo = NULL;
}

/* Given a short adapter name, look into __hostInfo to see if there is
 * an adapter with the same name. Returns the pointer to the HostAdapterInfoNode
 * if found, or NULL if not found
 */
static textwindows struct HostAdapterInfoNode *findAdapterByName(
    const char *name) {
  struct HostAdapterInfoNode *node = __hostInfo;
  while (node) {
    if (!strncmp(name, node->name, IFNAMSIZ)) {
      return node;
    }
    node = node->next;
  }
  return NULL;
}

/* Creates a new HostAdapterInfoNode object, initializes it from
 * the given adapter, unicast address and address prefixes
 * and insert it in the __hostInfo.
 * Increments the pointers to the unicast addresses and
 * the address prefixes
 * Returns NULL if an error occurred or the newly created
 * HostAdapterInfoNode object (last in the list)
 */
static textwindows struct HostAdapterInfoNode *appendHostInfo(
    struct HostAdapterInfoNode *parentInfoNode,
    const char *baseName, /* Max length = IFNAMSIZ-1 */
    const struct NtIpAdapterAddresses
        *aa, /* Top level adapter object being processed */
    struct NtIpAdapterUnicastAddress *
        *ptrUA, /* Ptr to ptr to unicast address list node */
    struct NtIpAdapterPrefix *
        *ptrAP,  /* Ptr to ptr to Adapter prefix list node */
    int count) { /* count is used to create a unique name in case of alias */

  struct HostAdapterInfoNode *temp;
  struct HostAdapterInfoNode *node;
  uint32_t ip, netmask, broadcast;
  struct sockaddr_in *a;
  int attemptNum;

  if (!_weaken(calloc) || !(node = _weaken(calloc)(1, sizeof(*node)))) {
    errno = ENOMEM;
    return NULL;
  }

  memcpy(node->name, baseName, IFNAMSIZ);

  /* Are there more than a single unicast address ? */
  if (count > 0 || ((*ptrUA)->Next != NULL)) {
    /* Yes, compose it using <baseName>:<count> */
    size_t nameLen = strlen(node->name);
    if (nameLen + 2 > IFNAMSIZ - 2) {
      /* Appending the ":x" will exceed the size, need to chop the end */
      nameLen -= 2;
    }
    node->name[nameLen - 2] = ':';
    node->name[nameLen - 1] = '0' + count;
    node->name[nameLen] = '\0';
  }

  /* Is there a name clash with other interfaces? */
  for (attemptNum = 0; attemptNum < MAX_NAME_CLASH; ++attemptNum) {
    temp = findAdapterByName(node->name);
    if (!temp) {
      break;
    } else {
      /* Yes, this name has been already used, append an extra
       * character to resolve conflict. Note since the max length
       * of the string now is IFNAMSIZ-2, we have just enough space for this.
       * E.g. 'Ethernet_1' -> 'Ethernet_1a'
       */
      size_t pos = strlen(node->name);
      node->name[pos] = 'a' + attemptNum;
      node->name[pos + 1] = '\0';
      /* Try again */
    }
  }

  if (attemptNum == MAX_NAME_CLASH) {
    /* Cannot resolve the conflict */
    if (_weaken(free)) {
      _weaken(free)(node);
    }
    errno = EEXIST;
    return NULL;
  }

  /* Finally we got a unique short and friendly name */
  node->unicast = *((*ptrUA)->Address.lpSockaddr);
  if (*ptrUA == aa->FirstUnicastAddress) {
    short flags;
    /* This is the first unicast address of this interface
     * calculate the flags for this adapter. Flags to consider:
     * IFF_UP
     * IFF_BROADCAST        ** TODO: We need to validate
     * IFF_LOOPBACK
     * IFF_POINTOPOINT
     * IFF_MULTICAST
     * IFF_RUNNING          ** Same as IFF_UP for now
     * IFF_PROMISC          ** NOT SUPPORTED, unknown how to retrieve it
     */
    flags = 0;
    if (aa->OperStatus == kNtIfOperStatusUp) flags |= IFF_UP | IFF_RUNNING;
    if (aa->IfType == kNtIfTypePpp) flags |= IFF_POINTOPOINT;
    if (!(aa->Flags & kNtIpAdapterNoMulticast)) flags |= IFF_MULTICAST;
    if (aa->IfType == kNtIfTypeSoftwareLoopback) flags |= IFF_LOOPBACK;
    if (aa->FirstPrefix) flags |= IFF_BROADCAST;
    node->flags = flags;
  } else {
    /* Copy from previous node */
    node->flags = parentInfoNode->flags;
  }

  ip = ntohl(
      ((struct sockaddr_in *)(*ptrUA)->Address.lpSockaddr)->sin_addr.s_addr);
  netmask = (uint32_t)-1 << (32 - (*ptrUA)->OnLinkPrefixLength);
  broadcast = (ip & netmask) | (~netmask & -1);

  a = (struct sockaddr_in *)&node->netmask;
  a->sin_family = AF_INET;
  a->sin_addr.s_addr = htonl(netmask);

  a = (struct sockaddr_in *)&node->broadcast;
  a->sin_family = AF_INET;
  a->sin_addr.s_addr = htonl(broadcast);

  /* Process the prefix and extract the netmask and broadcast */
  /* According to the doc:
   *
   *     On Windows Vista and later, the linked IP_ADAPTER_PREFIX
   *     structures pointed to by the FirstPrefix member include three
   *     IP adapter prefixes for each IP address assigned to the
   *     adapter. These include the host IP address prefix, the subnet
   *     IP address prefix, and the subnet broadcast IP address prefix.
   *     In addition, for each adapter there is a multicast address
   *     prefix and a broadcast address prefix.
   *                   -Source: MSDN on IP_ADAPTER_ADDRESSES_LH
   *
   * For example, interface "Ethernet", with 2 unicast addresses:
   *
   *  - 192.168.1.84
   *  - 192.168.5.99
   *
   * The Prefix list has 8 elements:
   *
   *  #1: 192.168.1.0/24      <- Network, use the PrefixLength for netmask
   *  #2: 192.168.1.84/32     <- Host IP
   *  #3: 192.168.1.255/32    <- Subnet broadcast
   *
   *  #4: 192.168.5.0/24      <- Network
   *  #5: 192.168.5.99/32     <- Host IP
   *  #6: 192.168.5.255/32    <- Subnet broadcast
   *
   *  #7: 224.0.0.0/4         <- Multicast
   *  #8: 255.255.255.255/32  <- Broadcast
   */

  if (ptrAP && *ptrAP) {
    *ptrAP = (*ptrAP)->Next; /* skip net ip */
    if (*ptrAP) {
      *ptrAP = (*ptrAP)->Next; /* skip host ip */
      if (*ptrAP) {
        node->broadcast = *((*ptrAP)->Address.lpSockaddr);
      }
    }
  }

  *ptrUA = (*ptrUA)->Next;

  /* Append this node to the last node (if any) */
  if (parentInfoNode) {
    parentInfoNode->next = node;
  }

  /* Success */
  return node;
}

/* Returns -1 in case of failure */
static textwindows int createHostInfo(
    struct NtIpAdapterAddresses *firstAdapter) {
  static bool once;
  struct NtIpAdapterAddresses *aa;
  struct NtIpAdapterUnicastAddress *ua;
  struct NtIpAdapterPrefix *ap;
  struct HostAdapterInfoNode *node = NULL;
  char baseName[IFNAMSIZ];
  int count, i;
  /* __hostInfo must be empty */
  unassert(__hostInfo == NULL);
  for (aa = firstAdapter; aa; aa = aa->Next) {
    /* Skip all the interfaces with no address and the ones that are not AF_INET
     */
    if (!aa->FirstUnicastAddress ||
        aa->FirstUnicastAddress->Address.lpSockaddr->sa_family != AF_INET) {
      continue;
    }
    /* Use max IFNAMSIZ-1 chars, leave the last char for eventual conflicts */
    tprecode16to8(baseName, IFNAMSIZ - 1, aa->FriendlyName);
    baseName[IFNAMSIZ - 2] = '\0';
    /* Replace any space with a '_' */
    for (i = 0; i < IFNAMSIZ - 2; ++i) {
      if (baseName[i] == ' ') baseName[i] = '_';
      if (!baseName[i]) break;
    }
    for (count = 0, ua = aa->FirstUnicastAddress, ap = aa->FirstPrefix;
         (ua != NULL) && (count < MAX_UNICAST_ADDR); ++count) {
      node = appendHostInfo(node, baseName, aa, &ua, &ap, count);
      if (!node) goto err;
      if (!__hostInfo) {
        __hostInfo = node;
        if (_cmpxchg(&once, false, true)) {
          atexit(freeHostInfo);
        }
      }
    }
    /* Note: do we need to process the remaining adapter prefix?
     *      ap       - points to broadcast addr
     *      ap->Next - points to interface multicast addr
     * Ignoring them for now
     */
  }
  return 0;
err:
  freeHostInfo();
  return -1;
}

static textwindows int readAdapterAddresses(void) {
  uint32_t size, rc;
  struct NtIpAdapterAddresses *aa = NULL;
  /*
   * Calculate the required data size
   * Note: alternatively you can use AF_UNSPEC to also return IPv6 interfaces
   */
  rc = GetAdaptersAddresses(AF_INET,
                            kNtGaaFlagSkipAnycast | kNtGaaFlagSkipMulticast |
                                kNtGaaFlagSkipDnsServer |
                                kNtGaaFlagIncludePrefix,
                            NULL, /* Reserved */
                            NULL, /* Ptr */
                            &size);
  if (rc != kNtErrorBufferOverflow) {
    ebadf();
    goto err;
  }
  if (!_weaken(malloc) ||
      !(aa = (struct NtIpAdapterAddresses *)_weaken(malloc)(size))) {
    enomem();
    goto err;
  }
  /* Re-run GetAdaptersAddresses this time with a valid buffer */
  rc = GetAdaptersAddresses(AF_INET,
                            kNtGaaFlagSkipAnycast | kNtGaaFlagSkipMulticast |
                                kNtGaaFlagSkipDnsServer |
                                kNtGaaFlagIncludePrefix,
                            // kNtGaaFlagIncludePrefix,
                            NULL, aa, &size);
  if (rc != kNtErrorSuccess) {
    errno = GetLastError();
    goto err;
  }
  if (createHostInfo(aa) == -1) {
    goto err;
  }
  if (_weaken(free)) {
    _weaken(free)(aa);
  }
  return 0;
err:
  if (_weaken(free)) {
    _weaken(free)(aa);
  }
  freeHostInfo();
  return -1;
}

static textwindows int ioctl_siocgifconf_nt(int fd, struct ifconf *ifc) {
  struct ifreq *ptr;
  struct HostAdapterInfoNode *node;
  if (__hostInfo) {
    freeHostInfo();
  }
  if (readAdapterAddresses() == -1) {
    return -1;
  }
  for (ptr = ifc->ifc_req, node = __hostInfo;
       (((char *)(ptr + 1) - ifc->ifc_buf) < ifc->ifc_len) && node;
       ptr++, node = node->next) {
    memcpy(ptr->ifr_name, node->name, IFNAMSIZ);
    memcpy(&ptr->ifr_addr, &node->unicast, sizeof(struct sockaddr));
  }
  ifc->ifc_len = (char *)ptr - ifc->ifc_buf;
  return 0;
}

/**
 * Returns unicast addresses.
 */
static textwindows int ioctl_siocgifaddr_nt(int fd, struct ifreq *ifr) {
  struct HostAdapterInfoNode *node;
  node = findAdapterByName(ifr->ifr_name);
  if (!node) return ebadf();
  memcpy(&ifr->ifr_addr, &node->unicast, sizeof(struct sockaddr));
  return 0;
}

/* Performs the SIOCGIFFLAGS operation */
static textwindows int ioctl_siocgifflags_nt(int fd, struct ifreq *ifr) {
  struct HostAdapterInfoNode *node;
  node = findAdapterByName(ifr->ifr_name);
  if (!node) return ebadf();
  ifr->ifr_flags = node->flags;
  return 0;
}

/* Performs the SIOCGIFNETMASK operation */
static textwindows int ioctl_siocgifnetmask_nt(int fd, struct ifreq *ifr) {
  struct HostAdapterInfoNode *node;
  node = findAdapterByName(ifr->ifr_name);
  if (!node) return ebadf();
  memcpy(&ifr->ifr_netmask, &node->netmask, sizeof(struct sockaddr));
  return 0;
}

/**
 * Returns broadcast address.
 */
static textwindows int ioctl_siocgifbrdaddr_nt(int fd, struct ifreq *ifr) {
  struct HostAdapterInfoNode *node;
  node = findAdapterByName(ifr->ifr_name);
  if (!node) return ebadf();
  memcpy(&ifr->ifr_broadaddr, &node->broadcast, sizeof(struct sockaddr));
  return 0;
}

static int ioctl_siocgifconf_sysv(int fd, struct ifconf *ifc) {
  /*
   * We're 100% compatible with Linux.
   * BSD ABIs mainly differ by having sockaddr::sa_len
   * XNU uses a 32-bit length in a struct that's packed!
   */
  int rc, fam;
  size_t bufMax;
  char *b, *p, *e;
  char ifcBsd[16];
  struct ifreq *req;
  uint32_t bufLen, ip;
  if (IsLinux()) {
    return sys_ioctl(fd, SIOCGIFCONF, ifc);
  }
#pragma GCC push_options
#pragma GCC diagnostic ignored "-Walloca-larger-than="
  bufMax = 15000; /* conservative guesstimate */
  b = alloca(bufMax);
  CheckLargeStackAllocation(b, bufMax);
#pragma GCC pop_options
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
    for (p = b, e = p + MIN(bufMax, READ32LE(ifcBsd)); p + 16 + 16 <= e;
         p += IsBsd() ? 16 + MAX(16, p[16] & 255) : 40) {
      fam = p[IsBsd() ? 17 : 16] & 255;
      if (fam != AF_INET) continue;
      ip = READ32BE(p + 20);
      bzero(req, sizeof(*req));
      memcpy(req->ifr_name, p, 16);
      memcpy(&req->ifr_addr, p + 16, 16);
      req->ifr_addr.sa_family = fam;
      ((struct sockaddr_in *)&req->ifr_addr)->sin_addr.s_addr = htonl(ip);
      ++req;
    }
    ifc->ifc_len = (char *)req - ifc->ifc_buf; /* Adjust len */
  }
  return rc;
}

static inline void ioctl_sockaddr2linux(void *saddr) {
  char *p;
  if (saddr) {
    p = saddr;
    p[0] = p[1];
    p[1] = 0;
  }
}

/*
 * Used for all the ioctl that returns sockaddr structure that
 * requires adjustment between Linux and XNU
 */
static int ioctl_siocgifaddr_sysv(int fd, uint64_t op, struct ifreq *ifr) {
  if (sys_ioctl(fd, op, ifr) == -1) return -1;
  if (IsBsd()) ioctl_sockaddr2linux(&ifr->ifr_addr);
  return 0;
}

static int ioctl_siocgifconf(int fd, void *arg) {
  if (!IsWindows()) {
    return ioctl_siocgifconf_sysv(fd, arg);
  } else {
    return ioctl_siocgifconf_nt(fd, arg);
  }
}

static int ioctl_siocgifaddr(int fd, void *arg) {
  if (!IsWindows()) {
    return ioctl_siocgifaddr_sysv(fd, SIOCGIFADDR, arg);
  } else {
    return ioctl_siocgifaddr_nt(fd, arg);
  }
}

static int ioctl_siocgifnetmask(int fd, void *arg) {
  if (!IsWindows()) {
    return ioctl_siocgifaddr_sysv(fd, SIOCGIFNETMASK, arg);
  } else {
    return ioctl_siocgifnetmask_nt(fd, arg);
  }
}

static int ioctl_siocgifbrdaddr(int fd, void *arg) {
  if (!IsWindows()) {
    return ioctl_siocgifaddr_sysv(fd, SIOCGIFBRDADDR, arg);
  } else {
    return ioctl_siocgifbrdaddr_nt(fd, arg);
  }
}

static int ioctl_siocgifdstaddr(int fd, void *arg) {
  if (!IsWindows()) {
    return ioctl_siocgifaddr_sysv(fd, SIOCGIFDSTADDR, arg);
  } else {
    return enotsup();
    /* Not supported - Unknown how to find out how to retrieve the destination
     * address of a PPP from the interface list returned by the
     * GetAdaptersAddresses function
     *
     return ioctl_siocgifdstaddr_nt(fd, arg);
     */
  }
}

static int ioctl_siocgifflags(int fd, void *arg) {
  if (!IsWindows()) {
    /* Both XNU and Linux are for once compatible here... */
    return ioctl_default(fd, SIOCGIFFLAGS, arg);
  } else {
    return ioctl_siocgifflags_nt(fd, arg);
  }
}

/**
 * Performs special i/o operation on file descriptor.
 *
 * The following i/o requests are available.
 *
 * - `FIONREAD` takes an `int *` and returns how many bytes of input are
 *   available on a terminal/socket/pipe, waiting to be read. Be sure to
 *   only use it on the reading end of a pipe.
 *
 * - `TIOCGWINSZ` populates `struct winsize *` with the dimensions of
 *   your teletypewriter. It's an alias for tcgetwinsize().
 *
 * - `TIOCSWINSZ` with the dimensions of your teletypewriter to `struct
 *   winsize *`. It's an alias for tcsetwinsize().
 *
 * - `TIOCOUTQ` takes an `int *` and returns the number of bytes in the
 *   terminal's output buffer. Only available on UNIX.
 *
 * - `TIOCSTI` takes a `const char *` and may be used to fake input to a
 *   tty. This API isn't available on OpenBSD. Only available on UNIX.
 *
 * - `TIOCNOTTY` takes an `int tty_fd` arg and makes it the controlling
 *   terminal of the calling process, which should have called setsid()
 *   beforehand.
 *
 * - `TIOCNOTTY` to give up the controlling terminal. Only available on
 *   UNIX.
 *
 * - `TIOCNXCL` to give up exclusive mode on terminal. Only available on
 *   UNIX.
 *
 * - `SIOCGIFCONF` takes an struct ifconf object of a given size,
 *   whose arg is `struct ifconf *`. It implements the Linux style
 *   and modifies the following:
 *   - ifc_len: set it to the number of valid ifreq structures
 *     representingthe interfaces
 *   - ifc_ifcu.ifcu_req: sets the name of the interface for each
 *     interface
 *   The ifc_len is an input/output parameter: set it to the total
 *   size of the ifcu_buf (ifcu_req) buffer on input.
 *
 * - `SIOCGIFNETMASK` populates a `struct ifconf *` record with the
 *   network interface mask. This data structure should be obtained by
 *   calling `SIOCGIFCONF`.
 *
 * - `SIOCGIFBRDADDR` populates a `struct ifconf *` record with the
 *   network broadcast addr. This data structure should be obtained by
 *   calling `SIOCGIFCONF`.
 *
 * - `FIONBIO` isn't polyfilled; use `fcntl(F_SETFL, O_NONBLOCK)`
 * - `FIOCLEX` isn't polyfilled; use `fcntl(F_SETFD, FD_CLOEXEC)`
 * - `FIONCLEX` isn't polyfilled; use `fcntl(F_SETFD, 0)`
 * - `TIOCSCTTY` isn't polyfilled; use `login_tty()`
 * - `TCGETS` isn't polyfilled; use tcgetattr()
 * - `TCSETS` isn't polyfilled; use tcsetattr()
 * - `TCSETSW` isn't polyfilled; use tcsetattr()
 * - `TCSETSF` isn't polyfilled; use tcsetattr()
 * - `TCXONC` isn't polyfilled; use tcflow()
 * - `TCSBRK` isn't polyfilled; use tcdrain()
 * - `TCFLSH` isn't polyfilled; use tcflush()
 * - `TIOCGPTN` isn't polyfilled; use ptsname()
 * - `TIOCGSID` isn't polyfilled; use tcgetsid()
 * - `TCSBRK` isn't polyfilled; use tcsendbreak()
 * - `TCSBRK` isn't polyfilled; use tcsendbreak()
 * - `TIOCSPGRP` isn't polyfilled; use tcsetpgrp()
 * - `TIOCSPTLCK` isn't polyfilled; use unlockpt()
 *
 * @restartable
 * @vforksafe
 */
int ioctl(int fd, unsigned long request, ...) {
  int rc;
  void *arg;
  va_list va;
  va_start(va, request);
  arg = va_arg(va, void *);
  va_end(va);
  if (request == FIONREAD) {
    rc = ioctl_fionread(fd, arg);
  } else if (request == TIOCGWINSZ) {
    return tcgetwinsize(fd, arg);
  } else if (request == TIOCSWINSZ) {
    return tcsetwinsize(fd, arg);
  } else if (request == SIOCGIFCONF) {
    rc = ioctl_siocgifconf(fd, arg);
  } else if (request == SIOCGIFADDR) {
    rc = ioctl_siocgifaddr(fd, arg);
  } else if (request == SIOCGIFNETMASK) {
    rc = ioctl_siocgifnetmask(fd, arg);
  } else if (request == SIOCGIFBRDADDR) {
    rc = ioctl_siocgifbrdaddr(fd, arg);
  } else if (request == SIOCGIFDSTADDR) {
    rc = ioctl_siocgifdstaddr(fd, arg);
  } else if (request == SIOCGIFFLAGS) {
    rc = ioctl_siocgifflags(fd, arg);
  } else {
    rc = ioctl_default(fd, request, arg);
  }
  STRACE("ioctl(%d, %#lx, %p) → %d% m", fd, request, arg, rc);
  return rc;
}
