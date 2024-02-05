/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "libc/nt/enum/formatmessageflags.h"
#include "libc/nt/enum/lang.h"
#include "libc/nt/errors.h"
#include "libc/nt/iphlpapi.h"
#include "libc/nt/memory.h"
#include "libc/nt/process.h"
#include "libc/nt/struct/ipadapteraddresses.h"
#include "libc/runtime/runtime.h"
#include "libc/serialize.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr6.h"
#include "libc/stdalign.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/sio.h"
#include "libc/sysv/consts/sock.h"

/**
 * @fileoverview prints detailed network interface info on windows
 */

const char *DescribeIfType(int x) {
  switch (x) {
    case kNtIfTypeOther:
      return "OTHER";
    case kNtIfTypeEthernetCsmacd:
      return "ETHERNET";
    case kNtIfTypeIso88025Tokenring:
      return "TOKENRING";
    case kNtIfTypePpp:
      return "PPP";
    case kNtIfTypeSoftwareLoopback:
      return "LOOPBACK";
    case kNtIfTypeAtm:
      return "ATM";
    case kNtIfTypeIeee80211:
      return "WIFI";
    case kNtIfTypeTunnel:
      return "TUNNEL";
    case kNtIfTypeIeee1394:
      return "FIREWIRE";
    default:
      return "WUT";
  }
}

const char *DescribeTunnelType(int x) {
  switch (x) {
    case kNtTunnelTypeNone:
      return "NONE";
    case kNtTunnelTypeOther:
      return "OTHER";
    case kNtTunnelTypeDirect:
      return "DIRECT";
    case kNtTunnelType6to4:
      return "6TO4";
    case kNtTunnelTypeIsatap:
      return "ISATAP";
    case kNtTunnelTypeTeredo:
      return "TEREDO";
    case kNtTunnelTypeIphttps:
      return "IPHTTPS";
    default:
      return "WUT";
  }
}

const char *DescribeOperStatus(int x) {
  switch (x) {
    case kNtIfOperStatusUp:
      return "UP";
    case kNtIfOperStatusDown:
      return "DOWN";
    case kNtIfOperStatusTesting:
      return "TESTING";
    case kNtIfOperStatusUnknown:
      return "UNKNOWN";
    case kNtIfOperStatusDormant:
      return "DORMANT";
    case kNtIfOperStatusNotPresent:
      return "NOTPRESENT";
    case kNtIfOperStatusLowerLayerDown:
      return "LOWERLAYERDOWN";
    default:
      return "WUT";
  }
}

const char *DescribeDadState(int x) {
  switch (x) {
    case kNtIpDadStateInvalid:
      return "INVALID";
    case kNtIpDadStateTentative:
      return "TENTATIVE";
    case kNtIpDadStateDuplicate:
      return "DUPLICATE";
    case kNtIpDadStateDeprecated:
      return "DEPRECATED";
    case kNtIpDadStatePreferred:
      return "PREFERRED";
    default:
      return "WUT";
  }
}

const char *DescribeSuffixOrigin(int x) {
  switch (x) {
    case kNtIpPrefixOriginOther:
      return "OTHER";
    case kNtIpPrefixOriginManual:
      return "MANUAL";
    case kNtIpPrefixOriginWellKnown:
      return "WELLKNOWN";
    case kNtIpPrefixOriginDhcp:
      return "DHCP";
    case kNtIpPrefixOriginRouterAdvertisement:
      return "ROUTERADVERTISEMENT";
    case kNtIpPrefixOriginUnchanged:
      return "UNCHANGED";
    default:
      return "WUT";
  }
}

const char *DescribePrefixOrigin(int x) {
  switch (x) {
    case kNtIpPrefixOriginOther:
      return "OTHER";
    case kNtIpPrefixOriginManual:
      return "MANUAL";
    case kNtIpPrefixOriginWellKnown:
      return "WELLKNOWN";
    case kNtIpPrefixOriginDhcp:
      return "DHCP";
    case kNtIpPrefixOriginRouterAdvertisement:
      return "ROUTERADVERTISEMENT";
    case kNtIpPrefixOriginUnchanged:
      return "UNCHANGED";
    default:
      return "WUT";
  }
}

const char *DescribeIpAdapterFlags(int x) {
  char *p;
  static char buf[256];
  p = buf;
  if (x & kNtIpAdapterDdnsEnabled) {
    if (p > buf) *p++ = ' ';
    p = stpcpy(p, "DDNS");
  }
  if (x & kNtIpAdapterDhcpv4Enabled) {
    if (p > buf) *p++ = ' ';
    p = stpcpy(p, "DHCPv4");
  }
  if (x & kNtIpAdapterReceiveOnly) {
    if (p > buf) *p++ = ' ';
    p = stpcpy(p, "RECV_ONLY");
  }
  if (x & kNtIpAdapterNoMulticast) {
    if (p > buf) *p++ = ' ';
    p = stpcpy(p, "NO_MULTICAST");
  }
  if (x & kNtIpAdapterIpv4Enabled) {
    if (p > buf) *p++ = ' ';
    p = stpcpy(p, "IPv4");
  }
  if (x & kNtIpAdapterIpv6Enabled) {
    if (p > buf) *p++ = ' ';
    p = stpcpy(p, "IPv6");
  }
  if (x & kNtIpAdapterIpv6Managed) {
    if (p > buf) *p++ = ' ';
    p = stpcpy(p, "IPv6_MANAGED");
  }
  if (x & kNtIpAdapterIpv6OtherStatefulConfig) {
    if (p > buf) *p++ = ' ';
    p = stpcpy(p, "IPv6_OTHER_STATEFUL_CONFIG");
  }
  if (x & kNtIpAdapterNetbiosOverTcpipEnabled) {
    if (p > buf) *p++ = ' ';
    p = stpcpy(p, "NETBIOS_OVER_TCP");
  }
  if (x & kNtIpAdapterRegisterAdapterSuffix) {
    if (p > buf) *p++ = ' ';
    p = stpcpy(p, "REGISTER_ADAPTER_SUFFIX");
  }
  return buf;
}

char *ConvertIpv6ToStr(const struct in6_addr *addr) {
  char *p;
  int i, t, a, b, c, d;
  static char buf[16 * 4 + 8];
  t = 0;
  i = 0;
  p = buf;
  for (i = 0; i < 16; i += 2) {
    switch (t) {
      case 0:
        if (!addr->s6_addr[i] && !addr->s6_addr[i + 1]) {
          t = 1;
          *p++ = ':';
          *p++ = ':';
          continue;
        } else if (i) {
          *p++ = ':';
        }
        break;
      case 1:
        if (!addr->s6_addr[i] && !addr->s6_addr[i + 1]) {
          continue;
        } else {
          t = 2;
        }
        break;
      case 2:
        *p++ = ':';
        break;
      default:
        __builtin_unreachable();
    }
    a = (addr->s6_addr[i + 0] & 0xF0) >> 4;
    b = (addr->s6_addr[i + 0] & 0x0F) >> 0;
    c = (addr->s6_addr[i + 1] & 0xF0) >> 4;
    d = (addr->s6_addr[i + 1] & 0x0F) >> 0;
    if (a) *p++ = "0123456789abcdef"[a];
    if (a || b) *p++ = "0123456789abcdef"[b];
    if (a || b || c) *p++ = "0123456789abcdef"[c];
    *p++ = "0123456789abcdef"[d];
  }
  *p = '\0';
  return buf;
}

void PrintSockAddr(struct sockaddr *addr) {
  if (addr->sa_family == AF_INET) {
    printf("%s", inet_ntoa(((struct sockaddr_in *)addr)->sin_addr));
  } else if (addr->sa_family == AF_INET6) {
    printf("%s", ConvertIpv6ToStr(&((struct sockaddr_in6 *)addr)->sin6_addr));
  } else {
    printf("[UNKNOWN FAMILY: %d]", addr->sa_family);
  }
}

void ShowWinNicCidrs(void) {
  // uint32_t dwSize = 0;
  uint32_t dwRetVal = 0;
  unsigned int i = 0;
  // default to unspecified address family (both)
  uint32_t family = AF_UNSPEC;
  void *lpMsgBuf = NULL;
  struct NtIpAdapterAddresses *pAddresses = NULL;
  uint32_t outBufLen = 0;
  uint32_t Iterations = 0;
  struct NtIpAdapterAddresses *p = NULL;
  struct NtIpAdapterUnicastAddress *pu = NULL;
  struct NtIpAdapterAnycastAddress *pa = NULL;
  struct NtIpAdapterMulticastAddress *pm = NULL;
  struct NtIpAdapterDnsServerAddress *pd = NULL;
  struct NtIpAdapterGatewayAddress *pg = NULL;
  struct NtIpAdapterWinsServerAddress *pw = NULL;
  struct NtIpAdapterPrefix *pp = NULL;
  outBufLen = 15000;

  do {
    pAddresses = malloc(outBufLen);
    if (pAddresses == NULL) {
      printf("Memory allocation failed for IP_ADAPTER_ADDRESSES struct\n");
      exit(1);
    }
    dwRetVal = GetAdaptersAddresses(
        family,
        kNtGaaFlagIncludePrefix | kNtGaaFlagIncludeWinsInfo |
            kNtGaaFlagIncludeGateways | kNtGaaFlagIncludeAllInterfaces,
        NULL, pAddresses, &outBufLen);
    if (dwRetVal == kNtErrorBufferOverflow) {
      free(pAddresses);
      pAddresses = NULL;
    } else {
      break;
    }
    Iterations++;
  } while ((dwRetVal == kNtErrorBufferOverflow) && (Iterations < 3));

  if (dwRetVal == kNtNoError) {
    for (p = pAddresses; p; p = p->Next) {
      /* if (p->OperStatus != kNtIfOperStatusUp) continue; */

      printf("\t%s %s %s\n", DescribeOperStatus(p->OperStatus),
             DescribeIfType(p->IfType), p->AdapterName);
      printf("\t%hs (%hs)\n", p->Description, p->FriendlyName);

      if (p->PhysicalAddressLength != 0) {
        printf("\tPhysical address: ");
        for (i = 0; i < (int)p->PhysicalAddressLength; i++) {
          if (i == (p->PhysicalAddressLength - 1))
            printf("%.2X\n", (int)p->PhysicalAddress[i]);
          else
            printf("%.2X-", (int)p->PhysicalAddress[i]);
        }
      }

      for (pu = p->FirstUnicastAddress; pu; pu = pu->Next) {
        printf("\tAddress: ");
        PrintSockAddr(pu->Address.lpSockaddr);
        printf("/%d %s %s %s VALID:%u PREFERRED:%u LEASE:%u",
               pu->OnLinkPrefixLength,
               inet_ntoa((struct in_addr){
                   htonl((uint32_t)-1 << (32 - pu->OnLinkPrefixLength))}),
               DescribePrefixOrigin(pu->PrefixOrigin),
               DescribeSuffixOrigin(pu->SuffixOrigin),
               DescribeDadState(pu->DadState), pu->ValidLifetime,
               pu->PreferredLifetime);
        printf("\n");
      }

      for (pg = p->FirstGatewayAddress; pg; pg = pg->Next) {
        printf("\tGateway: ");
        PrintSockAddr(pg->Address.lpSockaddr);
        printf("\n");
      }

      for (pd = p->FirstDnsServerAddress; pd; pd = pd->Next) {
        printf("\tDNS Server: ");
        PrintSockAddr(pd->Address.lpSockaddr);
        printf("\n");
      }
      if (p->DnsSuffix && p->DnsSuffix[0]) {
        printf("\tDNS Suffix: %hs\n", p->DnsSuffix);
      }

      for (pp = p->FirstPrefix; pp; pp = pp->Next) {
        printf("\tRoute: ");
        PrintSockAddr(pp->Address.lpSockaddr);
        printf("/%d", pp->PrefixLength);
        printf("\n");
      }

      for (pm = p->FirstMulticastAddress; pm; pm = pm->Next) {
        printf("\tMulticast: ");
        PrintSockAddr(pm->Address.lpSockaddr);
        printf("\n");
      }

      for (pa = p->FirstAnycastAddress; pa; pa = pa->Next) {
        printf("\tAnycast: ");
        PrintSockAddr(pa->Address.lpSockaddr);
        printf("\n");
      }

      for (pw = p->FirstWinsServerAddress; pw; pw = pw->Next) {
        printf("\tWINS: ");
        PrintSockAddr(pw->Address.lpSockaddr);
        printf("\n");
      }

      if (p->TunnelType) {
        printf("\tTunnel Type: %s\n", DescribeTunnelType(p->TunnelType));
      }

      printf("\tFlags: %ld %s\n", p->Flags, DescribeIpAdapterFlags(p->Flags));
      printf("\tMTU: %lu\n", p->Mtu);
      printf("\tTx Speed: %,lu\n", p->TransmitLinkSpeed);
      printf("\tRx Speed: %,lu\n", p->ReceiveLinkSpeed);
      printf("\n");
    }
  } else {
    printf("Call to GetAdaptersAddresses failed with error: %d\n", dwRetVal);
    if (dwRetVal == kNtErrorNoData)
      printf("\tNo addresses were found for the requested parameters\n");
    else {
      if (FormatMessage(
              kNtFormatMessageAllocateBuffer | kNtFormatMessageFromSystem |
                  kNtFormatMessageIgnoreInserts,
              NULL, dwRetVal, MAKELANGID(kNtLangNeutral, kNtSublangDefault),
              // Default language
              (char16_t *)&lpMsgBuf, 0, NULL)) {
        printf("\tError: %s", lpMsgBuf);
        LocalFree(lpMsgBuf);
        if (pAddresses) free(pAddresses);
        exit(1);
      }
    }
  }
  if (pAddresses) {
    free(pAddresses);
  }
}

int main(int argc, char *argv[]) {
  if (!IsWindows()) {
    fprintf(stderr, "error: this tool is intended for windows\n");
    return 1;
  }
  ShowWinNicCidrs();
  return 0;
}
