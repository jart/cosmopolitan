#ifndef COSMOPOLITAN_THIRD_PARTY_LINENOISE_LINENOISE_H_
#define COSMOPOLITAN_THIRD_PARTY_LINENOISE_LINENOISE_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

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

char *linenoise(const char *) nodiscard;
char *linenoiseRaw(const char *, int, int) nodiscard;
char *linenoiseWithHistory(const char *, const char *) nodiscard;
int linenoiseHistoryAdd(const char *);
int linenoiseHistorySave(const char *);
int linenoiseHistoryLoad(const char *);
void linenoiseFreeCompletions(linenoiseCompletions *);
void linenoiseHistoryFree(void);
void linenoiseClearScreen(int);
void linenoiseMaskModeEnable(void);
void linenoiseMaskModeDisable(void);
void linenoiseDisableRawMode(void);
void linenoiseFree(void *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_LINENOISE_LINENOISE_H_ */
