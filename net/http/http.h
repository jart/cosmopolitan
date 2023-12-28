#ifndef COSMOPOLITAN_LIBC_HTTP_HTTP_H_
#define COSMOPOLITAN_LIBC_HTTP_HTTP_H_
#include "libc/serialize.h"
#include "libc/time.h"

#define kHttpRequest  0
#define kHttpResponse 1

#define kHttpGet     READ32LE("GET")
#define kHttpHead    READ32LE("HEAD")
#define kHttpPost    READ32LE("POST")
#define kHttpPut     READ32LE("PUT")
#define kHttpDelete  READ64LE("DELETE\0")
#define kHttpOptions READ64LE("OPTIONS")
#define kHttpConnect READ64LE("CONNECT")
#define kHttpTrace   READ64LE("TRACE\0\0")

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
#define kHttpWebsocketKey                  93
#define kHttpHeadersMax                    94

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
  unsigned char version;
  uint64_t method;
  struct HttpSlice k;
  struct HttpSlice uri;
  struct HttpSlice scratch;
  struct HttpSlice message;
  struct HttpSlice headers[kHttpHeadersMax];
  struct HttpHeaders xheaders;
};

struct HttpUnchunker {
  int t;
  size_t i;
  size_t j;
  ssize_t m;
};

extern const char kHttpToken[256];
extern const bool kHttpRepeatable[kHttpHeadersMax];

const char *GetHttpReason(int) libcesque;
const char *GetHttpHeaderName(int) libcesque;
int GetHttpHeader(const char *, size_t) libcesque;
void InitHttpMessage(struct HttpMessage *, int) libcesque;
void DestroyHttpMessage(struct HttpMessage *) libcesque;
void ResetHttpMessage(struct HttpMessage *, int) libcesque;
int ParseHttpMessage(struct HttpMessage *, const char *, size_t,
                     size_t) libcesque;
bool HeaderHas(struct HttpMessage *, const char *, int, const char *,
               size_t) libcesque;
int64_t ParseContentLength(const char *, size_t) libcesque;
char *FormatHttpDateTime(char[hasatleast 30], struct tm *) libcesque;
bool ParseHttpRange(const char *, size_t, long, long *, long *) libcesque;
int64_t ParseHttpDateTime(const char *, size_t) libcesque;
uint64_t ParseHttpMethod(const char *, size_t) libcesque;
bool IsValidHttpToken(const char *, size_t) libcesque;
bool IsValidCookieValue(const char *, size_t) libcesque;
bool IsAcceptablePath(const char *, size_t) libcesque;
bool IsAcceptableHost(const char *, size_t) libcesque;
bool IsAcceptablePort(const char *, size_t) libcesque;
bool IsReasonablePath(const char *, size_t) libcesque;
int ParseForwarded(const char *, size_t, uint32_t *, uint16_t *) libcesque;
bool IsMimeType(const char *, size_t, const char *) libcesque;
ssize_t Unchunk(struct HttpUnchunker *, char *, size_t, size_t *) libcesque;
const char *FindContentType(const char *, size_t) libcesque;
bool IsNoCompressExt(const char *, size_t) libcesque;
char *FoldHeader(struct HttpMessage *, const char *, int, size_t *) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_HTTP_HTTP_H_ */
