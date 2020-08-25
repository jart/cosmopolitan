#ifndef _MAIN_H
#define _MAIN_H
#include "third_party/ctags/general.h"
/* must always come first */
#include "third_party/ctags/vstring.h"

/*
 *   FUNCTION PROTOTYPES
 */
extern void addTotals(const unsigned int files, const long unsigned int lines,
                      const long unsigned int bytes);
extern boolean isDestinationStdout(void);
extern int main(int argc, char **argv);

#endif /* _MAIN_H */
