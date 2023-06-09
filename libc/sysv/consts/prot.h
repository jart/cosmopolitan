#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_PROT_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_PROT_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const int PROT_NONE;
extern const int PROT_READ;
extern const int PROT_WRITE;
extern const int PROT_EXEC;
extern const int PROT_GROWSDOWN;
extern const int PROT_GROWSUP;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define PROT_NONE  0
#define PROT_READ  1
#define PROT_WRITE 2
#define PROT_EXEC  4

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_PROT_H_ */
