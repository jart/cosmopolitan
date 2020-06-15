#ifndef COSMOPOLITAN_LIBC_NT_ENUM_JOBOBJECTINFOCLASS_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_JOBOBJECTINFOCLASS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

enum NtJobObjectInfoClass {
  kNtJobObjectBasicAccountingInformation = 1,
  kNtJobObjectBasicLimitInformation,
  kNtJobObjectBasicProcessIdList,
  kNtJobObjectBasicUIRestrictions,
  kNtJobObjectSecurityLimitInformation,
  kNtJobObjectEndOfJobTimeInformation,
  kNtJobObjectAssociateCompletionPortInformation,
  kNtJobObjectBasicAndIoAccountingInformation,
  kNtJobObjectExtendedLimitInformation,
  kNtJobObjectJobSetInformation,
  kNtJobObjectInfoClass_MAX
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_ENUM_JOBOBJECTINFOCLASS_H_ */
