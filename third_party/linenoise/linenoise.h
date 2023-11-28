#ifndef COSMOPOLITAN_THIRD_PARTY_LINENOISE_LINENOISE_H_
#define COSMOPOLITAN_THIRD_PARTY_LINENOISE_LINENOISE_H_
#include "libc/calls/struct/winsize.h"
#include "libc/stdio/stdio.h"
COSMOPOLITAN_C_START_

struct linenoiseState;
typedef struct linenoiseCompletions {
  size_t len;
  char **cvec;
} linenoiseCompletions;

typedef void(linenoiseCompletionCallback)(const char *, linenoiseCompletions *);
typedef char *(linenoiseHintsCallback)(const char *, const char **,
                                       const char **);
typedef void(linenoiseFreeHintsCallback)(void *);
typedef wint_t(linenoiseXlatCallback)(wint_t);

void linenoiseSetCompletionCallback(linenoiseCompletionCallback *);
void linenoiseSetHintsCallback(linenoiseHintsCallback *);
void linenoiseSetFreeHintsCallback(linenoiseFreeHintsCallback *);
void linenoiseAddCompletion(linenoiseCompletions *, const char *);
void linenoiseSetXlatCallback(linenoiseXlatCallback *);

char *linenoise(const char *) __wur;
char *linenoiseWithHistory(const char *, const char *) __wur;
int linenoiseHistoryAdd(const char *);
int linenoiseHistorySave(const char *);
int linenoiseHistoryLoad(const char *);
void linenoiseFreeCompletions(linenoiseCompletions *);
void linenoiseHistoryFree(void);
void linenoiseClearScreen(int);
void linenoiseMaskModeEnable(void);
void linenoiseMaskModeDisable(void);
int linenoiseEnableRawMode(int);
void linenoiseDisableRawMode(void);
void linenoiseFree(void *);
int linenoiseIsTerminal(void);
int linenoiseIsTeletype(void);

char *linenoiseGetHistoryPath(const char *);
struct linenoiseState *linenoiseBegin(const char *, int, int);
ssize_t linenoiseEdit(struct linenoiseState *, const char *, char **, bool);
int linenoiseGetInterrupt(void);
void linenoiseEnd(struct linenoiseState *);

char *linenoiseGetLine(FILE *);
struct winsize linenoiseGetTerminalSize(struct winsize, int, int);
void linenoiseRefreshLine(struct linenoiseState *);
char *linenoiseRaw(const char *, int, int) __wur;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_LINENOISE_LINENOISE_H_ */
