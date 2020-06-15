#ifndef COSMOPOLITAN_LIBC_CALLS_HEFTY_NTSPAWN_H_
#define COSMOPOLITAN_LIBC_CALLS_HEFTY_NTSPAWN_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct NtProcessInformation;
struct NtSecurityAttributes;
struct NtStartupInfo;

char **sortenvp(char *const[]) hidden nodiscard paramsnonnull();
char16_t *mkntcmdline(char *const[]) hidden nodiscard paramsnonnull();
char16_t *mkntenvblock(char *const[]) hidden nodiscard paramsnonnull();
int ntspawn(const char *, char *const[], char *const[],
            struct NtSecurityAttributes *, struct NtSecurityAttributes *,
            bool32, uint32_t, const char16_t *, const struct NtStartupInfo *,
            struct NtProcessInformation *) paramsnonnull((1, 2, 3, 9)) hidden;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_HEFTY_NTSPAWN_H_ */
