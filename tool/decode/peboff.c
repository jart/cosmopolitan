#include "libc/nt/struct/peb.h"
#include "libc/stdio/stdio.h"

int main() {
  printf("InheritedAddressSpace = 0x%x\n",
         offsetof(struct NtPeb, InheritedAddressSpace));
  printf("ReadImageFileExecOptions = 0x%x\n",
         offsetof(struct NtPeb, ReadImageFileExecOptions));
  printf("BeingDebugged = 0x%x\n", offsetof(struct NtPeb, BeingDebugged));
  printf("Mutant = 0x%x\n", offsetof(struct NtPeb, Mutant));
  printf("ImageBaseAddress = 0x%x\n", offsetof(struct NtPeb, ImageBaseAddress));
  printf("Ldr = 0x%x\n", offsetof(struct NtPeb, Ldr));
  printf("ProcessParameters = 0x%x\n",
         offsetof(struct NtPeb, ProcessParameters));
  printf("SubSystemData = 0x%x\n", offsetof(struct NtPeb, SubSystemData));
  printf("ProcessHeap = 0x%x\n", offsetof(struct NtPeb, ProcessHeap));
  printf("FastPebLock = 0x%x\n", offsetof(struct NtPeb, FastPebLock));
  printf("KernelCallbackTable = 0x%x\n",
         offsetof(struct NtPeb, KernelCallbackTable));
  printf("UserSharedInfoPtr = 0x%x\n",
         offsetof(struct NtPeb, UserSharedInfoPtr));
  printf("SystemReserved = 0x%x\n", offsetof(struct NtPeb, SystemReserved));
  printf("__wut6 = 0x%x\n", offsetof(struct NtPeb, __wut6));
  printf("__wut7 = 0x%x\n", offsetof(struct NtPeb, __wut7));
  printf("TlsExpansionCounter = 0x%x\n",
         offsetof(struct NtPeb, TlsExpansionCounter));
  printf("TlsBitmap = 0x%x\n", offsetof(struct NtPeb, TlsBitmap));
  printf("TlsBitmapBits = 0x%x\n", offsetof(struct NtPeb, TlsBitmapBits));
  printf("ReadOnlySharedMemoryBase = 0x%x\n",
         offsetof(struct NtPeb, ReadOnlySharedMemoryBase));
  printf("ReadOnlyStaticServerData = 0x%x\n",
         offsetof(struct NtPeb, ReadOnlyStaticServerData));
  printf("AnsiCodePageData = 0x%x\n", offsetof(struct NtPeb, AnsiCodePageData));
  printf("OemCodePageData = 0x%x\n", offsetof(struct NtPeb, OemCodePageData));
  printf("UnicodeCaseTableData = 0x%x\n",
         offsetof(struct NtPeb, UnicodeCaseTableData));
  printf("NumberOfProcessors = 0x%x\n",
         offsetof(struct NtPeb, NumberOfProcessors));
  printf("NtGlobalFlag = 0x%x\n", offsetof(struct NtPeb, NtGlobalFlag));
  printf("CriticalSectionTimeout = 0x%x\n",
         offsetof(struct NtPeb, CriticalSectionTimeout));
  printf("HeapSegmentReserve = 0x%x\n",
         offsetof(struct NtPeb, HeapSegmentReserve));
  printf("HeapSegmentCommit = 0x%x\n",
         offsetof(struct NtPeb, HeapSegmentCommit));
  printf("HeapDeCommitTotalFreeThreshold = 0x%x\n",
         offsetof(struct NtPeb, HeapDeCommitTotalFreeThreshold));
  printf("HeapDeCommitFreeBlockThreshold = 0x%x\n",
         offsetof(struct NtPeb, HeapDeCommitFreeBlockThreshold));
  printf("NumberOfHeaps = 0x%x\n", offsetof(struct NtPeb, NumberOfHeaps));
  printf("MaximumNumberOfHeaps = 0x%x\n",
         offsetof(struct NtPeb, MaximumNumberOfHeaps));
  printf("ProcessHeaps = 0x%x\n", offsetof(struct NtPeb, ProcessHeaps));
  printf("GdiSharedHandleTable = 0x%x\n",
         offsetof(struct NtPeb, GdiSharedHandleTable));
  printf("ProcessStarterHelper = 0x%x\n",
         offsetof(struct NtPeb, ProcessStarterHelper));
  printf("GdiDCAttributeList = 0x%x\n",
         offsetof(struct NtPeb, GdiDCAttributeList));
  printf("LoaderLock = 0x%x\n", offsetof(struct NtPeb, LoaderLock));
  printf("OSMajorVersion = 0x%x\n", offsetof(struct NtPeb, OSMajorVersion));
  printf("OSMinorVersion = 0x%x\n", offsetof(struct NtPeb, OSMinorVersion));
  printf("OSVersion = 0x%x\n", offsetof(struct NtPeb, OSVersion));
  printf("OSBuildNumber = 0x%x\n", offsetof(struct NtPeb, OSBuildNumber));
  printf("OSCSDVersion = 0x%x\n", offsetof(struct NtPeb, OSCSDVersion));
  printf("OSPlatformId = 0x%x\n", offsetof(struct NtPeb, OSPlatformId));
  printf("ImageSubsystem = 0x%x\n", offsetof(struct NtPeb, ImageSubsystem));
  printf("ImageSubsystemMajorVersion = 0x%x\n",
         offsetof(struct NtPeb, ImageSubsystemMajorVersion));
  printf("ImageSubsystemMinorVersion = 0x%x\n",
         offsetof(struct NtPeb, ImageSubsystemMinorVersion));
  printf("ImageProcessAffinityMask = 0x%x\n",
         offsetof(struct NtPeb, ImageProcessAffinityMask));
  printf("ActiveProcessAffinityMask = 0x%x\n",
         offsetof(struct NtPeb, ActiveProcessAffinityMask));
  printf("GdiHandleBuffer = 0x%x\n", offsetof(struct NtPeb, GdiHandleBuffer));
  printf("PostProcessInitRoutine = 0x%x\n",
         offsetof(struct NtPeb, PostProcessInitRoutine));
  printf("TlsExpansionBitmap = 0x%x\n",
         offsetof(struct NtPeb, TlsExpansionBitmap));
  printf("TlsExpansionBitmapBits = 0x%x\n",
         offsetof(struct NtPeb, TlsExpansionBitmapBits));
  printf("SessionId = 0x%x\n", offsetof(struct NtPeb, SessionId));
  printf("AppCompatFlags = 0x%x\n", offsetof(struct NtPeb, AppCompatFlags));
  printf("AppCompatFlagsUser = 0x%x\n",
         offsetof(struct NtPeb, AppCompatFlagsUser));
  printf("pShimData = 0x%x\n", offsetof(struct NtPeb, pShimData));
  printf("AppCompatInfo = 0x%x\n", offsetof(struct NtPeb, AppCompatInfo));
  printf("CSDVersion = 0x%x\n", offsetof(struct NtPeb, CSDVersion));
  printf("ActivationContextData = 0x%x\n",
         offsetof(struct NtPeb, ActivationContextData));
  printf("ProcessAssemblyStorageMap = 0x%x\n",
         offsetof(struct NtPeb, ProcessAssemblyStorageMap));
  printf("SystemDefaultActivationContextData = 0x%x\n",
         offsetof(struct NtPeb, SystemDefaultActivationContextData));
  printf("SystemAssemblyStorageMap = 0x%x\n",
         offsetof(struct NtPeb, SystemAssemblyStorageMap));
  printf("MinimumStackCommit = 0x%x\n",
         offsetof(struct NtPeb, MinimumStackCommit));
  return 0;
}
