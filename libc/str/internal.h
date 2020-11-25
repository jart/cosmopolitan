#ifndef COSMOPOLITAN_LIBC_STR_INTERNAL_H_
#define COSMOPOLITAN_LIBC_STR_INTERNAL_H_
#ifndef __STRICT_ANSI__
#include "libc/str/str.h"

#if !(__ASSEMBLER__ + __LINKER__ + 0)

hidden extern const uint32_t kSha256Tab[64];

nodebuginfo forceinline bool32 ismoar(wint_t c) {
  return (c & 0300) == 0300;
}

nodebuginfo forceinline bool32 iscont(wint_t c) {
  return (c & 0300) == 0200;
}

char *strstr$sse42(const char *, const char *) strlenesque hidden;
char16_t *strstr16$sse42(const char16_t *, const char16_t *) strlenesque hidden;
void *memmem$sse42(const void *, size_t, const void *,
                   size_t) strlenesque hidden;
void sha256$x86(uint32_t[hasatleast 8], const uint8_t[hasatleast 64],
                uint32_t) hidden;

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* !ANSI */
#endif /* COSMOPOLITAN_LIBC_STR_INTERNAL_H_ */
