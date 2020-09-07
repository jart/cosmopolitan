#ifndef COSMOPOLITAN_LIBC_HTTP_HTTP_H_
#define COSMOPOLITAN_LIBC_HTTP_HTTP_H_
#include "libc/alg/alg.h"

#define kHttpGet         0
#define kHttpHead        1
#define kHttpPost        2
#define kHttpPut         3
#define kHttpDelete      4
#define kHttpConnect     5
#define kHttpOptions     6
#define kHttpTrace       7
#define kHttpCopy        8
#define kHttpCheckout    9
#define kHttpLock        10
#define kHttpMerge       11
#define kHttpMkactivity  12
#define kHttpMkcol       13
#define kHttpMove        14
#define kHttpNotify      15
#define kHttpPatch       16
#define kHttpPropfind    17
#define kHttpProppatch   18
#define kHttpReport      19
#define kHttpSubscribe   20
#define kHttpUnlock      21
#define kHttpUnsubscribe 22

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
  struct HttpRequestSlice uri;
  struct HttpRequestSlice version;
  struct HttpRequestSlice scratch;
  struct HttpRequestSlice headers[kHttpHeadersMax];
};

int GetHttpHeader(const char *, size_t);
int GetHttpMethod(const char *, size_t);
int ParseHttpRequest(struct HttpRequest *, const char *, size_t);
int NegotiateHttpRequest(int, const char *, uint32_t *, char *, uint32_t *,
                         uint32_t *, bool, long double);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_HTTP_HTTP_H_ */
