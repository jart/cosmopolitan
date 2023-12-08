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

/**
 * Set of standard comma-separate HTTP headers that may span lines.
 *
 * These headers may specified on multiple lines, e.g.
 *
 *     Allow: GET
 *     Allow: POST
 *
 * Is the same as:
 *
 *     Allow: GET, POST
 *
 * Standard headers that aren't part of this set will be overwritten in
 * the event that they're specified multiple times. For example,
 *
 *     Content-Type: application/octet-stream
 *     Content-Type: text/plain; charset=utf-8
 *
 * Is the same as:
 *
 *     Content-Type: text/plain; charset=utf-8
 *
 * This set exists to optimize header lookups and parsing. The existence
 * of standard headers that aren't in this set is an O(1) operation. The
 * repeatable headers in this list require an O(1) operation if they are
 * not present, otherwise the extended headers list needs to be crawled.
 *
 * Please note non-standard headers exist, e.g. Cookie, that may span
 * multiple lines, even though they're not comma-delimited. For those
 * headers we simply don't add them to the perfect hash table.
 *
 * @note we choose to not recognize this grammar for kHttpConnection
 * @note `grep '[A-Z][a-z]*".*":"' rfc2616`
 * @note `grep ':.*#' rfc2616`
 * @see RFC7230 § 4.2
 */
const bool kHttpRepeatable[kHttpHeadersMax] = {
    [kHttpAcceptCharset] = true,
    [kHttpAcceptEncoding] = true,
    [kHttpAcceptLanguage] = true,
    [kHttpAccept] = true,
    [kHttpAllow] = true,
    [kHttpCacheControl] = true,
    [kHttpContentEncoding] = true,
    [kHttpContentLanguage] = true,
    [kHttpExpect] = true,
    [kHttpIfMatch] = true,
    [kHttpIfNoneMatch] = true,
    [kHttpPragma] = true,
    [kHttpProxyAuthenticate] = true,
    [kHttpPublic] = true,
    [kHttpTe] = true,
    [kHttpTrailer] = true,
    [kHttpTransferEncoding] = true,
    [kHttpUpgrade] = true,
    [kHttpVary] = true,
    [kHttpVia] = true,
    [kHttpWarning] = true,
    [kHttpWwwAuthenticate] = true,
    [kHttpXForwardedFor] = true,
    [kHttpAccessControlAllowHeaders] = true,
    [kHttpAccessControlAllowMethods] = true,
    [kHttpAccessControlRequestHeaders] = true,
    [kHttpAccessControlRequestMethods] = true,
};
