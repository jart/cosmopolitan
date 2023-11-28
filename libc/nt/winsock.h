#ifndef COSMOPOLITAN_LIBC_NT_WINSOCK_H_
#define COSMOPOLITAN_LIBC_NT_WINSOCK_H_
#include "libc/nt/struct/fdset.h"
#include "libc/nt/struct/guid.h"
#include "libc/nt/struct/iovec.h"
#include "libc/nt/struct/overlapped.h"
#include "libc/nt/struct/pollfd.h"
#include "libc/nt/struct/timeval.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
/* ░▓█████████████████████████████████████████████▓▒
   ░█▓░░░░░░░░░▓██▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓██▓▒░
   ░█▓░ ░▒▒▒▒  ▓██▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒██▓▒▒
   ░█▓░ ░▓▓▓▒  ▓██▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒██▓▒▒
   ░█▓░       ░▓██▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓██▓▒▒
   ░███████████████████████████████████████████████▓▒▒
   ░█▓░                                          ▒█▓▒▒
   ░█▓░                                          ▒█▓▒▒
   ░█▓░          ░▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒░             ▒█▓▒▒
   ░█▓░        ░▒░                 ▒█▓░          ▒█▓▒▒
   ░█▓░      ░░░░    ░░░░░░░░      ▒▓▓▓▒░        ▒█▓▒▒
   ░█▓░    ░░░░  ░░░░░▒▒▓███▓░░░░░░░░▒▓▓▓▓▒      ▒█▓▒▒
   ░█▓░   ░▒▒  ░░░░░░░▒▒████▓░░░░░░░░░░▒██▓      ▒█▓▒▒
   ░█▓░   ░▒▒  ░░░░░░░▒▒▓▓▓▓▓░░░░░░░░░▒▒██▓      ▒█▓▒▒
   ░█▓░   ░▒▒   ░░▒▒▒▒░░░░░ ░▒▒▒▒░░░░░▒▒██▓      ▒█▓▒▒
   ░█▓░   ░▒▒   ░░▒▓█▓░░░░░░░▒▓██▓░░░░▒▒██▓      ▒█▓▒▒
   ░█▓░   ░▒▒   ░░▒▓█▓░░░░░░░▒▓██▓░░░░▒▒██▓      ▒█▓▒▒
   ░█▓░   ░▒▒   ░░▒▓█▓░░░░░░░▒▓██▓░░░░▒▒██▓   ░▓█▓▒▒▒▒
   ░█▓░   ░▒▒   ░░▒▓█▓░░░░░░░▒▓██▓░░░░░▒██▓   ░████▓▒░
   ░█▓░     ░░░░░░░░▒▒░░░░░░░░░▒▒░░░▒▒▓▓▒░░    ░░▓███▓▒░
   ░█▓░      ░░░░░░░░░░░░░░░░░░░░░░▒▓▓▓▒░        ▒████▓▒░░░░░░
   ░█▓░        ░░▒▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▒░░        ░▓▓▓▓██▒░░░░░░░░
   ░█▓░          ▒█████████████████▒  ▓█▓▒░      ▒█▓ ░█▓      ░▓▓░
   ░█▓░                              ░▓████▒░    ▒█▓▒░  ░░░░░░░  ▓█▓░
   ░█▓░                                ░▓████▒░  ░▒░  ░░░░░░░░░░░  ░█▓
   ░█▓                                    ▒███▓▒▒░  ░░░░░░░░░░░░░░░  ▒▓▓
   ░██████████████████████████████████████▓▒▓█▓░  ░░░░░░░░░░░░░░░░░░ ▒█▓
    ▒▒▒▒▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▒░ ░░░░░░░░░░░░░░░░░░░░▒█▓
         ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░▒██▒▒▒░░░░░░░░░░░░░░░░░░░░░▒█▓
                                          ░██▒▒▒▒▒░░░░░░░░░░░░░░░░░░░▒█▓
                                          ░▓▓▓▒▒▒▒▒▒░░░░░░░░░░░░░░░░▒▓█▓
                                            ░▓▓▓▒▒▒▒▒▒░░░░░░░░░░░▒▒▒▒▓▓▒
                                              ░██▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒  ░█▓
                                                 ▒█▓▒▒▒▒▒▒▒▒▒▒▒██▓▒░ ░█▓
                                                   ▒█████████████▓▒▒░  ░██▒
╔────────────────────────────────────────────────────────────────▀▀▀▀───▀▀▀▀─│─╗
│ cosmopolitan § new technology » winsock                                  ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define kNtWsaFlagOverlapped      0x01
#define kNtWsaFlagNoHandleInherit 0x80

#define kNtCompEqual   0
#define kNtCompNotless 1

#define kNtTfDisconnect       0x01
#define kNtTfReuseSocket      0x02
#define kNtTfWriteBehind      0x04
#define kNtTfUseDefaultWorker 0x00
#define kNtTfUseSystemThread  0x10
#define kNtTfUseKernelApc     0x20

#define kNtSoConnectTime          0x700C
#define kNtSoUpdateAcceptContext  0x700B
#define kNtSoUpdateConnectContext 0x7010

#define kNtNspNotifyImmediately 0
#define kNtNspNotifyHwnd        1
#define kNtNspNotifyEvent       2
#define kNtNspNotifyPort        3
#define kNtNspNotifyApc         4

COSMOPOLITAN_C_START_

struct NtMsgHdr {
  struct sockaddr *name;
  int32_t namelen;
  struct NtIovec *lpBuffers;
  uint32_t dwBufferCount;
  struct NtIovec Control;
  uint32_t dwFlags;
};

struct NtWsaData {
  uint16_t wVersion;
  uint16_t wHighVersion;
  uint16_t iMaxSockets;
  uint16_t iMaxUdpDg;
  char *lpVendorInfo;
  char szDescription[257];
  char szSystemStatus[129];
};

struct NtSocketAddress {
  struct sockaddr *lpSockaddr;
  int32_t iSockaddrLength;
};

struct NtSocketAddressList {
  int32_t iAddressCount;
  struct NtSocketAddress Address[1];
};

struct NtAddrInfoEx {  /* win8+ */
  int32_t ai_flags;    /* AI_PASSIVE, AI_CANONNAME, AI_NUMERICHOST */
  int32_t ai_family;   /* PF_XXX */
  int32_t ai_socktype; /* SOCK_XXX */
  int32_t ai_protocol;
  uint64_t ai_addrlen;
  char16_t *ai_canonname;
  struct sockaddr *ai_addr;
  void *ai_blob;
  uint64_t ai_bloblen;
  struct NtGuid *ai_provider;
  struct NtAddrInfoEx *ai_next;
  int32_t ai_version;          /* v2 */
  char16_t *ai_fqdn;           /* v2 */
  int32_t ai_interfaceindex;   /* v3 */
  int64_t ai_resolutionhandle; /* v4 */
};

struct NtWsaProtocolChain {
  int32_t ChainLen;
  uint32_t ChainEntries[7];
};

struct NtWsaProtocolInfo {
  uint32_t dwServiceFlags1;
  uint32_t dwServiceFlags2;
  uint32_t dwServiceFlags3;
  uint32_t dwServiceFlags4;
  uint32_t dwProviderFlags;
  struct NtGuid ProviderId;
  uint32_t dwCatalogEntryId;
  struct NtWsaProtocolChain ProtocolChain;
  int32_t iVersion;
  int32_t iAddressFamily;
  int32_t iMaxSockAddr;
  int32_t iMinSockAddr;
  int32_t iSocketType;
  int32_t iProtocol;
  int32_t iProtocolMaxOffset;
  int32_t iNetworkByteOrder;
  int32_t iSecurityScheme;
  uint32_t dwMessageSize;
  uint32_t dwProviderReserved;
  char16_t szProtocol[256];
};

struct NtFlowSpec {
  uint32_t TokenRate;          /* bytes/sec */
  uint32_t TokenBucketSize;    /* bytes */
  uint32_t PeakBandwidth;      /* bytes/sec */
  uint32_t Latency;            /* µs */
  uint32_t DelayVariation;     /* µs */
  uint32_t ServiceType;        /* kNtServicetypeXxx */
  uint32_t MaxSduSize;         /* bytes */
  uint32_t MinimumPolicedSize; /* bytes */
};

struct NtQos {
  struct NtFlowSpec SendingFlowspec;
  struct NtFlowSpec ReceivingFlowspec;
  struct NtIovec ProviderSpecific;
};

struct NtWsaVersion {
  uint32_t dwVersion;
  int ecHow;
};

struct NtAfProtocols {
  int32_t iAddressFamily;
  int32_t iProtocol;
};

struct NtBlob {
  uint32_t cbSize;
  uint8_t pBlobData;
};

struct NtCsAddrInfo {
  struct NtSocketAddress LocalAddr;
  struct NtSocketAddress RemoteAddr;
  int32_t iSocketType;
  int32_t iProtocol;
};

struct NtWsaQuerySet {
  uint32_t dwSize; /* of this */
  char16_t *lpszServiceInstanceName;
  struct NtGuid *lpServiceClassId;
  struct NtWsaVersion *lpVersion;
  char16_t *lpszComment;
  uint32_t dwNameSpace;
  struct NtGuid *lpNSProviderId;
  char16_t *lpszContext;
  uint32_t dwNumberOfProtocols;
  struct NtAfProtocols *lpafpProtocols /*[dwNumberOfProtocols]*/;
  char16_t *lpszQueryString;
  uint32_t dwNumberOfCsAddrs;
  struct NtCsAddrInfo *lpcsaBuffer /*[dwNumberOfCsAddrs]*/;
  uint32_t dwOutputFlags;
  struct NtBlob *lpBlob;
};

struct NtWsaNamespaceInfoEx {
  struct NtGuid NSProviderId;
  uint32_t dwNameSpace;
  bool32 fActive;
  uint32_t dwVersion;
  char16_t *lpszIdentifier;
  struct NtBlob *ProviderSpecific;
};

struct NtWsansClassInfo {
  char16_t *lpszName;
  uint32_t dwNameSpace;
  uint32_t dwValueType;
  uint32_t dwValueSize;
  void *lpValue;
};

struct NtWsaServiceClassInfo {
  struct NtGuid *lpServiceClassId;
  char16_t *lpszServiceClassName;
  uint32_t dwCount;
  struct NtWsansClassInfo *lpClassInfos;
};

struct NtWsaNetworkEvents {
  int32_t lNetworkEvents;
  int32_t iErrorCode[10];
};

struct NtTransmitFileBuffers {
  void *Head;
  uint32_t HeadLength;
  void *Tail;
  uint32_t TailLength;
};

typedef int (*NtConditionProc)(
    const struct NtIovec *lpCallerId, const struct NtIovec *lpCallerData,
    struct NtQos *inout_lpSQOS, struct NtQos *inout_lpGQOS,
    const struct NtIovec *lpCalleeId, const struct NtIovec *lpCalleeData,
    uint32_t *out_group, const uint32_t *dwCallbackData);

typedef void (*NtWsaOverlappedCompletionRoutine)(
    uint32_t dwError, uint32_t cbTransferred,
    const struct NtOverlapped *lpOverlapped, uint32_t dwFlags);

struct NtWsaCompletion {
  int Type;
  union {
    struct {
      int64_t hWnd;
      uint32_t uMsg;
      uintptr_t context;
    } WindowMessage;
    struct {
      struct NtOverlapped *lpOverlapped;
    } Event;
    struct {
      struct NtOverlapped *lpOverlapped;
      NtWsaOverlappedCompletionRoutine lpfnCompletionProc;
    } Apc;
    struct {
      struct NtOverlapped *lpOverlapped;
      int64_t hPort;
      uint32_t Key;
    } Port;
  } Parameters;
};

struct NtInterfaceInfo {
  uint64_t iiFlags;
  struct sockaddr_in iiAddress;
  struct sockaddr_in iiBroadcastAddress;
  struct sockaddr_in iiNetmask;
};

/**
 * Winsock2 prototypes.
 *
 * @note Some of the functions exported by WS2_32.DLL, e.g. bind(),
 *       overlap with the names used by System V. Prototypes for these
 *       functions are declared within their respective wrappers.
 */

int32_t WSAStartup(uint16_t wVersionRequested, struct NtWsaData *lpWSAData)
    paramsnonnull();

int WSACleanup(void);
int WSAGetLastError(void) nosideeffect;
void WSASetLastError(int);

int64_t __sys_socket_nt(int, int, int);
int __sys_bind_nt(uint64_t, const void *, int);
int __sys_closesocket_nt(uint64_t);
int __sys_getpeername_nt(uint64_t, void *, uint32_t *);
int __sys_getsockname_nt(uint64_t, void *, uint32_t *);
int __sys_getsockopt_nt(uint64_t, int, int, void *, uint32_t *);
int __sys_ioctlsocket_nt(uint64_t, int32_t, uint32_t *);
int __sys_listen_nt(uint64_t, int);
int __sys_setsockopt_nt(uint64_t, int, int, const void *, int);
int __sys_shutdown_nt(uint64_t, int);
int __sys_select_nt(int, struct NtFdSet *, struct NtFdSet *, struct NtFdSet *,
                    struct NtTimeval *);

uint64_t WSASocket(int af, int type, int protocol,
                   const struct NtWsaProtocolInfo *opt_lpProtocolInfo,
                   const uint32_t opt_group, uint32_t dwFlags) __wur;

int WSAConnect(uint64_t s, const struct sockaddr *name, const int namelen,
               const struct NtIovec *opt_lpCallerData,
               struct NtIovec *opt_out_lpCalleeData,
               const struct NtQos *opt_lpSQOS, const struct NtQos *opt_lpGQOS)
    paramsnonnull((2));

bool32 WSAConnectByName(uint64_t s, const char16_t *nodename,
                        const char16_t *servicename,
                        uint32_t *opt_inout_LocalAddressLength,
                        struct sockaddr *out_LocalAddress,
                        uint32_t *opt_inout_RemoteAddressLength,
                        struct sockaddr *out_RemoteAddress,
                        const struct NtTimeval *opt_timeout,
                        struct NtOverlapped *__Reserved) paramsnonnull((2, 3));

bool32 WSAConnectByList(uint64_t s,
                        const struct NtSocketAddressList *SocketAddress,
                        uint32_t *opt_inout_LocalAddressLength,
                        struct sockaddr *out_LocalAddress,
                        uint32_t *opt_inout_RemoteAddressLength,
                        struct sockaddr *out_RemoteAddress,
                        const struct NtTimeval *opt_timeout,
                        struct NtOverlapped *__Reserved) paramsnonnull((2));

int64_t WSAAccept(uint64_t s, struct sockaddr *out_addr,
                  int32_t *opt_inout_addrlen,
                  const NtConditionProc opt_lpfnCondition,
                  const uint32_t *opt_dwCallbackData) paramsnonnull((2)) __wur;

int WSASend(uint64_t s, const struct NtIovec *lpBuffers, uint32_t dwBufferCount,
            uint32_t *opt_out_lpNumberOfBytesSent, uint32_t dwFlags,
            struct NtOverlapped *opt_inout_lpOverlapped,
            const NtWsaOverlappedCompletionRoutine opt_lpCompletionRoutine)
    paramsnonnull((2));

int WSASendMsg(int64_t Handle, const struct NtMsgHdr *lpMsg, uint32_t dwFlags,
               uint32_t *opt_out_lpNumberOfBytesSent,
               struct NtOverlapped *opt_inout_lpOverlapped,
               const NtWsaOverlappedCompletionRoutine opt_lpCompletionRoutine)
    paramsnonnull((2));

int WSASendTo(uint64_t s, const struct NtIovec *lpBuffers,
              uint32_t dwBufferCount,
              uint32_t *opt_out_lpNumberOfBytesSent /* opt if !overlapped */,
              uint32_t dwFlags, const void *opt_tosockaddr,
              int32_t tosockaddrlen,
              struct NtOverlapped *opt_inout_lpOverlapped,
              const NtWsaOverlappedCompletionRoutine opt_lpCompletionRoutine)
    paramsnonnull((2));

int WSAPoll(struct sys_pollfd_nt *inout_fdArray, uint32_t nfds,
            signed timeout_ms) paramsnonnull();

int WSARecv(uint64_t s, const struct NtIovec *inout_lpBuffers,
            uint32_t dwBufferCount, uint32_t *opt_out_lpNumberOfBytesRecvd,
            uint32_t *inout_lpFlags,
            struct NtOverlapped *opt_inout_lpOverlapped,
            const NtWsaOverlappedCompletionRoutine opt_lpCompletionRoutine)
    paramsnonnull((2, 5));

int WSARecvFrom(uint64_t s, const struct NtIovec *inout_lpBuffers,
                uint32_t dwBufferCount, uint32_t *opt_out_lpNumberOfBytesRecvd,
                uint32_t *inout_lpFlags, void *out_fromsockaddr,
                uint32_t *opt_inout_fromsockaddrlen,
                struct NtOverlapped *opt_inout_lpOverlapped,
                const NtWsaOverlappedCompletionRoutine opt_lpCompletionRoutine)
    paramsnonnull((2, 5));

int WSARecvDisconnect(uint64_t s, struct NtIovec *out_InboundDisconnectData);
int WSASendDisconnect(int64_t s, struct NtIovec *opt_OutboundDisconnectData);

int WSADuplicateSocket(uint64_t s, uint32_t dwProcessId,
                       struct NtWsaProtocolInfo *out_lpProtocolInfo)
    paramsnonnull((3));

int WSAIoctl(uint64_t s, uint32_t dwIoControlCode, const void *lpvInBuffer,
             uint32_t cbInBuffer, void *out_lpvOutBuffer, uint32_t cbOutBuffer,
             uint32_t *out_lpcbBytesReturned,
             struct NtOverlapped *opt_inout_lpOverlapped,
             const NtWsaOverlappedCompletionRoutine opt_lpCompletionRoutine)
    paramsnonnull((5, 7));

int WSANSPIoctl(int64_t hLookup, uint32_t dwControlCode,
                const void *lpvInBuffer, uint32_t cbInBuffer,
                void *out_lpvOutBuffer, uint32_t cbOutBuffer,
                uint32_t *out_lpcbBytesReturned,
                const struct NtWsaCompletion *opt_lpCompletion)
    paramsnonnull((3, 5, 7));

int64_t WSACreateEvent(void) __wur;
bool32 WSACloseEvent(const int64_t hEvent);
bool32 WSAResetEvent(const int64_t hEvent);
bool32 WSASetEvent(const int64_t hEvent);

int WSAEventSelect(uint64_t s, const int64_t opt_hEventObject,
                   long lNetworkEvents);

uint32_t WSAWaitForMultipleEvents(uint32_t cEvents, const int64_t *lphEvents,
                                  bool32 fWaitAll, uint32_t dwTimeout_ms,
                                  bool32 fAlertable) paramsnonnull();

int WSAEnumNetworkEvents(uint64_t s, const int64_t hEventObject,
                         struct NtWsaNetworkEvents *out_lpNetworkEvents)
    paramsnonnull();

bool32 WSAGetOverlappedResult(uint64_t s,
                              const struct NtOverlapped *lpOverlapped,
                              uint32_t *out_lpcbTransfer, bool32 fWait,
                              uint32_t *out_lpdwFlags) paramsnonnull();

int WSAEnumProtocols(const int32_t *opt_lpiProtocols,
                     struct NtWsaProtocolInfo *out_lpProtocolBuffer,
                     uint32_t *inout_lpdwBufferLength) paramsnonnull();

bool32 WSAGetQOSByName(uint64_t s, const struct NtIovec *lpQOSName,
                       struct NtQos *out_lpQOS) paramsnonnull();

uint64_t WSAJoinLeaf(uint64_t s, const struct sockaddr *name, const int namelen,
                     const struct NtIovec *opt_lpCallerData,
                     struct NtIovec *opt_out_lpCalleeData,
                     const struct NtQos *opt_lpSQOS,
                     const struct NtQos *opt_lpGQOS, uint32_t dwFlags)
    paramsnonnull((2, 4));

int WSALookupServiceBegin(const struct NtWsaQuerySet *lpqsRestrictions,
                          uint32_t dwControlFlags, int64_t *out_lphLookup)
    paramsnonnull();

int WSALookupServiceNext(const int64_t hLookup, uint32_t dwControlFlags,
                         uint32_t *inout_lpdwBufferLength,
                         struct NtWsaQuerySet *out_lpqsResults) paramsnonnull();

int WSALookupServiceEnd(int64_t hLookup);

int WSAAddressToString(const struct sockaddr *lpsaAddress,
                       uint32_t dwAddressLength,
                       const struct NtWsaProtocolInfo *opt_lpProtocolInfo,
                       char16_t *out_lpszAddressString,
                       uint32_t *inout_lpdwAddressStringLength)
    paramsnonnull((1, 4, 5));

int WSAStringToAddress(const char16_t *AddressString, int AddressFamily,
                       const struct NtWsaProtocolInfo *opt_lpProtocolInfo,
                       struct sockaddr *out_lpAddress,
                       int *inout_lpAddressLength) paramsnonnull((1, 3, 4));

int WSAEnumNameSpaceProvidersEx(uint32_t *inout_lpdwBufferLength,
                                struct NtWsaNamespaceInfoEx *out_lpnspBuffer)
    paramsnonnull();

int WSAProviderConfigChange(
    int64_t *inout_lpNotificationHandle,
    struct NtOverlapped *opt_inout_lpOverlapped,
    NtWsaOverlappedCompletionRoutine opt_lpCompletionRoutine)
    paramsnonnull((1));

int WSAInstallServiceClass(
    const struct NtWsaServiceClassInfo *lpServiceClassInfo) paramsnonnull();

int WSARemoveServiceClass(const struct NtGuid *lpServiceClassId)
    paramsnonnull();

int WSAGetServiceClassInfo(const struct NtGuid *lpProviderId,
                           const struct NtGuid *lpServiceClassId,
                           uint32_t *inout_lpdwBufSize,
                           struct NtWsaServiceClassInfo *out_lpServiceClassInfo)
    paramsnonnull((1, 2, 3));

int WSASetService(const struct NtWsaQuerySet *lpqsRegInfo, int essoperation,
                  uint32_t dwControlFlags) paramsnonnull();

int /* success==0 */ WSAGetServiceClassNameByClassId(
    const struct NtGuid *lpServiceClassId, char16_t *out_lpszServiceClassName,
    uint32_t *inout_lpdwBufferLength) paramsnonnull();

void GetAcceptExSockaddrs(
    const void *lpOutputBuffer /*[recvsize+addrsize+addrlen]*/,
    uint32_t dwReceiveDataLength, uint32_t dwLocalAddressLength,
    uint32_t dwRemoteAddressLength,
    struct sockaddr **out_LocalSockaddr /*[*LocalSockaddrLength]*/,
    int *out_LocalSockaddrLength,
    struct sockaddr **out_RemoteSockaddr /*[*RemoteSockaddrLength]*/,
    int *out_RemoteSockaddrLength);

bool32 DisconnectEx(int64_t s, struct NtOverlapped *inout_opt_lpOverlapped,
                    uint32_t dwFlags, uint32_t dwReserved);

#if ShouldUseMsabiAttribute()
#include "libc/nt/thunk/winsock.inc"
#endif /* ShouldUseMsabiAttribute() */
COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_WINSOCK_H_ */
