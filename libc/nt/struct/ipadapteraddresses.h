#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IP_ADAPTER_ADDRESSES_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IP_ADAPTER_ADDRESSES_H_
#include "libc/nt/struct/guid.h"
#include "libc/nt/winsock.h"

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

#define kNtIpAdapterDdnsEnabled             0x0001
#define kNtIpAdapterRegisterAdapterSuffix   0x0002
#define kNtIpAdapterDhcpv4Enabled           0x0004
#define kNtIpAdapterReceiveOnly             0x0008
#define kNtIpAdapterNoMulticast             0x0010
#define kNtIpAdapterIpv6OtherStatefulConfig 0x0020
#define kNtIpAdapterNetbiosOverTcpipEnabled 0x0040
#define kNtIpAdapterIpv4Enabled             0x0080
#define kNtIpAdapterIpv6Enabled             0x0100
#define kNtIpAdapterIpv6Managed             0x0200

#define kNtIpPrefixOriginOther               0
#define kNtIpPrefixOriginManual              1
#define kNtIpPrefixOriginWellKnown           2
#define kNtIpPrefixOriginDhcp                3
#define kNtIpPrefixOriginRouterAdvertisement 4
#define kNtIpPrefixOriginUnchanged           16

#define kNtIpSuffixOriginOther            0
#define kNtIpSuffixOriginManual           1
#define kNtIpSuffixOriginWellKnown        2
#define kNtIpSuffixOriginDhcp             3
#define kNtIpSuffixOriginLinkLayerAddress 4
#define kNtIpSuffixOriginRandom           5
#define kNtIpSuffixOriginUnchanged        16

#define kNtIpDadStateInvalid    0
#define kNtIpDadStateTentative  1
#define kNtIpDadStateDuplicate  2
#define kNtIpDadStateDeprecated 3
#define kNtIpDadStatePreferred  4

#define kNtIfOperStatusUp             1
#define kNtIfOperStatusDown           2
#define kNtIfOperStatusTesting        3
#define kNtIfOperStatusUnknown        4
#define kNtIfOperStatusDormant        5
#define kNtIfOperStatusNotPresent     6
#define kNtIfOperStatusLowerLayerDown 7

#define kNtNetIfConnectionDedicated 1
#define kNtNetIfConnectionPassive   2
#define kNtNetIfConnectionDemand    3
#define kNtNetIfConnectionMaximum   4

#define kNtTunnelTypeNone    0
#define kNtTunnelTypeOther   1
#define kNtTunnelTypeDirect  2
#define kNtTunnelType6to4    11
#define kNtTunnelTypeIsatap  13
#define kNtTunnelTypeTeredo  14
#define kNtTunnelTypeIphttps 15

COSMOPOLITAN_C_START_

/* Inner Types --------------------------------------------------------- */

struct NtIpAdapterUnicastAddress {
  uint32_t Length;
  uint32_t Flags;
  struct NtIpAdapterUnicastAddress *Next;
  struct NtSocketAddress Address;
  uint32_t PrefixOrigin; /* kNtIpPrefixOrigin... */
  uint32_t SuffixOrigin; /* kNtIpSuffixOrigin... */
  uint32_t DadState;     /* kNtIpDadState... */
  uint32_t ValidLifetime;
  uint32_t PreferredLifetime;
  uint32_t LeaseLifetime;
  uint8_t OnLinkPrefixLength;
};

struct NtIpAdapterAnycastAddress {
  uint32_t Length;
  uint32_t Flags;
  struct NtIpAdapterAnycastAddress *Next;
  struct NtSocketAddress Address;
};

struct NtIpAdapterMulticastAddress {
  uint32_t Length;
  uint32_t Flags;
  struct NtIpAdapterMulticastAddress *Next;
  struct NtSocketAddress Address;
};

struct NtIpAdapterDnsServerAddress {
  uint32_t Length;
  uint32_t Reserved;
  struct NtIpAdapterDnsServerAddress *Next;
  struct NtSocketAddress Address;
};

struct NtIpAdapterPrefix {
  uint32_t Length;
  uint32_t Flags;
  struct NtIpAdapterPrefix *Next;
  struct NtSocketAddress Address;
  uint32_t PrefixLength;
};

struct NtIpAdapterWinsServerAddress {
  uint32_t Length;
  uint32_t Reserved;
  struct NtIpAdapterWinsServerAddress *Next;
  struct NtSocketAddress Address;
};

struct NtIpAdapterGatewayAddress {
  uint32_t Length;
  uint32_t Reserved;
  struct NtIpAdapterGatewayAddress *Next;
  struct NtSocketAddress Address;
};

struct NtIpAdapterDnsSuffix {
  struct NtIpAdapterDnsSuffix *Next;
  uint16_t String[kNtMaxDnsSuffixStringLength];
};

/* Top level ----------------------------------------------------------- */

struct NtIpAdapterAddresses {
  uint32_t Length;
  uint32_t IfIndex;
  struct NtIpAdapterAddresses *Next;
  char *AdapterName;
  struct NtIpAdapterUnicastAddress *FirstUnicastAddress;
  struct NtIpAdapterAnycastAddress *FirstAnycastAddress;
  struct NtIpAdapterMulticastAddress *FirstMulticastAddress;
  struct NtIpAdapterDnsServerAddress *FirstDnsServerAddress;
  uint16_t *DnsSuffix;
  uint16_t *Description;
  uint16_t *FriendlyName;
  uint8_t PhysicalAddress[kNtMaxAdapterAddressLength];
  uint32_t PhysicalAddressLength;
  uint32_t Flags; /* kNtIpAdapter... */
  uint32_t Mtu;
  uint32_t IfType;     /* kNtIfType... */
  uint32_t OperStatus; /* kNtIfOperStatus... */
  uint32_t Ipv6IfIndex;
  uint32_t ZoneIndices[16];
  struct NtIpAdapterPrefix *FirstPrefix;
  uint64_t TransmitLinkSpeed;
  uint64_t ReceiveLinkSpeed;
  struct NtIpAdapterWinsServerAddress *FirstWinsServerAddress;
  struct NtIpAdapterGatewayAddress *FirstGatewayAddress;
  uint32_t Ipv4Metric;
  uint32_t Ipv6Metric;
  uint64_t Luid; /* reserved(24bits),NetLuidIndex(24b),IfType(16b) */
  struct NtSocketAddress Dhcpv4Server;
  uint32_t CompartmentId;
  struct NtGuid NetworkGuid;
  uint32_t ConnectionType; /* kNtNetIfConnection... */
  uint32_t TunnelType;     /* kNtTunnelType... */
  struct NtSocketAddress Dhcpv6Server;
  uint8_t Dhcpv6ClientDuid[kNtMaxDhcpv6DuidLength];
  uint32_t Dhcpv6ClientDuidLength;
  uint32_t Dhcpv6Iaid;
  struct NtIpAdapterDnsSuffix *FirstDnsSuffix;
};

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IP_ADAPTER_ADDRESSES_H_ */
