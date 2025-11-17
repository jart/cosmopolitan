#ifndef COSMOPOLITAN_LIBC_NT_ENUM_COMPRESSION_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_COMPRESSION_H_

#define kNtCompressionFormatNone       0x0000
#define kNtCompressionFormatDefault    0x0001
#define kNtCompressionFormatLznt1      0x0002
#define kNtCompressionFormatXpress     0x0003
#define kNtCompressionFormatXpressHuff 0x0004
#define kNtCompressionFormatXp10       0x0005

#define kNtCompressionEngineStandard 0x0000
#define kNtCompressionEngineMaximum  0x0100
#define kNtCompressionEngineHiber    0x0200

#endif /* COSMOPOLITAN_LIBC_NT_ENUM_COMPRESSION_H_ */
