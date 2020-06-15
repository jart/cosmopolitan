#ifndef COSMOPOLITAN_LIBC_NT_ENUM_MOVEFILEEXFLAGS_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_MOVEFILEEXFLAGS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

enum NtMoveFileExFlags {
  kNtMovefileReplaceExisting = 1,
  kNtMovefileCopyAllowed = 2,
  kNtMovefileDelayUntilReboot = 4,
  kNtMovefileCreateHardlink = 16,
  kNtMovefileFailIfNotTrackable = 32,
  kNtMovefileWriteThrough = 8
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_ENUM_MOVEFILEEXFLAGS_H_ */
