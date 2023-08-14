#ifndef COSMOPOLITAN_LIBC_STR_TAB_INTERNAL_H_
#define COSMOPOLITAN_LIBC_STR_TAB_INTERNAL_H_

#define kHexToInt __kHexToInt
#define kToLower  __kToLower
#define kToUpper  __kToUpper
#define kBase36   __kBase36
#define kCp437    __kCp437

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define gperf_downcase kToLower

extern const int8_t kHexToInt[256];
extern const uint8_t kToLower[256];
extern const uint8_t kToUpper[256];
extern const uint8_t kBase36[256];
extern const char16_t kCp437[256];

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STR_TAB_INTERNAL_H_ */
