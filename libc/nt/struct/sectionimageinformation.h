#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_SECTIONIMAGEINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_SECTIONIMAGEINFORMATION_H_

struct NtSectionImageInformation {
  void *EntryPoint;
  uint32_t Unknown1;
  uint32_t StackReserve;
  uint32_t StackCommit;
  uint32_t Subsystem;
  uint16_t MinorSubsystemVersion;
  uint16_t MajorSubsystemVersion;
  uint32_t Unknown2;
  uint32_t Characteristics;
  uint16_t ImageNumber;
  uint32_t IsExecutable;
  uint8_t __wut1;
  uint32_t __wut2[3];
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_SECTIONIMAGEINFORMATION_H_ */
