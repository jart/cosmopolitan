#ifndef COSMOPOLITAN_THIRD_PARTY_PYTHON_INCLUDE_YOINK_H_
#define COSMOPOLITAN_THIRD_PARTY_PYTHON_INCLUDE_YOINK_H_

#ifdef __x86_64__
#define PYTHON_YOINK(s)             \
  __asm__(".section .yoink\n\t"     \
          "nopl\t\"pyc:" s "\"\n\t" \
          ".previous")
#elif defined(__aarch64__)
#define PYTHON_YOINK(s)           \
  __asm__(".section .yoink\n\t"   \
          "bl\t\"pyc:" s "\"\n\t" \
          ".previous")
#else
#error "architecture unsupported"
#endif /* __x86_64__ */

#define PYTHON_PROVIDE(s)             \
  __asm__(".section .yoink\n"         \
          "\"pyc:" s "\":\n\t"        \
          ".globl\t\"pyc:" s "\"\n\t" \
          ".previous")

#endif /* COSMOPOLITAN_THIRD_PARTY_PYTHON_INCLUDE_YOINK_H_ */
