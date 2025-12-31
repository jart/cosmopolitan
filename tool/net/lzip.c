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
#include "libc/dos.h"
#include "libc/errno.h"
#include "libc/limits.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/time.h"
#include "libc/zip.h"
#include "third_party/lua/lauxlib.h"
#include "third_party/lua/lua.h"
#include "third_party/zlib/zlib.h"

#define LUA_ZIP_READER "zip.Reader"
#define LUA_ZIP_WRITER "zip.Writer"
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

static struct LuaZipReader *GetZipReader(lua_State *L) {
  return luaL_checkudata(L, 1, LUA_ZIP_READER);
}

static struct LuaZipWriter *GetZipWriter(lua_State *L) {
  return luaL_checkudata(L, 1, LUA_ZIP_WRITER);
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

////////////////////////////////////////////////////////////////////////////////
// Reader Implementation
////////////////////////////////////////////////////////////////////////////////

// zip.open(path|fd, [options]) -> reader, nil | nil, error
static int LuaZipOpen(lua_State *L) {
  const char *path = NULL;
  struct LuaZipReader *z;
  int64_t zsize, off, amt;
  int64_t cnt, cdir_off, cdir_size;
  int64_t max_file_size = MAX_FILE_SIZE;
  char last64[65536];
  ssize_t rc;
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

  // read last 64kb
  if (zsize <= 65536) {
    off = 0;
    amt = zsize;
  } else {
    off = zsize - 65536;
    amt = zsize - off;
  }
  if (pread(fd, last64, amt, off) != amt) {
    if (owns_fd) close(fd);
    return SysError(L, path ? path : "fd");
  }

  // find end of central directory
  cnt = -1;
  cdir_off = 0;
  cdir_size = 0;
  for (int i = amt - kZipCdirHdrMinSize; i >= 0; --i) {
    uint32_t magic = ZIP_READ32(last64 + i);
    if (magic == kZipCdir64LocatorMagic &&
        i + (int)kZipCdir64LocatorSize <= amt) {
      char hdr[kZipCdir64HdrMinSize];
      if (pread(fd, hdr, kZipCdir64HdrMinSize,
                ZIP_LOCATE64_OFFSET(last64 + i)) == (long)kZipCdir64HdrMinSize &&
          ZIP_READ32(hdr) == kZipCdir64HdrMagic &&
          ZIP_CDIR64_RECORDS(hdr) == ZIP_CDIR64_RECORDSONDISK(hdr)) {
        cnt = ZIP_CDIR64_RECORDS(hdr);
        cdir_off = ZIP_CDIR64_OFFSET(hdr);
        cdir_size = ZIP_CDIR64_SIZE(hdr);
        break;
      }
    }
    if (magic == kZipCdirHdrMagic && i + (int)kZipCdirHdrMinSize <= amt &&
        ZIP_CDIR_RECORDS(last64 + i) == ZIP_CDIR_RECORDSONDISK(last64 + i) &&
        ZIP_CDIR_OFFSET(last64 + i) != 0xffffffffu) {
      cnt = ZIP_CDIR_RECORDS(last64 + i);
      cdir_off = ZIP_CDIR_OFFSET(last64 + i);
      cdir_size = ZIP_CDIR_SIZE(last64 + i);
      break;
    }
  }

  if (cnt < 0) {
    if (owns_fd) close(fd);
    return ZipError(L, "not a zip file");
  }

  if (cdir_size > MAX_CDIR_SIZE) {
    if (owns_fd) close(fd);
    return ZipError(L, "central directory too large");
  }

  if (cdir_off < 0 || cdir_off + cdir_size > zsize) {
    if (owns_fd) close(fd);
    return ZipError(L, "central directory offset out of bounds");
  }

  // read central directory
  uint8_t *cdir = malloc(cdir_size);
  if (!cdir) {
    if (owns_fd) close(fd);
    return SysError(L, "malloc");
  }
  for (int64_t i = 0; i < cdir_size; i += rc) {
    rc = pread(fd, cdir + i, cdir_size - i, cdir_off + i);
    if (rc <= 0) {
      free(cdir);
      if (owns_fd) close(fd);
      return SysError(L, path ? path : "fd");
    }
  }

  // create userdata
  z = lua_newuserdatauv(L, sizeof(*z), 0);
  luaL_setmetatable(L, LUA_ZIP_READER);
  z->fd = fd;
  z->owns_fd = owns_fd;
  z->cdir = cdir;
  z->cdir_size = cdir_size;
  z->count = cnt;
  z->file_size = zsize;
  z->max_file_size = max_file_size;

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
  return 0;
}

// reader:__gc()
static int LuaZipReaderGc(lua_State *L) {
  return LuaZipReaderClose(L);
}

// reader:list() -> {name, ...}
static int LuaZipReaderList(lua_State *L) {
  struct LuaZipReader *z = GetZipReader(L);
  if (z->fd == -1)
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

  if (z->fd == -1)
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

  if (z->fd == -1)
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
  if (pread(z->fd, lfile_hdr, kZipLfileHdrMinSize, lfile_off) !=
      kZipLfileHdrMinSize)
    return SysError(L, "pread lfile");
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
    rc = pread(z->fd, compressed + i, compressed_size - i, data_off + i);
    if (rc <= 0) {
      free(compressed);
      return SysError(L, "pread data");
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
  if (z->fd == -1) {
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
  int method = w->level > 0 ? kZipCompressionDeflate : kZipCompressionNone;
  int64_t mtime_unix = time(NULL);
  uint32_t mode = 0100644;  // regular file with 644 permissions

  if (lua_istable(L, 4)) {
    lua_getfield(L, 4, "method");
    if (!lua_isnil(L, -1)) {
      const char *m = luaL_checkstring(L, -1);
      if (!strcmp(m, "store"))
        method = kZipCompressionNone;
      else if (!strcmp(m, "deflate"))
        method = kZipCompressionDeflate;
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

  // validate mode is a regular file (reject symlinks, devices, etc.)
  if ((mode & 0170000) != 0100000 && (mode & 0170000) != 0)
    return ZipError(L, "mode must be a regular file");

  // ensure regular file bit is set
  if ((mode & 0170000) == 0)
    mode |= 0100000;

  // compute CRC32 of uncompressed data
  uint32_t crc = crc32_z(0, (const uint8_t *)content, contentlen);

  // convert mtime
  uint16_t mtime, mdate;
  GetDosLocalTime(mtime_unix, &mtime, &mdate);

  // compress if needed
  uint8_t *compdata = NULL;
  size_t compsize = contentlen;

  if (method == kZipCompressionDeflate && contentlen > 0) {
    z_stream strm = {0};
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;

    int ret = deflateInit2(&strm, w->level, Z_DEFLATED, -MAX_WBITS,
                           DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY);
    if (ret != Z_OK)
      return ZipError(L, "deflateInit2 failed");

    size_t bound = deflateBound(&strm, contentlen);
    compdata = malloc(bound);
    if (!compdata) {
      deflateEnd(&strm);
      return SysError(L, "malloc");
    }

    strm.next_in = (uint8_t *)content;
    strm.avail_in = contentlen;
    strm.next_out = compdata;
    strm.avail_out = bound;

    ret = deflate(&strm, Z_FINISH);
    if (ret != Z_STREAM_END) {
      deflateEnd(&strm);
      free(compdata);
      return ZipError(L, "deflate failed");
    }

    compsize = strm.total_out;
    deflateEnd(&strm);

    // if compressed is larger, store instead
    if (compsize >= contentlen) {
      free(compdata);
      compdata = NULL;
      compsize = contentlen;
      method = kZipCompressionNone;
    }
  } else {
    method = kZipCompressionNone;
  }

  // determine if we need zip64 extra field
  bool needzip64 = (contentlen >= 0xffffffffu || compsize >= 0xffffffffu ||
                    w->offset >= 0xffffffffu);
  size_t extlen = needzip64 ? (2 + 2 + 8 + 8) : 0;
  size_t hdrlen = kZipLfileHdrMinSize + namelen + extlen;

  // build local file header
  uint8_t *lochdr = malloc(hdrlen);
  if (!lochdr) {
    if (compdata)
      free(compdata);
    return SysError(L, "malloc");
  }

  uint8_t *p = lochdr;
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
    p = ZIP_WRITE32(p, contentlen);
  }
  p = ZIP_WRITE16(p, namelen);
  p = ZIP_WRITE16(p, extlen);
  memcpy(p, name, namelen);
  p += namelen;

  if (needzip64) {
    p = ZIP_WRITE16(p, kZipExtraZip64);
    p = ZIP_WRITE16(p, 8 + 8);
    p = ZIP_WRITE64(p, contentlen);
    p = ZIP_WRITE64(p, compsize);
  }

  // write local file header
  ssize_t written = write(w->fd, lochdr, hdrlen);
  free(lochdr);
  if (written != (ssize_t)hdrlen) {
    if (compdata)
      free(compdata);
    return WriterSysError(L, w, "write header");
  }

  // write file data
  const void *writedata = compdata ? (const void *)compdata : (const void *)content;
  written = write(w->fd, writedata, compsize);
  if (compdata)
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
    struct LuaZipCdirEntry *e = &w->entries[i];

    bool needzip64 = (e->uncompsize >= 0xffffffffu ||
                      e->compsize >= 0xffffffffu || e->offset >= 0xffffffffu);
    size_t extlen = needzip64 ? (2 + 2 + 8 + 8 + 8) : 0;
    size_t hdrlen = kZipCfileHdrMinSize + e->namelen + extlen;

    uint8_t *cdirhdr = malloc(hdrlen);
    if (!cdirhdr)
      return SysError(L, "malloc");

    uint8_t *p = cdirhdr;
    p = ZIP_WRITE32(p, kZipCfileHdrMagic);
    p = ZIP_WRITE16(p, kZipOsUnix << 8 | (needzip64 ? kZipEra2001 : kZipEra1993));
    p = ZIP_WRITE16(p, needzip64 ? kZipEra2001 : kZipEra1993);
    p = ZIP_WRITE16(p, kZipGflagUtf8);
    p = ZIP_WRITE16(p, e->method);
    p = ZIP_WRITE16(p, e->mtime);
    p = ZIP_WRITE16(p, e->mdate);
    p = ZIP_WRITE32(p, e->crc32);
    if (needzip64) {
      p = ZIP_WRITE32(p, 0xffffffffu);
      p = ZIP_WRITE32(p, 0xffffffffu);
    } else {
      p = ZIP_WRITE32(p, e->compsize);
      p = ZIP_WRITE32(p, e->uncompsize);
    }
    p = ZIP_WRITE16(p, e->namelen);
    p = ZIP_WRITE16(p, extlen);
    p = ZIP_WRITE16(p, 0);  // comment length
    p = ZIP_WRITE16(p, 0);  // disk number start
    p = ZIP_WRITE16(p, 0);  // internal file attributes
    p = ZIP_WRITE32(p, e->mode << 16);  // external file attributes
    if (needzip64) {
      p = ZIP_WRITE32(p, 0xffffffffu);
    } else {
      p = ZIP_WRITE32(p, e->offset);
    }
    memcpy(p, e->name, e->namelen);
    p += e->namelen;

    if (needzip64) {
      p = ZIP_WRITE16(p, kZipExtraZip64);
      p = ZIP_WRITE16(p, 8 + 8 + 8);
      p = ZIP_WRITE64(p, e->uncompsize);
      p = ZIP_WRITE64(p, e->compsize);
      p = ZIP_WRITE64(p, e->offset);
    }

    ssize_t written = write(w->fd, cdirhdr, hdrlen);
    free(cdirhdr);
    if (written != (ssize_t)hdrlen)
      return WriterSysError(L, w, "write cdir entry");

    cdir_size += hdrlen;
  }

  // determine if we need zip64 end of central directory
  bool needzip64 =
      (w->entry_count >= 0xffff || cdir_size >= 0xffffffffu ||
       cdir_offset >= 0xffffffffu);

  if (needzip64) {
    // write zip64 end of central directory record
    uint8_t eocd64[kZipCdir64HdrMinSize];
    uint8_t *p = eocd64;
    p = ZIP_WRITE32(p, kZipCdir64HdrMagic);
    p = ZIP_WRITE64(p, kZipCdir64HdrMinSize - 12);
    p = ZIP_WRITE16(p, kZipOsUnix << 8 | kZipEra2001);
    p = ZIP_WRITE16(p, kZipEra2001);
    p = ZIP_WRITE32(p, 0);  // disk number
    p = ZIP_WRITE32(p, 0);  // disk with cdir
    p = ZIP_WRITE64(p, w->entry_count);
    p = ZIP_WRITE64(p, w->entry_count);
    p = ZIP_WRITE64(p, cdir_size);
    p = ZIP_WRITE64(p, cdir_offset);

    if (write(w->fd, eocd64, sizeof(eocd64)) != sizeof(eocd64))
      return WriterSysError(L, w, "write eocd64");

    // write zip64 end of central directory locator
    uint8_t loc64[kZipCdir64LocatorSize];
    p = loc64;
    p = ZIP_WRITE32(p, kZipCdir64LocatorMagic);
    p = ZIP_WRITE32(p, 0);  // disk with eocd64
    p = ZIP_WRITE64(p, cdir_offset + cdir_size);
    p = ZIP_WRITE32(p, 1);  // total disks

    if (write(w->fd, loc64, sizeof(loc64)) != sizeof(loc64))
      return WriterSysError(L, w, "write loc64");
  }

  // write end of central directory record
  uint8_t eocd[kZipCdirHdrMinSize];
  uint8_t *p = eocd;
  p = ZIP_WRITE32(p, kZipCdirHdrMagic);
  p = ZIP_WRITE16(p, 0);  // disk number
  p = ZIP_WRITE16(p, 0);  // disk with cdir
  p = ZIP_WRITE16(p, w->entry_count >= 0xffff ? 0xffff : w->entry_count);
  p = ZIP_WRITE16(p, w->entry_count >= 0xffff ? 0xffff : w->entry_count);
  p = ZIP_WRITE32(p, cdir_size >= 0xffffffffu ? 0xffffffffu : cdir_size);
  p = ZIP_WRITE32(p, cdir_offset >= 0xffffffffu ? 0xffffffffu : cdir_offset);
  p = ZIP_WRITE16(p, 0);  // comment length

  if (write(w->fd, eocd, sizeof(eocd)) != sizeof(eocd))
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
    {"create", LuaZipCreate},
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

  // create module table
  luaL_newlib(L, kLuaZip);
  return 1;
}
