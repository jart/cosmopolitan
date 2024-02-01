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
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/log/log.h"
#include "libc/nt/dll.h"
#include "libc/nt/enum/filetype.h"
#include "libc/nt/enum/startf.h"
#include "libc/nt/files.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/ldr.h"
#include "libc/nt/struct/ldrdatatableentry.h"
#include "libc/nt/struct/linkedlist.h"
#include "libc/nt/struct/peb.h"
#include "libc/nt/struct/systeminfo.h"
#include "libc/nt/struct/teb.h"
#include "libc/nt/struct/unicodestring.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/consts/o.h"
#include "libc/time/time.h"
#include "tool/decode/lib/flagger.h"
#include "tool/decode/lib/idname.h"
#if defined(__x86_64__) && SupportsWindows()

char *GetString(const struct NtUnicodeString *s) {
  static char buf[1024];
  unsigned len = min(sizeof(buf) - 1, s->Length);
  for (unsigned i = 0; i < len; ++i) {
    buf[i] = (unsigned char)s->Data[i];
  }
  buf[len] = '\0';
  return &buf[0];
}

int NextBestThing(void) {
  int64_t fd = open("/proc/self/maps", O_RDONLY);
  posix_fadvise(fd, 0, 0, MADV_SEQUENTIAL);
  ssize_t wrote;
  while ((wrote = copyfd(fd, 1, -1)) != -1) {
    if (wrote == 0) break;
  }
  close(fd);
  return 0;
}

const struct IdName kNtStartfFlagNames[] = {
    {kNtStartfUseshowwindow, "kNtStartfUseshowwindow"},
    {kNtStartfUsesize, "kNtStartfUsesize"},
    {kNtStartfUseposition, "kNtStartfUseposition"},
    {kNtStartfUsecountchars, "kNtStartfUsecountchars"},
    {kNtStartfUsefillattribute, "kNtStartfUsefillattribute"},
    {kNtStartfRunfullscreen, "kNtStartfRunfullscreen"},
    {kNtStartfForceonfeedback, "kNtStartfForceonfeedback"},
    {kNtStartfForceofffeedback, "kNtStartfForceofffeedback"},
    {kNtStartfUsestdhandles, "kNtStartfUsestdhandles"},
    {kNtStartfUsehotkey, "kNtStartfUsehotkey"},
    {kNtStartfTitleislinkname, "kNtStartfTitleislinkname"},
    {kNtStartfTitleisappid, "kNtStartfTitleisappid"},
    {kNtStartfPreventpinning, "kNtStartfPreventpinning"},
    {kNtStartfUntrustedsource, "kNtStartfUntrustedsource"},
    {0, 0},
};

dontasan void PrintStartupInfo(void) {
#if 0
  printf("\n\
╔──────────────────────────────────────────────────────────────────────────────╗\n\
│ new technology § startup info                                                │\n\
╚──────────────────────────────────────────────────────────────────────────────╝\n\
\n");
#define X(D, F) \
  printf("%s.%-22s= " D "\n", "__nt_startupinfo", #F, __nt_startupinfo.F);
  X("%u", cb);
  X("%p", lpReserved);
  X("%hs", lpDesktop);
  X("%hs", lpTitle);
  X("%u", dwX);
  X("%u", dwY);
  X("%u", dwXSize);
  X("%u", dwYSize);
  X("%u", dwXCountChars);
  X("%u", dwYCountChars);
  X("%u", dwFillAttribute);
  printf("%s.%-22s: %s\n", "__nt_startupinfo", "dwFlags",
         RecreateFlags(kNtStartfFlagNames, __nt_startupinfo.dwFlags));
  X("%hu", wShowWindow);
  X("%hu", cbReserved2);
  X("%s", lpReserved2);
  X("%ld", hStdInput);
  X("%ld", hStdOutput);
  X("%ld", hStdError);
#undef X
#endif
}

void PrintSystemInfo(void) {
#if 0
  printf("\n\
╔──────────────────────────────────────────────────────────────────────────────╗\n\
│ new technology § system info                                                 │\n\
╚──────────────────────────────────────────────────────────────────────────────╝\n\
\n");
#define X(D, F) \
  printf("%s.%-28s= " D "\n", "__nt_systeminfo", #F, __nt_systeminfo.F);
  X("%08x", dwOemId);
  X("%04hx", wProcessorArchitecture);
  X("%d", dwPageSize);
  X("%p", lpMinimumApplicationAddress);
  X("%p", lpMaximumApplicationAddress);
  X("%p", dwActiveProcessorMask);
  X("%u", dwNumberOfProcessors);
  X("%u", dwProcessorType);
  X("%u", dwAllocationGranularity);
  X("%hu", wProcessorLevel);
  X("%hu", wProcessorRevision);
#undef X
#endif
}

const char *ft2str(int ft) {
  if (ft == kNtFileTypeUnknown) return "kNtFileTypeUnknown";
  if (ft == kNtFileTypeDisk) return "kNtFileTypeDisk";
  if (ft == kNtFileTypeChar) return "kNtFileTypeChar";
  if (ft == kNtFileTypePipe) return "kNtFileTypePipe";
  if (ft == kNtFileTypeRemote) return "kNtFileTypeRemote";
  return "wut?";
}

void PrintStdioInfo(void) {
  printf("\n\
╔──────────────────────────────────────────────────────────────────────────────╗\n\
│ new technology § stdio info                                                  │\n\
╚──────────────────────────────────────────────────────────────────────────────╝\n\
\n");
  printf("%s: %ld (%s)\n", "g_fds.p[0].handle", g_fds.p[0].handle,
         ft2str(GetFileType(g_fds.p[0].handle)));
  printf("%s: %ld (%s)\n", "g_fds.p[1].handle", g_fds.p[1].handle,
         ft2str(GetFileType(g_fds.p[1].handle)));
  printf("%s: %ld (%s)\n", "g_fds.p[2].handle", g_fds.p[2].handle,
         ft2str(GetFileType(g_fds.p[2].handle)));
}

dontasan void PrintTeb(void) {
  GetCurrentProcessId();
  SetLastError(0x1234);
  printf("\n\
╔──────────────────────────────────────────────────────────────────────────────╗\n\
│ new technology § teb?                                                        │\n\
╚──────────────────────────────────────────────────────────────────────────────╝\n\
\n");
  printf("gs:0x%02x: %-39s = 0x%lx\n", 0x00, "NtGetSeh()", _NtGetSeh());
  printf("gs:0x%02x: %-39s = 0x%lx\n", 0x08, "NtGetStackHigh()",
         _NtGetStackHigh());
  printf("gs:0x%02x: %-39s = 0x%lx\n", 0x10, "NtGetStackLow()",
         _NtGetStackLow());
  printf("gs:0x%02x: %-39s = 0x%lx\n", 0x18, "_NtGetSubsystemTib()",
         _NtGetSubsystemTib());
  printf("gs:0x%02x: %-39s = 0x%lx\n", 0x20, "NtGetFib()", _NtGetFib());
  printf("gs:0x%02x: %-39s = 0x%lx\n", 0x30, "NtGetTeb()", NtGetTeb());
  printf("gs:0x%02x: %-39s = 0x%lx\n", 0x38, "NtGetEnv()", _NtGetEnv());
  printf("gs:0x%02x: %-39s = 0x%lx\n", 0x40, "NtGetPid()", NtGetPid());
  printf("gs:0x%02x: %-39s = 0x%lx\n", 0x48, "NtGetTid()", NtGetTid());
  printf("gs:0x%02x: %-39s = 0x%lx\n", 0x50, "NtGetRpc()", _NtGetRpc());
  printf("gs:0x%02x: %-39s = 0x%lx\n", 0x58, "NtGetTls()", _NtGetTls());
  printf("gs:0x%02x: %-39s = 0x%lx\n", 0x60, "NtGetPeb()", NtGetPeb());
  printf("gs:0x%02x: %-39s = 0x%lx\n", 0x68, "NtGetErr()", NtGetErr());
}

void PrintPeb(void) {
  __seg_gs struct NtPeb *peb = NtGetPeb();
  printf("\n\
╔──────────────────────────────────────────────────────────────────────────────╗\n\
│ new technology § peb                                                         │\n\
╚──────────────────────────────────────────────────────────────────────────────╝\n\
\n");
  printf("0x%04x: %-40s = %u\n", offsetof(struct NtPeb, InheritedAddressSpace),
         "InheritedAddressSpace", (unsigned)peb->InheritedAddressSpace);
  printf("0x%04x: %-40s = %u\n",
         offsetof(struct NtPeb, ReadImageFileExecOptions),
         "ReadImageFileExecOptions", (unsigned)peb->ReadImageFileExecOptions);
  printf("0x%04x: %-40s = %u\n", offsetof(struct NtPeb, BeingDebugged),
         "BeingDebugged", (unsigned)peb->BeingDebugged);
  printf("0x%04x: %-40s = %u\n", offsetof(struct NtPeb, __wut1), "__wut1",
         (unsigned)peb->__wut1);
  printf("0x%04x: %-40s = 0x%lx\n", offsetof(struct NtPeb, Mutant), "Mutant",
         peb->Mutant);
  printf("0x%04x: %-40s = 0x%lx\n", offsetof(struct NtPeb, ImageBaseAddress),
         "ImageBaseAddress", peb->ImageBaseAddress);
  /* struct NtLdr *Ldr; */
  printf("0x%04x: %-40s = 0x%lx\n", offsetof(struct NtPeb, ProcessParameters),
         "ProcessParameters", peb->ProcessParameters);
  printf("0x%04x: %-40s = 0x%lx\n", offsetof(struct NtPeb, SubSystemData),
         "SubSystemData", peb->SubSystemData);
  printf("0x%04x: %-40s = 0x%lx\n", offsetof(struct NtPeb, ProcessHeap),
         "ProcessHeap", peb->ProcessHeap);
  printf("0x%04x: %-40s = 0x%lx\n", offsetof(struct NtPeb, FastPebLock),
         "FastPebLock", peb->FastPebLock);
  printf("0x%04x: %-40s = 0x%lx\n", offsetof(struct NtPeb, __wut3), "__wut3",
         peb->__wut3);
  printf("0x%04x: %-40s = 0x%lx\n", offsetof(struct NtPeb, __wut4), "__wut4",
         peb->__wut4);
  printf("0x%04x: %-40s = 0x%lx\n", offsetof(struct NtPeb, __wut5), "__wut5",
         peb->__wut5);
  printf("0x%04x: %-40s = 0x%lx\n", offsetof(struct NtPeb, KernelCallbackTable),
         "KernelCallbackTable", peb->KernelCallbackTable);
  printf("0x%04x: %-40s = 0x%lx\n", offsetof(struct NtPeb, UserSharedInfoPtr),
         "UserSharedInfoPtr", peb->UserSharedInfoPtr);
  printf("0x%04x: %-40s = 0x%x\n", offsetof(struct NtPeb, SystemReserved),
         "SystemReserved", peb->SystemReserved);
  printf("0x%04x: %-40s = 0x%x\n", offsetof(struct NtPeb, __wut6), "__wut6",
         peb->__wut6);
  printf("0x%04x: %-40s = 0x%lx\n", offsetof(struct NtPeb, __wut7), "__wut7",
         peb->__wut7);
  printf("0x%04x: %-40s = 0x%lx\n", offsetof(struct NtPeb, TlsExpansionCounter),
         "TlsExpansionCounter", peb->TlsExpansionCounter);
  printf("0x%04x: %-40s = 0x%lx\n", offsetof(struct NtPeb, TlsBitmap),
         "TlsBitmap", peb->TlsBitmap);
  printf("0x%04x: %-40s = 0x%x 0x%x\n", offsetof(struct NtPeb, TlsBitmapBits),
         "TlsBitmapBits", peb->TlsBitmapBits[0], peb->TlsBitmapBits[1]);
  printf("0x%04x: %-40s = 0x%lx\n",
         offsetof(struct NtPeb, ReadOnlySharedMemoryBase),
         "ReadOnlySharedMemoryBase", peb->ReadOnlySharedMemoryBase);
  printf("0x%04x: %-40s = 0x%lx\n", offsetof(struct NtPeb, __wut8), "__wut8",
         peb->__wut8);
  printf("0x%04x: %-40s = 0x%lx\n",
         offsetof(struct NtPeb, ReadOnlyStaticServerData),
         "ReadOnlyStaticServerData", peb->ReadOnlyStaticServerData);
  printf("0x%04x: %-40s = 0x%lx\n", offsetof(struct NtPeb, AnsiCodePageData),
         "AnsiCodePageData", peb->AnsiCodePageData);
  printf("0x%04x: %-40s = 0x%lx\n", offsetof(struct NtPeb, OemCodePageData),
         "OemCodePageData", peb->OemCodePageData);
  printf("0x%04x: %-40s = 0x%lx\n",
         offsetof(struct NtPeb, UnicodeCaseTableData), "UnicodeCaseTableData",
         peb->UnicodeCaseTableData);
  printf("0x%04x: %-40s = 0x%x\n", offsetof(struct NtPeb, NumberOfProcessors),
         "NumberOfProcessors", peb->NumberOfProcessors);
  printf("0x%04x: %-40s = 0x%x\n", offsetof(struct NtPeb, NtGlobalFlag),
         "NtGlobalFlag", peb->NtGlobalFlag);
  printf("0x%04x: %-40s = %ld\n",
         offsetof(struct NtPeb, CriticalSectionTimeout),
         "CriticalSectionTimeout", peb->CriticalSectionTimeout);
  printf("0x%04x: %-40s = 0x%lx\n", offsetof(struct NtPeb, HeapSegmentReserve),
         "HeapSegmentReserve", peb->HeapSegmentReserve);
  printf("0x%04x: %-40s = 0x%lx\n", offsetof(struct NtPeb, HeapSegmentCommit),
         "HeapSegmentCommit", peb->HeapSegmentCommit);
  printf("0x%04x: %-40s = 0x%lx\n",
         offsetof(struct NtPeb, HeapDeCommitTotalFreeThreshold),
         "HeapDeCommitTotalFreeThreshold", peb->HeapDeCommitTotalFreeThreshold);
  printf("0x%04x: %-40s = 0x%lx\n",
         offsetof(struct NtPeb, HeapDeCommitFreeBlockThreshold),
         "HeapDeCommitFreeBlockThreshold", peb->HeapDeCommitFreeBlockThreshold);
  printf("0x%04x: %-40s = 0x%x\n", offsetof(struct NtPeb, NumberOfHeaps),
         "NumberOfHeaps", peb->NumberOfHeaps);
  printf("0x%04x: %-40s = 0x%x\n", offsetof(struct NtPeb, MaximumNumberOfHeaps),
         "MaximumNumberOfHeaps", peb->MaximumNumberOfHeaps);
  printf("0x%04x: %-40s = 0x%lx\n", offsetof(struct NtPeb, ProcessHeaps),
         "ProcessHeaps", peb->ProcessHeaps);
  printf("0x%04x: %-40s = 0x%lx\n",
         offsetof(struct NtPeb, GdiSharedHandleTable), "GdiSharedHandleTable",
         peb->GdiSharedHandleTable);
  printf("0x%04x: %-40s = 0x%lx\n",
         offsetof(struct NtPeb, ProcessStarterHelper), "ProcessStarterHelper",
         peb->ProcessStarterHelper);
  printf("0x%04x: %-40s = 0x%lx\n", offsetof(struct NtPeb, GdiDCAttributeList),
         "GdiDCAttributeList", peb->GdiDCAttributeList);
  printf("0x%04x: %-40s = 0x%lx\n", offsetof(struct NtPeb, LoaderLock),
         "LoaderLock", peb->LoaderLock);
  printf("0x%04x: %-40s = 0x%x\n", offsetof(struct NtPeb, OSMajorVersion),
         "OSMajorVersion", peb->OSMajorVersion);
  printf("0x%04x: %-40s = 0x%x\n", offsetof(struct NtPeb, OSMinorVersion),
         "OSMinorVersion", peb->OSMinorVersion);
  printf("0x%04x: %-40s = %hu\n", offsetof(struct NtPeb, OSBuildNumber),
         "OSBuildNumber", peb->OSBuildNumber);
  printf("0x%04x: %-40s = %hu\n", offsetof(struct NtPeb, OSCSDVersion),
         "OSCSDVersion", peb->OSCSDVersion);
  printf("0x%04x: %-40s = 0x%x\n", offsetof(struct NtPeb, OSPlatformId),
         "OSPlatformId", peb->OSPlatformId);
  printf("0x%04x: %-40s = 0x%x\n", offsetof(struct NtPeb, ImageSubsystem),
         "ImageSubsystem", peb->ImageSubsystem);
  printf("0x%04x: %-40s = 0x%x\n",
         offsetof(struct NtPeb, ImageSubsystemMajorVersion),
         "ImageSubsystemMajorVersion", peb->ImageSubsystemMajorVersion);
  printf("0x%04x: %-40s = 0x%lx\n",
         offsetof(struct NtPeb, ImageSubsystemMinorVersion),
         "ImageSubsystemMinorVersion", peb->ImageSubsystemMinorVersion);
  printf("0x%04x: %-40s = 0x%lx\n",
         offsetof(struct NtPeb, ImageProcessAffinityMask),
         "ImageProcessAffinityMask", peb->ImageProcessAffinityMask);
  printf("0x%04x: %-40s = 0x%lx\n",
         offsetof(struct NtPeb, ActiveProcessAffinityMask),
         "ActiveProcessAffinityMask", peb->ActiveProcessAffinityMask);
  /* "0x%lx", GdiHandleBuffer[38 - __SIZEOF_POINTER__]; */
  printf("0x%04x: %-40s = 0x%lx\n",
         offsetof(struct NtPeb, PostProcessInitRoutine),
         "PostProcessInitRoutine", peb->PostProcessInitRoutine);
  printf("0x%04x: %-40s = 0x%lx\n", offsetof(struct NtPeb, TlsExpansionBitmap),
         "TlsExpansionBitmap", peb->TlsExpansionBitmap);
  /* "0x%x", TlsExpansionBitmapBits[32]; */
  printf("0x%04x: %-40s = 0x%lx\n", offsetof(struct NtPeb, SessionId),
         "SessionId", peb->SessionId);
  printf("0x%04x: %-40s = 0x%lx\n", offsetof(struct NtPeb, AppCompatFlags),
         "AppCompatFlags", peb->AppCompatFlags);
  printf("0x%04x: %-40s = 0x%lx\n", offsetof(struct NtPeb, AppCompatFlagsUser),
         "AppCompatFlagsUser", peb->AppCompatFlagsUser);
  printf("0x%04x: %-40s = 0x%lx\n", offsetof(struct NtPeb, pShimData),
         "pShimData", peb->pShimData);
  printf("0x%04x: %-40s = 0x%lx\n", offsetof(struct NtPeb, AppCompatInfo),
         "AppCompatInfo", peb->AppCompatInfo);
  printf("0x%04x: %-40s = 0x%lx\n",
         offsetof(struct NtPeb, ActivationContextData), "ActivationContextData",
         peb->ActivationContextData);
  printf("0x%04x: %-40s = 0x%lx\n",
         offsetof(struct NtPeb, ProcessAssemblyStorageMap),
         "ProcessAssemblyStorageMap", peb->ProcessAssemblyStorageMap);
  printf("0x%04x: %-40s = 0x%lx\n",
         offsetof(struct NtPeb, SystemDefaultActivationContextData),
         "SystemDefaultActivationContextData",
         peb->SystemDefaultActivationContextData);
  printf("0x%04x: %-40s = 0x%lx\n",
         offsetof(struct NtPeb, SystemAssemblyStorageMap),
         "SystemAssemblyStorageMap", peb->SystemAssemblyStorageMap);
  printf("0x%04x: %-40s = 0x%lx\n", offsetof(struct NtPeb, MinimumStackCommit),
         "MinimumStackCommit", peb->MinimumStackCommit);
}

void PrintPebLdr(void) {
  printf("\n\
╔──────────────────────────────────────────────────────────────────────────────╗\n\
│ new technology § peb » ldr                                                   │\n\
╚──────────────────────────────────────────────────────────────────────────────╝\n\
\n");
  printf("0x%04x: %-40s = 0x%lx\n", offsetof(struct NtLdr, SizeOfThis),
         "SizeOfThis", NtGetPeb()->Ldr->SizeOfThis);
  printf("0x%04x: %-40s = 0x%lx\n", offsetof(struct NtLdr, IsInitialized),
         "IsInitialized", NtGetPeb()->Ldr->IsInitialized);
  printf("0x%04x: %-40s = 0x%lx\n", offsetof(struct NtLdr, SsHandle),
         "SsHandle", NtGetPeb()->Ldr->SsHandle);
  printf("0x%04x: %-40s = 0x%lx\n",
         offsetof(struct NtLdr, InLoadOrderModuleList), "InLoadOrderModuleList",
         NtGetPeb()->Ldr->InLoadOrderModuleList);
  printf("0x%04x: %-40s = 0x%lx\n",
         offsetof(struct NtLdr, InMemoryOrderModuleList),
         "InMemoryOrderModuleList", NtGetPeb()->Ldr->InMemoryOrderModuleList);
  printf("0x%04x: %-40s = 0x%lx\n",
         offsetof(struct NtLdr, InInitOrderModuleList), "InInitOrderModuleList",
         NtGetPeb()->Ldr->InInitOrderModuleList);
}

void PrintModulesLoadOrder(void) {
  {
    printf("\n\
╔──────────────────────────────────────────────────────────────────────────────╗\n\
│ new technology § modules » load order                                        │\n\
╚──────────────────────────────────────────────────────────────────────────────╝\n\
\n");
    struct NtLinkedList *head = &NtGetPeb()->Ldr->InLoadOrderModuleList;
    struct NtLinkedList *ldr = head->Next;
    do {
      const struct NtLdrDataTableEntry *dll =
          (const struct NtLdrDataTableEntry *)ldr;
      /* struct NtLinkedList InLoadOrderLinks; /\* msdn:reserved *\/ */
      /* struct NtLinkedList InMemoryOrderLinks; */
      /* struct NtLinkedList InInitOrderLinks; /\* msdn:reserved *\/ */
      printf("%p\n", ldr);
      printf("%p vs. %p\n", dll, GetModuleHandleW(dll->FullDllName.Data));
      printf("0x%04x: %-40s = 0x%lx\n",
             offsetof(struct NtLdrDataTableEntry, DllBase), "DllBase",
             dll->DllBase);
      printf("0x%04x: %-40s = 0x%lx\n",
             offsetof(struct NtLdrDataTableEntry, EntryPoint), "EntryPoint",
             dll->EntryPoint);
      printf("0x%04x: %-40s = 0x%x\n",
             offsetof(struct NtLdrDataTableEntry, SizeOfImage), "SizeOfImage",
             dll->SizeOfImage);
      printf("0x%04x: %-40s = \"%s\"\n",
             offsetof(struct NtLdrDataTableEntry, FullDllName), "FullDllName",
             GetString(&dll->FullDllName));
      printf("0x%04x: %-40s = \"%s\"\n",
             offsetof(struct NtLdrDataTableEntry, BaseDllName), "BaseDllName",
             GetString(&dll->BaseDllName));
      printf("0x%04x: %-40s = 0x%x\n",
             offsetof(struct NtLdrDataTableEntry, Flags), "Flags", dll->Flags);
      printf("0x%04x: %-40s = %hu\n",
             offsetof(struct NtLdrDataTableEntry, Load_Count), "Load_Count",
             dll->Load_Count);
      printf("0x%04x: %-40s = %hu\n",
             offsetof(struct NtLdrDataTableEntry, TlsIndex), "TlsIndex",
             dll->TlsIndex);
      /* union { */
      /*   struct NtLinkedList HashLinks; */
      /*   struct { */
      /*     void *SectionPointer; */
      /*     uint32_t CheckSum; */
      /*   }; */
      /* }; */
      /* union { */
      /*   void *LoadedImports; */
      /*   uint32_t TimeDateStamp; */
      /* }; */
      printf("0x%04x: %-40s = 0x%lx\n",
             offsetof(struct NtLdrDataTableEntry, EntryPointActivationContext),
             "EntryPointActivationContext", dll->EntryPointActivationContext);
      printf("0x%04x: %-40s = 0x%lx\n",
             offsetof(struct NtLdrDataTableEntry, PatchInformation),
             "PatchInformation", dll->PatchInformation);
      /* struct NtLinkedList ForwarderLinks; */
      /* struct NtLinkedList ServiceTagLinks; */
      /* struct NtLinkedList StaticLinks; */
      printf("0x%04x: %-40s = 0x%lx\n",
             offsetof(struct NtLdrDataTableEntry, ContextInformation),
             "ContextInformation", dll->ContextInformation);
      printf("0x%04x: %-40s = 0x%lx\n",
             offsetof(struct NtLdrDataTableEntry, OriginalBase), "OriginalBase",
             dll->OriginalBase);
      printf("0x%04x: %-40s = %ld\n",
             offsetof(struct NtLdrDataTableEntry, LoadTime), "LoadTime",
             dll->LoadTime);
      printf("\n");
    } while ((ldr = ldr->Next) && ldr != head);
  }
}

void PrintModulesMemoryOrder(void) {
  {
    printf("\n\
╔──────────────────────────────────────────────────────────────────────────────╗\n\
│ new technology § modules » memory order                                      │\n\
╚──────────────────────────────────────────────────────────────────────────────╝\n\
\n");
    struct NtLinkedList *head = &NtGetPeb()->Ldr->InMemoryOrderModuleList;
    struct NtLinkedList *ldr = head->Next;
    do {
      const struct NtLdrDataTableEntry *dll =
          (const struct NtLdrDataTableEntry *)ldr;
      /* struct NtLinkedList InLoadOrderLinks; /\* msdn:reserved *\/ */
      /* struct NtLinkedList InMemoryOrderLinks; */
      /* struct NtLinkedList InInitOrderLinks; /\* msdn:reserved *\/ */
      printf("%p\n", dll);
      printf("0x%04x: %-40s = 0x%lx\n",
             offsetof(struct NtLdrDataTableEntry, DllBase), "DllBase",
             dll->DllBase);
      printf("0x%04x: %-40s = 0x%lx\n",
             offsetof(struct NtLdrDataTableEntry, EntryPoint), "EntryPoint",
             dll->EntryPoint);
      printf("0x%04x: %-40s = 0x%x\n",
             offsetof(struct NtLdrDataTableEntry, SizeOfImage), "SizeOfImage",
             dll->SizeOfImage);
      printf("0x%04x: %-40s = \"%s\"\n",
             offsetof(struct NtLdrDataTableEntry, FullDllName), "FullDllName",
             GetString(&dll->FullDllName));
      printf("0x%04x: %-40s = \"%s\"\n",
             offsetof(struct NtLdrDataTableEntry, BaseDllName), "BaseDllName",
             GetString(&dll->BaseDllName));
      printf("0x%04x: %-40s = 0x%x\n",
             offsetof(struct NtLdrDataTableEntry, Flags), "Flags", dll->Flags);
      printf("0x%04x: %-40s = %hu\n",
             offsetof(struct NtLdrDataTableEntry, Load_Count), "Load_Count",
             dll->Load_Count);
      printf("0x%04x: %-40s = %hu\n",
             offsetof(struct NtLdrDataTableEntry, TlsIndex), "TlsIndex",
             dll->TlsIndex);
      /* /\* union { *\/ */
      /* /\*   struct NtLinkedList HashLinks; *\/ */
      /* /\*   struct { *\/ */
      /* /\*     void *SectionPointer; *\/ */
      /* /\*     uint32_t CheckSum; *\/ */
      /* /\*   }; *\/ */
      /* /\* }; *\/ */
      /* /\* union { *\/ */
      /* /\*   void *LoadedImports; *\/ */
      /* /\*   uint32_t TimeDateStamp; *\/ */
      /* /\* }; *\/ */
      /* printf("0x%04x: %-40s = 0x%lx\n", */
      /*        offsetof(struct NtLdrDataTableEntry,
       * EntryPointActivationContext), */
      /*        "EntryPointActivationContext",
       * dll->EntryPointActivationContext); */
      /* printf("0x%04x: %-40s = 0x%lx\n", */
      /*        offsetof(struct NtLdrDataTableEntry, PatchInformation), */
      /*        "PatchInformation", dll->PatchInformation); */
      /* /\* struct NtLinkedList ForwarderLinks; *\/ */
      /* /\* struct NtLinkedList ServiceTagLinks; *\/ */
      /* /\* struct NtLinkedList StaticLinks; *\/ */
      /* printf("0x%04x: %-40s = 0x%lx\n", */
      /*        offsetof(struct NtLdrDataTableEntry, ContextInformation), */
      /*        "ContextInformation", dll->ContextInformation); */
      /* printf("0x%04x: %-40s = 0x%lx\n", */
      /*        offsetof(struct NtLdrDataTableEntry, OriginalBase),
       * "OriginalBase", */
      /*        dll->OriginalBase); */
      /* printf("0x%04x: %-40s = %ld\n", */
      /*        offsetof(struct NtLdrDataTableEntry, LoadTime), "LoadTime", */
      /*        dll->LoadTime); */
      printf("\n");
    } while ((ldr = ldr->Next) && ldr != head);
  }
}

int main(int argc, char *argv[]) {
  if (IsLinux()) {
    return NextBestThing();
  }
  if (!IsWindows()) {
    fprintf(stderr, "error: this tool is intended for windows\n");
    return 1;
  }
  PrintStartupInfo();
  PrintSystemInfo();
  PrintStdioInfo();
  PrintTeb();
  PrintPeb();
  PrintPebLdr();
  PrintModulesLoadOrder();
  PrintModulesMemoryOrder();
  return 0;
}

#else
int main(int argc, char *argv[]) {
  fprintf(stderr, "printpeb not supported on this cpu arch or build config\n");
  return 1;
}
#endif /* __x86_64__ && SupportsWindows() */
