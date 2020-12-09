#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/alg/alg.h"
#include "libc/alg/arraylist.internal.h"
#include "libc/alg/critbit0.h"
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/fmt/conv.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/str/tpdecode.internal.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/x/x.h"

STATIC_YOINK("zip_uri_support");

/**
 * @fileoverview Simple Interactive Spell Checker.
 *
 * This is an attempt to get spell checking to work in Emacs across
 * platforms. While the computer science behind spell checking is very
 * simple, unfortunately Emacs probes all these System Five spell progs
 * similar to how websites have been known to probe User-Agent strings.
 *
 * Here's how we believe the repl works:
 *
 *     $ make -j8 o//examples/ispell.com
 *     $ o//examples/ispell.com
 *     @(#) Cosmopolitan Ispell Version 3.4.00 8 Feb 2015
 *     word: hello
 *     ok
 *     word: héllo
 *     how about: hills, hello, hilly, jello
 *     word: lova
 *     how about: diva, dona, dora, dove, elva, fora, hove, iota
 *     word: hecruhecrue
 *     not found
 *
 * The dictionary for this program is embedded as a text file within the
 * zip structure of the binary. It can be edited after distribution.
 *
 * It's possible to go even fancier than what this code is doing, by
 * using cmudict phonemes, bloom filters, unicode tables e.g. e vs. é,
 * and even doing ML similar to Google's online spell checker.
 *
 * TODO: Figure out why Emacs rejects this interface.
 */

#define MISSING_LETTER_DISTANCE  5
#define MAX_NEARBY_WORD_DISTANCE 6
#define MAX_NEARBY_RESULTS       8

FILE *f;
char *line;
size_t linesize;
const char *query;
struct critbit0 words; /* does O(log 𝑛) fast path lookup */

struct NearbyWords {
  size_t i, n;
  struct WordMatch {
    long dist;
    char *word;
  } * p;
} nearby;

long WordDistance(const char *a, const char *b) {
  long dist;
  int gota, gotb;
  unsigned long h, p;
  wint_t chara, charb;
  dist = p = 0;
  for (;;) {
    gota = abs(tpdecode(a, &chara)); /* parses utf-8 multibyte characters */
    gotb = abs(tpdecode(b, &charb)); /* abs() handles -1, always yields � */
    if (!chara && !charb) break;
    if (!chara || !charb) {
      dist += MISSING_LETTER_DISTANCE;
    } else if ((h = hamming(chara, charb))) {
      dist += h + p++; /* penalize multiple mismatched letters */
    }
    if (chara) a += gota;
    if (charb) b += gotb;
  }
  return dist;
}

intptr_t ConsiderWord(const char *word, void *arg) {
  long dist;
  if ((dist = WordDistance(word, query)) < MAX_NEARBY_WORD_DISTANCE) {
    append(&nearby, &((struct WordMatch){dist, word}));
  }
  return 0;
}

int CompareWords(const struct WordMatch *a, const struct WordMatch *b) {
  return a->dist < b->dist;
}

void FindNearbyWords(void) {
  nearby.i = 0;
  critbit0_allprefixed(&words, "", ConsiderWord, NULL);
  qsort(nearby.p, nearby.i, sizeof(struct WordMatch), (void *)CompareWords);
}

void SpellChecker(void) {
  int i;
  printf("@(#) Cosmopolitan Ispell Version 3.4.00 8 Feb 2015\r\n");
  while (!feof(stdin)) {
    printf("word: ");
    fflush(stdout);
    if (getline(&line, &linesize, stdin) > 0) {
      query = strtolower(chomp(line));
      if (critbit0_contains(&words, query)) {
        printf("ok\r\n");
      } else {
        FindNearbyWords();
        if (nearby.i) {
          printf("how about: ");
          for (i = 0; i < MIN(MAX_NEARBY_RESULTS, nearby.i); ++i) {
            if (i) printf(", ");
            fputs(nearby.p[i].word, stdout);
          }
          printf("\r\n");
        } else {
          printf("not found\r\n");
        }
      }
    }
    printf("\r\n");
  }
  CHECK_NE(-1, fclose(stdin));
  CHECK_NE(-1, fclose(stdout));
}

void LoadWords(void) {
  CHECK_NOTNULL((f = fopen("zip:usr/share/dict/words", "r")));
  while (getline(&line, &linesize, f) > 0) {
    critbit0_insert(&words, strtolower(chomp(line)));
  }
  CHECK_NE(-1, fclose(f));
}

int main(int argc, char *argv[]) {
  showcrashreports();
  LoadWords();
  SpellChecker();
  return 0;
}
