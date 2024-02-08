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
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/libgen.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/mem/gc.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/nt/struct/imagedosheader.internal.h"
#include "libc/nt/struct/imagentheaders.internal.h"
#include "libc/nt/struct/imageoptionalheader.internal.h"
#include "libc/nt/struct/imagesectionheader.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/serialize.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/x/xasprintf.h"
#include "third_party/xed/x86.h"
#include "tool/decode/lib/asmcodegen.h"
#include "tool/decode/lib/flagger.h"
#include "tool/decode/lib/peidnames.h"
#include "tool/decode/lib/titlegen.h"

/**
 * @fileoverview Portable executable metadata disassembler.
 * @see https://www.aldeid.com/wiki/PE-Portable-executable
 */

static const char *path;

static struct NtImageDosHeader *mz;
static size_t mzsize;

static struct NtImageSectionHeader *sections;
static size_t section_count;

static void *GetOff(uint32_t off) {
  if (off < mzsize) return (char *)mz + off;
  fprintf(stderr, "%s: off %#x not defined within image\n", path, off);
  exit(1);
}

static void *GetRva(uint32_t rva) {
  int i;
  for (i = 0; i < section_count; ++i) {
    if (sections[i].VirtualAddress <= rva &&
        rva < sections[i].VirtualAddress + sections[i].Misc.VirtualSize) {
      return (char *)mz + sections[i].PointerToRawData +
             (rva - sections[i].VirtualAddress);
    }
  }
  fprintf(stderr, "%s: rva %#x not defined by any sections\n", path, rva);
  exit(1);
}

static struct XedDecodedInst *ildreal(void *addr) {
  static struct XedDecodedInst xedd;
  if (xed_instruction_length_decode(
          xed_decoded_inst_zero_set_mode(&xedd, XED_MACHINE_MODE_REAL), addr,
          XED_MAX_INSTRUCTION_BYTES) != XED_ERROR_NONE ||
      !xedd.length) {
    xedd.length = 1;
  }
  return &xedd;
}

static void startfile(void) {
  showtitle("αcτµαlly pδrταblε εxεcµταblε", "tool/decode/pe", NULL, NULL,
            &kModelineAsm);
  printf("#include \"libc/nt/pedef.internal.h\"\n\n", path);
}

static void *pecheckaddress(struct NtImageDosHeader *mz, size_t mzsize,
                            void *addr, uint32_t addrsize) {
  if ((intptr_t)addr < (intptr_t)mz ||
      (intptr_t)addr + addrsize > (intptr_t)mz + mzsize) {
    abort();
  }
  return addr;
}

static void showmzheader(void) {
  showtitle(basename(gc(strdup(path))), "dos", "mz header",
            "\tMZ = Mark 'Zibo' Joseph Zbikowski\n"
            "\te_cblp: bytes on last page\n"
            "\te_cp: 512-byte pages in file\n"
            "\te_crlc: reloc table entry count\n"
            "\te_cparhdr: data segment file offset / 16\n"
            "\te_{min,max}alloc: lowers upper bound load / 16\n"
            "\te_ss: lower bound on stack segment\n"
            "\te_sp: initialize stack pointer\n"
            "\te_csum: ∑bₙ checksum don't bother\n"
            "\te_ip: initial ip value\n"
            "\te_cs: increases cs load lower bound\n"
            "\te_lfarlc: reloc table offset\n"
            "\te_ovno: overlay number\n"
            "\te_lfanew: portable executable header rva",
            NULL);
  printf("\n");
  show(".ascii", format(b1, "%`'.*s", 2, (const char *)&mz->e_magic),
       "mz->e_magic");
  showshorthex(mz->e_cblp);
  showshorthex(mz->e_cp);
  showshorthex(mz->e_crlc);
  showshorthex(mz->e_cparhdr);
  showshorthex(mz->e_minalloc);
  showshorthex(mz->e_maxalloc);
  showshorthex(mz->e_ss);
  showshorthex(mz->e_sp);
  showshorthex(mz->e_csum);
  showshorthex(mz->e_ip);
  showshorthex(mz->e_cs);
  showshorthex(mz->e_lfarlc);
  showshorthex(mz->e_ovno);
  show(".short",
       format(b1, "%hu,%hu,%hu,%hu", mz->e_res[0], mz->e_res[1], mz->e_res[2],
              mz->e_res[3]),
       "mz->e_res");
  showshorthex(mz->e_oemid);
  showshorthex(mz->e_oeminfo);
  show(".short",
       format(b1, "%hu,%hu,%hu,%hu,%hu,%hu,%hu,%hu,%hu,%hu", mz->e_res2[0],
              mz->e_res2[1], mz->e_res2[2], mz->e_res2[3], mz->e_res2[4],
              mz->e_res2[5], mz->e_res2[6], mz->e_res2[7], mz->e_res2[8],
              mz->e_res2[9]),
       "mz->e_res2");
  showinthex(mz->e_lfanew);
  printf("\n");
}

static void showdosstub(void) {
}

static void showpeoptionalheader(struct NtImageOptionalHeader *opt) {
  int i;
  showtitle(basename(gc(strdup(path))), "windows", "pe \"optional\" header",
            NULL, NULL);
  printf("\n");
  show(".short",
       firstnonnull(findnamebyid(kNtPeOptionalHeaderMagicNames, opt->Magic),
                    format(b1, "%#hx", opt->Magic)),
       "opt->Magic");
  showint(opt->MajorLinkerVersion);
  showint(opt->MinorLinkerVersion);
  showinthex(opt->SizeOfCode);
  showinthex(opt->SizeOfInitializedData);
  showinthex(opt->SizeOfUninitializedData);
  showinthex(opt->AddressOfEntryPoint);
  showinthex(opt->BaseOfCode);
  showint64hex(opt->ImageBase);
  showinthex(opt->SectionAlignment);
  showinthex(opt->FileAlignment);
  showshort(opt->MajorOperatingSystemVersion);
  showshort(opt->MinorOperatingSystemVersion);
  showshort(opt->MajorImageVersion);
  showshort(opt->MinorImageVersion);
  showshort(opt->MajorSubsystemVersion);
  showshort(opt->MinorSubsystemVersion);
  showint(opt->Win32VersionValue);
  showinthex(opt->SizeOfImage);
  showinthex(opt->SizeOfHeaders);
  showinthex(opt->CheckSum);
  show(".short",
       firstnonnull(findnamebyid(kNtImageSubsystemNames, opt->Subsystem),
                    format(b1, "%#hx", opt->Subsystem)),
       "opt->Subsystem");
  show(".short",
       firstnonnull(RecreateFlags(kNtImageDllcharacteristicNames,
                                  opt->DllCharacteristics),
                    format(b1, "%#hx", opt->DllCharacteristics)),
       "opt->DllCharacteristics");
  showint64hex(opt->SizeOfStackReserve);
  showint64hex(opt->SizeOfStackCommit);
  showint64hex(opt->SizeOfHeapReserve);
  showint64hex(opt->SizeOfHeapCommit);
  showinthex(opt->LoaderFlags);
  showinthex(opt->NumberOfRvaAndSizes);

  i = 0;
#define DATA_DIRECTORY(x)                                             \
  do {                                                                \
    if (i++ < opt->NumberOfRvaAndSizes) {                             \
      show(".long",                                                   \
           format(b1, "%#X,%u", opt->DataDirectory[x].VirtualAddress, \
                  opt->DataDirectory[x].Size),                        \
           gc(xasprintf("opt->DataDirectory[%s]", #x)));              \
    }                                                                 \
  } while (0)

  DATA_DIRECTORY(kNtImageDirectoryEntryExport);
  DATA_DIRECTORY(kNtImageDirectoryEntryImport);
  DATA_DIRECTORY(kNtImageDirectoryEntryResource);
  DATA_DIRECTORY(kNtImageDirectoryEntryException);
  DATA_DIRECTORY(kNtImageDirectoryEntrySecurity);
  DATA_DIRECTORY(kNtImageDirectoryEntryBasereloc);
  DATA_DIRECTORY(kNtImageDirectoryEntryDebug);
  DATA_DIRECTORY(kNtImageDirectoryEntryArchitecture);
  DATA_DIRECTORY(kNtImageDirectoryEntryGlobalptr);
  DATA_DIRECTORY(kNtImageDirectoryEntryTls);
  DATA_DIRECTORY(kNtImageDirectoryEntryLoadConfig);
  DATA_DIRECTORY(kNtImageDirectoryEntryBoundImport);
  DATA_DIRECTORY(kNtImageDirectoryEntryIat);
  DATA_DIRECTORY(kNtImageDirectoryEntryDelayImport);
  DATA_DIRECTORY(kNtImageDirectoryEntryComDescriptor);

  for (; i < opt->NumberOfRvaAndSizes; ++i) {
    showint64hex(opt->DataDirectory[i]);
  }
}

static void ShowIlt(uint32_t rva) {
  int64_t *ilt, *ilt0;
  ilt = ilt0 = GetRva(rva);
  do {
    printf("\n");
    show(".quad", format(b1, "%#lx", *ilt),
         gc(xasprintf("rva=%#lx off=%#lx", (char *)ilt - (char *)ilt0 + rva,
                      (intptr_t)ilt - (intptr_t)mz)));
    if (*ilt) {
      char *hint = GetRva(*ilt);
      printf("/\t.short\t%d\t\t\t# @%#lx\n", READ16LE(hint),
             (intptr_t)hint - (intptr_t)mz);
      char *name = GetRva(*ilt + 2);
      printf("/\t.asciz\t%`'s\n", name);
      printf("/\t.align\t2\n");
    }
  } while (*ilt++);
}

static void ShowIdt(char *idt, size_t size) {
  char *p, *e;
  printf("\n");
  showtitle(basename(gc(strdup(path))), "windows",
            "import descriptor table (idt)", 0, 0);
  for (p = idt, e = idt + size; p + 20 <= e; p += 20) {
    printf("\n");
    show(".long", format(b1, "%#x", READ32LE(p)),
         gc(xasprintf("ImportLookupTable RVA @%#lx",
                      (intptr_t)p - (intptr_t)mz)));
    show(".long", format(b1, "%#x", READ32LE(p + 4)), "TimeDateStamp");
    show(".long", format(b1, "%#x", READ32LE(p + 8)), "ForwarderChain");
    show(".long", format(b1, "%#x", READ32LE(p + 12)),
         READ32LE(p + 12)
             ? gc(xasprintf("DllName RVA (%s)", GetRva(READ32LE(p + 12))))
             : "DllName RVA");
    show(".long", format(b1, "%#x", READ32LE(p + 16)),
         "ImportAddressTable RVA");
  }
  for (p = idt, e = idt + size; p + 20 <= e; p += 20) {
    if (READ32LE(p)) {
      printf("\n");
      showtitle(basename(gc(strdup(path))), "windows",
                "import lookup table (ilt)", 0, 0);
      ShowIlt(READ32LE(p));
    }
  }
  for (p = idt, e = idt + size; p + 20 <= e; p += 20) {
    if (READ32LE(p)) {
      printf("\n");
      showtitle(basename(gc(strdup(path))), "windows",
                "import address table (iat)", 0, 0);
      ShowIlt(READ32LE(p + 16));
    }
  }
}

static void ShowSection(struct NtImageSectionHeader *s) {
  char name[9] = {0};
  memcpy(name, s->Name, 8);
  printf("\n");
  printf("\t.ascin\t\"%'s\",8\n", name);
  printf("\t.long\t%#x\t\t# VirtualSize\n", s->Misc.VirtualSize);
  printf("\t.long\t%#x\t\t# VirtualAddress\n", s->VirtualAddress);
  printf("\t.long\t%#x\t\t# SizeOfRawData\n", s->SizeOfRawData);
  printf("\t.long\t%#x\t\t# PointerToRawData\n", s->PointerToRawData);
  printf("\t.long\t%#x\t\t# PointerToRelocations\n", s->PointerToRelocations);
  printf("\t.long\t%#x\t\t# PointerToLinenumbers\n", s->PointerToLinenumbers);
  printf("\t.short\t%#x\t\t# NumberOfRelocations\n", s->NumberOfRelocations);
  printf("\t.short\t%#x\t\t# NumberOfLinenumbers\n", s->NumberOfLinenumbers);

  printf("\
//	          ┌31:Writeable                        ┌─────────────────────────┐\n\
//	          │┌30:Readable                        │ PE Section Flags        │\n\
//	          ││┌29:Executable                     ├─────────────────────────┤\n\
//	          │││┌28:Shareable                     │ o │ for object files    │\n\
//	          ││││┌27:Unpageable                   │ r │ reserved            │\n\
//	          │││││┌26:Uncacheable                 └───┴─────────────────────┘\n\
//	          ││││││┌25:Discardable\n\
//	          │││││││┌24:Contains Extended Relocations\n\
//	          ││││││││        ┌15:Contains Global Pointer (GP) Relative Data\n\
//	          ││││││││        │       ┌7:Contains Uninitialized Data\n\
//	          ││││││││        │       │┌6:Contains Initialized Data\n\
//	          ││││││││ o      │       ││┌5:Contains Code\n\
//	          ││││││││┌┴─┐rrrr│  ooror│││rorrr\n\
\t.long\t0b%.32b\t\t# Characteristics\n",
         s->Characteristics);
}

static void ShowSections(struct NtImageSectionHeader *s, size_t n) {
  size_t i;
  sections = s;
  section_count = n;
  printf("\n");
  showtitle(basename(gc(strdup(path))), "windows", "sections", 0, 0);
  for (i = 0; i < n; ++i) {
    ShowSection(s + i);
  }
}

static void showpeheader(struct NtImageNtHeaders *pe) {
  showtitle(basename(gc(strdup(path))), "windows", "pe header", NULL, NULL);
  printf("\n");
  showorg(mz->e_lfanew);
  show(".ascii", format(b1, "%`'.*s", 4, (const char *)&pe->Signature),
       "pe->Signature");
  show(".short",
       firstnonnull(
           findnamebyid(kNtImageFileMachineNames, pe->FileHeader.Machine),
           format(b1, "%#hx", pe->FileHeader.Machine)),
       "pe->FileHeader.Machine");
  showshort(pe->FileHeader.NumberOfSections);
  showinthex(pe->FileHeader.TimeDateStamp);
  showinthex(pe->FileHeader.PointerToSymbolTable);
  showint(pe->FileHeader.NumberOfSymbols);
  showshort(pe->FileHeader.SizeOfOptionalHeader);
  show(".short",
       firstnonnull(RecreateFlags(kNtImageCharacteristicNames,
                                  pe->FileHeader.Characteristics),
                    format(b1, "%#hx", pe->FileHeader.Characteristics)),
       "pe->FileHeader.Characteristics");
  printf("\n");
  showpeoptionalheader(pecheckaddress(mz, mzsize, &pe->OptionalHeader,
                                      pe->FileHeader.SizeOfOptionalHeader));
  ShowSections(pecheckaddress(mz, mzsize,
                              (char *)(pe + 1) +
                                  pe->OptionalHeader.NumberOfRvaAndSizes * 8,
                              pe->FileHeader.NumberOfSections *
                                  sizeof(struct NtImageSectionHeader)),
               pe->FileHeader.NumberOfSections);
  ShowIdt(GetRva(pe->OptionalHeader.DataDirectory[kNtImageDirectoryEntryImport]
                     .VirtualAddress),
          pe->OptionalHeader.DataDirectory[kNtImageDirectoryEntryImport].Size);
}

static void showall(void) {

  startfile();
  showmzheader();
  showdosstub();
  if (mz->e_lfanew) {
    showpeheader(GetOff(mz->e_lfanew));
  }
}

int main(int argc, char *argv[]) {
  int64_t fd;
  struct stat st[1];
  ShowCrashReports();
  if (argc != 2) fprintf(stderr, "usage: %s FILE\n", argv[0]), exit(1);
  if ((fd = open((path = argv[1]), O_RDONLY)) == -1 || fstat(fd, st) == -1 ||
      (mz = mmap(NULL, (mzsize = st->st_size), PROT_READ, MAP_SHARED, fd, 0)) ==
          MAP_FAILED) {
    fprintf(stderr, "error: %'s %m\n", path);
    exit(1);
  }
  if (mz->e_magic != kNtImageDosSignature) {
    fprintf(stderr, "error: %'s not a dos executable\n", path);
    exit(1);
  }
  showall();
  munmap(mz, mzsize);
  close(fd);
  return 0;
}
