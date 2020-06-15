#ifndef COSMOPOLITAN_LIBC_REGEX_REGEX_H_
#define COSMOPOLITAN_LIBC_REGEX_REGEX_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#if 0
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § regular expressions                                       ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/
#endif

#define REG_EXTENDED 1
#define REG_ICASE 2
#define REG_NEWLINE 4
#define REG_NOSUB 8

#define REG_NOTBOL 1
#define REG_NOTEOL 2

#define REG_OK 0
#define REG_NOMATCH 1
#define REG_BADPAT 2
#define REG_ECOLLATE 3
#define REG_ECTYPE 4
#define REG_EESCAPE 5
#define REG_ESUBREG 6
#define REG_EBRACK 7
#define REG_EPAREN 8
#define REG_EBRACE 9
#define REG_BADBR 10
#define REG_ERANGE 11
#define REG_ESPACE 12
#define REG_BADRPT 13

#define REG_ENOSYS -1

typedef long regoff_t;

typedef struct re_pattern_buffer {
  size_t re_nsub;
  void *__opaque, *__padding[4];
  size_t __nsub2;
  char __padding2;
} regex_t;

typedef struct {
  regoff_t rm_so;
  regoff_t rm_eo;
} regmatch_t;

int regcomp(regex_t *preg, const char *regex, int flags);
int regexec(const regex_t *preg, const char *string, size_t nmatch,
            regmatch_t pmatch[], int eflags);
void regfree(regex_t *preg);
size_t regerror(int errcode, const regex_t *preg, char *errbuf,
                size_t errbufsize);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_REGEX_REGEX_H_ */
