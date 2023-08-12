// clang-format off
// This file contains functions to read an archive file (.a file).
// An archive file is just a bundle of object files. It's similar to
// tar or zip, but the contents are not compressed.
//
// An archive file is either "regular" or "thin". A regular archive
// contains object files directly, while a thin archive contains only
// pathnames. In the latter case, actual file contents have to be read
// from given pathnames. A regular archive is sometimes called "fat"
// archive as opposed to "thin".
//
// If an archive file is given to the linker, the linker pulls out
// object files that are needed to resolve undefined symbols. So,
// bunding object files as an archive and giving that archive to the
// linker has a different meaning than directly giving the same set of
// object files to the linker. The former links only needed object
// files, while the latter links all the given object files.
//
// Therefore, if you link libc.a for example, not all the libc
// functions are linked to your binary. Instead, only object files
// that provides functions and variables used in your program get
// linked. To make this efficient, static library functions are
// usually separated to each object file in an archive file. You can
// see the contents of libc.a by running `ar t
// /usr/lib/x86_64-linux-gnu/libc.a`.

#pragma once

#include "third_party/mold/common.h"
#include "third_party/mold/filetype.h"

namespace mold {

struct ArHdr {
  char ar_name[16];
  char ar_date[12];
  char ar_uid[6];
  char ar_gid[6];
  char ar_mode[8];
  char ar_size[10];
  char ar_fmag[2];

  bool starts_with(std::string_view s) const {
    return std::string_view(ar_name, s.size()) == s;
  }

  bool is_strtab() const {
    return starts_with("// ");
  }

  bool is_symtab() const {
    return starts_with("/ ") || starts_with("/SYM64/ ");
  }

  std::string read_name(std::string_view strtab, u8 *&ptr) const {
    // BSD-style long filename
    if (starts_with("#1/")) {
      int namelen = atoi(ar_name + 3);
      std::string name{(char *)ptr, (size_t)namelen};
      ptr += namelen;

      if (size_t pos = name.find('\0'))
        name = name.substr(0, pos);
      return name;
    }

    // SysV-style long filename
    if (starts_with("/")) {
      const char *start = strtab.data() + atoi(ar_name + 1);
      return {start, (const char *)strstr(start, "/\n")};
    }

    // Short fileanme
    if (const char *end = (char *)memchr(ar_name, '/', sizeof(ar_name)))
      return {ar_name, end};
    return {ar_name, sizeof(ar_name)};
  }
};

template <typename Context, typename MappedFile>
std::vector<MappedFile *>
read_thin_archive_members(Context &ctx, MappedFile *mf) {
  u8 *begin = mf->data;
  u8 *data = begin + 8;
  std::vector<MappedFile *> vec;
  std::string_view strtab;

  while (data < begin + mf->size) {
    // Each header is aligned to a 2 byte boundary.
    if ((begin - data) % 2)
      data++;

    ArHdr &hdr = *(ArHdr *)data;
    u8 *body = data + sizeof(hdr);
    u64 size = atol(hdr.ar_size);

    // Read a string table.
    if (hdr.is_strtab()) {
      strtab = {(char *)body, (size_t)size};
      data = body + size;
      continue;
    }

    // Skip a symbol table.
    if (hdr.is_symtab()) {
      data = body + size;
      continue;
    }

    if (!hdr.starts_with("#1/") && !hdr.starts_with("/"))
      Fatal(ctx) << mf->name << ": filename is not stored as a long filename";

    std::string name = hdr.read_name(strtab, body);

    // Skip if symbol table
    if (name == "__.SYMDEF" || name == "__.SYMDEF SORTED")
      continue;

    std::string path = name.starts_with('/') ?
      name : (filepath(mf->name).parent_path() / name).string();
    vec.push_back(MappedFile::must_open(ctx, path));
    vec.back()->thin_parent = mf;
    data = body;
  }
  return vec;
}

template <typename Context, typename MappedFile>
std::vector<MappedFile *> read_fat_archive_members(Context &ctx, MappedFile *mf) {
  u8 *begin = mf->data;
  u8 *data = begin + 8;
  std::vector<MappedFile *> vec;
  std::string_view strtab;

  while (begin + mf->size - data >= 2) {
    if ((begin - data) % 2)
      data++;

    ArHdr &hdr = *(ArHdr *)data;
    u8 *body = data + sizeof(hdr);
    u64 size = atol(hdr.ar_size);
    data = body + size;

    // Read if string table
    if (hdr.is_strtab()) {
      strtab = {(char *)body, (size_t)size};
      continue;
    }

    // Skip if symbol table
    if (hdr.is_symtab())
      continue;

    // Read the name field
    std::string name = hdr.read_name(strtab, body);

    // Skip if symbol table
    if (name == "__.SYMDEF" || name == "__.SYMDEF SORTED")
      continue;

    vec.push_back(mf->slice(ctx, name, body - begin, data - body));
  }
  return vec;
}

template <typename Context, typename MappedFile>
std::vector<MappedFile *> read_archive_members(Context &ctx, MappedFile *mf) {
  switch (get_file_type(ctx, mf)) {
  case FileType::AR:
    return read_fat_archive_members(ctx, mf);
  case FileType::THIN_AR:
    return read_thin_archive_members(ctx, mf);
  default:
    unreachable();
  }
}

} // namespace mold
