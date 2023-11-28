#ifndef COSMOPOLITAN_NOMULTICS_H_
#define COSMOPOLITAN_NOMULTICS_H_

#define kTtySilence 1  /* do not relay read() into write() */
#define kTtyEchoRaw 2  /* don't ^X visualize control codes */
#define kTtyUncanon 4  /* enables non-canonical (raw) mode */
#define kTtyNoCr2Nl 8  /* don't map \r → \n (a.k.a !ICRNL) */
#define kTtyNoIsigs 16 /* don't auto-raise signals on keys */
#define kTtyXtMouse 32 /* enables eXtreme Xterm mouse mode */

COSMOPOLITAN_C_START_

struct TtyConf {
  unsigned char magic;
  unsigned char mousebs;
  unsigned char replmode;
  unsigned char replstderr;
  union {
    unsigned char c_cc[20];
    struct {
      unsigned char vline;
      unsigned char vintr;  /* SIGINT keystroke (isigs) */
      unsigned char vquit;  /* SIGQUIT keystroke (isigs) */
      unsigned char verase; /* backspace keystroke (canon) */
      unsigned char vkill;  /* kill line back keystroke (canon) */
      unsigned char veof;   /* EOF keystroke (canon) */
      unsigned char vtime;  /* vtime*100ms can control read delay */
      unsigned char vmin;   /* use 0 for special non-blocking mode */
      unsigned char vswtc;
      unsigned char vstart;
      unsigned char vstop;
      unsigned char vsusp; /* keystroke for SIGTSTP (isigs) */
      unsigned char veol;
      unsigned char vreprint; /* keystroke to print unread line (canon) */
      unsigned char vdiscard;
      unsigned char vwerase; /* keystroke for kill word back (canon) */
      unsigned char vlnext;  /* print next keystroke as ascii (canon) */
      unsigned char veol2;
    };
  };
};

extern struct TtyConf __ttyconf;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_NOMULTICS_H_ */
