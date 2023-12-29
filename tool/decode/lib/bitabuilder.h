#ifndef COSMOPOLITAN_TOOL_DECODE_LIB_BITABUILDER_H_
#define COSMOPOLITAN_TOOL_DECODE_LIB_BITABUILDER_H_
#include "libc/stdio/stdio.h"
COSMOPOLITAN_C_START_

struct BitaBuilder;
struct BitaBuilder *bitabuilder_new(void);
bool bitabuilder_setbit(struct BitaBuilder *, size_t);
bool bitabuilder_fwrite(const struct BitaBuilder *, FILE *);
void bitabuilder_free(struct BitaBuilder **);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TOOL_DECODE_LIB_BITABUILDER_H_ */
