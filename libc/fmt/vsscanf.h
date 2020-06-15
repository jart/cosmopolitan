#ifndef COSMOPOLITAN_LIBC_INTERNAL_VSSCANF_H_
#define COSMOPOLITAN_LIBC_INTERNAL_VSSCANF_H_
#include "libc/fmt/fmt.h"
#include "libc/fmt/vcscanf.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct StringScannerState {
  const unsigned char *s;
  size_t i;
};

static inline int vsscanfcb(void *arg) {
  struct StringScannerState *state = arg;
  int res;
  if ((res = state->s[state->i])) {
    state->i++;
  } else {
    res = -1;
  }
  return res;
}

/**
 * String decoder builder.
 *
 * This macro grants sscanf() and vsscanf() the choice to either link or
 * inline the full vcscanf() implementation.
 *
 * @see libc/fmt/vcscanf.h (for docs and implementation)
 */
static inline int __vsscanf(const char *str, const char *fmt, va_list ap,
                            int impl(int callback(void *), void *arg,
                                     const char *fmt, va_list ap)) {
  struct StringScannerState state = {(const unsigned char *)str, 0};
  return impl(vsscanfcb, &state, fmt, ap);
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTERNAL_VSSCANF_H_ */
