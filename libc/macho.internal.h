#ifndef COSMOPOLITAN_LIBC_MACHO_H_
#define COSMOPOLITAN_LIBC_MACHO_H_
#ifndef __STRICT_ANSI__

/*
 * xnu osfmk/mach/machine.h
 */

#define MAC_ARCH_MASK     0xff000000
#define MAC_ARCH_ABI64    0x01000000
#define MAC_ARCH_ABI64_32 0x02000000

#define MAC_CPU_MC680x0   6
#define MAC_CPU_X86       7
#define MAC_CPU_I386      MAC_CPU_X86
#define MAC_CPU_NEXGEN32E (MAC_CPU_X86 | MAC_ARCH_ABI64)
#define MAC_CPU_MC98000   10
#define MAC_CPU_HPPA      11
#define MAC_CPU_ARM       12
#define MAC_CPU_ARM64     (MAC_CPU_ARM | MAC_ARCH_ABI64)
#define MAC_CPU_ARM64_32  (MAC_CPU_ARM | MAC_ARCH_ABI64_32)
#define MAC_CPU_MC88000   13
#define MAC_CPU_SPARC     14
#define MAC_CPU_I860      15
#define MAC_CPU_POWERPC   18
#define MAC_CPU_POWERPC64 (MAC_CPU_POWERPC | MAC_ARCH_ABI64)

#define MAC_CPU_SUBTYPE_MASK          0xff000000
#define MAC_CPU_SUBTYPE_LIB64         0x80000000
#define MAC_CPU_SUBTYPE_PTRAUTH_ABI   0x80000000
#define MAC_CPU_SUBTYPE_MULTIPLE      -1
#define MAC_CPU_SUBTYPE_LITTLE_ENDIAN 0
#define MAC_CPU_SUBTYPE_BIG_ENDIAN    1

#define MAC_OBJECT   0x1
#define MAC_EXECUTE  0x2
#define MAC_FVMLIB   0x3
#define MAC_CORE     0x4
#define MAC_PRELOAD  0x5
#define MAC_DYLIB    0x6
#define MAC_DYLINKER 0x7
#define MAC_BUNDLE   0x8

#define MAC_CPU_NEXGEN32E_ALL 3

#define MAC_CPU_ARM64_ALL 0
#define MAC_CPU_ARM64_V8  1
#define MAC_CPU_ARM64E    2

/*
 * xnu osfmk/mach/i386/thread_status.h
 */

#define MAC_THREAD_NEXGEN32E        4  // x86_THREAD_STATE64
#define MAC_THREAD_NEXGEN32E_256BIT 17

/*
 * xnu osfmk/mach/arm/thread_status.h
 *
 * struct arm_neon_saved_state64 {
 *   union {
 *     uint128_t q[32];
 *     uint64x2_t d[32];
 *     uint32x4_t s[32];
 *   } v;
 *   uint32_t fpsr;
 *   uint32_t fpcr;
 * };
 */

#define MAC_THREAD_ARM_STATE64       6
#define ARM_NEON_SAVED_STATE64_COUNT 68

/*
 * xnu EXTERNAL_HEADERS/mach-o/loader.h
 */

#define MAC_NOUNDEFS                0x1
#define MAC_INCRLINK                0x2
#define MAC_DYLDLINK                0x4
#define MAC_BINDATLOAD              0x8
#define MAC_PREBOUND                0x10
#define MAC_SPLIT_SEGS              0x20
#define MAC_LAZY_INIT               0x40
#define MAC_TWOLEVEL                0x80
#define MAC_FORCE_FLAT              0x100
#define MAC_NOMULTIDEFS             0x200
#define MAC_NOFIXPREBINDING         0x400
#define MAC_PREBINDABLE             0x800
#define MAC_ALLMODSBOUND            0x1000
#define MAC_SUBSECTIONS_VIA_SYMBOLS 0x2000
#define MAC_CANONICAL               0x4000
#define MAC_ALLOW_STACK_EXECUTION   0x20000
#define MAC_ROOT_SAFE               0x40000
#define MAC_SETUID_SAFE             0x80000
#define MAC_PIE                     0x200000
#define MAC_HAS_TLV_DESCRIPTORS     0x800000
#define MAC_NO_HEAP_EXECUTION       0x1000000

#define MAC_SG_HIGHVM  0x1
#define MAC_SG_FVMLIB  0x2
#define MAC_SG_NORELOC 0x4

/* section type */
#define MAC_S_REGULAR                  0x0
#define MAC_S_ZEROFILL                 0x1
#define MAC_S_CSTRING_LITERALS         0x2
#define MAC_S_4BYTE_LITERALS           0x3
#define MAC_S_8BYTE_LITERALS           0x4
#define MAC_S_LITERAL_POINTERS         0x5
#define MAC_S_NON_LAZY_SYMBOL_POINTERS 0x6
#define MAC_S_LAZY_SYMBOL_POINTERS     0x7
#define MAC_S_SYMBOL_STUBS             0x8
#define MAC_S_MOD_INIT_FUNC_POINTERS   0x9
#define MAC_S_MOD_TERM_FUNC_POINTERS   0xa
#define MAC_S_COALESCED                0xb
#define MAC_S_GB_ZEROFILL              0xc
#define MAC_S_INTERPOSING              0xd
#define MAC_S_16BYTE_LITERALS          0xe

/* section attributes */
#define MAC_S_ATTRIBUTES_USR           0xff000000
#define MAC_S_ATTR_PURE_INSTRUCTIONS   0x80000000
#define MAC_S_ATTR_NO_TOC              0x40000000
#define MAC_S_ATTR_STRIP_STATIC_SYMS   0x20000000
#define MAC_S_ATTR_NO_DEAD_STRIP       0x10000000
#define MAC_S_ATTR_LIVE_SUPPORT        0x08000000
#define MAC_S_ATTR_SELF_MODIFYING_CODE 0x04000000
#define MAC_S_ATTR_DEBUG               0x02000000
#define MAC_S_ATTRIBUTES_SYS           0x00ffff00
#define MAC_S_ATTR_SOME_INSTRUCTIONS   0x00000400
#define MAC_S_ATTR_EXT_RELOC           0x00000200
#define MAC_S_ATTR_LOC_RELOC           0x00000100

#define MAC_LC_REQ_DYLD                 0x80000000
#define MAC_LC_SEGMENT                  0x1
#define MAC_LC_SYMTAB                   0x2
#define MAC_LC_SYMSEG                   0x3
#define MAC_LC_THREAD                   0x4
#define MAC_LC_UNIXTHREAD               0x5
#define MAC_LC_LOADFVMLIB               0x6
#define MAC_LC_IDFVMLIB                 0x7
#define MAC_LC_IDENT                    0x8
#define MAC_LC_FVMFILE                  0x9
#define MAC_LC_PREPAGE                  0xa
#define MAC_LC_DYSYMTAB                 0xb
#define MAC_LC_LOAD_DYLIB               0xc
#define MAC_LC_ID_DYLIB                 0xd
#define MAC_LC_LOAD_DYLINKER            0xe
#define MAC_LC_ID_DYLINKER              0xf
#define MAC_LC_PREBOUND_DYLIB           0x10
#define MAC_LC_ROUTINES                 0x11
#define MAC_LC_SUB_FRAMEWORK            0x12
#define MAC_LC_SUB_UMBRELLA             0x13
#define MAC_LC_SUB_CLIENT               0x14
#define MAC_LC_SUB_LIBRARY              0x15
#define MAC_LC_TWOLEVEL_HINTS           0x16
#define MAC_LC_PREBIND_CKSUM            0x17
#define MAC_LC_LOAD_WEAK_DYLIB          (0x18 | MAC_LC_REQ_DYLD)
#define MAC_LC_SEGMENT_64               0x19
#define MAC_LC_ROUTINES_64              0x1a
#define MAC_LC_UUID                     0x1b
#define MAC_LC_CODE_SIGNATURE           0x1d
#define MAC_LC_SEGMENT_SPLIT_INFO       0x1e
#define MAC_LC_LAZY_LOAD_DYLIB          0x20
#define MAC_LC_ENCRYPTION_INFO          0x21
#define MAC_LC_DYLD_INFO                0x22
#define MAC_LC_DYLD_INFO_ONLY           (0x22 | MAC_LC_REQ_DYLD)
#define MAC_LC_VERSION_MIN_MACOSX       0x24
#define MAC_LC_VERSION_MIN_IPHONEOS     0x25
#define MAC_LC_FUNCTION_STARTS          0x26
#define MAC_LC_DYLD_ENVIRONMENT         0x27
#define MAC_LC_DATA_IN_CODE             0x29
#define MAC_LC_SOURCE_VERSION           0x2a
#define MAC_LC_DYLIB_CODE_SIGN_DRS      0x2B
#define MAC_LC_ENCRYPTION_INFO_64       0x2C
#define MAC_LC_LINKER_OPTION            0x2D
#define MAC_LC_LINKER_OPTIMIZATION_HINT 0x2E
#define MAC_LC_VERSION_MIN_TVOS         0x2F
#define MAC_LC_VERSION_MIN_WATCHOS      0x30
#define MAC_LC_NOTE                     0x31
#define MAC_LC_BUILD_VERSION            0x32
#define MAC_LC_DYLD_EXPORTS_TRIE        (0x33 | MAC_LC_REQ_DYLD)
#define MAC_LC_DYLD_CHAINED_FIXUPS      (0x34 | MAC_LC_REQ_DYLD)
#define MAC_LC_FILESET_ENTRY            (0x35 | MAC_LC_REQ_DYLD)
#define MAC_LC_RPATH                    (0x1c | MAC_LC_REQ_DYLD)
#define MAC_LC_MAIN                     (0x28 | MAC_LC_REQ_DYLD)
#define MAC_LC_REEXPORT_DYLIB           (0x1f | MAC_LC_REQ_DYLD)

#define VM_PROT_NONE       0
#define VM_PROT_READ       1
#define VM_PROT_WRITE      2
#define VM_PROT_EXECUTE    4
#define VM_PROT_NO_CHANGE  8
#define VM_PROT_COPY       16
#define VM_PROT_TRUSTED    32
#define VM_PROT_STRIP_READ 64

#define MACHO_VERSION(x, y, z) ((x) << 16 | (y) << 8 | (z))

#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct MachoHeader {
  uint32_t magic;
  uint32_t arch;
  uint32_t arch2;
  uint32_t filetype;
  uint32_t loadcount;
  uint32_t loadsize;
  uint32_t flags;
  uint32_t __reserved;
};

struct MachoLoadCommand {
  uint32_t command;
  uint32_t size;
};

struct MachoLoadSegment {
  uint32_t command;
  uint32_t size;
  char name[16];
  uint64_t vaddr;
  uint64_t memsz;
  uint64_t offset;
  uint64_t filesz;
  uint32_t maxprot;
  uint32_t initprot;
  uint32_t sectioncount;
  uint32_t flags;
};

struct MachoSection {
  char name[16];
  char commandname[16];
  uint64_t vaddr;
  uint64_t memsz;
  uint32_t offset;
  uint32_t alignlog2;
  uint32_t relotaboff;
  uint32_t relocount;
  uint32_t attr;
  uint32_t __reserved[3];
};

struct MachoLoadSymtab {
  uint32_t command;
  uint32_t size;
  uint32_t offset;
  uint32_t count;
  uint32_t stroff;
  uint32_t strsize;
};

struct MachoLoadMinCommand {
  uint32_t command;
  uint32_t size;
  uint32_t version; /* X.Y.Z is encoded in nibbles xxxx.yy.zz */
  uint32_t sdk;
};

struct MachoLoadSourceVersionCommand {
  uint32_t command;
  uint32_t size;
  uint64_t version; /* A.B.C.D.E packed as a24.b10.c10.d10.e10 */
};

struct MachoLoadEntrypointCommand {
  uint32_t command;
  uint32_t size;
  uint64_t entryoff;
  uint64_t stacksize;
};

struct MachoLoadThreadCommand {
  uint32_t command;
  uint32_t size;
  uint32_t flavor;
  uint32_t count;
  uint32_t wut[];
};

struct MachoLoadUuid {
  uint32_t command;
  uint32_t size;
  uint8_t uuid[16];
};

struct MachoDyldInfoCommand {
  uint32_t cmd;
  uint32_t cmdsize;
  uint32_t rebase_off;
  uint32_t rebase_size;
  uint32_t bind_off;
  uint32_t bind_size;
  uint32_t weak_bind_off;
  uint32_t weak_bind_size;
  uint32_t lazy_bind_off;
  uint32_t lazy_bind_size;
  uint32_t export_off;
  uint32_t export_size;
};

struct MachoDylib {
  uint32_t name; /* (char *)&lc->command + name */
  uint32_t timestamp;
  uint32_t current_version;       /* MACHO_VERSION(x,y,z) */
  uint32_t compatibility_version; /* MACHO_VERSION(x,y,z) */
};

struct MachoDylibCommand {
  uint32_t cmd;
  uint32_t cmdsize;
  struct MachoDylib dylib;
};

struct MachoEntryPointCommand {
  uint32_t cmd;       /* MAC_LC_MAIN */
  uint32_t cmdsize;   /* 24 */
  uint64_t entryoff;  /* __TEXT offset of _start() */
  uint64_t stacksize; /* initial stack size [optional] */
};

struct MachoLinkeditDataCommand {
  /* MAC_LC_CODE_SIGNATURE */
  /* MAC_LC_SEGMENT_SPLIT_INFO */
  /* MAC_LC_FUNCTION_STARTS */
  /* MAC_LC_DATA_IN_CODE */
  /* MAC_LC_DYLIB_CODE_SIGN_DRS */
  /* MAC_LC_LINKER_OPTIMIZATION_HINT */
  /* MAC_LC_DYLD_EXPORTS_TRIE */
  /* MAC_LC_DYLD_CHAINED_FIXUPS */
  uint32_t cmd;
  uint32_t cmdsize;
  uint32_t dataoff;
  uint32_t datasize;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* !ANSI */
#endif /* COSMOPOLITAN_LIBC_MACHO_H_ */
