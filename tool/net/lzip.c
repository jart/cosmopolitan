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
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/zip.h"
#include "third_party/lua/lauxlib.h"
#include "third_party/lua/lua.h"
#include "third_party/zlib/zlib.h"

#define LUA_ZIP_READER "zip.Reader"
#define MAX_CDIR_SIZE (256 * 1024 * 1024)
#define MAX_FILE_SIZE (1024 * 1024 * 1024)

struct LuaZipReader {
  int fd;
  uint8_t *cdir;
  int64_t cdir_size;
  int64_t count;
  int64_t file_size;
};

static struct LuaZipReader *GetZipReader(lua_State *L) {
  return luaL_checkudata(L, 1, LUA_ZIP_READER);
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

// zip.open(path) -> reader, nil | nil, error
static int LuaZipOpen(lua_State *L) {
  const char *path;
  struct LuaZipReader *z;
  int64_t zsize, off, amt;
  int64_t cnt, cdir_off, cdir_size;
  char last64[65536];
  ssize_t rc;

  path = luaL_checkstring(L, 1);

  // open file
  int fd = open(path, O_RDONLY);
  if (fd == -1)
    return SysError(L, path);

  // get file size
  zsize = lseek(fd, 0, SEEK_END);
  if (zsize == -1) {
    close(fd);
    return SysError(L, path);
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
    close(fd);
    return SysError(L, path);
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
    close(fd);
    return ZipError(L, "not a zip file");
  }

  if (cdir_size > MAX_CDIR_SIZE) {
    close(fd);
    return ZipError(L, "central directory too large");
  }

  if (cdir_off < 0 || cdir_off + cdir_size > zsize) {
    close(fd);
    return ZipError(L, "central directory offset out of bounds");
  }

  // read central directory
  uint8_t *cdir = malloc(cdir_size);
  if (!cdir) {
    close(fd);
    return SysError(L, "malloc");
  }
  for (int64_t i = 0; i < cdir_size; i += rc) {
    rc = pread(fd, cdir + i, cdir_size - i, cdir_off + i);
    if (rc <= 0) {
      free(cdir);
      close(fd);
      return SysError(L, path);
    }
  }

  // create userdata
  z = lua_newuserdatauv(L, sizeof(*z), 0);
  luaL_setmetatable(L, LUA_ZIP_READER);
  z->fd = fd;
  z->cdir = cdir;
  z->cdir_size = cdir_size;
  z->count = cnt;
  z->file_size = zsize;

  return 1;
}

// reader:close()
static int LuaZipReaderClose(lua_State *L) {
  struct LuaZipReader *z = GetZipReader(L);
  if (z->fd != -1) {
    close(z->fd);
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

  if (compressed_size > MAX_FILE_SIZE)
    return ZipError(L, "compressed size too large");
  if (uncompressed_size > MAX_FILE_SIZE)
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

static const luaL_Reg kLuaZip[] = {
    {"open", LuaZipOpen},
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

  // create module table
  luaL_newlib(L, kLuaZip);
  return 1;
}
