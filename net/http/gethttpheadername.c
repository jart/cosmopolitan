/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
    case kHttpHost:
      return "Host";
    case kHttpCacheControl:
      return "Cache-Control";
    case kHttpConnection:
      return "Connection";
    case kHttpAccept:
      return "Accept";
    case kHttpAcceptLanguage:
      return "Accept-Language";
    case kHttpAcceptEncoding:
      return "Accept-Encoding";
    case kHttpUserAgent:
      return "User-Agent";
    case kHttpReferer:
      return "Referer";
    case kHttpXForwardedFor:
      return "X-Forwarded-For";
    case kHttpOrigin:
      return "Origin";
    case kHttpUpgradeInsecureRequests:
      return "Upgrade-Insecure-Requests";
    case kHttpPragma:
      return "Pragma";
    case kHttpCookie:
      return "Cookie";
    case kHttpDnt:
      return "DNT";
    case kHttpSecGpc:
      return "Sec-GPC";
    case kHttpFrom:
      return "From";
    case kHttpIfModifiedSince:
      return "If-Modified-Since";
    case kHttpXRequestedWith:
      return "X-Requested-With";
    case kHttpXForwardedHost:
      return "X-Forwarded-Host";
    case kHttpXForwardedProto:
      return "X-Forwarded-Proto";
    case kHttpXCsrfToken:
      return "X-CSRF-Token";
    case kHttpSaveData:
      return "Save-Data";
    case kHttpRange:
      return "Range";
    case kHttpContentLength:
      return "Content-Length";
    case kHttpContentType:
      return "Content-Type";
    case kHttpVary:
      return "Vary";
    case kHttpDate:
      return "Date";
    case kHttpServer:
      return "Server";
    case kHttpExpires:
      return "Expires";
    case kHttpContentEncoding:
      return "Content-Encoding";
    case kHttpLastModified:
      return "Last-Modified";
    case kHttpEtag:
      return "ETag";
    case kHttpAllow:
      return "Allow";
    case kHttpContentRange:
      return "Content-Range";
    case kHttpAcceptCharset:
      return "Accept-Charset";
    case kHttpAccessControlAllowCredentials:
      return "Access-Control-Allow-Credentials";
    case kHttpAccessControlAllowHeaders:
      return "Access-Control-Allow-Headers";
    case kHttpAccessControlAllowMethods:
      return "Access-Control-Allow-Methods";
    case kHttpAccessControlAllowOrigin:
      return "Access-Control-Allow-Origin";
    case kHttpAccessControlMaxAge:
      return "Access-Control-MaxAge";
    case kHttpAccessControlMethod:
      return "Access-Control-Method";
    case kHttpAccessControlRequestHeaders:
      return "Access-Control-RequestHeaders";
    case kHttpAccessControlRequestMethod:
      return "Access-Control-Request-Method";
    case kHttpAccessControlRequestMethods:
      return "Access-Control-Request-Methods";
    case kHttpAge:
      return "Age";
    case kHttpAuthorization:
      return "Authorization";
    case kHttpContentBase:
      return "Content-Base";
    case kHttpContentDescription:
      return "Content-Description";
    case kHttpContentDisposition:
      return "Content-Disposition";
    case kHttpContentLanguage:
      return "Content-Language";
    case kHttpContentLocation:
      return "Content-Location";
    case kHttpContentMd5:
      return "Content-MD5";
    case kHttpExpect:
      return "Expect";
    case kHttpIfMatch:
      return "If-Match";
    case kHttpIfNoneMatch:
      return "If-None-Match";
    case kHttpIfRange:
      return "If-Range";
    case kHttpIfUnmodifiedSince:
      return "If-Unmodified-Since";
    case kHttpKeepAlive:
      return "Keep-Alive";
    case kHttpLink:
      return "Link";
    case kHttpLocation:
      return "Location";
    case kHttpMaxForwards:
      return "Max-Forwards";
    case kHttpProxyAuthenticate:
      return "Proxy-Authenticate";
    case kHttpProxyAuthorization:
      return "Proxy-Authorization";
    case kHttpProxyConnection:
      return "Proxy-Connection";
    case kHttpPublic:
      return "Public";
    case kHttpRetryAfter:
      return "Retry-After";
    case kHttpTe:
      return "TE";
    case kHttpTrailer:
      return "Trailer";
    case kHttpTransferEncoding:
      return "Transfer-Encoding";
    case kHttpUpgrade:
      return "Upgrade";
    case kHttpWarning:
      return "Warning";
    case kHttpWwwAuthenticate:
      return "WWW-Authenticate";
    case kHttpVia:
      return "Via";
    case kHttpStrictTransportSecurity:
      return "Strict-Transport-Security";
    case kHttpXFrameOptions:
      return "X-Frame-Options";
    case kHttpXContentTypeOptions:
      return "X-Content-Type-Options";
    case kHttpAltSvc:
      return "Alt-Svc";
    case kHttpReferrerPolicy:
      return "Referrer-Policy";
    case kHttpXXssProtection:
      return "X-XSS-Protection";
    case kHttpAcceptRanges:
      return "Accept-Ranges";
    case kHttpSetCookie:
      return "Set-Cookie";
    case kHttpSecChUa:
      return "Sec-CH-UA";
    case kHttpSecChUaMobile:
      return "Sec-CH-UA-Mobile";
    case kHttpSecFetchSite:
      return "Sec-Fetch-Site";
    case kHttpSecFetchMode:
      return "Sec-Fetch-Mode";
    case kHttpSecFetchUser:
      return "Sec-Fetch-User";
    case kHttpSecFetchDest:
      return "Sec-Fetch-Dest";
    case kHttpCfRay:
      return "CF-RAY";
    case kHttpCfVisitor:
      return "CF-Visitor";
    case kHttpCfConnectingIp:
      return "CF-Connecting-IP";
    case kHttpCfIpcountry:
      return "CF-IPCountry";
    case kHttpCdnLoop:
      return "CDN-Loop";
    case kHttpSecChUaPlatform:
      return "Sec-CH-UA-Platform";
    default:
      return NULL;
  }
}
