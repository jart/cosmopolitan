#ifndef COSMOPOLITAN_TOOL_DECODE_LIB_ASMCODEGEN_H_
#define COSMOPOLITAN_TOOL_DECODE_LIB_ASMCODEGEN_H_
#include "libc/stdio/stdio.h"
#include "tool/decode/lib/idname.h"
COSMOPOLITAN_C_START_

#define COLUMN_WIDTH 24

#define showint(x)      show(".long", format(b1, "%d", x), #x)
#define showint64(x)    show(".quad", format(b1, "%ld", x), #x)
#define showbyte(x)     show(".byte", format(b1, "%hhu", x), #x)
#define showshort(x)    show(".short", format(b1, "%hu", x), #x)
#define showshorthex(x) show(".short", format(b1, "%#-6hx", x), #x)
#define showinthex(x)   show(".long", format(b1, "%#x", x), #x)
#define showint64hex(x) show(".quad", format(b1, "%#lx", x), #x)
#define showorg(x)      show(".org", format(b1, "%#lx", x), #x)

extern char b1[BUFSIZ];
extern char b2[BUFSIZ];

char *format(char *buf, const char *fmt, ...);
char *tabpad(const char *s, unsigned width) __wur;
void show(const char *directive, const char *value, const char *comment);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TOOL_DECODE_LIB_ASMCODEGEN_H_ */
