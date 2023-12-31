#ifndef COSMOPOLITAN_DSP_TTY_TTY_H_
#define COSMOPOLITAN_DSP_TTY_TTY_H_

#define kTtyIdScreen 83

COSMOPOLITAN_C_START_

struct termios;

struct TtyIdent {
  int id;                /* first number sent back by \e[>c */
  int version;           /* second number sent back by \e[>c */
  struct TtyIdent *next; /* yo dawg */
};

struct TtyCursor {
  int y;
  int x;
  int bg;
  int fg;
};

enum TtyRawFlags {
  kTtyEcho = 1 << 0,     /* echo input */
  kTtyCursor = 1 << 1,   /* show cursor */
  kTtySigs = 1 << 2,     /* auto raise() on CTRL+C, CTRL+Z, and CTRL+\ */
  kTtyLfToCrLf = 1 << 3, /* enables unix newline magic */
};

typedef int (*ttyconf_f)(struct termios *, int64_t);

int ttyraw(enum TtyRawFlags);
int ttyhidecursor(int);
int ttyshowcursor(int);
int ttysavecursor(int);
int ttyrestorecursor(int);
int ttyenablealtbuf(int);
int ttydisablealtbuf(int);
int ttysend(int, const char *);
ssize_t ttywrite(int, const void *, size_t);
int ttysendtitle(int, const char *, const struct TtyIdent *);
int ttyident(struct TtyIdent *, int, int);
void ttyidentclear(struct TtyIdent *);
char *ttydescribe(char *, size_t, const struct TtyIdent *);
int ttyconfig(int, ttyconf_f, int64_t, struct termios *);
int ttyrestore(int, const struct termios *);
int ttysetrawdeadline(struct termios *, int64_t);
int ttysetrawmode(struct termios *, int64_t);
int ttysetraw(struct termios *, int64_t);
char *ttymove(struct TtyCursor *, char *, int, int)
    paramsnonnull() returnsnonnull;
void ttyhisto(uint32_t[hasatleast 256], uint8_t[hasatleast 256],
              const uint8_t *, const uint8_t *, size_t);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_DSP_TTY_TTY_H_ */
