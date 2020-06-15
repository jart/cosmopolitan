#ifndef COSMOPOLITAN_LIBC_NT_ENUM_FILETYPE_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_FILETYPE_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/**
 * NT Device File Types.
 *
 * @see GetFileType()
 * @see libc/sysv/consts.sh
 */
enum NtFileType {
  kNtFileTypeUnknown = 0x0000,
  kNtFileTypeDisk = 0x0001,  /* @see S_ISBLK() */
  kNtFileTypeChar = 0x0002,  /* @see S_ISCHR() */
  kNtFileTypePipe = 0x0003,  /* @see S_ISFIFO() */
  kNtFileTypeRemote = 0x8000 /* unused -MSDN */
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_ENUM_FILETYPE_H_ */
