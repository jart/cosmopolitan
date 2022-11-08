#ifndef COSMOPOLITAN_LIBC_KOMPRESSOR_KOMPRESSOR_H_
#define COSMOPOLITAN_LIBC_KOMPRESSOR_KOMPRESSOR_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
#if 0
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § standard library » compression                            ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/
#endif

struct RlDecode {
  uint8_t repititions;
  uint8_t byte;
};

void rldecode(void *dest, const struct RlDecode *) _Hide;
void rldecode2(void *dest, const struct RlDecode *) _Hide;
const uint8_t *lz4check(const void *data) _Hide;
void *lz4cpy(void *dest, const void *blockdata, size_t blocksize) _Hide;
void *lz4decode(void *dest, const void *src) _Hide;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_KOMPRESSOR_KOMPRESSOR_H_ */
