#ifndef COSMOPOLITAN_LIBC_CALLS_TTYDEFAULTS_H_
#define COSMOPOLITAN_LIBC_CALLS_TTYDEFAULTS_H_
#include "libc/sysv/consts/baud.internal.h"
#include "libc/sysv/consts/termios.h"

#define TTYDEF_IFLAG (BRKINT | ISTRIP | ICRNL | IMAXBEL | IXON | IXANY)
#define TTYDEF_OFLAG (OPOST | ONLCR | XTABS)
#define TTYDEF_LFLAG (ECHO | ICANON | ISIG | IEXTEN | ECHOE | ECHOKE | ECHOCTL)
#define TTYDEF_CFLAG (CREAD | CS8 | HUPCL)
#define TTYDEF_SPEED (B9600)

#define CTRL(x)  ((x) ^ 0100)
#define CEOF     CTRL('D')
#define CERASE   CTRL('?')
#define CINTR    CTRL('C')
#define CKILL    CTRL('U')
#define CQUIT    CTRL('\\')
#define CSUSP    CTRL('Z')
#define CDSUSP   CTRL('Y')
#define CSTART   CTRL('Q')
#define CSTOP    CTRL('S')
#define CLNEXT   CTRL('V')
#define CDISCARD CTRL('O')
#define CWERASE  CTRL('W')
#define CREPRINT CTRL('R')
#define CEOT     CEOF
#define CBRK     CEOL
#define CRPRNT   CREPRINT
#define CFLUSH   CDISCARD
#define CEOL     255
#define CMIN     1
#define CTIME    0

#endif /* COSMOPOLITAN_LIBC_CALLS_TTYDEFAULTS_H_ */
