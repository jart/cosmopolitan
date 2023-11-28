#ifndef COSMOPOLITAN_TOOL_VIZ_LIB_STRINGBUILDER_H_
#define COSMOPOLITAN_TOOL_VIZ_LIB_STRINGBUILDER_H_
COSMOPOLITAN_C_START_

struct StringBuilder {
  size_t i, n;
  char *p;
};

struct StringBuilder *NewStringBuilder(void) mallocesque returnsnonnull;

int StringBuilderAppend(const char *, struct StringBuilder *)
    paramsnonnull((2));

char *FreeStringBuilder(struct StringBuilder *) mallocesque returnsnonnull
    paramsnonnull();

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TOOL_VIZ_LIB_STRINGBUILDER_H_ */
