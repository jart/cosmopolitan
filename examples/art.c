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
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

/**
 * @fileoverview program for viewing bbs art files
 * @see https://github.com/blocktronics/artpacks
 * @see http://www.textfiles.com/art/
 */

#define HELP \
  "Usage:\n\
    art [-b %d] [-f %s] [-t %s] FILE...\n\
\n\
Flags:\n\
    -b NUMBER   specifies simulated modem baud rate, which defaults to\n\
                2400 since that was the most common modem speed in the\n\
                later half of the 1980s during the BBS golden age; you\n\
                could also say 300 for the slowest experience possible\n\
                or you could say 14.4k to get more of a 90's feel, and\n\
                there's also the infamous 56k to bring you back to y2k\n\
    -f CHARSET  specifies charset of input bytes, where the default is\n\
                cp347 which means IBM Code Page 347 a.k.a. DOS\n\
    -t CHARSET  specifies output charset used by your terminal, and it\n\
                defaults to utf8 a.k.a. thompson-pike encoding\n\
\n\
Supported charsets:\n\
    utf8, ascii, wchar_t, ucs2be, ucs2le, utf16be, utf16le, ucs4be,\n\
    ucs4le, utf16, ucs4, ucs2, eucjp, shiftjis, iso2022jp, gb18030, gbk,\n\
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
    https://github.com/blocktronics/artpacks\n\
\n"

#define INBUFSZ 256
#define OUBUFSZ (INBUFSZ * 6)
#define SLIT(s) ((unsigned)s[3] << 24 | s[2] << 16 | s[1] << 8 | s[0])

// "When new technology comes out, people don't all buy it right away.
//  If what they have works, some will wait until it doesn't. A few
//  people do get the latest though. In 1984 2400 baud modems became
//  available, so some people had them, but many didn't. A BBS list
//  from 1986 shows operators were mostly 300 and 1200, but some were
//  using 2400. The next 5 years were the hayday of the 2400."
//
// https://forum.vcfed.org/index.php?threads/the-2400-baud-modem.44241/

int baud_rate = 2400;                // -b 2400
const char* from_charset = "CP437";  // -f CP437
const char* to_charset = "UTF-8";    // -t UTF-8

volatile sig_atomic_t done;

void on_signal(int sig) {
  done = 1;
  (void)sig;
}

void print(const char* s) {
  (void)!write(STDOUT_FILENO, s, strlen(s));
}

int encode_character(char output[8], const char* codec, wchar_t character) {
  size_t inbytesleft = sizeof(wchar_t);
  size_t outbytesleft = 7;
  char* inbuf = (char*)&character;
  char* outbuf = output;
  iconv_t cd = iconv_open(codec, "wchar_t");
  if (cd == (iconv_t)-1)
    return -1;
  size_t result = iconv(cd, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
  iconv_close(cd);
  if (result == (size_t)-1)
    return -1;
  *outbuf = '\0';
  return 7 - outbytesleft;
}

void append_replacement_character(char** b) {
  int n = encode_character(*b, to_charset, 0xFFFD);
  if (n == -1)
    n = encode_character(*b, to_charset, '?');
  if (n != -1)
    *b += n;
}

int compare_time(struct timespec a, struct timespec b) {
  int cmp;
  if (!(cmp = (a.tv_sec > b.tv_sec) - (a.tv_sec < b.tv_sec)))
    cmp = (a.tv_nsec > b.tv_nsec) - (a.tv_nsec < b.tv_nsec);
  return cmp;
}

struct timespec add_time(struct timespec x, struct timespec y) {
  x.tv_sec += y.tv_sec;
  x.tv_nsec += y.tv_nsec;
  if (x.tv_nsec >= 1000000000) {
    x.tv_nsec -= 1000000000;
    x.tv_sec += 1;
  }
  return x;
}

struct timespec subtract_time(struct timespec a, struct timespec b) {
  a.tv_sec -= b.tv_sec;
  if (a.tv_nsec < b.tv_nsec) {
    a.tv_nsec += 1000000000;
    a.tv_sec--;
  }
  a.tv_nsec -= b.tv_nsec;
  return a;
}

struct timespec fromnanos(long long x) {
  struct timespec ts;
  ts.tv_sec = x / 1000000000;
  ts.tv_nsec = x % 1000000000;
  return ts;
}

void process_file(const char* path, int fd, iconv_t cd) {
  size_t carry = 0;
  struct timespec next;
  char input_buffer[INBUFSZ];

  clock_gettime(CLOCK_MONOTONIC, &next);

  for (;;) {

    // read from file
    ssize_t bytes_read = read(fd, input_buffer + carry, INBUFSZ - carry);
    if (!bytes_read)
      return;
    if (bytes_read == -1) {
      perror(path);
      done = 1;
      return;
    }

    // modernize character set
    char* input_ptr = input_buffer;
    size_t input_left = carry + bytes_read;
    char output_buffer[OUBUFSZ];
    char* output_ptr = output_buffer;
    size_t output_left = OUBUFSZ;
    size_t ir = iconv(cd, &input_ptr, &input_left, &output_ptr, &output_left);
    carry = 0;
    if (ir == (size_t)-1) {
      if (errno == EINVAL) {
        // incomplete multibyte sequence encountered
        memmove(input_buffer, input_ptr, input_left);
        carry = input_left;
      } else if (errno == EILSEQ && input_left) {
        // EILSEQ means either
        // 1. illegal input sequence encountered
        // 2. code not encodable in output codec
        //
        // so we skip one byte of input, and insert � or ? in the output
        // this isn't the most desirable behavior, but it is the best we
        // can do, since we don't know specifics about the codecs in use
        //
        // unlike glibc cosmo's iconv implementation may handle case (2)
        // automatically by inserting an asterisk in place of a sequence
        ++input_ptr;
        --input_left;
        memmove(input_buffer, input_ptr, input_left);
        carry = input_left;
        if (output_left >= 8)
          append_replacement_character(&output_ptr);
      } else {
        perror(path);
        done = 1;
        return;
      }
    }

    // write to terminal
    for (char* p = output_buffer; p < output_ptr; p++) {
      if (done)
        return;

      (void)!write(STDOUT_FILENO, p, 1);

      // allow arrow keys to change baud rate
      int have;
      if (ioctl(STDIN_FILENO, FIONREAD, &have)) {
        perror("ioctl");
        done = 1;
        return;
      }
      if (have > 0) {
        char key[4] = {0};
        if (read(STDIN_FILENO, key, sizeof(key)) > 0) {
          if (SLIT(key) == SLIT("\33[A") ||  // up
              SLIT(key) == SLIT("\33[C")) {  // right
            baud_rate *= 1.4;
          } else if (SLIT(key) == SLIT("\33[B") ||  // down
                     SLIT(key) == SLIT("\33[D")) {  // left
            baud_rate *= 0.6;
          }
          if (baud_rate < 3)
            baud_rate = 3;
          if (baud_rate > 1000000000)
            baud_rate = 1000000000;
        }
      }

      // insert artificial delay for one byte. we divide by 10 to convert
      // bits to bytes, because that is how many bits 8-N-1 encoding used
      struct timespec now;
      clock_gettime(CLOCK_MONOTONIC, &now);
      next = add_time(next, fromnanos(1e9 / (baud_rate / 10.)));
      if (compare_time(next, now) > 0) {
        struct timespec sleep = subtract_time(next, now);
        nanosleep(&sleep, 0);
      }
    }
  }
}

int main(int argc, char* argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "hb:f:t:")) != -1) {
    switch (opt) {
      case 'b': {
        char* endptr;
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
        fprintf(stderr, HELP, baud_rate, from_charset, to_charset);
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

  // create character transcoder
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

  // Process each file specified on the command line
  for (int i = optind; i < argc && !done; i++) {

    // open file
    int fd = open(argv[i], O_RDONLY);
    if (fd == -1) {
      perror(argv[i]);
      break;
    }

    // wait between files
    if (i > optind)
      sleep(1);

    print("\33[?25l");   // hide cursor
    print("\33[H");      // move cursor to top-left
    print("\33[J");      // erase display forward
    print("\33[1;24r");  // set scrolling region to first 24 lines
    print("\33[?7h");    // enable auto-wrap mode
    print("\33[?3l");    // 80 column mode (deccolm) vt100
    print("\33[H");      // move cursor to top-left, again

    // get busy
    process_file(argv[i], fd, cd);
    close(fd);
  }

  // cleanup
  iconv_close(cd);

  print("\33[s");        // save cursor position
  print("\33[?25h");     // show cursor
  print("\33[0m");       // reset text attributes (color, bold, etc.)
  print("\33[?1049l");   // exit alternate screen mode
  print("\33(B");        // exit line drawing and other alt charset modes
  print("\33[r");        // reset scrolling region
  print("\33[?2004l");   // turn off bracketed paste mode
  print("\33[4l");       // exit insert mode
  print("\33[?1l\33>");  // exit application keypad mode
  print("\33[?7h");      // reset text wrapping mode
  print("\33[?12l");     // reset cursor blinking mode
  print("\33[?6l");      // reset origin mode
  print("\33[20l");      // reset auto newline mode
  print("\33[u");        // restore cursor position

  // restore terminal
  tcsetattr(STDIN_FILENO, TCSANOW, &t);
}
