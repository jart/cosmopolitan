#ifndef COSMOPOLITAN_LIBC_NT_ENUM_FILETYPE_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_FILETYPE_H_

#define kNtFileTypeUnknown 0x0000
#define kNtFileTypeDisk    0x0001 /* @see S_ISBLK() */
#define kNtFileTypeChar    0x0002 /* @see S_ISCHR() */
#define kNtFileTypePipe    0x0003 /* @see S_ISFIFO() */
#define kNtFileTypeRemote  0x8000 /* unused -MSDN */

#endif /* COSMOPOLITAN_LIBC_NT_ENUM_FILETYPE_H_ */
