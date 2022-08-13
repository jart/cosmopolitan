#ifndef COSMOPOLITAN_LIBC_ALG_CRITBIT0_H_
#define COSMOPOLITAN_LIBC_ALG_CRITBIT0_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § data structures » critical bit tree (for c strings)       ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

struct critbit0 {
  void *root;
  size_t count;
};

bool critbit0_contains(struct critbit0 *, const char *) dontthrow nosideeffect
    paramsnonnull();
int critbit0_insert(struct critbit0 *, const char *) paramsnonnull();
bool critbit0_delete(struct critbit0 *, const char *) dontthrow paramsnonnull();
void critbit0_clear(struct critbit0 *) dontthrow paramsnonnull();
char *critbit0_get(struct critbit0 *, const char *);
intptr_t critbit0_allprefixed(struct critbit0 *, const char *,
                              intptr_t (*)(const char *, void *), void *)
    paramsnonnull((1, 2, 3)) dontthrow;
int critbit0_emplace(struct critbit0 *, char *, size_t) paramsnonnull();

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ALG_CRITBIT0_H_ */
