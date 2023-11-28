#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_FILEFSFULLSIZEINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_FILEFSFULLSIZEINFORMATION_H_
COSMOPOLITAN_C_START_

struct NtFileFsFullSizeInformation {
  int64_t TotalAllocationUnits;
  int64_t CallerAvailableAllocationUnits;
  int64_t ActualAvailableAllocationUnits;
  uint32_t SectorsPerAllocationUnit;
  uint32_t BytesPerSector;
};

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_FILEFSFULLSIZEINFORMATION_H_ */
