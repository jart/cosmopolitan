#ifndef COSMOPOLITAN_LIBC_NT_ENUM_SECURITYIMPERSONATIONLEVEL_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_SECURITYIMPERSONATIONLEVEL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

enum NtSecurityImpersonationLevel {
  kNtSecurityAnonymous,
  kNtSecurityIdentification,
  kNtSecurityImpersonation,
  kNtSecurityDelegation
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_ENUM_SECURITYIMPERSONATIONLEVEL_H_ */
