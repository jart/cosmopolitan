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
#include "libc/calls/calls.h"
#include "libc/calls/struct/iovec.h"
#include "libc/log/check.h"
#include "libc/macros.h"
#include "libc/nexgen32e/bsr.h"
#include "libc/runtime/gc.h"
#include "libc/sock/sock.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/x/x.h"
#include "tool/build/lib/persist.h"

static bool IsWithin(size_t sz1, void *vp1, size_t sz2, void *vp2) {
  char *p1 = vp1, *p2 = vp2;
  return p1 >= p2 && p1 + sz1 <= p2 + sz2;
}

static bool IsOverlapping(void *vx1, void *vy1, void *vx2, void *vy2) {
  char *x1 = vx1, *y1 = vy1, *x2 = vx2, *y2 = vy2;
  return (x1 >= x2 && x1 <= y2) || (y1 >= x2 && y1 <= y2);
}

static bool IsOverlappingIov(struct iovec *a, struct iovec *b) {
  char *ap = a->iov_base, *bp = b->iov_base;
  return IsOverlapping(ap, ap + a->iov_len, bp, bp + b->iov_len);
}

/**
 * Writes struct w/ dynamic arrays to mappable file, e.g.
 *
 *   PersistObject(path, 64, &(struct ObjectParam){
 *       sizeof(*o), o, &o->magic, &o->abi,
 *       &(struct ObjectArrayParam){
 *           {&o->a1.i, sizeof(o->a1.p[0]), &o->a1.p},
 *           {&o->a2.i, sizeof(o->a2.p[0]), &o->a2.p},
 *           {0},
 *       }});
 *
 * @param obj->magic needs to be unique for struct
 * @param obj->abi monotonically tracks breaking changes
 * @param obj->arrays needs sentinel with item size of zero
 * @note non-recursive i.e. array elements can't have pointers
 * @see MapObject()
 */
void PersistObject(const char *path, size_t align,
                   const struct ObjectParam *obj) {
  struct iovec *iov;
  int i, n, fd, iovlen;
  const char *tmp, *pad;
  long len, size, bytes, filesize;
  unsigned char *hdr, *p1, *p2, **pp;
  intptr_t arrayptroffset, arraydataoffset;
  filesize = 0;
  DCHECK_GE(align, 1);
  CHECK_GT(*obj->magic, 0);
  CHECK_GT(*obj->abi, 0);
  CHECK(IsWithin(sizeof(*obj->magic), obj->magic, obj->size, obj->p));
  CHECK(IsWithin(sizeof(*obj->abi), obj->abi, obj->size, obj->p));
  for (n = i = 0; obj->arrays[i].size; ++i) ++n;
  iovlen = (n + 1) * 2;
  pad = gc(xcalloc(align, 1));
  hdr = gc(xmalloc(obj->size));
  iov = gc(xcalloc(iovlen, sizeof(*iov)));
  tmp = gc(xasprintf("%s.%d.%s", path, getpid(), "tmp"));
  bytes = obj->size;
  iov[0].iov_base = memcpy(hdr, obj->p, obj->size);
  iov[0].iov_len = bytes;
  iov[1].iov_base = pad;
  iov[1].iov_len = ROUNDUP(bytes, align) - bytes;
  filesize += ROUNDUP(bytes, align);
  for (i = 0; i < n; ++i) {
    pp = obj->arrays[i].pp;
    len = obj->arrays[i].len;
    size = obj->arrays[i].size;
    if (!*pp || !len) continue;
    p1 = obj->p;
    p2 = obj->arrays[i].pp;
    arrayptroffset = p2 - p1;
    arraydataoffset = filesize;
    CHECK((!len || bsrl(len) + bsrl(size) < 31),
          "path=%s i=%d len=%,lu size=%,lu", path, i, len, size);
    CHECK(IsWithin(sizeof(void *), pp, obj->size, obj->p));
    CHECK(!IsOverlapping(pp, pp + sizeof(void *), obj->magic,
                         obj->magic + sizeof(*obj->magic)));
    CHECK(!IsOverlapping(pp, pp + sizeof(void *), obj->abi,
                         obj->abi + sizeof(*obj->abi)));
    memcpy(hdr + arrayptroffset, &arraydataoffset, sizeof(intptr_t));
    CHECK_LT(filesize + arraydataoffset, 0x7ffff000);
    bytes = len * size;
    iov[(i + 1) * 2 + 0].iov_base = *pp;
    iov[(i + 1) * 2 + 0].iov_len = bytes;
    iov[(i + 1) * 2 + 1].iov_base = pad;
    iov[(i + 1) * 2 + 1].iov_len = ROUNDUP(bytes, align) - bytes;
    filesize += ROUNDUP(bytes, align);
    CHECK(!IsOverlappingIov(&iov[(i + 0) * 2], &iov[(i + 1) * 2]),
          "iov[%d]={%#p,%#x}, iov[%d]={%#p,%#x} path=%s", (i + 0) * 2,
          iov[(i + 0) * 2].iov_base, iov[(i + 0) * 2].iov_len, (i + 1) * 2,
          iov[(i + 1) * 2].iov_base, iov[(i + 1) * 2].iov_len, path);
  }
  CHECK_NE(-1, (fd = open(tmp, O_CREAT | O_WRONLY | O_EXCL, 0644)), "%s", tmp);
  CHECK_EQ(filesize, writev(fd, iov, iovlen));
  CHECK_NE(-1, close(fd));
  CHECK_NE(-1, rename(tmp, path), "%s", path);
}
