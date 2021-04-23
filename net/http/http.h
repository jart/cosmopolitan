#ifndef COSMOPOLITAN_LIBC_HTTP_HTTP_H_
#define COSMOPOLITAN_LIBC_HTTP_HTTP_H_
#include "libc/time/struct/tm.h"

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

#define kHttpAccept                      0
#define kHttpAcceptCharset               1
#define kHttpAcceptEncoding              2
#define kHttpAcceptLanguage              3
#define kHttpAge                         4
#define kHttpAllow                       5
#define kHttpAuthorization               6
#define kHttpCacheControl                7
#define kHttpChunked                     8
#define kHttpLink                        9
#define kHttpConnection                  10
#define kHttpContentBase                 11
#define kHttpContentEncoding             12
#define kHttpContentLanguage             13
#define kHttpContentLength               14
#define kHttpContentLocation             15
#define kHttpContentMd5                  16
#define kHttpContentRange                17
#define kHttpContentType                 18
#define kHttpDate                        19
#define kHttpEtag                        20
#define kHttpExpires                     21
#define kHttpFrom                        22
#define kHttpHost                        23
#define kHttpIfMatch                     24
#define kHttpIfModifiedSince             25
#define kHttpIfNoneMatch                 26
#define kHttpIfRange                     27
#define kHttpIfUnmodifiedSince           28
#define kHttpKeepAlive                   29
#define kHttpMaxForwards                 30
#define kHttpPragma                      31
#define kHttpProxyAuthenticate           32
#define kHttpProxyAuthorization          33
#define kHttpProxyConnection             34
#define kHttpRange                       35
#define kHttpReferer                     36
#define kHttpTransferEncoding            37
#define kHttpUpgrade                     38
#define kHttpUserAgent                   39
#define kHttpVia                         40
#define kHttpLocation                    41
#define kHttpPublic                      42
#define kHttpRetryAfter                  43
#define kHttpServer                      44
#define kHttpVary                        45
#define kHttpWarning                     46
#define kHttpWwwAuthenticate             47
#define kHttpLastModified                48
#define kHttpTrailer                     49
#define kHttpTe                          50
#define kHttpDnt                         51
#define kHttpExpect                      52
#define kHttpContentDisposition          53
#define kHttpContentDescription          54
#define kHttpOrigin                      55
#define kHttpUpgradeInsecureRequests     56
#define kHttpUri                         57
#define kHttpXCsrfToken                  58
#define kHttpXForwardedFor               59
#define kHttpXForwardedHost              60
#define kHttpXForwardedProto             61
#define kHttpXRequestedWith              62
#define kHttpAccessControlRequestMethod  63
#define kHttpAccessControlRequestHeaders 64
#define kHttpHeadersMax                  65

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct HttpRequestSlice {
  short a, b;
};

struct HttpRequest {
  int i, a;
  unsigned char t;
  unsigned char method;
  unsigned char version;
  struct HttpRequestSlice k;
  struct HttpRequestSlice uri;
  struct HttpRequestSlice scratch;
  struct HttpRequestSlice headers[kHttpHeadersMax];
  struct HttpRequestSlice xmethod;
  struct HttpRequestHeaders {
    unsigned n;
    struct HttpRequestHeader {
      struct HttpRequestSlice k;
      struct HttpRequestSlice v;
    } * p;
  } xheaders;
};

extern const char kHttpToken[256];
extern const char kHttpMethod[18][8];
extern const bool kHttpRepeatable[kHttpHeadersMax];

const char *GetHttpReason(int);
const char *GetHttpHeaderName(int);
int GetHttpHeader(const char *, size_t);
int GetHttpMethod(const char *, size_t);
void InitHttpRequest(struct HttpRequest *);
void DestroyHttpRequest(struct HttpRequest *);
int ParseHttpRequest(struct HttpRequest *, const char *, size_t);
bool HeaderHas(struct HttpRequest *, const char *, int, const char *, size_t);
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
