#ifndef COSMOPOLITAN_LIBC_CALLS_HEFTY_SPAWN_H_
#define COSMOPOLITAN_LIBC_CALLS_HEFTY_SPAWN_H_

#define SPAWN_DETACH 1
#define SPAWN_TABULARASA 2

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int spawnve(unsigned, int[3], const char *, char *const[], char *const[])
    paramsnonnull((3, 4, 5));
int spawnl(unsigned, int[3], const char *, const char *, ...) nullterminated()
    paramsnonnull((3, 4));
int spawnlp(unsigned, int[3], const char *, const char *, ...) nullterminated()
    paramsnonnull((3, 4));
int spawnle(unsigned, int[3], const char *, const char *, ...)
    nullterminated((1)) paramsnonnull((3, 4));
int spawnv(unsigned, int[3], const char *, char *const[]) paramsnonnull((3, 4));
int spawnvp(unsigned, int[3], const char *, char *const[])
    paramsnonnull((3, 4));
int spawnvpe(unsigned, int[3], const char *, char *const[], char *const[])
    paramsnonnull((3, 4, 5));

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_HEFTY_SPAWN_H_ */
