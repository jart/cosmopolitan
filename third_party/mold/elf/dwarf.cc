// clang-format off
// This file contains code to read DWARF debug info to create .gdb_index.
//
// .gdb_index is an optional section to speed up GNU debugger. It contains
// two maps: 1) a map from function/variable/type names to compunits, and
// 2) a map from function address ranges to compunits. gdb uses these
// maps to quickly find a compunit given a name or an instruction pointer.
//
// (Terminology: a compilation unit, which often abbreviated as compunit
// or cu, is a unit of debug info. An input .debug_info section usually
// contains one compunit, and thus an output .debug_info contains as
// many compunits as the number of input files.)
//
// .gdb_index is not mandatory. All the information in .gdb_index is
// also in other debug info sections. You can actually create an
// executable without .gdb_index and later add it using `gdb-add-index`
// post-processing tool that comes with gdb.
//
// The mapping from names to compunits is 1:n while the mapping from
// address ranges to compunits is 1:1. That is, two object files may
// define the same type name (with the same definition), while there
// should be no two functions that overlap with each other in memory.
//
// .gdb_index contains an on-disk hash table for names, so gdb can
// lookup names without loading all strings into memory and construct an
// in-memory hash table.
//
// Names are in .debug_gnu_pubnames and .debug_gnu_pubtypes input
// sections. These sections are created if `-ggnu-pubnames` is given.
// Besides names, these sections contains attributes for each name so
// that gdb can distinguish type names from function names, for example.
//
// A compunit contains one or more function address ranges. If an
// object file is compiled without -ffunction-sections, it contains
// only one .text section and therefore contains a single address range.
// Such range is typically stored directly to the compunit.
//
// If an object file is compiled with -fucntion-sections, it contains
// more than one .text section, and it has as many address ranges as
// the number of .text sections. Such discontiguous address ranges are
// stored to .debug_ranges in DWARF 2/3/4/5 and
// .debug_rnglists/.debug_addr in DWARF 5.
//
// .debug_info section contains DWARF debug info. Although we don't need
// to parse the whole .debug_info section to read address ranges, we
// have to do a little bit. DWARF is complicated and often handled using
// a library such as libdwarf. But we don't use any library because we
// don't want to add an extra run-time dependency just for --gdb-index.
//
// This page explains the format of .gdb_index:
// https://sourceware.org/gdb/onlinedocs/gdb/Index-Section-Format.html

#include "third_party/mold/elf/mold.h"

namespace mold::elf {

// The hash function for .gdb_index.
static u32 gdb_hash(std::string_view name) {
  u32 h = 0;
  for (u8 c : name) {
    if ('A' <= c && c <= 'Z')
      c = 'a' + c - 'A';
    h = h * 67 + c - 113;
  }
  return h;
}

// Split .debug_info into so-called "compilation units". A .debug_info
// section usually contains one compunit unless it was created by `ld -r`.
// This is for --gdb-index.
template <typename E>
std::vector<std::string_view>
read_compunits(Context<E> &ctx, ObjectFile<E> &file) {
  file.debug_info->uncompress(ctx);
  std::string_view data = file.debug_info->contents;
  std::vector<std::string_view> vec;

  while (!data.empty()) {
    if (data.size() < 4)
      Fatal(ctx) << *file.debug_info << ": corrupted .debug_info";
    if (*(U32<E> *)data.data() == 0xffff'ffff)
      Fatal(ctx) << *file.debug_info << ": --gdb-index: DWARF64 not supported";
    i64 len = *(U32<E> *)data.data() + 4;
    vec.push_back(data.substr(0, len));
    data = data.substr(len);
  }
  return vec;
}

// Parses .debug_gnu_pubnames and .debug_gnu_pubtypes. These sections
// start with a 14 bytes header followed by (4-byte offset, 1-byte type,
// null-terminated string) tuples.
//
// The 4-byte offset is an offset into .debug_info that contains details
// about the name. The 1-byte type is a type of the corresponding name
// (e.g. function, variable or datatype). The string is a name of a
// function, a variable or a type.
template <typename E>
std::vector<GdbIndexName> read_pubnames(Context<E> &ctx, ObjectFile<E> &file) {
  std::vector<GdbIndexName> vec;

  auto get_cu_idx = [&](InputSection<E> &isec, i64 offset) {
    i64 off = 0;
    for (i64 i = 0; i < file.compunits.size(); i++) {
      if (offset == off)
        return file.compunits_idx + i;
      off += file.compunits[i].size();
    }
    Fatal(ctx) << isec << ": corrupted debug_info_offset";
  };

  auto read = [&](InputSection<E> &isec) {
    isec.uncompress(ctx);
    std::string_view contents = isec.contents;

    while (!contents.empty()) {
      if (contents.size() < 14)
        Fatal(ctx) << isec << ": corrupted header";

      u32 len = *(U32<E> *)contents.data() + 4;
      u32 debug_info_offset = *(U32<E> *)(contents.data() + 6);
      u32 cu_idx = get_cu_idx(isec, debug_info_offset);

      std::string_view data = contents.substr(14, len - 14);
      contents = contents.substr(len);

      while (!data.empty()) {
        u32 offset = *(U32<E> *)data.data();
        data = data.substr(4);
        if (offset == 0)
          break;

        u8 type = data[0];
        data = data.substr(1);

        std::string_view name = data.data();
        data = data.substr(name.size() + 1);

        vec.push_back({name, gdb_hash(name), (type << 24) | cu_idx});
      }
    }
  };

  if (file.debug_pubnames)
    read(*file.debug_pubnames);
  if (file.debug_pubtypes)
    read(*file.debug_pubtypes);

  // Uniquify elements because GCC 11 seems to emit one record for each
  // comdat group which results in having a lot of duplicate records.
  auto less = [](const GdbIndexName &a, const GdbIndexName &b) {
    return std::tuple{a.hash, a.attr, a.name} <
           std::tuple{b.hash, b.attr, b.name};
  };

  auto equal = [](const GdbIndexName &a, const GdbIndexName &b) {
    return std::tuple{a.hash, a.attr, a.name} ==
           std::tuple{b.hash, b.attr, b.name};
  };

  std::sort(vec.begin(), vec.end(), less);
  vec.erase(std::unique(vec.begin(), vec.end(), equal), vec.end());
  return vec;
}

template <typename E>
static u8 *get_buffer(Context<E> &ctx, Chunk<E> *chunk) {
  if (u8 *buf = chunk->get_uncompressed_data())
    return buf;
  return ctx.buf + chunk->shdr.sh_offset;
}

// Try to find a compilation unit from .debug_info and its
// corresponding record from .debug_abbrev and returns them.
template <typename E>
static std::tuple<u8 *, u8 *, u32>
find_compunit(Context<E> &ctx, ObjectFile<E> &file, i64 offset) {
  // Read .debug_info to find the record at a given offset.
  u8 *cu = get_buffer(ctx, ctx.debug_info) + offset;
  u32 dwarf_version = *(U16<E> *)(cu + 4);
  u32 abbrev_offset;

  // Skip a header.
  switch (dwarf_version) {
  case 2:
  case 3:
  case 4:
    abbrev_offset = *(U32<E> *)(cu + 6);
    if (u32 address_size = cu[10]; address_size != sizeof(Word<E>))
      Fatal(ctx) << file << ": --gdb-index: unsupported address size "
                 << address_size;
    cu += 11;
    break;
  case 5: {
    abbrev_offset = *(U32<E> *)(cu + 8);
    if (u32 address_size = cu[7]; address_size != sizeof(Word<E>))
      Fatal(ctx) << file << ": --gdb-index: unsupported address size "
                 << address_size;

    switch (u32 unit_type = cu[6]; unit_type) {
    case DW_UT_compile:
    case DW_UT_partial:
      cu += 12;
      break;
    case DW_UT_skeleton:
    case DW_UT_split_compile:
      cu += 20;
      break;
    default:
      Fatal(ctx) << file << ": --gdb-index: unknown DW_UT_* value: 0x"
                 << std::hex << unit_type;
    }
    break;
  }
  default:
    Fatal(ctx) << file << ": --gdb-index: unknown DWARF version: "
               << dwarf_version;
  }

  u32 abbrev_code = read_uleb(cu);

  // Find a .debug_abbrev record corresponding to the .debug_info record.
  // We assume the .debug_info record at a given offset is of
  // DW_TAG_compile_unit which describes a compunit.
  u8 *abbrev = get_buffer(ctx, ctx.debug_abbrev) + abbrev_offset;

  for (;;) {
    u32 code = read_uleb(abbrev);
    if (code == 0)
      Fatal(ctx) << file << ": --gdb-index: .debug_abbrev does not contain"
                 << " a record for the first .debug_info record";

    if (code == abbrev_code) {
      // Found a record
      u64 abbrev_tag = read_uleb(abbrev);
      if (abbrev_tag != DW_TAG_compile_unit && abbrev_tag != DW_TAG_skeleton_unit)
        Fatal(ctx) << file << ": --gdb-index: the first entry's tag is not"
                   << " DW_TAG_compile_unit/DW_TAG_skeleton_unit but 0x"
                   << std::hex << abbrev_tag;
      break;
    }

    // Skip an uninteresting record
    read_uleb(abbrev); // tag
    abbrev++; // has_children byte
    for (;;) {
      u64 name = read_uleb(abbrev);
      u64 form = read_uleb(abbrev);
      if (name == 0 && form == 0)
        break;
      if (form == DW_FORM_implicit_const)
        read_uleb(abbrev);
    }
  }

  abbrev++; // skip has_children byte
  return {cu, abbrev, dwarf_version};
}

// Estimate the number of address ranges contained in a given file.
// It may over-estimate but never under-estimate.
template <typename E>
i64 estimate_address_areas(Context<E> &ctx, ObjectFile<E> &file) {
  // Each CU contains zero or one address area.
  i64 ret = file.compunits.size();

  // In DWARF 4, a CU can refer address ranges in .debug_ranges.
  // .debug_ranges contains a vector of [begin, end) address pairs.
  // The last entry must be a null terminator, so we do -1.
  if (file.debug_ranges)
    ret += file.debug_ranges->sh_size / sizeof(Word<E>) / 2 - 1;

  // In DWARF 5, a CU can refer address ranges in .debug_rnglists, which
  // contains variable-length entries. The smallest possible range entry
  // is one byte for the code and two ULEB128 values (each can be as
  // small as one byte), so 3 bytes.
  if (file.debug_rnglists)
    ret += file.debug_rnglists->sh_size / 3;
  return ret;
}

// .debug_info contains variable-length fields. This class reads them.
template <typename E>
class DebugInfoReader {
public:
  DebugInfoReader(Context<E> &ctx, ObjectFile<E> &file, u8 *cu)
    : ctx(ctx), file(file), cu(cu) {}

  u64 read(u64 form);

  Context<E> &ctx;
  ObjectFile<E> &file;
  u8 *cu;
};

// Read value of the given DW_FORM_* form. If a value is not scalar,
// returns a dummy value 0.
template <typename E>
u64 DebugInfoReader<E>::read(u64 form) {
  switch (form) {
  case DW_FORM_flag_present:
    return 0;
  case DW_FORM_data1:
  case DW_FORM_flag:
  case DW_FORM_strx1:
  case DW_FORM_addrx1:
  case DW_FORM_ref1:
    return *cu++;
  case DW_FORM_data2:
  case DW_FORM_strx2:
  case DW_FORM_addrx2:
  case DW_FORM_ref2: {
    u64 val = *(U16<E> *)cu;
    cu += 2;
    return val;
  }
  case DW_FORM_strx3:
  case DW_FORM_addrx3: {
    u64 val = *(U24<E> *)cu;
    cu += 3;
    return val;
  }
  case DW_FORM_data4:
  case DW_FORM_strp:
  case DW_FORM_sec_offset:
  case DW_FORM_line_strp:
  case DW_FORM_strx4:
  case DW_FORM_addrx4:
  case DW_FORM_ref4: {
    u64 val = *(U32<E> *)cu;
    cu += 4;
    return val;
  }
  case DW_FORM_data8:
  case DW_FORM_ref8: {
    u64 val = *(U64<E> *)cu;
    cu += 8;
    return val;
  }
  case DW_FORM_addr:
  case DW_FORM_ref_addr: {
    u64 val = *(Word<E> *)cu;
    cu += sizeof(Word<E>);
    return val;
  }
  case DW_FORM_strx:
  case DW_FORM_addrx:
  case DW_FORM_udata:
  case DW_FORM_ref_udata:
  case DW_FORM_loclistx:
  case DW_FORM_rnglistx:
    return read_uleb(cu);
  case DW_FORM_string:
    cu += strlen((char *)cu) + 1;
    return 0;
  default:
    Fatal(ctx) << file << ": --gdb-index: unhandled debug info form: 0x"
               << std::hex << form;
    return 0;
  }
}

// Read a range list from .debug_ranges starting at the given offset.
template <typename E>
static std::vector<u64>
read_debug_range(Context<E> &ctx, ObjectFile<E> &file, Word<E> *range) {
  std::vector<u64> vec;
  u64 base = 0;

  for (i64 i = 0; range[i] || range[i + 1]; i += 2) {
    if (range[i] + 1 == 0) {
      // base address selection entry
      base = range[i + 1];
    } else {
      vec.push_back(range[i] + base);
      vec.push_back(range[i + 1] + base);
    }
  }
  return vec;
}

// Read a range list from .debug_rnglists starting at the given offset.
template <typename E>
static std::vector<u64>
read_rnglist_range(Context<E> &ctx, ObjectFile<E> &file, u8 *rnglist,
                   Word<E> *addrx) {
  std::vector<u64> vec;
  u64 base = 0;

  for (;;) {
    switch (*rnglist++) {
    case DW_RLE_end_of_list:
      return vec;
    case DW_RLE_base_addressx:
      base = addrx[read_uleb(rnglist)];
      break;
    case DW_RLE_startx_endx:
      vec.push_back(addrx[read_uleb(rnglist)]);
      vec.push_back(addrx[read_uleb(rnglist)]);
      break;
    case DW_RLE_startx_length:
      vec.push_back(addrx[read_uleb(rnglist)]);
      vec.push_back(vec.back() + read_uleb(rnglist));
      break;
    case DW_RLE_offset_pair:
      vec.push_back(base + read_uleb(rnglist));
      vec.push_back(base + read_uleb(rnglist));
      break;
    case DW_RLE_base_address:
      base = *(Word<E> *)rnglist;
      rnglist += sizeof(Word<E>);
      break;
    case DW_RLE_start_end:
      vec.push_back(*(Word<E> *)rnglist);
      rnglist += sizeof(Word<E>);
      vec.push_back(*(Word<E> *)rnglist);
      rnglist += sizeof(Word<E>);
      break;
    case DW_RLE_start_length:
      vec.push_back(*(Word<E> *)rnglist);
      rnglist += sizeof(Word<E>);
      vec.push_back(vec.back() + read_uleb(rnglist));
      break;
    }
  }
}

// Returns a list of address ranges explained by a compunit at the
// `offset` in an output .debug_info section.
//
// .debug_info contains DWARF debug info records, so this function
// parses DWARF. If a designated compunit contains multiple ranges, the
// ranges are read from .debug_ranges (or .debug_rnglists for DWARF5).
// Otherwise, a range is read directly from .debug_info (or possibly
// from .debug_addr for DWARF5).
template <typename E>
std::vector<u64>
read_address_areas(Context<E> &ctx, ObjectFile<E> &file, i64 offset) {
  u8 *cu;
  u8 *abbrev;
  u32 dwarf_version;
  std::tie(cu, abbrev, dwarf_version) = find_compunit(ctx, file, offset);

  DebugInfoReader<E> reader{ctx, file, cu};

  struct Record {
    u64 form = 0;
    u64 value = 0;
  };

  Record low_pc;
  Record high_pc;
  Record ranges;
  std::optional<u64> rnglists_base;
  Word<E> *addrx = nullptr;

  // Read all interesting debug records.
  for (;;) {
    u64 name = read_uleb(abbrev);
    u64 form = read_uleb(abbrev);
    if (name == 0 && form == 0)
      break;

    u64 val = reader.read(form);

    switch (name) {
    case DW_AT_low_pc:
      low_pc = {form, val};
      break;
    case DW_AT_high_pc:
      high_pc = {form, val};
      break;
    case DW_AT_rnglists_base:
      rnglists_base = val;
      break;
    case DW_AT_addr_base:
      addrx = (Word<E> *)(get_buffer(ctx, ctx.debug_addr) + val);
      break;
    case DW_AT_ranges:
      ranges = {form, val};
      break;
    }
  }

  // Handle non-contiguous address ranges.
  if (ranges.form) {
    if (dwarf_version <= 4) {
       Word<E> *range_begin =
        (Word<E> *)(get_buffer(ctx, ctx.debug_ranges) + ranges.value);
      return read_debug_range(ctx, file, range_begin);
    }

    assert(dwarf_version == 5);

    u8 *buf = get_buffer(ctx, ctx.debug_rnglists);
    if (ranges.form == DW_FORM_sec_offset)
      return read_rnglist_range(ctx, file, buf + ranges.value, addrx);

    if (!rnglists_base)
      Fatal(ctx) << file << ": --gdb-index: missing DW_AT_rnglists_base";

    u8 *base = buf + *rnglists_base;
    return read_rnglist_range(ctx, file, base + *(U32<E> *)base, addrx);
  }

  // Handle a contiguous address range.
  if (low_pc.form && high_pc.form) {
    u64 lo;

    switch (low_pc.form) {
    case DW_FORM_addr:
      lo = low_pc.value;
      break;
    case DW_FORM_addrx:
    case DW_FORM_addrx1:
    case DW_FORM_addrx2:
    case DW_FORM_addrx4:
      lo = addrx[low_pc.value];
      break;
    default:
      Fatal(ctx) << file << ": --gdb-index: unhandled form for DW_AT_low_pc: 0x"
                 << std::hex << high_pc.form;
    }

    switch (high_pc.form) {
    case DW_FORM_addr:
      return {lo, high_pc.value};
    case DW_FORM_addrx:
    case DW_FORM_addrx1:
    case DW_FORM_addrx2:
    case DW_FORM_addrx4:
      return {lo, addrx[high_pc.value]};
    case DW_FORM_udata:
    case DW_FORM_data1:
    case DW_FORM_data2:
    case DW_FORM_data4:
    case DW_FORM_data8:
      return {lo, lo + high_pc.value};
    default:
      Fatal(ctx) << file << ": --gdb-index: unhandled form for DW_AT_high_pc: 0x"
                 << std::hex << high_pc.form;
    }
  }

  return {};
}

using E = MOLD_TARGET;

template std::vector<std::string_view> read_compunits(Context<E> &, ObjectFile<E> &);
template std::vector<GdbIndexName> read_pubnames(Context<E> &, ObjectFile<E> &);
template i64 estimate_address_areas(Context<E> &, ObjectFile<E> &);
template std::vector<u64> read_address_areas(Context<E> &, ObjectFile<E> &, i64);

} // namespace mold::elf
