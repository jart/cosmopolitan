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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/af.h"
#include "libc/sock/sock.h"
#include "libc/sock/internal.h"
#include "libc/sysv/errfuns.h"
#include "libc/sysv/consts/iff.h"
#include "libc/nt/winsock.h"
#include "libc/nt/errors.h"
#include "libc/nt/iphlpapi.h"
#include "libc/nt/struct/ipAdapterAddress.h"
#include "libc/nt/windows.h"        /* Needed for WideCharToMultiByte */
#include "libc/bits/weaken.h"
#include "libc/assert.h"

// TODO: Remove me
#include "libc/stdio/stdio.h"
#define PRINTF  weaken(printf)

#define MAX_INTERFACES  32

static int insertAdapterName(NtPIpAdapterAddresses aa);

/* Reference:
 *  - Description of ioctls: https://docs.microsoft.com/en-us/windows/win32/winsock/winsock-ioctls
 *  - Structure INTERFACE_INFO: https://docs.microsoft.com/en-us/windows/win32/api/ws2ipdef/ns-ws2ipdef-interface_info
 *  - WSAIoctl man page: https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsaioctl
 *  - Using SIOCGIFCONF in Win32: https://docs.microsoft.com/en-us/windows/win32/winsock/using-unix-ioctls-in-winsock
 */


/* Design note:
 * Because the Linux struct ifreq.ifr_name has a max length of 16, this could 
 * potentially lead to ambiguouity in interfaces.
 * E.g. "Ethernet Interface 1", "Ethernet Interface 2" they are both truncated
 *      as "Ethernet Interfa"
 * Since subsequent ioctl() (e.g. SIOCGIFFLAGS) uses this name to
 * identify the speficif interface, we need to make sure that all the returned
 * names are unique and have a way to corelate to the original adapter addresses
 * record.
 */
struct HostAdapterDesc {
    char name[IFNAMSIZ];    /* Given name */
    NtIpAdapterAddresses * addresses;   /* NULL = invalid record */
    short                  flags;       /* Holds the same value as ifr_flags */
};

NtIpAdapterAddresses * theHostAdapterAddress;
struct HostAdapterDesc theAdapterName[MAX_INTERFACES];


/* Given a short adapter name, look into theAdapterName to see if there is
 * an adapter with the same name. Returns the index of theAdapterName
 * if found, or -1 if not found
 */
static int findAdapterByName(const char *name) {
    int i;
    for (i = 0; (i < MAX_INTERFACES) && theAdapterName[i].addresses; ++i) {
        if (!strncmp(name, theAdapterName[i].name, IFNAMSIZ)) {
            /* Found */
            return i;
        }
    }
    /* Not found */
    return -1;
}

static void freeAdapterInfo(void) {
    if (theHostAdapterAddress) {
        /* Free any existing adapter informations */
        weaken(free)(theHostAdapterAddress);
        theHostAdapterAddress = NULL;
        memset(theAdapterName, 0, sizeof(theAdapterName));
    }
}


/* Sets theHostAdapterAddress.
 * Returns -1 in case of failure (setting errno appropriately), 0 if success */
static int _readAdapterAddresses(void) {
    NtPIpAdapterAddresses aa;
    uint32_t size, rc;
    short flags;
    int i;
    PRINTF("FABDEBUG> _readAdapterAddresses:\n");

    assert(theHostAdapterAddress == NULL);
    assert(weaken(GetAdaptersAddresses));
    assert(weaken(WideCharToMultiByte));

    /* Calculate the required data size
     * Note: alternatively you can use AF_UNSPEC to also return IPv6 interfaces 
     */
    rc = weaken(GetAdaptersAddresses)(AF_INET,  
             NT_GAA_FLAG_INCLUDE_PREFIX, 
             NULL,  /* Reserved */
             NULL,  /* Ptr */
             &size);
    if (rc != kNtErrorBufferOverflow) {
        PRINTF("FABDEBUG> \tGetAdaptersAddresses failed with error %d\n", WSAGetLastError());
        return ebadf();
    }
    PRINTF("FABDEBUG> \tsize required=%lu, allocating...\n", size);
    // TODO: Do we need to access malloc/free through a weak ref?
    theHostAdapterAddress = (NtIpAdapterAddresses *)weaken(malloc)(size);
    if (!theHostAdapterAddress) {
        PRINTF("FABDEBUG> \tmalloc failed\n");
        return enomem();
    }

    /* Re-run GetAdaptersAddresses this time with a valid buffer */
    rc = weaken(GetAdaptersAddresses)(AF_INET, 
        NT_GAA_FLAG_INCLUDE_PREFIX, 
        NULL, 
        theHostAdapterAddress, 
        &size);
    if (rc != kNtErrorSuccess) {
        PRINTF("FABDEBUG> GetAdaptersAddresses failed %d\n", WSAGetLastError());
        freeAdapterInfo();
        return efault();
    }

    /* Iterate through all the adapters and populate theAdapterName */
    for (aa = theHostAdapterAddress; aa != NULL; aa = aa->Next) {
        i = insertAdapterName(aa);
        if (i == -1) {
            PRINTF("FABDEBUG> Too many adapters\n");
            freeAdapterInfo();
            return enomem();
        }

        /* Calculate the flags for this adapter. Flags to consider:
         * IFF_UP
         * IFF_BROADCAST        ** TODO: We need to validate
         * IFF_LOOPBACK
         * IFF_POINTOPOINT
         * IFF_MULTICAST
         * IFF_RUNNING          ** Same as IFF_UP for now
         * IFF_PROMISC          ** NOT SUPPORTED, unknown how to retrieve it
         */
        flags = 0;
        if (aa->OperStatus == IfOperStatusUp) flags |= IFF_UP | IFF_RUNNING;
        if (aa->IfType == NT_IF_TYPE_PPP) flags |= IFF_POINTOPOINT;
        //if (aa->TunnelType != TUNNEL_TYPE_NONE) flags |= IFF_POINTOPOINT;
        if (aa->NoMulticast == 0) flags |= IFF_MULTICAST;
        if (aa->IfType == NT_IF_TYPE_SOFTWARE_LOOPBACK) flags |= IFF_LOOPBACK;
        if (aa->FirstPrefix != NULL) flags |= IFF_BROADCAST;

        /* Broadcast - Broadcast address should be detectable though the FirstPrefix
         * linked list
         */
        flags |= IFF_BROADCAST; /* Assume yes for all the interfaces */

        theAdapterName[i].flags = flags;
    }
    PRINTF("FABDEBUG> \tDone reading.\n");
    return 0;
}


#if 1

void printUnicastAddressList(NtPIpAdapterUnicastAddress addr) {
    const char * sep = "";
    for(; addr; addr=addr->Next) {
        PRINTF("%s%s", sep, weaken(inet_ntoa)(
                    ((struct sockaddr_in *)(addr->Address.lpSockaddr))->sin_addr));
        if (!sep[0]) {
            sep=", ";
        }
    }
}

#endif

static int insertAdapterName(NtPIpAdapterAddresses aa) {
    char name[IFNAMSIZ];
    int i;
    int sfx;

    memset(name, 0, sizeof(name));
    /* On Windows, wchar_t is 16 bit, Linux is 32 bit, cannot use wcstombs() */
    weaken(WideCharToMultiByte)(0, 0, aa->FriendlyName, -1, name, IFNAMSIZ, NULL, NULL);

    /* Ensure the name is unique */
    for (sfx = 0; sfx < 10; ++sfx) {
        i = findAdapterByName(name);
        if (i != -1) {
            /* Found a duplicate */
            name[strlen(name)-1] = '0' + sfx;
            continue;
        }
        /* Unique */
        for (i = 0; i < MAX_INTERFACES; ++i) {
            if (theAdapterName[i].addresses == NULL) {
                break;
            }
        }
        if (i == MAX_INTERFACES) {
            /* Too many interfaces */
            break;
        }
        strncpy(theAdapterName[i].name, name, IFNAMSIZ-1);
        theAdapterName[i].name[IFNAMSIZ-1] = '\0';
        theAdapterName[i].addresses = aa;
        return i;
    }
    /* Reached max of 10 duplicates */
    return -1;
}
            


/* Returns the index in theAdapterName if found or -1 if
 * an error occurred
 */
static int findOrInitAdapter(const char *name) {
    int i;
    if (!theHostAdapterAddress) {
        /* Never invoked GetAdaptersAddresses */
        if (_readAdapterAddresses() == -1) {
            return -1;
        }
    }

    i = findAdapterByName(name);
    if (i == -1) {
        PRINTF("FABDEBUG> Bad adapter\n");
        return ebadf();
    }
    return i;
}



textwindows int ioctl_siocgifconf_nt(int fd, struct ifconf *ifc) {
    NtPIpAdapterAddresses aa;
    struct ifreq *ptr;
    int i;

    PRINTF("---------------------------------------\n");
    PRINTF("Sizeof() = %d\n", sizeof(NtIpAdapterAddresses));
    PRINTF("Next = %d\n", offsetof(NtIpAdapterAddresses, Next));
    PRINTF("AdapterName = %d\n", offsetof(NtIpAdapterAddresses, AdapterName));
    PRINTF("FirstUnicastAddress = %d\n", offsetof(NtIpAdapterAddresses, FirstUnicastAddress));
    PRINTF("FirstAnycastAddress = %d\n", offsetof(NtIpAdapterAddresses, FirstAnycastAddress));
    PRINTF("FirstMulticastAddress = %d\n", offsetof(NtIpAdapterAddresses, FirstMulticastAddress));
    PRINTF("FirstDnsServerAddress = %d\n", offsetof(NtIpAdapterAddresses, FirstDnsServerAddress));
    PRINTF("DnsSuffix = %d\n", offsetof(NtIpAdapterAddresses, DnsSuffix));
    PRINTF("Description = %d\n", offsetof(NtIpAdapterAddresses, Description));
    PRINTF("FriendlyName = %d\n", offsetof(NtIpAdapterAddresses, FriendlyName));

    PRINTF("OperStatus = %d\n", offsetof(NtIpAdapterAddresses, OperStatus));
    PRINTF("Flags = %d\n", offsetof(NtIpAdapterAddresses, Flags));
    PRINTF("---------------------------------------\n");

    if (theHostAdapterAddress) {
        freeAdapterInfo();
    }
    if (_readAdapterAddresses() == -1) {
        return -1;
    }

    for (ptr = ifc->ifc_req, i=0; 
            ((char *)(ptr+1) - ifc->ifc_buf) < ifc->ifc_len; 
            ++i, ptr++) {

        aa = theAdapterName[i].addresses;
        if (!aa) {
            break;
        }
        memcpy(ptr->ifr_name, theAdapterName[i].name, IFNAMSIZ);

        PRINTF("FABDEBUG> #%d: Adapter name = %s\n", i, ptr->ifr_name);
        if (aa->FirstUnicastAddress) {
            ptr->ifr_addr = *aa->FirstUnicastAddress->Address.lpSockaddr;   /* Windows sockaddr is compatible with Linux */
            PRINTF("FABDEBUG> \tIP=");
            printUnicastAddressList(aa->FirstUnicastAddress);
            PRINTF("\n");
        }

    }
    ifc->ifc_len = (char *)ptr - ifc->ifc_buf;

    return 0;

}

/* Performs the SIOCGIFADDR operation */
int ioctl_siocgifaddr_nt(int fd, struct ifreq *ifr) {
    NtPIpAdapterAddresses aa;
    int i;

    i = findOrInitAdapter(ifr->ifr_name);
    if (i == -1) {
        return -1;
    }

    aa = theAdapterName[i].addresses;
    if (aa->FirstUnicastAddress) {
        ifr->ifr_addr = *aa->FirstUnicastAddress->Address.lpSockaddr;   /* Windows sockaddr is compatible with Linux */
    }
    return 0;
}

/* Performs the SIOCGIFFLAGS operation */
int ioctl_siocgifflags_nt(int fd, struct ifreq *ifr) {
    int i;
    
    i = findOrInitAdapter(ifr->ifr_name);
    if (i == -1) {
        return -1;
    }

    PRINTF(">>>>CAZZO=%d - %d\n", i, theAdapterName[i].flags);
    ifr->ifr_flags = theAdapterName[i].flags;

    return 0;
}


/* Performs the SIOCGIFNETMASK operation */
int ioctl_siocgifnetmask_nt(int fd, struct ifreq *ifr) {
    NtPIpAdapterAddresses aa;
    int i;

    i = findOrInitAdapter(ifr->ifr_name);
    if (i == -1) {
        return -1;
    }

    PRINTF("FABDEBUG>>>> NETMASK\n");
    aa = theAdapterName[i].addresses;
    /* According to the doc:
     *  ... On Windows Vista and later, the linked IP_ADAPTER_PREFIX structures pointed to 
     *  by the FirstPrefix member include three IP adapter prefixes for each IP address 
     *  assigned to the adapter. These include the host IP address prefix, the subnet IP 
     *  address prefix, and the subnet broadcast IP address prefix. In addition, for each 
     *  adapter there is a multicast address prefix and a broadcast address prefix.
     *
     *  So, in short, for each of the assigned addresses, the IP_ADAPTER_PREFIX objects are:
     *  #0: Host IP Address
     *  #1: Subnet
     *  #2: Broadcast
     *  #3: Multicast
     *  #4: Broadcast
     */
    if (aa->FirstPrefix) {
        int count;
        NtPIpAdapterPrefix prefix;
        for (count=0, prefix=aa->FirstPrefix; prefix; prefix=prefix->Next, ++count) {
            PRINTF("FABDEBUG>>>>\t%d: %s/%d\n", count, weaken(inet_ntoa)(
                    ((struct sockaddr_in *)(prefix->Address.lpSockaddr))->sin_addr), prefix->PrefixLength);
            if (count == 4) {
                ifr->ifr_netmask = *prefix->Address.lpSockaddr;
            }
        }
    }
    return 0;
}


