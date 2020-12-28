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
#include "libc/bits/bits.h"
#include "libc/bits/initializer.internal.h"
#include "libc/errno.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/runtime/gc.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "net/http/uri.h"

#define URIPARSE(URI) uriparse(&uri, (p = URI), (size = sizeof(URI) - 1))

static const char kHttpCosmopolitanVideoUrl[] =
    "http://cosmopolitan.storage.googleapis.com/pub/vid/blankspace.mpg";

static const char kSipPriceIsTortureUri[] =
    "sip:bob%20barker:priceisright@[dead:beef::666]:5060;isup-oli=00";

static const char kWinsockIcoPngBase64[] = "\
base64,iVBORw0KGgoAAAANSUhEUgAAAJcAAACXCAYAAAAYn8l5AAAABmJLR0QA/\
wD/AP+gvaeTAAAACXBIWXMAAAsTAAALEwEAmpwYAAAAB3RJTUUH4woLByMP6uwgW\
QAAABl0RVh0Q29tbWVudABDcmVhdGVkIHdpdGggR0lNUFeBDhcAAAMeSURBVHja7\
d1BcuIwEAVQPMW94GbAzeBkZJepUXpw01ixDO+tE0PML+lHtsV0v9/vO+jgj1OAc\
CFcIFwIF8IFP+wrvzRNkzP3y7a4YmTkQrgQLnitc/XqA3GX+9SrU/+ei8vl8uMnT\
qeTkQvTIggXwoVC36mOJhZa3Upm5ALhQrjQuV6jT2HkQrgQLhAuNlLo+96Z6q5XI\
xcIF8KFzrXbWTDt0jTf4AkrIxfChXCBcCFcCBcIF8LFO9iP/gajx9jXMvrj80Yuh\
AuEC52r2q9G6jnRxWQX7Y1cCBfCBcLFxxb6tsBH5f12uz08xvV6Lb328Xh8+nfO5\
/NsyVfwjVwIF8IFa3auzALpXL96pRst0dWinta+loVWIxfChXCBcCFcCBcIF8LFe\
xn+6Z+5xc5oYTOzQJr5mcrFbYxcCBfCBcKFQv9AexdC9U7UueMueWwjFwgXwoVwO\
QUIF8IFwkV3e6dgfdETQ5knmIxcmBZBuBAuUOgH1Rb6LRZ8IxfChXDBt+le2N9nq\
a0a222VRn/aJrp5sO1CS22XlPkC9fa1R/tuIiMXwoVwgXDx5oV+ruCPJlrI7LXfa\
XsuMouo1YXWXv8IGLkwLSJcMGbnyrzWmqK/s31/Ue+pdJr2uNECbrvoXP0cen2eR\
i5MiwgXCBf9DX8n6ta+lCmzkFkp+FGhb89N9Yu52uMs9eVYRi5MiwgXbKdzba0TV\
h7NjzpY5i7Tpb78tD1OZrE408GMXJgWES4QLhT6zRf8qAxXFlqXKu+Vgp/5xyX6u\
41cmBYRLvg7dS5xJyqPzW2HFH0Ev9mxKjJ3wRq5MC0iXCBc9FdaRM38DzD6o/kjF\
frRy7uRC+FCuOBlpUVUnjzJhQvXo+8PaxEV0yLCBU9xs+Cg2ies1+5g0RPfRi5Mi\
wgXCBcK/UeYe3Ims6ia2RN1zfJu5MK0iHDBQy5cj/AhFLZd6inarskWSpgWES4QL\
sZkEXUAS227VJU5ti2UMC0iXKBzfUIPW3vbqrm96qP3Z+TCtIhwgXCh0POfAt1T5\
i6Nw+Ew+/6MXJgWES7Quejf74xcdPMFQQsgQ0YEZnUAAAAASUVORK5CYII=";

static size_t size;
static const char *p;
static struct Uri uri;
static struct UriMem {
  struct UriSlice segs[8];
  struct UriRef paramsegs[8];
  struct UriKeyval params[4], queries[4];
} urimem_;

static textstartup void init() {
  uri.segs.n = ARRAYLEN(urimem_.segs);
  uri.segs.p = urimem_.segs;
  uri.params.n = ARRAYLEN(urimem_.params);
  uri.params.p = urimem_.params;
  uri.queries.n = ARRAYLEN(urimem_.queries);
  uri.queries.p = urimem_.queries;
  uri.paramsegs.n = ARRAYLEN(urimem_.paramsegs);
  uri.paramsegs.p = urimem_.paramsegs;
}

const void *const g_name_ctor[] initarray = {init};

TEST(uriparse, sipPstnUri) {
  EXPECT_NE(-1, URIPARSE("sip:+12125650666"));
  EXPECT_STREQ("sip", gc(strndup(p + uri.scheme.i, uri.scheme.n)));
  EXPECT_STREQ("+12125650666", gc(strndup(p + uri.host.i, uri.host.n)));
  EXPECT_STREQ("", gc(strndup(p + uri.opaque.i, uri.opaque.n)));
}

TEST(uriparse, printVideoUrl) {
  EXPECT_NE(-1, URIPARSE(kHttpCosmopolitanVideoUrl));
  EXPECT_STREQ("http", gc(strndup(p + uri.scheme.i, uri.scheme.n)));
  EXPECT_STREQ("cosmopolitan.storage.googleapis.com",
               gc(strndup(p + uri.host.i, uri.host.n)));
  EXPECT_STREQ("", gc(strndup(p + uri.port.i, uri.port.n)));
  EXPECT_STREQ("/pub/vid/blankspace.mpg",
               gc(strndup(p + uri.segs.p[0].i,
                          (uri.segs.p[uri.segs.i - 1].n +
                           (uri.segs.p[uri.segs.i - 1].i - uri.segs.p[0].i)))));
}

TEST(uriparse, localRelativeFile) {
  EXPECT_NE(-1, URIPARSE("blankspace.mpg"));
  EXPECT_STREQ("", gc(strndup(p + uri.scheme.i, uri.scheme.n)));
  EXPECT_STREQ("", gc(strndup(p + uri.host.i, uri.host.n)));
  EXPECT_STREQ("", gc(strndup(p + uri.port.i, uri.port.n)));
  EXPECT_STREQ("blankspace.mpg",
               gc(strndup(p + uri.segs.p[0].i,
                          (uri.segs.p[uri.segs.i - 1].n +
                           (uri.segs.p[uri.segs.i - 1].i - uri.segs.p[0].i)))));
}

TEST(uriparse, badPort_einval) {
  EXPECT_EQ(-1, URIPARSE("http://hello.example:http/"));
  EXPECT_EQ(EINVAL, errno);
}

TEST(uriparse, datauri) {
  size = strlen((p = gc(xstrcat("data:image/png;", kWinsockIcoPngBase64))));
  EXPECT_NE(-1, uriparse(&uri, p, size));
  EXPECT_EQ(5, uri.opaque.i);
  EXPECT_EQ(size - 5, uri.opaque.n);
}

////////////////////////////////////////////////////////////////////////////////

BENCH(uriparse, bench) {
  EZBENCH(donothing, URIPARSE("sip:+12125650666"));
  EZBENCH(donothing, URIPARSE("http://hello.example"));
  EZBENCH(donothing, URIPARSE(kHttpCosmopolitanVideoUrl));
  EZBENCH(donothing, URIPARSE(kSipPriceIsTortureUri));
}

BENCH(uriparse, bigWinsockIcoPngUri) {
  const char *BigDataIconUri;
  BigDataIconUri = gc(xstrcat("data:image/png;", kWinsockIcoPngBase64));
  size = strlen(kWinsockIcoPngBase64);
  EZBENCH(donothing, uriparse(&uri, BigDataIconUri, size));
}
