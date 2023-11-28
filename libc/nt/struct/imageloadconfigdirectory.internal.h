#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGELOADCONFIGDIRECTORY_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGELOADCONFIGDIRECTORY_H_
#include "libc/nt/struct/imageloadconfigcodeintegrity.internal.h"

struct NtImageLoadConfigDirectory {
  uint32_t Size;
  uint32_t TimeDateStamp;
  uint16_t MajorVersion;
  uint16_t MinorVersion;
  uint32_t GlobalFlagsClear;
  uint32_t GlobalFlagsSet;
  uint32_t CriticalSectionDefaultTimeout;
  uint64_t DeCommitFreeBlockThreshold;
  uint64_t DeCommitTotalFreeThreshold;
  uint64_t LockPrefixTable;
  uint64_t MaximumAllocationSize;
  uint64_t VirtualMemoryThreshold;
  uint64_t ProcessAffinityMask;
  uint32_t ProcessHeapFlags;
  uint16_t CSDVersion;
  uint16_t DependentLoadFlags;
  uint64_t EditList;
  uint64_t SecurityCookie;
  uint64_t SEHandlerTable;
  uint64_t SEHandlerCount;
  uint64_t GuardCFCheckFunctionPointer;
  uint64_t GuardCFDispatchFunctionPointer;
  uint64_t GuardCFFunctionTable;
  uint64_t GuardCFFunctionCount;
  uint32_t GuardFlags;
  struct NtImageLoadConfigCodeIntegrity CodeIntegrity;
  uint64_t GuardAddressTakenIatEntryTable;
  uint64_t GuardAddressTakenIatEntryCount;
  uint64_t GuardLongJumpTargetTable;
  uint64_t GuardLongJumpTargetCount;
  uint64_t DynamicValueRelocTable;
  uint64_t CHPEMetadataPointer;
  uint64_t GuardRFFailureRoutine;
  uint64_t GuardRFFailureRoutineFunctionPointer;
  uint32_t DynamicValueRelocTableOffset;
  uint16_t DynamicValueRelocTableSection;
  uint16_t Reserved2;
  uint64_t GuardRFVerifyStackPointerFunctionPointer;
  uint32_t HotPatchTableOffset;
  uint32_t Reserved3;
  uint64_t EnclaveConfigurationPointer;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGELOADCONFIGDIRECTORY_H_ */
