#ifndef COSMOPOLITAN_LIBC_UNICODE_LOCALE_H_
#define COSMOPOLITAN_LIBC_UNICODE_LOCALE_H_

#define LC_CTYPE         0
#define LC_NUMERIC       1
#define LC_CTYPE_MASK    1
#define LC_TIME          2
#define LC_NUMERIC_MASK  2
#define LC_COLLATE       3
#define LC_MONETARY      4
#define LC_TIME_MASK     4
#define LC_MESSAGES      5
#define LC_ALL           6
#define LC_COLLATE_MASK  8
#define LC_MONETARY_MASK 16
#define LC_MESSAGES_MASK 32
#define LC_ALL_MASK      0x1fbf

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

char *setlocale(int, const char *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_UNICODE_LOCALE_H_ */
