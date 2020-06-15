#ifndef COSMOPOLITAN_LIBC_HTTP_HTTP_H_
#define COSMOPOLITAN_LIBC_HTTP_HTTP_H_
#include "libc/alg/alg.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct FILE;

enum HttpHeader {
  kHttpAccept = 1,
  kHttpAcceptCharset,
  kHttpAcceptEncoding,
  kHttpAcceptLanguage,
  kHttpAge,
  kHttpAllow,
  kHttpAuthorization,
  kHttpCacheControl,
  kHttpChunked,
  kHttpClose,
  kHttpConnection,
  kHttpContentBase,
  kHttpContentEncoding,
  kHttpContentLanguage,
  kHttpContentLength,
  kHttpContentLocation,
  kHttpContentMd5,
  kHttpContentRange,
  kHttpContentType,
  kHttpDate,
  kHttpEtag,
  kHttpExpires,
  kHttpFrom,
  kHttpHost,
  kHttpIfMatch,
  kHttpIfModifiedSince,
  kHttpIfNoneMatch,
  kHttpIfRange,
  kHttpIfUnmodifiedSince,
  kHttpKeepAlive,
  kHttpMaxForwards,
  kHttpPragma,
  kHttpProxyAuthenticate,
  kHttpProxyAuthorization,
  kHttpProxyConnection,
  kHttpRange,
  kHttpReferer,
  kHttpTransferEncoding,
  kHttpUpgrade,
  kHttpUserAgent,
  kHttpVia,
  kHttpLocation,
  kHttpPublic,
  kHttpRetryAfter,
  kHttpServer,
  kHttpVary,
  kHttpWarning,
  kHttpWwwAuthenticate,
  kHttpLastModified,
};

struct HttpStr {
  char *p;
  size_t i, n;
};

struct HttpRequest {
  struct HttpStr uri;      /* /foo/bar.html, etc. */
  struct HttpStr method;   /* "GET", "POST", etc. */
  struct critbit0 headers; /* TreeMultiMap<"key:value"> (no space delims) */
  struct HttpStr version;  /* "HTTP/1.1", etc. */
  struct HttpStr scratch;  /* "HTTP/1.1", etc. */
};

int parsehttprequest(struct HttpRequest *, struct FILE *) paramsnonnull();
void clearhttprequest(struct HttpRequest *) paramsnonnull();
void freehttprequest(struct HttpRequest **) paramsnonnull();
int negotiatehttprequest(int, const char *, uint32_t *, char *, uint32_t *,
                         uint32_t *, bool, long double);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_HTTP_HTTP_H_ */
