/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for        │
│ any purpose with or without fee is hereby granted, provided that the        │
│ above copyright notice and this permission notice appear in all copies.     │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL               │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED               │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE            │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL        │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR       │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER              │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR            │
│ PERFORMANCE OF THIS SOFTWARE.                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "tool/net/lzip.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/timespec.h"
#include "libc/cosmo.h"
#include "libc/dos.h"
#include "libc/errno.h"
#include "libc/limits.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/time.h"
#include "libc/zip.h"
#include "net/http/http.h"
#include "third_party/lua/lauxlib.h"
#include "third_party/lua/lua.h"
#include "third_party/zlib/zlib.h"

#define LUA_ZIP_READER "zip.Reader"
#define LUA_ZIP_WRITER "zip.Writer"
#define LUA_ZIP_APPENDER "zip.Appender"
#define MAX_CDIR_SIZE (256 * 1024 * 1024)
#define MAX_FILE_SIZE (1024 * 1024 * 1024)

struct LuaZipReader {
  int fd;
  int owns_fd;
  uint8_t *cdir;
  int64_t cdir_size;
  int64_t count;
  int64_t file_size;
  int64_t max_file_size;
  const uint8_t *data;  // non-NULL when reading from buffer (uservalue 1)
};

struct LuaZipCdirEntry {
  char *name;
  size_t namelen;
  uint64_t offset;
  uint64_t compsize;
  uint64_t uncompsize;
  uint32_t crc32;
  uint16_t method;
  uint16_t mtime;
  uint16_t mdate;
  uint32_t mode;
};

struct LuaZipWriter {
  int fd;
  int owns_fd;
  char *path;
  int64_t offset;
  struct LuaZipCdirEntry *entries;
  size_t entry_count;
  size_t entry_capacity;
  int level;
  int64_t max_file_size;
};

struct LuaZipAppender {
  int fd;
  char *path;
  int64_t prefix_size;     // bytes before first local file (APE binary prefix)
  int64_t data_end;        // offset where central directory starts
  struct LuaZipCdirEntry *existing;
  size_t existing_count;
  struct LuaZipCdirEntry *pending;
  size_t pending_count;
  size_t pending_capacity;
  uint8_t **pending_data;  // compressed data for pending entries
  int level;
  int64_t max_file_size;
};

static struct LuaZipReader *GetZipReader(lua_State *L) {
  return luaL_checkudata(L, 1, LUA_ZIP_READER);
}

static struct LuaZipWriter *GetZipWriter(lua_State *L) {
  return luaL_checkudata(L, 1, LUA_ZIP_WRITER);
}

static struct LuaZipAppender *GetZipAppender(lua_State *L) {
  return luaL_checkudata(L, 1, LUA_ZIP_APPENDER);
}

static int SysError(lua_State *L, const char *what) {
  lua_pushnil(L);
  lua_pushfstring(L, "%s: %s", what, strerror(errno));
  return 2;
}

static int ZipError(lua_State *L, const char *msg) {
  lua_pushnil(L);
  lua_pushstring(L, msg);
  return 2;
}

static int WriterSysError(lua_State *L, struct LuaZipWriter *w,
                          const char *what) {
  int saved_errno = errno;
  if (w->path) {
    unlink(w->path);
  }
  errno = saved_errno;
  return SysError(L, what);
}

static uint8_t *FindEntry(struct LuaZipReader *z, const char *name,
                          size_t namelen) {
  int64_t i, got, hdrsize;
  for (i = got = 0;
       i + kZipCfileHdrMinSize <= z->cdir_size && got < z->count;
       i += hdrsize, ++got) {
    if (ZIP_CFILE_MAGIC(z->cdir + i) != kZipCfileHdrMagic)
      return NULL;
    hdrsize = ZIP_CFILE_HDRSIZE(z->cdir + i);
    if (hdrsize < kZipCfileHdrMinSize || i + hdrsize > z->cdir_size)
      return NULL;
    const char *entry_name = ZIP_CFILE_NAME(z->cdir + i);
    int entry_namelen = ZIP_CFILE_NAMESIZE(z->cdir + i);
    if (entry_namelen == (int)namelen &&
        !memcmp(entry_name, name, namelen)) {
      return z->cdir + i;
    }
  }
  return NULL;
}

// Read from either fd or in-memory buffer
// Returns bytes read, or -1 on error (sets errno)
static ssize_t ReaderPread(struct LuaZipReader *z, void *buf, size_t count,
                           int64_t offset) {
  if (z->data) {
    // reading from buffer
    if (offset < 0 || offset >= z->file_size)
      return 0;
    size_t avail = z->file_size - offset;
    if (count > avail)
      count = avail;
    memcpy(buf, z->data + offset, count);
    return count;
  } else {
    // reading from file descriptor
    return pread(z->fd, buf, count, offset);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Reader Implementation
////////////////////////////////////////////////////////////////////////////////

// zip.open(path|fd, [options]) -> reader, nil | nil, error
static int LuaZipOpen(lua_State *L) {
  const char *path = NULL;
  struct LuaZipReader *z;
  int64_t zsize;
  int64_t cnt, cdir_off, cdir_size;
  int64_t max_file_size = MAX_FILE_SIZE;
  int fd;
  int owns_fd;

  if (lua_isinteger(L, 1)) {
    fd = lua_tointeger(L, 1);
    owns_fd = 0;
  } else {
    path = luaL_checkstring(L, 1);
    fd = open(path, O_RDONLY);
    if (fd == -1)
      return SysError(L, path);
    owns_fd = 1;
  }

  if (lua_istable(L, 2)) {
    lua_getfield(L, 2, "max_file_size");
    if (!lua_isnil(L, -1)) {
      max_file_size = luaL_checkinteger(L, -1);
      if (max_file_size <= 0) {
        if (owns_fd) close(fd);
        return luaL_error(L, "max_file_size must be positive");
      }
    }
    lua_pop(L, 1);
  }

  // get file size
  zsize = lseek(fd, 0, SEEK_END);
  if (zsize == -1) {
    if (owns_fd) close(fd);
    return SysError(L, path ? path : "fd");
  }

  if (zsize == 0) {
    if (owns_fd) close(fd);
    return ZipError(L, "not a zip file");
  }

  // mmap file and use GetZipEocd to find end of central directory
  uint8_t *map = mmap(NULL, zsize, PROT_READ, MAP_PRIVATE, fd, 0);
  if (map == MAP_FAILED) {
    if (owns_fd) close(fd);
    return SysError(L, path ? path : "mmap");
  }

  int ziperr;
  uint8_t *eocd = GetZipEocd(map, zsize, &ziperr);
  if (!eocd) {
    munmap(map, zsize);
    if (owns_fd) close(fd);
    return ZipError(L, "not a zip file");
  }

  // use existing utilities to extract cdir info (handles ZIP64 transparently)
  cnt = GetZipCdirRecords(eocd);
  cdir_off = GetZipCdirOffset(eocd);
  cdir_size = GetZipCdirSize(eocd);

  if (cdir_size > MAX_CDIR_SIZE) {
    munmap(map, zsize);
    if (owns_fd) close(fd);
    return ZipError(L, "central directory too large");
  }

  if (cdir_off < 0 || cdir_off > zsize || cdir_size > zsize - cdir_off) {
    munmap(map, zsize);
    if (owns_fd) close(fd);
    return ZipError(L, "central directory offset out of bounds");
  }

  // create userdata first with safe defaults so __gc handles cleanup on error
  z = lua_newuserdatauv(L, sizeof(*z), 0);
  luaL_setmetatable(L, LUA_ZIP_READER);
  z->fd = fd;
  z->owns_fd = owns_fd;
  z->cdir = NULL;
  z->cdir_size = 0;
  z->count = 0;
  z->file_size = 0;
  z->max_file_size = 0;
  z->data = NULL;

  // allocate and copy central directory
  uint8_t *cdir = malloc(cdir_size ? cdir_size : 1);
  if (!cdir) {
    munmap(map, zsize);
    return SysError(L, "malloc");
  }
  if (cdir_size > 0) {
    memcpy(cdir, map + cdir_off, cdir_size);
  }

  munmap(map, zsize);

  z->cdir = cdir;
  z->cdir_size = cdir_size;
  z->count = cnt;
  z->file_size = zsize;
  z->max_file_size = max_file_size;

  return 1;
}

// zip.from(data, [options]) -> reader | nil, error
// Creates a zip reader from an in-memory string
static int LuaZipFrom(lua_State *L) {
  size_t zsize;
  const char *data = luaL_checklstring(L, 1, &zsize);
  struct LuaZipReader *z;
  int64_t cnt, cdir_off, cdir_size;
  int64_t max_file_size = MAX_FILE_SIZE;

  if (lua_istable(L, 2)) {
    lua_getfield(L, 2, "max_file_size");
    if (!lua_isnil(L, -1)) {
      max_file_size = luaL_checkinteger(L, -1);
      if (max_file_size <= 0) {
        return luaL_error(L, "max_file_size must be positive");
      }
    }
    lua_pop(L, 1);
  }

  if (zsize == 0) {
    return ZipError(L, "not a zip file");
  }

  // find end of central directory
  int ziperr;
  uint8_t *eocd = GetZipEocd((uint8_t *)data, zsize, &ziperr);
  if (!eocd) {
    return ZipError(L, "not a zip file");
  }

  // extract cdir info (handles ZIP64 transparently)
  cnt = GetZipCdirRecords(eocd);
  cdir_off = GetZipCdirOffset(eocd);
  cdir_size = GetZipCdirSize(eocd);

  if (cdir_size > MAX_CDIR_SIZE) {
    return ZipError(L, "central directory too large");
  }

  if (cdir_off < 0 || cdir_off > (int64_t)zsize ||
      cdir_size > (int64_t)zsize - cdir_off) {
    return ZipError(L, "central directory offset out of bounds");
  }

  // create userdata first with safe defaults so __gc handles cleanup on error
  z = lua_newuserdatauv(L, sizeof(*z), 1);
  luaL_setmetatable(L, LUA_ZIP_READER);

  // store reference to the input string so it doesn't get GC'd
  lua_pushvalue(L, 1);
  lua_setiuservalue(L, -2, 1);

  z->fd = -1;
  z->owns_fd = 0;
  z->cdir = NULL;
  z->cdir_size = 0;
  z->count = 0;
  z->file_size = 0;
  z->max_file_size = 0;
  z->data = NULL;

  // allocate and copy central directory
  uint8_t *cdir = malloc(cdir_size ? cdir_size : 1);
  if (!cdir) {
    return SysError(L, "malloc");
  }
  if (cdir_size > 0) {
    memcpy(cdir, data + cdir_off, cdir_size);
  }

  z->cdir = cdir;
  z->cdir_size = cdir_size;
  z->count = cnt;
  z->file_size = zsize;
  z->max_file_size = max_file_size;
  z->data = (const uint8_t *)data;

  return 1;
}

// reader:close()
static int LuaZipReaderClose(lua_State *L) {
  struct LuaZipReader *z = GetZipReader(L);
  if (z->fd != -1) {
    if (z->owns_fd) close(z->fd);
    z->fd = -1;
  }
  if (z->cdir) {
    free(z->cdir);
    z->cdir = NULL;
  }
  z->data = NULL;  // uservalue will be GC'd
  return 0;
}

// reader:__gc()
static int LuaZipReaderGc(lua_State *L) {
  return LuaZipReaderClose(L);
}

// reader:list() -> {name, ...}
static int LuaZipReaderList(lua_State *L) {
  struct LuaZipReader *z = GetZipReader(L);
  if (z->fd == -1 && !z->data)
    return ZipError(L, "zip reader is closed");

  lua_newtable(L);
  int idx = 1;
  int64_t i, got, hdrsize;
  for (i = got = 0;
       i + kZipCfileHdrMinSize <= z->cdir_size && got < z->count;
       i += hdrsize, ++got) {
    if (ZIP_CFILE_MAGIC(z->cdir + i) != kZipCfileHdrMagic)
      return ZipError(L, "corrupted central directory");
    hdrsize = ZIP_CFILE_HDRSIZE(z->cdir + i);
    if (hdrsize < kZipCfileHdrMinSize || i + hdrsize > z->cdir_size)
      return ZipError(L, "corrupted central directory");
    const char *name = ZIP_CFILE_NAME(z->cdir + i);
    int namelen = ZIP_CFILE_NAMESIZE(z->cdir + i);
    lua_pushlstring(L, name, namelen);
    lua_rawseti(L, -2, idx++);
  }
  return 1;
}

// reader:stat(name) -> {size, compressed_size, crc32, mtime, mode, method}
static int LuaZipReaderStat(lua_State *L) {
  struct LuaZipReader *z = GetZipReader(L);
  size_t namelen;
  const char *name = luaL_checklstring(L, 2, &namelen);

  if (z->fd == -1 && !z->data)
    return ZipError(L, "zip reader is closed");

  uint8_t *cfile = FindEntry(z, name, namelen);
  if (!cfile) {
    lua_pushnil(L);
    return 1;
  }

  lua_newtable(L);

  lua_pushinteger(L, GetZipCfileUncompressedSize(cfile));
  lua_setfield(L, -2, "size");

  lua_pushinteger(L, GetZipCfileCompressedSize(cfile));
  lua_setfield(L, -2, "compressed_size");

  lua_pushinteger(L, ZIP_CFILE_CRC32(cfile));
  lua_setfield(L, -2, "crc32");

  lua_pushinteger(L, GetZipCfileMode(cfile));
  lua_setfield(L, -2, "mode");

  lua_pushinteger(L, ZIP_CFILE_COMPRESSIONMETHOD(cfile));
  lua_setfield(L, -2, "method");

  struct timespec mtime, atime, ctime;
  GetZipCfileTimestamps(cfile, &mtime, &atime, &ctime, 0);
  lua_pushinteger(L, mtime.tv_sec);
  lua_setfield(L, -2, "mtime");

  return 1;
}

// reader:read(name) -> string | nil, error
static int LuaZipReaderRead(lua_State *L) {
  struct LuaZipReader *z = GetZipReader(L);
  size_t namelen;
  const char *name = luaL_checklstring(L, 2, &namelen);

  if (z->fd == -1 && !z->data)
    return ZipError(L, "zip reader is closed");

  uint8_t *cfile = FindEntry(z, name, namelen);
  if (!cfile)
    return ZipError(L, "entry not found");

  int64_t lfile_off = GetZipCfileOffset(cfile);
  int64_t compressed_size = GetZipCfileCompressedSize(cfile);
  int64_t uncompressed_size = GetZipCfileUncompressedSize(cfile);
  uint32_t expected_crc = ZIP_CFILE_CRC32(cfile);
  int method = ZIP_CFILE_COMPRESSIONMETHOD(cfile);

  if (compressed_size > z->max_file_size)
    return ZipError(L, "compressed size too large");
  if (uncompressed_size > z->max_file_size)
    return ZipError(L, "uncompressed size too large");
  if (lfile_off < 0 || lfile_off + kZipLfileHdrMinSize > z->file_size)
    return ZipError(L, "local file offset out of bounds");

  // read local file header to get data offset
  uint8_t lfile_hdr[kZipLfileHdrMinSize];
  if (ReaderPread(z, lfile_hdr, kZipLfileHdrMinSize, lfile_off) !=
      kZipLfileHdrMinSize)
    return SysError(L, "read lfile");
  if (ZIP_LFILE_MAGIC(lfile_hdr) != kZipLfileHdrMagic)
    return ZipError(L, "bad local file header");
  int64_t data_off = lfile_off + ZIP_LFILE_HDRSIZE(lfile_hdr);

  if (data_off + compressed_size > z->file_size)
    return ZipError(L, "file data extends beyond end of archive");

  // read compressed data
  uint8_t *compressed = malloc(compressed_size ? compressed_size : 1);
  if (!compressed)
    return SysError(L, "malloc");

  ssize_t rc;
  for (int64_t i = 0; i < compressed_size; i += rc) {
    rc = ReaderPread(z, compressed + i, compressed_size - i, data_off + i);
    if (rc <= 0) {
      free(compressed);
      return SysError(L, "read data");
    }
  }

  if (method == kZipCompressionNone) {
    // stored - verify CRC32 and return as-is
    uint32_t actual_crc = crc32_z(0, compressed, compressed_size);
    if (actual_crc != expected_crc) {
      free(compressed);
      return ZipError(L, "crc32 mismatch");
    }
    lua_pushlstring(L, (char *)compressed, compressed_size);
    free(compressed);
    return 1;
  } else if (method == kZipCompressionDeflate) {
    // deflated - decompress
    uint8_t *uncompressed = malloc(uncompressed_size ? uncompressed_size : 1);
    if (!uncompressed) {
      free(compressed);
      return SysError(L, "malloc");
    }

    z_stream strm = {0};
    strm.next_in = compressed;
    strm.avail_in = compressed_size;
    strm.next_out = uncompressed;
    strm.avail_out = uncompressed_size;

    int ret = inflateInit2(&strm, -MAX_WBITS);
    if (ret != Z_OK) {
      free(compressed);
      free(uncompressed);
      return ZipError(L, "inflateInit2 failed");
    }

    ret = inflate(&strm, Z_FINISH);
    inflateEnd(&strm);
    free(compressed);

    if (ret != Z_STREAM_END) {
      free(uncompressed);
      return ZipError(L, "decompression failed");
    }

    // verify CRC32
    uint32_t actual_crc = crc32_z(0, uncompressed, uncompressed_size);
    if (actual_crc != expected_crc) {
      free(uncompressed);
      return ZipError(L, "crc32 mismatch");
    }

    lua_pushlstring(L, (char *)uncompressed, uncompressed_size);
    free(uncompressed);
    return 1;
  } else {
    free(compressed);
    return ZipError(L, "unsupported compression method");
  }
}

// reader:__tostring()
static int LuaZipReaderTostring(lua_State *L) {
  struct LuaZipReader *z = GetZipReader(L);
  if (z->fd == -1 && !z->data) {
    lua_pushliteral(L, "zip.Reader (closed)");
  } else {
    lua_pushfstring(L, "zip.Reader (%d entries)", (int)z->count);
  }
  return 1;
}

////////////////////////////////////////////////////////////////////////////////
// Writer Implementation
////////////////////////////////////////////////////////////////////////////////

static bool HasDuplicateEntry(struct LuaZipWriter *w, const char *name,
                              size_t namelen) {
  for (size_t i = 0; i < w->entry_count; i++) {
    if (w->entries[i].namelen == namelen &&
        !memcmp(w->entries[i].name, name, namelen)) {
      return true;
    }
  }
  return false;
}

static bool IsUnsafePath(const char *name, size_t namelen) {
  if (namelen == 0)
    return true;
  if (name[0] == '/')
    return true;
  for (size_t i = 0; i + 1 < namelen; i++) {
    if (name[i] == '.' && name[i + 1] == '.') {
      if (i == 0 || name[i - 1] == '/')
        if (i + 2 == namelen || name[i + 2] == '/')
          return true;
    }
  }
  if (namelen >= 2 && name[namelen - 2] == '.' && name[namelen - 1] == '.') {
    if (namelen == 2 || name[namelen - 3] == '/')
      return true;
  }
  return false;
}

// Returns NULL if name is valid, or an error message if invalid
static const char *ValidateEntryName(const char *name, size_t namelen) {
  if (namelen == 0)
    return "name cannot be empty";
  if (namelen > 65535)
    return "name too long";
  if (memchr(name, '\0', namelen))
    return "name contains null byte";
  if (IsUnsafePath(name, namelen))
    return "unsafe path (contains '..' or starts with '/')";
  return NULL;
}

static void GetDosLocalTime(int64_t utcunixts, uint16_t *out_time,
                            uint16_t *out_date) {
  struct tm tm;
  localtime_r(&utcunixts, &tm);
  *out_time = DOS_TIME(tm.tm_hour, tm.tm_min, tm.tm_sec);
  *out_date = DOS_DATE(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

////////////////////////////////////////////////////////////////////////////////
// ZIP Helper Functions
////////////////////////////////////////////////////////////////////////////////

// Decide whether to attempt compression based on file extension and entropy.
// Returns true if compression should be attempted.
static bool ShouldCompress(const char *name, size_t namesize,
                           const void *data, size_t datasize, int level) {
  if (level <= 0)
    return false;
  if (datasize < 64)
    return false;
  if (IsNoCompressExt(name, namesize))
    return false;
  if (datasize >= 1000 && cosmo_entropy(data, 1000) >= 7)
    return false;
  return true;
}

// Compress data using deflate. Returns 0 on success, -1 on error.
// If compression doesn't help, *out will be NULL and *outlen unchanged.
// Caller must free(*out) if non-NULL.
static int ZipDeflate(const void *in, size_t inlen, void **out, size_t *outlen,
                      int level) {
  *out = NULL;
  if (inlen == 0)
    return 0;

  z_stream strm = {0};
  int ret = deflateInit2(&strm, level, Z_DEFLATED, -MAX_WBITS, DEF_MEM_LEVEL,
                         Z_DEFAULT_STRATEGY);
  if (ret != Z_OK)
    return -1;

  size_t bound = deflateBound(&strm, inlen);
  uint8_t *compdata = malloc(bound);
  if (!compdata) {
    deflateEnd(&strm);
    return -1;
  }

  strm.next_in = (uint8_t *)in;
  strm.avail_in = inlen;
  strm.next_out = compdata;
  strm.avail_out = bound;

  ret = deflate(&strm, Z_FINISH);
  if (ret != Z_STREAM_END) {
    deflateEnd(&strm);
    free(compdata);
    return -1;
  }

  size_t compsize = strm.total_out;
  deflateEnd(&strm);

  // If compressed is larger or equal, don't use it
  if (compsize >= inlen) {
    free(compdata);
    return 0;
  }

  *out = compdata;
  *outlen = compsize;
  return 0;
}

// Build a ZIP local file header. Returns pointer past end of header.
// Buffer must be at least kZipLfileHdrMinSize + namelen + 20 bytes.
static uint8_t *EmitZipLfileHdr(uint8_t *p, const char *name, size_t namelen,
                                uint32_t crc, uint16_t method, uint16_t mtime,
                                uint16_t mdate, uint64_t compsize,
                                uint64_t uncompsize) {
  bool needzip64 =
      (uncompsize >= 0xffffffffu || compsize >= 0xffffffffu);
  size_t extlen = needzip64 ? (2 + 2 + 8 + 8) : 0;

  p = ZIP_WRITE32(p, kZipLfileHdrMagic);
  p = ZIP_WRITE16(p, needzip64 ? kZipEra2001 : kZipEra1993);
  p = ZIP_WRITE16(p, kZipGflagUtf8);
  p = ZIP_WRITE16(p, method);
  p = ZIP_WRITE16(p, mtime);
  p = ZIP_WRITE16(p, mdate);
  p = ZIP_WRITE32(p, crc);
  if (needzip64) {
    p = ZIP_WRITE32(p, 0xffffffffu);
    p = ZIP_WRITE32(p, 0xffffffffu);
  } else {
    p = ZIP_WRITE32(p, compsize);
    p = ZIP_WRITE32(p, uncompsize);
  }
  p = ZIP_WRITE16(p, namelen);
  p = ZIP_WRITE16(p, extlen);
  memcpy(p, name, namelen);
  p += namelen;

  if (needzip64) {
    p = ZIP_WRITE16(p, kZipExtraZip64);
    p = ZIP_WRITE16(p, 8 + 8);
    p = ZIP_WRITE64(p, uncompsize);
    p = ZIP_WRITE64(p, compsize);
  }

  return p;
}

// Calculate size of local file header
static size_t GetLfileHdrSize(size_t namelen, uint64_t compsize,
                              uint64_t uncompsize) {
  bool needzip64 = (uncompsize >= 0xffffffffu || compsize >= 0xffffffffu);
  size_t extlen = needzip64 ? (2 + 2 + 8 + 8) : 0;
  return kZipLfileHdrMinSize + namelen + extlen;
}

// Build a ZIP central directory file header. Returns pointer past end.
// Buffer must be at least kZipCfileHdrMinSize + namelen + 28 bytes.
static uint8_t *EmitZipCfileHdr(uint8_t *p, const char *name, size_t namelen,
                                uint32_t crc, uint16_t method, uint16_t mtime,
                                uint16_t mdate, uint32_t mode, uint64_t offset,
                                uint64_t compsize, uint64_t uncompsize) {
  bool needzip64 = (uncompsize >= 0xffffffffu || compsize >= 0xffffffffu ||
                    offset >= 0xffffffffu);
  size_t extlen = needzip64 ? (2 + 2 + 8 + 8 + 8) : 0;

  p = ZIP_WRITE32(p, kZipCfileHdrMagic);
  p = ZIP_WRITE16(p, kZipOsUnix << 8 | (needzip64 ? kZipEra2001 : kZipEra1993));
  p = ZIP_WRITE16(p, needzip64 ? kZipEra2001 : kZipEra1993);
  p = ZIP_WRITE16(p, kZipGflagUtf8);
  p = ZIP_WRITE16(p, method);
  p = ZIP_WRITE16(p, mtime);
  p = ZIP_WRITE16(p, mdate);
  p = ZIP_WRITE32(p, crc);
  if (needzip64) {
    p = ZIP_WRITE32(p, 0xffffffffu);
    p = ZIP_WRITE32(p, 0xffffffffu);
  } else {
    p = ZIP_WRITE32(p, compsize);
    p = ZIP_WRITE32(p, uncompsize);
  }
  p = ZIP_WRITE16(p, namelen);
  p = ZIP_WRITE16(p, extlen);
  p = ZIP_WRITE16(p, 0);  // comment length
  p = ZIP_WRITE16(p, 0);  // disk number start
  p = ZIP_WRITE16(p, 0);  // internal file attributes
  p = ZIP_WRITE32(p, mode << 16);  // external file attributes
  if (needzip64) {
    p = ZIP_WRITE32(p, 0xffffffffu);
  } else {
    p = ZIP_WRITE32(p, offset);
  }
  memcpy(p, name, namelen);
  p += namelen;

  if (needzip64) {
    p = ZIP_WRITE16(p, kZipExtraZip64);
    p = ZIP_WRITE16(p, 8 + 8 + 8);
    p = ZIP_WRITE64(p, uncompsize);
    p = ZIP_WRITE64(p, compsize);
    p = ZIP_WRITE64(p, offset);
  }

  return p;
}

// Calculate size of central directory file header
static size_t GetCfileHdrSize(size_t namelen, uint64_t offset, uint64_t compsize,
                              uint64_t uncompsize) {
  bool needzip64 = (uncompsize >= 0xffffffffu || compsize >= 0xffffffffu ||
                    offset >= 0xffffffffu);
  size_t extlen = needzip64 ? (2 + 2 + 8 + 8 + 8) : 0;
  return kZipCfileHdrMinSize + namelen + extlen;
}

// Write end of central directory (handles ZIP64 automatically).
// Returns 0 on success, -1 on error.
static int WriteZipEocd(int fd, size_t entry_count, int64_t cdir_offset,
                        int64_t cdir_size) {
  bool needzip64 = (entry_count >= 0xffff || cdir_size >= 0xffffffffu ||
                    cdir_offset >= 0xffffffffu);

  if (needzip64) {
    // Write ZIP64 end of central directory record
    uint8_t eocd64[kZipCdir64HdrMinSize];
    uint8_t *p = eocd64;
    p = ZIP_WRITE32(p, kZipCdir64HdrMagic);
    p = ZIP_WRITE64(p, kZipCdir64HdrMinSize - 12);
    p = ZIP_WRITE16(p, kZipOsUnix << 8 | kZipEra2001);
    p = ZIP_WRITE16(p, kZipEra2001);
    p = ZIP_WRITE32(p, 0);  // disk number
    p = ZIP_WRITE32(p, 0);  // disk with cdir
    p = ZIP_WRITE64(p, entry_count);
    p = ZIP_WRITE64(p, entry_count);
    p = ZIP_WRITE64(p, cdir_size);
    p = ZIP_WRITE64(p, cdir_offset);

    if (write(fd, eocd64, sizeof(eocd64)) != sizeof(eocd64))
      return -1;

    // Write ZIP64 end of central directory locator
    uint8_t loc64[kZipCdir64LocatorSize];
    p = loc64;
    p = ZIP_WRITE32(p, kZipCdir64LocatorMagic);
    p = ZIP_WRITE32(p, 0);  // disk with eocd64
    p = ZIP_WRITE64(p, cdir_offset + cdir_size);
    p = ZIP_WRITE32(p, 1);  // total disks

    if (write(fd, loc64, sizeof(loc64)) != sizeof(loc64))
      return -1;
  }

  // Write end of central directory record
  uint8_t eocd[kZipCdirHdrMinSize];
  uint8_t *p = eocd;
  p = ZIP_WRITE32(p, kZipCdirHdrMagic);
  p = ZIP_WRITE16(p, 0);  // disk number
  p = ZIP_WRITE16(p, 0);  // disk with cdir
  p = ZIP_WRITE16(p, entry_count >= 0xffff ? 0xffff : entry_count);
  p = ZIP_WRITE16(p, entry_count >= 0xffff ? 0xffff : entry_count);
  p = ZIP_WRITE32(p, cdir_size >= 0xffffffffu ? 0xffffffffu : cdir_size);
  p = ZIP_WRITE32(p, cdir_offset >= 0xffffffffu ? 0xffffffffu : cdir_offset);
  p = ZIP_WRITE16(p, 0);  // comment length

  if (write(fd, eocd, sizeof(eocd)) != sizeof(eocd))
    return -1;

  return 0;
}

// Write a central directory entry to fd. Returns bytes written or -1 on error.
static ssize_t WriteCdirEntry(int fd, const struct LuaZipCdirEntry *e,
                              uint64_t offset) {
  size_t hdrlen = GetCfileHdrSize(e->namelen, offset, e->compsize, e->uncompsize);
  uint8_t *buf = malloc(hdrlen);
  if (!buf)
    return -1;

  EmitZipCfileHdr(buf, e->name, e->namelen, e->crc32, e->method, e->mtime,
                  e->mdate, e->mode, offset, e->compsize, e->uncompsize);

  ssize_t written = write(fd, buf, hdrlen);
  free(buf);

  if (written != (ssize_t)hdrlen)
    return -1;

  return hdrlen;
}

////////////////////////////////////////////////////////////////////////////////
// Writer Implementation
////////////////////////////////////////////////////////////////////////////////

// zip.create(path|fd, [options]) -> writer | nil, error
static int LuaZipCreate(lua_State *L) {
  const char *path = NULL;
  int level = Z_DEFAULT_COMPRESSION;
  int64_t max_file_size = MAX_FILE_SIZE;
  int fd;
  int owns_fd;
  char *pathcopy = NULL;

  if (lua_isinteger(L, 1)) {
    fd = lua_tointeger(L, 1);
    owns_fd = 0;
  } else {
    path = luaL_checkstring(L, 1);
    owns_fd = 1;
  }

  if (lua_istable(L, 2)) {
    lua_getfield(L, 2, "level");
    if (!lua_isnil(L, -1)) {
      level = luaL_checkinteger(L, -1);
      if (level < 0 || level > 9)
        return luaL_error(L, "compression level must be 0-9");
    }
    lua_pop(L, 1);

    lua_getfield(L, 2, "max_file_size");
    if (!lua_isnil(L, -1)) {
      max_file_size = luaL_checkinteger(L, -1);
      if (max_file_size <= 0)
        return luaL_error(L, "max_file_size must be positive");
    }
    lua_pop(L, 1);
  }

  if (owns_fd) {
    fd = open(path, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1)
      return SysError(L, path);
    pathcopy = strdup(path);
    if (!pathcopy) {
      close(fd);
      return SysError(L, "strdup");
    }
  }

  struct LuaZipWriter *w = lua_newuserdatauv(L, sizeof(*w), 0);
  luaL_setmetatable(L, LUA_ZIP_WRITER);
  w->fd = fd;
  w->owns_fd = owns_fd;
  w->path = pathcopy;
  w->offset = 0;
  w->entries = NULL;
  w->entry_count = 0;
  w->entry_capacity = 0;
  w->level = level;
  w->max_file_size = max_file_size;

  return 1;
}

static int AddCdirEntry(struct LuaZipWriter *w, const char *name, size_t namelen,
                        uint64_t offset, uint64_t compsize, uint64_t uncompsize,
                        uint32_t crc32, uint16_t method, uint16_t mtime,
                        uint16_t mdate, uint32_t mode) {
  if (w->entry_count >= w->entry_capacity) {
    size_t newcap = w->entry_capacity ? w->entry_capacity * 2 : 16;
    struct LuaZipCdirEntry *newentries =
        realloc(w->entries, newcap * sizeof(*newentries));
    if (!newentries)
      return -1;
    w->entries = newentries;
    w->entry_capacity = newcap;
  }

  struct LuaZipCdirEntry *e = &w->entries[w->entry_count++];
  e->name = malloc(namelen + 1);
  if (!e->name) {
    w->entry_count--;
    return -1;
  }
  memcpy(e->name, name, namelen);
  e->name[namelen] = '\0';
  e->namelen = namelen;
  e->offset = offset;
  e->compsize = compsize;
  e->uncompsize = uncompsize;
  e->crc32 = crc32;
  e->method = method;
  e->mtime = mtime;
  e->mdate = mdate;
  e->mode = mode;
  return 0;
}

// writer:add(name, content, [options]) -> true | nil, error
static int LuaZipWriterAdd(lua_State *L) {
  struct LuaZipWriter *w = GetZipWriter(L);
  size_t namelen, contentlen;
  const char *name = luaL_checklstring(L, 2, &namelen);
  const char *content = luaL_checklstring(L, 3, &contentlen);

  if (w->fd == -1)
    return ZipError(L, "zip writer is closed");

  const char *name_err = ValidateEntryName(name, namelen);
  if (name_err)
    return ZipError(L, name_err);

  if (HasDuplicateEntry(w, name, namelen))
    return ZipError(L, "duplicate entry name");

  if (contentlen > UINT_MAX)
    return ZipError(L, "content too large (exceeds 4GB limit)");

  if ((int64_t)contentlen > w->max_file_size)
    return ZipError(L, "content exceeds max_file_size limit");

  // parse options
  bool force_store = false;
  bool force_deflate = false;
  int64_t mtime_unix = time(NULL);
  uint32_t mode = 0100644;

  if (lua_istable(L, 4)) {
    lua_getfield(L, 4, "method");
    if (!lua_isnil(L, -1)) {
      const char *m = luaL_checkstring(L, -1);
      if (!strcmp(m, "store"))
        force_store = true;
      else if (!strcmp(m, "deflate"))
        force_deflate = true;
      else
        return luaL_error(L, "unknown method: %s", m);
    }
    lua_pop(L, 1);

    lua_getfield(L, 4, "mtime");
    if (!lua_isnil(L, -1))
      mtime_unix = luaL_checkinteger(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 4, "mode");
    if (!lua_isnil(L, -1))
      mode = luaL_checkinteger(L, -1);
    lua_pop(L, 1);
  }

  // validate mode is a regular file
  if ((mode & 0170000) != 0100000 && (mode & 0170000) != 0)
    return ZipError(L, "mode must be a regular file");
  if ((mode & 0170000) == 0)
    mode |= 0100000;

  // compute CRC32
  uint32_t crc = crc32_z(0, (const uint8_t *)content, contentlen);

  // convert mtime
  uint16_t mtime, mdate;
  GetDosLocalTime(mtime_unix, &mtime, &mdate);

  // decide compression method and compress if needed
  uint16_t method = kZipCompressionNone;
  void *compdata = NULL;
  size_t compsize = contentlen;

  if (!force_store &&
      (force_deflate || ShouldCompress(name, namelen, content, contentlen, w->level))) {
    if (ZipDeflate(content, contentlen, &compdata, &compsize, w->level) < 0)
      return ZipError(L, "deflate failed");
    if (compdata)
      method = kZipCompressionDeflate;
  }

  // build and write local file header
  size_t hdrlen = GetLfileHdrSize(namelen, compsize, contentlen);
  uint8_t *lochdr = malloc(hdrlen);
  if (!lochdr) {
    free(compdata);
    return SysError(L, "malloc");
  }

  EmitZipLfileHdr(lochdr, name, namelen, crc, method, mtime, mdate, compsize,
                  contentlen);

  ssize_t written = write(w->fd, lochdr, hdrlen);
  free(lochdr);
  if (written != (ssize_t)hdrlen) {
    free(compdata);
    return WriterSysError(L, w, "write header");
  }

  // write file data
  const void *writedata = compdata ? compdata : content;
  written = write(w->fd, writedata, compsize);
  free(compdata);
  if (written != (ssize_t)compsize)
    return WriterSysError(L, w, "write data");

  // record entry for central directory
  if (AddCdirEntry(w, name, namelen, w->offset, compsize, contentlen, crc,
                   method, mtime, mdate, mode) < 0)
    return SysError(L, "malloc");

  w->offset += hdrlen + compsize;
  lua_pushboolean(L, 1);
  return 1;
}

// writer:close() -> true | nil, error
static int LuaZipWriterClose(lua_State *L) {
  struct LuaZipWriter *w = GetZipWriter(L);

  if (w->fd == -1)
    return ZipError(L, "zip writer is already closed");

  // write central directory entries
  int64_t cdir_offset = w->offset;
  int64_t cdir_size = 0;

  for (size_t i = 0; i < w->entry_count; i++) {
    ssize_t written = WriteCdirEntry(w->fd, &w->entries[i], w->entries[i].offset);
    if (written < 0)
      return WriterSysError(L, w, "write cdir entry");
    cdir_size += written;
  }

  // write end of central directory
  if (WriteZipEocd(w->fd, w->entry_count, cdir_offset, cdir_size) < 0)
    return WriterSysError(L, w, "write eocd");

  // cleanup - set fd to -1 before close to prevent double-close in GC
  int fd = w->fd;
  w->fd = -1;
  if (w->owns_fd && close(fd) == -1)
    return SysError(L, "close");

  for (size_t i = 0; i < w->entry_count; i++)
    free(w->entries[i].name);
  free(w->entries);
  w->entries = NULL;
  w->entry_count = 0;
  w->entry_capacity = 0;

  if (w->path) {
    free(w->path);
    w->path = NULL;
  }

  lua_pushboolean(L, 1);
  return 1;
}

// writer:__gc()
static int LuaZipWriterGc(lua_State *L) {
  struct LuaZipWriter *w = GetZipWriter(L);
  if (w->fd != -1) {
    if (w->owns_fd) close(w->fd);
    w->fd = -1;
  }
  for (size_t i = 0; i < w->entry_count; i++)
    free(w->entries[i].name);
  free(w->entries);
  w->entries = NULL;
  if (w->path) {
    free(w->path);
    w->path = NULL;
  }
  return 0;
}

// writer:__tostring()
static int LuaZipWriterTostring(lua_State *L) {
  struct LuaZipWriter *w = GetZipWriter(L);
  if (w->fd == -1) {
    lua_pushliteral(L, "zip.Writer (closed)");
  } else {
    lua_pushfstring(L, "zip.Writer (%d entries)", (int)w->entry_count);
  }
  return 1;
}

////////////////////////////////////////////////////////////////////////////////
// Appender Implementation
////////////////////////////////////////////////////////////////////////////////

static void AppenderCleanup(struct LuaZipAppender *a) {
  if (a->fd != -1) {
    close(a->fd);
    a->fd = -1;
  }
  if (a->path) {
    free(a->path);
    a->path = NULL;
  }
  for (size_t i = 0; i < a->existing_count; i++)
    free(a->existing[i].name);
  free(a->existing);
  a->existing = NULL;
  a->existing_count = 0;
  for (size_t i = 0; i < a->pending_count; i++) {
    free(a->pending[i].name);
    free(a->pending_data[i]);
  }
  free(a->pending);
  free(a->pending_data);
  a->pending = NULL;
  a->pending_data = NULL;
  a->pending_count = 0;
  a->pending_capacity = 0;
}

static bool AppenderHasEntry(struct LuaZipAppender *a, const char *name,
                             size_t namelen) {
  for (size_t i = 0; i < a->existing_count; i++) {
    if (a->existing[i].namelen == namelen &&
        !memcmp(a->existing[i].name, name, namelen)) {
      return true;
    }
  }
  for (size_t i = 0; i < a->pending_count; i++) {
    if (a->pending[i].namelen == namelen &&
        !memcmp(a->pending[i].name, name, namelen)) {
      return true;
    }
  }
  return false;
}

// zip.append(path, [options]) -> appender | nil, error
static int LuaZipAppend(lua_State *L) {
  const char *path = luaL_checkstring(L, 1);
  int level = Z_DEFAULT_COMPRESSION;
  int64_t max_file_size = MAX_FILE_SIZE;

  if (lua_istable(L, 2)) {
    lua_getfield(L, 2, "level");
    if (!lua_isnil(L, -1)) {
      level = luaL_checkinteger(L, -1);
      if (level < 0 || level > 9)
        return ZipError(L, "compression level must be 0-9");
    }
    lua_pop(L, 1);

    lua_getfield(L, 2, "max_file_size");
    if (!lua_isnil(L, -1)) {
      max_file_size = luaL_checkinteger(L, -1);
      if (max_file_size <= 0)
        return ZipError(L, "max_file_size must be positive");
    }
    lua_pop(L, 1);
  }

  // Try to open existing file for read/write
  int fd = open(path, O_RDWR);
  bool is_new_file = false;
  if (fd == -1) {
    if (errno == ENOENT) {
      // File doesn't exist - create it
      fd = open(path, O_CREAT | O_RDWR, 0644);
      if (fd == -1)
        return SysError(L, path);
      is_new_file = true;
    } else {
      return SysError(L, path);
    }
  }

  char *pathcopy = strdup(path);
  if (!pathcopy) {
    close(fd);
    return SysError(L, "strdup");
  }

  struct LuaZipAppender *a = lua_newuserdatauv(L, sizeof(*a), 0);
  luaL_setmetatable(L, LUA_ZIP_APPENDER);
  memset(a, 0, sizeof(*a));
  a->fd = fd;
  a->path = pathcopy;
  a->level = level;
  a->max_file_size = max_file_size;

  if (is_new_file) {
    // Empty new file - no existing entries
    a->prefix_size = 0;
    a->data_end = 0;
    return 1;
  }

  // Get file size
  int64_t zsize = lseek(fd, 0, SEEK_END);
  if (zsize == -1) {
    AppenderCleanup(a);
    return SysError(L, path);
  }

  if (zsize == 0) {
    // Empty file - no existing entries
    a->prefix_size = 0;
    a->data_end = 0;
    return 1;
  }

  // mmap file and use GetZipEocd to find end of central directory
  uint8_t *map = mmap(NULL, zsize, PROT_READ, MAP_PRIVATE, fd, 0);
  if (map == MAP_FAILED) {
    AppenderCleanup(a);
    return SysError(L, "mmap");
  }

  int ziperr;
  uint8_t *eocd = GetZipEocd(map, zsize, &ziperr);
  if (!eocd) {
    munmap(map, zsize);
    AppenderCleanup(a);
    return ZipError(L, "not a zip file");
  }

  // use existing utilities to extract cdir info (handles ZIP64 transparently)
  int64_t cnt = GetZipCdirRecords(eocd);
  int64_t cdir_off = GetZipCdirOffset(eocd);
  int64_t cdir_size = GetZipCdirSize(eocd);

  if (cdir_size > MAX_CDIR_SIZE) {
    munmap(map, zsize);
    AppenderCleanup(a);
    return ZipError(L, "central directory too large");
  }

  if (cdir_off < 0 || cdir_off + cdir_size > zsize) {
    munmap(map, zsize);
    AppenderCleanup(a);
    return ZipError(L, "central directory offset out of bounds");
  }

  // Parse central directory entries directly from mmap
  a->existing = malloc(cnt * sizeof(*a->existing));
  if (!a->existing) {
    munmap(map, zsize);
    AppenderCleanup(a);
    return SysError(L, "malloc");
  }

  uint8_t *cdir = map + cdir_off;
  int64_t min_lfile_off = INT64_MAX;
  int64_t max_data_end = 0;
  int64_t i, got, hdrsize;
  for (i = got = 0;
       i + kZipCfileHdrMinSize <= cdir_size && got < cnt;
       i += hdrsize, ++got) {
    if (ZIP_CFILE_MAGIC(cdir + i) != kZipCfileHdrMagic) {
      munmap(map, zsize);
      AppenderCleanup(a);
      return ZipError(L, "corrupted central directory");
    }
    hdrsize = ZIP_CFILE_HDRSIZE(cdir + i);
    if (hdrsize < kZipCfileHdrMinSize || i + hdrsize > cdir_size) {
      munmap(map, zsize);
      AppenderCleanup(a);
      return ZipError(L, "corrupted central directory");
    }

    struct LuaZipCdirEntry *e = &a->existing[a->existing_count++];
    uint32_t mode = GetZipCfileMode(cdir + i);
    const char *name = ZIP_CFILE_NAME(cdir + i);
    int namelen = ZIP_CFILE_NAMESIZE(cdir + i);
    e->name = malloc(namelen + 1);
    if (!e->name) {
      munmap(map, zsize);
      AppenderCleanup(a);
      return SysError(L, "malloc");
    }
    memcpy(e->name, name, namelen);
    e->name[namelen] = '\0';
    e->namelen = namelen;
    e->offset = GetZipCfileOffset(cdir + i);
    e->compsize = GetZipCfileCompressedSize(cdir + i);
    e->uncompsize = GetZipCfileUncompressedSize(cdir + i);
    e->crc32 = ZIP_CFILE_CRC32(cdir + i);
    e->method = ZIP_CFILE_COMPRESSIONMETHOD(cdir + i);
    e->mtime = ZIP_CFILE_LASTMODIFIEDTIME(cdir + i);
    e->mdate = ZIP_CFILE_LASTMODIFIEDDATE(cdir + i);
    e->mode = mode;

    if ((int64_t)e->offset < min_lfile_off)
      min_lfile_off = e->offset;

    // Read local file header from mmap to get actual data end
    if ((int64_t)e->offset >= 0 &&
        (int64_t)e->offset + kZipLfileHdrMinSize <= zsize) {
      uint8_t *lfile = map + e->offset;
      if (ZIP_LFILE_MAGIC(lfile) == kZipLfileHdrMagic) {
        int64_t hdr_size = ZIP_LFILE_HDRSIZE(lfile);
        if (hdr_size >= 0 && (int64_t)e->compsize >= 0 &&
            (int64_t)e->offset <= zsize - hdr_size &&
            (int64_t)e->offset + hdr_size <= zsize - (int64_t)e->compsize) {
          int64_t this_end = e->offset + hdr_size + e->compsize;
          if (this_end > max_data_end)
            max_data_end = this_end;
        }
      }
    }
  }

  munmap(map, zsize);

  a->prefix_size = (min_lfile_off == INT64_MAX) ? 0 : min_lfile_off;
  a->data_end = max_data_end;

  return 1;
}

// appender:add(name, content, [options]) -> true | nil, error
static int LuaZipAppenderAdd(lua_State *L) {
  struct LuaZipAppender *a = GetZipAppender(L);
  size_t namelen, contentlen;
  const char *name = luaL_checklstring(L, 2, &namelen);
  const char *content = luaL_checklstring(L, 3, &contentlen);

  if (a->fd == -1)
    return ZipError(L, "zip appender is closed");

  const char *name_err = ValidateEntryName(name, namelen);
  if (name_err)
    return ZipError(L, name_err);

  if (AppenderHasEntry(a, name, namelen))
    return ZipError(L, "duplicate entry name");

  if (contentlen > UINT_MAX)
    return ZipError(L, "content too large (exceeds 4GB limit)");

  if ((int64_t)contentlen > a->max_file_size)
    return ZipError(L, "content exceeds max_file_size limit");

  // Parse options
  bool force_store = false;
  bool force_deflate = false;
  int64_t mtime_unix = time(NULL);
  uint32_t mode = 0100644;

  if (lua_istable(L, 4)) {
    lua_getfield(L, 4, "method");
    if (!lua_isnil(L, -1)) {
      const char *m = luaL_checkstring(L, -1);
      if (!strcmp(m, "store"))
        force_store = true;
      else if (!strcmp(m, "deflate"))
        force_deflate = true;
      else
        return ZipError(L, "unknown method");
    }
    lua_pop(L, 1);

    lua_getfield(L, 4, "mtime");
    if (!lua_isnil(L, -1))
      mtime_unix = luaL_checkinteger(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 4, "mode");
    if (!lua_isnil(L, -1))
      mode = luaL_checkinteger(L, -1);
    lua_pop(L, 1);
  }

  // Validate mode
  if ((mode & 0170000) != 0100000 && (mode & 0170000) != 0)
    return ZipError(L, "mode must be a regular file");
  if ((mode & 0170000) == 0)
    mode |= 0100000;

  // Compute CRC32
  uint32_t crc = crc32_z(0, (const uint8_t *)content, contentlen);

  // Convert mtime
  uint16_t mtime, mdate;
  GetDosLocalTime(mtime_unix, &mtime, &mdate);

  // Decide compression and compress if needed
  uint16_t method = kZipCompressionNone;
  void *compdata = NULL;
  size_t compsize = contentlen;

  if (!force_store &&
      (force_deflate || ShouldCompress(name, namelen, content, contentlen, a->level))) {
    if (ZipDeflate(content, contentlen, &compdata, &compsize, a->level) < 0)
      return ZipError(L, "deflate failed");
    if (compdata)
      method = kZipCompressionDeflate;
  }

  // Appender needs to store compressed data for later writing
  uint8_t *stored_data;
  if (compdata) {
    stored_data = compdata;
  } else {
    stored_data = malloc(contentlen ? contentlen : 1);
    if (!stored_data)
      return SysError(L, "malloc");
    if (contentlen > 0)
      memcpy(stored_data, content, contentlen);
  }

  // Grow pending arrays if needed
  if (a->pending_count >= a->pending_capacity) {
    size_t newcap = a->pending_capacity ? a->pending_capacity * 2 : 16;
    struct LuaZipCdirEntry *newpending =
        realloc(a->pending, newcap * sizeof(*newpending));
    if (!newpending) {
      free(stored_data);
      return SysError(L, "malloc");
    }
    a->pending = newpending;
    uint8_t **newdata = realloc(a->pending_data, newcap * sizeof(*newdata));
    if (!newdata) {
      free(stored_data);
      return SysError(L, "malloc");
    }
    a->pending_data = newdata;
    a->pending_capacity = newcap;
  }

  // Add entry
  struct LuaZipCdirEntry *e = &a->pending[a->pending_count];
  e->name = malloc(namelen + 1);
  if (!e->name) {
    free(stored_data);
    return SysError(L, "malloc");
  }
  memcpy(e->name, name, namelen);
  e->name[namelen] = '\0';
  e->namelen = namelen;
  e->offset = 0;  // will be set during close
  e->compsize = compsize;
  e->uncompsize = contentlen;
  e->crc32 = crc;
  e->method = method;
  e->mtime = mtime;
  e->mdate = mdate;
  e->mode = mode;

  a->pending_data[a->pending_count] = stored_data;
  a->pending_count++;

  lua_pushboolean(L, 1);
  return 1;
}

// appender:close() -> true | nil, error
static int LuaZipAppenderClose(lua_State *L) {
  struct LuaZipAppender *a = GetZipAppender(L);

  if (a->fd == -1) {
    lua_pushboolean(L, 1);
    return 1;
  }

  // If no pending entries, just close
  if (a->pending_count == 0) {
    AppenderCleanup(a);
    lua_pushboolean(L, 1);
    return 1;
  }

  int fd = a->fd;

  // Seek to where we'll write new local files
  int64_t write_offset = a->data_end;
  if (lseek(fd, write_offset, SEEK_SET) == -1) {
    AppenderCleanup(a);
    return SysError(L, "lseek");
  }

  // Write new local file entries
  for (size_t i = 0; i < a->pending_count; i++) {
    struct LuaZipCdirEntry *e = &a->pending[i];
    e->offset = write_offset;  // absolute file offset

    size_t hdrlen = GetLfileHdrSize(e->namelen, e->compsize, e->uncompsize);
    uint8_t *lochdr = malloc(hdrlen);
    if (!lochdr) {
      AppenderCleanup(a);
      return SysError(L, "malloc");
    }

    EmitZipLfileHdr(lochdr, e->name, e->namelen, e->crc32, e->method, e->mtime,
                    e->mdate, e->compsize, e->uncompsize);

    ssize_t written = write(fd, lochdr, hdrlen);
    free(lochdr);
    if (written != (ssize_t)hdrlen) {
      AppenderCleanup(a);
      return SysError(L, "write header");
    }

    written = write(fd, a->pending_data[i], e->compsize);
    if (written != (ssize_t)e->compsize) {
      AppenderCleanup(a);
      return SysError(L, "write data");
    }

    write_offset += hdrlen + e->compsize;
  }

  // Write central directory
  int64_t cdir_offset = write_offset;  // absolute file offset
  int64_t cdir_size = 0;
  size_t total_entries = a->existing_count + a->pending_count;

  // Write existing entries
  for (size_t i = 0; i < a->existing_count; i++) {
    ssize_t written = WriteCdirEntry(fd, &a->existing[i], a->existing[i].offset);
    if (written < 0) {
      AppenderCleanup(a);
      return SysError(L, "write cdir");
    }
    cdir_size += written;
  }

  // Write pending entries
  for (size_t i = 0; i < a->pending_count; i++) {
    ssize_t written = WriteCdirEntry(fd, &a->pending[i], a->pending[i].offset);
    if (written < 0) {
      AppenderCleanup(a);
      return SysError(L, "write cdir");
    }
    cdir_size += written;
  }

  // Write end of central directory
  if (WriteZipEocd(fd, total_entries, cdir_offset, cdir_size) < 0) {
    AppenderCleanup(a);
    return SysError(L, "write eocd");
  }

  // Truncate file to remove old central directory
  int64_t final_size = lseek(fd, 0, SEEK_CUR);
  if (ftruncate(fd, final_size) == -1) {
    AppenderCleanup(a);
    return SysError(L, "ftruncate");
  }

  AppenderCleanup(a);
  lua_pushboolean(L, 1);
  return 1;
}

// appender:__gc()
static int LuaZipAppenderGc(lua_State *L) {
  struct LuaZipAppender *a = GetZipAppender(L);
  AppenderCleanup(a);
  return 0;
}

// appender:__tostring()
static int LuaZipAppenderTostring(lua_State *L) {
  struct LuaZipAppender *a = GetZipAppender(L);
  if (a->fd == -1) {
    lua_pushliteral(L, "zip.Appender (closed)");
  } else {
    lua_pushfstring(L, "zip.Appender (%d pending)",
                    (int)a->pending_count);
  }
  return 1;
}

////////////////////////////////////////////////////////////////////////////////
// Module Registration
////////////////////////////////////////////////////////////////////////////////

static const luaL_Reg kLuaZipReaderMeta[] = {
    {"__gc", LuaZipReaderGc},
    {"__tostring", LuaZipReaderTostring},
    {"__close", LuaZipReaderClose},
    {0},
};

static const luaL_Reg kLuaZipReaderMethods[] = {
    {"close", LuaZipReaderClose},
    {"list", LuaZipReaderList},
    {"stat", LuaZipReaderStat},
    {"read", LuaZipReaderRead},
    {0},
};

static const luaL_Reg kLuaZipWriterMeta[] = {
    {"__gc", LuaZipWriterGc},
    {"__tostring", LuaZipWriterTostring},
    {"__close", LuaZipWriterClose},
    {0},
};

static const luaL_Reg kLuaZipWriterMethods[] = {
    {"close", LuaZipWriterClose},
    {"add", LuaZipWriterAdd},
    {0},
};

static const luaL_Reg kLuaZipAppenderMeta[] = {
    {"__gc", LuaZipAppenderGc},
    {"__tostring", LuaZipAppenderTostring},
    {"__close", LuaZipAppenderClose},
    {0},
};

static const luaL_Reg kLuaZipAppenderMethods[] = {
    {"close", LuaZipAppenderClose},
    {"add", LuaZipAppenderAdd},
    {0},
};

// zip.validate_name(name) -> true | nil, error
// Validates a zip entry name without adding it to an archive
static int LuaZipValidateName(lua_State *L) {
  size_t namelen;
  const char *name = luaL_checklstring(L, 1, &namelen);
  const char *err = ValidateEntryName(name, namelen);
  if (err) {
    lua_pushnil(L);
    lua_pushstring(L, err);
    return 2;
  }
  lua_pushboolean(L, 1);
  return 1;
}

static const luaL_Reg kLuaZip[] = {
    {"open", LuaZipOpen},
    {"from", LuaZipFrom},
    {"create", LuaZipCreate},
    {"append", LuaZipAppend},
    {"validate_name", LuaZipValidateName},
    {0},
};

int LuaZip(lua_State *L) {
  // create zip.Reader metatable
  luaL_newmetatable(L, LUA_ZIP_READER);
  luaL_setfuncs(L, kLuaZipReaderMeta, 0);
  luaL_newlibtable(L, kLuaZipReaderMethods);
  luaL_setfuncs(L, kLuaZipReaderMethods, 0);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);

  // create zip.Writer metatable
  luaL_newmetatable(L, LUA_ZIP_WRITER);
  luaL_setfuncs(L, kLuaZipWriterMeta, 0);
  luaL_newlibtable(L, kLuaZipWriterMethods);
  luaL_setfuncs(L, kLuaZipWriterMethods, 0);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);

  // create zip.Appender metatable
  luaL_newmetatable(L, LUA_ZIP_APPENDER);
  luaL_setfuncs(L, kLuaZipAppenderMeta, 0);
  luaL_newlibtable(L, kLuaZipAppenderMethods);
  luaL_setfuncs(L, kLuaZipAppenderMethods, 0);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);

  // create module table
  luaL_newlib(L, kLuaZip);
  return 1;
}
