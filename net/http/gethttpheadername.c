/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "net/http/http.h"

const char *GetHttpHeaderName(int h) {
  switch (h) {
    case kHttpAccept:
      return "Accept";
    case kHttpAcceptCharset:
      return "Accept-Charset";
    case kHttpAcceptEncoding:
      return "Accept-Encoding";
    case kHttpAcceptLanguage:
      return "Accept-Language";
    case kHttpAge:
      return "Age";
    case kHttpAllow:
      return "Allow";
    case kHttpAuthorization:
      return "Authorization";
    case kHttpCacheControl:
      return "Cache-Control";
    case kHttpChunked:
      return "Chunked";
    case kHttpLink:
      return "Link";
    case kHttpConnection:
      return "Connection";
    case kHttpContentBase:
      return "Content-Base";
    case kHttpContentEncoding:
      return "Content-Encoding";
    case kHttpContentLanguage:
      return "Content-Language";
    case kHttpContentLength:
      return "Content-Length";
    case kHttpContentLocation:
      return "Content-Location";
    case kHttpContentMd5:
      return "Content-MD5";
    case kHttpContentRange:
      return "Content-Range";
    case kHttpContentType:
      return "Content-Type";
    case kHttpDate:
      return "Date";
    case kHttpEtag:
      return "ETag";
    case kHttpExpires:
      return "Expires";
    case kHttpFrom:
      return "From";
    case kHttpHost:
      return "Host";
    case kHttpIfMatch:
      return "If-Match";
    case kHttpIfModifiedSince:
      return "If-Modified-Since";
    case kHttpIfNoneMatch:
      return "If-None-Match";
    case kHttpIfRange:
      return "If-Range";
    case kHttpIfUnmodifiedSince:
      return "If-Unmodified-Since";
    case kHttpKeepAlive:
      return "Keep-Alive";
    case kHttpMaxForwards:
      return "Max-Forwards";
    case kHttpPragma:
      return "Pragma";
    case kHttpProxyAuthenticate:
      return "Proxy-Authenticate";
    case kHttpProxyAuthorization:
      return "Proxy-Authorization";
    case kHttpProxyConnection:
      return "Proxy-Connection";
    case kHttpRange:
      return "Range";
    case kHttpReferer:
      return "Referer";
    case kHttpTransferEncoding:
      return "Transfer-Encoding";
    case kHttpUpgrade:
      return "Upgrade";
    case kHttpUserAgent:
      return "User-Agent";
    case kHttpVia:
      return "Via";
    case kHttpLocation:
      return "Location";
    case kHttpPublic:
      return "Public";
    case kHttpRetryAfter:
      return "Retry-After";
    case kHttpServer:
      return "Server";
    case kHttpVary:
      return "Vary";
    case kHttpWarning:
      return "Warning";
    case kHttpWwwAuthenticate:
      return "WWW-Authenticate";
    case kHttpLastModified:
      return "Last-Modified";
    case kHttpTrailer:
      return "Trailer";
    case kHttpTe:
      return "TE";
    case kHttpDnt:
      return "DNT";
    case kHttpExpect:
      return "Expect";
    case kHttpContentDisposition:
      return "Content-Disposition";
    case kHttpContentDescription:
      return "Content-Description";
    case kHttpOrigin:
      return "Origin";
    case kHttpUpgradeInsecureRequests:
      return "Upgrade-Insecure-Requests";
    case kHttpUri:
      return "URI";
    case kHttpXCsrfToken:
      return "X-Csrf-Token";
    case kHttpXForwardedFor:
      return "X-Forwarded-For";
    case kHttpXForwardedHost:
      return "X-Forwarded-Host";
    case kHttpXForwardedProto:
      return "X-Forwarded-Proto";
    case kHttpXRequestedWith:
      return "X-Requested-With";
    case kHttpAccessControlRequestMethod:
      return "Access-Control-Request-Method";
    case kHttpAccessControlRequestHeaders:
      return "Access-Control-Request-Headers";
    default:
      return NULL;
  }
}
