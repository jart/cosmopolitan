#ifndef COSMOPOLITAN_LIBC_NT_ENUM_VERSION_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_VERSION_H_

/**
 * Known versions of the New Technology executive.
 * @see IsAtLeastWindows10()
 * @see NtGetVersion()
 */
#define kNtVersionWindows10    0x0a00
#define kNtVersionWindows81    0x0603
#define kNtVersionWindows8     0x0602
#define kNtVersionWindows7     0x0601
#define kNtVersionWindowsVista 0x0600 /* intended baseline */
#define kNtVersionWindowsXp64  0x0502 /* end of the road */
#define kNtVersionWindowsXp    0x0501 /* snowball's chance */
#define kNtVersionWindows2000  0x0500 /* the golden age */
#define kNtVersionFuture       0x0b00

#endif /* COSMOPOLITAN_LIBC_NT_ENUM_VERSION_H_ */
