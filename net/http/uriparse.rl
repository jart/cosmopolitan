/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/assert.h"
#include "libc/dce.h"
#include "libc/dns/dns.h"
#include "libc/log/log.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"
#include "net/http/uri.h"

#define static

/* clang-format off */
%% machine uriparse;
%% write data;
/* clang-format on */

/**
 * Parses URI.
 *
 * This is a general URL parser. It's typically used for HTTP. Support
 * for the bonus syntax needed by SIP is provided. The whirlwhind tour
 * of the URI rabbit hole is as follows:
 *
 *     /foo.html
 *     //justine.local/foo.html
 *     http://justine.local/foo.html
 *     http://bettersearchengine.local/search.cgi?q=my%20query
 *     file:///etc/passwd
 *     gs://bucket/object.txt
 *     zip:///usr/share/zoneinfo/GMT
 *     sip:127.0.0.1:5060;lr
 *     sip:+12125650666@gateway.example
 *     sip:bob%20barker:priceisright@[dead:beef::666]:5060;isup-oli=00
 *     data:video/mpeg;base64,gigabytesofhex
 *
 * This parser operates on slices rather than C strings. It performs
 * slicing and validation only. Operations like turning "%20"→" " or
 * "80"→80 and perfect hashing can be done later, if needed.
 *
 * The Uri object is owned by the caller; it has a lifecycle like the
 * following:
 *
 *    struct Uri uri;
 *    memset(&uri, 0, sizeof(uri));
 *
 *    uriparse(&uri, s1, strlen(s1));
 *    CHECK_EQ(kUriSchemeHttp, urischeme(uri->scheme, s1));
 *
 *    uriparse(&uri, s2, strlen(s2));
 *    printf("host = %`.*s\n", uri->host.n, s2 + uri->host.i);
 *
 * Inner arrays may be granted memory by the caller. The uri->𝐴.i field
 * is cleared at the mark of this function. No more than uri->𝐴.n items
 * can be inserted. If we need more than that, then ENOMEM is returned
 * rather than dynamically extending uri->𝐴.p. However, if uri->𝐴.n==0,
 * we assume caller doesn't care about uri->𝐴 and its data is discarded.
 *
 * @param uri is owned by caller
 * @param p is caller-owned uri string; won't copy/alias/mutate
 * @return 0 on success, or -1 w/ errno
 * @see RFC2396: Uniform Resource Identifiers (URI): Generic Syntax
 * @see RFC3261: SIP: Session Initiation Protocol
 */
int uriparse(struct Uri *uri, const char *p, size_t size) {
  unsigned zero, cs;
  struct UriKeyval kv;
  const char *pe, *eof, *buf, *mark;

  assert(p || !size);
  assert(size <= 0x7ffff000);

#define ABSENT ((struct UriSlice){zero, zero})
#define SLICE  ((struct UriSlice){mark - buf, p - mark})

  cs = zero = VEIL("r", 0u);
  eof = pe = (mark = buf = p) + size;

  uri->scheme = ABSENT;
  uri->opaque = ABSENT;
  uri->userinfo = ABSENT;
  uri->host = ABSENT;
  uri->port = ABSENT;
  uri->fragment = ABSENT;
  uri->segs.i = zero;
  uri->paramsegs.i = zero;
  uri->params.i = zero;
  uri->queries.i = zero;

  /* clang-format off */

  %%{
    action Mark { mark = p; }
    action SetScheme { uri->scheme = SLICE; }
    action SetFragment { uri->fragment = SLICE; }
    action SetUserinfo { uri->userinfo = SLICE; }
    action SetHost { uri->host = SLICE; }
    action SetPort { uri->port = SLICE; }

    action SetKey {
      kv.k = SLICE;
      kv.v = (struct UriSlice){zero, zero};
    }

    action SetVal {
      kv.v = SLICE;
    }

    action RestartSegs {
      uri->segs.i = zero;
      uri->paramsegs.i = zero;
    }

    action AppendParam {
      if (uri->params.n) {
        if (uri->params.i < uri->params.n) {
          uri->params.p[uri->params.i++] = kv;
        } else {
          return enomem();
        }
      }
    }

    action AppendQuery {
      if (uri->queries.n) {
        if (uri->queries.i < uri->queries.n) {
          uri->queries.p[uri->queries.i++] = kv;
        } else {
          return enomem();
        }
      }
    }

    action AppendSegment {
      if (p > mark && uri->segs.n) {
        if (uri->segs.i < uri->segs.n) {
          uri->segs.p[uri->segs.i++] = SLICE;
        } else {
          return enomem();
        }
      }
    }

    action HandleOpaquePart {
      switch (urischeme(uri->scheme, buf)) {
        case kUriSchemeSip:
        case kUriSchemeSips:
          --p;
          fgoto sip;
        default:
          if (uricspn(p, pe - p) == pe - p) {
            uri->opaque = (struct UriSlice){p - buf, pe - p};
            return zero;
          } else {
            return einval();
          }
      }
    }

    mark = "-" | "_" | "." | "!" | "~" | "*" | "'" | "(" | ")";
    reserved = ";" | "/" | "?" | ":" | "@" | "&" | "=" | "+" | "$" | ",";
    unreserved = alnum | mark;
    ipv4c = digit | ".";
    ipv6c = xdigit | "." | ":";
    hostc = alnum | "-" | ".";
    telc = digit | "+" | "-";
    schemec = alnum | "+" | "-" | ".";
    userinfoc = unreserved | "&" | "=" | "+" | "$" | "," | "?" | ":";
    paramc = unreserved | "[" | "]" | ":" | "&" | "+" | "$";
    queryc = unreserved | "[" | "]" | "/" | "?" | ":" | "+" | "$";
    pathc = unreserved | ":" | "@" | "&" | "=" | "+" | "$" | ",";
    relc = unreserved | ";" | "@" | "&" | "=" | "+" | "$" | ",";
    uric = reserved | unreserved;

    escaped = "%" xdigit xdigit;
    pathchar = escaped | pathc;
    urichar = escaped | uric;
    relchar = escaped | relc;
    userinfochar = escaped | userinfoc;
    paramchar = escaped | paramc;
    querychar = escaped | queryc;

    paramkey = paramchar+ >Mark %SetKey;
    paramval = paramchar+ >Mark %SetVal;
    param = ";" paramkey ( "=" paramval )? %AppendParam;

    querykey = querychar+ >Mark %SetKey;
    queryval = querychar+ >Mark %SetVal;
    query = querykey ( "=" queryval )? %AppendQuery;
    queries = "?" query ( "&" query )*;

    scheme = ( alpha @Mark schemec* ) ":" @SetScheme;
    userinfo = userinfochar+ >Mark "@" @SetUserinfo;
    host6 = "[" ( ipv6c+ >Mark %SetHost ) "]";
    host = host6 | ( ( ipv4c | hostc | telc )+ >Mark %SetHost );
    port = digit+ >Mark %SetPort;
    hostport = host ( ":" port )?;
    authority = userinfo? hostport;
    segment = pathchar+ %AppendSegment param*;
    rel_segment = relchar+ >Mark %AppendSegment;
    path_segments = segment ( "/" @Mark segment )*;
    abs_path = "/" @Mark path_segments;
    net_path = "//" authority abs_path? >RestartSegs;
    hier_part = ( net_path | abs_path ) queries?;
    rel_path = rel_segment abs_path?;
    opaque_part = ( urichar -- "/" ) @HandleOpaquePart;
    fragment = "#" urichar* >Mark %SetFragment;
    relativeURI = ( net_path | abs_path | rel_path ) queries?;
    absoluteURI = scheme ( hier_part | opaque_part );
    sip := authority >Mark param*;
    uri := ( relativeURI | absoluteURI )? fragment?;
  }%%

  %% write init;
  cs = uriparse_en_uri;
  %% write exec;

  /* clang-format on */

  if (cs >= uriparse_first_final) {
    if (uri->host.n <= DNS_NAME_MAX && uri->port.n <= 6) {
      return zero;
    } else {
      return eoverflow();
    }
  } else {
    return einval();
  }
}
