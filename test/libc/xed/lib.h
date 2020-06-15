#ifndef COSMOPOLITAN_TEST_LIBC_XED_LIB_H_
#define COSMOPOLITAN_TEST_LIBC_XED_LIB_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int ild(const char16_t *codez);
int ildreal(const char16_t *codez);
int ildlegacy(const char16_t *codez);
uint8_t *unbingx86op(const char16_t *codez) nodiscard;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TEST_LIBC_XED_LIB_H_ */
