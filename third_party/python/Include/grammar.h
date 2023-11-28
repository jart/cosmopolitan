#ifndef Py_GRAMMAR_H
#define Py_GRAMMAR_H
#include "libc/stdio/stdio.h"
#include "third_party/python/Include/bitset.h"
COSMOPOLITAN_C_START_

/* A label of an arc */
typedef struct {
    int		 lb_type;
    char	*lb_str;
} label;

#define EMPTY 0		/* Label number 0 is by definition the empty label */

/* A list of labels */
typedef struct {
    int		 ll_nlabels;
    label	*ll_label;
} labellist;

/* An arc from one state to another */
typedef struct {
    short	a_lbl;		/* Label of this arc */
    short	a_arrow;	/* State where this arc goes to */
} arc;

/* A state in a DFA */
typedef struct {
    int		 s_narcs;       /* [jart/abi] moving s_accept here saves 4k */
    int		 s_accept;	/* [optional] Nonzero for accepting state */
    arc		*s_arc;		/* Array of arcs */
    int		 s_lower;	/* [optional] Lowest label index */
    int		 s_upper;	/* [optional] Highest label index */
    int		*s_accel;	/* [optional] Accelerator */
} state;

/* A DFA */
typedef struct {
    int		 d_type;	/* Non-terminal this represents */
    char	*d_name;	/* For printing */
    int		 d_initial;	/* Initial state */
    int		 d_nstates;
    state	*d_state;	/* Array of states */
    bitset	 d_first;
} dfa;

/* A grammar */
typedef struct {
    int		 g_ndfas;
    dfa		*g_dfa;		/* Array of DFAs */
    labellist	 g_ll;
    int		 g_start;	/* Start symbol of the grammar */
    int		 g_accel;	/* Set if accelerators present */
} grammar;

grammar *newgrammar(int);
void freegrammar(grammar *);
dfa *adddfa(grammar *, int, const char *);
int addstate(dfa *);
void addarc(dfa *, int, int, int);
dfa *PyGrammar_FindDFA(grammar *, int);
int addlabel(labellist *, int, const char *);
int findlabel(labellist *, int, const char *);
const char *PyGrammar_LabelRepr(label *);
void translatelabels(grammar *);
void addfirstsets(grammar *);
void PyGrammar_AddAccelerators(grammar *);
void PyGrammar_RemoveAccelerators(grammar *);
void printgrammar(grammar *, FILE *);
void printnonterminals(grammar *, FILE *);

#ifndef Py_LIMITED_API
extern grammar _PyParser_Grammar;
#endif

COSMOPOLITAN_C_END_
#endif /* !Py_GRAMMAR_H */
