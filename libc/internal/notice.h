#ifndef COSMOPOLITAN_LIBC_INTERNAL_NOTICE_H_
#define COSMOPOLITAN_LIBC_INTERNAL_NOTICE_H_
#ifndef __STRICT_ANSI__

#ifdef __ASSEMBLER__
.include "libc/notice.inc"
#else
asm(".include \"libc/notice.inc\"");
#endif

#endif /* !ANSI */
#endif /* COSMOPOLITAN_LIBC_INTERNAL_NOTICE_H_ */
