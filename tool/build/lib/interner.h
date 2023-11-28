#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_INTERNER_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_INTERNER_H_
COSMOPOLITAN_C_START_

struct Interner {
  size_t i; /* byte size of 𝑝 */
  size_t n; /* byte capacity of 𝑝 */
  char *p;  /* will relocate */
};

struct Interner *newinterner(void) returnsnonnull paramsnonnull();
void freeinterner(struct Interner *);
size_t interncount(const struct Interner *) paramsnonnull();
size_t internobj(struct Interner *, const void *, size_t) paramsnonnull();
size_t intern(struct Interner *, const char *) paramsnonnull();
bool isinterned(struct Interner *, const char *) paramsnonnull();

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_INTERNER_H_ */
