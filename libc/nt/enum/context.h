#ifndef COSMOPOLITAN_LIBC_NT_ENUM_CONTEXT_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_CONTEXT_H_

#define kNtContextAmd64 0x00100000

#define kNtContextControl        (kNtContextAmd64 | 0x00000001)
#define kNtContextInteger        (kNtContextAmd64 | 0x00000002)
#define kNtContextSegments       (kNtContextAmd64 | 0x00000004)
#define kNtContextFloatingPoint  (kNtContextAmd64 | 0x00000008)
#define kNtContextDebugRegisters (kNtContextAmd64 | 0x00000010)

#define kNtContextFull \
  (kNtContextControl | kNtContextInteger | kNtContextFloatingPoint)

#define kNtContextAll                                           \
  (kNtContextControl | kNtContextInteger | kNtContextSegments | \
   kNtContextFloatingPoint | kNtContextDebugRegisters)

#define kNtContextXstate (kNtContextAmd64 | 0x00000040)

#endif /* COSMOPOLITAN_LIBC_NT_ENUM_CONTEXT_H_ */
