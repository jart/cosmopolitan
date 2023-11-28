#ifndef COSMOPOLITAN_THIRD_PARTY_SED_EXTERN_H_
#define COSMOPOLITAN_THIRD_PARTY_SED_EXTERN_H_
#include "libc/calls/typedef/u.h"
#include "libc/stdio/stdio.h"
#include "third_party/regex/regex.h"
#include "third_party/sed/defs.h"
COSMOPOLITAN_C_START_

extern struct s_command *prog;
extern struct s_appends *appends_;
extern regmatch_t *g_match;
extern size_t maxnsub;
extern u_long linenum;
extern size_t appendnum;
extern int aflag, eflag, nflag;
extern const char *fname, *outfname;
extern FILE *infile, *outfile;
extern int rflags;	/* regex flags to use */

void	 cfclose(struct s_command *, struct s_command *);
void	 compile(void);
void	 cspace(SPACE *, const char *, size_t, enum e_spflag);
char	*cu_fgets(char *, int, int *);
int	 mf_fgets(SPACE *, enum e_spflag);
int	 lastline(void);
void	 process(void);
void	 resetstate(void);
char	*strregerror(int, regex_t *);
void	*xmalloc(size_t);
void	*xrealloc(void *, size_t);
void	*xcalloc(size_t, size_t);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_SED_EXTERN_H_ */
