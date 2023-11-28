#ifndef COSMOPOLITAN_THIRD_PARTY_MUSL_GLOB_H_
#define COSMOPOLITAN_THIRD_PARTY_MUSL_GLOB_H_
COSMOPOLITAN_C_START_

#define GLOB_ERR      0x01
#define GLOB_MARK     0x02
#define GLOB_NOSORT   0x04
#define GLOB_DOOFFS   0x08 /* reserves null slots at start of gl_pathv */
#define GLOB_NOCHECK  0x10 /* just yield pattern if GLOB_NOMATCH happens */
#define GLOB_APPEND   0x20 /* enables us to call glob() multiple times */
#define GLOB_NOESCAPE 0x40 /* don't allow things like \*\?\[\] escaping */
#define GLOB_PERIOD   0x80

#define GLOB_TILDE       0x1000
#define GLOB_TILDE_CHECK 0x4000

#define GLOB_NOSPACE 1
#define GLOB_ABORTED 2
#define GLOB_NOMATCH 3
#define GLOB_NOSYS   4

typedef struct {
  size_t gl_pathc;
  char **gl_pathv;
  size_t gl_offs;
  int __dummy1;
  void *__dummy2[5];
} glob_t;

int glob(const char *, int, int (*)(const char *, int), glob_t *);
void globfree(glob_t *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_MUSL_GLOB_H_ */
