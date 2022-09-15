#ifndef COSMOPOLITAN_LIBC_STR_TAB_INTERNAL_H_
#define COSMOPOLITAN_LIBC_STR_TAB_INTERNAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const int8_t kHexToInt[256];
extern const uint8_t gperf_downcase[256];
extern const uint8_t kToLower[256];
extern const uint8_t kToUpper[256];
extern const uint8_t kBase36[256];
extern const char16_t kCp437[256];

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STR_TAB_INTERNAL_H_ */
