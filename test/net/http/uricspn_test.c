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
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "net/http/uri.h"

_Alignas(32) const char kWinsockIcoPngBase64[] = "\
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

size_t size;

void SetUp(void) {
  size = strlen(kWinsockIcoPngBase64);
}
BENCH(strlen, bench) {
  EZBENCH(donothing, (size = strlen(kWinsockIcoPngBase64)));
}
TEST(uricspn, test) {
  EXPECT_EQ(size, uricspn(kWinsockIcoPngBase64, size));
}
BENCH(uricspn, bench) {
  EZBENCH(donothing, uricspn(kWinsockIcoPngBase64, size));
}
