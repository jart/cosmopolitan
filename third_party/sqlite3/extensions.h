#ifndef COSMOPOLITAN_THIRD_PARTY_SQLITE3_EXTENSIONS_H_
#define COSMOPOLITAN_THIRD_PARTY_SQLITE3_EXTENSIONS_H_
#include "libc/stdio/stdio.h"
#include "third_party/sqlite3/sqlite3.h"
COSMOPOLITAN_C_START_

int sqlite3MemTraceActivate(FILE *);
int sqlite3MemTraceDeactivate(void);

int sqlite3_appendvfs_init(sqlite3 *, char **, const sqlite3_api_routines *);
int sqlite3_completion_init(sqlite3 *, char **, const sqlite3_api_routines *);
int sqlite3_dbdata_init(sqlite3 *, char **, const sqlite3_api_routines *);
int sqlite3_decimal_init(sqlite3 *, char **, const sqlite3_api_routines *);
int sqlite3_fileio_init(sqlite3 *, char **, const sqlite3_api_routines *);
int sqlite3_ieee_init(sqlite3 *, char **, const sqlite3_api_routines *);
int sqlite3_series_init(sqlite3 *, char **, const sqlite3_api_routines *);
int sqlite3_shathree_init(sqlite3 *, char **, const sqlite3_api_routines *);
int sqlite3_sqlar_init(sqlite3 *, char **, const sqlite3_api_routines *);
int sqlite3_uint_init(sqlite3 *, char **, const sqlite3_api_routines *);
int sqlite3_zipfile_init(sqlite3 *, char **, const sqlite3_api_routines *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_SQLITE3_EXTENSIONS_H_ */
