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

int64_t ParseIp(const char *, size_t);
struct Cidr ParseCidr(const char *, size_t);
bool IsDodIp(uint32_t);
bool IsArinIp(uint32_t);
bool IsRipeIp(uint32_t);
bool IsApnicIp(uint32_t);
bool IsLacnicIp(uint32_t);
bool IsPublicIp(uint32_t);
bool IsPrivateIp(uint32_t);
bool IsAfrinicIp(uint32_t);
bool IsTestnetIp(uint32_t);
bool IsLoopbackIp(uint32_t);
bool IsMulticastIp(uint32_t);
bool IsAnonymousIp(uint32_t);
int CategorizeIp(uint32_t);
const char *GetIpCategoryName(int);
bool IsCloudflareIp(uint32_t);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_NET_HTTP_IP_H_ */
