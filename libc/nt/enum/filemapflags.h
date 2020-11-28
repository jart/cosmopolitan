#ifndef COSMOPOLITAN_LIBC_NT_ENUM_FILEMAPFLAGS_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_FILEMAPFLAGS_H_

#define kNtFileMapCopy           0x00000001u
#define kNtFileMapWrite          0x00000002u
#define kNtFileMapRead           0x00000004u
#define kNtFileMapExecute        0x00000020u
#define kNtFileMapReserve        0x80000000u
#define kNtFileMapTargetsInvalid 0x40000000u
#define kNtFileMapLargePages     0x20000000u

#endif /* COSMOPOLITAN_LIBC_NT_ENUM_FILEMAPFLAGS_H_ */
