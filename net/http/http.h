#ifndef COSMOPOLITAN_LIBC_HTTP_HTTP_H_
#define COSMOPOLITAN_LIBC_HTTP_HTTP_H_
#include "libc/alg/alg.h"
#include "libc/time/struct/tm.h"

#define kHttpGet     0
#define kHttpHead    1
#define kHttpPost    2
#define kHttpPut     3
#define kHttpDelete  4
#define kHttpConnect 5
#define kHttpOptions 6
#define kHttpTrace   7
#define kHttpCopy    8
#define kHttpLock    9
#define kHttpMerge   10
#define kHttpMkcol   11
#define kHttpMove    12
#define kHttpNotify  13
#define kHttpPatch   14
#define kHttpReport  15
#define kHttpUnlock  16

#define kHttpAccept             0
#define kHttpAcceptCharset      1
#define kHttpAcceptEncoding     2
#define kHttpAcceptLanguage     3
#define kHttpAge                4
#define kHttpAllow              5
#define kHttpAuthorization      6
#define kHttpCacheControl       7
#define kHttpChunked            8
#define kHttpClose              9
#define kHttpConnection         10
#define kHttpContentBase        11
#define kHttpContentEncoding    12
#define kHttpContentLanguage    13
#define kHttpContentLength      14
#define kHttpContentLocation    15
#define kHttpContentMd5         16
#define kHttpContentRange       17
#define kHttpContentType        18
#define kHttpDate               19
#define kHttpEtag               20
#define kHttpExpires            21
#define kHttpFrom               22
#define kHttpHost               23
#define kHttpIfMatch            24
#define kHttpIfModifiedSince    25
#define kHttpIfNoneMatch        26
#define kHttpIfRange            27
#define kHttpIfUnmodifiedSince  28
#define kHttpKeepAlive          29
#define kHttpMaxForwards        30
#define kHttpPragma             31
#define kHttpProxyAuthenticate  32
#define kHttpProxyAuthorization 33
#define kHttpProxyConnection    34
#define kHttpRange              35
#define kHttpReferer            36
#define kHttpTransferEncoding   37
#define kHttpUpgrade            38
#define kHttpUserAgent          39
#define kHttpVia                40
#define kHttpLocation           41
#define kHttpPublic             42
#define kHttpRetryAfter         43
#define kHttpServer             44
#define kHttpVary               45
#define kHttpWarning            46
#define kHttpWwwAuthenticate    47
#define kHttpLastModified       48
#define kHttpHeadersMax         49

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct HttpRequestSlice {
  short a, b;
};

struct HttpRequest {
  int method;
  int length;
  struct HttpRequestSlice uri;
  struct HttpRequestSlice version;
  struct HttpRequestSlice scratch;
  struct HttpRequestSlice headers[kHttpHeadersMax];
};

extern const char kHttpMethod[17][8];

int GetHttpHeader(const char *, size_t);
int GetHttpMethod(const char *, size_t);
int ParseHttpRequest(struct HttpRequest *, const char *, size_t);
int NegotiateHttpRequest(int, const char *, uint32_t *, char *, uint32_t *,
                         uint32_t *, bool, long double);
long ParseContentLength(const struct HttpRequest *, const char *);
char *FormatHttpDateTime(char[hasatleast 30], struct tm *);
bool ParseHttpRange(const char *, size_t, long, long *, long *);
unsigned ParseHttpVersion(const char *, size_t);
int64_t ParseHttpDateTime(const char *, size_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_HTTP_HTTP_H_ */
