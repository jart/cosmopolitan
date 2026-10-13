#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_TEB_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_TEB_H_

#include "libc/assert.h"
#include "libc/nt/enum/status.h"
#include "libc/nt/struct/activationcontextstack.h"
#include "libc/nt/struct/clientid.h"
#include "libc/nt/struct/guid.h"
#include "libc/nt/struct/peb.h"
#include "libc/nt/struct/processornumber.h"
#include "libc/nt/struct/tib.h"
#include "libc/nt/struct/unicodestring.h"

struct NtGdiTebBatch {
  uint32_t Offset;
  uintptr_t HDC;
  uint32_t Buffer[310];
};

struct NtTeb {
  struct NtTib NtTib;                                                       /* msdn:reserved */
  void* EnvironmentPointer;                                                 /* msdn:reserved */
  struct NtClientId ClientId;                                               /* msdn:reserved */
  void* ActiveRpcHandle;                                                    /* msdn:reserved */
  void* ThreadLocalStoragePointer;                                          /* msdn:reserved */
  struct NtPeb* ProcessEnvironmentBlock;
  uint32_t LastErrorValue;                                                  /* msdn:reserved */
  uint32_t CountOfOwnedCriticalSections;                                    /* msdn:reserved */
  void* CsrClientThread;                                                    /* msdn:reserved */
  void* Win32ThreadInfo;                                                    /* msdn:reserved */
  uint32_t User32Reserved[26];                                              /* msdn:reserved */
  uint32_t UserReserved[5];                                                 /* msdn:reserved */
  void* WOW32Reserved;                                                      /* msdn:reserved */
  uint32_t CurrentLocale;                                                   /* msdn:reserved */
  uint32_t FpSoftwareStatusRegister;                                        /* msdn:reserved */
  void* ReservedForDebuggerInstrumentation[16];                             /* msdn:reserved */
  void* SystemReserved1[25];                                                /* msdn:reserved */
  void* HeapFlsData;                                                        /* msdn:reserved */
  uintptr_t RngState[4];                                                    /* msdn:reserved */
  char PlaceholderCompatibilityMode;                                        /* msdn:reserved */
  bool PlaceholderHydrationAlwaysExplicit;                                  /* msdn:reserved */
  char PlaceholderReserved[10];                                             /* msdn:reserved */
  uint32_t ProxiedProcessId;                                                /* msdn:reserved */
  struct NtActivationContextStack ActivationStack;                          /* msdn:reserved */
  unsigned char WorkingOnBehalfTicket[8];                                   /* msdn:reserved */
  NtStatus ExceptionCode;                                                   /* msdn:reserved */
  struct NtActivationContextStack* ActivationContextStackPointer;           /* msdn:reserved */
  uintptr_t InstrumentationCallbackSp;                                      /* msdn:reserved */
  uintptr_t InstrumentationCallbackPreviousPc;                              /* msdn:reserved */
  uintptr_t InstrumentationCallbackPreviousSp;                              /* msdn:reserved */
  uint32_t TxFsContext;                                                     /* msdn:reserved */
  bool InstrumentationCallbackDisabled;                                     /* msdn:reserved */
  bool UnalignedLoadStoreExceptions;                                        /* msdn:reserved */
  struct NtGdiTebBatch GdiTebBatch;                                         /* msdn:reserved */
  struct NtClientId RealClientId;                                           /* msdn:reserved */
  int64_t GdiCachedProcessHandle;                                           /* msdn:reserved */
  uint32_t GdiClientPID;                                                    /* msdn:reserved */
  uint32_t GdiClientTID;                                                    /* msdn:reserved */
  void* GdiThreadLocalInfo;                                                 /* msdn:reserved */
  uintptr_t Win32ClientInfo[62];                                            /* msdn:reserved */
  void* glDispatchTable[233];                                               /* msdn:reserved */
  uintptr_t glReserved1[29];                                                /* msdn:reserved */
  void* glReserved2;                                                        /* msdn:reserved */
  void* glSectionInfo;                                                      /* msdn:reserved */
  void* glSection;                                                          /* msdn:reserved */
  void* glTable;                                                            /* msdn:reserved */
  void* glCurrentRC;                                                        /* msdn:reserved */
  void* glContext;                                                          /* msdn:reserved */
  NtStatus LastStatusValue;                                                 /* msdn:reserved */
  struct NtUnicodeString StaticUnicodeString;                               /* msdn:reserved */
  char16_t StaticUnicodeBuffer[261];                                        /* msdn:reserved */
  void* DeallocationStack;                                                  /* msdn:reserved */
  void* TlsSlots[64];
  struct NtLinkedList TlsLinks;                                             /* msdn:reserved */
  void* Vdm;                                                                /* msdn:reserved */
  void* ReservedForNtRpc;                                                   /* msdn:reserved */
  void* DbgSsReserved[2];                                                   /* msdn:reserved */
  uint32_t HardErrorMode;                                                   /* msdn:reserved */
  void* Instrumentation[11];                                                /* msdn:reserved */
  struct NtGuid ActivityId;                                                 /* msdn:reserved */
  void* SubProcessTag;                                                      /* msdn:reserved */
  void* PerflibData;                                                        /* msdn:reserved */
  void* EtwTraceData;                                                       /* msdn:reserved */
  int64_t WinSockData;                                                      /* msdn:reserved */
  uint32_t GdiBatchCount;                                                   /* msdn:reserved */
  union {
    struct NtProcessorNumber CurrentIdealProcessor;
    uint32_t IdealProcessorValue;
    struct {
      unsigned char ReservedPad0;
      unsigned char ReservedPad1;
      unsigned char ReservedPad2;
      unsigned char IdealProcessor;
    };
  };                                                                        /* msdn:reserved */
  uint32_t GuaranteedStackBytes;                                            /* msdn:reserved */
  void* ReservedForPerf;                                                    /* msdn:reserved */
  void* ReservedForOle;
  uint32_t WaitingOnLoaderLock;                                             /* msdn:reserved */
  void* SavedPriorityState;                                                 /* msdn:reserved */
  uintptr_t ReservedForCodeCoverage;                                        /* msdn:reserved */
  void* ThreadPoolData;                                                     /* msdn:reserved */
  void** /* [sic] */ TlsExpansionSlots;
};

static_assert(offsetof(struct NtTeb, ProcessEnvironmentBlock) == 0x60);
static_assert(offsetof(struct NtTeb, TlsSlots) == 0x1480);
static_assert(offsetof(struct NtTeb, ReservedForOle) == 0x1758);
static_assert(offsetof(struct NtTeb, TlsExpansionSlots) == 0x1780);

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_TEB_H_ */
