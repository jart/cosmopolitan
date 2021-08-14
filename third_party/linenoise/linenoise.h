#ifndef __LINENOISE_H
#define __LINENOISE_H
COSMOPOLITAN_C_START_
/* clang-format off */

typedef struct linenoiseCompletions {
  size_t len;
  char **cvec;
} linenoiseCompletions;

typedef void(linenoiseCompletionCallback)(const char *, linenoiseCompletions *);
typedef char*(linenoiseHintsCallback)(const char *, int *, int *);
typedef void(linenoiseFreeHintsCallback)(void *);

void linenoiseSetCompletionCallback(linenoiseCompletionCallback *);
void linenoiseSetHintsCallback(linenoiseHintsCallback *);
void linenoiseSetFreeHintsCallback(linenoiseFreeHintsCallback *);
void linenoiseAddCompletion(linenoiseCompletions *, const char *);

char *linenoise(const char *);
void linenoiseFree(void *);
int linenoiseHistoryAdd(const char *);
int linenoiseHistorySetMaxLen(int);
int linenoiseHistorySave(const char *);
int linenoiseHistoryLoad(const char *);
void linenoiseClearScreen(void);
void linenoiseSetMultiLine(int );
void linenoisePrintKeyCodes(void);
void linenoiseMaskModeEnable(void);
void linenoiseMaskModeDisable(void);
void linenoiseDisableRawMode(int);

COSMOPOLITAN_C_END_
#endif /* __LINENOISE_H */
