#ifndef COSMOPOLITAN_LIBC_BITS_ASMFLAG_H_
#define COSMOPOLITAN_LIBC_BITS_ASMFLAG_H_

/*
 * Constraints for virtual machine flags.
 * @note we beseech clang devs for flag constraints
 */
#ifdef __GCC_ASM_FLAG_OUTPUTS__ /* GCC6+ CLANG10+ */
#define CFLAG_CONSTRAINT  "=@ccc"
#define CFLAG_ASM(OP)     OP
#define ZFLAG_CONSTRAINT  "=@ccz"
#define ZFLAG_ASM(OP)     OP
#define OFLAG_CONSTRAINT  "=@cco"
#define OFLAG_ASM(OP)     OP
#define SFLAG_CONSTRAINT  "=@ccs"
#define SFLAG_ASM(SP)     SP
#define ABOVE_CONSTRAINT  "=@cca" /* i.e. !ZF && !CF */
#define ABOVEFLAG_ASM(OP) OP
#else
#define CFLAG_CONSTRAINT  "=q"
#define CFLAG_ASM(OP)     OP "\n\tsetc\t%b0"
#define ZFLAG_CONSTRAINT  "=q"
#define ZFLAG_ASM(OP)     OP "\n\tsetz\t%b0"
#define OFLAG_CONSTRAINT  "=q"
#define OFLAG_ASM(OP)     OP "\n\tseto\t%b0"
#define SFLAG_CONSTRAINT  "=q"
#define SFLAG_ASM(SP)     OP "\n\tsets\t%b0"
#define ABOVE_CONSTRAINT  "=@cca"
#define ABOVEFLAG_ASM(OP) OP "\n\tseta\t%b0"
#endif

#endif /* COSMOPOLITAN_LIBC_BITS_ASMFLAG_H_ */
