#ifndef COSMOPOLITAN_THIRD_PARTY_F2C_LIB_FIO_H_
#define COSMOPOLITAN_THIRD_PARTY_F2C_LIB_FIO_H_
#include "libc/errno.h"
#include "libc/stdio/stdio.h"
#include "third_party/f2c/f2c.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#ifndef OFF_T
#define OFF_T long
#endif

#ifdef UIOLEN_int
typedef int uiolen;
#else
typedef long uiolen;
#endif

/*units*/
typedef struct {
  FILE *ufd; /*0=unconnected*/
  char *ufnm;
  long uinode;
  int udev;
  int url;    /*0=sequential*/
  flag useek; /*true=can backspace, use dir, ...*/
  flag ufmt;
  flag urw; /* (1 for can read) | (2 for can write) */
  flag ublnk;
  flag uend;
  flag uwrt; /*last io was write*/
  flag uscrtch;
} unit;

void x_putc(int);
long f__inode(char *, int *);
void sig_die(const char *, int);
void f__fatal(int, const char *);
int t_runc(alist *);
int f__nowreading(unit *);
int f__nowwriting(unit *);
int fk_open(int, int, ftnint);
int en_fio(void);
void f_init(void);
int t_putc(int);
int x_wSL(void);
void b_char(const char *, char *, ftnlen);
void g_char(const char *, ftnlen, char *);
int c_sfe(cilist *);
int z_rnew(void);
int err__fl(int, int, const char *);
int xrd_SL(void);
int f__putbuf(int);

extern cilist *f__elist; /*active external io list*/
extern flag f__reading, f__external, f__sequential, f__formatted;
extern flag f__init;
extern FILE *f__cf;      /*current file*/
extern unit *f__curunit; /*current unit*/
extern unit f__units[];
extern int (*f__doend)(void);
extern int (*f__getn)(void); /* for formatted input */
extern void (*f__putn)(int); /* for formatted output */
extern int (*f__donewrec)(void);

#define err(f, m, s)  \
  {                   \
    if (f)            \
      errno = m;      \
    else              \
      f__fatal(m, s); \
    return (m);       \
  }

#define errfl(f, m, s) return err__fl((int)f, m, s)

/*Table sizes*/
#define MXUNIT 100

extern int f__recpos;    /*position in current record*/
extern OFF_T f__cursor;  /* offset to move to */
extern OFF_T f__hiwater; /* so TL doesn't confuse us */

#define WRITE 1
#define READ  2
#define SEQ   3
#define DIR   4
#define FMT   5
#define UNF   6
#define EXT   7
#define INT   8

#define buf_end(x) (x->_flag & _IONBF ? x->_ptr : x->_base + BUFSIZ)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_F2C_LIB_FIO_H_ */
