#ifndef COSMOPOLITAN_THIRD_PARTY_EDITLINE_INTERNAL_H_
#define COSMOPOLITAN_THIRD_PARTY_EDITLINE_INTERNAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define MEM_INC    64
#define SCREEN_INC 256

extern int rl_eof;
extern int rl_erase;
extern int rl_intr;
extern int rl_kill;
extern int rl_quit;
extern int rl_susp;

void rl_ttyset(int);
void rl_add_slash(char *, char *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_EDITLINE_INTERNAL_H_ */
