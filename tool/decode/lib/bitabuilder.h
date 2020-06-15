#ifndef COSMOPOLITAN_TOOL_DECODE_LIB_BITABUILDER_H_
#define COSMOPOLITAN_TOOL_DECODE_LIB_BITABUILDER_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct FILE;
struct BitaBuilder;
struct BitaBuilder *bitabuilder_new(void);
bool bitabuilder_setbit(struct BitaBuilder *, size_t);
bool bitabuilder_fwrite(const struct BitaBuilder *, struct FILE *);
void bitabuilder_free(struct BitaBuilder **);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_DECODE_LIB_BITABUILDER_H_ */
