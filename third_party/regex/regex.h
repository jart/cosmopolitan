#ifndef COSMOPOLITAN_LIBC_REGEX_REGEX_H_
#define COSMOPOLITAN_LIBC_REGEX_REGEX_H_
COSMOPOLITAN_C_START_

#if 0
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § regular expressions                                       ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/
#endif

#define REG_EXTENDED 1
#define REG_ICASE    2
#define REG_NEWLINE  4
#define REG_NOSUB    8

#define REG_NOTBOL 1 /* ^ should not match beginning of string */
#define REG_NOTEOL 2 /* $ should not match end of string */

#define REG_OK       0
#define REG_NOMATCH  1
#define REG_BADPAT   2
#define REG_ECOLLATE 3
#define REG_ECTYPE   4
#define REG_EESCAPE  5
#define REG_ESUBREG  6
#define REG_EBRACK   7
#define REG_EPAREN   8
#define REG_EBRACE   9
#define REG_BADBR    10
#define REG_ERANGE   11
#define REG_ESPACE   12
#define REG_BADRPT   13

#define REG_ENOSYS -1

typedef long regoff_t;

struct PosixRegex {
  size_t re_nsub;
  void *__opaque, *__padding[4];
  size_t __nsub2;
  char __padding2;
};

struct PosixRegexMatch {
  regoff_t rm_so;
  regoff_t rm_eo;
};

typedef struct PosixRegex regex_t;
typedef struct PosixRegexMatch regmatch_t;

int regcomp(regex_t *, const char *, int);
int regexec(const regex_t *, const char *, size_t, regmatch_t *, int);
size_t regerror(int, const regex_t *, char *, size_t);
void regfree(regex_t *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_REGEX_REGEX_H_ */
