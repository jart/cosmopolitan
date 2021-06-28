#ifndef COSMOPOLITAN_LIBC_HTTP_HTTP_H_
#define COSMOPOLITAN_LIBC_HTTP_HTTP_H_
#include "libc/time/struct/tm.h"

#define kHttpRequest  0
#define kHttpResponse 1

#define kHttpGet     1
#define kHttpHead    2
#define kHttpPost    3
#define kHttpPut     4
#define kHttpDelete  5
#define kHttpOptions 6
#define kHttpConnect 7
#define kHttpTrace   8
#define kHttpCopy    9
#define kHttpLock    10
#define kHttpMerge   11
#define kHttpMkcol   12
#define kHttpMove    13
#define kHttpNotify  14
#define kHttpPatch   15
#define kHttpReport  16
#define kHttpUnlock  17

#define kHttpHost                          0
#define kHttpCacheControl                  1
#define kHttpConnection                    2
#define kHttpAccept                        3
#define kHttpAcceptLanguage                4
#define kHttpAcceptEncoding                5
#define kHttpUserAgent                     6
#define kHttpReferer                       7
#define kHttpXForwardedFor                 8
#define kHttpOrigin                        9
#define kHttpUpgradeInsecureRequests       10
#define kHttpPragma                        11
#define kHttpCookie                        12
#define kHttpDnt                           13
#define kHttpSecGpc                        14
#define kHttpFrom                          15
#define kHttpIfModifiedSince               16
#define kHttpXRequestedWith                17
#define kHttpXForwardedHost                18
#define kHttpXForwardedProto               19
#define kHttpXCsrfToken                    20
#define kHttpSaveData                      21
#define kHttpRange                         22
#define kHttpContentLength                 23
#define kHttpContentType                   24
#define kHttpVary                          25
#define kHttpDate                          26
#define kHttpServer                        27
#define kHttpExpires                       28
#define kHttpContentEncoding               29
#define kHttpLastModified                  30
#define kHttpEtag                          31
#define kHttpAllow                         32
#define kHttpContentRange                  33
#define kHttpAcceptCharset                 34
#define kHttpAccessControlAllowCredentials 35
#define kHttpAccessControlAllowHeaders     36
#define kHttpAccessControlAllowMethods     37
#define kHttpAccessControlAllowOrigin      38
#define kHttpAccessControlMaxAge           39
#define kHttpAccessControlMethod           40
#define kHttpAccessControlRequestHeaders   41
#define kHttpAccessControlRequestMethod    42
#define kHttpAccessControlRequestMethods   43
#define kHttpAge                           44
#define kHttpAuthorization                 45
#define kHttpContentBase                   46
#define kHttpContentDescription            47
#define kHttpContentDisposition            48
#define kHttpContentLanguage               49
#define kHttpContentLocation               50
#define kHttpContentMd5                    51
#define kHttpExpect                        52
#define kHttpIfMatch                       53
#define kHttpIfNoneMatch                   54
#define kHttpIfRange                       55
#define kHttpIfUnmodifiedSince             56
#define kHttpKeepAlive                     57
#define kHttpLink                          58
#define kHttpLocation                      59
#define kHttpMaxForwards                   60
#define kHttpProxyAuthenticate             61
#define kHttpProxyAuthorization            62
#define kHttpProxyConnection               63
#define kHttpPublic                        64
#define kHttpRetryAfter                    65
#define kHttpTe                            66
#define kHttpTrailer                       67
#define kHttpTransferEncoding              68
#define kHttpUpgrade                       69
#define kHttpUri                           70
#define kHttpWarning                       71
#define kHttpWwwAuthenticate               72
#define kHttpVia                           73
#define kHttpStrictTransportSecurity       74
#define kHttpXFrameOptions                 75
#define kHttpXContentTypeOptions           76
#define kHttpAltSvc                        77
#define kHttpReferrerPolicy                78
#define kHttpXXssProtection                79
#define kHttpAcceptRanges                  80
#define kHttpSetCookie                     81
#define kHttpHeadersMax                    82

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct HttpSlice {
  short a, b;
};

struct HttpHeader {
  struct HttpSlice k;
  struct HttpSlice v;
};

struct HttpHeaders {
  unsigned n;
  struct HttpHeader *p;
};

struct HttpMessage {
  int i, a, status;
  unsigned char t;
  unsigned char type;
  unsigned char method;
  unsigned char version;
  struct HttpSlice k;
  struct HttpSlice uri;
  struct HttpSlice scratch;
  struct HttpSlice message;
  struct HttpSlice headers[kHttpHeadersMax];
  struct HttpSlice xmethod;
  struct HttpHeaders xheaders;
};

extern const char kHttpToken[256];
extern const char kHttpMethod[18][8];
extern const bool kHttpRepeatable[kHttpHeadersMax];

const char *GetHttpReason(int);
const char *GetHttpHeaderName(int);
int GetHttpHeader(const char *, size_t);
int GetHttpMethod(const char *, size_t);
void InitHttpMessage(struct HttpMessage *, int);
void DestroyHttpMessage(struct HttpMessage *);
int ParseHttpMessage(struct HttpMessage *, const char *, size_t);
bool HeaderHas(struct HttpMessage *, const char *, int, const char *, size_t);
int64_t ParseContentLength(const char *, size_t);
char *FormatHttpDateTime(char[hasatleast 30], struct tm *);
bool ParseHttpRange(const char *, size_t, long, long *, long *);
int64_t ParseHttpDateTime(const char *, size_t);
bool IsValidHttpToken(const char *, size_t);
bool IsAcceptablePath(const char *, size_t);
bool IsAcceptableHost(const char *, size_t);
bool IsAcceptablePort(const char *, size_t);
bool IsReasonablePath(const char *, size_t);
int64_t ParseIp(const char *, size_t);
int ParseForwarded(const char *, size_t, uint32_t *, uint16_t *);
bool IsMimeType(const char *, size_t, const char *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_HTTP_HTTP_H_ */
