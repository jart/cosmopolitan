#ifndef COSMOPOLITAN_NET_HTTP_IP_H_
#define COSMOPOLITAN_NET_HTTP_IP_H_

#define kIpUnknown    0
#define kIpMulticast  1
#define kIpLoopback   2
#define kIpPrivate    3
#define kIpTestnet    4
#define kIpAfrinic    5
#define kIpLacnic     6
#define kIpApnic      7
#define kIpArin       8
#define kIpRipe       9
#define kIpDod        10
#define kIpAtt        11
#define kIpApple      12
#define kIpFord       13
#define kIpCogent     14
#define kIpPrudential 15
#define kIpUsps       16
#define kIpComcast    17
#define kIpFuture     18
#define kIpAnonymous  19

COSMOPOLITAN_C_START_

struct Cidr {
  int64_t addr;
  int cidr;
};

int64_t ParseIp(const char *, size_t) libcesque;
struct Cidr ParseCidr(const char *, size_t) libcesque;
bool IsDodIp(uint32_t) libcesque;
bool IsArinIp(uint32_t) libcesque;
bool IsRipeIp(uint32_t) libcesque;
bool IsApnicIp(uint32_t) libcesque;
bool IsLacnicIp(uint32_t) libcesque;
bool IsPublicIp(uint32_t) libcesque;
bool IsPrivateIp(uint32_t) libcesque;
bool IsAfrinicIp(uint32_t) libcesque;
bool IsTestnetIp(uint32_t) libcesque;
bool IsLoopbackIp(uint32_t) libcesque;
bool IsMulticastIp(uint32_t) libcesque;
bool IsAnonymousIp(uint32_t) libcesque;
int CategorizeIp(uint32_t) libcesque;
const char *GetIpCategoryName(int) libcesque;
bool IsCloudflareIp(uint32_t) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_NET_HTTP_IP_H_ */
