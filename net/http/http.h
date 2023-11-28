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

#define kHttpStateStart   0
#define kHttpStateMethod  1
#define kHttpStateUri     2
#define kHttpStateVersion 3
#define kHttpStateStatus  4
#define kHttpStateMessage 5
#define kHttpStateName    6
#define kHttpStateColon   7
#define kHttpStateValue   8
#define kHttpStateCr      9
#define kHttpStateLf1     10
#define kHttpStateLf2     11

#define kHttpClientStateHeaders      0
#define kHttpClientStateBody         1
#define kHttpClientStateBodyChunked  2
#define kHttpClientStateBodyLengthed 3

#define kHttpStateChunkStart   0
#define kHttpStateChunkSize    1
#define kHttpStateChunkExt     2
#define kHttpStateChunkLf1     3
#define kHttpStateChunk        4
#define kHttpStateChunkCr2     5
#define kHttpStateChunkLf2     6
#define kHttpStateTrailerStart 7
#define kHttpStateTrailer      8
#define kHttpStateTrailerLf1   9
#define kHttpStateTrailerLf2   10

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
#define kHttpWarning                       70
#define kHttpWwwAuthenticate               71
#define kHttpVia                           72
#define kHttpStrictTransportSecurity       73
#define kHttpXFrameOptions                 74
#define kHttpXContentTypeOptions           75
#define kHttpAltSvc                        76
#define kHttpReferrerPolicy                77
#define kHttpXXssProtection                78
#define kHttpAcceptRanges                  79
#define kHttpSetCookie                     80
#define kHttpSecChUa                       81
#define kHttpSecChUaMobile                 82
#define kHttpSecFetchSite                  83
#define kHttpSecFetchMode                  84
#define kHttpSecFetchUser                  85
#define kHttpSecFetchDest                  86
#define kHttpCfRay                         87
#define kHttpCfVisitor                     88
#define kHttpCfConnectingIp                89
#define kHttpCfIpcountry                   90
#define kHttpSecChUaPlatform               91
#define kHttpCdnLoop                       92
#define kHttpHeadersMax                    93

COSMOPOLITAN_C_START_

struct HttpSlice {
  short a, b;
};

struct HttpHeader {
  struct HttpSlice k;
  struct HttpSlice v;
};

struct HttpHeaders {
  unsigned n, c;
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

struct HttpUnchunker {
  int t;
  size_t i;
  size_t j;
  ssize_t m;
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
bool IsValidCookieValue(const char *, size_t);
bool IsAcceptablePath(const char *, size_t);
bool IsAcceptableHost(const char *, size_t);
bool IsAcceptablePort(const char *, size_t);
bool IsReasonablePath(const char *, size_t);
int ParseForwarded(const char *, size_t, uint32_t *, uint16_t *);
bool IsMimeType(const char *, size_t, const char *);
ssize_t Unchunk(struct HttpUnchunker *, char *, size_t, size_t *);
const char *FindContentType(const char *, size_t);
bool IsNoCompressExt(const char *, size_t);
char *FoldHeader(struct HttpMessage *, const char *, int, size_t *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_HTTP_HTTP_H_ */
