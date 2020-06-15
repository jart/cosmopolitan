/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "dsp/mpeg/mpeg.h"
#include "dsp/mpeg/ycbcrio.h"
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/log/check.h"
#include "libc/macros.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"

static void CheckPlmFrame(const struct plm_frame_t *frame) {
  CHECK_NE(0, frame->width);
  CHECK_NE(0, frame->height);
  CHECK_GE(frame->y.width, frame->width);
  CHECK_GE(frame->y.height, frame->height);
  CHECK_EQ(frame->cr.width, frame->cb.width);
  CHECK_EQ(frame->cr.height, frame->cb.height);
  CHECK_EQ(frame->y.width, frame->cr.width * 2);
  CHECK_EQ(frame->y.height, frame->cr.height * 2);
  CHECK_NOTNULL(frame->y.data);
  CHECK_NOTNULL(frame->cr.data);
  CHECK_NOTNULL(frame->cb.data);
}

static size_t GetHeaderBytes(const struct plm_frame_t *frame) {
  return MAX(sizeof(struct Ycbcrio), ROUNDUP(frame->y.width, 16));
}

static size_t GetPlaneBytes(const struct plm_plane_t *plane) {
  /*
   * planes must be 16-byte aligned, but due to their hugeness, and the
   * recommendation of intel's 6,000 page manual, it makes sense to have
   * planes on isolated 64kb frames for multiprocessing.
   */
  return ROUNDUP(ROUNDUP(plane->height, 16) * ROUNDUP(plane->width, 16),
                 FRAMESIZE);
}

static size_t CalcMapBytes(const struct plm_frame_t *frame) {
  return ROUNDUP(GetHeaderBytes(frame) + GetPlaneBytes(&frame->y) +
                     GetPlaneBytes(&frame->cb) + GetPlaneBytes(&frame->cb),
                 FRAMESIZE);
}

static void FixupPointers(struct Ycbcrio *map) {
  map->frame.y.data = (unsigned char *)map + GetHeaderBytes(&map->frame);
  map->frame.cr.data = map->frame.y.data + GetPlaneBytes(&map->frame.y);
  map->frame.cb.data = map->frame.cr.data + GetPlaneBytes(&map->frame.cr);
}

static struct Ycbcrio *YcbcrioOpenNew(const char *path,
                                      const struct plm_frame_t *frame) {
  int fd;
  size_t size;
  struct stat st;
  struct Ycbcrio *map;
  CheckPlmFrame(frame);
  size = CalcMapBytes(frame);
  CHECK_NE(-1, (fd = open(path, O_CREAT | O_RDWR, 0644)));
  CHECK_NE(-1, fstat(fd, &st));
  if (st.st_size < size) {
    CHECK_NE(-1, ftruncate(fd, size));
  }
  CHECK_NE(MAP_FAILED,
           (map = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)));
  map->magic = YCBCRIO_MAGIC;
  map->fd = fd;
  map->size = size;
  memcpy(&map->frame, frame, sizeof(map->frame));
  FixupPointers(map);
  memcpy(&map->frame.y.data, frame->y.data, GetPlaneBytes(&frame->y));
  memcpy(&map->frame.cb.data, frame->cb.data, GetPlaneBytes(&frame->cb));
  memcpy(&map->frame.cr.data, frame->cr.data, GetPlaneBytes(&frame->cr));
  return map;
}

static struct Ycbcrio *YcbcrioOpenExisting(const char *path) {
  int fd;
  struct stat st;
  struct Ycbcrio *map;
  CHECK_NE(-1, (fd = open(path, O_RDWR)));
  CHECK_NE(-1, fstat(fd, &st));
  CHECK_NE(MAP_FAILED, (map = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE,
                                   MAP_SHARED, fd, 0)));
  CHECK_EQ(YCBCRIO_MAGIC, map->magic);
  CHECK_GE(st.st_size, CalcMapBytes(&map->frame));
  FixupPointers(map);
  map->fd = fd;
  map->size = st.st_size;
  return map;
}

/**
 * Opens shareable persistable MPEG video frame memory.
 *
 * @param path is a file name
 * @param frame if NULL means open existing file, otherwise copies new
 * @param points to pointer returned by YcbcrioOpen() which is cleared
 * @return memory mapping needing YcbcrioClose()
 */
struct Ycbcrio *YcbcrioOpen(const char *path, const struct plm_frame_t *frame) {
  if (frame) {
    return YcbcrioOpenNew(path, frame);
  } else {
    return YcbcrioOpenExisting(path);
  }
}

/**
 * Closes mapped video frame file.
 *
 * @param points to pointer returned by YcbcrioOpen() which is cleared
 */
void YcbcrioClose(struct Ycbcrio **map) {
  CHECK_NE(-1, close_s(&(*map)->fd));
  CHECK_NE(-1, munmap_s(map, (*map)->size));
}
