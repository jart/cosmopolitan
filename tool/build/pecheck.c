/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/serialize.h"
#include "libc/limits.h"
#include "libc/nt/struct/imageimportbyname.internal.h"
#include "libc/nt/struct/imageimportdescriptor.internal.h"
#include "libc/nt/struct/imagentheaders.internal.h"
#include "libc/nt/struct/imageoptionalheader.internal.h"
#include "libc/nt/struct/imagesectionheader.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdckdint.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"

/**
 * @fileoverview Linter for PE static executable files.
 *
 * Generating PE files from scratch is tricky. There's numerous things
 * that can go wrong, and operating systems don't explain what's wrong
 * when they refuse to run a program. This program can help illuminate
 * any issues with your generated binaries, with better error messages
 */

struct Exe {
  char *map;
  size_t size;
  const char *path;
  struct NtImageNtHeaders *pe;
  struct NtImageSectionHeader *sections;
  uint32_t section_count;
};

static wontreturn void Die(const char *thing, const char *reason) {
  tinyprint(2, thing, ": ", reason, "\n", NULL);
  exit(1);
}

static wontreturn void DieSys(const char *thing) {
  perror(thing);
  exit(1);
}

static void LogPeSections(FILE *f, struct NtImageSectionHeader *p, size_t n) {
  size_t i;
  fprintf(f, "Name     Offset   RelativeVirtAddr   FileSiz  MemSiz   Flg\n");
  for (i = 0; i < n; ++i) {
    fprintf(f, "%-8.8s 0x%06lx 0x%016lx 0x%06lx 0x%06lx %c%c%c\n", p[i].Name,
            p[i].PointerToRawData, p[i].VirtualAddress, p[i].SizeOfRawData,
            p[i].Misc.VirtualSize,
            p[i].Characteristics & kNtPeSectionMemRead ? 'R' : ' ',
            p[i].Characteristics & kNtPeSectionMemWrite ? 'W' : ' ',
            p[i].Characteristics & kNtPeSectionMemExecute ? 'E' : ' ');
  }
}

// resolves relative virtual address
//
// this is a trivial process when an executable has been loaded properly
// i.e. a separate mmap() call was made for each individual section; but
// we've only mapped the executable file itself into memory; thus, we'll
// need to remap a virtual address into a file offset to get the pointer
//
// returns pointer to image data, or null on error
static void *GetRva(struct Exe *exe, uint32_t rva, uint32_t size) {
  int i;
  for (i = 0; i < exe->section_count; ++i) {
    if (exe->sections[i].VirtualAddress <= rva &&
        rva < exe->sections[i].VirtualAddress +
                  exe->sections[i].Misc.VirtualSize) {
      if (rva + size <=
          exe->sections[i].VirtualAddress + exe->sections[i].Misc.VirtualSize) {
        return exe->map + exe->sections[i].PointerToRawData +
               (rva - exe->sections[i].VirtualAddress);
      } else {
        break;
      }
    }
  }
  return 0;
}

static bool HasControlCodes(const char *s) {
  int c;
  while ((c = *s++)) {
    if (isascii(c) && iscntrl(c)) {
      return true;
    }
  }
  return false;
}

static void CheckPeImportByName(struct Exe *exe, uint32_t rva) {
  struct NtImageImportByName *hintname;
  if (rva & 1)
    Die(exe->path, "PE IMAGE_IMPORT_BY_NAME (hint name) structures must "
                   "be 2-byte aligned");
  if (!(hintname = GetRva(exe, rva, sizeof(struct NtImageImportByName))))
    Die(exe->path, "PE import table RVA entry didn't reslove");
  if (!*hintname->Name)
    Die(exe->path, "PE imported function name is empty string");
  if (HasControlCodes(hintname->Name))
    Die(exe->path, "PE imported function name contains ascii control codes");
}

static void CheckPe(const char *path, char *map, size_t size) {

  int pagesz = 4096;

  // sanity check mz header
  if (size < 64)  //
    Die(path, "Image too small for MZ header");
  if (READ16LE(map) != ('M' | 'Z' << 8))
    Die(path, "Image doesn't start with MZ");
  uint32_t pe_offset;
  if ((pe_offset = READ32LE(map + 60)) >= size)
    Die(path, "PE header offset points past end of image");
  if (pe_offset & 7)
    Die(path, "PE header offset must possess an 8-byte alignment");
  if (pe_offset + sizeof(struct NtImageNtHeaders) > size)
    Die(path, "PE mandatory headers overlap end of image");
  struct NtImageNtHeaders *pe = (struct NtImageNtHeaders *)(map + pe_offset);
  if ((pe_offset + sizeof(struct NtImageFileHeader) + 4 +
       pe->FileHeader.SizeOfOptionalHeader) > size)
    Die(path, "PE optional header size overlaps end of image");

  // sanity check pe header
  if (pe->Signature != ('P' | 'E' << 8))
    Die(path, "PE Signature must be 0x00004550");
  if (!(pe->FileHeader.Characteristics & kNtPeFileExecutableImage))
    Die(path, "PE Characteristics must have executable bit set");
  if (pe->FileHeader.Characteristics & kNtPeFileDll)
    Die(path, "PE Characteristics can't have DLL bit set");
  if (pe->FileHeader.NumberOfSections < 1)
    Die(path, "PE NumberOfSections >= 1 must be the case");
  if (pe->OptionalHeader.Magic != kNtPe64bit)
    Die(path, "PE OptionalHeader Magic must be 0x020b");
  if (pe->OptionalHeader.FileAlignment < 512)
    Die(path, "PE FileAlignment must be at least 512");
  if (pe->OptionalHeader.FileAlignment > 65536)
    Die(path, "PE FileAlignment can't exceed 65536");
  if (pe->OptionalHeader.FileAlignment & (pe->OptionalHeader.FileAlignment - 1))
    Die(path, "PE FileAlignment must be a two power");
  if (pe->OptionalHeader.SectionAlignment &
      (pe->OptionalHeader.SectionAlignment - 1))
    Die(path, "PE SectionAlignment must be a two power");
  if (pe->OptionalHeader.SectionAlignment < pe->OptionalHeader.FileAlignment)
    Die(path, "PE SectionAlignment >= FileAlignment must be the case");
  if (pe->OptionalHeader.SectionAlignment < pagesz &&
      pe->OptionalHeader.SectionAlignment != pe->OptionalHeader.FileAlignment)
    Die(path, "PE SectionAlignment must equal FileAlignment if it's less than "
              "the microprocessor architecture's page size");
  if (pe->OptionalHeader.ImageBase & 65535)
    Die(path, "PE ImageBase must be multiple of 65536");
  if (pe->OptionalHeader.ImageBase > INT_MAX &&
      !(pe->FileHeader.Characteristics & kNtImageFileLargeAddressAware))
    Die(path, "PE FileHeader.Characteristics needs "
              "IMAGE_FILE_LARGE_ADDRESS_AWARE if ImageBase > INT_MAX");

  // validate the size of the pe optional headers
  int len;
  if (ckd_mul(&len, pe->OptionalHeader.NumberOfRvaAndSizes,
              sizeof(struct NtImageDataDirectory)) ||
      ckd_add(&len, len, sizeof(struct NtImageOptionalHeader)))
    Die(path, "encountered overflow computing PE SizeOfOptionalHeader");
  if (pe->FileHeader.SizeOfOptionalHeader != len)
    Die(path, "PE SizeOfOptionalHeader had incorrect value");
  if (len > size || (char *)&pe->OptionalHeader + len > map + size)
    Die(path, "PE OptionalHeader overflows image");

  // perform even more pe validation
  if (pe->OptionalHeader.SizeOfImage &
      (pe->OptionalHeader.SectionAlignment - 1))
    Die(path, "PE SizeOfImage must be multiple of SectionAlignment");
  if (pe->OptionalHeader.SizeOfHeaders & (pe->OptionalHeader.FileAlignment - 1))
    Die(path, "PE SizeOfHeaders must be multiple of FileAlignment");
  if (pe->OptionalHeader.SizeOfHeaders > pe->OptionalHeader.AddressOfEntryPoint)
    Die(path, "PE SizeOfHeaders <= AddressOfEntryPoint must be the case");
  if (pe->OptionalHeader.SizeOfHeaders >= pe->OptionalHeader.SizeOfImage)
    Die(path, "PE SizeOfHeaders < SizeOfImage must be the case");
  if (pe->OptionalHeader.SizeOfStackCommit >> 32)
    Die(path, "PE SizeOfStackCommit can't exceed 4GB");
  if (pe->OptionalHeader.SizeOfStackReserve >> 32)
    Die(path, "PE SizeOfStackReserve can't exceed 4GB");
  if (pe->OptionalHeader.SizeOfHeapCommit >> 32)
    Die(path, "PE SizeOfHeapCommit can't exceed 4GB");
  if (pe->OptionalHeader.SizeOfHeapReserve >> 32)
    Die(path, "PE SizeOfHeapReserve can't exceed 4GB");

  // check pe section headers
  struct NtImageSectionHeader *sections =
      (struct NtImageSectionHeader *)((char *)&pe->OptionalHeader +
                                      pe->FileHeader.SizeOfOptionalHeader);
  for (int i = 0; i < pe->FileHeader.NumberOfSections; ++i) {
    if (sections[i].SizeOfRawData & (pe->OptionalHeader.FileAlignment - 1))
      Die(path, "PE SizeOfRawData should be multiple of FileAlignment");
    if (sections[i].PointerToRawData & (pe->OptionalHeader.FileAlignment - 1))
      Die(path, "PE PointerToRawData must be multiple of FileAlignment");
    if (map + sections[i].PointerToRawData >= map + size)
      Die(path, "PE PointerToRawData points outside image");
    if (map + sections[i].PointerToRawData + sections[i].SizeOfRawData >
        map + size)
      Die(path, "PE SizeOfRawData overlaps end of image");
    if (!sections[i].VirtualAddress)
      Die(path, "PE VirtualAddress shouldn't be zero");
    if (sections[i].VirtualAddress & (pe->OptionalHeader.SectionAlignment - 1))
      Die(path, "PE VirtualAddress must be multiple of SectionAlignment");
    if ((sections[i].Characteristics &
         (kNtPeSectionCntCode | kNtPeSectionCntInitializedData |
          kNtPeSectionCntUninitializedData)) ==
        kNtPeSectionCntUninitializedData) {
      if (sections[i].SizeOfRawData)
        Die(path, "PE SizeOfRawData should be zero for pure BSS section");
      if (sections[i].PointerToRawData)
        Die(path, "PE PointerToRawData should be zero for pure BSS section");
    }
    if (!i) {
      if (sections[i].VirtualAddress !=
          ((pe->OptionalHeader.SizeOfHeaders +
            (pe->OptionalHeader.SectionAlignment - 1)) &
           -pe->OptionalHeader.SectionAlignment))
        Die(path, "PE VirtualAddress of first section must be SizeOfHeaders "
                  "rounded up to SectionAlignment");
    } else {
      if (sections[i].VirtualAddress !=
          sections[i - 1].VirtualAddress +
              ((sections[i - 1].Misc.VirtualSize +
                (pe->OptionalHeader.SectionAlignment - 1)) &
               -pe->OptionalHeader.SectionAlignment))
        Die(path, "PE sections must be in ascending order and the virtual "
                  "memory they define must be adjacent after VirtualSize is "
                  "rounded up to the SectionAlignment");
    }
  }

  // create an object for our portable executable
  struct Exe exe[1] = {{
      .pe = pe,
      .path = path,
      .map = map,
      .size = size,
      .sections = sections,
      .section_count = pe->FileHeader.NumberOfSections,
  }};

  // validate dll imports
  struct NtImageDataDirectory *ddImports =
      exe->pe->OptionalHeader.DataDirectory + kNtImageDirectoryEntryImport;
  if (exe->pe->OptionalHeader.NumberOfRvaAndSizes >= 2 && ddImports->Size) {
    if (ddImports->Size % sizeof(struct NtImageImportDescriptor) != 0)
      Die(exe->path, "PE Imports data directory entry Size should be a "
                     "multiple of sizeof(IMAGE_IMPORT_DESCRIPTOR)");
    if (ddImports->VirtualAddress & 3)
      Die(exe->path, "PE IMAGE_IMPORT_DESCRIPTOR table must be 4-byte aligned");
    struct NtImageImportDescriptor *idt;
    if (!(idt = GetRva(exe, ddImports->VirtualAddress, ddImports->Size)))
      Die(exe->path, "couldn't resolve VirtualAddress/Size RVA of PE Import "
                     "Directory Table to within a defined PE section");
    if (idt->ImportLookupTable >= exe->size)
      Die(exe->path, "Import Directory Table VirtualAddress/Size RVA resolved "
                     "to dense unrelated binary content");
    for (int i = 0; idt->ImportLookupTable; ++i, ++idt) {
      char *dllname;
      if (!(dllname = GetRva(exe, idt->DllNameRva, 2)))
        Die(exe->path, "PE DllNameRva doesn't resolve to a PE section");
      if (!*dllname)
        Die(exe->path, "PE import DllNameRva pointed to empty string");
      if (HasControlCodes(dllname))
        Die(exe->path, "PE import DllNameRva contained ascii control codes");
      if (idt->ImportLookupTable & 7)
        Die(exe->path, "PE ImportLookupTable must be 8-byte aligned");
      if (idt->ImportAddressTable & 7)
        Die(exe->path, "PE ImportAddressTable must be 8-byte aligned");
      uint64_t *ilt, *iat;
      if (!(ilt = GetRva(exe, idt->ImportLookupTable, 8)))
        Die(exe->path, "PE ImportLookupTable RVA didn't resolve to a section");
      if (!(iat = GetRva(exe, idt->ImportAddressTable, 8)))
        Die(exe->path, "PE ImportAddressTable RVA didn't resolve to a section");
      for (int j = 0;; ++j, ++ilt, ++iat) {
        if (*ilt != *iat) {
          Die(exe->path, "PE ImportLookupTable and ImportAddressTable should "
                         "have identical content");
        }
        if (!*ilt) break;
        CheckPeImportByName(exe, *ilt);
      }
    }
  }
}

int main(int argc, char *argv[]) {
  int i, fd;
  void *map;
  ssize_t size;
  const char *path;
#ifdef MODE_DBG
  ShowCrashReports();
#endif
  for (i = 1; i < argc; ++i) {
    path = argv[i];
    if ((fd = open(path, O_RDONLY)) == -1) DieSys(path);
    if ((size = lseek(fd, 0, SEEK_END)) == -1) DieSys(path);
    map = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED) DieSys(path);
    CheckPe(path, map, size);
    if (munmap(map, size)) DieSys(path);
    if (close(fd)) DieSys(path);
  }
}
