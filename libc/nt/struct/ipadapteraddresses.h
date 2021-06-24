#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IP_ADAPTER_ADDRESSES_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IP_ADAPTER_ADDRESSES_H_
#include "libc/nt/winsock.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/* Constants ----------------------------------------------------------- */
#define kNtMaxAdapterAddressLength  8
#define kNtMaxDnsSuffixStringLength 256
#define kNtMaxDhcpv6DuidLength      130

/* Values for the 'Flags' parameter of GetAdaptersAddresses */
#define kNtGaaFlagSkipUnicast               0x0001
#define kNtGaaFlagSkipAnycast               0x0002
#define kNtGaaFlagSkipMulticast             0x0004
#define kNtGaaFlagSkipDnsServer             0x0008
#define kNtGaaFlagIncludePrefix             0x0010
#define kNtGaaFlagSkipFriendlyName          0x0020
#define kNtGaaFlagIncludeWinsInfo           0x0040
#define kNtGaaFlagIncludeGateways           0x0080
#define kNtGaaFlagIncludeAllInterfaces      0x0100
#define kNtGaaFlagIncludeAllCompartments    0x0200
#define kNtGaaFlagIncludeTunnelBindingorder 0x0400
#define kNtGaaFlagSkipDnsInfo               0x0800

/* Values for the IfType parameter
 * See:
 * https://docs.microsoft.com/en-us/windows/win32/api/iptypes/ns-iptypes-ip_adapter_addresses_lh
 */
#define kNtIfTypeOther             1
#define kNtIfTypeEthernetCsmacd    6
#define kNtIfTypeIso88025Tokenring 9
#define kNtIfTypePpp               23
#define kNtIfTypeSoftwareLoopback  24
#define kNtIfTypeAtm               37
#define kNtIfTypeIeee80211         71 /* wifi */
#define kNtIfTypeTunnel            131
#define kNtIfTypeIeee1394          144 /* firewire */

/* Enums --------------------------------------------------------------- */
typedef enum {
  kNtIpPrefixOriginOther,
  kNtIpPrefixOriginManual,
  kNtIpPrefixOriginWellKnown,
  kNtIpPrefixOriginDhcp,
  kNtIpPrefixOriginRouterAdvertisement,
  kNtIpPrefixOriginUnchanged
} NtPrefixOrigin;

typedef enum {
  kNtNlsoOther,
  kNtNlsoManual,
  kNtNlsoWellKnown,
  kNtNlsoDhcp,
  kNtNlsoLinkLayerAddress,
  kNtNlsoRandom,
  kNtIpSuffixOriginOther,
  kNtIpSuffixOriginManual,
  kNtIpSuffixOriginWellKnown,
  kNtIpSuffixOriginDhcp,
  kNtIpSuffixOriginLinkLayerAddress,
  kNtIpSuffixOriginRandom,
  kNtIpSuffixOriginUnchanged
} NtSuffixOrigin;

typedef enum {
  kNtNldsInvalid,
  kNtNldsTentative,
  kNtNldsDuplicate,
  kNtNldsDeprecated,
  kNtNldsPreferred,
  kNtIpDadStateInvalid,
  kNtIpDadStateTentative,
  kNtIpDadStateDuplicate,
  kNtIpDadStateDeprecated,
  kNtIpDadStatePreferred
} NtDadState;

typedef enum {
  kNtIfOperStatusUp = 1,
  kNtIfOperStatusDown,
  kNtIfOperStatusTesting,
  kNtIfOperStatusUnknown,
  kNtIfOperStatusDormant,
  kNtIfOperStatusNotPresent,
  kNtIfOperStatusLowerLayerDown
} NtIfOperStatus;

typedef enum {
  kNtNetIfConnectionDedicated = 1,
  kNtNetIfConnectionPassive = 2,
  kNtNetIfConnectionDemand = 3,
  kNtNetIfConnectionMaximum = 4
} NtNetIfConnectionType;

typedef enum {
  kNtTunnelTypeNone = 0,
  kNtTunnelTypeOther = 1,
  kNtTunnelTypeDirect = 2,
  kNtTunnelType6to4 = 11,
  kNtTunnelTypeIsatap = 13,
  kNtTunnelTypeTeredo = 14,
  kNtTunnelTypeIphttps = 15
} NtTunnelType;

/* Inner Types --------------------------------------------------------- */
typedef struct _NtIpAdapterUnicastAddress {
  union {
    uint64_t Alignment;
    struct {
      uint32_t Length;
      uint32_t Flags;
    };
  };
  struct _NtIpAdapterUnicastAddress *Next;
  struct NtSocketAddress Address;
  NtPrefixOrigin PrefixOrigin;
  NtSuffixOrigin SuffixOrigin;
  NtDadState DadState;
  uint32_t ValidLifetime;
  uint32_t PreferredLifetime;
  uint32_t LeaseLifetime;
  uint8_t OnLinkPrefixLength;
} NtIpAdapterUnicastAddress;

typedef struct NtIpAdapterAnycastAddress {
  union {
    uint64_t Alignment;
    struct {
      uint32_t Length;
      uint32_t Flags;
    };
  };
  struct _NtIpAdapterAnycastAddress *Next;
  struct NtSocketAddress Address;
} NtIpAdapterAnycastAddress;

typedef struct NtIpAdapterMulticastAddress {
  union {
    uint64_t Alignment;
    struct {
      uint32_t Length;
      uint32_t Flags;
    };
  };
  struct _NtIpAdapterMulticastAddress *Next;
  struct NtSocketAddress Address;
} NtIpAdapterMulticastAddress;

typedef struct _NtIpAdapterDnsServerAddress {
  union {
    uint64_t Alignment;
    struct {
      uint32_t Length;
      uint32_t Reserved;
    };
  };
  struct _NtIpAdapterDnsServerAddress *Next;
  struct NtSocketAddress Address;
} NtIpAdapterDnsServerAddress;

typedef struct _NtIpAdapterPrefix {
  union {
    uint64_t Alignment;
    struct {
      uint32_t Length;
      uint32_t Flags;
    };
  };
  struct _NtIpAdapterPrefix *Next;
  struct NtSocketAddress Address;
  uint32_t PrefixLength;
} NtIpAdapterPrefix;

typedef struct _NtIpAdapterWinsServerAddress {
  union {
    uint64_t Alignment;
    struct {
      uint32_t Length;
      uint32_t Reserved;
    };
  };
  struct _NtIpAdapterWinsServerAddress *Next;
  struct NtSocketAddress Address;
} NtIpAdapterWinsServerAddress;

typedef struct _NtIpAdapterGatewayAddress {
  union {
    uint64_t Alignment;
    struct {
      uint32_t Length;
      uint32_t Reserved;
    };
  };
  struct _NtIpAdapterGatewayAddress *Next;
  struct NtSocketAddress Address;
} NtIpAdapterGatewayAddress;

typedef struct _NtGUID {
  uint32_t Data1;
  uint16_t Data2;
  uint16_t Data3;
  uint8_t Data4[8];
} NtGUID;

typedef union _NtNetLUID {
  uint64_t Value;
  struct {
    uint64_t Reserved : 24;
    uint64_t NetLuidIndex : 24;
    uint64_t IfType : 16;
  } Info;
} NtNetLUID;

typedef struct _NtIpAdapterDnsSuffix {
  struct _NtIpAdapterDnsSuffix *Next;
  uint16_t String[kNtMaxDnsSuffixStringLength];
} NtIpAdapterDnsSuffix;

/* Top level ----------------------------------------------------------- */
typedef struct _NtIpAdapterAddresses {
  union {
    uint64_t Alignment;
    struct {
      uint32_t Length;
      uint32_t IfIndex;
    };
  };
  struct _NtIpAdapterAddresses *Next;
  char *AdapterName;
  NtIpAdapterUnicastAddress *FirstUnicastAddress;
  NtIpAdapterAnycastAddress *FirstAnycastAddress;
  NtIpAdapterMulticastAddress *FirstMulticastAddress;
  NtIpAdapterDnsServerAddress *FirstDnsServerAddress;
  uint16_t *DnsSuffix;
  uint16_t *Description;
  uint16_t *FriendlyName;
  uint8_t PhysicalAddress[kNtMaxAdapterAddressLength];
  uint32_t PhysicalAddressLength;
  union {
    uint32_t Flags;
    struct {
      uint32_t DdnsEnabled : 1;
      uint32_t RegisterAdapterSuffix : 1;
      uint32_t Dhcpv4Enabled : 1;
      uint32_t ReceiveOnly : 1;
      uint32_t NoMulticast : 1;
      uint32_t Ipv6OtherStatefulConfig : 1;
      uint32_t NetbiosOverTcpipEnabled : 1;
      uint32_t Ipv4Enabled : 1;
      uint32_t Ipv6Enabled : 1;
      uint32_t Ipv6ManagedAddressConfigurationSupported : 1;
    };
  };
  uint32_t Mtu;
  uint32_t IfType;
  NtIfOperStatus OperStatus;
  uint32_t Ipv6IfIndex;
  uint32_t ZoneIndices[16];
  NtIpAdapterPrefix *FirstPrefix;
  uint64_t TransmitLinkSpeed;
  uint64_t ReceiveLinkSpeed;
  NtIpAdapterWinsServerAddress *FirstWinsServerAddress;
  NtIpAdapterGatewayAddress *FirstGatewayAddress;
  uint32_t Ipv4Metric;
  uint32_t Ipv6Metric;
  NtNetLUID Luid;
  struct NtSocketAddress Dhcpv4Server;
  uint32_t CompartmentId;
  NtGUID NetworkGuid;
  NtNetIfConnectionType ConnectionType;
  NtTunnelType TunnelType;
  struct NtSocketAddress Dhcpv6Server;
  uint8_t Dhcpv6ClientDuid[kNtMaxDhcpv6DuidLength];
  uint32_t Dhcpv6ClientDuidLength;
  uint32_t Dhcpv6Iaid;
  NtIpAdapterDnsSuffix *FirstDnsSuffix;
} NtIpAdapterAddresses;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IP_ADAPTER_ADDRESSES_H_ */
