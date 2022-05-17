#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_THREADED_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_THREADED_H_
#include "libc/dce.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern bool __threaded;
extern bool __tls_enabled;
extern unsigned __tls_index;

void *__initialize_tls(char[hasatleast 64]);
void __install_tls(char[hasatleast 64]);

#if defined(__GNUC__) && defined(__x86_64__) && !defined(__STRICT_ANSI__)
/**
 * Returns address of thread information block.
 *
 * This function must not be called until TLS is initialized.
 *
 * @see __install_tls()
 * @see clone()
 */
static noasan inline char *__get_tls(void) {
  char *tib, *lin = (char *)0x30;
  if (IsLinux() || IsFreebsd() || IsNetbsd() || IsOpenbsd()) {
    asm("mov\t%%fs:(%1),%0" : "=a"(tib) : "r"(lin));
  } else {
    asm("mov\t%%gs:(%1),%0" : "=a"(tib) : "r"(lin));
    if (IsWindows()) {
      tib = *(char **)(tib + 0x1480 + __tls_index * 8);
    }
  }
  return tib;
}
#endif /* GNU x86-64 */

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_THREADED_H_ */
