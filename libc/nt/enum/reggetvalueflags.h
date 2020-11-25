#ifndef COSMOPOLITAN_LIBC_NT_ENUM_REGGETVALUEFLAGS_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_REGGETVALUEFLAGS_H_

#define kNtRrfRtRegNone        0x00000001
#define kNtRrfRtRegSz          0x00000002
#define kNtRrfRtRegExpandSz    0x00000004
#define kNtRrfRtRegBinary      0x00000008
#define kNtRrfRtRegDword       0x00000010
#define kNtRrfRtRegMultiSz     0x00000020
#define kNtRrfRtRegQword       0x00000040
#define kNtRrfRtDword          (kNtRrfRtRegBinary | kNtRrfRtRegDword)
#define kNtRrfRtQword          (kNtRrfRtRegBinary | kNtRrfRtRegQword)
#define kNtRrfRtAny            0x0000ffff
#define kNtRrfSubkeyWow6464key 0x00010000
#define kNtRrfSubkeyWow6432key 0x00020000
#define kNtRrfWow64Mask        0x00030000
#define kNtRrfNoexpand         0x10000000
#define kNtRrfZeroonfailure    0x20000000

#endif /* COSMOPOLITAN_LIBC_NT_ENUM_REGGETVALUEFLAGS_H_ */
