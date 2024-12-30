#/*────────────────────────────────────────────────────────────────╗
┌┘ To the extent possible under law, Justine Tunney has waived     │
│  all copyright and related or neighboring rights to this file,   │
│  as it is written in the following disclaimers:                  │
│    • http://unlicense.org/                                       │
│    • http://creativecommons.org/publicdomain/zero/1.0/           │
╚─────────────────────────────────────────────────────────────────*/
#include <ctype.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>

/**
 * @fileoverview cosmopolitan flash cards viewer
 */

struct Card {
  char* qa[2];
};

atomic_int g_done;

void onsig(int sig) {
  g_done = 1;
}

void* xmalloc(int n) {
  void* p;
  if ((p = malloc(n)))
    return p;
  perror("malloc");
  exit(1);
}

void* xrealloc(void* p, int n) {
  if ((p = realloc(p, n)))
    return p;
  perror("realloc");
  exit(1);
}

char* xstrcat(const char* a, const char* b) {
  char* p;
  size_t n, m;
  n = strlen(a);
  m = strlen(b);
  p = xmalloc(n + m + 1);
  memcpy(p, a, n);
  memcpy(p + n, b, m + 1);
  return p;
}

void shuffle(struct Card* a, int n) {
  while (n > 1) {
    int i = rand() % n--;
    struct Card t = a[i];
    a[i] = a[n];
    a[n] = t;
  }
}

char* trim(char* s) {
  int i;
  if (s) {
    while (isspace(*s))
      ++s;
    for (i = strlen(s); i--;) {
      if (isspace(s[i])) {
        s[i] = 0;
      } else {
        break;
      }
    }
  }
  return s;
}

char* readline(FILE* f) {
  for (;;) {
    char* line = trim(fgetln(f, 0));
    if (!line)
      return 0;
    if (*line != '#')
      if (*line)
        return line;
  }
}

char* fill(const char* text, int max_line_width, int* out_line_count) {
  int text_len = strlen(text);
  char* result = xmalloc(text_len * 2 + 1);
  int result_pos = 0;
  int line_start = 0;
  int line_count = 1;
  int i = 0;
  while (i < text_len && isspace(text[i]))
    i++;
  while (i < text_len) {
    int word_end = i;
    while (word_end < text_len && !isspace(text[word_end]))
      word_end++;
    int word_length = word_end - i;
    if ((result_pos - line_start) + (result_pos > line_start ? 1 : 0) +
            word_length >
        max_line_width) {
      if (result_pos > line_start) {
        ++line_count;
        result[result_pos++] = '\n';
        line_start = result_pos;
      }
    } else if (result_pos > line_start) {
      result[result_pos++] = ' ';
    }
    memcpy(result + result_pos, text + i, word_length);
    result_pos += word_length;
    i = word_end;
    while (i < text_len && isspace(text[i]))
      i++;
  }
  result[result_pos] = '\0';
  result = xrealloc(result, result_pos + 1);
  if (out_line_count)
    *out_line_count = line_count;
  return result;
}

void show(const char* text, int i, int n) {

  // get pseudoteletypewriter dimensions
  struct winsize ws = {80, 25};
  tcgetwinsize(1, &ws);
  int width = ws.ws_col;
  if (width > (int)(ws.ws_col * .9))
    width = ws.ws_col * .9;
  if (width > 80)
    width = 80;
  width &= -2;

  // clear display
  printf("\033[H\033[J");

  // display flash card text in middle of display
  char buf[32];
  int line_count;
  char* lines = fill(text, width, &line_count);
  sprintf(buf, "%d/%d\r\n\r\n", i + 1, n);
  line_count += 2;
  char* extra = xstrcat(buf, lines);
  free(lines);
  char* tokens = extra;
  for (int j = 0;; ++j) {
    char* line = strtok(tokens, "\n");
    tokens = 0;
    if (!line)
      break;
    printf("\033[%d;%dH%s", ws.ws_row / 2 - line_count / 2 + j + 1,
           ws.ws_col / 2 - strlen(line) / 2 + 1, line);
  }
  free(extra);
  fflush(stdout);
}

void usage(FILE* f, const char* prog) {
  fprintf(f,
          "usage: %s FILE\n"
          "\n"
          "here's an example of what your file should look like:\n"
          "\n"
          "  # cosmopolitan flash cards\n"
          "  # california dmv drivers test\n"
          "  \n"
          "  which of the following point totals could result in "
          "your license being suspended by the dmv?\n"
          "  4 points in 12 months (middle)\n"
          "  \n"
          "  at 55 mph under good conditions a passenger vehicle can stop "
          "within\n"
          "  300 feet (not 200, not 400, middle)\n"
          "  \n"
          "  two sets of solid double yellow lines spaced two or more feet "
          "apart indicate\n"
          "  a BARRIER (do not cross unless there's an opening)\n"
          "\n"
          "more specifically, empty lines are ignored, lines starting with\n"
          "a hash are ignored, then an even number of lines must remain,\n"
          "where each two lines is a card, holding question and answer.\n",
          prog);
}

int main(int argc, char* argv[]) {

  // show help
  if (argc != 2) {
    usage(stderr, argv[0]);
    return 1;
  }
  if (!strcmp(argv[1], "-?") ||  //
      !strcmp(argv[1], "-h") ||  //
      !strcmp(argv[1], "--help")) {
    usage(stdout, argv[0]);
    return 0;
  }

  // teletypewriter is required
  if (!isatty(0) || !isatty(1)) {
    perror("isatty");
    return 2;
  }

  // load cards
  FILE* f = fopen(argv[1], "r");
  if (!f) {
    perror(argv[1]);
    return 3;
  }
  int count = 0;
  struct Card* cards = 0;
  for (;;) {
    struct Card card;
    if (!(card.qa[0] = readline(f)))
      break;
    card.qa[0] = strdup(card.qa[0]);
    if (!(card.qa[1] = readline(f))) {
      fprintf(stderr, "%s: flash card file has odd number of lines\n", argv[1]);
      exit(1);
    }
    card.qa[1] = strdup(card.qa[1]);
    cards = xrealloc(cards, (count + 1) * sizeof(struct Card));
    cards[count++] = card;
  }
  fclose(f);

  // randomize
  srand(time(0));
  shuffle(cards, count);

  // catch ctrl-c
  struct sigaction sa;
  sa.sa_flags = 0;
  sa.sa_handler = onsig;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGINT, &sa, 0);

  // enter raw mode
  struct termios ot;
  tcgetattr(1, &ot);
  struct termios nt = ot;
  cfmakeraw(&nt);
  nt.c_lflag |= ISIG;
  tcsetattr(1, TCSANOW, &nt);
  printf("\033[?25l");

  // show flash cards
  int i = 0;
  while (!g_done) {
    show(cards[i / 2].qa[i % 2], i / 2, count);

    // press any key
    char b[8] = {0};
    read(0, b, sizeof(b));

    // q quits
    if (b[0] == 'q')
      break;

    // b or ctrl-b goes backward
    if (b[0] == 'b' ||  //
        b[0] == ('B' ^ 0100)) {
      if (--i < 0)
        i = count * 2 - 1;
      i &= -2;
      continue;
    }

    // p or ctrl-p goes backward
    if (b[0] == 'p' ||  //
        b[0] == ('P' ^ 0100)) {
      if (--i < 0)
        i = count * 2 - 1;
      i &= -2;
      continue;
    }

    // up arrow goes backward
    if (b[0] == 033 &&  //
        b[1] == '[' &&  //
        b[2] == 'A') {
      if (--i < 0)
        i = count * 2 - 1;
      i &= -2;
      continue;
    }

    // left arrow goes backward
    if (b[0] == 033 &&  //
        b[1] == '[' &&  //
        b[2] == 'D') {
      if (--i < 0)
        i = count * 2 - 1;
      i &= -2;
      continue;
    }

    // only advance
    if (++i == count * 2)
      i = 0;
  }

  // free memory
  for (int i = 0; i < count; ++i)
    for (int j = 0; j < 2; ++j)
      free(cards[i].qa[j]);
  free(cards);

  // cleanup terminal and show cursor
  tcsetattr(1, TCSANOW, &ot);
  printf("\033[?25h");
  printf("\n");
}
