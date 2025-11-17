#ifndef COSMOPOLITAN_LIBC_NT_ENUM_LOCALMEM_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_LOCALMEM_H_

#define kNtLmemFixed         0x0000
#define kNtLmemMoveable      0x0002
#define kNtLmemNocompact     0x0010
#define kNtLmemNodiscard     0x0020
#define kNtLmemZeroinit      0x0040
#define kNtLmemModify        0x0080
#define kNtLmemDiscardable   0x0F00
#define kNtLmemValidFlags    0x0F72
#define kNtLmemInvalidHandle 0x8000

#define kNtLhnd (kNtLmemMoveable | kNtLmemZeroinit)
#define kNtLptr (kNtLmemFixed | kNtLmemZeroinit)

#endif /* COSMOPOLITAN_LIBC_NT_ENUM_LOCALMEM_H_ */
