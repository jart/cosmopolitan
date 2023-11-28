#ifndef COSMOPOLITAN_TEST_LIBC_XED_LIB_H_
#define COSMOPOLITAN_TEST_LIBC_XED_LIB_H_
COSMOPOLITAN_C_START_

int ild(const char16_t *codez);
int ildreal(const char16_t *codez);
int ildlegacy(const char16_t *codez);
uint8_t *unbingx86op(const char16_t *codez) __wur;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TEST_LIBC_XED_LIB_H_ */
