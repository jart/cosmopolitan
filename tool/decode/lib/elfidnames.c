/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/elf/def.h"
#include "libc/elf/elf.h"
#include "tool/decode/lib/elfidnames.h"

const struct IdName kElfTypeNames[] = {
    {ET_NONE, "ET_NONE"},
    {ET_REL, "ET_REL"},
    {ET_EXEC, "ET_EXEC"},
    {ET_DYN, "ET_DYN"},
    {ET_CORE, "ET_CORE"},
    {ET_NUM, "ET_NUM"},
    {ET_LOOS, "ET_LOOS"},
    {ET_HIOS, "ET_HIOS"},
    {ET_LOPROC, "ET_LOPROC"},
    {ET_HIPROC, "ET_HIPROC"},
    {0, 0},
};

const struct IdName kElfOsabiNames[] = {
    {ELFOSABI_NONE, "ELFOSABI_NONE"},
    {ELFOSABI_SYSV, "ELFOSABI_SYSV"},
    {ELFOSABI_HPUX, "ELFOSABI_HPUX"},
    {ELFOSABI_NETBSD, "ELFOSABI_NETBSD"},
    {ELFOSABI_GNU, "ELFOSABI_GNU"},
    {ELFOSABI_LINUX, "ELFOSABI_LINUX"},
    {ELFOSABI_SOLARIS, "ELFOSABI_SOLARIS"},
    {ELFOSABI_AIX, "ELFOSABI_AIX"},
    {ELFOSABI_IRIX, "ELFOSABI_IRIX"},
    {ELFOSABI_FREEBSD, "ELFOSABI_FREEBSD"},
    {ELFOSABI_TRU64, "ELFOSABI_TRU64"},
    {ELFOSABI_MODESTO, "ELFOSABI_MODESTO"},
    {ELFOSABI_OPENBSD, "ELFOSABI_OPENBSD"},
    {ELFOSABI_ARM, "ELFOSABI_ARM"},
    {ELFOSABI_STANDALONE, "ELFOSABI_STANDALONE"},
    {0, 0},
};

const struct IdName kElfClassNames[] = {
    {ELFCLASSNONE, "ELFCLASSNONE"},
    {ELFCLASS32, "ELFCLASS32"},
    {ELFCLASS64, "ELFCLASS64"},
    {0, 0},
};

const struct IdName kElfDataNames[] = {
    {ELFDATANONE, "ELFDATANONE"},
    {ELFDATA2LSB, "ELFDATA2LSB"},
    {ELFDATA2MSB, "ELFDATA2MSB"},
    {0, 0},
};

const struct IdName kElfMachineNames[] = {
    {EM_M32, "EM_M32"},
    {EM_386, "EM_386"},
    {EM_S390, "EM_S390"},
    {EM_ARM, "EM_ARM"},
    {EM_NEXGEN32E, "EM_NEXGEN32E"},
    {EM_PDP11, "EM_PDP11"},
    {EM_CRAYNV2, "EM_CRAYNV2"},
    {EM_L10M, "EM_L10M"},
    {EM_K10M, "EM_K10M"},
    {EM_AARCH64, "EM_AARCH64"},
    {EM_CUDA, "EM_CUDA"},
    {EM_Z80, "EM_Z80"},
    {EM_RISCV, "EM_RISCV"},
    {EM_BPF, "EM_BPF"},
    {0, 0},
};

const struct IdName kElfSegmentTypeNames[] = {
    {PT_NULL, "PT_NULL"},       /* Program header table entry unused */
    {PT_LOAD, "PT_LOAD"},       /* Loadable program segment */
    {PT_DYNAMIC, "PT_DYNAMIC"}, /* Dynamic linking information */
    {PT_INTERP, "PT_INTERP"},   /* Program interpreter */
    {PT_NOTE, "PT_NOTE"},       /* Auxiliary information */
    {PT_SHLIB, "PT_SHLIB"},     /* Reserved */
    {PT_PHDR, "PT_PHDR"},       /* Entry for header table itself */
    {PT_TLS, "PT_TLS"},         /* Thread-local storage segment */
    {PT_NUM, "PT_NUM"},         /* Number of defined types */
    {PT_LOOS, "PT_LOOS"},       /* Start of OS-specific */
    {PT_GNU_EH_FRAME, "PT_GNU_EH_FRAME"}, /* GCC .eh_frame_hdr segment */
    {PT_GNU_STACK, "PT_GNU_STACK"},       /* Indicates stack executability */
    {PT_GNU_RELRO, "PT_GNU_RELRO"},       /* Read-only after relocation */
    {PT_LOSUNW, "PT_LOSUNW"},       /* <Reserved for Sun Micrososystems> */
    {PT_SUNWBSS, "PT_SUNWBSS"},     /* Sun Specific segment */
    {PT_SUNWSTACK, "PT_SUNWSTACK"}, /* Stack segment */
    {PT_HISUNW, "PT_HISUNW"},       /* </Reserved for Sun Micrososystems> */
    {PT_HIOS, "PT_HIOS"},           /* End of OS-specific */
    {PT_LOPROC, "PT_LOPROC"},       /* Start of processor-specific */
    {PT_HIPROC, "PT_HIPROC"},       /* End of processor-specific */
    {0, 0},
};

const struct IdName kElfSectionTypeNames[] = {
    {SHT_NULL, "SHT_NULL"},
    {SHT_PROGBITS, "SHT_PROGBITS"},
    {SHT_SYMTAB, "SHT_SYMTAB"},
    {SHT_STRTAB, "SHT_STRTAB"},
    {SHT_RELA, "SHT_RELA"},
    {SHT_HASH, "SHT_HASH"},
    {SHT_DYNAMIC, "SHT_DYNAMIC"},
    {SHT_NOTE, "SHT_NOTE"},
    {SHT_NOBITS, "SHT_NOBITS"},
    {SHT_REL, "SHT_REL"},
    {SHT_SHLIB, "SHT_SHLIB"},
    {SHT_DYNSYM, "SHT_DYNSYM"},
    {SHT_INIT_ARRAY, "SHT_INIT_ARRAY"},
    {SHT_FINI_ARRAY, "SHT_FINI_ARRAY"},
    {SHT_PREINIT_ARRAY, "SHT_PREINIT_ARRAY"},
    {SHT_GROUP, "SHT_GROUP"},
    {SHT_SYMTAB_SHNDX, "SHT_SYMTAB_SHNDX"},
    {SHT_NUM, "SHT_NUM"},
    {SHT_LOOS, "SHT_LOOS"},
    {SHT_GNU_ATTRIBUTES, "SHT_GNU_ATTRIBUTES"},
    {SHT_GNU_HASH, "SHT_GNU_HASH"},
    {SHT_GNU_LIBLIST, "SHT_GNU_LIBLIST"},
    {SHT_CHECKSUM, "SHT_CHECKSUM"},
    {SHT_LOSUNW, "SHT_LOSUNW"},
    {SHT_SUNW_move, "SHT_SUNW_move"},
    {SHT_SUNW_COMDAT, "SHT_SUNW_COMDAT"},
    {SHT_SUNW_syminfo, "SHT_SUNW_syminfo"},
    {SHT_GNU_verdef, "SHT_GNU_verdef"},
    {SHT_GNU_verneed, "SHT_GNU_verneed"},
    {SHT_GNU_versym, "SHT_GNU_versym"},
    {SHT_HISUNW, "SHT_HISUNW"},
    {SHT_HIOS, "SHT_HIOS"},
    {SHT_LOPROC, "SHT_LOPROC"},
    {SHT_HIPROC, "SHT_HIPROC"},
    {SHT_LOUSER, "SHT_LOUSER"},
    {SHT_HIUSER, "SHT_HIUSER"},
    {0, 0},
};

const struct IdName kElfSegmentFlagNames[] = {
    {PF_X, "PF_X"},
    {PF_W, "PF_W"},
    {PF_R, "PF_R"},
    {PF_MASKOS, "PF_MASKOS"},
    {PF_MASKPROC, "PF_MASKPROC"},
    {0, 0},
};

const struct IdName kElfSectionFlagNames[] = {
    {SHF_WRITE, "SHF_WRITE"},
    {SHF_ALLOC, "SHF_ALLOC"},
    {SHF_EXECINSTR, "SHF_EXECINSTR"},
    {SHF_MERGE, "SHF_MERGE"},
    {SHF_STRINGS, "SHF_STRINGS"},
    {SHF_INFO_LINK, "SHF_INFO_LINK"},
    {SHF_LINK_ORDER, "SHF_LINK_ORDER"},
    {SHF_OS_NONCONFORMING, "SHF_OS_NONCONFORMING"},
    {SHF_GROUP, "SHF_GROUP"},
    {SHF_TLS, "SHF_TLS"},
    {SHF_COMPRESSED, "SHF_COMPRESSED"},
    {SHF_MASKOS, "SHF_MASKOS"},
    {SHF_MASKPROC, "SHF_MASKPROC"},
    {SHF_ORDERED, "SHF_ORDERED"},
    {SHF_EXCLUDE, "SHF_EXCLUDE"},
    {0, 0},
};

const struct IdName kElfSymbolTypeNames[] = {
    {STT_NOTYPE, "STT_NOTYPE"}, {STT_OBJECT, "STT_OBJECT"},
    {STT_FUNC, "STT_FUNC"},     {STT_SECTION, "STT_SECTION"},
    {STT_FILE, "STT_FILE"},     {STT_COMMON, "STT_COMMON"},
    {STT_TLS, "STT_TLS"},       {STT_NUM, "STT_NUM"},
    {STT_LOOS, "STT_LOOS"},     {STT_GNU_IFUNC, "STT_GNU_IFUNC"},
    {STT_HIOS, "STT_HIOS"},     {STT_LOPROC, "STT_LOPROC"},
    {STT_HIPROC, "STT_HIPROC"}, {0, 0},
};

const struct IdName kElfSymbolBindNames[] = {
    {STB_LOCAL, "STB_LOCAL"},   {STB_GLOBAL, "STB_GLOBAL"},
    {STB_WEAK, "STB_WEAK"},     {STB_NUM, "STB_NUM"},
    {STB_LOOS, "STB_LOOS"},     {STB_GNU_UNIQUE, "STB_GNU_UNIQUE"},
    {STB_HIOS, "STB_HIOS"},     {STB_LOPROC, "STB_LOPROC"},
    {STB_HIPROC, "STB_HIPROC"}, {0, 0},
};

const struct IdName kElfSymbolVisibilityNames[] = {
    {STV_DEFAULT, "STV_DEFAULT"},
    {STV_INTERNAL, "STV_INTERNAL"},
    {STV_HIDDEN, "STV_HIDDEN"},
    {STV_PROTECTED, "STV_PROTECTED"},
    {0, 0},
};

const struct IdName kElfSpecialSectionNames[] = {
    {SHN_UNDEF, "SHN_UNDEF"},
    {SHN_LORESERVE, "SHN_LORESERVE"},
    {SHN_LOPROC, "SHN_LOPROC"},
    {SHN_BEFORE, "SHN_BEFORE"},
    {SHN_AFTER, "SHN_AFTER"},
    {SHN_HIPROC, "SHN_HIPROC"},
    {SHN_LOOS, "SHN_LOOS"},
    {SHN_HIOS, "SHN_HIOS"},
    {SHN_ABS, "SHN_ABS"},
    {SHN_COMMON, "SHN_COMMON"},
    {SHN_XINDEX, "SHN_XINDEX"},
    {SHN_HIRESERVE, "SHN_HIRESERVE"},
    {0, 0},
};

const struct IdName kElfNexgen32eRelocationNames[] = {
    {R_X86_64_NONE, "NONE"},
    {R_X86_64_64, "64"},
    {R_X86_64_PC32, "PC32"},
    {R_X86_64_GOT32, "GOT32"},
    {R_X86_64_PLT32, "PLT32"},
    {R_X86_64_COPY, "COPY"},
    {R_X86_64_GLOB_DAT, "GLOB_DAT"},
    {R_X86_64_JUMP_SLOT, "JUMP_SLOT"},
    {R_X86_64_RELATIVE, "RELATIVE"},
    {R_X86_64_GOTPCREL, "GOTPCREL"},
    {R_X86_64_32, "32"},
    {R_X86_64_32S, "32S"},
    {R_X86_64_16, "16"},
    {R_X86_64_PC16, "PC16"},
    {R_X86_64_8, "8"},
    {R_X86_64_PC8, "PC8"},
    {R_X86_64_DTPMOD64, "DTPMOD64"},
    {R_X86_64_DTPOFF64, "DTPOFF64"},
    {R_X86_64_TPOFF64, "TPOFF64"},
    {R_X86_64_TLSGD, "TLSGD"},
    {R_X86_64_TLSLD, "TLSLD"},
    {R_X86_64_DTPOFF32, "DTPOFF32"},
    {R_X86_64_GOTTPOFF, "GOTTPOFF"},
    {R_X86_64_TPOFF32, "TPOFF32"},
    {R_X86_64_PC64, "PC64"},
    {R_X86_64_GOTOFF64, "GOTOFF64"},
    {R_X86_64_GOTPC32, "GOTPC32"},
    {R_X86_64_GOT64, "GOT64"},
    {R_X86_64_GOTPCREL64, "GOTPCREL64"},
    {R_X86_64_GOTPC64, "GOTPC64"},
    {R_X86_64_GOTPLT64, "GOTPLT64"},
    {R_X86_64_PLTOFF64, "PLTOFF64"},
    {R_X86_64_SIZE32, "SIZE32"},
    {R_X86_64_SIZE64, "SIZE64"},
    {R_X86_64_GOTPC32_TLSDESC, "GOTPC32_TLSDESC"},
    {R_X86_64_TLSDESC_CALL, "TLSDESC_CALL"},
    {R_X86_64_TLSDESC, "TLSDESC"},
    {R_X86_64_IRELATIVE, "IRELATIVE"},
    {R_X86_64_RELATIVE64, "RELATIVE64"},
    {R_X86_64_GOTPCRELX, "GOTPCRELX"},
    {R_X86_64_REX_GOTPCRELX, "REX_GOTPCRELX"},
    {0, 0},
};
