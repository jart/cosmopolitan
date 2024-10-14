#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <iconv.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

/**
 * @fileoverview program for viewing bbs art files
 * @see http://www.textfiles.com/art/
 */

#define INBUFSZ 256
#define OUBUFSZ (INBUFSZ * 6)
#define SLIT(s) ((unsigned)s[3] << 24 | s[2] << 16 | s[1] << 8 | s[0])

volatile sig_atomic_t got_signal;

void on_signal(int sig) {
  got_signal = 1;
}

void process_file(const char *path, int fd, iconv_t cd, int baud_rate) {
  char input_buffer[INBUFSZ];
  char output_buffer[OUBUFSZ];
  size_t input_left, output_left;
  char *input_ptr, *output_ptr;
  struct timespec next = timespec_mono();

  for (;;) {

    // read from file
    ssize_t bytes_read = read(fd, input_buffer, INBUFSZ);
    if (bytes_read == -1) {
      perror(path);
      exit(1);
    }
    if (!bytes_read)
      break;

    // modernize character set
    input_ptr = input_buffer;
    input_left = bytes_read;
    output_ptr = output_buffer;
    output_left = OUBUFSZ;
    if (iconv(cd, &input_ptr, &input_left, &output_ptr, &output_left) ==
        (size_t)-1) {
      perror(path);
      exit(1);
    }

    // write to terminal
    for (char *p = output_buffer; p < output_ptr; p++) {
      if (got_signal)
        return;

      write(STDOUT_FILENO, p, 1);

      // allow arrow keys to change baud rate
      char key[4] = {0};
      if (read(STDIN_FILENO, key, sizeof(key)) > 0) {
        if (SLIT(key) == SLIT("\e[A") ||  // up
            SLIT(key) == SLIT("\e[C")) {  // right
          baud_rate *= 1.4;
        } else if (SLIT(key) == SLIT("\e[B") ||  // down
                   SLIT(key) == SLIT("\e[D")) {  // left
          baud_rate *= 0.6;
        }
      }

      // insert artificial delay for one byte. we divide by 10 to convert
      // bits to bytes, because that is how many bits 8-N-1 encoding used
      next = timespec_add(next, timespec_fromnanos(1e9 / (baud_rate / 10.)));
      usleep(timespec_tomicros(timespec_subz(next, timespec_mono())));
    }
  }
}

int main(int argc, char *argv[]) {

  // "When new technology comes out, people don't all buy it right away.
  //  If what they have works, some will wait until it doesn't. A few
  //  people do get the latest though. In 1984 2400 baud modems became
  //  available, so some people had them, but many didn't. A BBS list
  //  from 1986 shows operators were mostly 300 and 1200, but some were
  //  using 2400. The next 5 years were the hayday of the 2400."
  //
  // https://forum.vcfed.org/index.php?threads/the-2400-baud-modem.44241/

  int baud_rate = 2400;                // -b 2400
  const char *from_charset = "CP437";  // -f CP437
  const char *to_charset = "UTF-8";    // -t UTF-8

  int opt;
  while ((opt = getopt(argc, argv, "hb:f:t:")) != -1) {
    switch (opt) {
      case 'b': {
        char *endptr;
        double rate = strtod(optarg, &endptr);
        if (*endptr == 'k') {
          rate *= 1e3;
          ++endptr;
        } else if (*endptr == 'm') {
          rate *= 1e6;
          ++endptr;
        }
        if (*endptr || baud_rate <= 0) {
          fprintf(stderr, "%s: invalid baud rate: %s\n", argv[0], optarg);
          exit(1);
        }
        baud_rate = rate;
        break;
      }
      case 'f':
        from_charset = optarg;
        break;
      case 't':
        to_charset = optarg;
        break;
      case 'h':
        fprintf(stderr, "\
Usage:\n\
    %s [-b BAUD] [-f CP437] [-t UTF-8] FILE...\n\
\n\
Supported charsets:\n\
    utf8, wchart, ucs2be, ucs2le, utf16be, utf16le, ucs4be, ucs4le,\n\
    ascii, utf16, ucs4, ucs2, eucjp, shiftjis, iso2022jp, gb18030, gbk,\n\
    gb2312, big5, euckr, iso88591, latin1, iso88592, iso88593, iso88594,\n\
    iso88595, iso88596, iso88597, iso88598, iso88599, iso885910,\n\
    iso885911, iso885913, iso885914, iso885915, iso885916, cp1250,\n\
    windows1250, cp1251, windows1251, cp1252, windows1252, cp1253,\n\
    windows1253, cp1254, windows1254, cp1255, windows1255, cp1256,\n\
    windows1256, cp1257, windows1257, cp1258, windows1258, koi8r, koi8u,\n\
    cp437, cp850, cp866, ibm1047, cp1047.\n\
\n\
See also:\n\
    http://www.textfiles.com/art/\n\
\n",
                argv[0]);
        exit(0);
      default:
        fprintf(stderr, "protip: pass the -h flag for help\n");
        exit(1);
    }
  }
  if (optind == argc) {
    fprintf(stderr, "%s: missing operand\n", argv[0]);
    exit(1);
  }

  iconv_t cd = iconv_open(to_charset, from_charset);
  if (cd == (iconv_t)-1) {
    fprintf(stderr, "error: conversion from %s to %s not supported\n",
            from_charset, to_charset);
    exit(1);
  }

  // catch ctrl-c
  signal(SIGINT, on_signal);

  // don't wait until newline to read() keystrokes
  struct termios t;
  if (!tcgetattr(STDIN_FILENO, &t)) {
    struct termios t2 = t;
    t2.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t2);
  }

  // make stdin nonblocking
  fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);

  // hide cursor
  write(STDOUT_FILENO, "\e[?25l", 6);

  // Process each file specified on the command line
  for (int i = optind; i < argc && !got_signal; i++) {
    int fd = open(argv[i], O_RDONLY);
    if (fd == -1) {
      perror(argv[i]);
      continue;
    }
    process_file(argv[i], fd, cd, baud_rate);
    close(fd);
  }

  // cleanup
  iconv_close(cd);

  // show cursor
  write(STDOUT_FILENO, "\e[?25h", 6);

  // restore terminal
  tcsetattr(STDIN_FILENO, TCSANOW, &t);
}
