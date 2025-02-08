#ifndef COSMOPOLITAN_LIBC_NT_ENUM_KEYACCESS_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_KEYACCESS_H_

#define kNtKeyQueryValue        0x00000001
#define kNtKeySetValue          0x00000002
#define kNtKeyCreateSubKey      0x00000004
#define kNtKeyEnumerateSubKeys  0x00000008
#define kNtKeyNotify            0x00000010
#define kNtKeyCreateLink        0x00000020
#define kNtWow6432Key           0x00000200
#define kNtWow6464Key           0x00000100
#define kNtWow64Res             0x00000300

#define kNtKeyRead      0x00020019
#define kNtKeyWrite     0x00020006
#define kNtKeyExecute   0x00020019
#define kNtKeyAllAccess 0x000f003f

#endif /* COSMOPOLITAN_LIBC_NT_ENUM_KEYACCESS_H_ */
