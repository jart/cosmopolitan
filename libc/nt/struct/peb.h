#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_PEB_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_PEB_H_

#include "libc/assert.h"
#include "libc/isystem/stdbool.h"
#include "libc/nt/struct/ldr.h"
#include "libc/nt/struct/rtluserprocessparameters.h"
#include "libc/nt/thunk/msabi.h"

typedef void (*__msabi NtPsPostProcessInitRoutine)(void);

struct NtPeb {
  bool InheritedAddressSpace;                        /* msdn:reserved */
  bool ReadImageFileExecOptions;                     /* msdn:reserved */
  bool BeingDebugged;
  unsigned char Bitfield;                            /* msdn:reserved */
  int64_t Mutant;                                    /* msdn:reserved */
  void* ImageBaseAddress;                            /* msdn:reserved */
  struct NtLdr* Ldr;
  struct NtRtlUserProcessParameters* ProcessParameters;
  void* SubSystemData;                               /* msdn:reserved */
  void* ProcessHeap;                                 /* msdn:reserved */
  struct NtCriticalSection* FastPebLock;             /* msdn:reserved */
  void* AtlThunkSListPtr;                            /* msdn:reserved */
  int64_t IFEOKey;                                   /* msdn:reserved */
  uint32_t CrossProcessFlags;                        /* msdn:reserved */
  void* KernelCallbackTable;                         /* msdn:reserved */
  uint32_t SystemReserved;                           /* msdn:reserved */
  uint32_t AtlThunkSListPtr32;                       /* msdn:reserved */
  void* ApiSetMap;                                   /* msdn:reserved */
  uint32_t TlsExpansionCounter;                      /* msdn:reserved */
  void* TlsBitmap;                                   /* msdn:reserved */
  uint32_t TlsBitmapBits[2];                         /* msdn:reserved */
  void* ReadOnlySharedMemoryBase;                    /* msdn:reserved */
  void* SharedData;                                  /* msdn:reserved */
  void** /* [sic] */ ReadOnlyStaticServerData;       /* msdn:reserved */
  void* AnsiCodePageData;                            /* msdn:reserved */
  void* OemCodePageData;                             /* msdn:reserved */
  void* UnicodeCaseTableData;                        /* msdn:reserved */
  uint32_t NumberOfProcessors;                       /* msdn:reserved */
  uint32_t NtGlobalFlag;                             /* msdn:reserved */
  int64_t CriticalSectionTimeout;                    /* msdn:reserved */
  size_t HeapSegmentReserve;                         /* msdn:reserved */
  size_t HeapSegmentCommit;                          /* msdn:reserved */
  size_t HeapDeCommitTotalFreeThreshold;             /* msdn:reserved */
  size_t HeapDeCommitFreeBlockThreshold;             /* msdn:reserved */
  uint32_t NumberOfHeaps;                            /* msdn:reserved */
  uint32_t MaximumNumberOfHeaps;                     /* msdn:reserved */
  void** /* [sic] */ ProcessHeaps;                   /* msdn:reserved */
  void* GdiSharedHandleTable;                        /* msdn:reserved */
  void* ProcessStarterHelper;                        /* msdn:reserved */
  uint32_t GdiDCAttributeList;                       /* msdn:reserved */
  struct NtCriticalSection* LoaderLock;              /* msdn:reserved */
  uint32_t OSMajorVersion;                           /* msdn:reserved */
  uint32_t OSMinorVersion;                           /* msdn:reserved */
  unsigned short OSBuildNumber;                      /* msdn:reserved */
  unsigned short OSCSDVersion;                       /* msdn:reserved */
  uint32_t OSPlatformId;                             /* msdn:reserved */
  uint32_t ImageSubsystem;                           /* msdn:reserved */
  uint32_t ImageSubsystemMajorVersion;               /* msdn:reserved */
  uint32_t ImageSubsystemMinorVersion;               /* msdn:reserved */
  uintptr_t ActiveProcessAffinityMask;               /* msdn:reserved */
  uint32_t GdiHandleBuffer[60];                      /* msdn:reserved */
  NtPsPostProcessInitRoutine PostProcessInitRoutine; /* msdn:unsupported */
  void* TlsExpansionBitmap;                          /* msdn:reserved */
  uint32_t TlsExpansionBitmapBits[32];               /* msdn:reserved */
  uint32_t SessionId;
};

static_assert(offsetof(struct NtPeb, BeingDebugged) == 0x2);
static_assert(offsetof(struct NtPeb, Ldr) == 0x18);
static_assert(offsetof(struct NtPeb, ProcessParameters) == 0x20);
static_assert(offsetof(struct NtPeb, AtlThunkSListPtr) == 0x40);
static_assert(offsetof(struct NtPeb, AtlThunkSListPtr32) == 0x64);
static_assert(offsetof(struct NtPeb, PostProcessInitRoutine) == 0x230);

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_PEB_H_ */
