#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_PEB_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_PEB_H_
#include "libc/nt/struct/ldr.h"
#include "libc/nt/struct/unicodestring.h"

struct NtPeb {
  union {
    struct {
      unsigned char InheritedAddressSpace;
      unsigned char ReadImageFileExecOptions;
      unsigned char BeingDebugged;
      unsigned char __wut1;
    };
    uint64_t __wut2;
  };
  uint64_t Mutant;
  uint64_t ImageBaseAddress;
  struct NtLdr *Ldr;
  uint64_t ProcessParameters;
  uint64_t SubSystemData;
  uint64_t ProcessHeap;
  uint64_t FastPebLock;
  uint64_t __wut3;
  uint64_t __wut4;
  uint64_t __wut5;
  union {
    uint64_t KernelCallbackTable;
    uint64_t UserSharedInfoPtr;
  };
  uint32_t SystemReserved;
  uint32_t __wut6;
  uint64_t __wut7;
  uint64_t TlsExpansionCounter;
  uint64_t TlsBitmap;
  uint32_t TlsBitmapBits[2];
  uint64_t ReadOnlySharedMemoryBase;
  uint64_t __wut8;
  uint64_t ReadOnlyStaticServerData;
  uint64_t AnsiCodePageData;
  uint64_t OemCodePageData;
  uint64_t UnicodeCaseTableData;
  uint32_t NumberOfProcessors;
#ifdef __x86_64__
  uint32_t NtGlobalFlag;
#else
  uint64_t NtGlobalFlag;
#endif
  int64_t CriticalSectionTimeout;
  uint64_t HeapSegmentReserve;
  uint64_t HeapSegmentCommit;
  uint64_t HeapDeCommitTotalFreeThreshold;
  uint64_t HeapDeCommitFreeBlockThreshold;
  uint32_t NumberOfHeaps;
  uint32_t MaximumNumberOfHeaps;
  uint64_t ProcessHeaps;
  uint64_t GdiSharedHandleTable;
  uint64_t ProcessStarterHelper;
  uint64_t GdiDCAttributeList;
  uint64_t LoaderLock;
  union {
    struct {
      uint32_t OSMajorVersion;
      uint32_t OSMinorVersion;
    };
    uint64_t OSVersion;
  };
  uint16_t OSBuildNumber;
  uint16_t OSCSDVersion;
  uint32_t OSPlatformId;
  uint32_t ImageSubsystem;
  uint32_t ImageSubsystemMajorVersion;
  uint64_t ImageSubsystemMinorVersion;
  union {
    uint64_t ImageProcessAffinityMask;
    uint64_t ActiveProcessAffinityMask;
  };
  uint64_t GdiHandleBuffer[38 - __SIZEOF_POINTER__];
  uint64_t PostProcessInitRoutine;
  uint64_t TlsExpansionBitmap;
  uint32_t TlsExpansionBitmapBits[32];
  uint64_t SessionId;
  uint64_t AppCompatFlags;
  uint64_t AppCompatFlagsUser;
  uint64_t pShimData;
  uint64_t AppCompatInfo;
  struct NtUnicodeString CSDVersion;
  uint64_t ActivationContextData;
  uint64_t ProcessAssemblyStorageMap;
  uint64_t SystemDefaultActivationContextData;
  uint64_t SystemAssemblyStorageMap;
  uint64_t MinimumStackCommit;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_PEB_H_ */
