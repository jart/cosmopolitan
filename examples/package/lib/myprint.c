#include "examples/package/lib/myprint.h"
#include "libc/stdio/stdio.h"

/**
 * Calls MyPrint2() indirected via assembly function.
 */
void MyPrint(const char *s) {
  MyAsm(s);
}

/**
 * Prints string to output.
 *
 * @param s is null-terminated c string usually having \n
 * @see printf() which has domain-specific language
 */
void MyPrint2(const char *s) {
  fputs(s, stdout);
}
