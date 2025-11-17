// Copyright 2025 Justine Alexandra Roberts Tunney
//
// Permission to use, copy, modify, and/or distribute this software for
// any purpose with or without fee is hereby granted, provided that the
// above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include "libc/stdckdint.h"
#include "tool/build/elf2pe.h"

#define MAX_SECTIONS 8

#define CP_UTF8 65001

#define STD_ERROR_HANDLE -12u

#define ERROR_IO_PENDING 997

#define READ_CONTROL         0x00020000u
#define GENERIC_EXECUTE      0x20000000u
#define STANDARD_RIGHTS_READ READ_CONTROL
#define FILE_READ_DATA       0x0001u
#define SYNCHRONIZE          0x00100000u
#define FILE_READ_ATTRIBUTES 0x0080u
#define FILE_READ_EA         0x0008u
#define FILE_GENERIC_READ                                         \
  (STANDARD_RIGHTS_READ | FILE_READ_DATA | FILE_READ_ATTRIBUTES | \
   FILE_READ_EA | SYNCHRONIZE)

#define FILE_SHARE_READ   0x00000001u
#define FILE_SHARE_WRITE  0x00000002u
#define FILE_SHARE_DELETE 0x00000004u

#define CREATE_NEW        1
#define CREATE_ALWAYS     2
#define OPEN_EXISTING     3
#define OPEN_ALWAYS       4
#define TRUNCATE_EXISTING 5

#define FILE_ATTRIBUTE_NORMAL 0x00000080u
#define FILE_FLAG_OVERLAPPED  0x40000000u

#define FILE_BEGIN   0
#define FILE_CURRENT 1
#define FILE_END     2

#define PROT_NONE  0
#define PROT_READ  1
#define PROT_WRITE 2
#define PROT_EXEC  4

#define IMAGE_SCN_MEM_EXECUTE 0x20000000u
#define IMAGE_SCN_MEM_READ    0x40000000u
#define IMAGE_SCN_MEM_WRITE   0x80000000u

#define IMAGE_SCN_CNT_CODE               0x00000020
#define IMAGE_SCN_CNT_INITIALIZED_DATA   0x00000040
#define IMAGE_SCN_CNT_UNINITIALIZED_DATA 0x00000080

#define PAGE_EXECUTE_WRITECOPY 0x080
#define PAGE_EXECUTE_READ      0x020
#define PAGE_EXECUTE_READWRITE 0x040
#define PAGE_WRITECOPY         0x008
#define PAGE_READWRITE         0x004
#define PAGE_READONLY          0x002
#define PAGE_NOACCESS          0x010

#define FILE_MAP_COPY    0x00000001u
#define FILE_MAP_READ    0x00000004u
#define FILE_MAP_WRITE   0x00000002u
#define FILE_MAP_EXECUTE 0x00000020u

#define MEM_COMMIT  0x1000
#define MEM_RESERVE 0x2000

#define PE64BIT 0x020b

#define IMAGE_DIRECTORY_ENTRY_EXPORT 0
#define IMAGE_DIRECTORY_ENTRY_IMPORT 1

#define PE_FILE_EXECUTABLE_IMAGE 0x00002
#define PE_FILE_DLL              0x02000

#define MIN(X, Y) ((Y) > (X) ? (X) : (Y))
#define MAX(X, Y) ((Y) < (X) ? (X) : (Y))

typedef struct {
  unsigned nLength;
  void *lpSecurityDescriptor;
  bool32 bInheritHandle;
} SECURITY_ATTRIBUTES;

typedef struct {
  unsigned long Internal;
  unsigned long InternalHigh;
  union {
    struct {
      unsigned int Offset;
      unsigned int OffsetHigh;
    };
    long Pointer;
  };
  long hEvent;
} OVERLAPPED;

typedef struct {
  union {
    unsigned int dwOemId;
    struct {
      unsigned short wProcessorArchitecture;
      unsigned short wReserved;
    };
  };
  int dwPageSize;
  void *lpMinimumApplicationAddress;
  void *lpMaximumApplicationAddress;
  unsigned long dwActiveProcessorMask;
  unsigned int dwNumberOfProcessors;
  unsigned int dwProcessorType;
  int dwAllocationGranularity;
  unsigned short wProcessorLevel;
  unsigned short wProcessorRevision;
} SYSTEM_INFO;

typedef struct {
  unsigned short Machine;
  unsigned short NumberOfSections;
  unsigned int TimeDateStamp;
  unsigned int PointerToSymbolTable;
  unsigned int NumberOfSymbols;
  unsigned short SizeOfOptionalHeader;
  unsigned short Characteristics;
} IMAGE_FILE_HEADER;

typedef struct {
  unsigned short Magic;
  unsigned char MajorLinkerVersion;
  unsigned char MinorLinkerVersion;
  unsigned int SizeOfCode;
  unsigned int SizeOfInitializedData;
  unsigned int SizeOfUninitializedData;
  unsigned int AddressOfEntryPoint;
  unsigned int BaseOfCode;
  unsigned long ImageBase;
  unsigned int SectionAlignment;
  unsigned int FileAlignment;
  unsigned short MajorOperatingSystemVersion;
  unsigned short MinorOperatingSystemVersion;
  unsigned short MajorImageVersion;
  unsigned short MinorImageVersion;
  unsigned short MajorSubsystemVersion;
  unsigned short MinorSubsystemVersion;
  unsigned int Win32VersionValue;
  unsigned int SizeOfImage;
  unsigned int SizeOfHeaders;
  unsigned int CheckSum;
  unsigned short Subsystem;
  unsigned short DllCharacteristics;
  unsigned long SizeOfStackReserve;
  unsigned long SizeOfStackCommit;
  unsigned long SizeOfHeapReserve;
  unsigned long SizeOfHeapCommit;
  unsigned int LoaderFlags;
  unsigned int NumberOfRvaAndSizes;
} IMAGE_OPTIONAL_HEADER;

typedef struct {
  unsigned int Signature;
  IMAGE_FILE_HEADER FileHeader;
  IMAGE_OPTIONAL_HEADER OptionalHeader;
} IMAGE_NT_HEADERS;

typedef struct {
  char Name[8];
  union {
    unsigned int PhysicalAddress;
    unsigned int VirtualSize;
  } Misc;
  unsigned int VirtualAddress;
  unsigned int SizeOfRawData;
  unsigned int PointerToRawData;
  unsigned int PointerToRelocations;
  unsigned int PointerToLinenumbers;
  unsigned short NumberOfRelocations;
  unsigned short NumberOfLinenumbers;
  unsigned int Characteristics;
} IMAGE_SECTION_HEADER;

typedef struct {
  unsigned int VirtualAddress;
  unsigned int Size;
} IMAGE_DATA_DIRECTORY;

typedef struct {
  unsigned int ImportLookupTable;
  unsigned int TimeDateStamp;
  unsigned int ForwarderChain;
  unsigned int DllNameRva;
  unsigned int ImportAddressTable;
} IMAGE_IMPORT_DESCRIPTOR;

#define GetCurrentProcess() -1  // officially documented

// clang-format off
__dll_import("kernel32.dll", int, CloseHandle, (long hObject));
__dll_import("kernel32.dll", int, GetOverlappedResult, (long hFile, OVERLAPPED *lpOverlapped, unsigned *lpNumberOfBytesTransferred, int bWait));
__dll_import("kernel32.dll", int, ReadFile, (long hFile, void *lpBuffer, unsigned nNumberOfBytesToRead, unsigned *lpNumberOfBytesRead, OVERLAPPED *lpOverlapped));
__dll_import("kernel32.dll", int, SetConsoleCP, (unsigned wCodePageID));
__dll_import("kernel32.dll", int, SetConsoleOutputCP, (unsigned wCodePageID));
__dll_import("kernel32.dll", int, SetFilePointerEx, (long hFile, long liDistanceToMove, long *opt_out_lpNewFilePointer, unsigned dwMoveMethod));
__dll_import("kernel32.dll", int, VirtualProtect, (void *lpAddress, size_t dwSize, unsigned flNewProtect, unsigned *lpflOldProtect));
__dll_import("kernel32.dll", int, WriteFile, (long hFile, const void *lpBuffer, unsigned nNumberOfBytesToWrite, unsigned *lpNumberOfBytesWritten, OVERLAPPED *lpOverlapped));
__dll_import("kernel32.dll", long, CreateEvent, (const SECURITY_ATTRIBUTES *opt_lpEventAttributes, int bManualReset, int bInitialState, const char16_t *opt_lpName));
__dll_import("kernel32.dll", long, CreateFileMappingW, (long hFile, const SECURITY_ATTRIBUTES *lpFileMappingAttributes, unsigned flProtect, unsigned dwMaximumSizeHigh, unsigned dwMaximumSizeLow, const char16_t *lpName));
__dll_import("kernel32.dll", long, CreateFileW, (const char16_t *wsPath, unsigned dwDesiredAccess, unsigned dwShareMode, SECURITY_ATTRIBUTES *opt_lpSecurityAttributes, int dwCreationDisposition, unsigned dwFlagsAndAttributes, long opt_hTemplateFile));
__dll_import("kernel32.dll", long, GetStdHandle, (unsigned nStdHandle));
__dll_import("kernel32.dll", long, LoadLibraryA, (const char *lpLibFileName));
__dll_import("kernel32.dll", unsigned, GetLastError, (void));
__dll_import("kernel32.dll", void *, GetProcAddress, (long, const char *));
__dll_import("kernel32.dll", void *, MapViewOfFileEx, (long hFileMappingObject, unsigned dwDesiredAccess, unsigned dwFileOffsetHigh, unsigned dwFileOffsetLow, size_t dwNumberOfBytesToMap, void *opt_lpBaseAddress));
__dll_import("kernel32.dll", void *, VirtualAlloc, (void *opt_lpAddress, size_t dwSize, unsigned flAllocationType, unsigned flProtect));
__dll_import("kernel32.dll", void, ExitProcess, (unsigned dwExitStatus));
__dll_import("kernel32.dll", void, GetSystemInfo, (SYSTEM_INFO *lpSystemInfo));
// clang-format on

static char *utoa(char p[static 21], unsigned long x) {
  int i, a, b, t;
  i = 0;
  do {
    p[i++] = x % 10 + '0';
    x = x / 10;
  } while (x > 0);
  p[i] = '\0';
  if (i) {
    for (a = 0, b = i - 1; a < b; ++a, --b) {
      t = p[a];
      p[a] = p[b];
      p[b] = t;
    }
  }
  return p + i;
}

static char *itoa(char p[static 21], long x) {
  if (x < 0)
    *p++ = '-', x = -(unsigned long)x;
  return utoa(p, x);
}

[[noreturn]] static void exit(int rc) {
  ExitProcess(rc << 8);
  __builtin_unreachable();
}

static long pread(long hFile, void *lpBuffer, long size, long qiOffset) {
  if (!SetFilePointerEx(hFile, qiOffset, 0, FILE_BEGIN))
    return -1;
  long toto = 0;
  char *p = lpBuffer;
  for (;;) {
    unsigned dwBytesRead;
    unsigned dwReadAmt = MIN(size - toto, 0x7fff0000);
    if (!ReadFile(hFile, p + toto, dwReadAmt, &dwBytesRead, 0))
      return -1;
    if (!dwBytesRead)
      break;
    if ((toto += dwBytesRead) == size)
      break;
  }
  return toto;
}

static void putc(char c) {
  WriteFile(GetStdHandle(STD_ERROR_HANDLE), &c, 1, 0, 0);
}

static void puts(const char *s) {
  while (*s)
    putc(*s++);
}

static void putwc(int c) {
  if (c <= 0x7f) {
    putc(c);
  } else if (c <= 0x7ff) {
    putc(0300 | (c >> 6));
    putc(0200 | (c & 077));
  } else if (c <= 0xffff) {
    if ((c & 0xf800) == 0xd800)
    EmitReplacementCharacter:
      c = 0xfffd;
    putc(0340 | (c >> 12));
    putc(0200 | ((c >> 6) & 077));
    putc(0200 | (c & 077));
  } else if (~(c >> 18) & 007) {
    putc(0360 | (c >> 18));
    putc(0200 | ((c >> 12) & 077));
    putc(0200 | ((c >> 6) & 077));
    putc(0200 | (c & 077));
  } else {
    goto EmitReplacementCharacter;
  }
}

static void putws(const char16_t *s) {
  int c;
  while ((c = *s++)) {
    switch (c & 0xfc00) {
      case 0xd800:
        if ((*s & 0xfc00) == 0xdc00) {
          c = ((c - 0xd800) << 10) + (*s++ - 0xdc00) + 0x10000;
          if (c <= 0x7f)
            c = 0xfffd;
        } else {
          c = 0xfffd;
        }
        break;
      case 0xdc00:
        c = 0xfffd;
      default:
        break;
    }
    putwc(c);
  }
}

static void puti(long u) {
  char b[21];
  itoa(b, u);
  puts(b);
}

[[noreturn]] static void die(const char *fmt, ...) {
  va_list va;
  va_start(va, fmt);
  for (;;) {
    char c;
    switch ((c = *fmt++)) {
      case '\0':
        va_end(va);
        exit(127);
      case '%': {
        switch ((c = *fmt++)) {
          case 's':
            puts(va_arg(va, const char *));
            break;
          case 'S':
            putws(va_arg(va, const char16_t *));
            break;
          case 'd':
            puti(va_arg(va, int));
            break;
          case 'x': {
            int d = va_arg(va, unsigned);
            for (int i = 8; i--;) {
              int u = (d >> (i * 4)) & 0xf;
              if (u < 10) {
                c = '0' + u;
              } else {
                u -= 10;
                c = 'a' + u;
              }
              putc(c);
            }
            break;
          }
          default:
            putc(c);
            break;
        }
      } break;
      default:
        putc(c);
        break;
    }
  }
}

[[noreturn]] static void sysfail(const char16_t *thing, const char *op) {
  die("%S: %s failed with error %d\n", thing, op, GetLastError());
}

static int pe2prot(unsigned dwCharacteristics) {
  int prot = 0;
  if (dwCharacteristics & IMAGE_SCN_MEM_READ)
    prot |= PROT_READ;
  if (dwCharacteristics & IMAGE_SCN_MEM_WRITE)
    prot |= PROT_READ | PROT_WRITE;
  if (dwCharacteristics & IMAGE_SCN_MEM_EXECUTE)
    prot |= PROT_READ | PROT_EXEC;
  return prot;
}

static int prot2nt(int prot, bool iscow) {
  switch (prot) {
    case PROT_READ:
      return PAGE_READONLY;
    case PROT_EXEC:
    case PROT_EXEC | PROT_READ:
      return PAGE_EXECUTE_READ;
    case PROT_WRITE:
    case PROT_READ | PROT_WRITE:
      if (iscow) {
        return PAGE_WRITECOPY;
      } else {
        return PAGE_READWRITE;
      }
    case PROT_WRITE | PROT_EXEC:
    case PROT_READ | PROT_WRITE | PROT_EXEC:
      if (iscow) {
        return PAGE_EXECUTE_WRITECOPY;
      } else {
        return PAGE_EXECUTE_READWRITE;
      }
    default:
      return PAGE_NOACCESS;
  }
}

void WinMain(void) {
  const char16_t *wsProg = u"hello";

  // use utf-8
  SetConsoleCP(CP_UTF8);
  SetConsoleOutputCP(CP_UTF8);

  // get page size and allocation granularity
  SYSTEM_INFO si;
  GetSystemInfo(&si);

  // open executable
  long hProg =
      CreateFileW(wsProg, FILE_GENERIC_READ | GENERIC_EXECUTE,
                  FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 0,
                  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
  if (hProg == -1)
    sysfail(wsProg, "CreateFileW");

  // get mz header
  long got;
  unsigned char mz[64];
  got = pread(hProg, mz, 64, 0);
  if (got == -1)
    sysfail(wsProg, "pread#1");
  if (got != 64)
    die("%S: Missing MZ header\n", wsProg);
  if (mz[0] != 'M' || mz[1] != 'Z')
    die("%S: Not an MZ executable\n", wsProg);
  unsigned e_lfanew =
      (mz[60] | mz[61] << 8 | mz[62] << 16 | (unsigned)mz[63] << 24);

  // read pe header
  IMAGE_NT_HEADERS pe;
  got = pread(hProg, &pe, sizeof(pe), e_lfanew);
  if (got == -1)
    sysfail(wsProg, "pread#2");
  if (got != sizeof(pe))
    die("%S: Missing PE header\n", wsProg);

  // validate pe headers according to microsoft's requirements
  if (pe.Signature != ('P' | 'E' << 8))
    die("%S: Missing PE signature\n", wsProg);
  if (pe.OptionalHeader.Magic != PE64BIT)
    die("%S: Not a 64-bit PE executable\n", wsProg);
  if ((pe.FileHeader.Characteristics & PE_FILE_DLL) ||
      !(pe.FileHeader.Characteristics & PE_FILE_EXECUTABLE_IMAGE))
    die("%S: PE image not an executable\n", wsProg);
  if (pe.FileHeader.NumberOfSections < 1)
    die("%S: PE NumberOfSections is zero\n", wsProg);
  if (pe.FileHeader.NumberOfSections > MAX_SECTIONS)
    die("%S: PE has too many sections\n", wsProg);
  if (pe.OptionalHeader.FileAlignment < 512)
    die("%S: PE FileAlignment must be at least 512\n", wsProg);
  if (pe.OptionalHeader.FileAlignment > 65536)
    die("%S: PE FileAlignment can't exceed 65536\n", wsProg);
  if (pe.OptionalHeader.FileAlignment & (pe.OptionalHeader.FileAlignment - 1))
    die("%S: PE FileAlignment must be two power\n", wsProg);
  if (pe.OptionalHeader.SectionAlignment &
      (pe.OptionalHeader.SectionAlignment - 1))
    die("%S: PE SectionAlignment must be two power\n", wsProg);
  if (pe.OptionalHeader.SectionAlignment < si.dwPageSize &&
      pe.OptionalHeader.SectionAlignment != pe.OptionalHeader.FileAlignment)
    die("%S: PE SectionAlignment must equal FileAlignment if it's less than "
        "the microprocessor architecture's page size\n",
        wsProg);
  if (pe.OptionalHeader.ImageBase & si.dwAllocationGranularity)
    die("%S: PE ImageBase must be multiple of %d\n", wsProg,
        si.dwAllocationGranularity);
  if (pe.OptionalHeader.SizeOfImage & (pe.OptionalHeader.SectionAlignment - 1))
    die("%S: PE SizeOfImage must be multiple of SectionAlignment\n", wsProg);
  if (pe.OptionalHeader.SizeOfHeaders & (pe.OptionalHeader.FileAlignment - 1))
    die("%S: PE SizeOfHeaders must be multiple of FileAlignment\n", wsProg);
  if (pe.OptionalHeader.SizeOfHeaders > pe.OptionalHeader.AddressOfEntryPoint)
    die("%S: PE SizeOfHeaders <= AddressOfEntryPoint not the case\n", wsProg);
  if (pe.OptionalHeader.SizeOfHeaders >= pe.OptionalHeader.SizeOfImage)
    die("%S: PE SizeOfHeaders < SizeOfImage must be the case\n", wsProg);
  if (pe.FileHeader.SizeOfOptionalHeader !=
      sizeof(IMAGE_OPTIONAL_HEADER) +
          pe.OptionalHeader.NumberOfRvaAndSizes * sizeof(IMAGE_DATA_DIRECTORY))
    die("%S: PE SizeOfOptionalHeader had incorrect value\n", wsProg);

  // read section definitions
  size_t size = pe.FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER);
  IMAGE_SECTION_HEADER sections[MAX_SECTIONS];
  got = pread(
      hProg, sections, size,
      e_lfanew + sizeof(IMAGE_NT_HEADERS) +
          pe.OptionalHeader.NumberOfRvaAndSizes * sizeof(IMAGE_DATA_DIRECTORY));
  if (got == -1)
    sysfail(wsProg, "pread#3");
  if (got != size)
    die("%S: Missing PE section data\n", wsProg);

  // validate section definitions according to microsoft's requirements
  for (int i = 0; i < pe.FileHeader.NumberOfSections; ++i) {
    if (sections[i].SizeOfRawData & (pe.OptionalHeader.FileAlignment - 1))
      die("%S: PE SizeOfRawData should be multiple of FileAlignment\n", wsProg);
    if (sections[i].PointerToRawData & (pe.OptionalHeader.FileAlignment - 1))
      die("%S: PE PointerToRawData must be multiple of FileAlignment\n",
          wsProg);
    if (!sections[i].Misc.VirtualSize)
      die("%S: PE VirtualSize shouldn't be zero\n", wsProg);
    if (sections[i].VirtualAddress & (pe.OptionalHeader.SectionAlignment - 1))
      die("%S: PE VirtualAddress must be multiple of SectionAlignment\n",
          wsProg);
    if ((sections[i].Characteristics &
         (IMAGE_SCN_CNT_CODE | IMAGE_SCN_CNT_INITIALIZED_DATA |
          IMAGE_SCN_CNT_UNINITIALIZED_DATA)) ==
        IMAGE_SCN_CNT_UNINITIALIZED_DATA) {
      if (sections[i].SizeOfRawData)
        die("%S: PE SizeOfRawData should be zero for pure BSS section\n",
            wsProg);
      if (sections[i].PointerToRawData)
        die("%S: PE PointerToRawData should be zero for pure BSS section\n",
            wsProg);
    }
    if (!i) {
      if (sections[i].VirtualAddress !=
          ((pe.OptionalHeader.SizeOfHeaders +
            (pe.OptionalHeader.SectionAlignment - 1)) &
           -pe.OptionalHeader.SectionAlignment))
        die("%S: PE VirtualAddress of first section must be SizeOfHeaders "
            "rounded up to SectionAlignment\n",
            wsProg);
    } else {
      if (sections[i].VirtualAddress !=
          sections[i - 1].VirtualAddress +
              ((sections[i - 1].Misc.VirtualSize +
                (pe.OptionalHeader.SectionAlignment - 1)) &
               -pe.OptionalHeader.SectionAlignment))
        die("%S: PE sections must be in ascending order and the virtual "
            "memory they define must be adjacent after VirtualSize is "
            "rounded up to the SectionAlignment\n",
            wsProg);
    }
  }

  // this loader requires sections be contiguous in memory and file
  // basically a flat executable, that uses sections for protection
  if (sections[0].PointerToRawData != pe.OptionalHeader.SizeOfHeaders)
    die("%S: PointerToRawData of first section didn't equal SizeOfHeaders\n",
        wsProg);
  long FileSize = pe.OptionalHeader.SizeOfHeaders;
  long VirtualSize = pe.OptionalHeader.SizeOfHeaders;
  for (int i = 0; i < pe.FileHeader.NumberOfSections; ++i) {
    if (sections[i].SizeOfRawData) {
      if (VirtualSize > FileSize)
        die("%S: PE section with initialized data encountered after one that "
            "had uninitialized data\n",
            wsProg);
      if (sections[i].VirtualAddress != sections[i].PointerToRawData)
        die("%S: PE section VirtualAddress must equal PointerToRawData when "
            "SizeOfRawData is nonzero\n",
            wsProg);
      if (sections[i].Misc.VirtualSize < sections[i].SizeOfRawData)
        die("%S: PE section VirtualSize >= SizeOfRawData wasn't the case\n",
            wsProg);
    }
    if (sections[i].VirtualAddress & (si.dwPageSize - 1))
      die("%S: PE sectiond VirtualAddress must be page size aligned\n", wsProg);
    if (sections[i].Misc.VirtualSize & (si.dwPageSize - 1))
      die("%S: PE section VirtualSize must be multiple of page size\n", wsProg);
    VirtualSize += sections[i].Misc.VirtualSize;
    FileSize += sections[i].SizeOfRawData;
    if (i)
      if (sections[i].VirtualAddress !=
          sections[i - 1].VirtualAddress + sections[i - 1].Misc.VirtualSize)
        die("%S: PE sections must be contiguous and ordered\n", wsProg);
  }

  // we MapViewOfFileEx() the file from MZ to the end of the .data
  // section. the rest is .bss which is created using VirtualAlloc().
  // the issue is that we can only use VirtualAlloc() on an address
  // that's aligned on the system allocator granularity, which is much
  // larger than the page size. so we round down .bss into the .data
  // section and read a few pages manually to reconcile differences.
  long MappableFileSize = FileSize & -si.dwAllocationGranularity;
  long StrayDataSize = FileSize - MappableFileSize;
  long BssSize = VirtualSize - MappableFileSize;

  // map executable into memory
  //
  // we need to greedily request permissions. we'll use VirtualProtect()
  // to remove them in a moment. however if we don't initially request
  // everything, then the user executable won't be able to get further
  // access like rwx later on.
  long hMapping;
  if (!(hMapping =
            CreateFileMappingW(hProg, 0, PAGE_EXECUTE_WRITECOPY,
                               MappableFileSize >> 32, MappableFileSize, 0)))
    sysfail(wsProg, "CreateFileMappingW");
  long base = pe.OptionalHeader.ImageBase;
  if (!MapViewOfFileEx(hMapping, FILE_MAP_COPY | FILE_MAP_EXECUTE, 0, 0,
                       MappableFileSize, (char *)base)) {
    die("MapViewOfFileEx failed addr=%x size=%x\n", base, MappableFileSize);
    sysfail(wsProg, "MapViewOfFileEx");
  }

  // allocate .bss section
  if (!VirtualAlloc((char *)(base + MappableFileSize), BssSize,
                    MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE))
    sysfail(wsProg, "VirtualAlloc");

  // read .data from file that had to become part of .bss
  if (StrayDataSize) {
    long got;
    if ((got = pread(hProg, (char *)(base + MappableFileSize), StrayDataSize,
                     MappableFileSize)) == -1)
      sysfail(wsProg, "pread#4");
    if (got != StrayDataSize)
      die("%S: failed to pread stray data\n", wsProg);
  }

  // import dynamic link library procedures
  if (pe.OptionalHeader.NumberOfRvaAndSizes >= 2) {
    IMAGE_DATA_DIRECTORY *idd =
        (void *)(base + e_lfanew + sizeof(IMAGE_NT_HEADERS) +
                 sizeof(IMAGE_DATA_DIRECTORY));
    IMAGE_IMPORT_DESCRIPTOR *iid = (void *)(base + idd->VirtualAddress);
    for (int i = 0; i < idd->Size / sizeof(IMAGE_IMPORT_DESCRIPTOR); ++i) {
      if (!iid[i].DllNameRva)
        break;
      long hDll = LoadLibraryA((const char *)(base + iid[i].DllNameRva));
      if (!hDll)
        die("%S: Failed to import library\n", wsProg);
      long *ilt = (long *)(base + iid[i].ImportLookupTable);
      void **iat = (void **)(base + iid[i].ImportAddressTable);
      for (; *ilt; ++ilt, ++iat)
        if (!(*iat = GetProcAddress(hDll, (const char *)(base + *ilt + 2))))
          die("%S: Failed to import procedure\n", wsProg);
    }
  }

  // apply desired memory protections
  unsigned op;
  char *cow = (char *)(base + MappableFileSize);
  for (int i = 0; i < pe.FileHeader.NumberOfSections; ++i) {
    int prot = pe2prot(sections[i].Characteristics);
    char *a = (char *)(base + sections[i].VirtualAddress);
    char *b = (char *)(base + sections[i].VirtualAddress +
                       sections[i].Misc.VirtualSize);
    if (a < cow && MIN(b, cow) > a)
      if (!VirtualProtect(a, MIN(b, cow) - a, prot2nt(prot, true), &op))
        sysfail(wsProg, "VirtualProtect#1");
    if (b > cow && MAX(a, cow) < b)
      if (!VirtualProtect(MAX(a, cow), b - MAX(a, cow), prot2nt(prot, false),
                          &op))
        sysfail(wsProg, "VirtualProtect#2");
  }
  if (!VirtualProtect((char *)base, pe.OptionalHeader.SizeOfHeaders,
                      PAGE_READONLY, &op))
    sysfail(wsProg, "VirtualProtect#3");

  if (!CloseHandle(hMapping))
    sysfail(wsProg, "CloseHandle#1");
  if (!CloseHandle(hProg))
    sysfail(wsProg, "CloseHandle#2");

  // call entrypoint function
  int (*__attribute__((__ms_abi__)) pWinMain)(long, long, long, long) =
      (void *)(base + pe.OptionalHeader.AddressOfEntryPoint);
  unsigned dwExitStatus = pWinMain(0, 0, 0, 0);
  ExitProcess(dwExitStatus);
  __builtin_unreachable();
}
