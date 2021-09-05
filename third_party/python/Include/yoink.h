#ifndef COSMOPOLITAN_THIRD_PARTY_PYTHON_INCLUDE_YOINK_H_
#define COSMOPOLITAN_THIRD_PARTY_PYTHON_INCLUDE_YOINK_H_

#define PYTHON_YOINK(s)             \
  __asm__(".section .yoink\n\t"     \
          "nopl\t\"pyc:" s "\"\n\t" \
          ".previous")

#define PYTHON_PROVIDE(s)             \
  __asm__(".section .yoink\n"         \
          "\"pyc:" s "\":\n\t"        \
          ".globl\t\"pyc:" s "\"\n\t" \
          ".previous")

#endif /* COSMOPOLITAN_THIRD_PARTY_PYTHON_INCLUDE_YOINK_H_ */
