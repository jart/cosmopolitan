#ifndef COSMOPOLITAN_LIBC_ELF_STRUCT_DEF_H_
#define COSMOPOLITAN_LIBC_ELF_STRUCT_DEF_H_

/**
 * @fileoverview Executable and Linkable Format Definitions.
 */

#define EI_NIDENT 16

#define EI_MAG0 0
#define EI_MAG1 1
#define EI_MAG2 2
#define EI_MAG3 3

#define ELFMAG  "\177ELF"
#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'
#define SELFMAG 4

#define EI_CLASS     4
#define ELFCLASSNONE 0
#define ELFCLASS32   1
#define ELFCLASS64   2
#define ELFCLASSNUM  3

#define EI_DATA     5
#define ELFDATANONE 0
#define ELFDATA2LSB 1
#define ELFDATA2MSB 2
#define ELFDATANUM  3

#define EI_VERSION 6

#define EI_OSABI            7
#define ELFOSABI_NONE       0
#define ELFOSABI_SYSV       0
#define ELFOSABI_HPUX       1
#define ELFOSABI_NETBSD     2
#define ELFOSABI_LINUX      3
#define ELFOSABI_GNU        3
#define ELFOSABI_SOLARIS    6
#define ELFOSABI_AIX        7
#define ELFOSABI_IRIX       8
#define ELFOSABI_FREEBSD    9
#define ELFOSABI_TRU64      10
#define ELFOSABI_MODESTO    11
#define ELFOSABI_OPENBSD    12
#define ELFOSABI_ARM        97
#define ELFOSABI_STANDALONE 255

#define EI_ABIVERSION 8

#define EI_PAD 9

#define ET_NONE   0
#define ET_REL    1
#define ET_EXEC   2
#define ET_DYN    3
#define ET_CORE   4
#define ET_NUM    5
#define ET_LOOS   0xfe00
#define ET_HIOS   0xfeff
#define ET_LOPROC 0xff00
#define ET_HIPROC 0xffff

#define EM_NONE      0
#define EM_M32       1
#define EM_386       3
#define EM_PPC64     21
#define EM_S390      22
#define EM_ARM       40
#define EM_NEXGEN32E 62
#define EM_X86_64    EM_NEXGEN32E
#define EM_IA32E     EM_NEXGEN32E
#define EM_AMD64     EM_NEXGEN32E
#define EM_PDP11     65
#define EM_CRAYNV2   172
#define EM_L10M      180
#define EM_K10M      181
#define EM_AARCH64   183
#define EM_CUDA      190
#define EM_Z80       220
#define EM_RISCV     243
#define EM_BPF       247

#define GRP_COMDAT 1
#define STN_UNDEF  0

#define EV_NONE    0
#define EV_CURRENT 1
#define EV_NUM     2

#define SYMINFO_NONE          0
#define SYMINFO_CURRENT       1
#define SYMINFO_NUM           2
#define SYMINFO_BT_SELF       0xffff
#define SYMINFO_BT_PARENT     0xfffe
#define SYMINFO_BT_LOWRESERVE 0xff00
#define SYMINFO_FLG_DIRECT    0x0001
#define SYMINFO_FLG_PASSTHRU  0x0002
#define SYMINFO_FLG_COPY      0x0004
#define SYMINFO_FLG_LAZYLOAD  0x0008

#define PT_NULL         0
#define PT_LOAD         1
#define PT_DYNAMIC      2
#define PT_INTERP       3
#define PT_NOTE         4
#define PT_SHLIB        5
#define PT_PHDR         6
#define PT_TLS          7
#define PT_NUM          8
#define PT_LOOS         0x60000000
#define PT_GNU_EH_FRAME 0x6474e550
#define PT_GNU_STACK    0x6474e551
#define PT_GNU_RELRO    0x6474e552
#define PT_LOSUNW       0x6ffffffa
#define PT_SUNWBSS      0x6ffffffa
#define PT_SUNWSTACK    0x6ffffffb
#define PT_HISUNW       0x6fffffff
#define PT_HIOS         0x6fffffff
#define PT_LOPROC       0x70000000
#define PT_HIPROC       0x7fffffff

#define PN_XNUM 0xffff

#define PF_X        1
#define PF_W        2
#define PF_R        4
#define PF_MASKOS   0x0ff00000
#define PF_MASKPROC 0xf0000000

#define R_X86_64_NONE            0
#define R_X86_64_64              1
#define R_X86_64_PC32            2
#define R_X86_64_GOT32           3
#define R_X86_64_PLT32           4
#define R_X86_64_COPY            5
#define R_X86_64_GLOB_DAT        6
#define R_X86_64_JUMP_SLOT       7
#define R_X86_64_RELATIVE        8
#define R_X86_64_GOTPCREL        9
#define R_X86_64_32              10
#define R_X86_64_32S             11
#define R_X86_64_16              12
#define R_X86_64_PC16            13
#define R_X86_64_8               14
#define R_X86_64_PC8             15
#define R_X86_64_DTPMOD64        16
#define R_X86_64_DTPOFF64        17
#define R_X86_64_TPOFF64         18
#define R_X86_64_TLSGD           19
#define R_X86_64_TLSLD           20
#define R_X86_64_DTPOFF32        21
#define R_X86_64_GOTTPOFF        22
#define R_X86_64_TPOFF32         23
#define R_X86_64_PC64            24
#define R_X86_64_GOTOFF64        25
#define R_X86_64_GOTPC32         26
#define R_X86_64_GOT64           27
#define R_X86_64_GOTPCREL64      28
#define R_X86_64_GOTPC64         29
#define R_X86_64_GOTPLT64        30
#define R_X86_64_PLTOFF64        31
#define R_X86_64_SIZE32          32
#define R_X86_64_SIZE64          33
#define R_X86_64_GOTPC32_TLSDESC 34
#define R_X86_64_TLSDESC_CALL    35
#define R_X86_64_TLSDESC         36
#define R_X86_64_IRELATIVE       37
#define R_X86_64_RELATIVE64      38
#define R_X86_64_GOTPCRELX       41 /* 6 bytes */
#define R_X86_64_REX_GOTPCRELX   42 /* 7 bytes */
#define R_X86_64_NUM             43

#define R_AARCH64_NONE   0
#define R_AARCH64_ABS64  257
#define R_AARCH64_ABS32  258
#define R_AARCH64_ABS16  259
#define R_AARCH64_PREL64 260
#define R_AARCH64_PREL32 261
#define R_AARCH64_PREL16 262

#define R_PPC_NONE   0
#define R_PPC_ADDR32 1
#define R_PPC_ADDR24 2
#define R_PPC_ADDR16 3
#define R_PPC_REL32  26

#define R_PPC64_NONE   R_PPC_NONE
#define R_PPC64_ADDR32 R_PPC_ADDR32
#define R_PPC64_ADDR24 R_PPC_ADDR24
#define R_PPC64_ADDR16 R_PPC_ADDR16
#define R_PPC64_REL32  R_PPC_REL32

#define R_RISCV_NONE     0
#define R_RISCV_32       1
#define R_RISCV_64       2
#define R_RISCV_RELATIVE 3

#define R_390_NONE 0
#define R_390_8    1
#define R_390_12   2
#define R_390_16   3
#define R_390_32   4
#define R_390_PC32 5

#define STB_LOCAL      0
#define STB_GLOBAL     1
#define STB_WEAK       2
#define STB_NUM        3
#define STB_LOOS       10
#define STB_GNU_UNIQUE 10
#define STB_HIOS       12
#define STB_LOPROC     13
#define STB_HIPROC     15

#define STT_NOTYPE    0
#define STT_OBJECT    1
#define STT_FUNC      2
#define STT_SECTION   3
#define STT_FILE      4
#define STT_COMMON    5
#define STT_TLS       6
#define STT_NUM       7
#define STT_LOOS      10
#define STT_GNU_IFUNC 10
#define STT_HIOS      12
#define STT_LOPROC    13
#define STT_HIPROC    15

#define STV_DEFAULT   0
#define STV_INTERNAL  1
#define STV_HIDDEN    2
#define STV_PROTECTED 3

#define SHN_UNDEF     0
#define SHN_LORESERVE 0xff00
#define SHN_LOPROC    0xff00
#define SHN_BEFORE    0xff00
#define SHN_AFTER     0xff01
#define SHN_HIPROC    0xff1f
#define SHN_LOOS      0xff20
#define SHN_HIOS      0xff3f
#define SHN_ABS       0xfff1
#define SHN_COMMON    0xfff2
#define SHN_XINDEX    0xffff
#define SHN_HIRESERVE 0xffff

#define SHF_WRITE            (1 << 0)
#define SHF_ALLOC            (1 << 1)
#define SHF_EXECINSTR        (1 << 2)
#define SHF_MERGE            (1 << 4)
#define SHF_STRINGS          (1 << 5)
#define SHF_INFO_LINK        (1 << 6)
#define SHF_LINK_ORDER       (1 << 7)
#define SHF_OS_NONCONFORMING (1 << 8)
#define SHF_GROUP            (1 << 9)
#define SHF_TLS              (1 << 10)
#define SHF_COMPRESSED       (1 << 11)
#define SHF_MASKOS           0x0ff00000
#define SHF_MASKPROC         0xf0000000
#define SHF_ORDERED          (1 << 30)
#define SHF_EXCLUDE          (1U << 31)

#define ELFCOMPRESS_ZLIB   1
#define ELFCOMPRESS_LOOS   0x60000000
#define ELFCOMPRESS_HIOS   0x6fffffff
#define ELFCOMPRESS_LOPROC 0x70000000
#define ELFCOMPRESS_HIPROC 0x7fffffff

#define SHT_NULL           0
#define SHT_PROGBITS       1
#define SHT_SYMTAB         2
#define SHT_STRTAB         3
#define SHT_RELA           4
#define SHT_HASH           5
#define SHT_DYNAMIC        6
#define SHT_NOTE           7
#define SHT_NOBITS         8
#define SHT_REL            9
#define SHT_SHLIB          10
#define SHT_DYNSYM         11
#define SHT_INIT_ARRAY     14
#define SHT_FINI_ARRAY     15
#define SHT_PREINIT_ARRAY  16
#define SHT_GROUP          17
#define SHT_SYMTAB_SHNDX   18
#define SHT_NUM            19
#define SHT_LOOS           0x60000000
#define SHT_GNU_ATTRIBUTES 0x6ffffff5
#define SHT_GNU_HASH       0x6ffffff6
#define SHT_GNU_LIBLIST    0x6ffffff7
#define SHT_CHECKSUM       0x6ffffff8
#define SHT_LOSUNW         0x6ffffffa
#define SHT_SUNW_move      0x6ffffffa
#define SHT_SUNW_COMDAT    0x6ffffffb
#define SHT_SUNW_syminfo   0x6ffffffc
#define SHT_GNU_verdef     0x6ffffffd
#define SHT_GNU_verneed    0x6ffffffe
#define SHT_GNU_versym     0x6fffffff
#define SHT_HISUNW         0x6fffffff
#define SHT_HIOS           0x6fffffff
#define SHT_LOPROC         0x70000000
#define SHT_HIPROC         0x7fffffff
#define SHT_LOUSER         0x80000000
#define SHT_HIUSER         0x8fffffff

#define DT_NULL               0
#define DT_NEEDED             1
#define DT_PLTRELSZ           2
#define DT_PLTGOT             3
#define DT_HASH               4
#define DT_STRTAB             5
#define DT_SYMTAB             6
#define DT_RELA               7
#define DT_RELASZ             8
#define DT_RELAENT            9
#define DT_STRSZ              10
#define DT_SYMENT             11
#define DT_INIT               12
#define DT_FINI               13
#define DT_SONAME             14
#define DT_RPATH              15
#define DT_SYMBOLIC           16
#define DT_REL                17
#define DT_RELSZ              18
#define DT_RELENT             19
#define DT_PLTREL             20
#define DT_DEBUG              21
#define DT_TEXTREL            22
#define DT_JMPREL             23
#define DT_BIND_NOW           24
#define DT_INIT_ARRAY         25
#define DT_FINI_ARRAY         26
#define DT_INIT_ARRAYSZ       27
#define DT_FINI_ARRAYSZ       28
#define DT_RUNPATH            29
#define DT_FLAGS              30
#define DT_ENCODING           32
#define DT_PREINIT_ARRAY      32
#define DT_PREINIT_ARRAYSZ    33
#define DT_SYMTAB_SHNDX       34
#define DT_NUM                35
#define DT_LOOS               0x6000000d
#define DT_HIOS               0x6ffff000
#define DT_LOPROC             0x70000000
#define DT_HIPROC             0x7fffffff
#define DT_VALRNGLO           0x6ffffd00
#define DT_GNU_PRELINKED      0x6ffffdf5
#define DT_GNU_CONFLICTSZ     0x6ffffdf6
#define DT_GNU_LIBLISTSZ      0x6ffffdf7
#define DT_CHECKSUM           0x6ffffdf8
#define DT_PLTPADSZ           0x6ffffdf9
#define DT_MOVEENT            0x6ffffdfa
#define DT_MOVESZ             0x6ffffdfb
#define DT_FEATURE_1          0x6ffffdfc
#define DT_POSFLAG_1          0x6ffffdfd
#define DT_SYMINSZ            0x6ffffdfe
#define DT_SYMINENT           0x6ffffdff
#define DT_VALRNGHI           0x6ffffdff
#define DT_VALTAGIDX(tag)     (DT_VALRNGHI - (tag))
#define DT_VALNUM             12
#define DT_ADDRRNGLO          0x6ffffe00
#define DT_GNU_HASH           0x6ffffef5
#define DT_TLSDESC_PLT        0x6ffffef6
#define DT_TLSDESC_GOT        0x6ffffef7
#define DT_GNU_CONFLICT       0x6ffffef8
#define DT_GNU_LIBLIST        0x6ffffef9
#define DT_CONFIG             0x6ffffefa
#define DT_DEPAUDIT           0x6ffffefb
#define DT_AUDIT              0x6ffffefc
#define DT_PLTPAD             0x6ffffefd
#define DT_MOVETAB            0x6ffffefe
#define DT_SYMINFO            0x6ffffeff
#define DT_ADDRRNGHI          0x6ffffeff
#define DT_ADDRTAGIDX(tag)    (DT_ADDRRNGHI - (tag))
#define DT_ADDRNUM            11
#define DT_VERSYM             0x6ffffff0
#define DT_RELACOUNT          0x6ffffff9
#define DT_RELCOUNT           0x6ffffffa
#define DT_FLAGS_1            0x6ffffffb
#define DT_VERDEF             0x6ffffffc
#define DT_VERDEFNUM          0x6ffffffd
#define DT_VERNEED            0x6ffffffe
#define DT_VERNEEDNUM         0x6fffffff
#define DT_VERSIONTAGIDX(tag) (DT_VERNEEDNUM - (tag))
#define DT_VERSIONTAGNUM      16
#define DT_AUXILIARY          0x7ffffffd
#define DT_FILTER             0x7fffffff
#define DT_EXTRATAGIDX(tag)   ((Elf32_Word) - ((Elf32_Sword)(tag) << 1 >> 1) - 1)
#define DT_EXTRANUM           3

#define VER_NEED_NONE    0
#define VER_NEED_CURRENT 1
#define VER_NEED_NUM     2
#define VER_FLG_WEAK     0x2

#define ELF_NOTE_SOLARIS       "SUNW Solaris"
#define ELF_NOTE_GNU           "GNU"
#define ELF_NOTE_PAGESIZE_HINT 1
#define ELF_NOTE_ABI           NT_GNU_ABI_TAG
#define ELF_NOTE_OS_LINUX      0
#define ELF_NOTE_OS_GNU        1
#define ELF_NOTE_OS_SOLARIS2   2
#define ELF_NOTE_OS_FREEBSD    3

#define NT_GNU_ABI_TAG      1
#define NT_GNU_BUILD_ID     3
#define NT_GNU_GOLD_VERSION 4

#define EF_CPU32 0x00810000

#define DF_ORIGIN       0x00000001
#define DF_SYMBOLIC     0x00000002
#define DF_TEXTREL      0x00000004
#define DF_BIND_NOW     0x00000008
#define DF_STATIC_TLS   0x00000010
#define DF_1_NOW        0x00000001
#define DF_1_GLOBAL     0x00000002
#define DF_1_GROUP      0x00000004
#define DF_1_NODELETE   0x00000008
#define DF_1_LOADFLTR   0x00000010
#define DF_1_INITFIRST  0x00000020
#define DF_1_NOOPEN     0x00000040
#define DF_1_ORIGIN     0x00000080
#define DF_1_DIRECT     0x00000100
#define DF_1_TRANS      0x00000200
#define DF_1_INTERPOSE  0x00000400
#define DF_1_NODEFLIB   0x00000800
#define DF_1_NODUMP     0x00001000
#define DF_1_CONFALT    0x00002000
#define DF_1_ENDFILTEE  0x00004000
#define DF_1_DISPRELDNE 0x00008000
#define DF_1_DISPRELPND 0x00010000
#define DF_1_NODIRECT   0x00020000
#define DF_1_IGNMULDEF  0x00040000
#define DF_1_NOKSYMS    0x00080000
#define DF_1_NOHDR      0x00100000
#define DF_1_EDITED     0x00200000
#define DF_1_NORELOC    0x00400000
#define DF_1_SYMINTPOSE 0x00800000
#define DF_1_GLOBAUDIT  0x01000000
#define DF_1_SINGLETON  0x02000000
#define DF_1_STUB       0x04000000
#define DF_1_PIE        0x08000000
#define DTF_1_PARINIT   0x00000001
#define DTF_1_CONFEXP   0x00000002
#define DF_P1_LAZYLOAD  0x00000001
#define DF_P1_GROUPPERM 0x00000002

#define ELF64_ST_BIND(val)        (((unsigned char)(val)) >> 4)
#define ELF64_ST_TYPE(val)        ((val)&0xf)
#define ELF64_ST_INFO(bind, type) (((bind) << 4) + ((type)&0xf))
#define ELF64_ST_VISIBILITY(o)    ((o)&0x03)

#define ELF64_R_SYM(i)          ((i) >> 32)
#define ELF64_R_TYPE(i)         ((i)&0xffffffff)
#define ELF64_R_INFO(sym, type) ((((Elf64_Xword)(sym)) << 32) + (type))

#define ELF64_M_SYM(info)       ((info) >> 8)
#define ELF64_M_SIZE(info)      ((unsigned char)(info))
#define ELF64_M_INFO(sym, size) (((sym) << 8) + (unsigned char)(size))

#define NT_PRSTATUS         1
#define NT_PRFPREG          2
#define NT_FPREGSET         2
#define NT_PRPSINFO         3
#define NT_PRXREG           4
#define NT_TASKSTRUCT       4
#define NT_PLATFORM         5
#define NT_AUXV             6
#define NT_GWINDOWS         7
#define NT_ASRS             8
#define NT_PSTATUS          10
#define NT_PSINFO           13
#define NT_PRCRED           14
#define NT_UTSNAME          15
#define NT_LWPSTATUS        16
#define NT_LWPSINFO         17
#define NT_PRFPXREG         20
#define NT_SIGINFO          0x53494749
#define NT_FILE             0x46494c45
#define NT_PRXFPREG         0x46e62b7f
#define NT_PPC_VMX          0x100
#define NT_PPC_SPE          0x101
#define NT_PPC_VSX          0x102
#define NT_PPC_TAR          0x103
#define NT_PPC_PPR          0x104
#define NT_PPC_DSCR         0x105
#define NT_PPC_EBB          0x106
#define NT_PPC_PMU          0x107
#define NT_PPC_TM_CGPR      0x108
#define NT_PPC_TM_CFPR      0x109
#define NT_PPC_TM_CVMX      0x10a
#define NT_PPC_TM_CVSX      0x10b
#define NT_PPC_TM_SPR       0x10c
#define NT_PPC_TM_CTAR      0x10d
#define NT_PPC_TM_CPPR      0x10e
#define NT_PPC_TM_CDSCR     0x10f
#define NT_X86_XSTATE       0x202
#define NT_S390_HIGH_GPRS   0x300
#define NT_S390_TIMER       0x301
#define NT_S390_TODCMP      0x302
#define NT_S390_TODPREG     0x303
#define NT_S390_CTRS        0x304
#define NT_S390_PREFIX      0x305
#define NT_S390_LAST_BREAK  0x306
#define NT_S390_SYSTEM_CALL 0x307
#define NT_S390_TDB         0x308
#define NT_S390_VXRS_LOW    0x309
#define NT_S390_VXRS_HIGH   0x30a
#define NT_S390_GS_CB       0x30b
#define NT_S390_GS_BC       0x30c
#define NT_S390_RI_CB       0x30d
#define NT_ARM_VFP          0x400
#define NT_ARM_TLS          0x401
#define NT_ARM_HW_BREAK     0x402
#define NT_ARM_HW_WATCH     0x403
#define NT_ARM_SYSTEM_CALL  0x404
#define NT_ARM_SVE          0x405
#define NT_ARM_PAC_MASK     0x406
#define NT_METAG_CBUF       0x500
#define NT_METAG_RPIPE      0x501
#define NT_METAG_TLS        0x502
#define NT_ARC_V2           0x600
#define NT_VMCOREDD         0x700
#define NT_VERSION          1

#define VER_DEF_NONE      0
#define VER_DEF_CURRENT   1
#define VER_DEF_NUM       2
#define VER_FLG_BASE      0x1
#define VER_FLG_WEAK      0x2
#define VER_NDX_LOCAL     0
#define VER_NDX_GLOBAL    1
#define VER_NDX_LORESERVE 0xff00
#define VER_NDX_ELIMINATE 0xff01

#define LL_NONE           0
#define LL_EXACT_MATCH    (1 << 0)
#define LL_IGNORE_INT_VER (1 << 1)
#define LL_REQUIRE_MINOR  (1 << 2)
#define LL_EXPORTS        (1 << 3)
#define LL_DELAY_LOAD     (1 << 4)
#define LL_DELTA          (1 << 5)

#define R_BPF_NONE   0
#define R_BPF_MAP_FD 1

#endif /* COSMOPOLITAN_LIBC_ELF_STRUCT_DEF_H_ */
