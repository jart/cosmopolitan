// clang-format off
#pragma once

#include "third_party/mold/integers.h"

#include "third_party/libcxx/ostream"
#include "third_party/libcxx/string"
#include "third_party/libcxx/type_traits"

namespace mold::elf {

struct X86_64;
struct I386;
struct ARM64;
struct ARM32;
struct RV64LE;
struct RV64BE;
struct RV32LE;
struct RV32BE;
struct PPC32;
struct PPC64V1;
struct PPC64V2;
struct S390X;
struct SPARC64;
struct M68K;
struct SH4;
struct ALPHA;

template <typename E> struct ElfSym;
template <typename E> struct ElfShdr;
template <typename E> struct ElfEhdr;
template <typename E> struct ElfPhdr;
template <typename E> struct ElfRel;
template <typename E> struct ElfDyn;
template <typename E> struct ElfVerneed;
template <typename E> struct ElfVernaux;
template <typename E> struct ElfVerdef;
template <typename E> struct ElfVerdaux;
template <typename E> struct ElfChdr;
template <typename E> struct ElfNhdr;

template <typename E>
std::string rel_to_string(u32 r_type);

template <typename E>
std::ostream &operator<<(std::ostream &out, const ElfRel<E> &rel) {
  out << rel_to_string<E>(rel.r_type);
  return out;
}

enum : u32 {
  R_NONE = 0,
};

enum : u32 {
  SHN_UNDEF = 0,
  SHN_ABS = 0xfff1,
  SHN_COMMON = 0xfff2,
  SHN_LORESERVE = 0xff00,
  SHN_XINDEX = 0xffff,
};

enum : u32 {
  SHT_NULL = 0,
  SHT_PROGBITS = 1,
  SHT_SYMTAB = 2,
  SHT_STRTAB = 3,
  SHT_RELA = 4,
  SHT_HASH = 5,
  SHT_DYNAMIC = 6,
  SHT_NOTE = 7,
  SHT_NOBITS = 8,
  SHT_REL = 9,
  SHT_SHLIB = 10,
  SHT_DYNSYM = 11,
  SHT_INIT_ARRAY = 14,
  SHT_FINI_ARRAY = 15,
  SHT_PREINIT_ARRAY = 16,
  SHT_GROUP = 17,
  SHT_SYMTAB_SHNDX = 18,
  SHT_RELR = 19,
  SHT_LLVM_ADDRSIG = 0x6fff4c03,
  SHT_GNU_HASH = 0x6ffffff6,
  SHT_GNU_VERDEF = 0x6ffffffd,
  SHT_GNU_VERNEED = 0x6ffffffe,
  SHT_GNU_VERSYM = 0x6fffffff,
  SHT_X86_64_UNWIND = 0x70000001,
  SHT_ARM_EXIDX = 0x70000001,
  SHT_ARM_ATTRIBUTES = 0x70000003,
};

enum : u32 {
  SHF_WRITE = 0x1,
  SHF_ALLOC = 0x2,
  SHF_EXECINSTR = 0x4,
  SHF_MERGE = 0x10,
  SHF_STRINGS = 0x20,
  SHF_INFO_LINK = 0x40,
  SHF_LINK_ORDER = 0x80,
  SHF_GROUP = 0x200,
  SHF_TLS = 0x400,
  SHF_COMPRESSED = 0x800,
  SHF_GNU_RETAIN = 0x200000,
  SHF_EXCLUDE = 0x80000000,
};

enum : u32 {
  GRP_COMDAT = 1,
};

enum : u32 {
  STT_NOTYPE = 0,
  STT_OBJECT = 1,
  STT_FUNC = 2,
  STT_SECTION = 3,
  STT_FILE = 4,
  STT_COMMON = 5,
  STT_TLS = 6,
  STT_GNU_IFUNC = 10,
  STT_SPARC_REGISTER = 13,
};

template <typename E>
inline std::string stt_to_string(u32 st_type) {
  switch (st_type) {
  case STT_NOTYPE: return "STT_NOTYPE";
  case STT_OBJECT: return "STT_OBJECT";
  case STT_FUNC: return "STT_FUNC";
  case STT_SECTION: return "STT_SECTION";
  case STT_FILE: return "STT_FILE";
  case STT_COMMON: return "STT_COMMON";
  case STT_TLS: return "STT_TLS";
  case STT_GNU_IFUNC: return "STT_GNU_IFUNC";
  }

  if constexpr (std::is_same_v<E, SPARC64>)
    if (st_type == STT_SPARC_REGISTER)
      return "STT_SPARC_REGISTER";

  return "unknown st_type (" + std::to_string(st_type) + ")";
}

enum : u32 {
  STB_LOCAL = 0,
  STB_GLOBAL = 1,
  STB_WEAK = 2,
  STB_GNU_UNIQUE = 10,
};

enum : u32 {
  STV_DEFAULT = 0,
  STV_INTERNAL = 1,
  STV_HIDDEN = 2,
  STV_PROTECTED = 3,
};

enum : u32 {
  VER_NDX_LOCAL = 0,
  VER_NDX_GLOBAL = 1,
  VER_NDX_LAST_RESERVED = 1,
};

enum : u32 {
  VER_FLG_BASE = 1,
  VER_FLG_WEAK = 2,
  VER_FLG_INFO = 4,
};

enum : u32 {
  VERSYM_HIDDEN = 0x8000,
};

enum : u32 {
  PT_NULL = 0,
  PT_LOAD = 1,
  PT_DYNAMIC = 2,
  PT_INTERP = 3,
  PT_NOTE = 4,
  PT_SHLIB = 5,
  PT_PHDR = 6,
  PT_TLS = 7,
  PT_GNU_EH_FRAME = 0x6474e550,
  PT_GNU_STACK = 0x6474e551,
  PT_GNU_RELRO = 0x6474e552,
  PT_OPENBSD_RANDOMIZE = 0x65a3dbe6,
  PT_ARM_EXIDX = 0x70000001,
};

enum : u32 {
  PF_NONE = 0,
  PF_X = 1,
  PF_W = 2,
  PF_R = 4,
};

enum : u32 {
  ET_NONE = 0,
  ET_REL = 1,
  ET_EXEC = 2,
  ET_DYN = 3,
};

enum : u32 {
  ELFDATA2LSB = 1,
  ELFDATA2MSB = 2,
};

enum : u32 {
  ELFCLASS32 = 1,
  ELFCLASS64 = 2,
};

enum : u32 {
  EV_CURRENT = 1,
};

enum : u32 {
  EM_NONE = 0,
  EM_386 = 3,
  EM_68K = 4,
  EM_PPC = 20,
  EM_PPC64 = 21,
  EM_S390X = 22,
  EM_ARM = 40,
  EM_SH = 42,
  EM_SPARC64 = 43,
  EM_X86_64 = 62,
  EM_AARCH64 = 183,
  EM_RISCV = 243,
  EM_ALPHA = 0x9026,
};

enum : u32 {
  EI_CLASS = 4,
  EI_DATA = 5,
  EI_VERSION = 6,
  EI_OSABI = 7,
  EI_ABIVERSION = 8,
};

enum : u32 {
  DT_NULL = 0,
  DT_NEEDED = 1,
  DT_PLTRELSZ = 2,
  DT_PLTGOT = 3,
  DT_HASH = 4,
  DT_STRTAB = 5,
  DT_SYMTAB = 6,
  DT_RELA = 7,
  DT_RELASZ = 8,
  DT_RELAENT = 9,
  DT_STRSZ = 10,
  DT_SYMENT = 11,
  DT_INIT = 12,
  DT_FINI = 13,
  DT_SONAME = 14,
  DT_RPATH = 15,
  DT_SYMBOLIC = 16,
  DT_REL = 17,
  DT_RELSZ = 18,
  DT_RELENT = 19,
  DT_PLTREL = 20,
  DT_DEBUG = 21,
  DT_TEXTREL = 22,
  DT_JMPREL = 23,
  DT_BIND_NOW = 24,
  DT_INIT_ARRAY = 25,
  DT_FINI_ARRAY = 26,
  DT_INIT_ARRAYSZ = 27,
  DT_FINI_ARRAYSZ = 28,
  DT_RUNPATH = 29,
  DT_FLAGS = 30,
  DT_PREINIT_ARRAY = 32,
  DT_PREINIT_ARRAYSZ = 33,
  DT_RELRSZ = 35,
  DT_RELR = 36,
  DT_RELRENT = 37,
  DT_GNU_HASH = 0x6ffffef5,
  DT_VERSYM = 0x6ffffff0,
  DT_RELACOUNT = 0x6ffffff9,
  DT_RELCOUNT = 0x6ffffffa,
  DT_FLAGS_1 = 0x6ffffffb,
  DT_VERDEF = 0x6ffffffc,
  DT_VERDEFNUM = 0x6ffffffd,
  DT_VERNEED = 0x6ffffffe,
  DT_VERNEEDNUM = 0x6fffffff,
  DT_PPC_GOT = 0x70000000,
  DT_PPC64_GLINK = 0x70000000,
  DT_AUXILIARY = 0x7ffffffd,
  DT_FILTER = 0x7fffffff,
};

enum : u32 {
  DF_ORIGIN = 0x01,
  DF_SYMBOLIC = 0x02,
  DF_TEXTREL = 0x04,
  DF_BIND_NOW = 0x08,
  DF_STATIC_TLS = 0x10,
};

enum : u32 {
  DF_1_NOW = 0x00000001,
  DF_1_NODELETE = 0x00000008,
  DF_1_INITFIRST = 0x00000020,
  DF_1_NOOPEN = 0x00000040,
  DF_1_ORIGIN = 0x00000080,
  DF_1_INTERPOSE = 0x00000400,
  DF_1_NODEFLIB = 0x00000800,
  DF_1_NODUMP = 0x00001000,
  DF_1_PIE = 0x08000000,
};

enum : u32 {
  NT_GNU_ABI_TAG = 1,
  NT_GNU_HWCAP = 2,
  NT_GNU_BUILD_ID = 3,
  NT_GNU_GOLD_VERSION = 4,
  NT_GNU_PROPERTY_TYPE_0 = 5,
  NT_FDO_PACKAGING_METADATA = 0xcafe1a7e,
};

enum : u32 {
  GNU_PROPERTY_X86_UINT32_AND_LO = 0xc0000002,
  GNU_PROPERTY_X86_UINT32_AND_HI = 0xc0007fff,
  GNU_PROPERTY_X86_UINT32_OR_LO = 0xc0008000,
  GNU_PROPERTY_X86_UINT32_OR_HI = 0xc000ffff,
  GNU_PROPERTY_X86_UINT32_OR_AND_LO = 0xc0010000,
  GNU_PROPERTY_X86_UINT32_OR_AND_HI = 0xc0017fff,

  GNU_PROPERTY_X86_FEATURE_1_IBT = 1,
  GNU_PROPERTY_X86_FEATURE_1_SHSTK = 2,
  GNU_PROPERTY_X86_FEATURE_1_AND = 0xc0000002,
};

enum : u32 {
  ELFCOMPRESS_ZLIB = 1,
  ELFCOMPRESS_ZSTD = 2,
};

enum : u32 {
  EF_ARM_ABI_FLOAT_SOFT = 0x00000200,
  EF_ARM_ABI_FLOAT_HARD = 0x00000400,
  EF_ARM_EABI_VER5 = 0x05000000,
};

enum : u32 {
  EF_RISCV_RVC = 1,
  EF_RISCV_FLOAT_ABI = 6,
  EF_RISCV_FLOAT_ABI_SOFT = 0,
  EF_RISCV_FLOAT_ABI_SINGLE = 2,
  EF_RISCV_FLOAT_ABI_DOUBLE = 4,
  EF_RISCV_FLOAT_ABI_QUAD = 6,
  EF_RISCV_RVE = 8,
  EF_RISCV_TSO = 16,
};

enum : u32 {
  EF_SPARC64_MM = 0x3,
  EF_SPARC64_TSO = 0x0,
  EF_SPARC64_PSO = 0x1,
  EF_SPARC64_RMO = 0x2,
  EF_SPARC_EXT_MASK = 0xffff00,
  EF_SPARC_SUN_US1 = 0x000200,
  EF_SPARC_HAL_R1 = 0x000400,
  EF_SPARC_SUN_US3 = 0x000800,
};

enum : u32 {
  STO_RISCV_VARIANT_CC = 0x80,
  STO_ALPHA_NOPV = 0x20,
  STO_ALPHA_STD_GPLOAD = 0x22,
};

//
// Relocation types
//

enum : u32 {
  R_X86_64_NONE = 0,
  R_X86_64_64 = 1,
  R_X86_64_PC32 = 2,
  R_X86_64_GOT32 = 3,
  R_X86_64_PLT32 = 4,
  R_X86_64_COPY = 5,
  R_X86_64_GLOB_DAT = 6,
  R_X86_64_JUMP_SLOT = 7,
  R_X86_64_RELATIVE = 8,
  R_X86_64_GOTPCREL = 9,
  R_X86_64_32 = 10,
  R_X86_64_32S = 11,
  R_X86_64_16 = 12,
  R_X86_64_PC16 = 13,
  R_X86_64_8 = 14,
  R_X86_64_PC8 = 15,
  R_X86_64_DTPMOD64 = 16,
  R_X86_64_DTPOFF64 = 17,
  R_X86_64_TPOFF64 = 18,
  R_X86_64_TLSGD = 19,
  R_X86_64_TLSLD = 20,
  R_X86_64_DTPOFF32 = 21,
  R_X86_64_GOTTPOFF = 22,
  R_X86_64_TPOFF32 = 23,
  R_X86_64_PC64 = 24,
  R_X86_64_GOTOFF64 = 25,
  R_X86_64_GOTPC32 = 26,
  R_X86_64_GOT64 = 27,
  R_X86_64_GOTPCREL64 = 28,
  R_X86_64_GOTPC64 = 29,
  R_X86_64_GOTPLT64 = 30,
  R_X86_64_PLTOFF64 = 31,
  R_X86_64_SIZE32 = 32,
  R_X86_64_SIZE64 = 33,
  R_X86_64_GOTPC32_TLSDESC = 34,
  R_X86_64_TLSDESC_CALL = 35,
  R_X86_64_TLSDESC = 36,
  R_X86_64_IRELATIVE = 37,
  R_X86_64_GOTPCRELX = 41,
  R_X86_64_REX_GOTPCRELX = 42,
};

enum : u32 {
  R_386_NONE = 0,
  R_386_32 = 1,
  R_386_PC32 = 2,
  R_386_GOT32 = 3,
  R_386_PLT32 = 4,
  R_386_COPY = 5,
  R_386_GLOB_DAT = 6,
  R_386_JUMP_SLOT = 7,
  R_386_RELATIVE = 8,
  R_386_GOTOFF = 9,
  R_386_GOTPC = 10,
  R_386_32PLT = 11,
  R_386_TLS_TPOFF = 14,
  R_386_TLS_IE = 15,
  R_386_TLS_GOTIE = 16,
  R_386_TLS_LE = 17,
  R_386_TLS_GD = 18,
  R_386_TLS_LDM = 19,
  R_386_16 = 20,
  R_386_PC16 = 21,
  R_386_8 = 22,
  R_386_PC8 = 23,
  R_386_TLS_GD_32 = 24,
  R_386_TLS_GD_PUSH = 25,
  R_386_TLS_GD_CALL = 26,
  R_386_TLS_GD_POP = 27,
  R_386_TLS_LDM_32 = 28,
  R_386_TLS_LDM_PUSH = 29,
  R_386_TLS_LDM_CALL = 30,
  R_386_TLS_LDM_POP = 31,
  R_386_TLS_LDO_32 = 32,
  R_386_TLS_IE_32 = 33,
  R_386_TLS_LE_32 = 34,
  R_386_TLS_DTPMOD32 = 35,
  R_386_TLS_DTPOFF32 = 36,
  R_386_TLS_TPOFF32 = 37,
  R_386_SIZE32 = 38,
  R_386_TLS_GOTDESC = 39,
  R_386_TLS_DESC_CALL = 40,
  R_386_TLS_DESC = 41,
  R_386_IRELATIVE = 42,
  R_386_GOT32X = 43,
};

enum : u32 {
  R_AARCH64_NONE = 0,
  R_AARCH64_ABS64 = 0x101,
  R_AARCH64_ABS32 = 0x102,
  R_AARCH64_ABS16 = 0x103,
  R_AARCH64_PREL64 = 0x104,
  R_AARCH64_PREL32 = 0x105,
  R_AARCH64_PREL16 = 0x106,
  R_AARCH64_MOVW_UABS_G0 = 0x107,
  R_AARCH64_MOVW_UABS_G0_NC = 0x108,
  R_AARCH64_MOVW_UABS_G1 = 0x109,
  R_AARCH64_MOVW_UABS_G1_NC = 0x10a,
  R_AARCH64_MOVW_UABS_G2 = 0x10b,
  R_AARCH64_MOVW_UABS_G2_NC = 0x10c,
  R_AARCH64_MOVW_UABS_G3 = 0x10d,
  R_AARCH64_MOVW_SABS_G0 = 0x10e,
  R_AARCH64_MOVW_SABS_G1 = 0x10f,
  R_AARCH64_MOVW_SABS_G2 = 0x110,
  R_AARCH64_LD_PREL_LO19 = 0x111,
  R_AARCH64_ADR_PREL_LO21 = 0x112,
  R_AARCH64_ADR_PREL_PG_HI21 = 0x113,
  R_AARCH64_ADR_PREL_PG_HI21_NC = 0x114,
  R_AARCH64_ADD_ABS_LO12_NC = 0x115,
  R_AARCH64_LDST8_ABS_LO12_NC = 0x116,
  R_AARCH64_TSTBR14 = 0x117,
  R_AARCH64_CONDBR19 = 0x118,
  R_AARCH64_JUMP26 = 0x11a,
  R_AARCH64_CALL26 = 0x11b,
  R_AARCH64_LDST16_ABS_LO12_NC = 0x11c,
  R_AARCH64_LDST32_ABS_LO12_NC = 0x11d,
  R_AARCH64_LDST64_ABS_LO12_NC = 0x11e,
  R_AARCH64_MOVW_PREL_G0 = 0x11f,
  R_AARCH64_MOVW_PREL_G0_NC = 0x120,
  R_AARCH64_MOVW_PREL_G1 = 0x121,
  R_AARCH64_MOVW_PREL_G1_NC = 0x122,
  R_AARCH64_MOVW_PREL_G2 = 0x123,
  R_AARCH64_MOVW_PREL_G2_NC = 0x124,
  R_AARCH64_MOVW_PREL_G3 = 0x125,
  R_AARCH64_LDST128_ABS_LO12_NC = 0x12b,
  R_AARCH64_ADR_GOT_PAGE = 0x137,
  R_AARCH64_LD64_GOT_LO12_NC = 0x138,
  R_AARCH64_LD64_GOTPAGE_LO15 = 0x139,
  R_AARCH64_PLT32 = 0x13a,
  R_AARCH64_TLSGD_ADR_PREL21 = 0x200,
  R_AARCH64_TLSGD_ADR_PAGE21 = 0x201,
  R_AARCH64_TLSGD_ADD_LO12_NC = 0x202,
  R_AARCH64_TLSGD_MOVW_G1 = 0x203,
  R_AARCH64_TLSGD_MOVW_G0_NC = 0x204,
  R_AARCH64_TLSLD_ADR_PREL21 = 0x205,
  R_AARCH64_TLSLD_ADR_PAGE21 = 0x206,
  R_AARCH64_TLSLD_ADD_LO12_NC = 0x207,
  R_AARCH64_TLSLD_MOVW_G1 = 0x208,
  R_AARCH64_TLSLD_MOVW_G0_NC = 0x209,
  R_AARCH64_TLSLD_LD_PREL19 = 0x20a,
  R_AARCH64_TLSLD_MOVW_DTPREL_G2 = 0x20b,
  R_AARCH64_TLSLD_MOVW_DTPREL_G1 = 0x20c,
  R_AARCH64_TLSLD_MOVW_DTPREL_G1_NC = 0x20d,
  R_AARCH64_TLSLD_MOVW_DTPREL_G0 = 0x20e,
  R_AARCH64_TLSLD_MOVW_DTPREL_G0_NC = 0x20f,
  R_AARCH64_TLSLD_ADD_DTPREL_HI12 = 0x210,
  R_AARCH64_TLSLD_ADD_DTPREL_LO12 = 0x211,
  R_AARCH64_TLSLD_ADD_DTPREL_LO12_NC = 0x212,
  R_AARCH64_TLSLD_LDST8_DTPREL_LO12 = 0x213,
  R_AARCH64_TLSLD_LDST8_DTPREL_LO12_NC = 0x214,
  R_AARCH64_TLSLD_LDST16_DTPREL_LO12 = 0x215,
  R_AARCH64_TLSLD_LDST16_DTPREL_LO12_NC = 0x216,
  R_AARCH64_TLSLD_LDST32_DTPREL_LO12 = 0x217,
  R_AARCH64_TLSLD_LDST32_DTPREL_LO12_NC = 0x218,
  R_AARCH64_TLSLD_LDST64_DTPREL_LO12 = 0x219,
  R_AARCH64_TLSLD_LDST64_DTPREL_LO12_NC = 0x21a,
  R_AARCH64_TLSIE_MOVW_GOTTPREL_G1 = 0x21b,
  R_AARCH64_TLSIE_MOVW_GOTTPREL_G0_NC = 0x21c,
  R_AARCH64_TLSIE_ADR_GOTTPREL_PAGE21 = 0x21d,
  R_AARCH64_TLSIE_LD64_GOTTPREL_LO12_NC = 0x21e,
  R_AARCH64_TLSIE_LD_GOTTPREL_PREL19 = 0x21f,
  R_AARCH64_TLSLE_MOVW_TPREL_G2 = 0x220,
  R_AARCH64_TLSLE_MOVW_TPREL_G1 = 0x221,
  R_AARCH64_TLSLE_MOVW_TPREL_G1_NC = 0x222,
  R_AARCH64_TLSLE_MOVW_TPREL_G0 = 0x223,
  R_AARCH64_TLSLE_MOVW_TPREL_G0_NC = 0x224,
  R_AARCH64_TLSLE_ADD_TPREL_HI12 = 0x225,
  R_AARCH64_TLSLE_ADD_TPREL_LO12 = 0x226,
  R_AARCH64_TLSLE_ADD_TPREL_LO12_NC = 0x227,
  R_AARCH64_TLSLE_LDST8_TPREL_LO12 = 0x228,
  R_AARCH64_TLSLE_LDST8_TPREL_LO12_NC = 0x229,
  R_AARCH64_TLSLE_LDST16_TPREL_LO12 = 0x22a,
  R_AARCH64_TLSLE_LDST16_TPREL_LO12_NC = 0x22b,
  R_AARCH64_TLSLE_LDST32_TPREL_LO12 = 0x22c,
  R_AARCH64_TLSLE_LDST32_TPREL_LO12_NC = 0x22d,
  R_AARCH64_TLSLE_LDST64_TPREL_LO12 = 0x22e,
  R_AARCH64_TLSLE_LDST64_TPREL_LO12_NC = 0x22f,
  R_AARCH64_TLSDESC_ADR_PAGE21 = 0x232,
  R_AARCH64_TLSDESC_LD64_LO12 = 0x233,
  R_AARCH64_TLSDESC_ADD_LO12 = 0x234,
  R_AARCH64_TLSDESC_CALL = 0x239,
  R_AARCH64_TLSLE_LDST128_TPREL_LO12_NC = 0x23b,
  R_AARCH64_COPY = 0x400,
  R_AARCH64_GLOB_DAT = 0x401,
  R_AARCH64_JUMP_SLOT = 0x402,
  R_AARCH64_RELATIVE = 0x403,
  R_AARCH64_TLS_DTPMOD64 = 0x404,
  R_AARCH64_TLS_DTPREL64 = 0x405,
  R_AARCH64_TLS_TPREL64 = 0x406,
  R_AARCH64_TLSDESC = 0x407,
  R_AARCH64_IRELATIVE = 0x408,
};

enum : u32 {
  R_ARM_NONE = 0x0,
  R_ARM_PC24 = 0x1,
  R_ARM_ABS32 = 0x2,
  R_ARM_REL32 = 0x3,
  R_ARM_LDR_PC_G0 = 0x4,
  R_ARM_ABS16 = 0x5,
  R_ARM_ABS12 = 0x6,
  R_ARM_THM_ABS5 = 0x7,
  R_ARM_ABS8 = 0x8,
  R_ARM_SBREL32 = 0x9,
  R_ARM_THM_CALL = 0xa,
  R_ARM_THM_PC8 = 0xb,
  R_ARM_BREL_ADJ = 0xc,
  R_ARM_TLS_DESC = 0xd,
  R_ARM_THM_SWI8 = 0xe,
  R_ARM_XPC25 = 0xf,
  R_ARM_THM_XPC22 = 0x10,
  R_ARM_TLS_DTPMOD32 = 0x11,
  R_ARM_TLS_DTPOFF32 = 0x12,
  R_ARM_TLS_TPOFF32 = 0x13,
  R_ARM_COPY = 0x14,
  R_ARM_GLOB_DAT = 0x15,
  R_ARM_JUMP_SLOT = 0x16,
  R_ARM_RELATIVE = 0x17,
  R_ARM_GOTOFF32 = 0x18,
  R_ARM_BASE_PREL = 0x19,
  R_ARM_GOT_BREL = 0x1a,
  R_ARM_PLT32 = 0x1b,
  R_ARM_CALL = 0x1c,
  R_ARM_JUMP24 = 0x1d,
  R_ARM_THM_JUMP24 = 0x1e,
  R_ARM_BASE_ABS = 0x1f,
  R_ARM_ALU_PCREL_7_0 = 0x20,
  R_ARM_ALU_PCREL_15_8 = 0x21,
  R_ARM_ALU_PCREL_23_15 = 0x22,
  R_ARM_LDR_SBREL_11_0_NC = 0x23,
  R_ARM_ALU_SBREL_19_12_NC = 0x24,
  R_ARM_ALU_SBREL_27_20_CK = 0x25,
  R_ARM_TARGET1 = 0x26,
  R_ARM_SBREL31 = 0x27,
  R_ARM_V4BX = 0x28,
  R_ARM_TARGET2 = 0x29,
  R_ARM_PREL31 = 0x2a,
  R_ARM_MOVW_ABS_NC = 0x2b,
  R_ARM_MOVT_ABS = 0x2c,
  R_ARM_MOVW_PREL_NC = 0x2d,
  R_ARM_MOVT_PREL = 0x2e,
  R_ARM_THM_MOVW_ABS_NC = 0x2f,
  R_ARM_THM_MOVT_ABS = 0x30,
  R_ARM_THM_MOVW_PREL_NC = 0x31,
  R_ARM_THM_MOVT_PREL = 0x32,
  R_ARM_THM_JUMP19 = 0x33,
  R_ARM_THM_JUMP6 = 0x34,
  R_ARM_THM_ALU_PREL_11_0 = 0x35,
  R_ARM_THM_PC12 = 0x36,
  R_ARM_ABS32_NOI = 0x37,
  R_ARM_REL32_NOI = 0x38,
  R_ARM_ALU_PC_G0_NC = 0x39,
  R_ARM_ALU_PC_G0 = 0x3a,
  R_ARM_ALU_PC_G1_NC = 0x3b,
  R_ARM_ALU_PC_G1 = 0x3c,
  R_ARM_ALU_PC_G2 = 0x3d,
  R_ARM_LDR_PC_G1 = 0x3e,
  R_ARM_LDR_PC_G2 = 0x3f,
  R_ARM_LDRS_PC_G0 = 0x40,
  R_ARM_LDRS_PC_G1 = 0x41,
  R_ARM_LDRS_PC_G2 = 0x42,
  R_ARM_LDC_PC_G0 = 0x43,
  R_ARM_LDC_PC_G1 = 0x44,
  R_ARM_LDC_PC_G2 = 0x45,
  R_ARM_ALU_SB_G0_NC = 0x46,
  R_ARM_ALU_SB_G0 = 0x47,
  R_ARM_ALU_SB_G1_NC = 0x48,
  R_ARM_ALU_SB_G1 = 0x49,
  R_ARM_ALU_SB_G2 = 0x4a,
  R_ARM_LDR_SB_G0 = 0x4b,
  R_ARM_LDR_SB_G1 = 0x4c,
  R_ARM_LDR_SB_G2 = 0x4d,
  R_ARM_LDRS_SB_G0 = 0x4e,
  R_ARM_LDRS_SB_G1 = 0x4f,
  R_ARM_LDRS_SB_G2 = 0x50,
  R_ARM_LDC_SB_G0 = 0x51,
  R_ARM_LDC_SB_G1 = 0x52,
  R_ARM_LDC_SB_G2 = 0x53,
  R_ARM_MOVW_BREL_NC = 0x54,
  R_ARM_MOVT_BREL = 0x55,
  R_ARM_MOVW_BREL = 0x56,
  R_ARM_THM_MOVW_BREL_NC = 0x57,
  R_ARM_THM_MOVT_BREL = 0x58,
  R_ARM_THM_MOVW_BREL = 0x59,
  R_ARM_TLS_GOTDESC = 0x5a,
  R_ARM_TLS_CALL = 0x5b,
  R_ARM_TLS_DESCSEQ = 0x5c,
  R_ARM_THM_TLS_CALL = 0x5d,
  R_ARM_PLT32_ABS = 0x5e,
  R_ARM_GOT_ABS = 0x5f,
  R_ARM_GOT_PREL = 0x60,
  R_ARM_GOT_BREL12 = 0x61,
  R_ARM_GOTOFF12 = 0x62,
  R_ARM_GOTRELAX = 0x63,
  R_ARM_GNU_VTENTRY = 0x64,
  R_ARM_GNU_VTINHERIT = 0x65,
  R_ARM_THM_JUMP11 = 0x66,
  R_ARM_THM_JUMP8 = 0x67,
  R_ARM_TLS_GD32 = 0x68,
  R_ARM_TLS_LDM32 = 0x69,
  R_ARM_TLS_LDO32 = 0x6a,
  R_ARM_TLS_IE32 = 0x6b,
  R_ARM_TLS_LE32 = 0x6c,
  R_ARM_TLS_LDO12 = 0x6d,
  R_ARM_TLS_LE12 = 0x6e,
  R_ARM_TLS_IE12GP = 0x6f,
  R_ARM_PRIVATE_0 = 0x70,
  R_ARM_PRIVATE_1 = 0x71,
  R_ARM_PRIVATE_2 = 0x72,
  R_ARM_PRIVATE_3 = 0x73,
  R_ARM_PRIVATE_4 = 0x74,
  R_ARM_PRIVATE_5 = 0x75,
  R_ARM_PRIVATE_6 = 0x76,
  R_ARM_PRIVATE_7 = 0x77,
  R_ARM_PRIVATE_8 = 0x78,
  R_ARM_PRIVATE_9 = 0x79,
  R_ARM_PRIVATE_10 = 0x7a,
  R_ARM_PRIVATE_11 = 0x7b,
  R_ARM_PRIVATE_12 = 0x7c,
  R_ARM_PRIVATE_13 = 0x7d,
  R_ARM_PRIVATE_14 = 0x7e,
  R_ARM_PRIVATE_15 = 0x7f,
  R_ARM_ME_TOO = 0x80,
  R_ARM_THM_TLS_DESCSEQ16 = 0x81,
  R_ARM_THM_TLS_DESCSEQ32 = 0x82,
  R_ARM_THM_BF16 = 0x88,
  R_ARM_THM_BF12 = 0x89,
  R_ARM_THM_BF18 = 0x8a,
  R_ARM_IRELATIVE = 0xa0,
};

enum : u32 {
  R_RISCV_NONE = 0,
  R_RISCV_32 = 1,
  R_RISCV_64 = 2,
  R_RISCV_RELATIVE = 3,
  R_RISCV_COPY = 4,
  R_RISCV_JUMP_SLOT = 5,
  R_RISCV_TLS_DTPMOD32 = 6,
  R_RISCV_TLS_DTPMOD64 = 7,
  R_RISCV_TLS_DTPREL32 = 8,
  R_RISCV_TLS_DTPREL64 = 9,
  R_RISCV_TLS_TPREL32 = 10,
  R_RISCV_TLS_TPREL64 = 11,
  R_RISCV_BRANCH = 16,
  R_RISCV_JAL = 17,
  R_RISCV_CALL = 18,
  R_RISCV_CALL_PLT = 19,
  R_RISCV_GOT_HI20 = 20,
  R_RISCV_TLS_GOT_HI20 = 21,
  R_RISCV_TLS_GD_HI20 = 22,
  R_RISCV_PCREL_HI20 = 23,
  R_RISCV_PCREL_LO12_I = 24,
  R_RISCV_PCREL_LO12_S = 25,
  R_RISCV_HI20 = 26,
  R_RISCV_LO12_I = 27,
  R_RISCV_LO12_S = 28,
  R_RISCV_TPREL_HI20 = 29,
  R_RISCV_TPREL_LO12_I = 30,
  R_RISCV_TPREL_LO12_S = 31,
  R_RISCV_TPREL_ADD = 32,
  R_RISCV_ADD8 = 33,
  R_RISCV_ADD16 = 34,
  R_RISCV_ADD32 = 35,
  R_RISCV_ADD64 = 36,
  R_RISCV_SUB8 = 37,
  R_RISCV_SUB16 = 38,
  R_RISCV_SUB32 = 39,
  R_RISCV_SUB64 = 40,
  R_RISCV_ALIGN = 43,
  R_RISCV_RVC_BRANCH = 44,
  R_RISCV_RVC_JUMP = 45,
  R_RISCV_RVC_LUI = 46,
  R_RISCV_RELAX = 51,
  R_RISCV_SUB6 = 52,
  R_RISCV_SET6 = 53,
  R_RISCV_SET8 = 54,
  R_RISCV_SET16 = 55,
  R_RISCV_SET32 = 56,
  R_RISCV_32_PCREL = 57,
  R_RISCV_IRELATIVE = 58,
  R_RISCV_PLT32 = 59,
  R_RISCV_SET_ULEB128 = 60,
  R_RISCV_SUB_ULEB128 = 61,
};

enum : u32 {
  R_PPC_NONE = 0,
  R_PPC_ADDR32 = 1,
  R_PPC_ADDR24 = 2,
  R_PPC_ADDR16 = 3,
  R_PPC_ADDR16_LO = 4,
  R_PPC_ADDR16_HI = 5,
  R_PPC_ADDR16_HA = 6,
  R_PPC_ADDR14 = 7,
  R_PPC_ADDR14_BRTAKEN = 8,
  R_PPC_ADDR14_BRNTAKEN = 9,
  R_PPC_REL24 = 10,
  R_PPC_REL14 = 11,
  R_PPC_REL14_BRTAKEN = 12,
  R_PPC_REL14_BRNTAKEN = 13,
  R_PPC_GOT16 = 14,
  R_PPC_GOT16_LO = 15,
  R_PPC_GOT16_HI = 16,
  R_PPC_GOT16_HA = 17,
  R_PPC_PLTREL24 = 18,
  R_PPC_COPY = 19,
  R_PPC_GLOB_DAT = 20,
  R_PPC_JMP_SLOT = 21,
  R_PPC_RELATIVE = 22,
  R_PPC_LOCAL24PC = 23,
  R_PPC_UADDR32 = 24,
  R_PPC_UADDR16 = 25,
  R_PPC_REL32 = 26,
  R_PPC_PLT32 = 27,
  R_PPC_PLTREL32 = 28,
  R_PPC_PLT16_LO = 29,
  R_PPC_PLT16_HI = 30,
  R_PPC_PLT16_HA = 31,
  R_PPC_SDAREL16 = 32,
  R_PPC_SECTOFF = 33,
  R_PPC_SECTOFF_LO = 34,
  R_PPC_SECTOFF_HI = 35,
  R_PPC_SECTOFF_HA = 36,
  R_PPC_ADDR30 = 37,
  R_PPC_TLS = 67,
  R_PPC_DTPMOD32 = 68,
  R_PPC_TPREL16 = 69,
  R_PPC_TPREL16_LO = 70,
  R_PPC_TPREL16_HI = 71,
  R_PPC_TPREL16_HA = 72,
  R_PPC_TPREL32 = 73,
  R_PPC_DTPREL16 = 74,
  R_PPC_DTPREL16_LO = 75,
  R_PPC_DTPREL16_HI = 76,
  R_PPC_DTPREL16_HA = 77,
  R_PPC_DTPREL32 = 78,
  R_PPC_GOT_TLSGD16 = 79,
  R_PPC_GOT_TLSGD16_LO = 80,
  R_PPC_GOT_TLSGD16_HI = 81,
  R_PPC_GOT_TLSGD16_HA = 82,
  R_PPC_GOT_TLSLD16 = 83,
  R_PPC_GOT_TLSLD16_LO = 84,
  R_PPC_GOT_TLSLD16_HI = 85,
  R_PPC_GOT_TLSLD16_HA = 86,
  R_PPC_GOT_TPREL16 = 87,
  R_PPC_GOT_TPREL16_LO = 88,
  R_PPC_GOT_TPREL16_HI = 89,
  R_PPC_GOT_TPREL16_HA = 90,
  R_PPC_GOT_DTPREL16 = 91,
  R_PPC_GOT_DTPREL16_LO = 92,
  R_PPC_GOT_DTPREL16_HI = 93,
  R_PPC_GOT_DTPREL16_HA = 94,
  R_PPC_TLSGD = 95,
  R_PPC_TLSLD = 96,
  R_PPC_PLTSEQ = 119,
  R_PPC_PLTCALL = 120,
  R_PPC_IRELATIVE = 248,
  R_PPC_REL16 = 249,
  R_PPC_REL16_LO = 250,
  R_PPC_REL16_HI = 251,
  R_PPC_REL16_HA = 252,
};

enum : u32 {
  R_PPC64_NONE = 0,
  R_PPC64_ADDR32 = 1,
  R_PPC64_ADDR24 = 2,
  R_PPC64_ADDR16 = 3,
  R_PPC64_ADDR16_LO = 4,
  R_PPC64_ADDR16_HI = 5,
  R_PPC64_ADDR16_HA = 6,
  R_PPC64_ADDR14 = 7,
  R_PPC64_ADDR14_BRTAKEN = 8,
  R_PPC64_ADDR14_BRNTAKEN = 9,
  R_PPC64_REL24 = 10,
  R_PPC64_REL14 = 11,
  R_PPC64_REL14_BRTAKEN = 12,
  R_PPC64_REL14_BRNTAKEN = 13,
  R_PPC64_GOT16 = 14,
  R_PPC64_GOT16_LO = 15,
  R_PPC64_GOT16_HI = 16,
  R_PPC64_GOT16_HA = 17,
  R_PPC64_COPY = 19,
  R_PPC64_GLOB_DAT = 20,
  R_PPC64_JMP_SLOT = 21,
  R_PPC64_RELATIVE = 22,
  R_PPC64_REL32 = 26,
  R_PPC64_PLT16_LO = 29,
  R_PPC64_PLT16_HI = 30,
  R_PPC64_PLT16_HA = 31,
  R_PPC64_ADDR64 = 38,
  R_PPC64_ADDR16_HIGHER = 39,
  R_PPC64_ADDR16_HIGHERA = 40,
  R_PPC64_ADDR16_HIGHEST = 41,
  R_PPC64_ADDR16_HIGHESTA = 42,
  R_PPC64_REL64 = 44,
  R_PPC64_TOC16 = 47,
  R_PPC64_TOC16_LO = 48,
  R_PPC64_TOC16_HI = 49,
  R_PPC64_TOC16_HA = 50,
  R_PPC64_TOC = 51,
  R_PPC64_ADDR16_DS = 56,
  R_PPC64_ADDR16_LO_DS = 57,
  R_PPC64_GOT16_DS = 58,
  R_PPC64_GOT16_LO_DS = 59,
  R_PPC64_PLT16_LO_DS = 60,
  R_PPC64_TOC16_DS = 63,
  R_PPC64_TOC16_LO_DS = 64,
  R_PPC64_TLS = 67,
  R_PPC64_DTPMOD64 = 68,
  R_PPC64_TPREL16 = 69,
  R_PPC64_TPREL16_LO = 70,
  R_PPC64_TPREL16_HI = 71,
  R_PPC64_TPREL16_HA = 72,
  R_PPC64_TPREL64 = 73,
  R_PPC64_DTPREL16 = 74,
  R_PPC64_DTPREL16_LO = 75,
  R_PPC64_DTPREL16_HI = 76,
  R_PPC64_DTPREL16_HA = 77,
  R_PPC64_DTPREL64 = 78,
  R_PPC64_GOT_TLSGD16 = 79,
  R_PPC64_GOT_TLSGD16_LO = 80,
  R_PPC64_GOT_TLSGD16_HI = 81,
  R_PPC64_GOT_TLSGD16_HA = 82,
  R_PPC64_GOT_TLSLD16 = 83,
  R_PPC64_GOT_TLSLD16_LO = 84,
  R_PPC64_GOT_TLSLD16_HI = 85,
  R_PPC64_GOT_TLSLD16_HA = 86,
  R_PPC64_GOT_TPREL16_DS = 87,
  R_PPC64_GOT_TPREL16_LO_DS = 88,
  R_PPC64_GOT_TPREL16_HI = 89,
  R_PPC64_GOT_TPREL16_HA = 90,
  R_PPC64_GOT_DTPREL16_DS = 91,
  R_PPC64_GOT_DTPREL16_LO_DS = 92,
  R_PPC64_GOT_DTPREL16_HI = 93,
  R_PPC64_GOT_DTPREL16_HA = 94,
  R_PPC64_TPREL16_DS = 95,
  R_PPC64_TPREL16_LO_DS = 96,
  R_PPC64_TPREL16_HIGHER = 97,
  R_PPC64_TPREL16_HIGHERA = 98,
  R_PPC64_TPREL16_HIGHEST = 99,
  R_PPC64_TPREL16_HIGHESTA = 100,
  R_PPC64_DTPREL16_DS = 101,
  R_PPC64_DTPREL16_LO_DS = 102,
  R_PPC64_DTPREL16_HIGHER = 103,
  R_PPC64_DTPREL16_HIGHERA = 104,
  R_PPC64_DTPREL16_HIGHEST = 105,
  R_PPC64_DTPREL16_HIGHESTA = 106,
  R_PPC64_TLSGD = 107,
  R_PPC64_TLSLD = 108,
  R_PPC64_ADDR16_HIGH = 110,
  R_PPC64_ADDR16_HIGHA = 111,
  R_PPC64_TPREL16_HIGH = 112,
  R_PPC64_TPREL16_HIGHA = 113,
  R_PPC64_DTPREL16_HIGH = 114,
  R_PPC64_DTPREL16_HIGHA = 115,
  R_PPC64_REL24_NOTOC = 116,
  R_PPC64_PLTSEQ = 119,
  R_PPC64_PLTCALL = 120,
  R_PPC64_PLTSEQ_NOTOC = 121,
  R_PPC64_PLTCALL_NOTOC = 122,
  R_PPC64_PCREL_OPT = 123,
  R_PPC64_PCREL34 = 132,
  R_PPC64_GOT_PCREL34 = 133,
  R_PPC64_PLT_PCREL34 = 134,
  R_PPC64_PLT_PCREL34_NOTOC = 135,
  R_PPC64_TPREL34 = 146,
  R_PPC64_DTPREL34 = 147,
  R_PPC64_GOT_TLSGD_PCREL34 = 148,
  R_PPC64_GOT_TLSLD_PCREL34 = 149,
  R_PPC64_GOT_TPREL_PCREL34 = 150,
  R_PPC64_IRELATIVE = 248,
  R_PPC64_REL16 = 249,
  R_PPC64_REL16_LO = 250,
  R_PPC64_REL16_HI = 251,
  R_PPC64_REL16_HA = 252,
};

enum : u32 {
  R_SPARC_NONE = 0,
  R_SPARC_8 = 1,
  R_SPARC_16 = 2,
  R_SPARC_32 = 3,
  R_SPARC_DISP8 = 4,
  R_SPARC_DISP16 = 5,
  R_SPARC_DISP32 = 6,
  R_SPARC_WDISP30 = 7,
  R_SPARC_WDISP22 = 8,
  R_SPARC_HI22 = 9,
  R_SPARC_22 = 10,
  R_SPARC_13 = 11,
  R_SPARC_LO10 = 12,
  R_SPARC_GOT10 = 13,
  R_SPARC_GOT13 = 14,
  R_SPARC_GOT22 = 15,
  R_SPARC_PC10 = 16,
  R_SPARC_PC22 = 17,
  R_SPARC_WPLT30 = 18,
  R_SPARC_COPY = 19,
  R_SPARC_GLOB_DAT = 20,
  R_SPARC_JMP_SLOT = 21,
  R_SPARC_RELATIVE = 22,
  R_SPARC_UA32 = 23,
  R_SPARC_PLT32 = 24,
  R_SPARC_HIPLT22 = 25,
  R_SPARC_LOPLT10 = 26,
  R_SPARC_PCPLT32 = 27,
  R_SPARC_PCPLT22 = 28,
  R_SPARC_PCPLT10 = 29,
  R_SPARC_10 = 30,
  R_SPARC_11 = 31,
  R_SPARC_64 = 32,
  R_SPARC_OLO10 = 33,
  R_SPARC_HH22 = 34,
  R_SPARC_HM10 = 35,
  R_SPARC_LM22 = 36,
  R_SPARC_PC_HH22 = 37,
  R_SPARC_PC_HM10 = 38,
  R_SPARC_PC_LM22 = 39,
  R_SPARC_WDISP16 = 40,
  R_SPARC_WDISP19 = 41,
  R_SPARC_7 = 43,
  R_SPARC_5 = 44,
  R_SPARC_6 = 45,
  R_SPARC_DISP64 = 46,
  R_SPARC_PLT64 = 47,
  R_SPARC_HIX22 = 48,
  R_SPARC_LOX10 = 49,
  R_SPARC_H44 = 50,
  R_SPARC_M44 = 51,
  R_SPARC_L44 = 52,
  R_SPARC_REGISTER = 53,
  R_SPARC_UA64 = 54,
  R_SPARC_UA16 = 55,
  R_SPARC_TLS_GD_HI22 = 56,
  R_SPARC_TLS_GD_LO10 = 57,
  R_SPARC_TLS_GD_ADD = 58,
  R_SPARC_TLS_GD_CALL = 59,
  R_SPARC_TLS_LDM_HI22 = 60,
  R_SPARC_TLS_LDM_LO10 = 61,
  R_SPARC_TLS_LDM_ADD = 62,
  R_SPARC_TLS_LDM_CALL = 63,
  R_SPARC_TLS_LDO_HIX22 = 64,
  R_SPARC_TLS_LDO_LOX10 = 65,
  R_SPARC_TLS_LDO_ADD = 66,
  R_SPARC_TLS_IE_HI22 = 67,
  R_SPARC_TLS_IE_LO10 = 68,
  R_SPARC_TLS_IE_LD = 69,
  R_SPARC_TLS_IE_LDX = 70,
  R_SPARC_TLS_IE_ADD = 71,
  R_SPARC_TLS_LE_HIX22 = 72,
  R_SPARC_TLS_LE_LOX10 = 73,
  R_SPARC_TLS_DTPMOD32 = 74,
  R_SPARC_TLS_DTPMOD64 = 75,
  R_SPARC_TLS_DTPOFF32 = 76,
  R_SPARC_TLS_DTPOFF64 = 77,
  R_SPARC_TLS_TPOFF32 = 78,
  R_SPARC_TLS_TPOFF64 = 79,
  R_SPARC_GOTDATA_HIX22 = 80,
  R_SPARC_GOTDATA_LOX10 = 81,
  R_SPARC_GOTDATA_OP_HIX22 = 82,
  R_SPARC_GOTDATA_OP_LOX10 = 83,
  R_SPARC_GOTDATA_OP = 84,
  R_SPARC_SIZE32 = 86,
  R_SPARC_JMP_IREL = 248,
  R_SPARC_IRELATIVE = 249,
};

enum : u32 {
  R_390_NONE = 0,
  R_390_8 = 1,
  R_390_12 = 2,
  R_390_16 = 3,
  R_390_32 = 4,
  R_390_PC32 = 5,
  R_390_GOT12 = 6,
  R_390_GOT32 = 7,
  R_390_PLT32 = 8,
  R_390_COPY = 9,
  R_390_GLOB_DAT = 10,
  R_390_JMP_SLOT = 11,
  R_390_RELATIVE = 12,
  R_390_GOTOFF32 = 13,
  R_390_GOTPC = 14,
  R_390_GOT16 = 15,
  R_390_PC16 = 16,
  R_390_PC16DBL = 17,
  R_390_PLT16DBL = 18,
  R_390_PC32DBL = 19,
  R_390_PLT32DBL = 20,
  R_390_GOTPCDBL = 21,
  R_390_64 = 22,
  R_390_PC64 = 23,
  R_390_GOT64 = 24,
  R_390_PLT64 = 25,
  R_390_GOTENT = 26,
  R_390_GOTOFF16 = 27,
  R_390_GOTOFF64 = 28,
  R_390_GOTPLT12 = 29,
  R_390_GOTPLT16 = 30,
  R_390_GOTPLT32 = 31,
  R_390_GOTPLT64 = 32,
  R_390_GOTPLTENT = 33,
  R_390_PLTOFF16 = 34,
  R_390_PLTOFF32 = 35,
  R_390_PLTOFF64 = 36,
  R_390_TLS_LOAD = 37,
  R_390_TLS_GDCALL = 38,
  R_390_TLS_LDCALL = 39,
  R_390_TLS_GD32 = 40,
  R_390_TLS_GD64 = 41,
  R_390_TLS_GOTIE12 = 42,
  R_390_TLS_GOTIE32 = 43,
  R_390_TLS_GOTIE64 = 44,
  R_390_TLS_LDM32 = 45,
  R_390_TLS_LDM64 = 46,
  R_390_TLS_IE32 = 47,
  R_390_TLS_IE64 = 48,
  R_390_TLS_IEENT = 49,
  R_390_TLS_LE32 = 50,
  R_390_TLS_LE64 = 51,
  R_390_TLS_LDO32 = 52,
  R_390_TLS_LDO64 = 53,
  R_390_TLS_DTPMOD = 54,
  R_390_TLS_DTPOFF = 55,
  R_390_TLS_TPOFF = 56,
  R_390_20 = 57,
  R_390_GOT20 = 58,
  R_390_GOTPLT20 = 59,
  R_390_TLS_GOTIE20 = 60,
  R_390_IRELATIVE = 61,
  R_390_PC12DBL = 62,
  R_390_PLT12DBL = 63,
  R_390_PC24DBL = 64,
  R_390_PLT24DBL = 65,
};

enum : u32 {
  R_68K_NONE = 0,
  R_68K_32 = 1,
  R_68K_16 = 2,
  R_68K_8 = 3,
  R_68K_PC32 = 4,
  R_68K_PC16 = 5,
  R_68K_PC8 = 6,
  R_68K_GOTPCREL32 = 7,
  R_68K_GOTPCREL16 = 8,
  R_68K_GOTPCREL8 = 9,
  R_68K_GOTOFF32 = 10,
  R_68K_GOTOFF16 = 11,
  R_68K_GOTOFF8 = 12,
  R_68K_PLT32 = 13,
  R_68K_PLT16 = 14,
  R_68K_PLT8 = 15,
  R_68K_PLTOFF32 = 16,
  R_68K_PLTOFF16 = 17,
  R_68K_PLTOFF8 = 18,
  R_68K_COPY = 19,
  R_68K_GLOB_DAT = 20,
  R_68K_JMP_SLOT = 21,
  R_68K_RELATIVE = 22,
  R_68K_TLS_GD32 = 25,
  R_68K_TLS_GD16 = 26,
  R_68K_TLS_GD8 = 27,
  R_68K_TLS_LDM32 = 28,
  R_68K_TLS_LDM16 = 29,
  R_68K_TLS_LDM8 = 30,
  R_68K_TLS_LDO32 = 31,
  R_68K_TLS_LDO16 = 32,
  R_68K_TLS_LDO8 = 33,
  R_68K_TLS_IE32 = 34,
  R_68K_TLS_IE16 = 35,
  R_68K_TLS_IE8 = 36,
  R_68K_TLS_LE32 = 37,
  R_68K_TLS_LE16 = 38,
  R_68K_TLS_LE8 = 39,
  R_68K_TLS_DTPMOD32 = 40,
  R_68K_TLS_DTPREL32 = 41,
  R_68K_TLS_TPREL32 = 42,
};

enum : u32 {
  R_SH_NONE = 0,
  R_SH_DIR32 = 1,
  R_SH_REL32 = 2,
  R_SH_DIR8WPN = 3,
  R_SH_IND12W = 4,
  R_SH_DIR8WPL = 5,
  R_SH_DIR8WPZ = 6,
  R_SH_DIR8BP = 7,
  R_SH_DIR8W = 8,
  R_SH_DIR8L = 9,
  R_SH_TLS_GD_32 = 144,
  R_SH_TLS_LD_32 = 145,
  R_SH_TLS_LDO_32 = 146,
  R_SH_TLS_IE_32 = 147,
  R_SH_TLS_LE_32 = 148,
  R_SH_TLS_DTPMOD32 = 149,
  R_SH_TLS_DTPOFF32 = 150,
  R_SH_TLS_TPOFF32 = 151,
  R_SH_GOT32 = 160,
  R_SH_PLT32 = 161,
  R_SH_COPY = 162,
  R_SH_GLOB_DAT = 163,
  R_SH_JMP_SLOT = 164,
  R_SH_RELATIVE = 165,
  R_SH_GOTOFF = 166,
  R_SH_GOTPC = 167,
  R_SH_GOTPLT32 = 168,
};

enum : u32 {
  R_ALPHA_NONE = 0,
  R_ALPHA_REFLONG = 1,
  R_ALPHA_REFQUAD = 2,
  R_ALPHA_GPREL32 = 3,
  R_ALPHA_LITERAL = 4,
  R_ALPHA_LITUSE = 5,
  R_ALPHA_GPDISP = 6,
  R_ALPHA_BRADDR = 7,
  R_ALPHA_HINT = 8,
  R_ALPHA_SREL16 = 9,
  R_ALPHA_SREL32 = 10,
  R_ALPHA_SREL64 = 11,
  R_ALPHA_GPRELHIGH = 17,
  R_ALPHA_GPRELLOW = 18,
  R_ALPHA_GPREL16 = 19,
  R_ALPHA_COPY = 24,
  R_ALPHA_GLOB_DAT = 25,
  R_ALPHA_JMP_SLOT = 26,
  R_ALPHA_RELATIVE = 27,
  R_ALPHA_BRSGP = 28,
  R_ALPHA_TLSGD = 29,
  R_ALPHA_TLSLDM = 30,
  R_ALPHA_DTPMOD64 = 31,
  R_ALPHA_GOTDTPREL = 32,
  R_ALPHA_DTPREL64 = 33,
  R_ALPHA_DTPRELHI = 34,
  R_ALPHA_DTPRELLO = 35,
  R_ALPHA_DTPREL16 = 36,
  R_ALPHA_GOTTPREL = 37,
  R_ALPHA_TPREL64 = 38,
  R_ALPHA_TPRELHI = 39,
  R_ALPHA_TPRELLO = 40,
  R_ALPHA_TPREL16 = 41,
};

//
// DWARF data types
//

enum : u32 {
  DW_EH_PE_absptr = 0,
  DW_EH_PE_omit = 0xff,
  DW_EH_PE_uleb128 = 0x01,
  DW_EH_PE_udata2 = 0x02,
  DW_EH_PE_udata4 = 0x03,
  DW_EH_PE_udata8 = 0x04,
  DW_EH_PE_signed = 0x08,
  DW_EH_PE_sleb128 = 0x09,
  DW_EH_PE_sdata2 = 0x0a,
  DW_EH_PE_sdata4 = 0x0b,
  DW_EH_PE_sdata8 = 0x0c,
  DW_EH_PE_pcrel = 0x10,
  DW_EH_PE_textrel = 0x20,
  DW_EH_PE_datarel = 0x30,
  DW_EH_PE_funcrel = 0x40,
  DW_EH_PE_aligned = 0x50,
};

enum : u32 {
  DW_AT_low_pc = 0x11,
  DW_AT_high_pc = 0x12,
  DW_AT_producer = 0x25,
  DW_AT_ranges = 0x55,
  DW_AT_addr_base = 0x73,
  DW_AT_rnglists_base = 0x74,
};

enum : u32 {
  DW_TAG_compile_unit = 0x11,
  DW_TAG_skeleton_unit = 0x4a,
};

enum : u32 {
  DW_UT_compile = 0x01,
  DW_UT_partial = 0x03,
  DW_UT_skeleton = 0x04,
  DW_UT_split_compile = 0x05,
};

enum : u32 {
  DW_FORM_addr = 0x01,
  DW_FORM_block2 = 0x03,
  DW_FORM_block4 = 0x04,
  DW_FORM_data2 = 0x05,
  DW_FORM_data4 = 0x06,
  DW_FORM_data8 = 0x07,
  DW_FORM_string = 0x08,
  DW_FORM_block = 0x09,
  DW_FORM_block1 = 0x0a,
  DW_FORM_data1 = 0x0b,
  DW_FORM_flag = 0x0c,
  DW_FORM_sdata = 0x0d,
  DW_FORM_strp = 0x0e,
  DW_FORM_udata = 0x0f,
  DW_FORM_ref_addr = 0x10,
  DW_FORM_ref1 = 0x11,
  DW_FORM_ref2 = 0x12,
  DW_FORM_ref4 = 0x13,
  DW_FORM_ref8 = 0x14,
  DW_FORM_ref_udata = 0x15,
  DW_FORM_indirect = 0x16,
  DW_FORM_sec_offset = 0x17,
  DW_FORM_exprloc = 0x18,
  DW_FORM_flag_present = 0x19,
  DW_FORM_strx = 0x1a,
  DW_FORM_addrx = 0x1b,
  DW_FORM_ref_sup4 = 0x1c,
  DW_FORM_strp_sup = 0x1d,
  DW_FORM_data16 = 0x1e,
  DW_FORM_line_strp = 0x1f,
  DW_FORM_ref_sig8 = 0x20,
  DW_FORM_implicit_const = 0x21,
  DW_FORM_loclistx = 0x22,
  DW_FORM_rnglistx = 0x23,
  DW_FORM_ref_sup8 = 0x24,
  DW_FORM_strx1 = 0x25,
  DW_FORM_strx2 = 0x26,
  DW_FORM_strx3 = 0x27,
  DW_FORM_strx4 = 0x28,
  DW_FORM_addrx1 = 0x29,
  DW_FORM_addrx2 = 0x2a,
  DW_FORM_addrx3 = 0x2b,
  DW_FORM_addrx4 = 0x2c,
};

enum : u32 {
  DW_RLE_end_of_list = 0x00,
  DW_RLE_base_addressx = 0x01,
  DW_RLE_startx_endx = 0x02,
  DW_RLE_startx_length = 0x03,
  DW_RLE_offset_pair = 0x04,
  DW_RLE_base_address = 0x05,
  DW_RLE_start_end = 0x06,
  DW_RLE_start_length = 0x07,
};

//
// ELF types
//

template <typename E> using I16 = std::conditional_t<E::is_le, il16, ib16>;
template <typename E> using I32 = std::conditional_t<E::is_le, il32, ib32>;
template <typename E> using I64 = std::conditional_t<E::is_le, il64, ib64>;
template <typename E> using U16 = std::conditional_t<E::is_le, ul16, ub16>;
template <typename E> using U24 = std::conditional_t<E::is_le, ul24, ub24>;
template <typename E> using U32 = std::conditional_t<E::is_le, ul32, ub32>;
template <typename E> using U64 = std::conditional_t<E::is_le, ul64, ub64>;

template <typename E> using Word = std::conditional_t<E::is_64, U64<E>, U32<E>>;
template <typename E> using IWord = std::conditional_t<E::is_64, I64<E>, I32<E>>;

template <typename E> requires E::is_64
struct ElfSym<E> {
  bool is_undef() const { return st_shndx == SHN_UNDEF; }
  bool is_abs() const { return st_shndx == SHN_ABS; }
  bool is_common() const { return st_shndx == SHN_COMMON; }
  bool is_weak() const { return st_bind == STB_WEAK; }
  bool is_undef_weak() const { return is_undef() && is_weak(); }

  U32<E> st_name;

#ifdef __LITTLE_ENDIAN__
  u8 st_type : 4;
  u8 st_bind : 4;
  u8 st_visibility : 2;
  u8 : 6;
#else
  u8 st_bind : 4;
  u8 st_type : 4;
  u8 : 6;
  u8 st_visibility : 2;
#endif

  U16<E> st_shndx;
  U64<E> st_value;
  U64<E> st_size;
};

template <typename E> requires (!E::is_64)
struct ElfSym<E> {
  bool is_undef() const { return st_shndx == SHN_UNDEF; }
  bool is_abs() const { return st_shndx == SHN_ABS; }
  bool is_common() const { return st_shndx == SHN_COMMON; }
  bool is_weak() const { return st_bind == STB_WEAK; }
  bool is_undef_weak() const { return is_undef() && is_weak(); }

  U32<E> st_name;
  U32<E> st_value;
  U32<E> st_size;

#ifdef __LITTLE_ENDIAN__
  u8 st_type : 4;
  u8 st_bind : 4;
  u8 st_visibility : 2;
  u8 : 6;
#else
  u8 st_bind : 4;
  u8 st_type : 4;
  u8 : 6;
  u8 st_visibility : 2;
#endif

  U16<E> st_shndx;
};

template <typename E>
struct ElfShdr {
  U32<E> sh_name;
  U32<E> sh_type;
  Word<E> sh_flags;
  Word<E> sh_addr;
  Word<E> sh_offset;
  Word<E> sh_size;
  U32<E> sh_link;
  U32<E> sh_info;
  Word<E> sh_addralign;
  Word<E> sh_entsize;
};

template <typename E>
struct ElfEhdr {
  u8 e_ident[16];
  U16<E> e_type;
  U16<E> e_machine;
  U32<E> e_version;
  Word<E> e_entry;
  Word<E> e_phoff;
  Word<E> e_shoff;
  U32<E> e_flags;
  U16<E> e_ehsize;
  U16<E> e_phentsize;
  U16<E> e_phnum;
  U16<E> e_shentsize;
  U16<E> e_shnum;
  U16<E> e_shstrndx;
};

template <typename E> requires E::is_64
struct ElfPhdr<E> {
  U32<E> p_type;
  U32<E> p_flags;
  U64<E> p_offset;
  U64<E> p_vaddr;
  U64<E> p_paddr;
  U64<E> p_filesz;
  U64<E> p_memsz;
  U64<E> p_align;
};

template <typename E> requires (!E::is_64)
struct ElfPhdr<E> {
  U32<E> p_type;
  U32<E> p_offset;
  U32<E> p_vaddr;
  U32<E> p_paddr;
  U32<E> p_filesz;
  U32<E> p_memsz;
  U32<E> p_flags;
  U32<E> p_align;
};

// Depending on the target, ElfRel may or may not contain r_addend member.
// The relocation record containing r_addend is called RELA, and that
// without r_addend is called REL.
//
// If REL, relocation addends are stored as parts of section contents.
// That means we add a computed value to an existing value when writing a
// relocated value if REL. If RELA, we just overwrite an existing value
// with a newly computed value.
//
// We don't want to have too many `if (REL)`s and `if (RELA)`s in our
// codebase, so ElfRel always takes r_addend as a constructor argument.
// If it's REL, the argument will simply be ignored.
template <typename E> requires E::is_le && E::is_rela
struct ElfRel<E> {
  ElfRel() = default;
  ElfRel(u64 offset, u32 type, u32 sym, i64 addend)
    : r_offset(offset), r_type(type), r_sym(sym), r_addend(addend) {}

  Word<E> r_offset;
  std::conditional_t<E::is_64, U32<E>, u8> r_type;
  std::conditional_t<E::is_64, U32<E>, U24<E>> r_sym;
  IWord<E> r_addend;
};

template <typename E> requires (!E::is_le) && E::is_rela
struct ElfRel<E> {
  ElfRel() = default;
  ElfRel(u64 offset, u32 type, u32 sym, i64 addend)
    : r_offset(offset), r_sym(sym), r_type(type), r_addend(addend) {}

  Word<E> r_offset;
  std::conditional_t<E::is_64, U32<E>, U24<E>> r_sym;
  std::conditional_t<E::is_64, U32<E>, u8> r_type;
  IWord<E> r_addend;
};

template <typename E> requires E::is_le && (!E::is_rela)
struct ElfRel<E> {
  ElfRel() = default;
  ElfRel(u64 offset, u32 type, u32 sym, i64 addend = 0)
    : r_offset(offset), r_type(type), r_sym(sym) {}

  Word<E> r_offset;
  std::conditional_t<E::is_64, U32<E>, u8> r_type;
  std::conditional_t<E::is_64, U32<E>, U24<E>> r_sym;
};

template <typename E> requires (!E::is_le) && (!E::is_rela)
struct ElfRel<E> {
  ElfRel() = default;
  ElfRel(u64 offset, u32 type, u32 sym, i64 addend = 0)
    : r_offset(offset), r_sym(sym), r_type(type) {}

  Word<E> r_offset;
  std::conditional_t<E::is_64, U32<E>, U24<E>> r_sym;
  std::conditional_t<E::is_64, U32<E>, u8> r_type;
};

template <typename E>
struct ElfDyn {
  Word<E> d_tag;
  Word<E> d_val;
};

template <typename E>
struct ElfVerneed {
  U16<E> vn_version;
  U16<E> vn_cnt;
  U32<E> vn_file;
  U32<E> vn_aux;
  U32<E> vn_next;
};

template <typename E>
struct ElfVernaux {
  U32<E> vna_hash;
  U16<E> vna_flags;
  U16<E> vna_other;
  U32<E> vna_name;
  U32<E> vna_next;
};

template <typename E>
struct ElfVerdef {
  U16<E> vd_version;
  U16<E> vd_flags;
  U16<E> vd_ndx;
  U16<E> vd_cnt;
  U32<E> vd_hash;
  U32<E> vd_aux;
  U32<E> vd_next;
};

template <typename E>
struct ElfVerdaux {
  U32<E> vda_name;
  U32<E> vda_next;
};

template <typename E> requires E::is_64
struct ElfChdr<E> {
  U32<E> ch_type;
  U32<E> ch_reserved;
  U64<E> ch_size;
  U64<E> ch_addralign;
};

template <typename E> requires (!E::is_64)
struct ElfChdr<E> {
  U32<E> ch_type;
  U32<E> ch_size;
  U32<E> ch_addralign;
};

template <typename E>
struct ElfNhdr {
  U32<E> n_namesz;
  U32<E> n_descsz;
  U32<E> n_type;
};

//
// Target-specific ELF data types
//

template <>
struct ElfSym<PPC64V2> {
  bool is_undef() const { return st_shndx == SHN_UNDEF; }
  bool is_abs() const { return st_shndx == SHN_ABS; }
  bool is_common() const { return st_shndx == SHN_COMMON; }
  bool is_weak() const { return st_bind == STB_WEAK; }
  bool is_undef_weak() const { return is_undef() && is_weak(); }

  bool preserves_r2() const { return ppc_local_entry != 1; }
  bool uses_toc() const { return ppc_local_entry > 1; }

  ul32 st_name;

#ifdef __LITTLE_ENDIAN__
  u8 st_type : 4;
  u8 st_bind : 4;
  u8 st_visibility : 2;
  u8 : 3;
  u8 ppc_local_entry : 3; // This is PPC64V2-specific
#else
  u8 st_bind : 4;
  u8 st_type : 4;
  u8 ppc_local_entry : 3;
  u8 : 3;
  u8 st_visibility : 2;
#endif

  ul16 st_shndx;
  ul64 st_value;
  ul64 st_size;
};

template <>
struct ElfSym<ALPHA> {
  bool is_undef() const { return st_shndx == SHN_UNDEF; }
  bool is_abs() const { return st_shndx == SHN_ABS; }
  bool is_common() const { return st_shndx == SHN_COMMON; }
  bool is_weak() const { return st_bind == STB_WEAK; }
  bool is_undef_weak() const { return is_undef() && is_weak(); }

  ul32 st_name;

#ifdef __LITTLE_ENDIAN__
  u8 st_type : 4;
  u8 st_bind : 4;
  u8 st_visibility : 2;
  u8 alpha_st_other : 6; // contains STO_ALPHA_NOPV, STO_ALPHA_STD_GPLOAD or 0
#else
  u8 st_bind : 4;
  u8 st_type : 4;
  u8 alpha_st_other : 6;
  u8 st_visibility : 2;
#endif

  ul16 st_shndx;
  ul64 st_value;
  ul64 st_size;
};

template <>
struct ElfRel<SPARC64> {
  ElfRel() = default;
  ElfRel(u64 offset, u32 type, u32 sym, i64 addend)
    : r_offset(offset), r_sym(sym), r_type_data(0), r_type(type),
      r_addend(addend) {}

  ub64 r_offset;
  ub32 r_sym;
  ub24 r_type_data; // SPARC-specific: used for R_SPARC_OLO10
  u8 r_type;
  ib64 r_addend;
};

//
// Machine descriptions
//

template <typename E>
static constexpr bool supports_ifunc = requires { E::R_IRELATIVE; };

template <typename E>
static constexpr bool supports_tlsdesc = requires { E::R_TLSDESC; };

template <typename E>
static constexpr bool needs_thunk = requires { E::thunk_size; };

template <typename E> static constexpr bool is_x86_64 = std::is_same_v<E, X86_64>;
template <typename E> static constexpr bool is_i386 = std::is_same_v<E, I386>;
template <typename E> static constexpr bool is_arm64 = std::is_same_v<E, ARM64>;
template <typename E> static constexpr bool is_arm32 = std::is_same_v<E, ARM32>;
template <typename E> static constexpr bool is_rv64le = std::is_same_v<E, RV64LE>;
template <typename E> static constexpr bool is_rv64be = std::is_same_v<E, RV64BE>;
template <typename E> static constexpr bool is_rv32le = std::is_same_v<E, RV32LE>;
template <typename E> static constexpr bool is_rv32be = std::is_same_v<E, RV32BE>;
template <typename E> static constexpr bool is_ppc32 = std::is_same_v<E, PPC32>;
template <typename E> static constexpr bool is_ppc64v1 = std::is_same_v<E, PPC64V1>;
template <typename E> static constexpr bool is_ppc64v2 = std::is_same_v<E, PPC64V2>;
template <typename E> static constexpr bool is_s390x = std::is_same_v<E, S390X>;
template <typename E> static constexpr bool is_sparc64 = std::is_same_v<E, SPARC64>;
template <typename E> static constexpr bool is_m68k = std::is_same_v<E, M68K>;
template <typename E> static constexpr bool is_sh4 = std::is_same_v<E, SH4>;
template <typename E> static constexpr bool is_alpha = std::is_same_v<E, ALPHA>;

template <typename E> static constexpr bool is_x86 = is_x86_64<E> || is_i386<E>;
template <typename E> static constexpr bool is_arm = is_arm64<E> || is_arm32<E>;
template <typename E> static constexpr bool is_rv64 = is_rv64le<E> || is_rv64be<E>;
template <typename E> static constexpr bool is_rv32 = is_rv32le<E> || is_rv32be<E>;
template <typename E> static constexpr bool is_riscv = is_rv64<E> || is_rv32<E>;
template <typename E> static constexpr bool is_ppc64 = is_ppc64v1<E> || is_ppc64v2<E>;
template <typename E> static constexpr bool is_ppc = is_ppc64<E> || is_ppc32<E>;
template <typename E> static constexpr bool is_sparc = is_sparc64<E>;

struct X86_64 {
  static constexpr std::string_view target_name = "x86_64";
  static constexpr bool is_64 = true;
  static constexpr bool is_le = true;
  static constexpr bool is_rela = true;
  static constexpr u32 page_size = 4096;
  static constexpr u32 e_machine = EM_X86_64;
  static constexpr u32 plt_hdr_size = 32;
  static constexpr u32 plt_size = 16;
  static constexpr u32 pltgot_size = 16;

  static constexpr u32 R_COPY = R_X86_64_COPY;
  static constexpr u32 R_GLOB_DAT = R_X86_64_GLOB_DAT;
  static constexpr u32 R_JUMP_SLOT = R_X86_64_JUMP_SLOT;
  static constexpr u32 R_ABS = R_X86_64_64;
  static constexpr u32 R_RELATIVE = R_X86_64_RELATIVE;
  static constexpr u32 R_IRELATIVE = R_X86_64_IRELATIVE;
  static constexpr u32 R_DTPOFF = R_X86_64_DTPOFF64;
  static constexpr u32 R_TPOFF = R_X86_64_TPOFF64;
  static constexpr u32 R_DTPMOD = R_X86_64_DTPMOD64;
  static constexpr u32 R_TLSDESC = R_X86_64_TLSDESC;
};

struct I386 {
  static constexpr std::string_view target_name = "i386";
  static constexpr bool is_64 = false;
  static constexpr bool is_le = true;
  static constexpr bool is_rela = false;
  static constexpr u32 page_size = 4096;
  static constexpr u32 e_machine = EM_386;
  static constexpr u32 plt_hdr_size = 16;
  static constexpr u32 plt_size = 16;
  static constexpr u32 pltgot_size = 16;

  static constexpr u32 R_COPY = R_386_COPY;
  static constexpr u32 R_GLOB_DAT = R_386_GLOB_DAT;
  static constexpr u32 R_JUMP_SLOT = R_386_JUMP_SLOT;
  static constexpr u32 R_ABS = R_386_32;
  static constexpr u32 R_RELATIVE = R_386_RELATIVE;
  static constexpr u32 R_IRELATIVE = R_386_IRELATIVE;
  static constexpr u32 R_DTPOFF = R_386_TLS_DTPOFF32;
  static constexpr u32 R_TPOFF = R_386_TLS_TPOFF;
  static constexpr u32 R_DTPMOD = R_386_TLS_DTPMOD32;
  static constexpr u32 R_TLSDESC = R_386_TLS_DESC;
};

struct ARM64 {
  static constexpr std::string_view target_name = "arm64";
  static constexpr bool is_64 = true;
  static constexpr bool is_le = true;
  static constexpr bool is_rela = true;
  static constexpr u32 page_size = 65536;
  static constexpr u32 e_machine = EM_AARCH64;
  static constexpr u32 plt_hdr_size = 32;
  static constexpr u32 plt_size = 16;
  static constexpr u32 pltgot_size = 16;
  static constexpr u32 thunk_hdr_size = 0;
  static constexpr u32 thunk_size = 12;

  static constexpr u32 R_COPY = R_AARCH64_COPY;
  static constexpr u32 R_GLOB_DAT = R_AARCH64_GLOB_DAT;
  static constexpr u32 R_JUMP_SLOT = R_AARCH64_JUMP_SLOT;
  static constexpr u32 R_ABS = R_AARCH64_ABS64;
  static constexpr u32 R_RELATIVE = R_AARCH64_RELATIVE;
  static constexpr u32 R_IRELATIVE = R_AARCH64_IRELATIVE;
  static constexpr u32 R_DTPOFF = R_AARCH64_TLS_DTPREL64;
  static constexpr u32 R_TPOFF = R_AARCH64_TLS_TPREL64;
  static constexpr u32 R_DTPMOD = R_AARCH64_TLS_DTPMOD64;
  static constexpr u32 R_TLSDESC = R_AARCH64_TLSDESC;
};

struct ARM32 {
  static constexpr std::string_view target_name = "arm32";
  static constexpr bool is_64 = false;
  static constexpr bool is_le = true;
  static constexpr bool is_rela = false;
  static constexpr u32 page_size = 4096;
  static constexpr u32 e_machine = EM_ARM;
  static constexpr u32 plt_hdr_size = 32;
  static constexpr u32 plt_size = 16;
  static constexpr u32 pltgot_size = 16;
  static constexpr u32 thunk_hdr_size = 12;
  static constexpr u32 thunk_size = 20;

  static constexpr u32 R_COPY = R_ARM_COPY;
  static constexpr u32 R_GLOB_DAT = R_ARM_GLOB_DAT;
  static constexpr u32 R_JUMP_SLOT = R_ARM_JUMP_SLOT;
  static constexpr u32 R_ABS = R_ARM_ABS32;
  static constexpr u32 R_RELATIVE = R_ARM_RELATIVE;
  static constexpr u32 R_IRELATIVE = R_ARM_IRELATIVE;
  static constexpr u32 R_DTPOFF = R_ARM_TLS_DTPOFF32;
  static constexpr u32 R_TPOFF = R_ARM_TLS_TPOFF32;
  static constexpr u32 R_DTPMOD = R_ARM_TLS_DTPMOD32;
  static constexpr u32 R_TLSDESC = R_ARM_TLS_DESC;
};

struct RV64LE {
  static constexpr std::string_view target_name = "riscv64";
  static constexpr bool is_64 = true;
  static constexpr bool is_le = true;
  static constexpr bool is_rela = true;
  static constexpr u32 page_size = 4096;
  static constexpr u32 e_machine = EM_RISCV;
  static constexpr u32 plt_hdr_size = 32;
  static constexpr u32 plt_size = 16;
  static constexpr u32 pltgot_size = 16;

  static constexpr u32 R_COPY = R_RISCV_COPY;
  static constexpr u32 R_GLOB_DAT = R_RISCV_64;
  static constexpr u32 R_JUMP_SLOT = R_RISCV_JUMP_SLOT;
  static constexpr u32 R_ABS = R_RISCV_64;
  static constexpr u32 R_RELATIVE = R_RISCV_RELATIVE;
  static constexpr u32 R_IRELATIVE = R_RISCV_IRELATIVE;
  static constexpr u32 R_DTPOFF = R_RISCV_TLS_DTPREL64;
  static constexpr u32 R_TPOFF = R_RISCV_TLS_TPREL64;
  static constexpr u32 R_DTPMOD = R_RISCV_TLS_DTPMOD64;
};

struct RV64BE {
  static constexpr std::string_view target_name = "riscv64be";
  static constexpr bool is_64 = true;
  static constexpr bool is_le = false;
  static constexpr bool is_rela = true;
  static constexpr u32 page_size = 4096;
  static constexpr u32 e_machine = EM_RISCV;
  static constexpr u32 plt_hdr_size = 32;
  static constexpr u32 plt_size = 16;
  static constexpr u32 pltgot_size = 16;

  static constexpr u32 R_COPY = R_RISCV_COPY;
  static constexpr u32 R_GLOB_DAT = R_RISCV_64;
  static constexpr u32 R_JUMP_SLOT = R_RISCV_JUMP_SLOT;
  static constexpr u32 R_ABS = R_RISCV_64;
  static constexpr u32 R_RELATIVE = R_RISCV_RELATIVE;
  static constexpr u32 R_IRELATIVE = R_RISCV_IRELATIVE;
  static constexpr u32 R_DTPOFF = R_RISCV_TLS_DTPREL64;
  static constexpr u32 R_TPOFF = R_RISCV_TLS_TPREL64;
  static constexpr u32 R_DTPMOD = R_RISCV_TLS_DTPMOD64;
};

struct RV32LE {
  static constexpr std::string_view target_name = "riscv32";
  static constexpr bool is_64 = false;
  static constexpr bool is_le = true;
  static constexpr bool is_rela = true;
  static constexpr u32 page_size = 4096;
  static constexpr u32 e_machine = EM_RISCV;
  static constexpr u32 plt_hdr_size = 32;
  static constexpr u32 plt_size = 16;
  static constexpr u32 pltgot_size = 16;

  static constexpr u32 R_COPY = R_RISCV_COPY;
  static constexpr u32 R_GLOB_DAT = R_RISCV_32;
  static constexpr u32 R_JUMP_SLOT = R_RISCV_JUMP_SLOT;
  static constexpr u32 R_ABS = R_RISCV_32;
  static constexpr u32 R_RELATIVE = R_RISCV_RELATIVE;
  static constexpr u32 R_IRELATIVE = R_RISCV_IRELATIVE;
  static constexpr u32 R_DTPOFF = R_RISCV_TLS_DTPREL32;
  static constexpr u32 R_TPOFF = R_RISCV_TLS_TPREL32;
  static constexpr u32 R_DTPMOD = R_RISCV_TLS_DTPMOD32;
};

struct RV32BE {
  static constexpr std::string_view target_name = "riscv32be";
  static constexpr bool is_64 = false;
  static constexpr bool is_le = false;
  static constexpr bool is_rela = true;
  static constexpr u32 page_size = 4096;
  static constexpr u32 e_machine = EM_RISCV;
  static constexpr u32 plt_hdr_size = 32;
  static constexpr u32 plt_size = 16;
  static constexpr u32 pltgot_size = 16;

  static constexpr u32 R_COPY = R_RISCV_COPY;
  static constexpr u32 R_GLOB_DAT = R_RISCV_32;
  static constexpr u32 R_JUMP_SLOT = R_RISCV_JUMP_SLOT;
  static constexpr u32 R_ABS = R_RISCV_32;
  static constexpr u32 R_RELATIVE = R_RISCV_RELATIVE;
  static constexpr u32 R_IRELATIVE = R_RISCV_IRELATIVE;
  static constexpr u32 R_DTPOFF = R_RISCV_TLS_DTPREL32;
  static constexpr u32 R_TPOFF = R_RISCV_TLS_TPREL32;
  static constexpr u32 R_DTPMOD = R_RISCV_TLS_DTPMOD32;
};

struct PPC32 {
  static constexpr std::string_view target_name = "ppc32";
  static constexpr bool is_64 = false;
  static constexpr bool is_le = false;
  static constexpr bool is_rela = true;
  static constexpr u32 page_size = 65536;
  static constexpr u32 e_machine = EM_PPC;
  static constexpr u32 plt_hdr_size = 64;
  static constexpr u32 plt_size = 36;
  static constexpr u32 pltgot_size = 36;
  static constexpr u32 thunk_hdr_size = 0;
  static constexpr u32 thunk_size = 36;

  static constexpr u32 R_COPY = R_PPC_COPY;
  static constexpr u32 R_GLOB_DAT = R_PPC_GLOB_DAT;
  static constexpr u32 R_JUMP_SLOT = R_PPC_JMP_SLOT;
  static constexpr u32 R_ABS = R_PPC_ADDR32;
  static constexpr u32 R_RELATIVE = R_PPC_RELATIVE;
  static constexpr u32 R_IRELATIVE = R_PPC_IRELATIVE;
  static constexpr u32 R_DTPOFF = R_PPC_DTPREL32;
  static constexpr u32 R_TPOFF = R_PPC_TPREL32;
  static constexpr u32 R_DTPMOD = R_PPC_DTPMOD32;
};

struct PPC64V1 {
  static constexpr std::string_view target_name = "ppc64v1";
  static constexpr bool is_64 = true;
  static constexpr bool is_le = false;
  static constexpr bool is_rela = true;
  static constexpr u32 page_size = 65536;
  static constexpr u32 e_machine = EM_PPC64;
  static constexpr u32 plt_hdr_size = 52;
  static constexpr u32 pltgot_size = 0;
  static constexpr u32 thunk_hdr_size = 0;
  static constexpr u32 thunk_size = 28;

  static constexpr u32 R_COPY = R_PPC64_COPY;
  static constexpr u32 R_GLOB_DAT = R_PPC64_GLOB_DAT;
  static constexpr u32 R_JUMP_SLOT = R_PPC64_JMP_SLOT;
  static constexpr u32 R_ABS = R_PPC64_ADDR64;
  static constexpr u32 R_RELATIVE = R_PPC64_RELATIVE;
  static constexpr u32 R_IRELATIVE = R_PPC64_IRELATIVE;
  static constexpr u32 R_DTPOFF = R_PPC64_DTPREL64;
  static constexpr u32 R_TPOFF = R_PPC64_TPREL64;
  static constexpr u32 R_DTPMOD = R_PPC64_DTPMOD64;
};

struct PPC64V2 {
  static constexpr std::string_view target_name = "ppc64v2";
  static constexpr bool is_64 = true;
  static constexpr bool is_le = true;
  static constexpr bool is_rela = true;
  static constexpr u32 page_size = 65536;
  static constexpr u32 e_machine = EM_PPC64;
  static constexpr u32 plt_hdr_size = 60;
  static constexpr u32 plt_size = 4;
  static constexpr u32 pltgot_size = 0;
  static constexpr u32 thunk_hdr_size = 0;
  static constexpr u32 thunk_size = 20;

  static constexpr u32 R_COPY = R_PPC64_COPY;
  static constexpr u32 R_GLOB_DAT = R_PPC64_GLOB_DAT;
  static constexpr u32 R_JUMP_SLOT = R_PPC64_JMP_SLOT;
  static constexpr u32 R_ABS = R_PPC64_ADDR64;
  static constexpr u32 R_RELATIVE = R_PPC64_RELATIVE;
  static constexpr u32 R_IRELATIVE = R_PPC64_IRELATIVE;
  static constexpr u32 R_DTPOFF = R_PPC64_DTPREL64;
  static constexpr u32 R_TPOFF = R_PPC64_TPREL64;
  static constexpr u32 R_DTPMOD = R_PPC64_DTPMOD64;
};

struct S390X {
  static constexpr std::string_view target_name = "s390x";
  static constexpr bool is_64 = true;
  static constexpr bool is_le = false;
  static constexpr bool is_rela = true;
  static constexpr u32 page_size = 4096;
  static constexpr u32 e_machine = EM_S390X;
  static constexpr u32 plt_hdr_size = 32;
  static constexpr u32 plt_size = 32;
  static constexpr u32 pltgot_size = 16;

  static constexpr u32 R_COPY = R_390_COPY;
  static constexpr u32 R_GLOB_DAT = R_390_GLOB_DAT;
  static constexpr u32 R_JUMP_SLOT = R_390_JMP_SLOT;
  static constexpr u32 R_ABS = R_390_64;
  static constexpr u32 R_RELATIVE = R_390_RELATIVE;
  static constexpr u32 R_IRELATIVE = R_390_IRELATIVE;
  static constexpr u32 R_DTPOFF = R_390_TLS_DTPOFF;
  static constexpr u32 R_TPOFF = R_390_TLS_TPOFF;
  static constexpr u32 R_DTPMOD = R_390_TLS_DTPMOD;
};

struct SPARC64 {
  static constexpr std::string_view target_name = "sparc64";
  static constexpr bool is_64 = true;
  static constexpr bool is_le = false;
  static constexpr bool is_rela = true;
  static constexpr u32 page_size = 8192;
  static constexpr u32 e_machine = EM_SPARC64;
  static constexpr u32 plt_hdr_size = 128;
  static constexpr u32 plt_size = 32;
  static constexpr u32 pltgot_size = 32;

  static constexpr u32 R_COPY = R_SPARC_COPY;
  static constexpr u32 R_GLOB_DAT = R_SPARC_GLOB_DAT;
  static constexpr u32 R_JUMP_SLOT = R_SPARC_JMP_SLOT;
  static constexpr u32 R_ABS = R_SPARC_64;
  static constexpr u32 R_RELATIVE = R_SPARC_RELATIVE;
  static constexpr u32 R_IRELATIVE = R_SPARC_IRELATIVE;
  static constexpr u32 R_DTPOFF = R_SPARC_TLS_DTPOFF64;
  static constexpr u32 R_TPOFF = R_SPARC_TLS_TPOFF64;
  static constexpr u32 R_DTPMOD = R_SPARC_TLS_DTPMOD64;
};

struct M68K {
  static constexpr std::string_view target_name = "m68k";
  static constexpr bool is_64 = false;
  static constexpr bool is_le = false;
  static constexpr bool is_rela = true;
  static constexpr u32 page_size = 8192;
  static constexpr u32 e_machine = EM_68K;
  static constexpr u32 plt_hdr_size = 18;
  static constexpr u32 plt_size = 14;
  static constexpr u32 pltgot_size = 8;

  static constexpr u32 R_COPY = R_68K_COPY;
  static constexpr u32 R_GLOB_DAT = R_68K_GLOB_DAT;
  static constexpr u32 R_JUMP_SLOT = R_68K_JMP_SLOT;
  static constexpr u32 R_ABS = R_68K_32;
  static constexpr u32 R_RELATIVE = R_68K_RELATIVE;
  static constexpr u32 R_DTPOFF = R_68K_TLS_DTPREL32;
  static constexpr u32 R_TPOFF = R_68K_TLS_TPREL32;
  static constexpr u32 R_DTPMOD = R_68K_TLS_DTPMOD32;
};

struct SH4 {
  static constexpr std::string_view target_name = "sh4";
  static constexpr bool is_64 = false;
  static constexpr bool is_le = true;
  static constexpr bool is_rela = true;
  static constexpr u32 page_size = 4096;
  static constexpr u32 e_machine = EM_SH;
  static constexpr u32 plt_hdr_size = 16;
  static constexpr u32 plt_size = 16;
  static constexpr u32 pltgot_size = 12;

  static constexpr u32 R_COPY = R_SH_COPY;
  static constexpr u32 R_GLOB_DAT = R_SH_GLOB_DAT;
  static constexpr u32 R_JUMP_SLOT = R_SH_JMP_SLOT;
  static constexpr u32 R_ABS = R_SH_DIR32;
  static constexpr u32 R_RELATIVE = R_SH_RELATIVE;
  static constexpr u32 R_DTPOFF = R_SH_TLS_DTPOFF32;
  static constexpr u32 R_TPOFF = R_SH_TLS_TPOFF32;
  static constexpr u32 R_DTPMOD = R_SH_TLS_DTPMOD32;
};

struct ALPHA {
  static constexpr std::string_view target_name = "alpha";
  static constexpr bool is_64 = true;
  static constexpr bool is_le = true;
  static constexpr bool is_rela = true;
  static constexpr u32 page_size = 65536;
  static constexpr u32 e_machine = EM_ALPHA;
  static constexpr u32 plt_hdr_size = 0;
  static constexpr u32 plt_size = 0;
  static constexpr u32 pltgot_size = 0;

  static constexpr u32 R_COPY = R_ALPHA_COPY;
  static constexpr u32 R_GLOB_DAT = R_ALPHA_GLOB_DAT;
  static constexpr u32 R_JUMP_SLOT = R_ALPHA_JMP_SLOT;
  static constexpr u32 R_ABS = R_ALPHA_REFQUAD;
  static constexpr u32 R_RELATIVE = R_ALPHA_RELATIVE;
  static constexpr u32 R_DTPOFF = R_ALPHA_DTPREL64;
  static constexpr u32 R_TPOFF = R_ALPHA_TPREL64;
  static constexpr u32 R_DTPMOD = R_ALPHA_DTPMOD64;
};

} // namespace mold::elf
