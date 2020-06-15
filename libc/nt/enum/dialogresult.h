#ifndef COSMOPOLITAN_LIBC_NT_ENUM_DIALOGRESULT_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_DIALOGRESULT_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

enum DialogResult {
  kNtIdok = 1,
  kNtIdcancel = 2,
  kNtIdabort = 3,
  kNtIdretry = 4,
  kNtIdignore = 5,
  kNtIdyes = 6,
  kNtIdno = 7,
  kNtIdclose = 8,
  kNtIdhelp = 9,
  kNtIdtryagain = 10,
  kNtIdcontinue = 11
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_ENUM_DIALOGRESULT_H_ */
