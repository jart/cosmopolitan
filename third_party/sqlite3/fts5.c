
#if !defined(SQLITE_CORE) || defined(SQLITE_ENABLE_FTS5) 

#if !defined(NDEBUG) && !defined(SQLITE_DEBUG) 
# define NDEBUG 1
#endif
#if defined(NDEBUG) && defined(SQLITE_DEBUG)
# undef NDEBUG
#endif

#line 1 "third_party/sqlite3/fts5.h"
/*
** 2014 May 31
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
******************************************************************************
**
** Interfaces to extend FTS5. Using the interfaces defined in this file, 
** FTS5 may be extended with:
**
**     * custom tokenizers, and
**     * custom auxiliary functions.
*/


#ifndef _FTS5_H
#define _FTS5_H

#include "third_party/sqlite3/sqlite3.h"

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************************
** CUSTOM AUXILIARY FUNCTIONS
**
** Virtual table implementations may overload SQL functions by implementing
** the sqlite3_module.xFindFunction() method.
*/

typedef struct Fts5ExtensionApi Fts5ExtensionApi;
typedef struct Fts5Context Fts5Context;
typedef struct Fts5PhraseIter Fts5PhraseIter;

typedef void (*fts5_extension_function)(
  const Fts5ExtensionApi *pApi,   /* API offered by current FTS version */
  Fts5Context *pFts,              /* First arg to pass to pApi functions */
  sqlite3_context *pCtx,          /* Context for returning result/error */
  int nVal,                       /* Number of values in apVal[] array */
  sqlite3_value **apVal           /* Array of trailing arguments */
);

struct Fts5PhraseIter {
  const unsigned char *a;
  const unsigned char *b;
};

/*
** EXTENSION API FUNCTIONS
**
** xUserData(pFts):
**   Return a copy of the context pointer the extension function was 
**   registered with.
**
** xColumnTotalSize(pFts, iCol, pnToken):
**   If parameter iCol is less than zero, set output variable *pnToken
**   to the total number of tokens in the FTS5 table. Or, if iCol is
**   non-negative but less than the number of columns in the table, return
**   the total number of tokens in column iCol, considering all rows in 
**   the FTS5 table.
**
**   If parameter iCol is greater than or equal to the number of columns
**   in the table, SQLITE_RANGE is returned. Or, if an error occurs (e.g.
**   an OOM condition or IO error), an appropriate SQLite error code is 
**   returned.
**
** xColumnCount(pFts):
**   Return the number of columns in the table.
**
** xColumnSize(pFts, iCol, pnToken):
**   If parameter iCol is less than zero, set output variable *pnToken
**   to the total number of tokens in the current row. Or, if iCol is
**   non-negative but less than the number of columns in the table, set
**   *pnToken to the number of tokens in column iCol of the current row.
**
**   If parameter iCol is greater than or equal to the number of columns
**   in the table, SQLITE_RANGE is returned. Or, if an error occurs (e.g.
**   an OOM condition or IO error), an appropriate SQLite error code is 
**   returned.
**
**   This function may be quite inefficient if used with an FTS5 table
**   created with the "columnsize=0" option.
**
** xColumnText:
**   This function attempts to retrieve the text of column iCol of the
**   current document. If successful, (*pz) is set to point to a buffer
**   containing the text in utf-8 encoding, (*pn) is set to the size in bytes
**   (not characters) of the buffer and SQLITE_OK is returned. Otherwise,
**   if an error occurs, an SQLite error code is returned and the final values
**   of (*pz) and (*pn) are undefined.
**
** xPhraseCount:
**   Returns the number of phrases in the current query expression.
**
** xPhraseSize:
**   Returns the number of tokens in phrase iPhrase of the query. Phrases
**   are numbered starting from zero.
**
** xInstCount:
**   Set *pnInst to the total number of occurrences of all phrases within
**   the query within the current row. Return SQLITE_OK if successful, or
**   an error code (i.e. SQLITE_NOMEM) if an error occurs.
**
**   This API can be quite slow if used with an FTS5 table created with the
**   "detail=none" or "detail=column" option. If the FTS5 table is created 
**   with either "detail=none" or "detail=column" and "content=" option 
**   (i.e. if it is a contentless table), then this API always returns 0.
**
** xInst:
**   Query for the details of phrase match iIdx within the current row.
**   Phrase matches are numbered starting from zero, so the iIdx argument
**   should be greater than or equal to zero and smaller than the value
**   output by xInstCount().
**
**   Usually, output parameter *piPhrase is set to the phrase number, *piCol
**   to the column in which it occurs and *piOff the token offset of the
**   first token of the phrase. Returns SQLITE_OK if successful, or an error
**   code (i.e. SQLITE_NOMEM) if an error occurs.
**
**   This API can be quite slow if used with an FTS5 table created with the
**   "detail=none" or "detail=column" option. 
**
** xRowid:
**   Returns the rowid of the current row.
**
** xTokenize:
**   Tokenize text using the tokenizer belonging to the FTS5 table.
**
** xQueryPhrase(pFts5, iPhrase, pUserData, xCallback):
**   This API function is used to query the FTS table for phrase iPhrase
**   of the current query. Specifically, a query equivalent to:
**
**       ... FROM ftstable WHERE ftstable MATCH $p ORDER BY rowid
**
**   with $p set to a phrase equivalent to the phrase iPhrase of the
**   current query is executed. Any column filter that applies to
**   phrase iPhrase of the current query is included in $p. For each 
**   row visited, the callback function passed as the fourth argument 
**   is invoked. The context and API objects passed to the callback 
**   function may be used to access the properties of each matched row.
**   Invoking Api.xUserData() returns a copy of the pointer passed as 
**   the third argument to pUserData.
**
**   If the callback function returns any value other than SQLITE_OK, the
**   query is abandoned and the xQueryPhrase function returns immediately.
**   If the returned value is SQLITE_DONE, xQueryPhrase returns SQLITE_OK.
**   Otherwise, the error code is propagated upwards.
**
**   If the query runs to completion without incident, SQLITE_OK is returned.
**   Or, if some error occurs before the query completes or is aborted by
**   the callback, an SQLite error code is returned.
**
**
** xSetAuxdata(pFts5, pAux, xDelete)
**
**   Save the pointer passed as the second argument as the extension function's 
**   "auxiliary data". The pointer may then be retrieved by the current or any
**   future invocation of the same fts5 extension function made as part of
**   the same MATCH query using the xGetAuxdata() API.
**
**   Each extension function is allocated a single auxiliary data slot for
**   each FTS query (MATCH expression). If the extension function is invoked 
**   more than once for a single FTS query, then all invocations share a 
**   single auxiliary data context.
**
**   If there is already an auxiliary data pointer when this function is
**   invoked, then it is replaced by the new pointer. If an xDelete callback
**   was specified along with the original pointer, it is invoked at this
**   point.
**
**   The xDelete callback, if one is specified, is also invoked on the
**   auxiliary data pointer after the FTS5 query has finished.
**
**   If an error (e.g. an OOM condition) occurs within this function,
**   the auxiliary data is set to NULL and an error code returned. If the
**   xDelete parameter was not NULL, it is invoked on the auxiliary data
**   pointer before returning.
**
**
** xGetAuxdata(pFts5, bClear)
**
**   Returns the current auxiliary data pointer for the fts5 extension 
**   function. See the xSetAuxdata() method for details.
**
**   If the bClear argument is non-zero, then the auxiliary data is cleared
**   (set to NULL) before this function returns. In this case the xDelete,
**   if any, is not invoked.
**
**
** xRowCount(pFts5, pnRow)
**
**   This function is used to retrieve the total number of rows in the table.
**   In other words, the same value that would be returned by:
**
**        SELECT count(*) FROM ftstable;
**
** xPhraseFirst()
**   This function is used, along with type Fts5PhraseIter and the xPhraseNext
**   method, to iterate through all instances of a single query phrase within
**   the current row. This is the same information as is accessible via the
**   xInstCount/xInst APIs. While the xInstCount/xInst APIs are more convenient
**   to use, this API may be faster under some circumstances. To iterate 
**   through instances of phrase iPhrase, use the following code:
**
**       Fts5PhraseIter iter;
**       int iCol, iOff;
**       for(pApi->xPhraseFirst(pFts, iPhrase, &iter, &iCol, &iOff);
**           iCol>=0;
**           pApi->xPhraseNext(pFts, &iter, &iCol, &iOff)
**       ){
**         // An instance of phrase iPhrase at offset iOff of column iCol
**       }
**
**   The Fts5PhraseIter structure is defined above. Applications should not
**   modify this structure directly - it should only be used as shown above
**   with the xPhraseFirst() and xPhraseNext() API methods (and by
**   xPhraseFirstColumn() and xPhraseNextColumn() as illustrated below).
**
**   This API can be quite slow if used with an FTS5 table created with the
**   "detail=none" or "detail=column" option. If the FTS5 table is created 
**   with either "detail=none" or "detail=column" and "content=" option 
**   (i.e. if it is a contentless table), then this API always iterates
**   through an empty set (all calls to xPhraseFirst() set iCol to -1).
**
** xPhraseNext()
**   See xPhraseFirst above.
**
** xPhraseFirstColumn()
**   This function and xPhraseNextColumn() are similar to the xPhraseFirst()
**   and xPhraseNext() APIs described above. The difference is that instead
**   of iterating through all instances of a phrase in the current row, these
**   APIs are used to iterate through the set of columns in the current row
**   that contain one or more instances of a specified phrase. For example:
**
**       Fts5PhraseIter iter;
**       int iCol;
**       for(pApi->xPhraseFirstColumn(pFts, iPhrase, &iter, &iCol);
**           iCol>=0;
**           pApi->xPhraseNextColumn(pFts, &iter, &iCol)
**       ){
**         // Column iCol contains at least one instance of phrase iPhrase
**       }
**
**   This API can be quite slow if used with an FTS5 table created with the
**   "detail=none" option. If the FTS5 table is created with either 
**   "detail=none" "content=" option (i.e. if it is a contentless table), 
**   then this API always iterates through an empty set (all calls to 
**   xPhraseFirstColumn() set iCol to -1).
**
**   The information accessed using this API and its companion
**   xPhraseFirstColumn() may also be obtained using xPhraseFirst/xPhraseNext
**   (or xInst/xInstCount). The chief advantage of this API is that it is
**   significantly more efficient than those alternatives when used with
**   "detail=column" tables.  
**
** xPhraseNextColumn()
**   See xPhraseFirstColumn above.
*/
struct Fts5ExtensionApi {
  int iVersion;                   /* Currently always set to 3 */

  void *(*xUserData)(Fts5Context*);

  int (*xColumnCount)(Fts5Context*);
  int (*xRowCount)(Fts5Context*, sqlite3_int64 *pnRow);
  int (*xColumnTotalSize)(Fts5Context*, int iCol, sqlite3_int64 *pnToken);

  int (*xTokenize)(Fts5Context*, 
    const char *pText, int nText, /* Text to tokenize */
    void *pCtx,                   /* Context passed to xToken() */
    int (*xToken)(void*, int, const char*, int, int, int)       /* Callback */
  );

  int (*xPhraseCount)(Fts5Context*);
  int (*xPhraseSize)(Fts5Context*, int iPhrase);

  int (*xInstCount)(Fts5Context*, int *pnInst);
  int (*xInst)(Fts5Context*, int iIdx, int *piPhrase, int *piCol, int *piOff);

  sqlite3_int64 (*xRowid)(Fts5Context*);
  int (*xColumnText)(Fts5Context*, int iCol, const char **pz, int *pn);
  int (*xColumnSize)(Fts5Context*, int iCol, int *pnToken);

  int (*xQueryPhrase)(Fts5Context*, int iPhrase, void *pUserData,
    int(*)(const Fts5ExtensionApi*,Fts5Context*,void*)
  );
  int (*xSetAuxdata)(Fts5Context*, void *pAux, void(*xDelete)(void*));
  void *(*xGetAuxdata)(Fts5Context*, int bClear);

  int (*xPhraseFirst)(Fts5Context*, int iPhrase, Fts5PhraseIter*, int*, int*);
  void (*xPhraseNext)(Fts5Context*, Fts5PhraseIter*, int *piCol, int *piOff);

  int (*xPhraseFirstColumn)(Fts5Context*, int iPhrase, Fts5PhraseIter*, int*);
  void (*xPhraseNextColumn)(Fts5Context*, Fts5PhraseIter*, int *piCol);
};

/* 
** CUSTOM AUXILIARY FUNCTIONS
*************************************************************************/

/*************************************************************************
** CUSTOM TOKENIZERS
**
** Applications may also register custom tokenizer types. A tokenizer 
** is registered by providing fts5 with a populated instance of the 
** following structure. All structure methods must be defined, setting
** any member of the fts5_tokenizer struct to NULL leads to undefined
** behaviour. The structure methods are expected to function as follows:
**
** xCreate:
**   This function is used to allocate and initialize a tokenizer instance.
**   A tokenizer instance is required to actually tokenize text.
**
**   The first argument passed to this function is a copy of the (void*)
**   pointer provided by the application when the fts5_tokenizer object
**   was registered with FTS5 (the third argument to xCreateTokenizer()). 
**   The second and third arguments are an array of nul-terminated strings
**   containing the tokenizer arguments, if any, specified following the
**   tokenizer name as part of the CREATE VIRTUAL TABLE statement used
**   to create the FTS5 table.
**
**   The final argument is an output variable. If successful, (*ppOut) 
**   should be set to point to the new tokenizer handle and SQLITE_OK
**   returned. If an error occurs, some value other than SQLITE_OK should
**   be returned. In this case, fts5 assumes that the final value of *ppOut 
**   is undefined.
**
** xDelete:
**   This function is invoked to delete a tokenizer handle previously
**   allocated using xCreate(). Fts5 guarantees that this function will
**   be invoked exactly once for each successful call to xCreate().
**
** xTokenize:
**   This function is expected to tokenize the nText byte string indicated 
**   by argument pText. pText may or may not be nul-terminated. The first
**   argument passed to this function is a pointer to an Fts5Tokenizer object
**   returned by an earlier call to xCreate().
**
**   The second argument indicates the reason that FTS5 is requesting
**   tokenization of the supplied text. This is always one of the following
**   four values:
**
**   <ul><li> <b>FTS5_TOKENIZE_DOCUMENT</b> - A document is being inserted into
**            or removed from the FTS table. The tokenizer is being invoked to
**            determine the set of tokens to add to (or delete from) the
**            FTS index.
**
**       <li> <b>FTS5_TOKENIZE_QUERY</b> - A MATCH query is being executed 
**            against the FTS index. The tokenizer is being called to tokenize 
**            a bareword or quoted string specified as part of the query.
**
**       <li> <b>(FTS5_TOKENIZE_QUERY | FTS5_TOKENIZE_PREFIX)</b> - Same as
**            FTS5_TOKENIZE_QUERY, except that the bareword or quoted string is
**            followed by a "*" character, indicating that the last token
**            returned by the tokenizer will be treated as a token prefix.
**
**       <li> <b>FTS5_TOKENIZE_AUX</b> - The tokenizer is being invoked to 
**            satisfy an fts5_api.xTokenize() request made by an auxiliary
**            function. Or an fts5_api.xColumnSize() request made by the same
**            on a columnsize=0 database.  
**   </ul>
**
**   For each token in the input string, the supplied callback xToken() must
**   be invoked. The first argument to it should be a copy of the pointer
**   passed as the second argument to xTokenize(). The third and fourth
**   arguments are a pointer to a buffer containing the token text, and the
**   size of the token in bytes. The 4th and 5th arguments are the byte offsets
**   of the first byte of and first byte immediately following the text from
**   which the token is derived within the input.
**
**   The second argument passed to the xToken() callback ("tflags") should
**   normally be set to 0. The exception is if the tokenizer supports 
**   synonyms. In this case see the discussion below for details.
**
**   FTS5 assumes the xToken() callback is invoked for each token in the 
**   order that they occur within the input text.
**
**   If an xToken() callback returns any value other than SQLITE_OK, then
**   the tokenization should be abandoned and the xTokenize() method should
**   immediately return a copy of the xToken() return value. Or, if the
**   input buffer is exhausted, xTokenize() should return SQLITE_OK. Finally,
**   if an error occurs with the xTokenize() implementation itself, it
**   may abandon the tokenization and return any error code other than
**   SQLITE_OK or SQLITE_DONE.
**
** SYNONYM SUPPORT
**
**   Custom tokenizers may also support synonyms. Consider a case in which a
**   user wishes to query for a phrase such as "first place". Using the 
**   built-in tokenizers, the FTS5 query 'first + place' will match instances
**   of "first place" within the document set, but not alternative forms
**   such as "1st place". In some applications, it would be better to match
**   all instances of "first place" or "1st place" regardless of which form
**   the user specified in the MATCH query text.
**
**   There are several ways to approach this in FTS5:
**
**   <ol><li> By mapping all synonyms to a single token. In this case, using
**            the above example, this means that the tokenizer returns the
**            same token for inputs "first" and "1st". Say that token is in
**            fact "first", so that when the user inserts the document "I won
**            1st place" entries are added to the index for tokens "i", "won",
**            "first" and "place". If the user then queries for '1st + place',
**            the tokenizer substitutes "first" for "1st" and the query works
**            as expected.
**
**       <li> By querying the index for all synonyms of each query term
**            separately. In this case, when tokenizing query text, the
**            tokenizer may provide multiple synonyms for a single term 
**            within the document. FTS5 then queries the index for each 
**            synonym individually. For example, faced with the query:
**
**   <codeblock>
**     ... MATCH 'first place'</codeblock>
**
**            the tokenizer offers both "1st" and "first" as synonyms for the
**            first token in the MATCH query and FTS5 effectively runs a query 
**            similar to:
**
**   <codeblock>
**     ... MATCH '(first OR 1st) place'</codeblock>
**
**            except that, for the purposes of auxiliary functions, the query
**            still appears to contain just two phrases - "(first OR 1st)" 
**            being treated as a single phrase.
**
**       <li> By adding multiple synonyms for a single term to the FTS index.
**            Using this method, when tokenizing document text, the tokenizer
**            provides multiple synonyms for each token. So that when a 
**            document such as "I won first place" is tokenized, entries are
**            added to the FTS index for "i", "won", "first", "1st" and
**            "place".
**
**            This way, even if the tokenizer does not provide synonyms
**            when tokenizing query text (it should not - to do so would be
**            inefficient), it doesn't matter if the user queries for 
**            'first + place' or '1st + place', as there are entries in the
**            FTS index corresponding to both forms of the first token.
**   </ol>
**
**   Whether it is parsing document or query text, any call to xToken that
**   specifies a <i>tflags</i> argument with the FTS5_TOKEN_COLOCATED bit
**   is considered to supply a synonym for the previous token. For example,
**   when parsing the document "I won first place", a tokenizer that supports
**   synonyms would call xToken() 5 times, as follows:
**
**   <codeblock>
**       xToken(pCtx, 0, "i",                      1,  0,  1);
**       xToken(pCtx, 0, "won",                    3,  2,  5);
**       xToken(pCtx, 0, "first",                  5,  6, 11);
**       xToken(pCtx, FTS5_TOKEN_COLOCATED, "1st", 3,  6, 11);
**       xToken(pCtx, 0, "place",                  5, 12, 17);
**</codeblock>
**
**   It is an error to specify the FTS5_TOKEN_COLOCATED flag the first time
**   xToken() is called. Multiple synonyms may be specified for a single token
**   by making multiple calls to xToken(FTS5_TOKEN_COLOCATED) in sequence. 
**   There is no limit to the number of synonyms that may be provided for a
**   single token.
**
**   In many cases, method (1) above is the best approach. It does not add 
**   extra data to the FTS index or require FTS5 to query for multiple terms,
**   so it is efficient in terms of disk space and query speed. However, it
**   does not support prefix queries very well. If, as suggested above, the
**   token "first" is substituted for "1st" by the tokenizer, then the query:
**
**   <codeblock>
**     ... MATCH '1s*'</codeblock>
**
**   will not match documents that contain the token "1st" (as the tokenizer
**   will probably not map "1s" to any prefix of "first").
**
**   For full prefix support, method (3) may be preferred. In this case, 
**   because the index contains entries for both "first" and "1st", prefix
**   queries such as 'fi*' or '1s*' will match correctly. However, because
**   extra entries are added to the FTS index, this method uses more space
**   within the database.
**
**   Method (2) offers a midpoint between (1) and (3). Using this method,
**   a query such as '1s*' will match documents that contain the literal 
**   token "1st", but not "first" (assuming the tokenizer is not able to
**   provide synonyms for prefixes). However, a non-prefix query like '1st'
**   will match against "1st" and "first". This method does not require
**   extra disk space, as no extra entries are added to the FTS index. 
**   On the other hand, it may require more CPU cycles to run MATCH queries,
**   as separate queries of the FTS index are required for each synonym.
**
**   When using methods (2) or (3), it is important that the tokenizer only
**   provide synonyms when tokenizing document text (method (2)) or query
**   text (method (3)), not both. Doing so will not cause any errors, but is
**   inefficient.
*/
typedef struct Fts5Tokenizer Fts5Tokenizer;
typedef struct fts5_tokenizer fts5_tokenizer;
struct fts5_tokenizer {
  int (*xCreate)(void*, const char **azArg, int nArg, Fts5Tokenizer **ppOut);
  void (*xDelete)(Fts5Tokenizer*);
  int (*xTokenize)(Fts5Tokenizer*, 
      void *pCtx,
      int flags,            /* Mask of FTS5_TOKENIZE_* flags */
      const char *pText, int nText, 
      int (*xToken)(
        void *pCtx,         /* Copy of 2nd argument to xTokenize() */
        int tflags,         /* Mask of FTS5_TOKEN_* flags */
        const char *pToken, /* Pointer to buffer containing token */
        int nToken,         /* Size of token in bytes */
        int iStart,         /* Byte offset of token within input text */
        int iEnd            /* Byte offset of end of token within input text */
      )
  );
};

/* Flags that may be passed as the third argument to xTokenize() */
#define FTS5_TOKENIZE_QUERY     0x0001
#define FTS5_TOKENIZE_PREFIX    0x0002
#define FTS5_TOKENIZE_DOCUMENT  0x0004
#define FTS5_TOKENIZE_AUX       0x0008

/* Flags that may be passed by the tokenizer implementation back to FTS5
** as the third argument to the supplied xToken callback. */
#define FTS5_TOKEN_COLOCATED    0x0001      /* Same position as prev. token */

/*
** END OF CUSTOM TOKENIZERS
*************************************************************************/

/*************************************************************************
** FTS5 EXTENSION REGISTRATION API
*/
typedef struct fts5_api fts5_api;
struct fts5_api {
  int iVersion;                   /* Currently always set to 2 */

  /* Create a new tokenizer */
  int (*xCreateTokenizer)(
    fts5_api *pApi,
    const char *zName,
    void *pContext,
    fts5_tokenizer *pTokenizer,
    void (*xDestroy)(void*)
  );

  /* Find an existing tokenizer */
  int (*xFindTokenizer)(
    fts5_api *pApi,
    const char *zName,
    void **ppContext,
    fts5_tokenizer *pTokenizer
  );

  /* Create a new auxiliary function */
  int (*xCreateFunction)(
    fts5_api *pApi,
    const char *zName,
    void *pContext,
    fts5_extension_function xFunction,
    void (*xDestroy)(void*)
  );
};

/*
** END OF REGISTRATION API
*************************************************************************/

#ifdef __cplusplus
}  /* end of the 'extern "C"' block */
#endif

#endif /* _FTS5_H */

#line 1 "fts5Int.h"
/*
** 2014 May 31
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
******************************************************************************
**
*/
#ifndef _FTS5INT_H
#define _FTS5INT_H

/* #include "fts5.h" */
#include "third_party/sqlite3/sqlite3ext.h"
SQLITE_EXTENSION_INIT1

#include "libc/assert.h"
#include "libc/math.h"
#include "libc/str/str.h"

#ifndef SQLITE_AMALGAMATION

typedef unsigned char  u8;
typedef unsigned int   u32;
typedef unsigned short u16;
typedef short i16;
typedef sqlite3_int64 i64;
typedef sqlite3_uint64 u64;

#ifndef ArraySize
# define ArraySize(x) ((int)(sizeof(x) / sizeof(x[0])))
#endif

#define testcase(x)

#if defined(SQLITE_COVERAGE_TEST) || defined(SQLITE_MUTATION_TEST)
# define SQLITE_OMIT_AUXILIARY_SAFETY_CHECKS 1
#endif
#if defined(SQLITE_OMIT_AUXILIARY_SAFETY_CHECKS)
# define ALWAYS(X)      (1)
# define NEVER(X)       (0)
#elif !defined(NDEBUG)
# define ALWAYS(X)      ((X)?1:(assert(0),0))
# define NEVER(X)       ((X)?(assert(0),1):0)
#else
# define ALWAYS(X)      (X)
# define NEVER(X)       (X)
#endif

#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#define MAX(x,y) (((x) > (y)) ? (x) : (y))

/*
** Constants for the largest and smallest possible 64-bit signed integers.
*/
# define LARGEST_INT64  (0xffffffff|(((i64)0x7fffffff)<<32))
# define SMALLEST_INT64 (((i64)-1) - LARGEST_INT64)

#endif

/* Truncate very long tokens to this many bytes. Hard limit is 
** (65536-1-1-4-9)==65521 bytes. The limiting factor is the 16-bit offset
** field that occurs at the start of each leaf page (see fts5_index.c). */
#define FTS5_MAX_TOKEN_SIZE 32768

/*
** Maximum number of prefix indexes on single FTS5 table. This must be
** less than 32. If it is set to anything large than that, an #error
** directive in fts5_index.c will cause the build to fail.
*/
#define FTS5_MAX_PREFIX_INDEXES 31

/*
** Maximum segments permitted in a single index 
*/
#define FTS5_MAX_SEGMENT 2000

#define FTS5_DEFAULT_NEARDIST 10
#define FTS5_DEFAULT_RANK     "bm25"

/* Name of rank and rowid columns */
#define FTS5_RANK_NAME "rank"
#define FTS5_ROWID_NAME "rowid"

#ifdef SQLITE_DEBUG
# define FTS5_CORRUPT sqlite3Fts5Corrupt()
static int sqlite3Fts5Corrupt(void);
#else
# define FTS5_CORRUPT SQLITE_CORRUPT_VTAB
#endif

/*
** The assert_nc() macro is similar to the assert() macro, except that it
** is used for assert() conditions that are true only if it can be 
** guranteed that the database is not corrupt.
*/
#ifdef SQLITE_DEBUG
extern int sqlite3_fts5_may_be_corrupt;
# define assert_nc(x) assert(sqlite3_fts5_may_be_corrupt || (x))
#else
# define assert_nc(x) assert(x)
#endif

/*
** A version of memcmp() that does not cause asan errors if one of the pointer
** parameters is NULL and the number of bytes to compare is zero.
*/
#define fts5Memcmp(s1, s2, n) ((n)<=0 ? 0 : memcmp((s1), (s2), (n)))

/* Mark a function parameter as unused, to suppress nuisance compiler
** warnings. */
#ifndef UNUSED_PARAM
# define UNUSED_PARAM(X)  (void)(X)
#endif

#ifndef UNUSED_PARAM2
# define UNUSED_PARAM2(X, Y)  (void)(X), (void)(Y)
#endif

typedef struct Fts5Global Fts5Global;
typedef struct Fts5Colset Fts5Colset;

/* If a NEAR() clump or phrase may only match a specific set of columns, 
** then an object of the following type is used to record the set of columns.
** Each entry in the aiCol[] array is a column that may be matched.
**
** This object is used by fts5_expr.c and fts5_index.c.
*/
struct Fts5Colset {
  int nCol;
  int aiCol[1];
};



/**************************************************************************
** Interface to code in fts5_config.c. fts5_config.c contains contains code
** to parse the arguments passed to the CREATE VIRTUAL TABLE statement.
*/

typedef struct Fts5Config Fts5Config;

/*
** An instance of the following structure encodes all information that can
** be gleaned from the CREATE VIRTUAL TABLE statement.
**
** And all information loaded from the %_config table.
**
** nAutomerge:
**   The minimum number of segments that an auto-merge operation should
**   attempt to merge together. A value of 1 sets the object to use the 
**   compile time default. Zero disables auto-merge altogether.
**
** zContent:
**
** zContentRowid:
**   The value of the content_rowid= option, if one was specified. Or 
**   the string "rowid" otherwise. This text is not quoted - if it is
**   used as part of an SQL statement it needs to be quoted appropriately.
**
** zContentExprlist:
**
** pzErrmsg:
**   This exists in order to allow the fts5_index.c module to return a 
**   decent error message if it encounters a file-format version it does
**   not understand.
**
** bColumnsize:
**   True if the %_docsize table is created.
**
** bPrefixIndex:
**   This is only used for debugging. If set to false, any prefix indexes
**   are ignored. This value is configured using:
**
**       INSERT INTO tbl(tbl, rank) VALUES('prefix-index', $bPrefixIndex);
**
*/
struct Fts5Config {
  sqlite3 *db;                    /* Database handle */
  char *zDb;                      /* Database holding FTS index (e.g. "main") */
  char *zName;                    /* Name of FTS index */
  int nCol;                       /* Number of columns */
  char **azCol;                   /* Column names */
  u8 *abUnindexed;                /* True for unindexed columns */
  int nPrefix;                    /* Number of prefix indexes */
  int *aPrefix;                   /* Sizes in bytes of nPrefix prefix indexes */
  int eContent;                   /* An FTS5_CONTENT value */
  char *zContent;                 /* content table */ 
  char *zContentRowid;            /* "content_rowid=" option value */ 
  int bColumnsize;                /* "columnsize=" option value (dflt==1) */
  int eDetail;                    /* FTS5_DETAIL_XXX value */
  char *zContentExprlist;
  Fts5Tokenizer *pTok;
  fts5_tokenizer *pTokApi;
  int bLock;                      /* True when table is preparing statement */
  int ePattern;                   /* FTS_PATTERN_XXX constant */

  /* Values loaded from the %_config table */
  int iCookie;                    /* Incremented when %_config is modified */
  int pgsz;                       /* Approximate page size used in %_data */
  int nAutomerge;                 /* 'automerge' setting */
  int nCrisisMerge;               /* Maximum allowed segments per level */
  int nUsermerge;                 /* 'usermerge' setting */
  int nHashSize;                  /* Bytes of memory for in-memory hash */
  char *zRank;                    /* Name of rank function */
  char *zRankArgs;                /* Arguments to rank function */

  /* If non-NULL, points to sqlite3_vtab.base.zErrmsg. Often NULL. */
  char **pzErrmsg;

#ifdef SQLITE_DEBUG
  int bPrefixIndex;               /* True to use prefix-indexes */
#endif
};

/* Current expected value of %_config table 'version' field */
#define FTS5_CURRENT_VERSION  4

#define FTS5_CONTENT_NORMAL   0
#define FTS5_CONTENT_NONE     1
#define FTS5_CONTENT_EXTERNAL 2

#define FTS5_DETAIL_FULL      0
#define FTS5_DETAIL_NONE      1
#define FTS5_DETAIL_COLUMNS   2

#define FTS5_PATTERN_NONE     0
#define FTS5_PATTERN_LIKE     65  /* matches SQLITE_INDEX_CONSTRAINT_LIKE */
#define FTS5_PATTERN_GLOB     66  /* matches SQLITE_INDEX_CONSTRAINT_GLOB */

static int sqlite3Fts5ConfigParse(
    Fts5Global*, sqlite3*, int, const char **, Fts5Config**, char**
);
static void sqlite3Fts5ConfigFree(Fts5Config*);

static int sqlite3Fts5ConfigDeclareVtab(Fts5Config *pConfig);

static int sqlite3Fts5Tokenize(
  Fts5Config *pConfig,            /* FTS5 Configuration object */
  int flags,                      /* FTS5_TOKENIZE_* flags */
  const char *pText, int nText,   /* Text to tokenize */
  void *pCtx,                     /* Context passed to xToken() */
  int (*xToken)(void*, int, const char*, int, int, int)    /* Callback */
);

static void sqlite3Fts5Dequote(char *z);

/* Load the contents of the %_config table */
static int sqlite3Fts5ConfigLoad(Fts5Config*, int);

/* Set the value of a single config attribute */
static int sqlite3Fts5ConfigSetValue(Fts5Config*, const char*, sqlite3_value*, int*);

static int sqlite3Fts5ConfigParseRank(const char*, char**, char**);

/*
** End of interface to code in fts5_config.c.
**************************************************************************/

/**************************************************************************
** Interface to code in fts5_buffer.c.
*/

/*
** Buffer object for the incremental building of string data.
*/
typedef struct Fts5Buffer Fts5Buffer;
struct Fts5Buffer {
  u8 *p;
  int n;
  int nSpace;
};

static int sqlite3Fts5BufferSize(int*, Fts5Buffer*, u32);
static void sqlite3Fts5BufferAppendVarint(int*, Fts5Buffer*, i64);
static void sqlite3Fts5BufferAppendBlob(int*, Fts5Buffer*, u32, const u8*);
static void sqlite3Fts5BufferAppendString(int *, Fts5Buffer*, const char*);
static void sqlite3Fts5BufferFree(Fts5Buffer*);
static void sqlite3Fts5BufferZero(Fts5Buffer*);
static void sqlite3Fts5BufferSet(int*, Fts5Buffer*, int, const u8*);
static void sqlite3Fts5BufferAppendPrintf(int *, Fts5Buffer*, char *zFmt, ...);

static char *sqlite3Fts5Mprintf(int *pRc, const char *zFmt, ...);

#define fts5BufferZero(x)             sqlite3Fts5BufferZero(x)
#define fts5BufferAppendVarint(a,b,c) sqlite3Fts5BufferAppendVarint(a,b,(i64)c)
#define fts5BufferFree(a)             sqlite3Fts5BufferFree(a)
#define fts5BufferAppendBlob(a,b,c,d) sqlite3Fts5BufferAppendBlob(a,b,c,d)
#define fts5BufferSet(a,b,c,d)        sqlite3Fts5BufferSet(a,b,c,d)

#define fts5BufferGrow(pRc,pBuf,nn) ( \
  (u32)((pBuf)->n) + (u32)(nn) <= (u32)((pBuf)->nSpace) ? 0 : \
    sqlite3Fts5BufferSize((pRc),(pBuf),(nn)+(pBuf)->n) \
)

/* Write and decode big-endian 32-bit integer values */
static void sqlite3Fts5Put32(u8*, int);
static int sqlite3Fts5Get32(const u8*);

#define FTS5_POS2COLUMN(iPos) (int)(iPos >> 32)
#define FTS5_POS2OFFSET(iPos) (int)(iPos & 0x7FFFFFFF)

typedef struct Fts5PoslistReader Fts5PoslistReader;
struct Fts5PoslistReader {
  /* Variables used only by sqlite3Fts5PoslistIterXXX() functions. */
  const u8 *a;                    /* Position list to iterate through */
  int n;                          /* Size of buffer at a[] in bytes */
  int i;                          /* Current offset in a[] */

  u8 bFlag;                       /* For client use (any custom purpose) */

  /* Output variables */
  u8 bEof;                        /* Set to true at EOF */
  i64 iPos;                       /* (iCol<<32) + iPos */
};
static int sqlite3Fts5PoslistReaderInit(
  const u8 *a, int n,             /* Poslist buffer to iterate through */
  Fts5PoslistReader *pIter        /* Iterator object to initialize */
);
static int sqlite3Fts5PoslistReaderNext(Fts5PoslistReader*);

typedef struct Fts5PoslistWriter Fts5PoslistWriter;
struct Fts5PoslistWriter {
  i64 iPrev;
};
static int sqlite3Fts5PoslistWriterAppend(Fts5Buffer*, Fts5PoslistWriter*, i64);
static void sqlite3Fts5PoslistSafeAppend(Fts5Buffer*, i64*, i64);

static int sqlite3Fts5PoslistNext64(
  const u8 *a, int n,             /* Buffer containing poslist */
  int *pi,                        /* IN/OUT: Offset within a[] */
  i64 *piOff                      /* IN/OUT: Current offset */
);

/* Malloc utility */
static void *sqlite3Fts5MallocZero(int *pRc, sqlite3_int64 nByte);
static char *sqlite3Fts5Strndup(int *pRc, const char *pIn, int nIn);

/* Character set tests (like isspace(), isalpha() etc.) */
static int sqlite3Fts5IsBareword(char t);


/* Bucket of terms object used by the integrity-check in offsets=0 mode. */
typedef struct Fts5Termset Fts5Termset;
static int sqlite3Fts5TermsetNew(Fts5Termset**);
static int sqlite3Fts5TermsetAdd(Fts5Termset*, int, const char*, int, int *pbPresent);
static void sqlite3Fts5TermsetFree(Fts5Termset*);

/*
** End of interface to code in fts5_buffer.c.
**************************************************************************/

/**************************************************************************
** Interface to code in fts5_index.c. fts5_index.c contains contains code
** to access the data stored in the %_data table.
*/

typedef struct Fts5Index Fts5Index;
typedef struct Fts5IndexIter Fts5IndexIter;

struct Fts5IndexIter {
  i64 iRowid;
  const u8 *pData;
  int nData;
  u8 bEof;
};

#define sqlite3Fts5IterEof(x) ((x)->bEof)

/*
** Values used as part of the flags argument passed to IndexQuery().
*/
#define FTS5INDEX_QUERY_PREFIX     0x0001   /* Prefix query */
#define FTS5INDEX_QUERY_DESC       0x0002   /* Docs in descending rowid order */
#define FTS5INDEX_QUERY_TEST_NOIDX 0x0004   /* Do not use prefix index */
#define FTS5INDEX_QUERY_SCAN       0x0008   /* Scan query (fts5vocab) */

/* The following are used internally by the fts5_index.c module. They are
** defined here only to make it easier to avoid clashes with the flags
** above. */
#define FTS5INDEX_QUERY_SKIPEMPTY  0x0010
#define FTS5INDEX_QUERY_NOOUTPUT   0x0020

/*
** Create/destroy an Fts5Index object.
*/
static int sqlite3Fts5IndexOpen(Fts5Config *pConfig, int bCreate, Fts5Index**, char**);
static int sqlite3Fts5IndexClose(Fts5Index *p);

/*
** Return a simple checksum value based on the arguments.
*/
static u64 sqlite3Fts5IndexEntryCksum(
  i64 iRowid, 
  int iCol, 
  int iPos, 
  int iIdx,
  const char *pTerm,
  int nTerm
);

/*
** Argument p points to a buffer containing utf-8 text that is n bytes in 
** size. Return the number of bytes in the nChar character prefix of the
** buffer, or 0 if there are less than nChar characters in total.
*/
static int sqlite3Fts5IndexCharlenToBytelen(
  const char *p, 
  int nByte, 
  int nChar
);

/*
** Open a new iterator to iterate though all rowids that match the 
** specified token or token prefix.
*/
static int sqlite3Fts5IndexQuery(
  Fts5Index *p,                   /* FTS index to query */
  const char *pToken, int nToken, /* Token (or prefix) to query for */
  int flags,                      /* Mask of FTS5INDEX_QUERY_X flags */
  Fts5Colset *pColset,            /* Match these columns only */
  Fts5IndexIter **ppIter          /* OUT: New iterator object */
);

/*
** The various operations on open token or token prefix iterators opened
** using sqlite3Fts5IndexQuery().
*/
static int sqlite3Fts5IterNext(Fts5IndexIter*);
static int sqlite3Fts5IterNextFrom(Fts5IndexIter*, i64 iMatch);

/*
** Close an iterator opened by sqlite3Fts5IndexQuery().
*/
static void sqlite3Fts5IterClose(Fts5IndexIter*);

/*
** Close the reader blob handle, if it is open.
*/
static void sqlite3Fts5IndexCloseReader(Fts5Index*);

/*
** This interface is used by the fts5vocab module.
*/
static const char *sqlite3Fts5IterTerm(Fts5IndexIter*, int*);
static int sqlite3Fts5IterNextScan(Fts5IndexIter*);
static void *sqlite3Fts5StructureRef(Fts5Index*);
static void sqlite3Fts5StructureRelease(void*);
static int sqlite3Fts5StructureTest(Fts5Index*, void*);


/*
** Insert or remove data to or from the index. Each time a document is 
** added to or removed from the index, this function is called one or more
** times.
**
** For an insert, it must be called once for each token in the new document.
** If the operation is a delete, it must be called (at least) once for each
** unique token in the document with an iCol value less than zero. The iPos
** argument is ignored for a delete.
*/
static int sqlite3Fts5IndexWrite(
  Fts5Index *p,                   /* Index to write to */
  int iCol,                       /* Column token appears in (-ve -> delete) */
  int iPos,                       /* Position of token within column */
  const char *pToken, int nToken  /* Token to add or remove to or from index */
);

/*
** Indicate that subsequent calls to sqlite3Fts5IndexWrite() pertain to
** document iDocid.
*/
static int sqlite3Fts5IndexBeginWrite(
  Fts5Index *p,                   /* Index to write to */
  int bDelete,                    /* True if current operation is a delete */
  i64 iDocid                      /* Docid to add or remove data from */
);

/*
** Flush any data stored in the in-memory hash tables to the database.
** Also close any open blob handles.
*/
static int sqlite3Fts5IndexSync(Fts5Index *p);

/*
** Discard any data stored in the in-memory hash tables. Do not write it
** to the database. Additionally, assume that the contents of the %_data
** table may have changed on disk. So any in-memory caches of %_data 
** records must be invalidated.
*/
static int sqlite3Fts5IndexRollback(Fts5Index *p);

/*
** Get or set the "averages" values.
*/
static int sqlite3Fts5IndexGetAverages(Fts5Index *p, i64 *pnRow, i64 *anSize);
static int sqlite3Fts5IndexSetAverages(Fts5Index *p, const u8*, int);

/*
** Functions called by the storage module as part of integrity-check.
*/
static int sqlite3Fts5IndexIntegrityCheck(Fts5Index*, u64 cksum, int bUseCksum);

/* 
** Called during virtual module initialization to register UDF 
** fts5_decode() with SQLite 
*/
static int sqlite3Fts5IndexInit(sqlite3*);

static int sqlite3Fts5IndexSetCookie(Fts5Index*, int);

/*
** Return the total number of entries read from the %_data table by 
** this connection since it was created.
*/
static int sqlite3Fts5IndexReads(Fts5Index *p);

static int sqlite3Fts5IndexReinit(Fts5Index *p);
static int sqlite3Fts5IndexOptimize(Fts5Index *p);
static int sqlite3Fts5IndexMerge(Fts5Index *p, int nMerge);
static int sqlite3Fts5IndexReset(Fts5Index *p);

static int sqlite3Fts5IndexLoadConfig(Fts5Index *p);

/*
** End of interface to code in fts5_index.c.
**************************************************************************/

/**************************************************************************
** Interface to code in fts5_varint.c. 
*/
static int sqlite3Fts5GetVarint32(const unsigned char *p, u32 *v);
static int sqlite3Fts5GetVarintLen(u32 iVal);
static u8 sqlite3Fts5GetVarint(const unsigned char*, u64*);
static int sqlite3Fts5PutVarint(unsigned char *p, u64 v);

#define fts5GetVarint32(a,b) sqlite3Fts5GetVarint32(a,(u32*)&b)
#define fts5GetVarint    sqlite3Fts5GetVarint

#define fts5FastGetVarint32(a, iOff, nVal) {      \
  nVal = (a)[iOff++];                             \
  if( nVal & 0x80 ){                              \
    iOff--;                                       \
    iOff += fts5GetVarint32(&(a)[iOff], nVal);    \
  }                                               \
}


/*
** End of interface to code in fts5_varint.c.
**************************************************************************/


/**************************************************************************
** Interface to code in fts5_main.c. 
*/

/*
** Virtual-table object.
*/
typedef struct Fts5Table Fts5Table;
struct Fts5Table {
  sqlite3_vtab base;              /* Base class used by SQLite core */
  Fts5Config *pConfig;            /* Virtual table configuration */
  Fts5Index *pIndex;              /* Full-text index */
};

static int sqlite3Fts5GetTokenizer(
  Fts5Global*, 
  const char **azArg,
  int nArg,
  Fts5Config*,
  char **pzErr
);

static Fts5Table *sqlite3Fts5TableFromCsrid(Fts5Global*, i64);

static int sqlite3Fts5FlushToDisk(Fts5Table*);

/*
** End of interface to code in fts5.c.
**************************************************************************/

/**************************************************************************
** Interface to code in fts5_hash.c. 
*/
typedef struct Fts5Hash Fts5Hash;

/*
** Create a hash table, free a hash table.
*/
static int sqlite3Fts5HashNew(Fts5Config*, Fts5Hash**, int *pnSize);
static void sqlite3Fts5HashFree(Fts5Hash*);

static int sqlite3Fts5HashWrite(
  Fts5Hash*,
  i64 iRowid,                     /* Rowid for this entry */
  int iCol,                       /* Column token appears in (-ve -> delete) */
  int iPos,                       /* Position of token within column */
  char bByte,
  const char *pToken, int nToken  /* Token to add or remove to or from index */
);

/*
** Empty (but do not delete) a hash table.
*/
static void sqlite3Fts5HashClear(Fts5Hash*);

static int sqlite3Fts5HashQuery(
  Fts5Hash*,                      /* Hash table to query */
  int nPre,
  const char *pTerm, int nTerm,   /* Query term */
  void **ppObj,                   /* OUT: Pointer to doclist for pTerm */
  int *pnDoclist                  /* OUT: Size of doclist in bytes */
);

static int sqlite3Fts5HashScanInit(
  Fts5Hash*,                      /* Hash table to query */
  const char *pTerm, int nTerm    /* Query prefix */
);
static void sqlite3Fts5HashScanNext(Fts5Hash*);
static int sqlite3Fts5HashScanEof(Fts5Hash*);
static void sqlite3Fts5HashScanEntry(Fts5Hash *,
  const char **pzTerm,            /* OUT: term (nul-terminated) */
  const u8 **ppDoclist,           /* OUT: pointer to doclist */
  int *pnDoclist                  /* OUT: size of doclist in bytes */
);


/*
** End of interface to code in fts5_hash.c.
**************************************************************************/

/**************************************************************************
** Interface to code in fts5_storage.c. fts5_storage.c contains contains 
** code to access the data stored in the %_content and %_docsize tables.
*/

#define FTS5_STMT_SCAN_ASC  0     /* SELECT rowid, * FROM ... ORDER BY 1 ASC */
#define FTS5_STMT_SCAN_DESC 1     /* SELECT rowid, * FROM ... ORDER BY 1 DESC */
#define FTS5_STMT_LOOKUP    2     /* SELECT rowid, * FROM ... WHERE rowid=? */

typedef struct Fts5Storage Fts5Storage;

static int sqlite3Fts5StorageOpen(Fts5Config*, Fts5Index*, int, Fts5Storage**, char**);
static int sqlite3Fts5StorageClose(Fts5Storage *p);
static int sqlite3Fts5StorageRename(Fts5Storage*, const char *zName);

static int sqlite3Fts5DropAll(Fts5Config*);
static int sqlite3Fts5CreateTable(Fts5Config*, const char*, const char*, int, char **);

static int sqlite3Fts5StorageDelete(Fts5Storage *p, i64, sqlite3_value**);
static int sqlite3Fts5StorageContentInsert(Fts5Storage *p, sqlite3_value**, i64*);
static int sqlite3Fts5StorageIndexInsert(Fts5Storage *p, sqlite3_value**, i64);

static int sqlite3Fts5StorageIntegrity(Fts5Storage *p, int iArg);

static int sqlite3Fts5StorageStmt(Fts5Storage *p, int eStmt, sqlite3_stmt**, char**);
static void sqlite3Fts5StorageStmtRelease(Fts5Storage *p, int eStmt, sqlite3_stmt*);

static int sqlite3Fts5StorageDocsize(Fts5Storage *p, i64 iRowid, int *aCol);
static int sqlite3Fts5StorageSize(Fts5Storage *p, int iCol, i64 *pnAvg);
static int sqlite3Fts5StorageRowCount(Fts5Storage *p, i64 *pnRow);

static int sqlite3Fts5StorageSync(Fts5Storage *p);
static int sqlite3Fts5StorageRollback(Fts5Storage *p);

static int sqlite3Fts5StorageConfigValue(
    Fts5Storage *p, const char*, sqlite3_value*, int
);

static int sqlite3Fts5StorageDeleteAll(Fts5Storage *p);
static int sqlite3Fts5StorageRebuild(Fts5Storage *p);
static int sqlite3Fts5StorageOptimize(Fts5Storage *p);
static int sqlite3Fts5StorageMerge(Fts5Storage *p, int nMerge);
static int sqlite3Fts5StorageReset(Fts5Storage *p);

/*
** End of interface to code in fts5_storage.c.
**************************************************************************/


/**************************************************************************
** Interface to code in fts5_expr.c. 
*/
typedef struct Fts5Expr Fts5Expr;
typedef struct Fts5ExprNode Fts5ExprNode;
typedef struct Fts5Parse Fts5Parse;
typedef struct Fts5Token Fts5Token;
typedef struct Fts5ExprPhrase Fts5ExprPhrase;
typedef struct Fts5ExprNearset Fts5ExprNearset;

struct Fts5Token {
  const char *p;                  /* Token text (not NULL terminated) */
  int n;                          /* Size of buffer p in bytes */
};

/* Parse a MATCH expression. */
static int sqlite3Fts5ExprNew(
  Fts5Config *pConfig, 
  int bPhraseToAnd,
  int iCol,                       /* Column on LHS of MATCH operator */
  const char *zExpr,
  Fts5Expr **ppNew, 
  char **pzErr
);
static int sqlite3Fts5ExprPattern(
  Fts5Config *pConfig, 
  int bGlob, 
  int iCol, 
  const char *zText, 
  Fts5Expr **pp
);

/*
** for(rc = sqlite3Fts5ExprFirst(pExpr, pIdx, bDesc);
**     rc==SQLITE_OK && 0==sqlite3Fts5ExprEof(pExpr);
**     rc = sqlite3Fts5ExprNext(pExpr)
** ){
**   // The document with rowid iRowid matches the expression!
**   i64 iRowid = sqlite3Fts5ExprRowid(pExpr);
** }
*/
static int sqlite3Fts5ExprFirst(Fts5Expr*, Fts5Index *pIdx, i64 iMin, int bDesc);
static int sqlite3Fts5ExprNext(Fts5Expr*, i64 iMax);
static int sqlite3Fts5ExprEof(Fts5Expr*);
static i64 sqlite3Fts5ExprRowid(Fts5Expr*);

static void sqlite3Fts5ExprFree(Fts5Expr*);
static int sqlite3Fts5ExprAnd(Fts5Expr **pp1, Fts5Expr *p2);

/* Called during startup to register a UDF with SQLite */
static int sqlite3Fts5ExprInit(Fts5Global*, sqlite3*);

static int sqlite3Fts5ExprPhraseCount(Fts5Expr*);
static int sqlite3Fts5ExprPhraseSize(Fts5Expr*, int iPhrase);
static int sqlite3Fts5ExprPoslist(Fts5Expr*, int, const u8 **);

typedef struct Fts5PoslistPopulator Fts5PoslistPopulator;
static Fts5PoslistPopulator *sqlite3Fts5ExprClearPoslists(Fts5Expr*, int);
static int sqlite3Fts5ExprPopulatePoslists(
    Fts5Config*, Fts5Expr*, Fts5PoslistPopulator*, int, const char*, int
);
static void sqlite3Fts5ExprCheckPoslists(Fts5Expr*, i64);

static int sqlite3Fts5ExprClonePhrase(Fts5Expr*, int, Fts5Expr**);

static int sqlite3Fts5ExprPhraseCollist(Fts5Expr *, int, const u8 **, int *);

/*******************************************
** The fts5_expr.c API above this point is used by the other hand-written
** C code in this module. The interfaces below this point are called by
** the parser code in fts5parse.y.  */

static void sqlite3Fts5ParseError(Fts5Parse *pParse, const char *zFmt, ...);

static Fts5ExprNode *sqlite3Fts5ParseNode(
  Fts5Parse *pParse,
  int eType,
  Fts5ExprNode *pLeft,
  Fts5ExprNode *pRight,
  Fts5ExprNearset *pNear
);

static Fts5ExprNode *sqlite3Fts5ParseImplicitAnd(
  Fts5Parse *pParse,
  Fts5ExprNode *pLeft,
  Fts5ExprNode *pRight
);

static Fts5ExprPhrase *sqlite3Fts5ParseTerm(
  Fts5Parse *pParse, 
  Fts5ExprPhrase *pPhrase, 
  Fts5Token *pToken,
  int bPrefix
);

static void sqlite3Fts5ParseSetCaret(Fts5ExprPhrase*);

static Fts5ExprNearset *sqlite3Fts5ParseNearset(
  Fts5Parse*, 
  Fts5ExprNearset*,
  Fts5ExprPhrase* 
);

static Fts5Colset *sqlite3Fts5ParseColset(
  Fts5Parse*, 
  Fts5Colset*, 
  Fts5Token *
);

static void sqlite3Fts5ParsePhraseFree(Fts5ExprPhrase*);
static void sqlite3Fts5ParseNearsetFree(Fts5ExprNearset*);
static void sqlite3Fts5ParseNodeFree(Fts5ExprNode*);

static void sqlite3Fts5ParseSetDistance(Fts5Parse*, Fts5ExprNearset*, Fts5Token*);
static void sqlite3Fts5ParseSetColset(Fts5Parse*, Fts5ExprNode*, Fts5Colset*);
static Fts5Colset *sqlite3Fts5ParseColsetInvert(Fts5Parse*, Fts5Colset*);
static void sqlite3Fts5ParseFinished(Fts5Parse *pParse, Fts5ExprNode *p);
static void sqlite3Fts5ParseNear(Fts5Parse *pParse, Fts5Token*);

/*
** End of interface to code in fts5_expr.c.
**************************************************************************/



/**************************************************************************
** Interface to code in fts5_aux.c. 
*/

static int sqlite3Fts5AuxInit(fts5_api*);
/*
** End of interface to code in fts5_aux.c.
**************************************************************************/

/**************************************************************************
** Interface to code in fts5_tokenizer.c. 
*/

static int sqlite3Fts5TokenizerInit(fts5_api*);
static int sqlite3Fts5TokenizerPattern(
    int (*xCreate)(void*, const char**, int, Fts5Tokenizer**),
    Fts5Tokenizer *pTok
);
/*
** End of interface to code in fts5_tokenizer.c.
**************************************************************************/

/**************************************************************************
** Interface to code in fts5_vocab.c. 
*/

static int sqlite3Fts5VocabInit(Fts5Global*, sqlite3*);

/*
** End of interface to code in fts5_vocab.c.
**************************************************************************/


/**************************************************************************
** Interface to automatically generated code in fts5_unicode2.c. 
*/
static int sqlite3Fts5UnicodeIsdiacritic(int c);
static int sqlite3Fts5UnicodeFold(int c, int bRemoveDiacritic);

static int sqlite3Fts5UnicodeCatParse(const char*, u8*);
static int sqlite3Fts5UnicodeCategory(u32 iCode);
static void sqlite3Fts5UnicodeAscii(u8*, u8*);
/*
** End of interface to code in fts5_unicode2.c.
**************************************************************************/

#endif

#line 1 "fts5parse.h"
#define FTS5_OR                               1
#define FTS5_AND                              2
#define FTS5_NOT                              3
#define FTS5_TERM                             4
#define FTS5_COLON                            5
#define FTS5_MINUS                            6
#define FTS5_LCP                              7
#define FTS5_RCP                              8
#define FTS5_STRING                           9
#define FTS5_LP                              10
#define FTS5_RP                              11
#define FTS5_CARET                           12
#define FTS5_COMMA                           13
#define FTS5_PLUS                            14
#define FTS5_STAR                            15

#line 1 "fts5parse.c"
/* This file is automatically generated by Lemon from input grammar
** source file "fts5parse.y". */
/*
** 2000-05-29
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** Driver template for the LEMON parser generator.
**
** The "lemon" program processes an LALR(1) input grammar file, then uses
** this template to construct a parser.  The "lemon" program inserts text
** at each "%%" line.  Also, any "P-a-r-s-e" identifer prefix (without the
** interstitial "-" characters) contained in this template is changed into
** the value of the %name directive from the grammar.  Otherwise, the content
** of this template is copied straight through into the generate parser
** source file.
**
** The following is the concatenation of all %include directives from the
** input grammar file:
*/
/************ Begin %include sections from the grammar ************************/
#line 47 "fts5parse.y"

/* #include "fts5Int.h" */
/* #include "fts5parse.h" */

/*
** Disable all error recovery processing in the parser push-down
** automaton.
*/
#define fts5YYNOERRORRECOVERY 1

/*
** Make fts5yytestcase() the same as testcase()
*/
#define fts5yytestcase(X) testcase(X)

/*
** Indicate that sqlite3ParserFree() will never be called with a null
** pointer.
*/
#define fts5YYPARSEFREENOTNULL 1

/*
** Alternative datatype for the argument to the malloc() routine passed
** into sqlite3ParserAlloc().  The default is size_t.
*/
#define fts5YYMALLOCARGTYPE  u64

#line 57 "fts5parse.c"
/**************** End of %include directives **********************************/
/* These constants specify the various numeric values for terminal symbols.
***************** Begin token definitions *************************************/
#ifndef FTS5_OR
#define FTS5_OR                              1
#define FTS5_AND                             2
#define FTS5_NOT                             3
#define FTS5_TERM                            4
#define FTS5_COLON                           5
#define FTS5_MINUS                           6
#define FTS5_LCP                             7
#define FTS5_RCP                             8
#define FTS5_STRING                          9
#define FTS5_LP                             10
#define FTS5_RP                             11
#define FTS5_CARET                          12
#define FTS5_COMMA                          13
#define FTS5_PLUS                           14
#define FTS5_STAR                           15
#endif
/**************** End token definitions ***************************************/

/* The next sections is a series of control #defines.
** various aspects of the generated parser.
**    fts5YYCODETYPE         is the data type used to store the integer codes
**                       that represent terminal and non-terminal symbols.
**                       "unsigned char" is used if there are fewer than
**                       256 symbols.  Larger types otherwise.
**    fts5YYNOCODE           is a number of type fts5YYCODETYPE that is not used for
**                       any terminal or nonterminal symbol.
**    fts5YYFALLBACK         If defined, this indicates that one or more tokens
**                       (also known as: "terminal symbols") have fall-back
**                       values which should be used if the original symbol
**                       would not parse.  This permits keywords to sometimes
**                       be used as identifiers, for example.
**    fts5YYACTIONTYPE       is the data type used for "action codes" - numbers
**                       that indicate what to do in response to the next
**                       token.
**    sqlite3Fts5ParserFTS5TOKENTYPE     is the data type used for minor type for terminal
**                       symbols.  Background: A "minor type" is a semantic
**                       value associated with a terminal or non-terminal
**                       symbols.  For example, for an "ID" terminal symbol,
**                       the minor type might be the name of the identifier.
**                       Each non-terminal can have a different minor type.
**                       Terminal symbols all have the same minor type, though.
**                       This macros defines the minor type for terminal 
**                       symbols.
**    fts5YYMINORTYPE        is the data type used for all minor types.
**                       This is typically a union of many types, one of
**                       which is sqlite3Fts5ParserFTS5TOKENTYPE.  The entry in the union
**                       for terminal symbols is called "fts5yy0".
**    fts5YYSTACKDEPTH       is the maximum depth of the parser's stack.  If
**                       zero the stack is dynamically sized using realloc()
**    sqlite3Fts5ParserARG_SDECL     A static variable declaration for the %extra_argument
**    sqlite3Fts5ParserARG_PDECL     A parameter declaration for the %extra_argument
**    sqlite3Fts5ParserARG_PARAM     Code to pass %extra_argument as a subroutine parameter
**    sqlite3Fts5ParserARG_STORE     Code to store %extra_argument into fts5yypParser
**    sqlite3Fts5ParserARG_FETCH     Code to extract %extra_argument from fts5yypParser
**    sqlite3Fts5ParserCTX_*         As sqlite3Fts5ParserARG_ except for %extra_context
**    fts5YYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
**    fts5YYNSTATE           the combined number of states.
**    fts5YYNRULE            the number of rules in the grammar
**    fts5YYNFTS5TOKEN           Number of terminal symbols
**    fts5YY_MAX_SHIFT       Maximum value for shift actions
**    fts5YY_MIN_SHIFTREDUCE Minimum value for shift-reduce actions
**    fts5YY_MAX_SHIFTREDUCE Maximum value for shift-reduce actions
**    fts5YY_ERROR_ACTION    The fts5yy_action[] code for syntax error
**    fts5YY_ACCEPT_ACTION   The fts5yy_action[] code for accept
**    fts5YY_NO_ACTION       The fts5yy_action[] code for no-op
**    fts5YY_MIN_REDUCE      Minimum value for reduce actions
**    fts5YY_MAX_REDUCE      Maximum value for reduce actions
*/
#ifndef INTERFACE
# define INTERFACE 1
#endif
/************* Begin control #defines *****************************************/
#define fts5YYCODETYPE unsigned char
#define fts5YYNOCODE 27
#define fts5YYACTIONTYPE unsigned char
#define sqlite3Fts5ParserFTS5TOKENTYPE Fts5Token
typedef union {
  int fts5yyinit;
  sqlite3Fts5ParserFTS5TOKENTYPE fts5yy0;
  int fts5yy4;
  Fts5Colset* fts5yy11;
  Fts5ExprNode* fts5yy24;
  Fts5ExprNearset* fts5yy46;
  Fts5ExprPhrase* fts5yy53;
} fts5YYMINORTYPE;
#ifndef fts5YYSTACKDEPTH
#define fts5YYSTACKDEPTH 100
#endif
#define sqlite3Fts5ParserARG_SDECL Fts5Parse *pParse;
#define sqlite3Fts5ParserARG_PDECL ,Fts5Parse *pParse
#define sqlite3Fts5ParserARG_PARAM ,pParse
#define sqlite3Fts5ParserARG_FETCH Fts5Parse *pParse=fts5yypParser->pParse;
#define sqlite3Fts5ParserARG_STORE fts5yypParser->pParse=pParse;
#define sqlite3Fts5ParserCTX_SDECL
#define sqlite3Fts5ParserCTX_PDECL
#define sqlite3Fts5ParserCTX_PARAM
#define sqlite3Fts5ParserCTX_FETCH
#define sqlite3Fts5ParserCTX_STORE
#define fts5YYNSTATE             35
#define fts5YYNRULE              28
#define fts5YYNRULE_WITH_ACTION  28
#define fts5YYNFTS5TOKEN             16
#define fts5YY_MAX_SHIFT         34
#define fts5YY_MIN_SHIFTREDUCE   52
#define fts5YY_MAX_SHIFTREDUCE   79
#define fts5YY_ERROR_ACTION      80
#define fts5YY_ACCEPT_ACTION     81
#define fts5YY_NO_ACTION         82
#define fts5YY_MIN_REDUCE        83
#define fts5YY_MAX_REDUCE        110
/************* End control #defines *******************************************/
#define fts5YY_NLOOKAHEAD ((int)(sizeof(fts5yy_lookahead)/sizeof(fts5yy_lookahead[0])))

/* Define the fts5yytestcase() macro to be a no-op if is not already defined
** otherwise.
**
** Applications can choose to define fts5yytestcase() in the %include section
** to a macro that can assist in verifying code coverage.  For production
** code the fts5yytestcase() macro should be turned off.  But it is useful
** for testing.
*/
#ifndef fts5yytestcase
# define fts5yytestcase(X)
#endif


/* Next are the tables used to determine what action to take based on the
** current state and lookahead token.  These tables are used to implement
** functions that take a state number and lookahead value and return an
** action integer.  
**
** Suppose the action integer is N.  Then the action is determined as
** follows
**
**   0 <= N <= fts5YY_MAX_SHIFT             Shift N.  That is, push the lookahead
**                                      token onto the stack and goto state N.
**
**   N between fts5YY_MIN_SHIFTREDUCE       Shift to an arbitrary state then
**     and fts5YY_MAX_SHIFTREDUCE           reduce by rule N-fts5YY_MIN_SHIFTREDUCE.
**
**   N == fts5YY_ERROR_ACTION               A syntax error has occurred.
**
**   N == fts5YY_ACCEPT_ACTION              The parser accepts its input.
**
**   N == fts5YY_NO_ACTION                  No such action.  Denotes unused
**                                      slots in the fts5yy_action[] table.
**
**   N between fts5YY_MIN_REDUCE            Reduce by rule N-fts5YY_MIN_REDUCE
**     and fts5YY_MAX_REDUCE
**
** The action table is constructed as a single large table named fts5yy_action[].
** Given state S and lookahead X, the action is computed as either:
**
**    (A)   N = fts5yy_action[ fts5yy_shift_ofst[S] + X ]
**    (B)   N = fts5yy_default[S]
**
** The (A) formula is preferred.  The B formula is used instead if
** fts5yy_lookahead[fts5yy_shift_ofst[S]+X] is not equal to X.
**
** The formulas above are for computing the action when the lookahead is
** a terminal symbol.  If the lookahead is a non-terminal (as occurs after
** a reduce action) then the fts5yy_reduce_ofst[] array is used in place of
** the fts5yy_shift_ofst[] array.
**
** The following are the tables generated in this section:
**
**  fts5yy_action[]        A single table containing all actions.
**  fts5yy_lookahead[]     A table containing the lookahead for each entry in
**                     fts5yy_action.  Used to detect hash collisions.
**  fts5yy_shift_ofst[]    For each state, the offset into fts5yy_action for
**                     shifting terminals.
**  fts5yy_reduce_ofst[]   For each state, the offset into fts5yy_action for
**                     shifting non-terminals after a reduce.
**  fts5yy_default[]       Default action for each state.
**
*********** Begin parsing tables **********************************************/
#define fts5YY_ACTTAB_COUNT (105)
static const fts5YYACTIONTYPE fts5yy_action[] = {
 /*     0 */    81,   20,   96,    6,   28,   99,   98,   26,   26,   18,
 /*    10 */    96,    6,   28,   17,   98,   56,   26,   19,   96,    6,
 /*    20 */    28,   14,   98,   14,   26,   31,   92,   96,    6,   28,
 /*    30 */   108,   98,   25,   26,   21,   96,    6,   28,   78,   98,
 /*    40 */    58,   26,   29,   96,    6,   28,  107,   98,   22,   26,
 /*    50 */    24,   16,   12,   11,    1,   13,   13,   24,   16,   23,
 /*    60 */    11,   33,   34,   13,   97,    8,   27,   32,   98,    7,
 /*    70 */    26,    3,    4,    5,    3,    4,    5,    3,   83,    4,
 /*    80 */     5,    3,   63,    5,    3,   62,   12,    2,   86,   13,
 /*    90 */     9,   30,   10,   10,   54,   57,   75,   78,   78,   53,
 /*   100 */    57,   15,   82,   82,   71,
};
static const fts5YYCODETYPE fts5yy_lookahead[] = {
 /*     0 */    16,   17,   18,   19,   20,   22,   22,   24,   24,   17,
 /*    10 */    18,   19,   20,    7,   22,    9,   24,   17,   18,   19,
 /*    20 */    20,    9,   22,    9,   24,   13,   17,   18,   19,   20,
 /*    30 */    26,   22,   24,   24,   17,   18,   19,   20,   15,   22,
 /*    40 */     9,   24,   17,   18,   19,   20,   26,   22,   21,   24,
 /*    50 */     6,    7,    9,    9,   10,   12,   12,    6,    7,   21,
 /*    60 */     9,   24,   25,   12,   18,    5,   20,   14,   22,    5,
 /*    70 */    24,    3,    1,    2,    3,    1,    2,    3,    0,    1,
 /*    80 */     2,    3,   11,    2,    3,   11,    9,   10,    5,   12,
 /*    90 */    23,   24,   10,   10,    8,    9,    9,   15,   15,    8,
 /*   100 */     9,    9,   27,   27,   11,   27,   27,   27,   27,   27,
 /*   110 */    27,   27,   27,   27,   27,   27,   27,   27,   27,   27,
 /*   120 */    27,
};
#define fts5YY_SHIFT_COUNT    (34)
#define fts5YY_SHIFT_MIN      (0)
#define fts5YY_SHIFT_MAX      (93)
static const unsigned char fts5yy_shift_ofst[] = {
 /*     0 */    44,   44,   44,   44,   44,   44,   51,   77,   43,   12,
 /*    10 */    14,   83,   82,   14,   23,   23,   31,   31,   71,   74,
 /*    20 */    78,   81,   86,   91,    6,   53,   53,   60,   64,   68,
 /*    30 */    53,   87,   92,   53,   93,
};
#define fts5YY_REDUCE_COUNT (17)
#define fts5YY_REDUCE_MIN   (-17)
#define fts5YY_REDUCE_MAX   (67)
static const signed char fts5yy_reduce_ofst[] = {
 /*     0 */   -16,   -8,    0,    9,   17,   25,   46,  -17,  -17,   37,
 /*    10 */    67,    4,    4,    8,    4,   20,   27,   38,
};
static const fts5YYACTIONTYPE fts5yy_default[] = {
 /*     0 */    80,   80,   80,   80,   80,   80,   95,   80,   80,  105,
 /*    10 */    80,  110,  110,   80,  110,  110,   80,   80,   80,   80,
 /*    20 */    80,   91,   80,   80,   80,  101,  100,   80,   80,   90,
 /*    30 */   103,   80,   80,  104,   80,
};
/********** End of lemon-generated parsing tables *****************************/

/* The next table maps tokens (terminal symbols) into fallback tokens.  
** If a construct like the following:
** 
**      %fallback ID X Y Z.
**
** appears in the grammar, then ID becomes a fallback token for X, Y,
** and Z.  Whenever one of the tokens X, Y, or Z is input to the parser
** but it does not parse, the type of the token is changed to ID and
** the parse is retried before an error is thrown.
**
** This feature can be used, for example, to cause some keywords in a language
** to revert to identifiers if they keyword does not apply in the context where
** it appears.
*/
#ifdef fts5YYFALLBACK
static const fts5YYCODETYPE fts5yyFallback[] = {
};
#endif /* fts5YYFALLBACK */

/* The following structure represents a single element of the
** parser's stack.  Information stored includes:
**
**   +  The state number for the parser at this level of the stack.
**
**   +  The value of the token stored at this level of the stack.
**      (In other words, the "major" token.)
**
**   +  The semantic value stored at this level of the stack.  This is
**      the information used by the action routines in the grammar.
**      It is sometimes called the "minor" token.
**
** After the "shift" half of a SHIFTREDUCE action, the stateno field
** actually contains the reduce action for the second half of the
** SHIFTREDUCE.
*/
struct fts5yyStackEntry {
  fts5YYACTIONTYPE stateno;  /* The state-number, or reduce action in SHIFTREDUCE */
  fts5YYCODETYPE major;      /* The major token value.  This is the code
                         ** number for the token at this stack level */
  fts5YYMINORTYPE minor;     /* The user-supplied minor token value.  This
                         ** is the value of the token  */
};
typedef struct fts5yyStackEntry fts5yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct fts5yyParser {
  fts5yyStackEntry *fts5yytos;          /* Pointer to top element of the stack */
#ifdef fts5YYTRACKMAXSTACKDEPTH
  int fts5yyhwm;                    /* High-water mark of the stack */
#endif
#ifndef fts5YYNOERRORRECOVERY
  int fts5yyerrcnt;                 /* Shifts left before out of the error */
#endif
  sqlite3Fts5ParserARG_SDECL                /* A place to hold %extra_argument */
  sqlite3Fts5ParserCTX_SDECL                /* A place to hold %extra_context */
#if fts5YYSTACKDEPTH<=0
  int fts5yystksz;                  /* Current side of the stack */
  fts5yyStackEntry *fts5yystack;        /* The parser's stack */
  fts5yyStackEntry fts5yystk0;          /* First stack entry */
#else
  fts5yyStackEntry fts5yystack[fts5YYSTACKDEPTH];  /* The parser's stack */
  fts5yyStackEntry *fts5yystackEnd;            /* Last entry in the stack */
#endif
};
typedef struct fts5yyParser fts5yyParser;

#include "libc/assert.h"
#ifndef NDEBUG
#include "libc/stdio/stdio.h"
static FILE *fts5yyTraceFILE = 0;
static char *fts5yyTracePrompt = 0;
#endif /* NDEBUG */

#ifndef NDEBUG
/* 
** Turn parser tracing on by giving a stream to which to write the trace
** and a prompt to preface each trace message.  Tracing is turned off
** by making either argument NULL 
**
** Inputs:
** <ul>
** <li> A FILE* to which trace output should be written.
**      If NULL, then tracing is turned off.
** <li> A prefix string written at the beginning of every
**      line of trace output.  If NULL, then tracing is
**      turned off.
** </ul>
**
** Outputs:
** None.
*/
static void sqlite3Fts5ParserTrace(FILE *TraceFILE, char *zTracePrompt){
  fts5yyTraceFILE = TraceFILE;
  fts5yyTracePrompt = zTracePrompt;
  if( fts5yyTraceFILE==0 ) fts5yyTracePrompt = 0;
  else if( fts5yyTracePrompt==0 ) fts5yyTraceFILE = 0;
}
#endif /* NDEBUG */

#if defined(fts5YYCOVERAGE) || !defined(NDEBUG)
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char *const fts5yyTokenName[] = { 
  /*    0 */ "$",
  /*    1 */ "OR",
  /*    2 */ "AND",
  /*    3 */ "NOT",
  /*    4 */ "TERM",
  /*    5 */ "COLON",
  /*    6 */ "MINUS",
  /*    7 */ "LCP",
  /*    8 */ "RCP",
  /*    9 */ "STRING",
  /*   10 */ "LP",
  /*   11 */ "RP",
  /*   12 */ "CARET",
  /*   13 */ "COMMA",
  /*   14 */ "PLUS",
  /*   15 */ "STAR",
  /*   16 */ "input",
  /*   17 */ "expr",
  /*   18 */ "cnearset",
  /*   19 */ "exprlist",
  /*   20 */ "colset",
  /*   21 */ "colsetlist",
  /*   22 */ "nearset",
  /*   23 */ "nearphrases",
  /*   24 */ "phrase",
  /*   25 */ "neardist_opt",
  /*   26 */ "star_opt",
};
#endif /* defined(fts5YYCOVERAGE) || !defined(NDEBUG) */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const fts5yyRuleName[] = {
 /*   0 */ "input ::= expr",
 /*   1 */ "colset ::= MINUS LCP colsetlist RCP",
 /*   2 */ "colset ::= LCP colsetlist RCP",
 /*   3 */ "colset ::= STRING",
 /*   4 */ "colset ::= MINUS STRING",
 /*   5 */ "colsetlist ::= colsetlist STRING",
 /*   6 */ "colsetlist ::= STRING",
 /*   7 */ "expr ::= expr AND expr",
 /*   8 */ "expr ::= expr OR expr",
 /*   9 */ "expr ::= expr NOT expr",
 /*  10 */ "expr ::= colset COLON LP expr RP",
 /*  11 */ "expr ::= LP expr RP",
 /*  12 */ "expr ::= exprlist",
 /*  13 */ "exprlist ::= cnearset",
 /*  14 */ "exprlist ::= exprlist cnearset",
 /*  15 */ "cnearset ::= nearset",
 /*  16 */ "cnearset ::= colset COLON nearset",
 /*  17 */ "nearset ::= phrase",
 /*  18 */ "nearset ::= CARET phrase",
 /*  19 */ "nearset ::= STRING LP nearphrases neardist_opt RP",
 /*  20 */ "nearphrases ::= phrase",
 /*  21 */ "nearphrases ::= nearphrases phrase",
 /*  22 */ "neardist_opt ::=",
 /*  23 */ "neardist_opt ::= COMMA STRING",
 /*  24 */ "phrase ::= phrase PLUS STRING star_opt",
 /*  25 */ "phrase ::= STRING star_opt",
 /*  26 */ "star_opt ::= STAR",
 /*  27 */ "star_opt ::=",
};
#endif /* NDEBUG */


#if fts5YYSTACKDEPTH<=0
/*
** Try to increase the size of the parser stack.  Return the number
** of errors.  Return 0 on success.
*/
static int fts5yyGrowStack(fts5yyParser *p){
  int newSize;
  int idx;
  fts5yyStackEntry *pNew;

  newSize = p->fts5yystksz*2 + 100;
  idx = p->fts5yytos ? (int)(p->fts5yytos - p->fts5yystack) : 0;
  if( p->fts5yystack==&p->fts5yystk0 ){
    pNew = malloc(newSize*sizeof(pNew[0]));
    if( pNew ) pNew[0] = p->fts5yystk0;
  }else{
    pNew = realloc(p->fts5yystack, newSize*sizeof(pNew[0]));
  }
  if( pNew ){
    p->fts5yystack = pNew;
    p->fts5yytos = &p->fts5yystack[idx];
#ifndef NDEBUG
    if( fts5yyTraceFILE ){
      fprintf(fts5yyTraceFILE,"%sStack grows from %d to %d entries.\n",
              fts5yyTracePrompt, p->fts5yystksz, newSize);
    }
#endif
    p->fts5yystksz = newSize;
  }
  return pNew==0; 
}
#endif

/* Datatype of the argument to the memory allocated passed as the
** second argument to sqlite3Fts5ParserAlloc() below.  This can be changed by
** putting an appropriate #define in the %include section of the input
** grammar.
*/
#ifndef fts5YYMALLOCARGTYPE
# define fts5YYMALLOCARGTYPE size_t
#endif

/* Initialize a new parser that has already been allocated.
*/
static void sqlite3Fts5ParserInit(void *fts5yypRawParser sqlite3Fts5ParserCTX_PDECL){
  fts5yyParser *fts5yypParser = (fts5yyParser*)fts5yypRawParser;
  sqlite3Fts5ParserCTX_STORE
#ifdef fts5YYTRACKMAXSTACKDEPTH
  fts5yypParser->fts5yyhwm = 0;
#endif
#if fts5YYSTACKDEPTH<=0
  fts5yypParser->fts5yytos = NULL;
  fts5yypParser->fts5yystack = NULL;
  fts5yypParser->fts5yystksz = 0;
  if( fts5yyGrowStack(fts5yypParser) ){
    fts5yypParser->fts5yystack = &fts5yypParser->fts5yystk0;
    fts5yypParser->fts5yystksz = 1;
  }
#endif
#ifndef fts5YYNOERRORRECOVERY
  fts5yypParser->fts5yyerrcnt = -1;
#endif
  fts5yypParser->fts5yytos = fts5yypParser->fts5yystack;
  fts5yypParser->fts5yystack[0].stateno = 0;
  fts5yypParser->fts5yystack[0].major = 0;
#if fts5YYSTACKDEPTH>0
  fts5yypParser->fts5yystackEnd = &fts5yypParser->fts5yystack[fts5YYSTACKDEPTH-1];
#endif
}

#ifndef sqlite3Fts5Parser_ENGINEALWAYSONSTACK
/* 
** This function allocates a new parser.
** The only argument is a pointer to a function which works like
** malloc.
**
** Inputs:
** A pointer to the function used to allocate memory.
**
** Outputs:
** A pointer to a parser.  This pointer is used in subsequent calls
** to sqlite3Fts5Parser and sqlite3Fts5ParserFree.
*/
static void *sqlite3Fts5ParserAlloc(void *(*mallocProc)(fts5YYMALLOCARGTYPE) sqlite3Fts5ParserCTX_PDECL){
  fts5yyParser *fts5yypParser;
  fts5yypParser = (fts5yyParser*)(*mallocProc)( (fts5YYMALLOCARGTYPE)sizeof(fts5yyParser) );
  if( fts5yypParser ){
    sqlite3Fts5ParserCTX_STORE
    sqlite3Fts5ParserInit(fts5yypParser sqlite3Fts5ParserCTX_PARAM);
  }
  return (void*)fts5yypParser;
}
#endif /* sqlite3Fts5Parser_ENGINEALWAYSONSTACK */


/* The following function deletes the "minor type" or semantic value
** associated with a symbol.  The symbol can be either a terminal
** or nonterminal. "fts5yymajor" is the symbol code, and "fts5yypminor" is
** a pointer to the value to be deleted.  The code used to do the 
** deletions is derived from the %destructor and/or %token_destructor
** directives of the input grammar.
*/
static void fts5yy_destructor(
  fts5yyParser *fts5yypParser,    /* The parser */
  fts5YYCODETYPE fts5yymajor,     /* Type code for object to destroy */
  fts5YYMINORTYPE *fts5yypminor   /* The object to be destroyed */
){
  sqlite3Fts5ParserARG_FETCH
  sqlite3Fts5ParserCTX_FETCH
  switch( fts5yymajor ){
    /* Here is inserted the actions which take place when a
    ** terminal or non-terminal is destroyed.  This can happen
    ** when the symbol is popped from the stack during a
    ** reduce or during error processing or when a parser is 
    ** being destroyed before it is finished parsing.
    **
    ** Note: during a reduce, the only symbols destroyed are those
    ** which appear on the RHS of the rule, but which are *not* used
    ** inside the C code.
    */
/********* Begin destructor definitions ***************************************/
    case 16: /* input */
{
#line 83 "fts5parse.y"
 (void)pParse; 
#line 586 "fts5parse.c"
}
      break;
    case 17: /* expr */
    case 18: /* cnearset */
    case 19: /* exprlist */
{
#line 89 "fts5parse.y"
 sqlite3Fts5ParseNodeFree((fts5yypminor->fts5yy24)); 
#line 595 "fts5parse.c"
}
      break;
    case 20: /* colset */
    case 21: /* colsetlist */
{
#line 93 "fts5parse.y"
 sqlite3_free((fts5yypminor->fts5yy11)); 
#line 603 "fts5parse.c"
}
      break;
    case 22: /* nearset */
    case 23: /* nearphrases */
{
#line 148 "fts5parse.y"
 sqlite3Fts5ParseNearsetFree((fts5yypminor->fts5yy46)); 
#line 611 "fts5parse.c"
}
      break;
    case 24: /* phrase */
{
#line 183 "fts5parse.y"
 sqlite3Fts5ParsePhraseFree((fts5yypminor->fts5yy53)); 
#line 618 "fts5parse.c"
}
      break;
/********* End destructor definitions *****************************************/
    default:  break;   /* If no destructor action specified: do nothing */
  }
}

/*
** Pop the parser's stack once.
**
** If there is a destructor routine associated with the token which
** is popped from the stack, then call it.
*/
static void fts5yy_pop_parser_stack(fts5yyParser *pParser){
  fts5yyStackEntry *fts5yytos;
  assert( pParser->fts5yytos!=0 );
  assert( pParser->fts5yytos > pParser->fts5yystack );
  fts5yytos = pParser->fts5yytos--;
#ifndef NDEBUG
  if( fts5yyTraceFILE ){
    fprintf(fts5yyTraceFILE,"%sPopping %s\n",
      fts5yyTracePrompt,
      fts5yyTokenName[fts5yytos->major]);
  }
#endif
  fts5yy_destructor(pParser, fts5yytos->major, &fts5yytos->minor);
}

/*
** Clear all secondary memory allocations from the parser
*/
static void sqlite3Fts5ParserFinalize(void *p){
  fts5yyParser *pParser = (fts5yyParser*)p;
  while( pParser->fts5yytos>pParser->fts5yystack ) fts5yy_pop_parser_stack(pParser);
#if fts5YYSTACKDEPTH<=0
  if( pParser->fts5yystack!=&pParser->fts5yystk0 ) free(pParser->fts5yystack);
#endif
}

#ifndef sqlite3Fts5Parser_ENGINEALWAYSONSTACK
/* 
** Deallocate and destroy a parser.  Destructors are called for
** all stack elements before shutting the parser down.
**
** If the fts5YYPARSEFREENEVERNULL macro exists (for example because it
** is defined in a %include section of the input grammar) then it is
** assumed that the input pointer is never NULL.
*/
static void sqlite3Fts5ParserFree(
  void *p,                    /* The parser to be deleted */
  void (*freeProc)(void*)     /* Function used to reclaim memory */
){
#ifndef fts5YYPARSEFREENEVERNULL
  if( p==0 ) return;
#endif
  sqlite3Fts5ParserFinalize(p);
  (*freeProc)(p);
}
#endif /* sqlite3Fts5Parser_ENGINEALWAYSONSTACK */

/*
** Return the peak depth of the stack for a parser.
*/
#ifdef fts5YYTRACKMAXSTACKDEPTH
static int sqlite3Fts5ParserStackPeak(void *p){
  fts5yyParser *pParser = (fts5yyParser*)p;
  return pParser->fts5yyhwm;
}
#endif

/* This array of booleans keeps track of the parser statement
** coverage.  The element fts5yycoverage[X][Y] is set when the parser
** is in state X and has a lookahead token Y.  In a well-tested
** systems, every element of this matrix should end up being set.
*/
#if defined(fts5YYCOVERAGE)
static unsigned char fts5yycoverage[fts5YYNSTATE][fts5YYNFTS5TOKEN];
#endif

/*
** Write into out a description of every state/lookahead combination that
**
**   (1)  has not been used by the parser, and
**   (2)  is not a syntax error.
**
** Return the number of missed state/lookahead combinations.
*/
#if defined(fts5YYCOVERAGE)
static int sqlite3Fts5ParserCoverage(FILE *out){
  int stateno, iLookAhead, i;
  int nMissed = 0;
  for(stateno=0; stateno<fts5YYNSTATE; stateno++){
    i = fts5yy_shift_ofst[stateno];
    for(iLookAhead=0; iLookAhead<fts5YYNFTS5TOKEN; iLookAhead++){
      if( fts5yy_lookahead[i+iLookAhead]!=iLookAhead ) continue;
      if( fts5yycoverage[stateno][iLookAhead]==0 ) nMissed++;
      if( out ){
        fprintf(out,"State %d lookahead %s %s\n", stateno,
                fts5yyTokenName[iLookAhead],
                fts5yycoverage[stateno][iLookAhead] ? "ok" : "missed");
      }
    }
  }
  return nMissed;
}
#endif

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
*/
static fts5YYACTIONTYPE fts5yy_find_shift_action(
  fts5YYCODETYPE iLookAhead,    /* The look-ahead token */
  fts5YYACTIONTYPE stateno      /* Current state number */
){
  int i;

  if( stateno>fts5YY_MAX_SHIFT ) return stateno;
  assert( stateno <= fts5YY_SHIFT_COUNT );
#if defined(fts5YYCOVERAGE)
  fts5yycoverage[stateno][iLookAhead] = 1;
#endif
  do{
    i = fts5yy_shift_ofst[stateno];
    assert( i>=0 );
    assert( i<=fts5YY_ACTTAB_COUNT );
    assert( i+fts5YYNFTS5TOKEN<=(int)fts5YY_NLOOKAHEAD );
    assert( iLookAhead!=fts5YYNOCODE );
    assert( iLookAhead < fts5YYNFTS5TOKEN );
    i += iLookAhead;
    assert( i<(int)fts5YY_NLOOKAHEAD );
    if( fts5yy_lookahead[i]!=iLookAhead ){
#ifdef fts5YYFALLBACK
      fts5YYCODETYPE iFallback;            /* Fallback token */
      assert( iLookAhead<sizeof(fts5yyFallback)/sizeof(fts5yyFallback[0]) );
      iFallback = fts5yyFallback[iLookAhead];
      if( iFallback!=0 ){
#ifndef NDEBUG
        if( fts5yyTraceFILE ){
          fprintf(fts5yyTraceFILE, "%sFALLBACK %s => %s\n",
             fts5yyTracePrompt, fts5yyTokenName[iLookAhead], fts5yyTokenName[iFallback]);
        }
#endif
        assert( fts5yyFallback[iFallback]==0 ); /* Fallback loop must terminate */
        iLookAhead = iFallback;
        continue;
      }
#endif
#ifdef fts5YYWILDCARD
      {
        int j = i - iLookAhead + fts5YYWILDCARD;
        assert( j<(int)(sizeof(fts5yy_lookahead)/sizeof(fts5yy_lookahead[0])) );
        if( fts5yy_lookahead[j]==fts5YYWILDCARD && iLookAhead>0 ){
#ifndef NDEBUG
          if( fts5yyTraceFILE ){
            fprintf(fts5yyTraceFILE, "%sWILDCARD %s => %s\n",
               fts5yyTracePrompt, fts5yyTokenName[iLookAhead],
               fts5yyTokenName[fts5YYWILDCARD]);
          }
#endif /* NDEBUG */
          return fts5yy_action[j];
        }
      }
#endif /* fts5YYWILDCARD */
      return fts5yy_default[stateno];
    }else{
      assert( i>=0 && i<(int)(sizeof(fts5yy_action)/sizeof(fts5yy_action[0])) );
      return fts5yy_action[i];
    }
  }while(1);
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
*/
static fts5YYACTIONTYPE fts5yy_find_reduce_action(
  fts5YYACTIONTYPE stateno,     /* Current state number */
  fts5YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
#ifdef fts5YYERRORSYMBOL
  if( stateno>fts5YY_REDUCE_COUNT ){
    return fts5yy_default[stateno];
  }
#else
  assert( stateno<=fts5YY_REDUCE_COUNT );
#endif
  i = fts5yy_reduce_ofst[stateno];
  assert( iLookAhead!=fts5YYNOCODE );
  i += iLookAhead;
#ifdef fts5YYERRORSYMBOL
  if( i<0 || i>=fts5YY_ACTTAB_COUNT || fts5yy_lookahead[i]!=iLookAhead ){
    return fts5yy_default[stateno];
  }
#else
  assert( i>=0 && i<fts5YY_ACTTAB_COUNT );
  assert( fts5yy_lookahead[i]==iLookAhead );
#endif
  return fts5yy_action[i];
}

/*
** The following routine is called if the stack overflows.
*/
static void fts5yyStackOverflow(fts5yyParser *fts5yypParser){
   sqlite3Fts5ParserARG_FETCH
   sqlite3Fts5ParserCTX_FETCH
#ifndef NDEBUG
   if( fts5yyTraceFILE ){
     fprintf(fts5yyTraceFILE,"%sStack Overflow!\n",fts5yyTracePrompt);
   }
#endif
   while( fts5yypParser->fts5yytos>fts5yypParser->fts5yystack ) fts5yy_pop_parser_stack(fts5yypParser);
   /* Here code is inserted which will execute if the parser
   ** stack every overflows */
/******** Begin %stack_overflow code ******************************************/
#line 36 "fts5parse.y"

  sqlite3Fts5ParseError(pParse, "fts5: parser stack overflow");
#line 839 "fts5parse.c"
/******** End %stack_overflow code ********************************************/
   sqlite3Fts5ParserARG_STORE /* Suppress warning about unused %extra_argument var */
   sqlite3Fts5ParserCTX_STORE
}

/*
** Print tracing information for a SHIFT action
*/
#ifndef NDEBUG
static void fts5yyTraceShift(fts5yyParser *fts5yypParser, int fts5yyNewState, const char *zTag){
  if( fts5yyTraceFILE ){
    if( fts5yyNewState<fts5YYNSTATE ){
      fprintf(fts5yyTraceFILE,"%s%s '%s', go to state %d\n",
         fts5yyTracePrompt, zTag, fts5yyTokenName[fts5yypParser->fts5yytos->major],
         fts5yyNewState);
    }else{
      fprintf(fts5yyTraceFILE,"%s%s '%s', pending reduce %d\n",
         fts5yyTracePrompt, zTag, fts5yyTokenName[fts5yypParser->fts5yytos->major],
         fts5yyNewState - fts5YY_MIN_REDUCE);
    }
  }
}
#else
# define fts5yyTraceShift(X,Y,Z)
#endif

/*
** Perform a shift action.
*/
static void fts5yy_shift(
  fts5yyParser *fts5yypParser,          /* The parser to be shifted */
  fts5YYACTIONTYPE fts5yyNewState,      /* The new state to shift in */
  fts5YYCODETYPE fts5yyMajor,           /* The major token to shift in */
  sqlite3Fts5ParserFTS5TOKENTYPE fts5yyMinor        /* The minor token to shift in */
){
  fts5yyStackEntry *fts5yytos;
  fts5yypParser->fts5yytos++;
#ifdef fts5YYTRACKMAXSTACKDEPTH
  if( (int)(fts5yypParser->fts5yytos - fts5yypParser->fts5yystack)>fts5yypParser->fts5yyhwm ){
    fts5yypParser->fts5yyhwm++;
    assert( fts5yypParser->fts5yyhwm == (int)(fts5yypParser->fts5yytos - fts5yypParser->fts5yystack) );
  }
#endif
#if fts5YYSTACKDEPTH>0 
  if( fts5yypParser->fts5yytos>fts5yypParser->fts5yystackEnd ){
    fts5yypParser->fts5yytos--;
    fts5yyStackOverflow(fts5yypParser);
    return;
  }
#else
  if( fts5yypParser->fts5yytos>=&fts5yypParser->fts5yystack[fts5yypParser->fts5yystksz] ){
    if( fts5yyGrowStack(fts5yypParser) ){
      fts5yypParser->fts5yytos--;
      fts5yyStackOverflow(fts5yypParser);
      return;
    }
  }
#endif
  if( fts5yyNewState > fts5YY_MAX_SHIFT ){
    fts5yyNewState += fts5YY_MIN_REDUCE - fts5YY_MIN_SHIFTREDUCE;
  }
  fts5yytos = fts5yypParser->fts5yytos;
  fts5yytos->stateno = fts5yyNewState;
  fts5yytos->major = fts5yyMajor;
  fts5yytos->minor.fts5yy0 = fts5yyMinor;
  fts5yyTraceShift(fts5yypParser, fts5yyNewState, "Shift");
}

/* For rule J, fts5yyRuleInfoLhs[J] contains the symbol on the left-hand side
** of that rule */
static const fts5YYCODETYPE fts5yyRuleInfoLhs[] = {
    16,  /* (0) input ::= expr */
    20,  /* (1) colset ::= MINUS LCP colsetlist RCP */
    20,  /* (2) colset ::= LCP colsetlist RCP */
    20,  /* (3) colset ::= STRING */
    20,  /* (4) colset ::= MINUS STRING */
    21,  /* (5) colsetlist ::= colsetlist STRING */
    21,  /* (6) colsetlist ::= STRING */
    17,  /* (7) expr ::= expr AND expr */
    17,  /* (8) expr ::= expr OR expr */
    17,  /* (9) expr ::= expr NOT expr */
    17,  /* (10) expr ::= colset COLON LP expr RP */
    17,  /* (11) expr ::= LP expr RP */
    17,  /* (12) expr ::= exprlist */
    19,  /* (13) exprlist ::= cnearset */
    19,  /* (14) exprlist ::= exprlist cnearset */
    18,  /* (15) cnearset ::= nearset */
    18,  /* (16) cnearset ::= colset COLON nearset */
    22,  /* (17) nearset ::= phrase */
    22,  /* (18) nearset ::= CARET phrase */
    22,  /* (19) nearset ::= STRING LP nearphrases neardist_opt RP */
    23,  /* (20) nearphrases ::= phrase */
    23,  /* (21) nearphrases ::= nearphrases phrase */
    25,  /* (22) neardist_opt ::= */
    25,  /* (23) neardist_opt ::= COMMA STRING */
    24,  /* (24) phrase ::= phrase PLUS STRING star_opt */
    24,  /* (25) phrase ::= STRING star_opt */
    26,  /* (26) star_opt ::= STAR */
    26,  /* (27) star_opt ::= */
};

/* For rule J, fts5yyRuleInfoNRhs[J] contains the negative of the number
** of symbols on the right-hand side of that rule. */
static const signed char fts5yyRuleInfoNRhs[] = {
   -1,  /* (0) input ::= expr */
   -4,  /* (1) colset ::= MINUS LCP colsetlist RCP */
   -3,  /* (2) colset ::= LCP colsetlist RCP */
   -1,  /* (3) colset ::= STRING */
   -2,  /* (4) colset ::= MINUS STRING */
   -2,  /* (5) colsetlist ::= colsetlist STRING */
   -1,  /* (6) colsetlist ::= STRING */
   -3,  /* (7) expr ::= expr AND expr */
   -3,  /* (8) expr ::= expr OR expr */
   -3,  /* (9) expr ::= expr NOT expr */
   -5,  /* (10) expr ::= colset COLON LP expr RP */
   -3,  /* (11) expr ::= LP expr RP */
   -1,  /* (12) expr ::= exprlist */
   -1,  /* (13) exprlist ::= cnearset */
   -2,  /* (14) exprlist ::= exprlist cnearset */
   -1,  /* (15) cnearset ::= nearset */
   -3,  /* (16) cnearset ::= colset COLON nearset */
   -1,  /* (17) nearset ::= phrase */
   -2,  /* (18) nearset ::= CARET phrase */
   -5,  /* (19) nearset ::= STRING LP nearphrases neardist_opt RP */
   -1,  /* (20) nearphrases ::= phrase */
   -2,  /* (21) nearphrases ::= nearphrases phrase */
    0,  /* (22) neardist_opt ::= */
   -2,  /* (23) neardist_opt ::= COMMA STRING */
   -4,  /* (24) phrase ::= phrase PLUS STRING star_opt */
   -2,  /* (25) phrase ::= STRING star_opt */
   -1,  /* (26) star_opt ::= STAR */
    0,  /* (27) star_opt ::= */
};

static void fts5yy_accept(fts5yyParser*);  /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
**
** The fts5yyLookahead and fts5yyLookaheadToken parameters provide reduce actions
** access to the lookahead token (if any).  The fts5yyLookahead will be fts5YYNOCODE
** if the lookahead token has already been consumed.  As this procedure is
** only called from one place, optimizing compilers will in-line it, which
** means that the extra parameters have no performance impact.
*/
static fts5YYACTIONTYPE fts5yy_reduce(
  fts5yyParser *fts5yypParser,         /* The parser */
  unsigned int fts5yyruleno,       /* Number of the rule by which to reduce */
  int fts5yyLookahead,             /* Lookahead token, or fts5YYNOCODE if none */
  sqlite3Fts5ParserFTS5TOKENTYPE fts5yyLookaheadToken  /* Value of the lookahead token */
  sqlite3Fts5ParserCTX_PDECL                   /* %extra_context */
){
  int fts5yygoto;                     /* The next state */
  fts5YYACTIONTYPE fts5yyact;             /* The next action */
  fts5yyStackEntry *fts5yymsp;            /* The top of the parser's stack */
  int fts5yysize;                     /* Amount to pop the stack */
  sqlite3Fts5ParserARG_FETCH
  (void)fts5yyLookahead;
  (void)fts5yyLookaheadToken;
  fts5yymsp = fts5yypParser->fts5yytos;

  switch( fts5yyruleno ){
  /* Beginning here are the reduction cases.  A typical example
  ** follows:
  **   case 0:
  **  #line <lineno> <grammarfile>
  **     { ... }           // User supplied code
  **  #line <lineno> <thisfile>
  **     break;
  */
/********** Begin reduce actions **********************************************/
        fts5YYMINORTYPE fts5yylhsminor;
      case 0: /* input ::= expr */
#line 82 "fts5parse.y"
{ sqlite3Fts5ParseFinished(pParse, fts5yymsp[0].minor.fts5yy24); }
#line 1016 "fts5parse.c"
        break;
      case 1: /* colset ::= MINUS LCP colsetlist RCP */
#line 97 "fts5parse.y"
{ 
    fts5yymsp[-3].minor.fts5yy11 = sqlite3Fts5ParseColsetInvert(pParse, fts5yymsp[-1].minor.fts5yy11);
}
#line 1023 "fts5parse.c"
        break;
      case 2: /* colset ::= LCP colsetlist RCP */
#line 100 "fts5parse.y"
{ fts5yymsp[-2].minor.fts5yy11 = fts5yymsp[-1].minor.fts5yy11; }
#line 1028 "fts5parse.c"
        break;
      case 3: /* colset ::= STRING */
#line 101 "fts5parse.y"
{
  fts5yylhsminor.fts5yy11 = sqlite3Fts5ParseColset(pParse, 0, &fts5yymsp[0].minor.fts5yy0);
}
#line 1035 "fts5parse.c"
  fts5yymsp[0].minor.fts5yy11 = fts5yylhsminor.fts5yy11;
        break;
      case 4: /* colset ::= MINUS STRING */
#line 104 "fts5parse.y"
{
  fts5yymsp[-1].minor.fts5yy11 = sqlite3Fts5ParseColset(pParse, 0, &fts5yymsp[0].minor.fts5yy0);
  fts5yymsp[-1].minor.fts5yy11 = sqlite3Fts5ParseColsetInvert(pParse, fts5yymsp[-1].minor.fts5yy11);
}
#line 1044 "fts5parse.c"
        break;
      case 5: /* colsetlist ::= colsetlist STRING */
#line 109 "fts5parse.y"
{ 
  fts5yylhsminor.fts5yy11 = sqlite3Fts5ParseColset(pParse, fts5yymsp[-1].minor.fts5yy11, &fts5yymsp[0].minor.fts5yy0); }
#line 1050 "fts5parse.c"
  fts5yymsp[-1].minor.fts5yy11 = fts5yylhsminor.fts5yy11;
        break;
      case 6: /* colsetlist ::= STRING */
#line 111 "fts5parse.y"
{ 
  fts5yylhsminor.fts5yy11 = sqlite3Fts5ParseColset(pParse, 0, &fts5yymsp[0].minor.fts5yy0); 
}
#line 1058 "fts5parse.c"
  fts5yymsp[0].minor.fts5yy11 = fts5yylhsminor.fts5yy11;
        break;
      case 7: /* expr ::= expr AND expr */
#line 115 "fts5parse.y"
{
  fts5yylhsminor.fts5yy24 = sqlite3Fts5ParseNode(pParse, FTS5_AND, fts5yymsp[-2].minor.fts5yy24, fts5yymsp[0].minor.fts5yy24, 0);
}
#line 1066 "fts5parse.c"
  fts5yymsp[-2].minor.fts5yy24 = fts5yylhsminor.fts5yy24;
        break;
      case 8: /* expr ::= expr OR expr */
#line 118 "fts5parse.y"
{
  fts5yylhsminor.fts5yy24 = sqlite3Fts5ParseNode(pParse, FTS5_OR, fts5yymsp[-2].minor.fts5yy24, fts5yymsp[0].minor.fts5yy24, 0);
}
#line 1074 "fts5parse.c"
  fts5yymsp[-2].minor.fts5yy24 = fts5yylhsminor.fts5yy24;
        break;
      case 9: /* expr ::= expr NOT expr */
#line 121 "fts5parse.y"
{
  fts5yylhsminor.fts5yy24 = sqlite3Fts5ParseNode(pParse, FTS5_NOT, fts5yymsp[-2].minor.fts5yy24, fts5yymsp[0].minor.fts5yy24, 0);
}
#line 1082 "fts5parse.c"
  fts5yymsp[-2].minor.fts5yy24 = fts5yylhsminor.fts5yy24;
        break;
      case 10: /* expr ::= colset COLON LP expr RP */
#line 125 "fts5parse.y"
{
  sqlite3Fts5ParseSetColset(pParse, fts5yymsp[-1].minor.fts5yy24, fts5yymsp[-4].minor.fts5yy11);
  fts5yylhsminor.fts5yy24 = fts5yymsp[-1].minor.fts5yy24;
}
#line 1091 "fts5parse.c"
  fts5yymsp[-4].minor.fts5yy24 = fts5yylhsminor.fts5yy24;
        break;
      case 11: /* expr ::= LP expr RP */
#line 129 "fts5parse.y"
{fts5yymsp[-2].minor.fts5yy24 = fts5yymsp[-1].minor.fts5yy24;}
#line 1097 "fts5parse.c"
        break;
      case 12: /* expr ::= exprlist */
      case 13: /* exprlist ::= cnearset */ fts5yytestcase(fts5yyruleno==13);
#line 130 "fts5parse.y"
{fts5yylhsminor.fts5yy24 = fts5yymsp[0].minor.fts5yy24;}
#line 1103 "fts5parse.c"
  fts5yymsp[0].minor.fts5yy24 = fts5yylhsminor.fts5yy24;
        break;
      case 14: /* exprlist ::= exprlist cnearset */
#line 133 "fts5parse.y"
{
  fts5yylhsminor.fts5yy24 = sqlite3Fts5ParseImplicitAnd(pParse, fts5yymsp[-1].minor.fts5yy24, fts5yymsp[0].minor.fts5yy24);
}
#line 1111 "fts5parse.c"
  fts5yymsp[-1].minor.fts5yy24 = fts5yylhsminor.fts5yy24;
        break;
      case 15: /* cnearset ::= nearset */
#line 137 "fts5parse.y"
{ 
  fts5yylhsminor.fts5yy24 = sqlite3Fts5ParseNode(pParse, FTS5_STRING, 0, 0, fts5yymsp[0].minor.fts5yy46); 
}
#line 1119 "fts5parse.c"
  fts5yymsp[0].minor.fts5yy24 = fts5yylhsminor.fts5yy24;
        break;
      case 16: /* cnearset ::= colset COLON nearset */
#line 140 "fts5parse.y"
{ 
  fts5yylhsminor.fts5yy24 = sqlite3Fts5ParseNode(pParse, FTS5_STRING, 0, 0, fts5yymsp[0].minor.fts5yy46); 
  sqlite3Fts5ParseSetColset(pParse, fts5yylhsminor.fts5yy24, fts5yymsp[-2].minor.fts5yy11);
}
#line 1128 "fts5parse.c"
  fts5yymsp[-2].minor.fts5yy24 = fts5yylhsminor.fts5yy24;
        break;
      case 17: /* nearset ::= phrase */
#line 151 "fts5parse.y"
{ fts5yylhsminor.fts5yy46 = sqlite3Fts5ParseNearset(pParse, 0, fts5yymsp[0].minor.fts5yy53); }
#line 1134 "fts5parse.c"
  fts5yymsp[0].minor.fts5yy46 = fts5yylhsminor.fts5yy46;
        break;
      case 18: /* nearset ::= CARET phrase */
#line 152 "fts5parse.y"
{ 
  sqlite3Fts5ParseSetCaret(fts5yymsp[0].minor.fts5yy53);
  fts5yymsp[-1].minor.fts5yy46 = sqlite3Fts5ParseNearset(pParse, 0, fts5yymsp[0].minor.fts5yy53); 
}
#line 1143 "fts5parse.c"
        break;
      case 19: /* nearset ::= STRING LP nearphrases neardist_opt RP */
#line 156 "fts5parse.y"
{
  sqlite3Fts5ParseNear(pParse, &fts5yymsp[-4].minor.fts5yy0);
  sqlite3Fts5ParseSetDistance(pParse, fts5yymsp[-2].minor.fts5yy46, &fts5yymsp[-1].minor.fts5yy0);
  fts5yylhsminor.fts5yy46 = fts5yymsp[-2].minor.fts5yy46;
}
#line 1152 "fts5parse.c"
  fts5yymsp[-4].minor.fts5yy46 = fts5yylhsminor.fts5yy46;
        break;
      case 20: /* nearphrases ::= phrase */
#line 162 "fts5parse.y"
{ 
  fts5yylhsminor.fts5yy46 = sqlite3Fts5ParseNearset(pParse, 0, fts5yymsp[0].minor.fts5yy53); 
}
#line 1160 "fts5parse.c"
  fts5yymsp[0].minor.fts5yy46 = fts5yylhsminor.fts5yy46;
        break;
      case 21: /* nearphrases ::= nearphrases phrase */
#line 165 "fts5parse.y"
{
  fts5yylhsminor.fts5yy46 = sqlite3Fts5ParseNearset(pParse, fts5yymsp[-1].minor.fts5yy46, fts5yymsp[0].minor.fts5yy53);
}
#line 1168 "fts5parse.c"
  fts5yymsp[-1].minor.fts5yy46 = fts5yylhsminor.fts5yy46;
        break;
      case 22: /* neardist_opt ::= */
#line 172 "fts5parse.y"
{ fts5yymsp[1].minor.fts5yy0.p = 0; fts5yymsp[1].minor.fts5yy0.n = 0; }
#line 1174 "fts5parse.c"
        break;
      case 23: /* neardist_opt ::= COMMA STRING */
#line 173 "fts5parse.y"
{ fts5yymsp[-1].minor.fts5yy0 = fts5yymsp[0].minor.fts5yy0; }
#line 1179 "fts5parse.c"
        break;
      case 24: /* phrase ::= phrase PLUS STRING star_opt */
#line 185 "fts5parse.y"
{ 
  fts5yylhsminor.fts5yy53 = sqlite3Fts5ParseTerm(pParse, fts5yymsp[-3].minor.fts5yy53, &fts5yymsp[-1].minor.fts5yy0, fts5yymsp[0].minor.fts5yy4);
}
#line 1186 "fts5parse.c"
  fts5yymsp[-3].minor.fts5yy53 = fts5yylhsminor.fts5yy53;
        break;
      case 25: /* phrase ::= STRING star_opt */
#line 188 "fts5parse.y"
{ 
  fts5yylhsminor.fts5yy53 = sqlite3Fts5ParseTerm(pParse, 0, &fts5yymsp[-1].minor.fts5yy0, fts5yymsp[0].minor.fts5yy4);
}
#line 1194 "fts5parse.c"
  fts5yymsp[-1].minor.fts5yy53 = fts5yylhsminor.fts5yy53;
        break;
      case 26: /* star_opt ::= STAR */
#line 196 "fts5parse.y"
{ fts5yymsp[0].minor.fts5yy4 = 1; }
#line 1200 "fts5parse.c"
        break;
      case 27: /* star_opt ::= */
#line 197 "fts5parse.y"
{ fts5yymsp[1].minor.fts5yy4 = 0; }
#line 1205 "fts5parse.c"
        break;
      default:
        break;
/********** End reduce actions ************************************************/
  };
  assert( fts5yyruleno<sizeof(fts5yyRuleInfoLhs)/sizeof(fts5yyRuleInfoLhs[0]) );
  fts5yygoto = fts5yyRuleInfoLhs[fts5yyruleno];
  fts5yysize = fts5yyRuleInfoNRhs[fts5yyruleno];
  fts5yyact = fts5yy_find_reduce_action(fts5yymsp[fts5yysize].stateno,(fts5YYCODETYPE)fts5yygoto);

  /* There are no SHIFTREDUCE actions on nonterminals because the table
  ** generator has simplified them to pure REDUCE actions. */
  assert( !(fts5yyact>fts5YY_MAX_SHIFT && fts5yyact<=fts5YY_MAX_SHIFTREDUCE) );

  /* It is not possible for a REDUCE to be followed by an error */
  assert( fts5yyact!=fts5YY_ERROR_ACTION );

  fts5yymsp += fts5yysize+1;
  fts5yypParser->fts5yytos = fts5yymsp;
  fts5yymsp->stateno = (fts5YYACTIONTYPE)fts5yyact;
  fts5yymsp->major = (fts5YYCODETYPE)fts5yygoto;
  fts5yyTraceShift(fts5yypParser, fts5yyact, "... then shift");
  return fts5yyact;
}

/*
** The following code executes when the parse fails
*/
#ifndef fts5YYNOERRORRECOVERY
static void fts5yy_parse_failed(
  fts5yyParser *fts5yypParser           /* The parser */
){
  sqlite3Fts5ParserARG_FETCH
  sqlite3Fts5ParserCTX_FETCH
#ifndef NDEBUG
  if( fts5yyTraceFILE ){
    fprintf(fts5yyTraceFILE,"%sFail!\n",fts5yyTracePrompt);
  }
#endif
  while( fts5yypParser->fts5yytos>fts5yypParser->fts5yystack ) fts5yy_pop_parser_stack(fts5yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
/************ Begin %parse_failure code ***************************************/
/************ End %parse_failure code *****************************************/
  sqlite3Fts5ParserARG_STORE /* Suppress warning about unused %extra_argument variable */
  sqlite3Fts5ParserCTX_STORE
}
#endif /* fts5YYNOERRORRECOVERY */

/*
** The following code executes when a syntax error first occurs.
*/
static void fts5yy_syntax_error(
  fts5yyParser *fts5yypParser,           /* The parser */
  int fts5yymajor,                   /* The major type of the error token */
  sqlite3Fts5ParserFTS5TOKENTYPE fts5yyminor         /* The minor type of the error token */
){
  sqlite3Fts5ParserARG_FETCH
  sqlite3Fts5ParserCTX_FETCH
#define FTS5TOKEN fts5yyminor
/************ Begin %syntax_error code ****************************************/
#line 30 "fts5parse.y"

  UNUSED_PARAM(fts5yymajor); /* Silence a compiler warning */
  sqlite3Fts5ParseError(
    pParse, "fts5: syntax error near \"%.*s\"",FTS5TOKEN.n,FTS5TOKEN.p
  );
#line 1273 "fts5parse.c"
/************ End %syntax_error code ******************************************/
  sqlite3Fts5ParserARG_STORE /* Suppress warning about unused %extra_argument variable */
  sqlite3Fts5ParserCTX_STORE
}

/*
** The following is executed when the parser accepts
*/
static void fts5yy_accept(
  fts5yyParser *fts5yypParser           /* The parser */
){
  sqlite3Fts5ParserARG_FETCH
  sqlite3Fts5ParserCTX_FETCH
#ifndef NDEBUG
  if( fts5yyTraceFILE ){
    fprintf(fts5yyTraceFILE,"%sAccept!\n",fts5yyTracePrompt);
  }
#endif
#ifndef fts5YYNOERRORRECOVERY
  fts5yypParser->fts5yyerrcnt = -1;
#endif
  assert( fts5yypParser->fts5yytos==fts5yypParser->fts5yystack );
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
/*********** Begin %parse_accept code *****************************************/
/*********** End %parse_accept code *******************************************/
  sqlite3Fts5ParserARG_STORE /* Suppress warning about unused %extra_argument variable */
  sqlite3Fts5ParserCTX_STORE
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "sqlite3Fts5ParserAlloc" which describes the current state of the parser.
** The second argument is the major token number.  The third is
** the minor token.  The fourth optional argument is whatever the
** user wants (and specified in the grammar) and is available for
** use by the action routines.
**
** Inputs:
** <ul>
** <li> A pointer to the parser (an opaque structure.)
** <li> The major token number.
** <li> The minor token number.
** <li> An option argument of a grammar-specified type.
** </ul>
**
** Outputs:
** None.
*/
static void sqlite3Fts5Parser(
  void *fts5yyp,                   /* The parser */
  int fts5yymajor,                 /* The major token code number */
  sqlite3Fts5ParserFTS5TOKENTYPE fts5yyminor       /* The value for the token */
  sqlite3Fts5ParserARG_PDECL               /* Optional %extra_argument parameter */
){
  fts5YYMINORTYPE fts5yyminorunion;
  fts5YYACTIONTYPE fts5yyact;   /* The parser action. */
#if !defined(fts5YYERRORSYMBOL) && !defined(fts5YYNOERRORRECOVERY)
  int fts5yyendofinput;     /* True if we are at the end of input */
#endif
#ifdef fts5YYERRORSYMBOL
  int fts5yyerrorhit = 0;   /* True if fts5yymajor has invoked an error */
#endif
  fts5yyParser *fts5yypParser = (fts5yyParser*)fts5yyp;  /* The parser */
  sqlite3Fts5ParserCTX_FETCH
  sqlite3Fts5ParserARG_STORE

  assert( fts5yypParser->fts5yytos!=0 );
#if !defined(fts5YYERRORSYMBOL) && !defined(fts5YYNOERRORRECOVERY)
  fts5yyendofinput = (fts5yymajor==0);
#endif

  fts5yyact = fts5yypParser->fts5yytos->stateno;
#ifndef NDEBUG
  if( fts5yyTraceFILE ){
    if( fts5yyact < fts5YY_MIN_REDUCE ){
      fprintf(fts5yyTraceFILE,"%sInput '%s' in state %d\n",
              fts5yyTracePrompt,fts5yyTokenName[fts5yymajor],fts5yyact);
    }else{
      fprintf(fts5yyTraceFILE,"%sInput '%s' with pending reduce %d\n",
              fts5yyTracePrompt,fts5yyTokenName[fts5yymajor],fts5yyact-fts5YY_MIN_REDUCE);
    }
  }
#endif

  while(1){ /* Exit by "break" */
    assert( fts5yypParser->fts5yytos>=fts5yypParser->fts5yystack );
    assert( fts5yyact==fts5yypParser->fts5yytos->stateno );
    fts5yyact = fts5yy_find_shift_action((fts5YYCODETYPE)fts5yymajor,fts5yyact);
    if( fts5yyact >= fts5YY_MIN_REDUCE ){
      unsigned int fts5yyruleno = fts5yyact - fts5YY_MIN_REDUCE; /* Reduce by this rule */
#ifndef NDEBUG
      assert( fts5yyruleno<(int)(sizeof(fts5yyRuleName)/sizeof(fts5yyRuleName[0])) );
      if( fts5yyTraceFILE ){
        int fts5yysize = fts5yyRuleInfoNRhs[fts5yyruleno];
        if( fts5yysize ){
          fprintf(fts5yyTraceFILE, "%sReduce %d [%s]%s, pop back to state %d.\n",
            fts5yyTracePrompt,
            fts5yyruleno, fts5yyRuleName[fts5yyruleno],
            fts5yyruleno<fts5YYNRULE_WITH_ACTION ? "" : " without external action",
            fts5yypParser->fts5yytos[fts5yysize].stateno);
        }else{
          fprintf(fts5yyTraceFILE, "%sReduce %d [%s]%s.\n",
            fts5yyTracePrompt, fts5yyruleno, fts5yyRuleName[fts5yyruleno],
            fts5yyruleno<fts5YYNRULE_WITH_ACTION ? "" : " without external action");
        }
      }
#endif /* NDEBUG */

      /* Check that the stack is large enough to grow by a single entry
      ** if the RHS of the rule is empty.  This ensures that there is room
      ** enough on the stack to push the LHS value */
      if( fts5yyRuleInfoNRhs[fts5yyruleno]==0 ){
#ifdef fts5YYTRACKMAXSTACKDEPTH
        if( (int)(fts5yypParser->fts5yytos - fts5yypParser->fts5yystack)>fts5yypParser->fts5yyhwm ){
          fts5yypParser->fts5yyhwm++;
          assert( fts5yypParser->fts5yyhwm ==
                  (int)(fts5yypParser->fts5yytos - fts5yypParser->fts5yystack));
        }
#endif
#if fts5YYSTACKDEPTH>0 
        if( fts5yypParser->fts5yytos>=fts5yypParser->fts5yystackEnd ){
          fts5yyStackOverflow(fts5yypParser);
          break;
        }
#else
        if( fts5yypParser->fts5yytos>=&fts5yypParser->fts5yystack[fts5yypParser->fts5yystksz-1] ){
          if( fts5yyGrowStack(fts5yypParser) ){
            fts5yyStackOverflow(fts5yypParser);
            break;
          }
        }
#endif
      }
      fts5yyact = fts5yy_reduce(fts5yypParser,fts5yyruleno,fts5yymajor,fts5yyminor sqlite3Fts5ParserCTX_PARAM);
    }else if( fts5yyact <= fts5YY_MAX_SHIFTREDUCE ){
      fts5yy_shift(fts5yypParser,fts5yyact,(fts5YYCODETYPE)fts5yymajor,fts5yyminor);
#ifndef fts5YYNOERRORRECOVERY
      fts5yypParser->fts5yyerrcnt--;
#endif
      break;
    }else if( fts5yyact==fts5YY_ACCEPT_ACTION ){
      fts5yypParser->fts5yytos--;
      fts5yy_accept(fts5yypParser);
      return;
    }else{
      assert( fts5yyact == fts5YY_ERROR_ACTION );
      fts5yyminorunion.fts5yy0 = fts5yyminor;
#ifdef fts5YYERRORSYMBOL
      int fts5yymx;
#endif
#ifndef NDEBUG
      if( fts5yyTraceFILE ){
        fprintf(fts5yyTraceFILE,"%sSyntax Error!\n",fts5yyTracePrompt);
      }
#endif
#ifdef fts5YYERRORSYMBOL
      /* A syntax error has occurred.
      ** The response to an error depends upon whether or not the
      ** grammar defines an error token "ERROR".  
      **
      ** This is what we do if the grammar does define ERROR:
      **
      **  * Call the %syntax_error function.
      **
      **  * Begin popping the stack until we enter a state where
      **    it is legal to shift the error symbol, then shift
      **    the error symbol.
      **
      **  * Set the error count to three.
      **
      **  * Begin accepting and shifting new tokens.  No new error
      **    processing will occur until three tokens have been
      **    shifted successfully.
      **
      */
      if( fts5yypParser->fts5yyerrcnt<0 ){
        fts5yy_syntax_error(fts5yypParser,fts5yymajor,fts5yyminor);
      }
      fts5yymx = fts5yypParser->fts5yytos->major;
      if( fts5yymx==fts5YYERRORSYMBOL || fts5yyerrorhit ){
#ifndef NDEBUG
        if( fts5yyTraceFILE ){
          fprintf(fts5yyTraceFILE,"%sDiscard input token %s\n",
             fts5yyTracePrompt,fts5yyTokenName[fts5yymajor]);
        }
#endif
        fts5yy_destructor(fts5yypParser, (fts5YYCODETYPE)fts5yymajor, &fts5yyminorunion);
        fts5yymajor = fts5YYNOCODE;
      }else{
        while( fts5yypParser->fts5yytos > fts5yypParser->fts5yystack ){
          fts5yyact = fts5yy_find_reduce_action(fts5yypParser->fts5yytos->stateno,
                                        fts5YYERRORSYMBOL);
          if( fts5yyact<=fts5YY_MAX_SHIFTREDUCE ) break;
          fts5yy_pop_parser_stack(fts5yypParser);
        }
        if( fts5yypParser->fts5yytos <= fts5yypParser->fts5yystack || fts5yymajor==0 ){
          fts5yy_destructor(fts5yypParser,(fts5YYCODETYPE)fts5yymajor,&fts5yyminorunion);
          fts5yy_parse_failed(fts5yypParser);
#ifndef fts5YYNOERRORRECOVERY
          fts5yypParser->fts5yyerrcnt = -1;
#endif
          fts5yymajor = fts5YYNOCODE;
        }else if( fts5yymx!=fts5YYERRORSYMBOL ){
          fts5yy_shift(fts5yypParser,fts5yyact,fts5YYERRORSYMBOL,fts5yyminor);
        }
      }
      fts5yypParser->fts5yyerrcnt = 3;
      fts5yyerrorhit = 1;
      if( fts5yymajor==fts5YYNOCODE ) break;
      fts5yyact = fts5yypParser->fts5yytos->stateno;
#elif defined(fts5YYNOERRORRECOVERY)
      /* If the fts5YYNOERRORRECOVERY macro is defined, then do not attempt to
      ** do any kind of error recovery.  Instead, simply invoke the syntax
      ** error routine and continue going as if nothing had happened.
      **
      ** Applications can set this macro (for example inside %include) if
      ** they intend to abandon the parse upon the first syntax error seen.
      */
      fts5yy_syntax_error(fts5yypParser,fts5yymajor, fts5yyminor);
      fts5yy_destructor(fts5yypParser,(fts5YYCODETYPE)fts5yymajor,&fts5yyminorunion);
      break;
#else  /* fts5YYERRORSYMBOL is not defined */
      /* This is what we do if the grammar does not define ERROR:
      **
      **  * Report an error message, and throw away the input token.
      **
      **  * If the input token is $, then fail the parse.
      **
      ** As before, subsequent error messages are suppressed until
      ** three input tokens have been successfully shifted.
      */
      if( fts5yypParser->fts5yyerrcnt<=0 ){
        fts5yy_syntax_error(fts5yypParser,fts5yymajor, fts5yyminor);
      }
      fts5yypParser->fts5yyerrcnt = 3;
      fts5yy_destructor(fts5yypParser,(fts5YYCODETYPE)fts5yymajor,&fts5yyminorunion);
      if( fts5yyendofinput ){
        fts5yy_parse_failed(fts5yypParser);
#ifndef fts5YYNOERRORRECOVERY
        fts5yypParser->fts5yyerrcnt = -1;
#endif
      }
      break;
#endif
    }
  }
#ifndef NDEBUG
  if( fts5yyTraceFILE ){
    fts5yyStackEntry *i;
    char cDiv = '[';
    fprintf(fts5yyTraceFILE,"%sReturn. Stack=",fts5yyTracePrompt);
    for(i=&fts5yypParser->fts5yystack[1]; i<=fts5yypParser->fts5yytos; i++){
      fprintf(fts5yyTraceFILE,"%c%s", cDiv, fts5yyTokenName[i->major]);
      cDiv = ' ';
    }
    fprintf(fts5yyTraceFILE,"]\n");
  }
#endif
  return;
}

/*
** Return the fallback token corresponding to canonical token iToken, or
** 0 if iToken has no fallback.
*/
static int sqlite3Fts5ParserFallback(int iToken){
#ifdef fts5YYFALLBACK
  assert( iToken<(int)(sizeof(fts5yyFallback)/sizeof(fts5yyFallback[0])) );
  return fts5yyFallback[iToken];
#else
  (void)iToken;
  return 0;
#endif
}

#line 1 "fts5_aux.c"
/*
** 2014 May 31
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
******************************************************************************
*/


/* #include "fts5Int.h" */

/*
** Object used to iterate through all "coalesced phrase instances" in 
** a single column of the current row. If the phrase instances in the
** column being considered do not overlap, this object simply iterates
** through them. Or, if they do overlap (share one or more tokens in
** common), each set of overlapping instances is treated as a single
** match. See documentation for the highlight() auxiliary function for
** details.
**
** Usage is:
**
**   for(rc = fts5CInstIterNext(pApi, pFts, iCol, &iter);
**      (rc==SQLITE_OK && 0==fts5CInstIterEof(&iter);
**      rc = fts5CInstIterNext(&iter)
**   ){
**     printf("instance starts at %d, ends at %d\n", iter.iStart, iter.iEnd);
**   }
**
*/
typedef struct CInstIter CInstIter;
struct CInstIter {
  const Fts5ExtensionApi *pApi;   /* API offered by current FTS version */
  Fts5Context *pFts;              /* First arg to pass to pApi functions */
  int iCol;                       /* Column to search */
  int iInst;                      /* Next phrase instance index */
  int nInst;                      /* Total number of phrase instances */

  /* Output variables */
  int iStart;                     /* First token in coalesced phrase instance */
  int iEnd;                       /* Last token in coalesced phrase instance */
};

/*
** Advance the iterator to the next coalesced phrase instance. Return
** an SQLite error code if an error occurs, or SQLITE_OK otherwise.
*/
static int fts5CInstIterNext(CInstIter *pIter){
  int rc = SQLITE_OK;
  pIter->iStart = -1;
  pIter->iEnd = -1;

  while( rc==SQLITE_OK && pIter->iInst<pIter->nInst ){
    int ip; int ic; int io;
    rc = pIter->pApi->xInst(pIter->pFts, pIter->iInst, &ip, &ic, &io);
    if( rc==SQLITE_OK ){
      if( ic==pIter->iCol ){
        int iEnd = io - 1 + pIter->pApi->xPhraseSize(pIter->pFts, ip);
        if( pIter->iStart<0 ){
          pIter->iStart = io;
          pIter->iEnd = iEnd;
        }else if( io<=pIter->iEnd ){
          if( iEnd>pIter->iEnd ) pIter->iEnd = iEnd;
        }else{
          break;
        }
      }
      pIter->iInst++;
    }
  }

  return rc;
}

/*
** Initialize the iterator object indicated by the final parameter to 
** iterate through coalesced phrase instances in column iCol.
*/
static int fts5CInstIterInit(
  const Fts5ExtensionApi *pApi,
  Fts5Context *pFts,
  int iCol,
  CInstIter *pIter
){
  int rc;

  memset(pIter, 0, sizeof(CInstIter));
  pIter->pApi = pApi;
  pIter->pFts = pFts;
  pIter->iCol = iCol;
  rc = pApi->xInstCount(pFts, &pIter->nInst);

  if( rc==SQLITE_OK ){
    rc = fts5CInstIterNext(pIter);
  }

  return rc;
}



/*************************************************************************
** Start of highlight() implementation.
*/
typedef struct HighlightContext HighlightContext;
struct HighlightContext {
  CInstIter iter;                 /* Coalesced Instance Iterator */
  int iPos;                       /* Current token offset in zIn[] */
  int iRangeStart;                /* First token to include */
  int iRangeEnd;                  /* If non-zero, last token to include */
  const char *zOpen;              /* Opening highlight */
  const char *zClose;             /* Closing highlight */
  const char *zIn;                /* Input text */
  int nIn;                        /* Size of input text in bytes */
  int iOff;                       /* Current offset within zIn[] */
  char *zOut;                     /* Output value */
};

/*
** Append text to the HighlightContext output string - p->zOut. Argument
** z points to a buffer containing n bytes of text to append. If n is 
** negative, everything up until the first '\0' is appended to the output.
**
** If *pRc is set to any value other than SQLITE_OK when this function is 
** called, it is a no-op. If an error (i.e. an OOM condition) is encountered, 
** *pRc is set to an error code before returning. 
*/
static void fts5HighlightAppend(
  int *pRc, 
  HighlightContext *p, 
  const char *z, int n
){
  if( *pRc==SQLITE_OK && z ){
    if( n<0 ) n = (int)strlen(z);
    p->zOut = sqlite3_mprintf("%z%.*s", p->zOut, n, z);
    if( p->zOut==0 ) *pRc = SQLITE_NOMEM;
  }
}

/*
** Tokenizer callback used by implementation of highlight() function.
*/
static int fts5HighlightCb(
  void *pContext,                 /* Pointer to HighlightContext object */
  int tflags,                     /* Mask of FTS5_TOKEN_* flags */
  const char *pToken,             /* Buffer containing token */
  int nToken,                     /* Size of token in bytes */
  int iStartOff,                  /* Start offset of token */
  int iEndOff                     /* End offset of token */
){
  HighlightContext *p = (HighlightContext*)pContext;
  int rc = SQLITE_OK;
  int iPos;

  UNUSED_PARAM2(pToken, nToken);

  if( tflags & FTS5_TOKEN_COLOCATED ) return SQLITE_OK;
  iPos = p->iPos++;

  if( p->iRangeEnd>0 ){
    if( iPos<p->iRangeStart || iPos>p->iRangeEnd ) return SQLITE_OK;
    if( p->iRangeStart && iPos==p->iRangeStart ) p->iOff = iStartOff;
  }

  if( iPos==p->iter.iStart ){
    fts5HighlightAppend(&rc, p, &p->zIn[p->iOff], iStartOff - p->iOff);
    fts5HighlightAppend(&rc, p, p->zOpen, -1);
    p->iOff = iStartOff;
  }

  if( iPos==p->iter.iEnd ){
    if( p->iRangeEnd && p->iter.iStart<p->iRangeStart ){
      fts5HighlightAppend(&rc, p, p->zOpen, -1);
    }
    fts5HighlightAppend(&rc, p, &p->zIn[p->iOff], iEndOff - p->iOff);
    fts5HighlightAppend(&rc, p, p->zClose, -1);
    p->iOff = iEndOff;
    if( rc==SQLITE_OK ){
      rc = fts5CInstIterNext(&p->iter);
    }
  }

  if( p->iRangeEnd>0 && iPos==p->iRangeEnd ){
    fts5HighlightAppend(&rc, p, &p->zIn[p->iOff], iEndOff - p->iOff);
    p->iOff = iEndOff;
    if( iPos>=p->iter.iStart && iPos<p->iter.iEnd ){
      fts5HighlightAppend(&rc, p, p->zClose, -1);
    }
  }

  return rc;
}

/*
** Implementation of highlight() function.
*/
static void fts5HighlightFunction(
  const Fts5ExtensionApi *pApi,   /* API offered by current FTS version */
  Fts5Context *pFts,              /* First arg to pass to pApi functions */
  sqlite3_context *pCtx,          /* Context for returning result/error */
  int nVal,                       /* Number of values in apVal[] array */
  sqlite3_value **apVal           /* Array of trailing arguments */
){
  HighlightContext ctx;
  int rc;
  int iCol;

  if( nVal!=3 ){
    const char *zErr = "wrong number of arguments to function highlight()";
    sqlite3_result_error(pCtx, zErr, -1);
    return;
  }

  iCol = sqlite3_value_int(apVal[0]);
  memset(&ctx, 0, sizeof(HighlightContext));
  ctx.zOpen = (const char*)sqlite3_value_text(apVal[1]);
  ctx.zClose = (const char*)sqlite3_value_text(apVal[2]);
  rc = pApi->xColumnText(pFts, iCol, &ctx.zIn, &ctx.nIn);

  if( ctx.zIn ){
    if( rc==SQLITE_OK ){
      rc = fts5CInstIterInit(pApi, pFts, iCol, &ctx.iter);
    }

    if( rc==SQLITE_OK ){
      rc = pApi->xTokenize(pFts, ctx.zIn, ctx.nIn, (void*)&ctx,fts5HighlightCb);
    }
    fts5HighlightAppend(&rc, &ctx, &ctx.zIn[ctx.iOff], ctx.nIn - ctx.iOff);

    if( rc==SQLITE_OK ){
      sqlite3_result_text(pCtx, (const char*)ctx.zOut, -1, SQLITE_TRANSIENT);
    }
    sqlite3_free(ctx.zOut);
  }
  if( rc!=SQLITE_OK ){
    sqlite3_result_error_code(pCtx, rc);
  }
}
/*
** End of highlight() implementation.
**************************************************************************/

/*
** Context object passed to the fts5SentenceFinderCb() function.
*/
typedef struct Fts5SFinder Fts5SFinder;
struct Fts5SFinder {
  int iPos;                       /* Current token position */
  int nFirstAlloc;                /* Allocated size of aFirst[] */
  int nFirst;                     /* Number of entries in aFirst[] */
  int *aFirst;                    /* Array of first token in each sentence */
  const char *zDoc;               /* Document being tokenized */
};

/*
** Add an entry to the Fts5SFinder.aFirst[] array. Grow the array if
** necessary. Return SQLITE_OK if successful, or SQLITE_NOMEM if an
** error occurs.
*/
static int fts5SentenceFinderAdd(Fts5SFinder *p, int iAdd){
  if( p->nFirstAlloc==p->nFirst ){
    int nNew = p->nFirstAlloc ? p->nFirstAlloc*2 : 64;
    int *aNew;

    aNew = (int*)sqlite3_realloc64(p->aFirst, nNew*sizeof(int));
    if( aNew==0 ) return SQLITE_NOMEM;
    p->aFirst = aNew;
    p->nFirstAlloc = nNew;
  }
  p->aFirst[p->nFirst++] = iAdd;
  return SQLITE_OK;
}

/*
** This function is an xTokenize() callback used by the auxiliary snippet()
** function. Its job is to identify tokens that are the first in a sentence.
** For each such token, an entry is added to the SFinder.aFirst[] array.
*/
static int fts5SentenceFinderCb(
  void *pContext,                 /* Pointer to HighlightContext object */
  int tflags,                     /* Mask of FTS5_TOKEN_* flags */
  const char *pToken,             /* Buffer containing token */
  int nToken,                     /* Size of token in bytes */
  int iStartOff,                  /* Start offset of token */
  int iEndOff                     /* End offset of token */
){
  int rc = SQLITE_OK;

  UNUSED_PARAM2(pToken, nToken);
  UNUSED_PARAM(iEndOff);

  if( (tflags & FTS5_TOKEN_COLOCATED)==0 ){
    Fts5SFinder *p = (Fts5SFinder*)pContext;
    if( p->iPos>0 ){
      int i;
      char c = 0;
      for(i=iStartOff-1; i>=0; i--){
        c = p->zDoc[i];
        if( c!=' ' && c!='\t' && c!='\n' && c!='\r' ) break;
      }
      if( i!=iStartOff-1 && (c=='.' || c==':') ){
        rc = fts5SentenceFinderAdd(p, p->iPos);
      }
    }else{
      rc = fts5SentenceFinderAdd(p, 0);
    }
    p->iPos++;
  }
  return rc;
}

static int fts5SnippetScore(
  const Fts5ExtensionApi *pApi,   /* API offered by current FTS version */
  Fts5Context *pFts,              /* First arg to pass to pApi functions */
  int nDocsize,                   /* Size of column in tokens */
  unsigned char *aSeen,           /* Array with one element per query phrase */
  int iCol,                       /* Column to score */
  int iPos,                       /* Starting offset to score */
  int nToken,                     /* Max tokens per snippet */
  int *pnScore,                   /* OUT: Score */
  int *piPos                      /* OUT: Adjusted offset */
){
  int rc;
  int i;
  int ip = 0;
  int ic = 0;
  int iOff = 0;
  int iFirst = -1;
  int nInst;
  int nScore = 0;
  int iLast = 0;
  sqlite3_int64 iEnd = (sqlite3_int64)iPos + nToken;

  rc = pApi->xInstCount(pFts, &nInst);
  for(i=0; i<nInst && rc==SQLITE_OK; i++){
    rc = pApi->xInst(pFts, i, &ip, &ic, &iOff);
    if( rc==SQLITE_OK && ic==iCol && iOff>=iPos && iOff<iEnd ){
      nScore += (aSeen[ip] ? 1 : 1000);
      aSeen[ip] = 1;
      if( iFirst<0 ) iFirst = iOff;
      iLast = iOff + pApi->xPhraseSize(pFts, ip);
    }
  }

  *pnScore = nScore;
  if( piPos ){
    sqlite3_int64 iAdj = iFirst - (nToken - (iLast-iFirst)) / 2;
    if( (iAdj+nToken)>nDocsize ) iAdj = nDocsize - nToken;
    if( iAdj<0 ) iAdj = 0;
    *piPos = (int)iAdj;
  }

  return rc;
}

/*
** Return the value in pVal interpreted as utf-8 text. Except, if pVal 
** contains a NULL value, return a pointer to a static string zero
** bytes in length instead of a NULL pointer.
*/
static const char *fts5ValueToText(sqlite3_value *pVal){
  const char *zRet = (const char*)sqlite3_value_text(pVal);
  return zRet ? zRet : "";
}

/*
** Implementation of snippet() function.
*/
static void fts5SnippetFunction(
  const Fts5ExtensionApi *pApi,   /* API offered by current FTS version */
  Fts5Context *pFts,              /* First arg to pass to pApi functions */
  sqlite3_context *pCtx,          /* Context for returning result/error */
  int nVal,                       /* Number of values in apVal[] array */
  sqlite3_value **apVal           /* Array of trailing arguments */
){
  HighlightContext ctx;
  int rc = SQLITE_OK;             /* Return code */
  int iCol;                       /* 1st argument to snippet() */
  const char *zEllips;            /* 4th argument to snippet() */
  int nToken;                     /* 5th argument to snippet() */
  int nInst = 0;                  /* Number of instance matches this row */
  int i;                          /* Used to iterate through instances */
  int nPhrase;                    /* Number of phrases in query */
  unsigned char *aSeen;           /* Array of "seen instance" flags */
  int iBestCol;                   /* Column containing best snippet */
  int iBestStart = 0;             /* First token of best snippet */
  int nBestScore = 0;             /* Score of best snippet */
  int nColSize = 0;               /* Total size of iBestCol in tokens */
  Fts5SFinder sFinder;            /* Used to find the beginnings of sentences */
  int nCol;

  if( nVal!=5 ){
    const char *zErr = "wrong number of arguments to function snippet()";
    sqlite3_result_error(pCtx, zErr, -1);
    return;
  }

  nCol = pApi->xColumnCount(pFts);
  memset(&ctx, 0, sizeof(HighlightContext));
  iCol = sqlite3_value_int(apVal[0]);
  ctx.zOpen = fts5ValueToText(apVal[1]);
  ctx.zClose = fts5ValueToText(apVal[2]);
  zEllips = fts5ValueToText(apVal[3]);
  nToken = sqlite3_value_int(apVal[4]);

  iBestCol = (iCol>=0 ? iCol : 0);
  nPhrase = pApi->xPhraseCount(pFts);
  aSeen = sqlite3_malloc(nPhrase);
  if( aSeen==0 ){
    rc = SQLITE_NOMEM;
  }
  if( rc==SQLITE_OK ){
    rc = pApi->xInstCount(pFts, &nInst);
  }

  memset(&sFinder, 0, sizeof(Fts5SFinder));
  for(i=0; i<nCol; i++){
    if( iCol<0 || iCol==i ){
      int nDoc;
      int nDocsize;
      int ii;
      sFinder.iPos = 0;
      sFinder.nFirst = 0;
      rc = pApi->xColumnText(pFts, i, &sFinder.zDoc, &nDoc);
      if( rc!=SQLITE_OK ) break;
      rc = pApi->xTokenize(pFts, 
          sFinder.zDoc, nDoc, (void*)&sFinder,fts5SentenceFinderCb
      );
      if( rc!=SQLITE_OK ) break;
      rc = pApi->xColumnSize(pFts, i, &nDocsize);
      if( rc!=SQLITE_OK ) break;

      for(ii=0; rc==SQLITE_OK && ii<nInst; ii++){
        int ip, ic, io;
        int iAdj;
        int nScore;
        int jj;

        rc = pApi->xInst(pFts, ii, &ip, &ic, &io);
        if( ic!=i ) continue;
        if( io>nDocsize ) rc = FTS5_CORRUPT;
        if( rc!=SQLITE_OK ) continue;
        memset(aSeen, 0, nPhrase);
        rc = fts5SnippetScore(pApi, pFts, nDocsize, aSeen, i,
            io, nToken, &nScore, &iAdj
        );
        if( rc==SQLITE_OK && nScore>nBestScore ){
          nBestScore = nScore;
          iBestCol = i;
          iBestStart = iAdj;
          nColSize = nDocsize;
        }

        if( rc==SQLITE_OK && sFinder.nFirst && nDocsize>nToken ){
          for(jj=0; jj<(sFinder.nFirst-1); jj++){
            if( sFinder.aFirst[jj+1]>io ) break;
          }

          if( sFinder.aFirst[jj]<io ){
            memset(aSeen, 0, nPhrase);
            rc = fts5SnippetScore(pApi, pFts, nDocsize, aSeen, i, 
              sFinder.aFirst[jj], nToken, &nScore, 0
            );

            nScore += (sFinder.aFirst[jj]==0 ? 120 : 100);
            if( rc==SQLITE_OK && nScore>nBestScore ){
              nBestScore = nScore;
              iBestCol = i;
              iBestStart = sFinder.aFirst[jj];
              nColSize = nDocsize;
            }
          }
        }
      }
    }
  }

  if( rc==SQLITE_OK ){
    rc = pApi->xColumnText(pFts, iBestCol, &ctx.zIn, &ctx.nIn);
  }
  if( rc==SQLITE_OK && nColSize==0 ){
    rc = pApi->xColumnSize(pFts, iBestCol, &nColSize);
  }
  if( ctx.zIn ){
    if( rc==SQLITE_OK ){
      rc = fts5CInstIterInit(pApi, pFts, iBestCol, &ctx.iter);
    }

    ctx.iRangeStart = iBestStart;
    ctx.iRangeEnd = iBestStart + nToken - 1;

    if( iBestStart>0 ){
      fts5HighlightAppend(&rc, &ctx, zEllips, -1);
    }

    /* Advance iterator ctx.iter so that it points to the first coalesced
    ** phrase instance at or following position iBestStart. */
    while( ctx.iter.iStart>=0 && ctx.iter.iStart<iBestStart && rc==SQLITE_OK ){
      rc = fts5CInstIterNext(&ctx.iter);
    }

    if( rc==SQLITE_OK ){
      rc = pApi->xTokenize(pFts, ctx.zIn, ctx.nIn, (void*)&ctx,fts5HighlightCb);
    }
    if( ctx.iRangeEnd>=(nColSize-1) ){
      fts5HighlightAppend(&rc, &ctx, &ctx.zIn[ctx.iOff], ctx.nIn - ctx.iOff);
    }else{
      fts5HighlightAppend(&rc, &ctx, zEllips, -1);
    }
  }
  if( rc==SQLITE_OK ){
    sqlite3_result_text(pCtx, (const char*)ctx.zOut, -1, SQLITE_TRANSIENT);
  }else{
    sqlite3_result_error_code(pCtx, rc);
  }
  sqlite3_free(ctx.zOut);
  sqlite3_free(aSeen);
  sqlite3_free(sFinder.aFirst);
}

/************************************************************************/

/*
** The first time the bm25() function is called for a query, an instance
** of the following structure is allocated and populated.
*/
typedef struct Fts5Bm25Data Fts5Bm25Data;
struct Fts5Bm25Data {
  int nPhrase;                    /* Number of phrases in query */
  double avgdl;                   /* Average number of tokens in each row */
  double *aIDF;                   /* IDF for each phrase */
  double *aFreq;                  /* Array used to calculate phrase freq. */
};

/*
** Callback used by fts5Bm25GetData() to count the number of rows in the
** table matched by each individual phrase within the query.
*/
static int fts5CountCb(
  const Fts5ExtensionApi *pApi, 
  Fts5Context *pFts,
  void *pUserData                 /* Pointer to sqlite3_int64 variable */
){
  sqlite3_int64 *pn = (sqlite3_int64*)pUserData;
  UNUSED_PARAM2(pApi, pFts);
  (*pn)++;
  return SQLITE_OK;
}

/*
** Set *ppData to point to the Fts5Bm25Data object for the current query. 
** If the object has not already been allocated, allocate and populate it
** now.
*/
static int fts5Bm25GetData(
  const Fts5ExtensionApi *pApi, 
  Fts5Context *pFts,
  Fts5Bm25Data **ppData           /* OUT: bm25-data object for this query */
){
  int rc = SQLITE_OK;             /* Return code */
  Fts5Bm25Data *p;                /* Object to return */

  p = (Fts5Bm25Data*)pApi->xGetAuxdata(pFts, 0);
  if( p==0 ){
    int nPhrase;                  /* Number of phrases in query */
    sqlite3_int64 nRow = 0;       /* Number of rows in table */
    sqlite3_int64 nToken = 0;     /* Number of tokens in table */
    sqlite3_int64 nByte;          /* Bytes of space to allocate */
    int i;

    /* Allocate the Fts5Bm25Data object */
    nPhrase = pApi->xPhraseCount(pFts);
    nByte = sizeof(Fts5Bm25Data) + nPhrase*2*sizeof(double);
    p = (Fts5Bm25Data*)sqlite3_malloc64(nByte);
    if( p==0 ){
      rc = SQLITE_NOMEM;
    }else{
      memset(p, 0, (size_t)nByte);
      p->nPhrase = nPhrase;
      p->aIDF = (double*)&p[1];
      p->aFreq = &p->aIDF[nPhrase];
    }

    /* Calculate the average document length for this FTS5 table */
    if( rc==SQLITE_OK ) rc = pApi->xRowCount(pFts, &nRow);
    assert( rc!=SQLITE_OK || nRow>0 );
    if( rc==SQLITE_OK ) rc = pApi->xColumnTotalSize(pFts, -1, &nToken);
    if( rc==SQLITE_OK ) p->avgdl = (double)nToken  / (double)nRow;

    /* Calculate an IDF for each phrase in the query */
    for(i=0; rc==SQLITE_OK && i<nPhrase; i++){
      sqlite3_int64 nHit = 0;
      rc = pApi->xQueryPhrase(pFts, i, (void*)&nHit, fts5CountCb);
      if( rc==SQLITE_OK ){
        /* Calculate the IDF (Inverse Document Frequency) for phrase i.
        ** This is done using the standard BM25 formula as found on wikipedia:
        **
        **   IDF = log( (N - nHit + 0.5) / (nHit + 0.5) )
        **
        ** where "N" is the total number of documents in the set and nHit
        ** is the number that contain at least one instance of the phrase
        ** under consideration.
        **
        ** The problem with this is that if (N < 2*nHit), the IDF is 
        ** negative. Which is undesirable. So the mimimum allowable IDF is
        ** (1e-6) - roughly the same as a term that appears in just over
        ** half of set of 5,000,000 documents.  */
        double idf = log( (nRow - nHit + 0.5) / (nHit + 0.5) );
        if( idf<=0.0 ) idf = 1e-6;
        p->aIDF[i] = idf;
      }
    }

    if( rc!=SQLITE_OK ){
      sqlite3_free(p);
    }else{
      rc = pApi->xSetAuxdata(pFts, p, sqlite3_free);
    }
    if( rc!=SQLITE_OK ) p = 0;
  }
  *ppData = p;
  return rc;
}

/*
** Implementation of bm25() function.
*/
static void fts5Bm25Function(
  const Fts5ExtensionApi *pApi,   /* API offered by current FTS version */
  Fts5Context *pFts,              /* First arg to pass to pApi functions */
  sqlite3_context *pCtx,          /* Context for returning result/error */
  int nVal,                       /* Number of values in apVal[] array */
  sqlite3_value **apVal           /* Array of trailing arguments */
){
  const double k1 = 1.2;          /* Constant "k1" from BM25 formula */
  const double b = 0.75;          /* Constant "b" from BM25 formula */
  int rc;                         /* Error code */
  double score = 0.0;             /* SQL function return value */
  Fts5Bm25Data *pData;            /* Values allocated/calculated once only */
  int i;                          /* Iterator variable */
  int nInst = 0;                  /* Value returned by xInstCount() */
  double D = 0.0;                 /* Total number of tokens in row */
  double *aFreq = 0;              /* Array of phrase freq. for current row */

  /* Calculate the phrase frequency (symbol "f(qi,D)" in the documentation)
  ** for each phrase in the query for the current row. */
  rc = fts5Bm25GetData(pApi, pFts, &pData);
  if( rc==SQLITE_OK ){
    aFreq = pData->aFreq;
    memset(aFreq, 0, sizeof(double) * pData->nPhrase);
    rc = pApi->xInstCount(pFts, &nInst);
  }
  for(i=0; rc==SQLITE_OK && i<nInst; i++){
    int ip; int ic; int io;
    rc = pApi->xInst(pFts, i, &ip, &ic, &io);
    if( rc==SQLITE_OK ){
      double w = (nVal > ic) ? sqlite3_value_double(apVal[ic]) : 1.0;
      aFreq[ip] += w;
    }
  }

  /* Figure out the total size of the current row in tokens. */
  if( rc==SQLITE_OK ){
    int nTok;
    rc = pApi->xColumnSize(pFts, -1, &nTok);
    D = (double)nTok;
  }

  /* Determine and return the BM25 score for the current row. Or, if an
  ** error has occurred, throw an exception. */
  if( rc==SQLITE_OK ){
    for(i=0; i<pData->nPhrase; i++){
      score += pData->aIDF[i] * (
          ( aFreq[i] * (k1 + 1.0) ) / 
          ( aFreq[i] + k1 * (1 - b + b * D / pData->avgdl) )
      );
    }
    sqlite3_result_double(pCtx, -1.0 * score);
  }else{
    sqlite3_result_error_code(pCtx, rc);
  }
}

static int sqlite3Fts5AuxInit(fts5_api *pApi){
  struct Builtin {
    const char *zFunc;            /* Function name (nul-terminated) */
    void *pUserData;              /* User-data pointer */
    fts5_extension_function xFunc;/* Callback function */
    void (*xDestroy)(void*);      /* Destructor function */
  } aBuiltin [] = {
    { "snippet",   0, fts5SnippetFunction, 0 },
    { "highlight", 0, fts5HighlightFunction, 0 },
    { "bm25",      0, fts5Bm25Function,    0 },
  };
  int rc = SQLITE_OK;             /* Return code */
  int i;                          /* To iterate through builtin functions */

  for(i=0; rc==SQLITE_OK && i<ArraySize(aBuiltin); i++){
    rc = pApi->xCreateFunction(pApi,
        aBuiltin[i].zFunc,
        aBuiltin[i].pUserData,
        aBuiltin[i].xFunc,
        aBuiltin[i].xDestroy
    );
  }

  return rc;
}

#line 1 "fts5_buffer.c"
/*
** 2014 May 31
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
******************************************************************************
*/



/* #include "fts5Int.h" */

static int sqlite3Fts5BufferSize(int *pRc, Fts5Buffer *pBuf, u32 nByte){
  if( (u32)pBuf->nSpace<nByte ){
    u64 nNew = pBuf->nSpace ? pBuf->nSpace : 64;
    u8 *pNew;
    while( nNew<nByte ){
      nNew = nNew * 2;
    }
    pNew = sqlite3_realloc64(pBuf->p, nNew);
    if( pNew==0 ){
      *pRc = SQLITE_NOMEM;
      return 1;
    }else{
      pBuf->nSpace = (int)nNew;
      pBuf->p = pNew;
    }
  }
  return 0;
}


/*
** Encode value iVal as an SQLite varint and append it to the buffer object
** pBuf. If an OOM error occurs, set the error code in p.
*/
static void sqlite3Fts5BufferAppendVarint(int *pRc, Fts5Buffer *pBuf, i64 iVal){
  if( fts5BufferGrow(pRc, pBuf, 9) ) return;
  pBuf->n += sqlite3Fts5PutVarint(&pBuf->p[pBuf->n], iVal);
}

static void sqlite3Fts5Put32(u8 *aBuf, int iVal){
  aBuf[0] = (iVal>>24) & 0x00FF;
  aBuf[1] = (iVal>>16) & 0x00FF;
  aBuf[2] = (iVal>> 8) & 0x00FF;
  aBuf[3] = (iVal>> 0) & 0x00FF;
}

static int sqlite3Fts5Get32(const u8 *aBuf){
  return (int)((((u32)aBuf[0])<<24) + (aBuf[1]<<16) + (aBuf[2]<<8) + aBuf[3]);
}

/*
** Append buffer nData/pData to buffer pBuf. If an OOM error occurs, set 
** the error code in p. If an error has already occurred when this function
** is called, it is a no-op.
*/
static void sqlite3Fts5BufferAppendBlob(
  int *pRc,
  Fts5Buffer *pBuf, 
  u32 nData, 
  const u8 *pData
){
  if( nData ){
    if( fts5BufferGrow(pRc, pBuf, nData) ) return;
    memcpy(&pBuf->p[pBuf->n], pData, nData);
    pBuf->n += nData;
  }
}

/*
** Append the nul-terminated string zStr to the buffer pBuf. This function
** ensures that the byte following the buffer data is set to 0x00, even 
** though this byte is not included in the pBuf->n count.
*/
static void sqlite3Fts5BufferAppendString(
  int *pRc,
  Fts5Buffer *pBuf, 
  const char *zStr
){
  int nStr = (int)strlen(zStr);
  sqlite3Fts5BufferAppendBlob(pRc, pBuf, nStr+1, (const u8*)zStr);
  pBuf->n--;
}

/*
** Argument zFmt is a printf() style format string. This function performs
** the printf() style processing, then appends the results to buffer pBuf.
**
** Like sqlite3Fts5BufferAppendString(), this function ensures that the byte 
** following the buffer data is set to 0x00, even though this byte is not
** included in the pBuf->n count.
*/ 
static void sqlite3Fts5BufferAppendPrintf(
  int *pRc,
  Fts5Buffer *pBuf, 
  char *zFmt, ...
){
  if( *pRc==SQLITE_OK ){
    char *zTmp;
    va_list ap;
    va_start(ap, zFmt);
    zTmp = sqlite3_vmprintf(zFmt, ap);
    va_end(ap);

    if( zTmp==0 ){
      *pRc = SQLITE_NOMEM;
    }else{
      sqlite3Fts5BufferAppendString(pRc, pBuf, zTmp);
      sqlite3_free(zTmp);
    }
  }
}

static char *sqlite3Fts5Mprintf(int *pRc, const char *zFmt, ...){
  char *zRet = 0;
  if( *pRc==SQLITE_OK ){
    va_list ap;
    va_start(ap, zFmt);
    zRet = sqlite3_vmprintf(zFmt, ap);
    va_end(ap);
    if( zRet==0 ){
      *pRc = SQLITE_NOMEM; 
    }
  }
  return zRet;
}
 

/*
** Free any buffer allocated by pBuf. Zero the structure before returning.
*/
static void sqlite3Fts5BufferFree(Fts5Buffer *pBuf){
  sqlite3_free(pBuf->p);
  memset(pBuf, 0, sizeof(Fts5Buffer));
}

/*
** Zero the contents of the buffer object. But do not free the associated 
** memory allocation.
*/
static void sqlite3Fts5BufferZero(Fts5Buffer *pBuf){
  pBuf->n = 0;
}

/*
** Set the buffer to contain nData/pData. If an OOM error occurs, leave an
** the error code in p. If an error has already occurred when this function
** is called, it is a no-op.
*/
static void sqlite3Fts5BufferSet(
  int *pRc,
  Fts5Buffer *pBuf, 
  int nData, 
  const u8 *pData
){
  pBuf->n = 0;
  sqlite3Fts5BufferAppendBlob(pRc, pBuf, nData, pData);
}

static int sqlite3Fts5PoslistNext64(
  const u8 *a, int n,             /* Buffer containing poslist */
  int *pi,                        /* IN/OUT: Offset within a[] */
  i64 *piOff                      /* IN/OUT: Current offset */
){
  int i = *pi;
  if( i>=n ){
    /* EOF */
    *piOff = -1;
    return 1;  
  }else{
    i64 iOff = *piOff;
    u32 iVal;
    fts5FastGetVarint32(a, i, iVal);
    if( iVal<=1 ){
      if( iVal==0 ){
        *pi = i;
        return 0;
      }
      fts5FastGetVarint32(a, i, iVal);
      iOff = ((i64)iVal) << 32;
      assert( iOff>=0 );
      fts5FastGetVarint32(a, i, iVal);
      if( iVal<2 ){
        /* This is a corrupt record. So stop parsing it here. */
        *piOff = -1;
        return 1;
      }
      *piOff = iOff + ((iVal-2) & 0x7FFFFFFF);
    }else{
      *piOff = (iOff & (i64)0x7FFFFFFF<<32)+((iOff + (iVal-2)) & 0x7FFFFFFF);
    }
    *pi = i;
    assert_nc( *piOff>=iOff );
    return 0;
  }
}


/*
** Advance the iterator object passed as the only argument. Return true
** if the iterator reaches EOF, or false otherwise.
*/
static int sqlite3Fts5PoslistReaderNext(Fts5PoslistReader *pIter){
  if( sqlite3Fts5PoslistNext64(pIter->a, pIter->n, &pIter->i, &pIter->iPos) ){
    pIter->bEof = 1;
  }
  return pIter->bEof;
}

static int sqlite3Fts5PoslistReaderInit(
  const u8 *a, int n,             /* Poslist buffer to iterate through */
  Fts5PoslistReader *pIter        /* Iterator object to initialize */
){
  memset(pIter, 0, sizeof(*pIter));
  pIter->a = a;
  pIter->n = n;
  sqlite3Fts5PoslistReaderNext(pIter);
  return pIter->bEof;
}

/*
** Append position iPos to the position list being accumulated in buffer
** pBuf, which must be already be large enough to hold the new data.
** The previous position written to this list is *piPrev. *piPrev is set
** to iPos before returning.
*/
static void sqlite3Fts5PoslistSafeAppend(
  Fts5Buffer *pBuf, 
  i64 *piPrev, 
  i64 iPos
){
  if( iPos>=*piPrev ){
    static const i64 colmask = ((i64)(0x7FFFFFFF)) << 32;
    if( (iPos & colmask) != (*piPrev & colmask) ){
      pBuf->p[pBuf->n++] = 1;
      pBuf->n += sqlite3Fts5PutVarint(&pBuf->p[pBuf->n], (iPos>>32));
      *piPrev = (iPos & colmask);
    }
    pBuf->n += sqlite3Fts5PutVarint(&pBuf->p[pBuf->n], (iPos-*piPrev)+2);
    *piPrev = iPos;
  }
}

static int sqlite3Fts5PoslistWriterAppend(
  Fts5Buffer *pBuf, 
  Fts5PoslistWriter *pWriter,
  i64 iPos
){
  int rc = 0;   /* Initialized only to suppress erroneous warning from Clang */
  if( fts5BufferGrow(&rc, pBuf, 5+5+5) ) return rc;
  sqlite3Fts5PoslistSafeAppend(pBuf, &pWriter->iPrev, iPos);
  return SQLITE_OK;
}

static void *sqlite3Fts5MallocZero(int *pRc, sqlite3_int64 nByte){
  void *pRet = 0;
  if( *pRc==SQLITE_OK ){
    pRet = sqlite3_malloc64(nByte);
    if( pRet==0 ){
      if( nByte>0 ) *pRc = SQLITE_NOMEM;
    }else{
      memset(pRet, 0, (size_t)nByte);
    }
  }
  return pRet;
}

/*
** Return a nul-terminated copy of the string indicated by pIn. If nIn
** is non-negative, then it is the length of the string in bytes. Otherwise,
** the length of the string is determined using strlen().
**
** It is the responsibility of the caller to eventually free the returned
** buffer using sqlite3_free(). If an OOM error occurs, NULL is returned. 
*/
static char *sqlite3Fts5Strndup(int *pRc, const char *pIn, int nIn){
  char *zRet = 0;
  if( *pRc==SQLITE_OK ){
    if( nIn<0 ){
      nIn = (int)strlen(pIn);
    }
    zRet = (char*)sqlite3_malloc(nIn+1);
    if( zRet ){
      memcpy(zRet, pIn, nIn);
      zRet[nIn] = '\0';
    }else{
      *pRc = SQLITE_NOMEM;
    }
  }
  return zRet;
}


/*
** Return true if character 't' may be part of an FTS5 bareword, or false
** otherwise. Characters that may be part of barewords:
**
**   * All non-ASCII characters,
**   * The 52 upper and lower case ASCII characters, and
**   * The 10 integer ASCII characters.
**   * The underscore character "_" (0x5F).
**   * The unicode "subsitute" character (0x1A).
*/
static int sqlite3Fts5IsBareword(char t){
  u8 aBareword[128] = {
    0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,   /* 0x00 .. 0x0F */
    0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 1, 0, 0, 0, 0, 0,   /* 0x10 .. 0x1F */
    0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,   /* 0x20 .. 0x2F */
    1, 1, 1, 1, 1, 1, 1, 1,    1, 1, 0, 0, 0, 0, 0, 0,   /* 0x30 .. 0x3F */
    0, 1, 1, 1, 1, 1, 1, 1,    1, 1, 1, 1, 1, 1, 1, 1,   /* 0x40 .. 0x4F */
    1, 1, 1, 1, 1, 1, 1, 1,    1, 1, 1, 0, 0, 0, 0, 1,   /* 0x50 .. 0x5F */
    0, 1, 1, 1, 1, 1, 1, 1,    1, 1, 1, 1, 1, 1, 1, 1,   /* 0x60 .. 0x6F */
    1, 1, 1, 1, 1, 1, 1, 1,    1, 1, 1, 0, 0, 0, 0, 0    /* 0x70 .. 0x7F */
  };

  return (t & 0x80) || aBareword[(int)t];
}


/*************************************************************************
*/
typedef struct Fts5TermsetEntry Fts5TermsetEntry;
struct Fts5TermsetEntry {
  char *pTerm;
  int nTerm;
  int iIdx;                       /* Index (main or aPrefix[] entry) */
  Fts5TermsetEntry *pNext;
};

struct Fts5Termset {
  Fts5TermsetEntry *apHash[512];
};

static int sqlite3Fts5TermsetNew(Fts5Termset **pp){
  int rc = SQLITE_OK;
  *pp = sqlite3Fts5MallocZero(&rc, sizeof(Fts5Termset));
  return rc;
}

static int sqlite3Fts5TermsetAdd(
  Fts5Termset *p, 
  int iIdx,
  const char *pTerm, int nTerm, 
  int *pbPresent
){
  int rc = SQLITE_OK;
  *pbPresent = 0;
  if( p ){
    int i;
    u32 hash = 13;
    Fts5TermsetEntry *pEntry;

    /* Calculate a hash value for this term. This is the same hash checksum
    ** used by the fts5_hash.c module. This is not important for correct
    ** operation of the module, but is necessary to ensure that some tests
    ** designed to produce hash table collisions really do work.  */
    for(i=nTerm-1; i>=0; i--){
      hash = (hash << 3) ^ hash ^ pTerm[i];
    }
    hash = (hash << 3) ^ hash ^ iIdx;
    hash = hash % ArraySize(p->apHash);

    for(pEntry=p->apHash[hash]; pEntry; pEntry=pEntry->pNext){
      if( pEntry->iIdx==iIdx 
          && pEntry->nTerm==nTerm 
          && memcmp(pEntry->pTerm, pTerm, nTerm)==0 
      ){
        *pbPresent = 1;
        break;
      }
    }

    if( pEntry==0 ){
      pEntry = sqlite3Fts5MallocZero(&rc, sizeof(Fts5TermsetEntry) + nTerm);
      if( pEntry ){
        pEntry->pTerm = (char*)&pEntry[1];
        pEntry->nTerm = nTerm;
        pEntry->iIdx = iIdx;
        memcpy(pEntry->pTerm, pTerm, nTerm);
        pEntry->pNext = p->apHash[hash];
        p->apHash[hash] = pEntry;
      }
    }
  }

  return rc;
}

static void sqlite3Fts5TermsetFree(Fts5Termset *p){
  if( p ){
    u32 i;
    for(i=0; i<ArraySize(p->apHash); i++){
      Fts5TermsetEntry *pEntry = p->apHash[i];
      while( pEntry ){
        Fts5TermsetEntry *pDel = pEntry;
        pEntry = pEntry->pNext;
        sqlite3_free(pDel);
      }
    }
    sqlite3_free(p);
  }
}

#line 1 "fts5_config.c"
/*
** 2014 Jun 09
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
******************************************************************************
**
** This is an SQLite module implementing full-text search.
*/


/* #include "fts5Int.h" */

#define FTS5_DEFAULT_PAGE_SIZE   4050
#define FTS5_DEFAULT_AUTOMERGE      4
#define FTS5_DEFAULT_USERMERGE      4
#define FTS5_DEFAULT_CRISISMERGE   16
#define FTS5_DEFAULT_HASHSIZE    (1024*1024)

/* Maximum allowed page size */
#define FTS5_MAX_PAGE_SIZE (64*1024)

static int fts5_iswhitespace(char x){
  return (x==' ');
}

static int fts5_isopenquote(char x){
  return (x=='"' || x=='\'' || x=='[' || x=='`');
}

/*
** Argument pIn points to a character that is part of a nul-terminated 
** string. Return a pointer to the first character following *pIn in 
** the string that is not a white-space character.
*/
static const char *fts5ConfigSkipWhitespace(const char *pIn){
  const char *p = pIn;
  if( p ){
    while( fts5_iswhitespace(*p) ){ p++; }
  }
  return p;
}

/*
** Argument pIn points to a character that is part of a nul-terminated 
** string. Return a pointer to the first character following *pIn in 
** the string that is not a "bareword" character.
*/
static const char *fts5ConfigSkipBareword(const char *pIn){
  const char *p = pIn;
  while ( sqlite3Fts5IsBareword(*p) ) p++;
  if( p==pIn ) p = 0;
  return p;
}

static int fts5_isdigit(char a){
  return (a>='0' && a<='9');
}



static const char *fts5ConfigSkipLiteral(const char *pIn){
  const char *p = pIn;
  switch( *p ){
    case 'n': case 'N':
      if( sqlite3_strnicmp("null", p, 4)==0 ){
        p = &p[4];
      }else{
        p = 0;
      }
      break;

    case 'x': case 'X':
      p++;
      if( *p=='\'' ){
        p++;
        while( (*p>='a' && *p<='f') 
            || (*p>='A' && *p<='F') 
            || (*p>='0' && *p<='9') 
            ){
          p++;
        }
        if( *p=='\'' && 0==((p-pIn)%2) ){
          p++;
        }else{
          p = 0;
        }
      }else{
        p = 0;
      }
      break;

    case '\'':
      p++;
      while( p ){
        if( *p=='\'' ){
          p++;
          if( *p!='\'' ) break;
        }
        p++;
        if( *p==0 ) p = 0;
      }
      break;

    default:
      /* maybe a number */
      if( *p=='+' || *p=='-' ) p++;
      while( fts5_isdigit(*p) ) p++;

      /* At this point, if the literal was an integer, the parse is 
      ** finished. Or, if it is a floating point value, it may continue
      ** with either a decimal point or an 'E' character. */
      if( *p=='.' && fts5_isdigit(p[1]) ){
        p += 2;
        while( fts5_isdigit(*p) ) p++;
      }
      if( p==pIn ) p = 0;

      break;
  }

  return p;
}

/*
** The first character of the string pointed to by argument z is guaranteed
** to be an open-quote character (see function fts5_isopenquote()).
**
** This function searches for the corresponding close-quote character within
** the string and, if found, dequotes the string in place and adds a new
** nul-terminator byte.
**
** If the close-quote is found, the value returned is the byte offset of
** the character immediately following it. Or, if the close-quote is not 
** found, -1 is returned. If -1 is returned, the buffer is left in an 
** undefined state.
*/
static int fts5Dequote(char *z){
  char q;
  int iIn = 1;
  int iOut = 0;
  q = z[0];

  /* Set stack variable q to the close-quote character */
  assert( q=='[' || q=='\'' || q=='"' || q=='`' );
  if( q=='[' ) q = ']';  

  while( z[iIn] ){
    if( z[iIn]==q ){
      if( z[iIn+1]!=q ){
        /* Character iIn was the close quote. */
        iIn++;
        break;
      }else{
        /* Character iIn and iIn+1 form an escaped quote character. Skip
        ** the input cursor past both and copy a single quote character 
        ** to the output buffer. */
        iIn += 2;
        z[iOut++] = q;
      }
    }else{
      z[iOut++] = z[iIn++];
    }
  }

  z[iOut] = '\0';
  return iIn;
}

/*
** Convert an SQL-style quoted string into a normal string by removing
** the quote characters.  The conversion is done in-place.  If the
** input does not begin with a quote character, then this routine
** is a no-op.
**
** Examples:
**
**     "abc"   becomes   abc
**     'xyz'   becomes   xyz
**     [pqr]   becomes   pqr
**     `mno`   becomes   mno
*/
static void sqlite3Fts5Dequote(char *z){
  char quote;                     /* Quote character (if any ) */

  assert( 0==fts5_iswhitespace(z[0]) );
  quote = z[0];
  if( quote=='[' || quote=='\'' || quote=='"' || quote=='`' ){
    fts5Dequote(z);
  }
}


struct Fts5Enum {
  const char *zName;
  int eVal;
};
typedef struct Fts5Enum Fts5Enum;

static int fts5ConfigSetEnum(
  const Fts5Enum *aEnum, 
  const char *zEnum, 
  int *peVal
){
  int nEnum = (int)strlen(zEnum);
  int i;
  int iVal = -1;

  for(i=0; aEnum[i].zName; i++){
    if( sqlite3_strnicmp(aEnum[i].zName, zEnum, nEnum)==0 ){
      if( iVal>=0 ) return SQLITE_ERROR;
      iVal = aEnum[i].eVal;
    }
  }

  *peVal = iVal;
  return iVal<0 ? SQLITE_ERROR : SQLITE_OK;
}

/*
** Parse a "special" CREATE VIRTUAL TABLE directive and update
** configuration object pConfig as appropriate.
**
** If successful, object pConfig is updated and SQLITE_OK returned. If
** an error occurs, an SQLite error code is returned and an error message
** may be left in *pzErr. It is the responsibility of the caller to
** eventually free any such error message using sqlite3_free().
*/
static int fts5ConfigParseSpecial(
  Fts5Global *pGlobal,
  Fts5Config *pConfig,            /* Configuration object to update */
  const char *zCmd,               /* Special command to parse */
  const char *zArg,               /* Argument to parse */
  char **pzErr                    /* OUT: Error message */
){
  int rc = SQLITE_OK;
  int nCmd = (int)strlen(zCmd);
  if( sqlite3_strnicmp("prefix", zCmd, nCmd)==0 ){
    const int nByte = sizeof(int) * FTS5_MAX_PREFIX_INDEXES;
    const char *p;
    int bFirst = 1;
    if( pConfig->aPrefix==0 ){
      pConfig->aPrefix = sqlite3Fts5MallocZero(&rc, nByte);
      if( rc ) return rc;
    }

    p = zArg;
    while( 1 ){
      int nPre = 0;

      while( p[0]==' ' ) p++;
      if( bFirst==0 && p[0]==',' ){
        p++;
        while( p[0]==' ' ) p++;
      }else if( p[0]=='\0' ){
        break;
      }
      if( p[0]<'0' || p[0]>'9' ){
        *pzErr = sqlite3_mprintf("malformed prefix=... directive");
        rc = SQLITE_ERROR;
        break;
      }

      if( pConfig->nPrefix==FTS5_MAX_PREFIX_INDEXES ){
        *pzErr = sqlite3_mprintf(
            "too many prefix indexes (max %d)", FTS5_MAX_PREFIX_INDEXES
        );
        rc = SQLITE_ERROR;
        break;
      }

      while( p[0]>='0' && p[0]<='9' && nPre<1000 ){
        nPre = nPre*10 + (p[0] - '0');
        p++;
      }

      if( nPre<=0 || nPre>=1000 ){
        *pzErr = sqlite3_mprintf("prefix length out of range (max 999)");
        rc = SQLITE_ERROR;
        break;
      }

      pConfig->aPrefix[pConfig->nPrefix] = nPre;
      pConfig->nPrefix++;
      bFirst = 0;
    }
    assert( pConfig->nPrefix<=FTS5_MAX_PREFIX_INDEXES );
    return rc;
  }

  if( sqlite3_strnicmp("tokenize", zCmd, nCmd)==0 ){
    const char *p = (const char*)zArg;
    sqlite3_int64 nArg = strlen(zArg) + 1;
    char **azArg = sqlite3Fts5MallocZero(&rc, sizeof(char*) * nArg);
    char *pDel = sqlite3Fts5MallocZero(&rc, nArg * 2);
    char *pSpace = pDel;

    if( azArg && pSpace ){
      if( pConfig->pTok ){
        *pzErr = sqlite3_mprintf("multiple tokenize=... directives");
        rc = SQLITE_ERROR;
      }else{
        for(nArg=0; p && *p; nArg++){
          const char *p2 = fts5ConfigSkipWhitespace(p);
          if( *p2=='\'' ){
            p = fts5ConfigSkipLiteral(p2);
          }else{
            p = fts5ConfigSkipBareword(p2);
          }
          if( p ){
            memcpy(pSpace, p2, p-p2);
            azArg[nArg] = pSpace;
            sqlite3Fts5Dequote(pSpace);
            pSpace += (p - p2) + 1;
            p = fts5ConfigSkipWhitespace(p);
          }
        }
        if( p==0 ){
          *pzErr = sqlite3_mprintf("parse error in tokenize directive");
          rc = SQLITE_ERROR;
        }else{
          rc = sqlite3Fts5GetTokenizer(pGlobal, 
              (const char**)azArg, (int)nArg, pConfig,
              pzErr
          );
        }
      }
    }

    sqlite3_free(azArg);
    sqlite3_free(pDel);
    return rc;
  }

  if( sqlite3_strnicmp("content", zCmd, nCmd)==0 ){
    if( pConfig->eContent!=FTS5_CONTENT_NORMAL ){
      *pzErr = sqlite3_mprintf("multiple content=... directives");
      rc = SQLITE_ERROR;
    }else{
      if( zArg[0] ){
        pConfig->eContent = FTS5_CONTENT_EXTERNAL;
        pConfig->zContent = sqlite3Fts5Mprintf(&rc, "%Q.%Q", pConfig->zDb,zArg);
      }else{
        pConfig->eContent = FTS5_CONTENT_NONE;
      }
    }
    return rc;
  }

  if( sqlite3_strnicmp("content_rowid", zCmd, nCmd)==0 ){
    if( pConfig->zContentRowid ){
      *pzErr = sqlite3_mprintf("multiple content_rowid=... directives");
      rc = SQLITE_ERROR;
    }else{
      pConfig->zContentRowid = sqlite3Fts5Strndup(&rc, zArg, -1);
    }
    return rc;
  }

  if( sqlite3_strnicmp("columnsize", zCmd, nCmd)==0 ){
    if( (zArg[0]!='0' && zArg[0]!='1') || zArg[1]!='\0' ){
      *pzErr = sqlite3_mprintf("malformed columnsize=... directive");
      rc = SQLITE_ERROR;
    }else{
      pConfig->bColumnsize = (zArg[0]=='1');
    }
    return rc;
  }

  if( sqlite3_strnicmp("detail", zCmd, nCmd)==0 ){
    const Fts5Enum aDetail[] = {
      { "none", FTS5_DETAIL_NONE },
      { "full", FTS5_DETAIL_FULL },
      { "columns", FTS5_DETAIL_COLUMNS },
      { 0, 0 }
    };

    if( (rc = fts5ConfigSetEnum(aDetail, zArg, &pConfig->eDetail)) ){
      *pzErr = sqlite3_mprintf("malformed detail=... directive");
    }
    return rc;
  }

  *pzErr = sqlite3_mprintf("unrecognized option: \"%.*s\"", nCmd, zCmd);
  return SQLITE_ERROR;
}

/*
** Allocate an instance of the default tokenizer ("simple") at 
** Fts5Config.pTokenizer. Return SQLITE_OK if successful, or an SQLite error
** code if an error occurs.
*/
static int fts5ConfigDefaultTokenizer(Fts5Global *pGlobal, Fts5Config *pConfig){
  assert( pConfig->pTok==0 && pConfig->pTokApi==0 );
  return sqlite3Fts5GetTokenizer(pGlobal, 0, 0, pConfig, 0);
}

/*
** Gobble up the first bareword or quoted word from the input buffer zIn.
** Return a pointer to the character immediately following the last in
** the gobbled word if successful, or a NULL pointer otherwise (failed
** to find close-quote character).
**
** Before returning, set pzOut to point to a new buffer containing a
** nul-terminated, dequoted copy of the gobbled word. If the word was
** quoted, *pbQuoted is also set to 1 before returning.
**
** If *pRc is other than SQLITE_OK when this function is called, it is
** a no-op (NULL is returned). Otherwise, if an OOM occurs within this
** function, *pRc is set to SQLITE_NOMEM before returning. *pRc is *not*
** set if a parse error (failed to find close quote) occurs.
*/
static const char *fts5ConfigGobbleWord(
  int *pRc,                       /* IN/OUT: Error code */
  const char *zIn,                /* Buffer to gobble string/bareword from */
  char **pzOut,                   /* OUT: malloc'd buffer containing str/bw */
  int *pbQuoted                   /* OUT: Set to true if dequoting required */
){
  const char *zRet = 0;

  sqlite3_int64 nIn = strlen(zIn);
  char *zOut = sqlite3_malloc64(nIn+1);

  assert( *pRc==SQLITE_OK );
  *pbQuoted = 0;
  *pzOut = 0;

  if( zOut==0 ){
    *pRc = SQLITE_NOMEM;
  }else{
    memcpy(zOut, zIn, (size_t)(nIn+1));
    if( fts5_isopenquote(zOut[0]) ){
      int ii = fts5Dequote(zOut);
      zRet = &zIn[ii];
      *pbQuoted = 1;
    }else{
      zRet = fts5ConfigSkipBareword(zIn);
      if( zRet ){
        zOut[zRet-zIn] = '\0';
      }
    }
  }

  if( zRet==0 ){
    sqlite3_free(zOut);
  }else{
    *pzOut = zOut;
  }

  return zRet;
}

static int fts5ConfigParseColumn(
  Fts5Config *p, 
  char *zCol, 
  char *zArg, 
  char **pzErr
){
  int rc = SQLITE_OK;
  if( 0==sqlite3_stricmp(zCol, FTS5_RANK_NAME) 
   || 0==sqlite3_stricmp(zCol, FTS5_ROWID_NAME) 
  ){
    *pzErr = sqlite3_mprintf("reserved fts5 column name: %s", zCol);
    rc = SQLITE_ERROR;
  }else if( zArg ){
    if( 0==sqlite3_stricmp(zArg, "unindexed") ){
      p->abUnindexed[p->nCol] = 1;
    }else{
      *pzErr = sqlite3_mprintf("unrecognized column option: %s", zArg);
      rc = SQLITE_ERROR;
    }
  }

  p->azCol[p->nCol++] = zCol;
  return rc;
}

/*
** Populate the Fts5Config.zContentExprlist string.
*/
static int fts5ConfigMakeExprlist(Fts5Config *p){
  int i;
  int rc = SQLITE_OK;
  Fts5Buffer buf = {0, 0, 0};

  sqlite3Fts5BufferAppendPrintf(&rc, &buf, "T.%Q", p->zContentRowid);
  if( p->eContent!=FTS5_CONTENT_NONE ){
    for(i=0; i<p->nCol; i++){
      if( p->eContent==FTS5_CONTENT_EXTERNAL ){
        sqlite3Fts5BufferAppendPrintf(&rc, &buf, ", T.%Q", p->azCol[i]);
      }else{
        sqlite3Fts5BufferAppendPrintf(&rc, &buf, ", T.c%d", i);
      }
    }
  }

  assert( p->zContentExprlist==0 );
  p->zContentExprlist = (char*)buf.p;
  return rc;
}

/*
** Arguments nArg/azArg contain the string arguments passed to the xCreate
** or xConnect method of the virtual table. This function attempts to 
** allocate an instance of Fts5Config containing the results of parsing
** those arguments.
**
** If successful, SQLITE_OK is returned and *ppOut is set to point to the
** new Fts5Config object. If an error occurs, an SQLite error code is 
** returned, *ppOut is set to NULL and an error message may be left in
** *pzErr. It is the responsibility of the caller to eventually free any 
** such error message using sqlite3_free().
*/
static int sqlite3Fts5ConfigParse(
  Fts5Global *pGlobal,
  sqlite3 *db,
  int nArg,                       /* Number of arguments */
  const char **azArg,             /* Array of nArg CREATE VIRTUAL TABLE args */
  Fts5Config **ppOut,             /* OUT: Results of parse */
  char **pzErr                    /* OUT: Error message */
){
  int rc = SQLITE_OK;             /* Return code */
  Fts5Config *pRet;               /* New object to return */
  int i;
  sqlite3_int64 nByte;

  *ppOut = pRet = (Fts5Config*)sqlite3_malloc(sizeof(Fts5Config));
  if( pRet==0 ) return SQLITE_NOMEM;
  memset(pRet, 0, sizeof(Fts5Config));
  pRet->db = db;
  pRet->iCookie = -1;

  nByte = nArg * (sizeof(char*) + sizeof(u8));
  pRet->azCol = (char**)sqlite3Fts5MallocZero(&rc, nByte);
  pRet->abUnindexed = pRet->azCol ? (u8*)&pRet->azCol[nArg] : 0;
  pRet->zDb = sqlite3Fts5Strndup(&rc, azArg[1], -1);
  pRet->zName = sqlite3Fts5Strndup(&rc, azArg[2], -1);
  pRet->bColumnsize = 1;
  pRet->eDetail = FTS5_DETAIL_FULL;
#ifdef SQLITE_DEBUG
  pRet->bPrefixIndex = 1;
#endif
  if( rc==SQLITE_OK && sqlite3_stricmp(pRet->zName, FTS5_RANK_NAME)==0 ){
    *pzErr = sqlite3_mprintf("reserved fts5 table name: %s", pRet->zName);
    rc = SQLITE_ERROR;
  }

  for(i=3; rc==SQLITE_OK && i<nArg; i++){
    const char *zOrig = azArg[i];
    const char *z;
    char *zOne = 0;
    char *zTwo = 0;
    int bOption = 0;
    int bMustBeCol = 0;

    z = fts5ConfigGobbleWord(&rc, zOrig, &zOne, &bMustBeCol);
    z = fts5ConfigSkipWhitespace(z);
    if( z && *z=='=' ){
      bOption = 1;
      assert( zOne!=0 );
      z++;
      if( bMustBeCol ) z = 0;
    }
    z = fts5ConfigSkipWhitespace(z);
    if( z && z[0] ){
      int bDummy;
      z = fts5ConfigGobbleWord(&rc, z, &zTwo, &bDummy);
      if( z && z[0] ) z = 0;
    }

    if( rc==SQLITE_OK ){
      if( z==0 ){
        *pzErr = sqlite3_mprintf("parse error in \"%s\"", zOrig);
        rc = SQLITE_ERROR;
      }else{
        if( bOption ){
          rc = fts5ConfigParseSpecial(pGlobal, pRet, 
            ALWAYS(zOne)?zOne:"",
            zTwo?zTwo:"",
            pzErr
          );
        }else{
          rc = fts5ConfigParseColumn(pRet, zOne, zTwo, pzErr);
          zOne = 0;
        }
      }
    }

    sqlite3_free(zOne);
    sqlite3_free(zTwo);
  }

  /* If a tokenizer= option was successfully parsed, the tokenizer has
  ** already been allocated. Otherwise, allocate an instance of the default
  ** tokenizer (unicode61) now.  */
  if( rc==SQLITE_OK && pRet->pTok==0 ){
    rc = fts5ConfigDefaultTokenizer(pGlobal, pRet);
  }

  /* If no zContent option was specified, fill in the default values. */
  if( rc==SQLITE_OK && pRet->zContent==0 ){
    const char *zTail = 0;
    assert( pRet->eContent==FTS5_CONTENT_NORMAL 
         || pRet->eContent==FTS5_CONTENT_NONE 
    );
    if( pRet->eContent==FTS5_CONTENT_NORMAL ){
      zTail = "content";
    }else if( pRet->bColumnsize ){
      zTail = "docsize";
    }

    if( zTail ){
      pRet->zContent = sqlite3Fts5Mprintf(
          &rc, "%Q.'%q_%s'", pRet->zDb, pRet->zName, zTail
      );
    }
  }

  if( rc==SQLITE_OK && pRet->zContentRowid==0 ){
    pRet->zContentRowid = sqlite3Fts5Strndup(&rc, "rowid", -1);
  }

  /* Formulate the zContentExprlist text */
  if( rc==SQLITE_OK ){
    rc = fts5ConfigMakeExprlist(pRet);
  }

  if( rc!=SQLITE_OK ){
    sqlite3Fts5ConfigFree(pRet);
    *ppOut = 0;
  }
  return rc;
}

/*
** Free the configuration object passed as the only argument.
*/
static void sqlite3Fts5ConfigFree(Fts5Config *pConfig){
  if( pConfig ){
    int i;
    if( pConfig->pTok ){
      pConfig->pTokApi->xDelete(pConfig->pTok);
    }
    sqlite3_free(pConfig->zDb);
    sqlite3_free(pConfig->zName);
    for(i=0; i<pConfig->nCol; i++){
      sqlite3_free(pConfig->azCol[i]);
    }
    sqlite3_free(pConfig->azCol);
    sqlite3_free(pConfig->aPrefix);
    sqlite3_free(pConfig->zRank);
    sqlite3_free(pConfig->zRankArgs);
    sqlite3_free(pConfig->zContent);
    sqlite3_free(pConfig->zContentRowid);
    sqlite3_free(pConfig->zContentExprlist);
    sqlite3_free(pConfig);
  }
}

/*
** Call sqlite3_declare_vtab() based on the contents of the configuration
** object passed as the only argument. Return SQLITE_OK if successful, or
** an SQLite error code if an error occurs.
*/
static int sqlite3Fts5ConfigDeclareVtab(Fts5Config *pConfig){
  int i;
  int rc = SQLITE_OK;
  char *zSql;

  zSql = sqlite3Fts5Mprintf(&rc, "CREATE TABLE x(");
  for(i=0; zSql && i<pConfig->nCol; i++){
    const char *zSep = (i==0?"":", ");
    zSql = sqlite3Fts5Mprintf(&rc, "%z%s%Q", zSql, zSep, pConfig->azCol[i]);
  }
  zSql = sqlite3Fts5Mprintf(&rc, "%z, %Q HIDDEN, %s HIDDEN)", 
      zSql, pConfig->zName, FTS5_RANK_NAME
  );

  assert( zSql || rc==SQLITE_NOMEM );
  if( zSql ){
    rc = sqlite3_declare_vtab(pConfig->db, zSql);
    sqlite3_free(zSql);
  }
 
  return rc;
}

/*
** Tokenize the text passed via the second and third arguments.
**
** The callback is invoked once for each token in the input text. The
** arguments passed to it are, in order:
**
**     void *pCtx          // Copy of 4th argument to sqlite3Fts5Tokenize()
**     const char *pToken  // Pointer to buffer containing token
**     int nToken          // Size of token in bytes
**     int iStart          // Byte offset of start of token within input text
**     int iEnd            // Byte offset of end of token within input text
**     int iPos            // Position of token in input (first token is 0)
**
** If the callback returns a non-zero value the tokenization is abandoned
** and no further callbacks are issued. 
**
** This function returns SQLITE_OK if successful or an SQLite error code
** if an error occurs. If the tokenization was abandoned early because
** the callback returned SQLITE_DONE, this is not an error and this function
** still returns SQLITE_OK. Or, if the tokenization was abandoned early
** because the callback returned another non-zero value, it is assumed
** to be an SQLite error code and returned to the caller.
*/
static int sqlite3Fts5Tokenize(
  Fts5Config *pConfig,            /* FTS5 Configuration object */
  int flags,                      /* FTS5_TOKENIZE_* flags */
  const char *pText, int nText,   /* Text to tokenize */
  void *pCtx,                     /* Context passed to xToken() */
  int (*xToken)(void*, int, const char*, int, int, int)    /* Callback */
){
  if( pText==0 ) return SQLITE_OK;
  return pConfig->pTokApi->xTokenize(
      pConfig->pTok, pCtx, flags, pText, nText, xToken
  );
}

/*
** Argument pIn points to the first character in what is expected to be
** a comma-separated list of SQL literals followed by a ')' character.
** If it actually is this, return a pointer to the ')'. Otherwise, return
** NULL to indicate a parse error.
*/
static const char *fts5ConfigSkipArgs(const char *pIn){
  const char *p = pIn;
  
  while( 1 ){
    p = fts5ConfigSkipWhitespace(p);
    p = fts5ConfigSkipLiteral(p);
    p = fts5ConfigSkipWhitespace(p);
    if( p==0 || *p==')' ) break;
    if( *p!=',' ){
      p = 0;
      break;
    }
    p++;
  }

  return p;
}

/*
** Parameter zIn contains a rank() function specification. The format of 
** this is:
**
**   + Bareword (function name)
**   + Open parenthesis - "("
**   + Zero or more SQL literals in a comma separated list
**   + Close parenthesis - ")"
*/
static int sqlite3Fts5ConfigParseRank(
  const char *zIn,                /* Input string */
  char **pzRank,                  /* OUT: Rank function name */
  char **pzRankArgs               /* OUT: Rank function arguments */
){
  const char *p = zIn;
  const char *pRank;
  char *zRank = 0;
  char *zRankArgs = 0;
  int rc = SQLITE_OK;

  *pzRank = 0;
  *pzRankArgs = 0;

  if( p==0 ){
    rc = SQLITE_ERROR;
  }else{
    p = fts5ConfigSkipWhitespace(p);
    pRank = p;
    p = fts5ConfigSkipBareword(p);

    if( p ){
      zRank = sqlite3Fts5MallocZero(&rc, 1 + p - pRank);
      if( zRank ) memcpy(zRank, pRank, p-pRank);
    }else{
      rc = SQLITE_ERROR;
    }

    if( rc==SQLITE_OK ){
      p = fts5ConfigSkipWhitespace(p);
      if( *p!='(' ) rc = SQLITE_ERROR;
      p++;
    }
    if( rc==SQLITE_OK ){
      const char *pArgs; 
      p = fts5ConfigSkipWhitespace(p);
      pArgs = p;
      if( *p!=')' ){
        p = fts5ConfigSkipArgs(p);
        if( p==0 ){
          rc = SQLITE_ERROR;
        }else{
          zRankArgs = sqlite3Fts5MallocZero(&rc, 1 + p - pArgs);
          if( zRankArgs ) memcpy(zRankArgs, pArgs, p-pArgs);
        }
      }
    }
  }

  if( rc!=SQLITE_OK ){
    sqlite3_free(zRank);
    assert( zRankArgs==0 );
  }else{
    *pzRank = zRank;
    *pzRankArgs = zRankArgs;
  }
  return rc;
}

static int sqlite3Fts5ConfigSetValue(
  Fts5Config *pConfig, 
  const char *zKey, 
  sqlite3_value *pVal,
  int *pbBadkey
){
  int rc = SQLITE_OK;

  if( 0==sqlite3_stricmp(zKey, "pgsz") ){
    int pgsz = 0;
    if( SQLITE_INTEGER==sqlite3_value_numeric_type(pVal) ){
      pgsz = sqlite3_value_int(pVal);
    }
    if( pgsz<32 || pgsz>FTS5_MAX_PAGE_SIZE ){
      *pbBadkey = 1;
    }else{
      pConfig->pgsz = pgsz;
    }
  }

  else if( 0==sqlite3_stricmp(zKey, "hashsize") ){
    int nHashSize = -1;
    if( SQLITE_INTEGER==sqlite3_value_numeric_type(pVal) ){
      nHashSize = sqlite3_value_int(pVal);
    }
    if( nHashSize<=0 ){
      *pbBadkey = 1;
    }else{
      pConfig->nHashSize = nHashSize;
    }
  }

  else if( 0==sqlite3_stricmp(zKey, "automerge") ){
    int nAutomerge = -1;
    if( SQLITE_INTEGER==sqlite3_value_numeric_type(pVal) ){
      nAutomerge = sqlite3_value_int(pVal);
    }
    if( nAutomerge<0 || nAutomerge>64 ){
      *pbBadkey = 1;
    }else{
      if( nAutomerge==1 ) nAutomerge = FTS5_DEFAULT_AUTOMERGE;
      pConfig->nAutomerge = nAutomerge;
    }
  }

  else if( 0==sqlite3_stricmp(zKey, "usermerge") ){
    int nUsermerge = -1;
    if( SQLITE_INTEGER==sqlite3_value_numeric_type(pVal) ){
      nUsermerge = sqlite3_value_int(pVal);
    }
    if( nUsermerge<2 || nUsermerge>16 ){
      *pbBadkey = 1;
    }else{
      pConfig->nUsermerge = nUsermerge;
    }
  }

  else if( 0==sqlite3_stricmp(zKey, "crisismerge") ){
    int nCrisisMerge = -1;
    if( SQLITE_INTEGER==sqlite3_value_numeric_type(pVal) ){
      nCrisisMerge = sqlite3_value_int(pVal);
    }
    if( nCrisisMerge<0 ){
      *pbBadkey = 1;
    }else{
      if( nCrisisMerge<=1 ) nCrisisMerge = FTS5_DEFAULT_CRISISMERGE;
      if( nCrisisMerge>=FTS5_MAX_SEGMENT ) nCrisisMerge = FTS5_MAX_SEGMENT-1;
      pConfig->nCrisisMerge = nCrisisMerge;
    }
  }

  else if( 0==sqlite3_stricmp(zKey, "rank") ){
    const char *zIn = (const char*)sqlite3_value_text(pVal);
    char *zRank;
    char *zRankArgs;
    rc = sqlite3Fts5ConfigParseRank(zIn, &zRank, &zRankArgs);
    if( rc==SQLITE_OK ){
      sqlite3_free(pConfig->zRank);
      sqlite3_free(pConfig->zRankArgs);
      pConfig->zRank = zRank;
      pConfig->zRankArgs = zRankArgs;
    }else if( rc==SQLITE_ERROR ){
      rc = SQLITE_OK;
      *pbBadkey = 1;
    }
  }else{
    *pbBadkey = 1;
  }
  return rc;
}

/*
** Load the contents of the %_config table into memory.
*/
static int sqlite3Fts5ConfigLoad(Fts5Config *pConfig, int iCookie){
  const char *zSelect = "SELECT k, v FROM %Q.'%q_config'";
  char *zSql;
  sqlite3_stmt *p = 0;
  int rc = SQLITE_OK;
  int iVersion = 0;

  /* Set default values */
  pConfig->pgsz = FTS5_DEFAULT_PAGE_SIZE;
  pConfig->nAutomerge = FTS5_DEFAULT_AUTOMERGE;
  pConfig->nUsermerge = FTS5_DEFAULT_USERMERGE;
  pConfig->nCrisisMerge = FTS5_DEFAULT_CRISISMERGE;
  pConfig->nHashSize = FTS5_DEFAULT_HASHSIZE;

  zSql = sqlite3Fts5Mprintf(&rc, zSelect, pConfig->zDb, pConfig->zName);
  if( zSql ){
    rc = sqlite3_prepare_v2(pConfig->db, zSql, -1, &p, 0);
    sqlite3_free(zSql);
  }

  assert( rc==SQLITE_OK || p==0 );
  if( rc==SQLITE_OK ){
    while( SQLITE_ROW==sqlite3_step(p) ){
      const char *zK = (const char*)sqlite3_column_text(p, 0);
      sqlite3_value *pVal = sqlite3_column_value(p, 1);
      if( 0==sqlite3_stricmp(zK, "version") ){
        iVersion = sqlite3_value_int(pVal);
      }else{
        int bDummy = 0;
        sqlite3Fts5ConfigSetValue(pConfig, zK, pVal, &bDummy);
      }
    }
    rc = sqlite3_finalize(p);
  }
  
  if( rc==SQLITE_OK && iVersion!=FTS5_CURRENT_VERSION ){
    rc = SQLITE_ERROR;
    if( pConfig->pzErrmsg ){
      assert( 0==*pConfig->pzErrmsg );
      *pConfig->pzErrmsg = sqlite3_mprintf(
          "invalid fts5 file format (found %d, expected %d) - run 'rebuild'",
          iVersion, FTS5_CURRENT_VERSION
      );
    }
  }

  if( rc==SQLITE_OK ){
    pConfig->iCookie = iCookie;
  }
  return rc;
}

#line 1 "fts5_expr.c"
/*
** 2014 May 31
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
******************************************************************************
**
*/



/* #include "fts5Int.h" */
/* #include "fts5parse.h" */

/*
** All token types in the generated fts5parse.h file are greater than 0.
*/
#define FTS5_EOF 0

#define FTS5_LARGEST_INT64  (0xffffffff|(((i64)0x7fffffff)<<32))

typedef struct Fts5ExprTerm Fts5ExprTerm;

/*
** Functions generated by lemon from fts5parse.y.
*/
static void *sqlite3Fts5ParserAlloc(void *(*mallocProc)(u64));
static void sqlite3Fts5ParserFree(void*, void (*freeProc)(void*));
static void sqlite3Fts5Parser(void*, int, Fts5Token, Fts5Parse*);
#ifndef NDEBUG
static void sqlite3Fts5ParserTrace(FILE*, char*);
#endif
static int sqlite3Fts5ParserFallback(int);


struct Fts5Expr {
  Fts5Index *pIndex;
  Fts5Config *pConfig;
  Fts5ExprNode *pRoot;
  int bDesc;                      /* Iterate in descending rowid order */
  int nPhrase;                    /* Number of phrases in expression */
  Fts5ExprPhrase **apExprPhrase;  /* Pointers to phrase objects */
};

/*
** eType:
**   Expression node type. Always one of:
**
**       FTS5_AND                 (nChild, apChild valid)
**       FTS5_OR                  (nChild, apChild valid)
**       FTS5_NOT                 (nChild, apChild valid)
**       FTS5_STRING              (pNear valid)
**       FTS5_TERM                (pNear valid)
*/
struct Fts5ExprNode {
  int eType;                      /* Node type */
  int bEof;                       /* True at EOF */
  int bNomatch;                   /* True if entry is not a match */

  /* Next method for this node. */
  int (*xNext)(Fts5Expr*, Fts5ExprNode*, int, i64);

  i64 iRowid;                     /* Current rowid */
  Fts5ExprNearset *pNear;         /* For FTS5_STRING - cluster of phrases */

  /* Child nodes. For a NOT node, this array always contains 2 entries. For 
  ** AND or OR nodes, it contains 2 or more entries.  */
  int nChild;                     /* Number of child nodes */
  Fts5ExprNode *apChild[1];       /* Array of child nodes */
};

#define Fts5NodeIsString(p) ((p)->eType==FTS5_TERM || (p)->eType==FTS5_STRING)

/*
** Invoke the xNext method of an Fts5ExprNode object. This macro should be
** used as if it has the same signature as the xNext() methods themselves.
*/
#define fts5ExprNodeNext(a,b,c,d) (b)->xNext((a), (b), (c), (d))

/*
** An instance of the following structure represents a single search term
** or term prefix.
*/
struct Fts5ExprTerm {
  u8 bPrefix;                     /* True for a prefix term */
  u8 bFirst;                      /* True if token must be first in column */
  char *zTerm;                    /* nul-terminated term */
  Fts5IndexIter *pIter;           /* Iterator for this term */
  Fts5ExprTerm *pSynonym;         /* Pointer to first in list of synonyms */
};

/*
** A phrase. One or more terms that must appear in a contiguous sequence
** within a document for it to match.
*/
struct Fts5ExprPhrase {
  Fts5ExprNode *pNode;            /* FTS5_STRING node this phrase is part of */
  Fts5Buffer poslist;             /* Current position list */
  int nTerm;                      /* Number of entries in aTerm[] */
  Fts5ExprTerm aTerm[1];          /* Terms that make up this phrase */
};

/*
** One or more phrases that must appear within a certain token distance of
** each other within each matching document.
*/
struct Fts5ExprNearset {
  int nNear;                      /* NEAR parameter */
  Fts5Colset *pColset;            /* Columns to search (NULL -> all columns) */
  int nPhrase;                    /* Number of entries in aPhrase[] array */
  Fts5ExprPhrase *apPhrase[1];    /* Array of phrase pointers */
};


/*
** Parse context.
*/
struct Fts5Parse {
  Fts5Config *pConfig;
  char *zErr;
  int rc;
  int nPhrase;                    /* Size of apPhrase array */
  Fts5ExprPhrase **apPhrase;      /* Array of all phrases */
  Fts5ExprNode *pExpr;            /* Result of a successful parse */
  int bPhraseToAnd;               /* Convert "a+b" to "a AND b" */
};

static void sqlite3Fts5ParseError(Fts5Parse *pParse, const char *zFmt, ...){
  va_list ap;
  va_start(ap, zFmt);
  if( pParse->rc==SQLITE_OK ){
    assert( pParse->zErr==0 );
    pParse->zErr = sqlite3_vmprintf(zFmt, ap);
    pParse->rc = SQLITE_ERROR;
  }
  va_end(ap);
}

static int fts5ExprIsspace(char t){
  return t==' ' || t=='\t' || t=='\n' || t=='\r';
}

/*
** Read the first token from the nul-terminated string at *pz.
*/
static int fts5ExprGetToken(
  Fts5Parse *pParse, 
  const char **pz,                /* IN/OUT: Pointer into buffer */
  Fts5Token *pToken
){
  const char *z = *pz;
  int tok;

  /* Skip past any whitespace */
  while( fts5ExprIsspace(*z) ) z++;

  pToken->p = z;
  pToken->n = 1;
  switch( *z ){
    case '(':  tok = FTS5_LP;    break;
    case ')':  tok = FTS5_RP;    break;
    case '{':  tok = FTS5_LCP;   break;
    case '}':  tok = FTS5_RCP;   break;
    case ':':  tok = FTS5_COLON; break;
    case ',':  tok = FTS5_COMMA; break;
    case '+':  tok = FTS5_PLUS;  break;
    case '*':  tok = FTS5_STAR;  break;
    case '-':  tok = FTS5_MINUS; break;
    case '^':  tok = FTS5_CARET; break;
    case '\0': tok = FTS5_EOF;   break;

    case '"': {
      const char *z2;
      tok = FTS5_STRING;

      for(z2=&z[1]; 1; z2++){
        if( z2[0]=='"' ){
          z2++;
          if( z2[0]!='"' ) break;
        }
        if( z2[0]=='\0' ){
          sqlite3Fts5ParseError(pParse, "unterminated string");
          return FTS5_EOF;
        }
      }
      pToken->n = (z2 - z);
      break;
    }

    default: {
      const char *z2;
      if( sqlite3Fts5IsBareword(z[0])==0 ){
        sqlite3Fts5ParseError(pParse, "fts5: syntax error near \"%.1s\"", z);
        return FTS5_EOF;
      }
      tok = FTS5_STRING;
      for(z2=&z[1]; sqlite3Fts5IsBareword(*z2); z2++);
      pToken->n = (z2 - z);
      if( pToken->n==2 && memcmp(pToken->p, "OR", 2)==0 )  tok = FTS5_OR;
      if( pToken->n==3 && memcmp(pToken->p, "NOT", 3)==0 ) tok = FTS5_NOT;
      if( pToken->n==3 && memcmp(pToken->p, "AND", 3)==0 ) tok = FTS5_AND;
      break;
    }
  }

  *pz = &pToken->p[pToken->n];
  return tok;
}

static void *fts5ParseAlloc(u64 t){ return sqlite3_malloc64((sqlite3_int64)t);}
static void fts5ParseFree(void *p){ sqlite3_free(p); }

static int sqlite3Fts5ExprNew(
  Fts5Config *pConfig,            /* FTS5 Configuration */
  int bPhraseToAnd,
  int iCol,
  const char *zExpr,              /* Expression text */
  Fts5Expr **ppNew, 
  char **pzErr
){
  Fts5Parse sParse;
  Fts5Token token;
  const char *z = zExpr;
  int t;                          /* Next token type */
  void *pEngine;
  Fts5Expr *pNew;

  *ppNew = 0;
  *pzErr = 0;
  memset(&sParse, 0, sizeof(sParse));
  sParse.bPhraseToAnd = bPhraseToAnd;
  pEngine = sqlite3Fts5ParserAlloc(fts5ParseAlloc);
  if( pEngine==0 ){ return SQLITE_NOMEM; }
  sParse.pConfig = pConfig;

  do {
    t = fts5ExprGetToken(&sParse, &z, &token);
    sqlite3Fts5Parser(pEngine, t, token, &sParse);
  }while( sParse.rc==SQLITE_OK && t!=FTS5_EOF );
  sqlite3Fts5ParserFree(pEngine, fts5ParseFree);

  /* If the LHS of the MATCH expression was a user column, apply the
  ** implicit column-filter.  */
  if( iCol<pConfig->nCol && sParse.pExpr && sParse.rc==SQLITE_OK ){
    int n = sizeof(Fts5Colset);
    Fts5Colset *pColset = (Fts5Colset*)sqlite3Fts5MallocZero(&sParse.rc, n);
    if( pColset ){
      pColset->nCol = 1;
      pColset->aiCol[0] = iCol;
      sqlite3Fts5ParseSetColset(&sParse, sParse.pExpr, pColset);
    }
  }

  assert( sParse.rc!=SQLITE_OK || sParse.zErr==0 );
  if( sParse.rc==SQLITE_OK ){
    *ppNew = pNew = sqlite3_malloc(sizeof(Fts5Expr));
    if( pNew==0 ){
      sParse.rc = SQLITE_NOMEM;
      sqlite3Fts5ParseNodeFree(sParse.pExpr);
    }else{
      if( !sParse.pExpr ){
        const int nByte = sizeof(Fts5ExprNode);
        pNew->pRoot = (Fts5ExprNode*)sqlite3Fts5MallocZero(&sParse.rc, nByte);
        if( pNew->pRoot ){
          pNew->pRoot->bEof = 1;
        }
      }else{
        pNew->pRoot = sParse.pExpr;
      }
      pNew->pIndex = 0;
      pNew->pConfig = pConfig;
      pNew->apExprPhrase = sParse.apPhrase;
      pNew->nPhrase = sParse.nPhrase;
      pNew->bDesc = 0;
      sParse.apPhrase = 0;
    }
  }else{
    sqlite3Fts5ParseNodeFree(sParse.pExpr);
  }

  sqlite3_free(sParse.apPhrase);
  *pzErr = sParse.zErr;
  return sParse.rc;
}

/*
** This function is only called when using the special 'trigram' tokenizer.
** Argument zText contains the text of a LIKE or GLOB pattern matched
** against column iCol. This function creates and compiles an FTS5 MATCH
** expression that will match a superset of the rows matched by the LIKE or
** GLOB. If successful, SQLITE_OK is returned. Otherwise, an SQLite error
** code.
*/
static int sqlite3Fts5ExprPattern(
  Fts5Config *pConfig, int bGlob, int iCol, const char *zText, Fts5Expr **pp
){
  i64 nText = strlen(zText);
  char *zExpr = (char*)sqlite3_malloc64(nText*4 + 1);
  int rc = SQLITE_OK;

  if( zExpr==0 ){
    rc = SQLITE_NOMEM;
  }else{
    char aSpec[3];
    int iOut = 0;
    int i = 0;
    int iFirst = 0;

    if( bGlob==0 ){
      aSpec[0] = '_';
      aSpec[1] = '%';
      aSpec[2] = 0;
    }else{
      aSpec[0] = '*';
      aSpec[1] = '?';
      aSpec[2] = '[';
    }

    while( i<=nText ){
      if( i==nText 
       || zText[i]==aSpec[0] || zText[i]==aSpec[1] || zText[i]==aSpec[2] 
      ){
        if( i-iFirst>=3 ){
          int jj;
          zExpr[iOut++] = '"';
          for(jj=iFirst; jj<i; jj++){
            zExpr[iOut++] = zText[jj];
            if( zText[jj]=='"' ) zExpr[iOut++] = '"';
          }
          zExpr[iOut++] = '"';
          zExpr[iOut++] = ' ';
        }
        if( zText[i]==aSpec[2] ){
          i += 2;
          if( zText[i-1]=='^' ) i++;
          while( i<nText && zText[i]!=']' ) i++;
        }
        iFirst = i+1;
      }
      i++;
    }
    if( iOut>0 ){
      int bAnd = 0;
      if( pConfig->eDetail!=FTS5_DETAIL_FULL ){
        bAnd = 1;
        if( pConfig->eDetail==FTS5_DETAIL_NONE ){
          iCol = pConfig->nCol;
        }
      }
      zExpr[iOut] = '\0';
      rc = sqlite3Fts5ExprNew(pConfig, bAnd, iCol, zExpr, pp,pConfig->pzErrmsg);
    }else{
      *pp = 0;
    }
    sqlite3_free(zExpr);
  }

  return rc;
}

/*
** Free the expression node object passed as the only argument.
*/
static void sqlite3Fts5ParseNodeFree(Fts5ExprNode *p){
  if( p ){
    int i;
    for(i=0; i<p->nChild; i++){
      sqlite3Fts5ParseNodeFree(p->apChild[i]);
    }
    sqlite3Fts5ParseNearsetFree(p->pNear);
    sqlite3_free(p);
  }
}

/*
** Free the expression object passed as the only argument.
*/
static void sqlite3Fts5ExprFree(Fts5Expr *p){
  if( p ){
    sqlite3Fts5ParseNodeFree(p->pRoot);
    sqlite3_free(p->apExprPhrase);
    sqlite3_free(p);
  }
}

static int sqlite3Fts5ExprAnd(Fts5Expr **pp1, Fts5Expr *p2){
  Fts5Parse sParse;
  memset(&sParse, 0, sizeof(sParse));

  if( *pp1 ){
    Fts5Expr *p1 = *pp1;
    int nPhrase = p1->nPhrase + p2->nPhrase;

    p1->pRoot = sqlite3Fts5ParseNode(&sParse, FTS5_AND, p1->pRoot, p2->pRoot,0);
    p2->pRoot = 0;

    if( sParse.rc==SQLITE_OK ){
      Fts5ExprPhrase **ap = (Fts5ExprPhrase**)sqlite3_realloc(
          p1->apExprPhrase, nPhrase * sizeof(Fts5ExprPhrase*)
      );
      if( ap==0 ){
        sParse.rc = SQLITE_NOMEM;
      }else{
        int i;
        memmove(&ap[p2->nPhrase], ap, p1->nPhrase*sizeof(Fts5ExprPhrase*));
        for(i=0; i<p2->nPhrase; i++){
          ap[i] = p2->apExprPhrase[i];
        }
        p1->nPhrase = nPhrase;
        p1->apExprPhrase = ap;
      }
    }
    sqlite3_free(p2->apExprPhrase);
    sqlite3_free(p2);
  }else{
    *pp1 = p2;
  }

  return sParse.rc;
}

/*
** Argument pTerm must be a synonym iterator. Return the current rowid
** that it points to.
*/
static i64 fts5ExprSynonymRowid(Fts5ExprTerm *pTerm, int bDesc, int *pbEof){
  i64 iRet = 0;
  int bRetValid = 0;
  Fts5ExprTerm *p;

  assert( pTerm );
  assert( pTerm->pSynonym );
  assert( bDesc==0 || bDesc==1 );
  for(p=pTerm; p; p=p->pSynonym){
    if( 0==sqlite3Fts5IterEof(p->pIter) ){
      i64 iRowid = p->pIter->iRowid;
      if( bRetValid==0 || (bDesc!=(iRowid<iRet)) ){
        iRet = iRowid;
        bRetValid = 1;
      }
    }
  }

  if( pbEof && bRetValid==0 ) *pbEof = 1;
  return iRet;
}

/*
** Argument pTerm must be a synonym iterator.
*/
static int fts5ExprSynonymList(
  Fts5ExprTerm *pTerm, 
  i64 iRowid,
  Fts5Buffer *pBuf,               /* Use this buffer for space if required */
  u8 **pa, int *pn
){
  Fts5PoslistReader aStatic[4];
  Fts5PoslistReader *aIter = aStatic;
  int nIter = 0;
  int nAlloc = 4;
  int rc = SQLITE_OK;
  Fts5ExprTerm *p;

  assert( pTerm->pSynonym );
  for(p=pTerm; p; p=p->pSynonym){
    Fts5IndexIter *pIter = p->pIter;
    if( sqlite3Fts5IterEof(pIter)==0 && pIter->iRowid==iRowid ){
      if( pIter->nData==0 ) continue;
      if( nIter==nAlloc ){
        sqlite3_int64 nByte = sizeof(Fts5PoslistReader) * nAlloc * 2;
        Fts5PoslistReader *aNew = (Fts5PoslistReader*)sqlite3_malloc64(nByte);
        if( aNew==0 ){
          rc = SQLITE_NOMEM;
          goto synonym_poslist_out;
        }
        memcpy(aNew, aIter, sizeof(Fts5PoslistReader) * nIter);
        nAlloc = nAlloc*2;
        if( aIter!=aStatic ) sqlite3_free(aIter);
        aIter = aNew;
      }
      sqlite3Fts5PoslistReaderInit(pIter->pData, pIter->nData, &aIter[nIter]);
      assert( aIter[nIter].bEof==0 );
      nIter++;
    }
  }

  if( nIter==1 ){
    *pa = (u8*)aIter[0].a;
    *pn = aIter[0].n;
  }else{
    Fts5PoslistWriter writer = {0};
    i64 iPrev = -1;
    fts5BufferZero(pBuf);
    while( 1 ){
      int i;
      i64 iMin = FTS5_LARGEST_INT64;
      for(i=0; i<nIter; i++){
        if( aIter[i].bEof==0 ){
          if( aIter[i].iPos==iPrev ){
            if( sqlite3Fts5PoslistReaderNext(&aIter[i]) ) continue;
          }
          if( aIter[i].iPos<iMin ){
            iMin = aIter[i].iPos;
          }
        }
      }
      if( iMin==FTS5_LARGEST_INT64 || rc!=SQLITE_OK ) break;
      rc = sqlite3Fts5PoslistWriterAppend(pBuf, &writer, iMin);
      iPrev = iMin;
    }
    if( rc==SQLITE_OK ){
      *pa = pBuf->p;
      *pn = pBuf->n;
    }
  }

 synonym_poslist_out:
  if( aIter!=aStatic ) sqlite3_free(aIter);
  return rc;
}


/*
** All individual term iterators in pPhrase are guaranteed to be valid and
** pointing to the same rowid when this function is called. This function 
** checks if the current rowid really is a match, and if so populates
** the pPhrase->poslist buffer accordingly. Output parameter *pbMatch
** is set to true if this is really a match, or false otherwise.
**
** SQLITE_OK is returned if an error occurs, or an SQLite error code 
** otherwise. It is not considered an error code if the current rowid is 
** not a match.
*/
static int fts5ExprPhraseIsMatch(
  Fts5ExprNode *pNode,            /* Node pPhrase belongs to */
  Fts5ExprPhrase *pPhrase,        /* Phrase object to initialize */
  int *pbMatch                    /* OUT: Set to true if really a match */
){
  Fts5PoslistWriter writer = {0};
  Fts5PoslistReader aStatic[4];
  Fts5PoslistReader *aIter = aStatic;
  int i;
  int rc = SQLITE_OK;
  int bFirst = pPhrase->aTerm[0].bFirst;
  
  fts5BufferZero(&pPhrase->poslist);

  /* If the aStatic[] array is not large enough, allocate a large array
  ** using sqlite3_malloc(). This approach could be improved upon. */
  if( pPhrase->nTerm>ArraySize(aStatic) ){
    sqlite3_int64 nByte = sizeof(Fts5PoslistReader) * pPhrase->nTerm;
    aIter = (Fts5PoslistReader*)sqlite3_malloc64(nByte);
    if( !aIter ) return SQLITE_NOMEM;
  }
  memset(aIter, 0, sizeof(Fts5PoslistReader) * pPhrase->nTerm);

  /* Initialize a term iterator for each term in the phrase */
  for(i=0; i<pPhrase->nTerm; i++){
    Fts5ExprTerm *pTerm = &pPhrase->aTerm[i];
    int n = 0;
    int bFlag = 0;
    u8 *a = 0;
    if( pTerm->pSynonym ){
      Fts5Buffer buf = {0, 0, 0};
      rc = fts5ExprSynonymList(pTerm, pNode->iRowid, &buf, &a, &n);
      if( rc ){
        sqlite3_free(a);
        goto ismatch_out;
      }
      if( a==buf.p ) bFlag = 1;
    }else{
      a = (u8*)pTerm->pIter->pData;
      n = pTerm->pIter->nData;
    }
    sqlite3Fts5PoslistReaderInit(a, n, &aIter[i]);
    aIter[i].bFlag = (u8)bFlag;
    if( aIter[i].bEof ) goto ismatch_out;
  }

  while( 1 ){
    int bMatch;
    i64 iPos = aIter[0].iPos;
    do {
      bMatch = 1;
      for(i=0; i<pPhrase->nTerm; i++){
        Fts5PoslistReader *pPos = &aIter[i];
        i64 iAdj = iPos + i;
        if( pPos->iPos!=iAdj ){
          bMatch = 0;
          while( pPos->iPos<iAdj ){
            if( sqlite3Fts5PoslistReaderNext(pPos) ) goto ismatch_out;
          }
          if( pPos->iPos>iAdj ) iPos = pPos->iPos-i;
        }
      }
    }while( bMatch==0 );

    /* Append position iPos to the output */
    if( bFirst==0 || FTS5_POS2OFFSET(iPos)==0 ){
      rc = sqlite3Fts5PoslistWriterAppend(&pPhrase->poslist, &writer, iPos);
      if( rc!=SQLITE_OK ) goto ismatch_out;
    }

    for(i=0; i<pPhrase->nTerm; i++){
      if( sqlite3Fts5PoslistReaderNext(&aIter[i]) ) goto ismatch_out;
    }
  }

 ismatch_out:
  *pbMatch = (pPhrase->poslist.n>0);
  for(i=0; i<pPhrase->nTerm; i++){
    if( aIter[i].bFlag ) sqlite3_free((u8*)aIter[i].a);
  }
  if( aIter!=aStatic ) sqlite3_free(aIter);
  return rc;
}

typedef struct Fts5LookaheadReader Fts5LookaheadReader;
struct Fts5LookaheadReader {
  const u8 *a;                    /* Buffer containing position list */
  int n;                          /* Size of buffer a[] in bytes */
  int i;                          /* Current offset in position list */
  i64 iPos;                       /* Current position */
  i64 iLookahead;                 /* Next position */
};

#define FTS5_LOOKAHEAD_EOF (((i64)1) << 62)

static int fts5LookaheadReaderNext(Fts5LookaheadReader *p){
  p->iPos = p->iLookahead;
  if( sqlite3Fts5PoslistNext64(p->a, p->n, &p->i, &p->iLookahead) ){
    p->iLookahead = FTS5_LOOKAHEAD_EOF;
  }
  return (p->iPos==FTS5_LOOKAHEAD_EOF);
}

static int fts5LookaheadReaderInit(
  const u8 *a, int n,             /* Buffer to read position list from */
  Fts5LookaheadReader *p          /* Iterator object to initialize */
){
  memset(p, 0, sizeof(Fts5LookaheadReader));
  p->a = a;
  p->n = n;
  fts5LookaheadReaderNext(p);
  return fts5LookaheadReaderNext(p);
}

typedef struct Fts5NearTrimmer Fts5NearTrimmer;
struct Fts5NearTrimmer {
  Fts5LookaheadReader reader;     /* Input iterator */
  Fts5PoslistWriter writer;       /* Writer context */
  Fts5Buffer *pOut;               /* Output poslist */
};

/*
** The near-set object passed as the first argument contains more than
** one phrase. All phrases currently point to the same row. The
** Fts5ExprPhrase.poslist buffers are populated accordingly. This function
** tests if the current row contains instances of each phrase sufficiently
** close together to meet the NEAR constraint. Non-zero is returned if it
** does, or zero otherwise.
**
** If in/out parameter (*pRc) is set to other than SQLITE_OK when this
** function is called, it is a no-op. Or, if an error (e.g. SQLITE_NOMEM)
** occurs within this function (*pRc) is set accordingly before returning.
** The return value is undefined in both these cases.
** 
** If no error occurs and non-zero (a match) is returned, the position-list
** of each phrase object is edited to contain only those entries that
** meet the constraint before returning.
*/
static int fts5ExprNearIsMatch(int *pRc, Fts5ExprNearset *pNear){
  Fts5NearTrimmer aStatic[4];
  Fts5NearTrimmer *a = aStatic;
  Fts5ExprPhrase **apPhrase = pNear->apPhrase;

  int i;
  int rc = *pRc;
  int bMatch;

  assert( pNear->nPhrase>1 );

  /* If the aStatic[] array is not large enough, allocate a large array
  ** using sqlite3_malloc(). This approach could be improved upon. */
  if( pNear->nPhrase>ArraySize(aStatic) ){
    sqlite3_int64 nByte = sizeof(Fts5NearTrimmer) * pNear->nPhrase;
    a = (Fts5NearTrimmer*)sqlite3Fts5MallocZero(&rc, nByte);
  }else{
    memset(aStatic, 0, sizeof(aStatic));
  }
  if( rc!=SQLITE_OK ){
    *pRc = rc;
    return 0;
  }

  /* Initialize a lookahead iterator for each phrase. After passing the
  ** buffer and buffer size to the lookaside-reader init function, zero
  ** the phrase poslist buffer. The new poslist for the phrase (containing
  ** the same entries as the original with some entries removed on account 
  ** of the NEAR constraint) is written over the original even as it is
  ** being read. This is safe as the entries for the new poslist are a
  ** subset of the old, so it is not possible for data yet to be read to
  ** be overwritten.  */
  for(i=0; i<pNear->nPhrase; i++){
    Fts5Buffer *pPoslist = &apPhrase[i]->poslist;
    fts5LookaheadReaderInit(pPoslist->p, pPoslist->n, &a[i].reader);
    pPoslist->n = 0;
    a[i].pOut = pPoslist;
  }

  while( 1 ){
    int iAdv;
    i64 iMin;
    i64 iMax;

    /* This block advances the phrase iterators until they point to a set of
    ** entries that together comprise a match.  */
    iMax = a[0].reader.iPos;
    do {
      bMatch = 1;
      for(i=0; i<pNear->nPhrase; i++){
        Fts5LookaheadReader *pPos = &a[i].reader;
        iMin = iMax - pNear->apPhrase[i]->nTerm - pNear->nNear;
        if( pPos->iPos<iMin || pPos->iPos>iMax ){
          bMatch = 0;
          while( pPos->iPos<iMin ){
            if( fts5LookaheadReaderNext(pPos) ) goto ismatch_out;
          }
          if( pPos->iPos>iMax ) iMax = pPos->iPos;
        }
      }
    }while( bMatch==0 );

    /* Add an entry to each output position list */
    for(i=0; i<pNear->nPhrase; i++){
      i64 iPos = a[i].reader.iPos;
      Fts5PoslistWriter *pWriter = &a[i].writer;
      if( a[i].pOut->n==0 || iPos!=pWriter->iPrev ){
        sqlite3Fts5PoslistWriterAppend(a[i].pOut, pWriter, iPos);
      }
    }

    iAdv = 0;
    iMin = a[0].reader.iLookahead;
    for(i=0; i<pNear->nPhrase; i++){
      if( a[i].reader.iLookahead < iMin ){
        iMin = a[i].reader.iLookahead;
        iAdv = i;
      }
    }
    if( fts5LookaheadReaderNext(&a[iAdv].reader) ) goto ismatch_out;
  }

  ismatch_out: {
    int bRet = a[0].pOut->n>0;
    *pRc = rc;
    if( a!=aStatic ) sqlite3_free(a);
    return bRet;
  }
}

/*
** Advance iterator pIter until it points to a value equal to or laster
** than the initial value of *piLast. If this means the iterator points
** to a value laster than *piLast, update *piLast to the new lastest value.
**
** If the iterator reaches EOF, set *pbEof to true before returning. If
** an error occurs, set *pRc to an error code. If either *pbEof or *pRc
** are set, return a non-zero value. Otherwise, return zero.
*/
static int fts5ExprAdvanceto(
  Fts5IndexIter *pIter,           /* Iterator to advance */
  int bDesc,                      /* True if iterator is "rowid DESC" */
  i64 *piLast,                    /* IN/OUT: Lastest rowid seen so far */
  int *pRc,                       /* OUT: Error code */
  int *pbEof                      /* OUT: Set to true if EOF */
){
  i64 iLast = *piLast;
  i64 iRowid;

  iRowid = pIter->iRowid;
  if( (bDesc==0 && iLast>iRowid) || (bDesc && iLast<iRowid) ){
    int rc = sqlite3Fts5IterNextFrom(pIter, iLast);
    if( rc || sqlite3Fts5IterEof(pIter) ){
      *pRc = rc;
      *pbEof = 1;
      return 1;
    }
    iRowid = pIter->iRowid;
    assert( (bDesc==0 && iRowid>=iLast) || (bDesc==1 && iRowid<=iLast) );
  }
  *piLast = iRowid;

  return 0;
}

static int fts5ExprSynonymAdvanceto(
  Fts5ExprTerm *pTerm,            /* Term iterator to advance */
  int bDesc,                      /* True if iterator is "rowid DESC" */
  i64 *piLast,                    /* IN/OUT: Lastest rowid seen so far */
  int *pRc                        /* OUT: Error code */
){
  int rc = SQLITE_OK;
  i64 iLast = *piLast;
  Fts5ExprTerm *p;
  int bEof = 0;

  for(p=pTerm; rc==SQLITE_OK && p; p=p->pSynonym){
    if( sqlite3Fts5IterEof(p->pIter)==0 ){
      i64 iRowid = p->pIter->iRowid;
      if( (bDesc==0 && iLast>iRowid) || (bDesc && iLast<iRowid) ){
        rc = sqlite3Fts5IterNextFrom(p->pIter, iLast);
      }
    }
  }

  if( rc!=SQLITE_OK ){
    *pRc = rc;
    bEof = 1;
  }else{
    *piLast = fts5ExprSynonymRowid(pTerm, bDesc, &bEof);
  }
  return bEof;
}


static int fts5ExprNearTest(
  int *pRc,
  Fts5Expr *pExpr,                /* Expression that pNear is a part of */
  Fts5ExprNode *pNode             /* The "NEAR" node (FTS5_STRING) */
){
  Fts5ExprNearset *pNear = pNode->pNear;
  int rc = *pRc;

  if( pExpr->pConfig->eDetail!=FTS5_DETAIL_FULL ){
    Fts5ExprTerm *pTerm;
    Fts5ExprPhrase *pPhrase = pNear->apPhrase[0];
    pPhrase->poslist.n = 0;
    for(pTerm=&pPhrase->aTerm[0]; pTerm; pTerm=pTerm->pSynonym){
      Fts5IndexIter *pIter = pTerm->pIter;
      if( sqlite3Fts5IterEof(pIter)==0 ){
        if( pIter->iRowid==pNode->iRowid && pIter->nData>0 ){
          pPhrase->poslist.n = 1;
        }
      }
    }
    return pPhrase->poslist.n;
  }else{
    int i;

    /* Check that each phrase in the nearset matches the current row.
    ** Populate the pPhrase->poslist buffers at the same time. If any
    ** phrase is not a match, break out of the loop early.  */
    for(i=0; rc==SQLITE_OK && i<pNear->nPhrase; i++){
      Fts5ExprPhrase *pPhrase = pNear->apPhrase[i];
      if( pPhrase->nTerm>1 || pPhrase->aTerm[0].pSynonym 
       || pNear->pColset || pPhrase->aTerm[0].bFirst
      ){
        int bMatch = 0;
        rc = fts5ExprPhraseIsMatch(pNode, pPhrase, &bMatch);
        if( bMatch==0 ) break;
      }else{
        Fts5IndexIter *pIter = pPhrase->aTerm[0].pIter;
        fts5BufferSet(&rc, &pPhrase->poslist, pIter->nData, pIter->pData);
      }
    }

    *pRc = rc;
    if( i==pNear->nPhrase && (i==1 || fts5ExprNearIsMatch(pRc, pNear)) ){
      return 1;
    }
    return 0;
  }
}


/*
** Initialize all term iterators in the pNear object. If any term is found
** to match no documents at all, return immediately without initializing any
** further iterators.
**
** If an error occurs, return an SQLite error code. Otherwise, return
** SQLITE_OK. It is not considered an error if some term matches zero
** documents.
*/
static int fts5ExprNearInitAll(
  Fts5Expr *pExpr,
  Fts5ExprNode *pNode
){
  Fts5ExprNearset *pNear = pNode->pNear;
  int i;

  assert( pNode->bNomatch==0 );
  for(i=0; i<pNear->nPhrase; i++){
    Fts5ExprPhrase *pPhrase = pNear->apPhrase[i];
    if( pPhrase->nTerm==0 ){
      pNode->bEof = 1;
      return SQLITE_OK;
    }else{
      int j;
      for(j=0; j<pPhrase->nTerm; j++){
        Fts5ExprTerm *pTerm = &pPhrase->aTerm[j];
        Fts5ExprTerm *p;
        int bHit = 0;

        for(p=pTerm; p; p=p->pSynonym){
          int rc;
          if( p->pIter ){
            sqlite3Fts5IterClose(p->pIter);
            p->pIter = 0;
          }
          rc = sqlite3Fts5IndexQuery(
              pExpr->pIndex, p->zTerm, (int)strlen(p->zTerm),
              (pTerm->bPrefix ? FTS5INDEX_QUERY_PREFIX : 0) |
              (pExpr->bDesc ? FTS5INDEX_QUERY_DESC : 0),
              pNear->pColset,
              &p->pIter
          );
          assert( (rc==SQLITE_OK)==(p->pIter!=0) );
          if( rc!=SQLITE_OK ) return rc;
          if( 0==sqlite3Fts5IterEof(p->pIter) ){
            bHit = 1;
          }
        }

        if( bHit==0 ){
          pNode->bEof = 1;
          return SQLITE_OK;
        }
      }
    }
  }

  pNode->bEof = 0;
  return SQLITE_OK;
}

/*
** If pExpr is an ASC iterator, this function returns a value with the
** same sign as:
**
**   (iLhs - iRhs)
**
** Otherwise, if this is a DESC iterator, the opposite is returned:
**
**   (iRhs - iLhs)
*/
static int fts5RowidCmp(
  Fts5Expr *pExpr,
  i64 iLhs,
  i64 iRhs
){
  assert( pExpr->bDesc==0 || pExpr->bDesc==1 );
  if( pExpr->bDesc==0 ){
    if( iLhs<iRhs ) return -1;
    return (iLhs > iRhs);
  }else{
    if( iLhs>iRhs ) return -1;
    return (iLhs < iRhs);
  }
}

static void fts5ExprSetEof(Fts5ExprNode *pNode){
  int i;
  pNode->bEof = 1;
  pNode->bNomatch = 0;
  for(i=0; i<pNode->nChild; i++){
    fts5ExprSetEof(pNode->apChild[i]);
  }
}

static void fts5ExprNodeZeroPoslist(Fts5ExprNode *pNode){
  if( pNode->eType==FTS5_STRING || pNode->eType==FTS5_TERM ){
    Fts5ExprNearset *pNear = pNode->pNear;
    int i;
    for(i=0; i<pNear->nPhrase; i++){
      Fts5ExprPhrase *pPhrase = pNear->apPhrase[i];
      pPhrase->poslist.n = 0;
    }
  }else{
    int i;
    for(i=0; i<pNode->nChild; i++){
      fts5ExprNodeZeroPoslist(pNode->apChild[i]);
    }
  }
}



/*
** Compare the values currently indicated by the two nodes as follows:
**
**    res = (*p1) - (*p2)
**
** Nodes that point to values that come later in the iteration order are
** considered to be larger. Nodes at EOF are the largest of all.
**
** This means that if the iteration order is ASC, then numerically larger
** rowids are considered larger. Or if it is the default DESC, numerically
** smaller rowids are larger.
*/
static int fts5NodeCompare(
  Fts5Expr *pExpr,
  Fts5ExprNode *p1, 
  Fts5ExprNode *p2
){
  if( p2->bEof ) return -1;
  if( p1->bEof ) return +1;
  return fts5RowidCmp(pExpr, p1->iRowid, p2->iRowid);
}

/*
** All individual term iterators in pNear are guaranteed to be valid when
** this function is called. This function checks if all term iterators
** point to the same rowid, and if not, advances them until they do.
** If an EOF is reached before this happens, *pbEof is set to true before
** returning.
**
** SQLITE_OK is returned if an error occurs, or an SQLite error code 
** otherwise. It is not considered an error code if an iterator reaches
** EOF.
*/
static int fts5ExprNodeTest_STRING(
  Fts5Expr *pExpr,                /* Expression pPhrase belongs to */
  Fts5ExprNode *pNode
){
  Fts5ExprNearset *pNear = pNode->pNear;
  Fts5ExprPhrase *pLeft = pNear->apPhrase[0];
  int rc = SQLITE_OK;
  i64 iLast;                      /* Lastest rowid any iterator points to */
  int i, j;                       /* Phrase and token index, respectively */
  int bMatch;                     /* True if all terms are at the same rowid */
  const int bDesc = pExpr->bDesc;

  /* Check that this node should not be FTS5_TERM */
  assert( pNear->nPhrase>1 
       || pNear->apPhrase[0]->nTerm>1 
       || pNear->apPhrase[0]->aTerm[0].pSynonym
       || pNear->apPhrase[0]->aTerm[0].bFirst
  );

  /* Initialize iLast, the "lastest" rowid any iterator points to. If the
  ** iterator skips through rowids in the default ascending order, this means
  ** the maximum rowid. Or, if the iterator is "ORDER BY rowid DESC", then it
  ** means the minimum rowid.  */
  if( pLeft->aTerm[0].pSynonym ){
    iLast = fts5ExprSynonymRowid(&pLeft->aTerm[0], bDesc, 0);
  }else{
    iLast = pLeft->aTerm[0].pIter->iRowid;
  }

  do {
    bMatch = 1;
    for(i=0; i<pNear->nPhrase; i++){
      Fts5ExprPhrase *pPhrase = pNear->apPhrase[i];
      for(j=0; j<pPhrase->nTerm; j++){
        Fts5ExprTerm *pTerm = &pPhrase->aTerm[j];
        if( pTerm->pSynonym ){
          i64 iRowid = fts5ExprSynonymRowid(pTerm, bDesc, 0);
          if( iRowid==iLast ) continue;
          bMatch = 0;
          if( fts5ExprSynonymAdvanceto(pTerm, bDesc, &iLast, &rc) ){
            pNode->bNomatch = 0;
            pNode->bEof = 1;
            return rc;
          }
        }else{
          Fts5IndexIter *pIter = pPhrase->aTerm[j].pIter;
          if( pIter->iRowid==iLast || pIter->bEof ) continue;
          bMatch = 0;
          if( fts5ExprAdvanceto(pIter, bDesc, &iLast, &rc, &pNode->bEof) ){
            return rc;
          }
        }
      }
    }
  }while( bMatch==0 );

  pNode->iRowid = iLast;
  pNode->bNomatch = ((0==fts5ExprNearTest(&rc, pExpr, pNode)) && rc==SQLITE_OK);
  assert( pNode->bEof==0 || pNode->bNomatch==0 );

  return rc;
}

/*
** Advance the first term iterator in the first phrase of pNear. Set output
** variable *pbEof to true if it reaches EOF or if an error occurs.
**
** Return SQLITE_OK if successful, or an SQLite error code if an error
** occurs.
*/
static int fts5ExprNodeNext_STRING(
  Fts5Expr *pExpr,                /* Expression pPhrase belongs to */
  Fts5ExprNode *pNode,            /* FTS5_STRING or FTS5_TERM node */
  int bFromValid,
  i64 iFrom 
){
  Fts5ExprTerm *pTerm = &pNode->pNear->apPhrase[0]->aTerm[0];
  int rc = SQLITE_OK;

  pNode->bNomatch = 0;
  if( pTerm->pSynonym ){
    int bEof = 1;
    Fts5ExprTerm *p;

    /* Find the firstest rowid any synonym points to. */
    i64 iRowid = fts5ExprSynonymRowid(pTerm, pExpr->bDesc, 0);

    /* Advance each iterator that currently points to iRowid. Or, if iFrom
    ** is valid - each iterator that points to a rowid before iFrom.  */
    for(p=pTerm; p; p=p->pSynonym){
      if( sqlite3Fts5IterEof(p->pIter)==0 ){
        i64 ii = p->pIter->iRowid;
        if( ii==iRowid 
         || (bFromValid && ii!=iFrom && (ii>iFrom)==pExpr->bDesc) 
        ){
          if( bFromValid ){
            rc = sqlite3Fts5IterNextFrom(p->pIter, iFrom);
          }else{
            rc = sqlite3Fts5IterNext(p->pIter);
          }
          if( rc!=SQLITE_OK ) break;
          if( sqlite3Fts5IterEof(p->pIter)==0 ){
            bEof = 0;
          }
        }else{
          bEof = 0;
        }
      }
    }

    /* Set the EOF flag if either all synonym iterators are at EOF or an
    ** error has occurred.  */
    pNode->bEof = (rc || bEof);
  }else{
    Fts5IndexIter *pIter = pTerm->pIter;

    assert( Fts5NodeIsString(pNode) );
    if( bFromValid ){
      rc = sqlite3Fts5IterNextFrom(pIter, iFrom);
    }else{
      rc = sqlite3Fts5IterNext(pIter);
    }

    pNode->bEof = (rc || sqlite3Fts5IterEof(pIter));
  }

  if( pNode->bEof==0 ){
    assert( rc==SQLITE_OK );
    rc = fts5ExprNodeTest_STRING(pExpr, pNode);
  }

  return rc;
}


static int fts5ExprNodeTest_TERM(
  Fts5Expr *pExpr,                /* Expression that pNear is a part of */
  Fts5ExprNode *pNode             /* The "NEAR" node (FTS5_TERM) */
){
  /* As this "NEAR" object is actually a single phrase that consists 
  ** of a single term only, grab pointers into the poslist managed by the
  ** fts5_index.c iterator object. This is much faster than synthesizing 
  ** a new poslist the way we have to for more complicated phrase or NEAR
  ** expressions.  */
  Fts5ExprPhrase *pPhrase = pNode->pNear->apPhrase[0];
  Fts5IndexIter *pIter = pPhrase->aTerm[0].pIter;

  assert( pNode->eType==FTS5_TERM );
  assert( pNode->pNear->nPhrase==1 && pPhrase->nTerm==1 );
  assert( pPhrase->aTerm[0].pSynonym==0 );

  pPhrase->poslist.n = pIter->nData;
  if( pExpr->pConfig->eDetail==FTS5_DETAIL_FULL ){
    pPhrase->poslist.p = (u8*)pIter->pData;
  }
  pNode->iRowid = pIter->iRowid;
  pNode->bNomatch = (pPhrase->poslist.n==0);
  return SQLITE_OK;
}

/*
** xNext() method for a node of type FTS5_TERM.
*/
static int fts5ExprNodeNext_TERM(
  Fts5Expr *pExpr, 
  Fts5ExprNode *pNode,
  int bFromValid,
  i64 iFrom
){
  int rc;
  Fts5IndexIter *pIter = pNode->pNear->apPhrase[0]->aTerm[0].pIter;

  assert( pNode->bEof==0 );
  if( bFromValid ){
    rc = sqlite3Fts5IterNextFrom(pIter, iFrom);
  }else{
    rc = sqlite3Fts5IterNext(pIter);
  }
  if( rc==SQLITE_OK && sqlite3Fts5IterEof(pIter)==0 ){
    rc = fts5ExprNodeTest_TERM(pExpr, pNode);
  }else{
    pNode->bEof = 1;
    pNode->bNomatch = 0;
  }
  return rc;
}

static void fts5ExprNodeTest_OR(
  Fts5Expr *pExpr,                /* Expression of which pNode is a part */
  Fts5ExprNode *pNode             /* Expression node to test */
){
  Fts5ExprNode *pNext = pNode->apChild[0];
  int i;

  for(i=1; i<pNode->nChild; i++){
    Fts5ExprNode *pChild = pNode->apChild[i];
    int cmp = fts5NodeCompare(pExpr, pNext, pChild);
    if( cmp>0 || (cmp==0 && pChild->bNomatch==0) ){
      pNext = pChild;
    }
  }
  pNode->iRowid = pNext->iRowid;
  pNode->bEof = pNext->bEof;
  pNode->bNomatch = pNext->bNomatch;
}

static int fts5ExprNodeNext_OR(
  Fts5Expr *pExpr, 
  Fts5ExprNode *pNode,
  int bFromValid,
  i64 iFrom
){
  int i;
  i64 iLast = pNode->iRowid;

  for(i=0; i<pNode->nChild; i++){
    Fts5ExprNode *p1 = pNode->apChild[i];
    assert( p1->bEof || fts5RowidCmp(pExpr, p1->iRowid, iLast)>=0 );
    if( p1->bEof==0 ){
      if( (p1->iRowid==iLast) 
       || (bFromValid && fts5RowidCmp(pExpr, p1->iRowid, iFrom)<0)
      ){
        int rc = fts5ExprNodeNext(pExpr, p1, bFromValid, iFrom);
        if( rc!=SQLITE_OK ){
          pNode->bNomatch = 0;
          return rc;
        }
      }
    }
  }

  fts5ExprNodeTest_OR(pExpr, pNode);
  return SQLITE_OK;
}

/*
** Argument pNode is an FTS5_AND node.
*/
static int fts5ExprNodeTest_AND(
  Fts5Expr *pExpr,                /* Expression pPhrase belongs to */
  Fts5ExprNode *pAnd              /* FTS5_AND node to advance */
){
  int iChild;
  i64 iLast = pAnd->iRowid;
  int rc = SQLITE_OK;
  int bMatch;

  assert( pAnd->bEof==0 );
  do {
    pAnd->bNomatch = 0;
    bMatch = 1;
    for(iChild=0; iChild<pAnd->nChild; iChild++){
      Fts5ExprNode *pChild = pAnd->apChild[iChild];
      int cmp = fts5RowidCmp(pExpr, iLast, pChild->iRowid);
      if( cmp>0 ){
        /* Advance pChild until it points to iLast or laster */
        rc = fts5ExprNodeNext(pExpr, pChild, 1, iLast);
        if( rc!=SQLITE_OK ){
          pAnd->bNomatch = 0;
          return rc;
        }
      }

      /* If the child node is now at EOF, so is the parent AND node. Otherwise,
      ** the child node is guaranteed to have advanced at least as far as
      ** rowid iLast. So if it is not at exactly iLast, pChild->iRowid is the
      ** new lastest rowid seen so far.  */
      assert( pChild->bEof || fts5RowidCmp(pExpr, iLast, pChild->iRowid)<=0 );
      if( pChild->bEof ){
        fts5ExprSetEof(pAnd);
        bMatch = 1;
        break;
      }else if( iLast!=pChild->iRowid ){
        bMatch = 0;
        iLast = pChild->iRowid;
      }

      if( pChild->bNomatch ){
        pAnd->bNomatch = 1;
      }
    }
  }while( bMatch==0 );

  if( pAnd->bNomatch && pAnd!=pExpr->pRoot ){
    fts5ExprNodeZeroPoslist(pAnd);
  }
  pAnd->iRowid = iLast;
  return SQLITE_OK;
}

static int fts5ExprNodeNext_AND(
  Fts5Expr *pExpr, 
  Fts5ExprNode *pNode,
  int bFromValid,
  i64 iFrom
){
  int rc = fts5ExprNodeNext(pExpr, pNode->apChild[0], bFromValid, iFrom);
  if( rc==SQLITE_OK ){
    rc = fts5ExprNodeTest_AND(pExpr, pNode);
  }else{
    pNode->bNomatch = 0;
  }
  return rc;
}

static int fts5ExprNodeTest_NOT(
  Fts5Expr *pExpr,                /* Expression pPhrase belongs to */
  Fts5ExprNode *pNode             /* FTS5_NOT node to advance */
){
  int rc = SQLITE_OK;
  Fts5ExprNode *p1 = pNode->apChild[0];
  Fts5ExprNode *p2 = pNode->apChild[1];
  assert( pNode->nChild==2 );

  while( rc==SQLITE_OK && p1->bEof==0 ){
    int cmp = fts5NodeCompare(pExpr, p1, p2);
    if( cmp>0 ){
      rc = fts5ExprNodeNext(pExpr, p2, 1, p1->iRowid);
      cmp = fts5NodeCompare(pExpr, p1, p2);
    }
    assert( rc!=SQLITE_OK || cmp<=0 );
    if( cmp || p2->bNomatch ) break;
    rc = fts5ExprNodeNext(pExpr, p1, 0, 0);
  }
  pNode->bEof = p1->bEof;
  pNode->bNomatch = p1->bNomatch;
  pNode->iRowid = p1->iRowid;
  if( p1->bEof ){
    fts5ExprNodeZeroPoslist(p2);
  }
  return rc;
}

static int fts5ExprNodeNext_NOT(
  Fts5Expr *pExpr, 
  Fts5ExprNode *pNode,
  int bFromValid,
  i64 iFrom
){
  int rc = fts5ExprNodeNext(pExpr, pNode->apChild[0], bFromValid, iFrom);
  if( rc==SQLITE_OK ){
    rc = fts5ExprNodeTest_NOT(pExpr, pNode);
  }
  if( rc!=SQLITE_OK ){
    pNode->bNomatch = 0;
  }
  return rc;
}

/*
** If pNode currently points to a match, this function returns SQLITE_OK
** without modifying it. Otherwise, pNode is advanced until it does point
** to a match or EOF is reached.
*/
static int fts5ExprNodeTest(
  Fts5Expr *pExpr,                /* Expression of which pNode is a part */
  Fts5ExprNode *pNode             /* Expression node to test */
){
  int rc = SQLITE_OK;
  if( pNode->bEof==0 ){
    switch( pNode->eType ){

      case FTS5_STRING: {
        rc = fts5ExprNodeTest_STRING(pExpr, pNode);
        break;
      }

      case FTS5_TERM: {
        rc = fts5ExprNodeTest_TERM(pExpr, pNode);
        break;
      }

      case FTS5_AND: {
        rc = fts5ExprNodeTest_AND(pExpr, pNode);
        break;
      }

      case FTS5_OR: {
        fts5ExprNodeTest_OR(pExpr, pNode);
        break;
      }

      default: assert( pNode->eType==FTS5_NOT ); {
        rc = fts5ExprNodeTest_NOT(pExpr, pNode);
        break;
      }
    }
  }
  return rc;
}

 
/*
** Set node pNode, which is part of expression pExpr, to point to the first
** match. If there are no matches, set the Node.bEof flag to indicate EOF.
**
** Return an SQLite error code if an error occurs, or SQLITE_OK otherwise.
** It is not an error if there are no matches.
*/
static int fts5ExprNodeFirst(Fts5Expr *pExpr, Fts5ExprNode *pNode){
  int rc = SQLITE_OK;
  pNode->bEof = 0;
  pNode->bNomatch = 0;

  if( Fts5NodeIsString(pNode) ){
    /* Initialize all term iterators in the NEAR object. */
    rc = fts5ExprNearInitAll(pExpr, pNode);
  }else if( pNode->xNext==0 ){
    pNode->bEof = 1;
  }else{
    int i;
    int nEof = 0;
    for(i=0; i<pNode->nChild && rc==SQLITE_OK; i++){
      Fts5ExprNode *pChild = pNode->apChild[i];
      rc = fts5ExprNodeFirst(pExpr, pNode->apChild[i]);
      assert( pChild->bEof==0 || pChild->bEof==1 );
      nEof += pChild->bEof;
    }
    pNode->iRowid = pNode->apChild[0]->iRowid;

    switch( pNode->eType ){
      case FTS5_AND:
        if( nEof>0 ) fts5ExprSetEof(pNode);
        break;

      case FTS5_OR:
        if( pNode->nChild==nEof ) fts5ExprSetEof(pNode);
        break;

      default:
        assert( pNode->eType==FTS5_NOT );
        pNode->bEof = pNode->apChild[0]->bEof;
        break;
    }
  }

  if( rc==SQLITE_OK ){
    rc = fts5ExprNodeTest(pExpr, pNode);
  }
  return rc;
}


/*
** Begin iterating through the set of documents in index pIdx matched by
** the MATCH expression passed as the first argument. If the "bDesc" 
** parameter is passed a non-zero value, iteration is in descending rowid 
** order. Or, if it is zero, in ascending order.
**
** If iterating in ascending rowid order (bDesc==0), the first document
** visited is that with the smallest rowid that is larger than or equal
** to parameter iFirst. Or, if iterating in ascending order (bDesc==1),
** then the first document visited must have a rowid smaller than or
** equal to iFirst.
**
** Return SQLITE_OK if successful, or an SQLite error code otherwise. It
** is not considered an error if the query does not match any documents.
*/
static int sqlite3Fts5ExprFirst(Fts5Expr *p, Fts5Index *pIdx, i64 iFirst, int bDesc){
  Fts5ExprNode *pRoot = p->pRoot;
  int rc;                         /* Return code */

  p->pIndex = pIdx;
  p->bDesc = bDesc;
  rc = fts5ExprNodeFirst(p, pRoot);

  /* If not at EOF but the current rowid occurs earlier than iFirst in
  ** the iteration order, move to document iFirst or later. */
  if( rc==SQLITE_OK 
   && 0==pRoot->bEof 
   && fts5RowidCmp(p, pRoot->iRowid, iFirst)<0 
  ){
    rc = fts5ExprNodeNext(p, pRoot, 1, iFirst);
  }

  /* If the iterator is not at a real match, skip forward until it is. */
  while( pRoot->bNomatch && rc==SQLITE_OK ){
    assert( pRoot->bEof==0 );
    rc = fts5ExprNodeNext(p, pRoot, 0, 0);
  }
  return rc;
}

/*
** Move to the next document 
**
** Return SQLITE_OK if successful, or an SQLite error code otherwise. It
** is not considered an error if the query does not match any documents.
*/
static int sqlite3Fts5ExprNext(Fts5Expr *p, i64 iLast){
  int rc;
  Fts5ExprNode *pRoot = p->pRoot;
  assert( pRoot->bEof==0 && pRoot->bNomatch==0 );
  do {
    rc = fts5ExprNodeNext(p, pRoot, 0, 0);
    assert( pRoot->bNomatch==0 || (rc==SQLITE_OK && pRoot->bEof==0) );
  }while( pRoot->bNomatch );
  if( fts5RowidCmp(p, pRoot->iRowid, iLast)>0 ){
    pRoot->bEof = 1;
  }
  return rc;
}

static int sqlite3Fts5ExprEof(Fts5Expr *p){
  return p->pRoot->bEof;
}

static i64 sqlite3Fts5ExprRowid(Fts5Expr *p){
  return p->pRoot->iRowid;
}

static int fts5ParseStringFromToken(Fts5Token *pToken, char **pz){
  int rc = SQLITE_OK;
  *pz = sqlite3Fts5Strndup(&rc, pToken->p, pToken->n);
  return rc;
}

/*
** Free the phrase object passed as the only argument.
*/
static void fts5ExprPhraseFree(Fts5ExprPhrase *pPhrase){
  if( pPhrase ){
    int i;
    for(i=0; i<pPhrase->nTerm; i++){
      Fts5ExprTerm *pSyn;
      Fts5ExprTerm *pNext;
      Fts5ExprTerm *pTerm = &pPhrase->aTerm[i];
      sqlite3_free(pTerm->zTerm);
      sqlite3Fts5IterClose(pTerm->pIter);
      for(pSyn=pTerm->pSynonym; pSyn; pSyn=pNext){
        pNext = pSyn->pSynonym;
        sqlite3Fts5IterClose(pSyn->pIter);
        fts5BufferFree((Fts5Buffer*)&pSyn[1]);
        sqlite3_free(pSyn);
      }
    }
    if( pPhrase->poslist.nSpace>0 ) fts5BufferFree(&pPhrase->poslist);
    sqlite3_free(pPhrase);
  }
}

/*
** Set the "bFirst" flag on the first token of the phrase passed as the
** only argument.
*/
static void sqlite3Fts5ParseSetCaret(Fts5ExprPhrase *pPhrase){
  if( pPhrase && pPhrase->nTerm ){
    pPhrase->aTerm[0].bFirst = 1;
  }
}

/*
** If argument pNear is NULL, then a new Fts5ExprNearset object is allocated
** and populated with pPhrase. Or, if pNear is not NULL, phrase pPhrase is
** appended to it and the results returned.
**
** If an OOM error occurs, both the pNear and pPhrase objects are freed and
** NULL returned.
*/
static Fts5ExprNearset *sqlite3Fts5ParseNearset(
  Fts5Parse *pParse,              /* Parse context */
  Fts5ExprNearset *pNear,         /* Existing nearset, or NULL */
  Fts5ExprPhrase *pPhrase         /* Recently parsed phrase */
){
  const int SZALLOC = 8;
  Fts5ExprNearset *pRet = 0;

  if( pParse->rc==SQLITE_OK ){
    if( pPhrase==0 ){
      return pNear;
    }
    if( pNear==0 ){
      sqlite3_int64 nByte;
      nByte = sizeof(Fts5ExprNearset) + SZALLOC * sizeof(Fts5ExprPhrase*);
      pRet = sqlite3_malloc64(nByte);
      if( pRet==0 ){
        pParse->rc = SQLITE_NOMEM;
      }else{
        memset(pRet, 0, (size_t)nByte);
      }
    }else if( (pNear->nPhrase % SZALLOC)==0 ){
      int nNew = pNear->nPhrase + SZALLOC;
      sqlite3_int64 nByte;

      nByte = sizeof(Fts5ExprNearset) + nNew * sizeof(Fts5ExprPhrase*);
      pRet = (Fts5ExprNearset*)sqlite3_realloc64(pNear, nByte);
      if( pRet==0 ){
        pParse->rc = SQLITE_NOMEM;
      }
    }else{
      pRet = pNear;
    }
  }

  if( pRet==0 ){
    assert( pParse->rc!=SQLITE_OK );
    sqlite3Fts5ParseNearsetFree(pNear);
    sqlite3Fts5ParsePhraseFree(pPhrase);
  }else{
    if( pRet->nPhrase>0 ){
      Fts5ExprPhrase *pLast = pRet->apPhrase[pRet->nPhrase-1];
      assert( pParse!=0 );
      assert( pParse->apPhrase!=0 );
      assert( pParse->nPhrase>=2 );
      assert( pLast==pParse->apPhrase[pParse->nPhrase-2] );
      if( pPhrase->nTerm==0 ){
        fts5ExprPhraseFree(pPhrase);
        pRet->nPhrase--;
        pParse->nPhrase--;
        pPhrase = pLast;
      }else if( pLast->nTerm==0 ){
        fts5ExprPhraseFree(pLast);
        pParse->apPhrase[pParse->nPhrase-2] = pPhrase;
        pParse->nPhrase--;
        pRet->nPhrase--;
      }
    }
    pRet->apPhrase[pRet->nPhrase++] = pPhrase;
  }
  return pRet;
}

typedef struct TokenCtx TokenCtx;
struct TokenCtx {
  Fts5ExprPhrase *pPhrase;
  int rc;
};

/*
** Callback for tokenizing terms used by ParseTerm().
*/
static int fts5ParseTokenize(
  void *pContext,                 /* Pointer to Fts5InsertCtx object */
  int tflags,                     /* Mask of FTS5_TOKEN_* flags */
  const char *pToken,             /* Buffer containing token */
  int nToken,                     /* Size of token in bytes */
  int iUnused1,                   /* Start offset of token */
  int iUnused2                    /* End offset of token */
){
  int rc = SQLITE_OK;
  const int SZALLOC = 8;
  TokenCtx *pCtx = (TokenCtx*)pContext;
  Fts5ExprPhrase *pPhrase = pCtx->pPhrase;

  UNUSED_PARAM2(iUnused1, iUnused2);

  /* If an error has already occurred, this is a no-op */
  if( pCtx->rc!=SQLITE_OK ) return pCtx->rc;
  if( nToken>FTS5_MAX_TOKEN_SIZE ) nToken = FTS5_MAX_TOKEN_SIZE;

  if( pPhrase && pPhrase->nTerm>0 && (tflags & FTS5_TOKEN_COLOCATED) ){
    Fts5ExprTerm *pSyn;
    sqlite3_int64 nByte = sizeof(Fts5ExprTerm) + sizeof(Fts5Buffer) + nToken+1;
    pSyn = (Fts5ExprTerm*)sqlite3_malloc64(nByte);
    if( pSyn==0 ){
      rc = SQLITE_NOMEM;
    }else{
      memset(pSyn, 0, (size_t)nByte);
      pSyn->zTerm = ((char*)pSyn) + sizeof(Fts5ExprTerm) + sizeof(Fts5Buffer);
      memcpy(pSyn->zTerm, pToken, nToken);
      pSyn->pSynonym = pPhrase->aTerm[pPhrase->nTerm-1].pSynonym;
      pPhrase->aTerm[pPhrase->nTerm-1].pSynonym = pSyn;
    }
  }else{
    Fts5ExprTerm *pTerm;
    if( pPhrase==0 || (pPhrase->nTerm % SZALLOC)==0 ){
      Fts5ExprPhrase *pNew;
      int nNew = SZALLOC + (pPhrase ? pPhrase->nTerm : 0);

      pNew = (Fts5ExprPhrase*)sqlite3_realloc64(pPhrase, 
          sizeof(Fts5ExprPhrase) + sizeof(Fts5ExprTerm) * nNew
      );
      if( pNew==0 ){
        rc = SQLITE_NOMEM;
      }else{
        if( pPhrase==0 ) memset(pNew, 0, sizeof(Fts5ExprPhrase));
        pCtx->pPhrase = pPhrase = pNew;
        pNew->nTerm = nNew - SZALLOC;
      }
    }

    if( rc==SQLITE_OK ){
      pTerm = &pPhrase->aTerm[pPhrase->nTerm++];
      memset(pTerm, 0, sizeof(Fts5ExprTerm));
      pTerm->zTerm = sqlite3Fts5Strndup(&rc, pToken, nToken);
    }
  }

  pCtx->rc = rc;
  return rc;
}


/*
** Free the phrase object passed as the only argument.
*/
static void sqlite3Fts5ParsePhraseFree(Fts5ExprPhrase *pPhrase){
  fts5ExprPhraseFree(pPhrase);
}

/*
** Free the phrase object passed as the second argument.
*/
static void sqlite3Fts5ParseNearsetFree(Fts5ExprNearset *pNear){
  if( pNear ){
    int i;
    for(i=0; i<pNear->nPhrase; i++){
      fts5ExprPhraseFree(pNear->apPhrase[i]);
    }
    sqlite3_free(pNear->pColset);
    sqlite3_free(pNear);
  }
}

static void sqlite3Fts5ParseFinished(Fts5Parse *pParse, Fts5ExprNode *p){
  assert( pParse->pExpr==0 );
  pParse->pExpr = p;
}

static int parseGrowPhraseArray(Fts5Parse *pParse){
  if( (pParse->nPhrase % 8)==0 ){
    sqlite3_int64 nByte = sizeof(Fts5ExprPhrase*) * (pParse->nPhrase + 8);
    Fts5ExprPhrase **apNew;
    apNew = (Fts5ExprPhrase**)sqlite3_realloc64(pParse->apPhrase, nByte);
    if( apNew==0 ){
      pParse->rc = SQLITE_NOMEM;
      return SQLITE_NOMEM;
    }
    pParse->apPhrase = apNew;
  }
  return SQLITE_OK;
}

/*
** This function is called by the parser to process a string token. The
** string may or may not be quoted. In any case it is tokenized and a
** phrase object consisting of all tokens returned.
*/
static Fts5ExprPhrase *sqlite3Fts5ParseTerm(
  Fts5Parse *pParse,              /* Parse context */
  Fts5ExprPhrase *pAppend,        /* Phrase to append to */
  Fts5Token *pToken,              /* String to tokenize */
  int bPrefix                     /* True if there is a trailing "*" */
){
  Fts5Config *pConfig = pParse->pConfig;
  TokenCtx sCtx;                  /* Context object passed to callback */
  int rc;                         /* Tokenize return code */
  char *z = 0;

  memset(&sCtx, 0, sizeof(TokenCtx));
  sCtx.pPhrase = pAppend;

  rc = fts5ParseStringFromToken(pToken, &z);
  if( rc==SQLITE_OK ){
    int flags = FTS5_TOKENIZE_QUERY | (bPrefix ? FTS5_TOKENIZE_PREFIX : 0);
    int n;
    sqlite3Fts5Dequote(z);
    n = (int)strlen(z);
    rc = sqlite3Fts5Tokenize(pConfig, flags, z, n, &sCtx, fts5ParseTokenize);
  }
  sqlite3_free(z);
  if( rc || (rc = sCtx.rc) ){
    pParse->rc = rc;
    fts5ExprPhraseFree(sCtx.pPhrase);
    sCtx.pPhrase = 0;
  }else{

    if( pAppend==0 ){
      if( parseGrowPhraseArray(pParse) ){
        fts5ExprPhraseFree(sCtx.pPhrase);
        return 0;
      }
      pParse->nPhrase++;
    }

    if( sCtx.pPhrase==0 ){
      /* This happens when parsing a token or quoted phrase that contains
      ** no token characters at all. (e.g ... MATCH '""'). */
      sCtx.pPhrase = sqlite3Fts5MallocZero(&pParse->rc, sizeof(Fts5ExprPhrase));
    }else if( sCtx.pPhrase->nTerm ){
      sCtx.pPhrase->aTerm[sCtx.pPhrase->nTerm-1].bPrefix = (u8)bPrefix;
    }
    pParse->apPhrase[pParse->nPhrase-1] = sCtx.pPhrase;
  }

  return sCtx.pPhrase;
}

/*
** Create a new FTS5 expression by cloning phrase iPhrase of the
** expression passed as the second argument.
*/
static int sqlite3Fts5ExprClonePhrase(
  Fts5Expr *pExpr, 
  int iPhrase, 
  Fts5Expr **ppNew
){
  int rc = SQLITE_OK;             /* Return code */
  Fts5ExprPhrase *pOrig;          /* The phrase extracted from pExpr */
  Fts5Expr *pNew = 0;             /* Expression to return via *ppNew */
  TokenCtx sCtx = {0,0};          /* Context object for fts5ParseTokenize */

  pOrig = pExpr->apExprPhrase[iPhrase];
  pNew = (Fts5Expr*)sqlite3Fts5MallocZero(&rc, sizeof(Fts5Expr));
  if( rc==SQLITE_OK ){
    pNew->apExprPhrase = (Fts5ExprPhrase**)sqlite3Fts5MallocZero(&rc, 
        sizeof(Fts5ExprPhrase*));
  }
  if( rc==SQLITE_OK ){
    pNew->pRoot = (Fts5ExprNode*)sqlite3Fts5MallocZero(&rc, 
        sizeof(Fts5ExprNode));
  }
  if( rc==SQLITE_OK ){
    pNew->pRoot->pNear = (Fts5ExprNearset*)sqlite3Fts5MallocZero(&rc, 
        sizeof(Fts5ExprNearset) + sizeof(Fts5ExprPhrase*));
  }
  if( rc==SQLITE_OK ){
    Fts5Colset *pColsetOrig = pOrig->pNode->pNear->pColset;
    if( pColsetOrig ){
      sqlite3_int64 nByte;
      Fts5Colset *pColset;
      nByte = sizeof(Fts5Colset) + (pColsetOrig->nCol-1) * sizeof(int);
      pColset = (Fts5Colset*)sqlite3Fts5MallocZero(&rc, nByte);
      if( pColset ){ 
        memcpy(pColset, pColsetOrig, (size_t)nByte);
      }
      pNew->pRoot->pNear->pColset = pColset;
    }
  }

  if( pOrig->nTerm ){
    int i;                          /* Used to iterate through phrase terms */
    for(i=0; rc==SQLITE_OK && i<pOrig->nTerm; i++){
      int tflags = 0;
      Fts5ExprTerm *p;
      for(p=&pOrig->aTerm[i]; p && rc==SQLITE_OK; p=p->pSynonym){
        const char *zTerm = p->zTerm;
        rc = fts5ParseTokenize((void*)&sCtx, tflags, zTerm, (int)strlen(zTerm),
            0, 0);
        tflags = FTS5_TOKEN_COLOCATED;
      }
      if( rc==SQLITE_OK ){
        sCtx.pPhrase->aTerm[i].bPrefix = pOrig->aTerm[i].bPrefix;
        sCtx.pPhrase->aTerm[i].bFirst = pOrig->aTerm[i].bFirst;
      }
    }
  }else{
    /* This happens when parsing a token or quoted phrase that contains
    ** no token characters at all. (e.g ... MATCH '""'). */
    sCtx.pPhrase = sqlite3Fts5MallocZero(&rc, sizeof(Fts5ExprPhrase));
  }

  if( rc==SQLITE_OK && ALWAYS(sCtx.pPhrase) ){
    /* All the allocations succeeded. Put the expression object together. */
    pNew->pIndex = pExpr->pIndex;
    pNew->pConfig = pExpr->pConfig;
    pNew->nPhrase = 1;
    pNew->apExprPhrase[0] = sCtx.pPhrase;
    pNew->pRoot->pNear->apPhrase[0] = sCtx.pPhrase;
    pNew->pRoot->pNear->nPhrase = 1;
    sCtx.pPhrase->pNode = pNew->pRoot;

    if( pOrig->nTerm==1 
     && pOrig->aTerm[0].pSynonym==0 
     && pOrig->aTerm[0].bFirst==0 
    ){
      pNew->pRoot->eType = FTS5_TERM;
      pNew->pRoot->xNext = fts5ExprNodeNext_TERM;
    }else{
      pNew->pRoot->eType = FTS5_STRING;
      pNew->pRoot->xNext = fts5ExprNodeNext_STRING;
    }
  }else{
    sqlite3Fts5ExprFree(pNew);
    fts5ExprPhraseFree(sCtx.pPhrase);
    pNew = 0;
  }

  *ppNew = pNew;
  return rc;
}


/*
** Token pTok has appeared in a MATCH expression where the NEAR operator
** is expected. If token pTok does not contain "NEAR", store an error
** in the pParse object.
*/
static void sqlite3Fts5ParseNear(Fts5Parse *pParse, Fts5Token *pTok){
  if( pTok->n!=4 || memcmp("NEAR", pTok->p, 4) ){
    sqlite3Fts5ParseError(
        pParse, "fts5: syntax error near \"%.*s\"", pTok->n, pTok->p
    );
  }
}

static void sqlite3Fts5ParseSetDistance(
  Fts5Parse *pParse, 
  Fts5ExprNearset *pNear,
  Fts5Token *p
){
  if( pNear ){
    int nNear = 0;
    int i;
    if( p->n ){
      for(i=0; i<p->n; i++){
        char c = (char)p->p[i];
        if( c<'0' || c>'9' ){
          sqlite3Fts5ParseError(
              pParse, "expected integer, got \"%.*s\"", p->n, p->p
              );
          return;
        }
        nNear = nNear * 10 + (p->p[i] - '0');
      }
    }else{
      nNear = FTS5_DEFAULT_NEARDIST;
    }
    pNear->nNear = nNear;
  }
}

/*
** The second argument passed to this function may be NULL, or it may be
** an existing Fts5Colset object. This function returns a pointer to
** a new colset object containing the contents of (p) with new value column
** number iCol appended. 
**
** If an OOM error occurs, store an error code in pParse and return NULL.
** The old colset object (if any) is not freed in this case.
*/
static Fts5Colset *fts5ParseColset(
  Fts5Parse *pParse,              /* Store SQLITE_NOMEM here if required */
  Fts5Colset *p,                  /* Existing colset object */
  int iCol                        /* New column to add to colset object */
){
  int nCol = p ? p->nCol : 0;     /* Num. columns already in colset object */
  Fts5Colset *pNew;               /* New colset object to return */

  assert( pParse->rc==SQLITE_OK );
  assert( iCol>=0 && iCol<pParse->pConfig->nCol );

  pNew = sqlite3_realloc64(p, sizeof(Fts5Colset) + sizeof(int)*nCol);
  if( pNew==0 ){
    pParse->rc = SQLITE_NOMEM;
  }else{
    int *aiCol = pNew->aiCol;
    int i, j;
    for(i=0; i<nCol; i++){
      if( aiCol[i]==iCol ) return pNew;
      if( aiCol[i]>iCol ) break;
    }
    for(j=nCol; j>i; j--){
      aiCol[j] = aiCol[j-1];
    }
    aiCol[i] = iCol;
    pNew->nCol = nCol+1;

#ifndef NDEBUG
    /* Check that the array is in order and contains no duplicate entries. */
    for(i=1; i<pNew->nCol; i++) assert( pNew->aiCol[i]>pNew->aiCol[i-1] );
#endif
  }

  return pNew;
}

/*
** Allocate and return an Fts5Colset object specifying the inverse of
** the colset passed as the second argument. Free the colset passed
** as the second argument before returning.
*/
static Fts5Colset *sqlite3Fts5ParseColsetInvert(Fts5Parse *pParse, Fts5Colset *p){
  Fts5Colset *pRet;
  int nCol = pParse->pConfig->nCol;

  pRet = (Fts5Colset*)sqlite3Fts5MallocZero(&pParse->rc, 
      sizeof(Fts5Colset) + sizeof(int)*nCol
  );
  if( pRet ){
    int i;
    int iOld = 0;
    for(i=0; i<nCol; i++){
      if( iOld>=p->nCol || p->aiCol[iOld]!=i ){
        pRet->aiCol[pRet->nCol++] = i;
      }else{
        iOld++;
      }
    }
  }

  sqlite3_free(p);
  return pRet;
}

static Fts5Colset *sqlite3Fts5ParseColset(
  Fts5Parse *pParse,              /* Store SQLITE_NOMEM here if required */
  Fts5Colset *pColset,            /* Existing colset object */
  Fts5Token *p
){
  Fts5Colset *pRet = 0;
  int iCol;
  char *z;                        /* Dequoted copy of token p */

  z = sqlite3Fts5Strndup(&pParse->rc, p->p, p->n);
  if( pParse->rc==SQLITE_OK ){
    Fts5Config *pConfig = pParse->pConfig;
    sqlite3Fts5Dequote(z);
    for(iCol=0; iCol<pConfig->nCol; iCol++){
      if( 0==sqlite3_stricmp(pConfig->azCol[iCol], z) ) break;
    }
    if( iCol==pConfig->nCol ){
      sqlite3Fts5ParseError(pParse, "no such column: %s", z);
    }else{
      pRet = fts5ParseColset(pParse, pColset, iCol);
    }
    sqlite3_free(z);
  }

  if( pRet==0 ){
    assert( pParse->rc!=SQLITE_OK );
    sqlite3_free(pColset);
  }

  return pRet;
}

/*
** If argument pOrig is NULL, or if (*pRc) is set to anything other than
** SQLITE_OK when this function is called, NULL is returned. 
**
** Otherwise, a copy of (*pOrig) is made into memory obtained from
** sqlite3Fts5MallocZero() and a pointer to it returned. If the allocation
** fails, (*pRc) is set to SQLITE_NOMEM and NULL is returned.
*/
static Fts5Colset *fts5CloneColset(int *pRc, Fts5Colset *pOrig){
  Fts5Colset *pRet;
  if( pOrig ){
    sqlite3_int64 nByte = sizeof(Fts5Colset) + (pOrig->nCol-1) * sizeof(int);
    pRet = (Fts5Colset*)sqlite3Fts5MallocZero(pRc, nByte);
    if( pRet ){ 
      memcpy(pRet, pOrig, (size_t)nByte);
    }
  }else{
    pRet = 0;
  }
  return pRet;
}

/*
** Remove from colset pColset any columns that are not also in colset pMerge.
*/
static void fts5MergeColset(Fts5Colset *pColset, Fts5Colset *pMerge){
  int iIn = 0;          /* Next input in pColset */
  int iMerge = 0;       /* Next input in pMerge */
  int iOut = 0;         /* Next output slot in pColset */

  while( iIn<pColset->nCol && iMerge<pMerge->nCol ){
    int iDiff = pColset->aiCol[iIn] - pMerge->aiCol[iMerge];
    if( iDiff==0 ){
      pColset->aiCol[iOut++] = pMerge->aiCol[iMerge];
      iMerge++;
      iIn++;
    }else if( iDiff>0 ){
      iMerge++;
    }else{
      iIn++;
    }
  }
  pColset->nCol = iOut;
}

/*
** Recursively apply colset pColset to expression node pNode and all of
** its decendents. If (*ppFree) is not NULL, it contains a spare copy
** of pColset. This function may use the spare copy and set (*ppFree) to
** zero, or it may create copies of pColset using fts5CloneColset().
*/
static void fts5ParseSetColset(
  Fts5Parse *pParse, 
  Fts5ExprNode *pNode, 
  Fts5Colset *pColset,
  Fts5Colset **ppFree
){
  if( pParse->rc==SQLITE_OK ){
    assert( pNode->eType==FTS5_TERM || pNode->eType==FTS5_STRING 
         || pNode->eType==FTS5_AND  || pNode->eType==FTS5_OR
         || pNode->eType==FTS5_NOT  || pNode->eType==FTS5_EOF
    );
    if( pNode->eType==FTS5_STRING || pNode->eType==FTS5_TERM ){
      Fts5ExprNearset *pNear = pNode->pNear;
      if( pNear->pColset ){
        fts5MergeColset(pNear->pColset, pColset);
        if( pNear->pColset->nCol==0 ){
          pNode->eType = FTS5_EOF;
          pNode->xNext = 0;
        }
      }else if( *ppFree ){
        pNear->pColset = pColset;
        *ppFree = 0;
      }else{
        pNear->pColset = fts5CloneColset(&pParse->rc, pColset);
      }
    }else{
      int i;
      assert( pNode->eType!=FTS5_EOF || pNode->nChild==0 );
      for(i=0; i<pNode->nChild; i++){
        fts5ParseSetColset(pParse, pNode->apChild[i], pColset, ppFree);
      }
    }
  }
}

/*
** Apply colset pColset to expression node pExpr and all of its descendents.
*/
static void sqlite3Fts5ParseSetColset(
  Fts5Parse *pParse, 
  Fts5ExprNode *pExpr, 
  Fts5Colset *pColset 
){
  Fts5Colset *pFree = pColset;
  if( pParse->pConfig->eDetail==FTS5_DETAIL_NONE ){
    sqlite3Fts5ParseError(pParse, 
        "fts5: column queries are not supported (detail=none)"
    );
  }else{
    fts5ParseSetColset(pParse, pExpr, pColset, &pFree);
  }
  sqlite3_free(pFree);
}

static void fts5ExprAssignXNext(Fts5ExprNode *pNode){
  switch( pNode->eType ){
    case FTS5_STRING: {
      Fts5ExprNearset *pNear = pNode->pNear;
      if( pNear->nPhrase==1 && pNear->apPhrase[0]->nTerm==1 
       && pNear->apPhrase[0]->aTerm[0].pSynonym==0
       && pNear->apPhrase[0]->aTerm[0].bFirst==0
      ){
        pNode->eType = FTS5_TERM;
        pNode->xNext = fts5ExprNodeNext_TERM;
      }else{
        pNode->xNext = fts5ExprNodeNext_STRING;
      }
      break;
    };

    case FTS5_OR: {
      pNode->xNext = fts5ExprNodeNext_OR;
      break;
    };

    case FTS5_AND: {
      pNode->xNext = fts5ExprNodeNext_AND;
      break;
    };

    default: assert( pNode->eType==FTS5_NOT ); {
      pNode->xNext = fts5ExprNodeNext_NOT;
      break;
    };
  }
}

static void fts5ExprAddChildren(Fts5ExprNode *p, Fts5ExprNode *pSub){
  if( p->eType!=FTS5_NOT && pSub->eType==p->eType ){
    int nByte = sizeof(Fts5ExprNode*) * pSub->nChild;
    memcpy(&p->apChild[p->nChild], pSub->apChild, nByte);
    p->nChild += pSub->nChild;
    sqlite3_free(pSub);
  }else{
    p->apChild[p->nChild++] = pSub;
  }
}

/*
** This function is used when parsing LIKE or GLOB patterns against
** trigram indexes that specify either detail=column or detail=none.
** It converts a phrase:
**
**     abc + def + ghi
**
** into an AND tree:
**
**     abc AND def AND ghi
*/
static Fts5ExprNode *fts5ParsePhraseToAnd(
  Fts5Parse *pParse, 
  Fts5ExprNearset *pNear
){
  int nTerm = pNear->apPhrase[0]->nTerm;
  int ii;
  int nByte;
  Fts5ExprNode *pRet;

  assert( pNear->nPhrase==1 );
  assert( pParse->bPhraseToAnd );

  nByte = sizeof(Fts5ExprNode) + nTerm*sizeof(Fts5ExprNode*);
  pRet = (Fts5ExprNode*)sqlite3Fts5MallocZero(&pParse->rc, nByte);
  if( pRet ){
    pRet->eType = FTS5_AND;
    pRet->nChild = nTerm;
    fts5ExprAssignXNext(pRet);
    pParse->nPhrase--;
    for(ii=0; ii<nTerm; ii++){
      Fts5ExprPhrase *pPhrase = (Fts5ExprPhrase*)sqlite3Fts5MallocZero(
          &pParse->rc, sizeof(Fts5ExprPhrase)
      );
      if( pPhrase ){
        if( parseGrowPhraseArray(pParse) ){
          fts5ExprPhraseFree(pPhrase);
        }else{
          pParse->apPhrase[pParse->nPhrase++] = pPhrase;
          pPhrase->nTerm = 1;
          pPhrase->aTerm[0].zTerm = sqlite3Fts5Strndup(
              &pParse->rc, pNear->apPhrase[0]->aTerm[ii].zTerm, -1
          );
          pRet->apChild[ii] = sqlite3Fts5ParseNode(pParse, FTS5_STRING, 
              0, 0, sqlite3Fts5ParseNearset(pParse, 0, pPhrase)
          );
        }
      }
    }
  
    if( pParse->rc ){
      sqlite3Fts5ParseNodeFree(pRet);
      pRet = 0;
    }else{
      sqlite3Fts5ParseNearsetFree(pNear);
    }
  }

  return pRet;
}

/*
** Allocate and return a new expression object. If anything goes wrong (i.e.
** OOM error), leave an error code in pParse and return NULL.
*/
static Fts5ExprNode *sqlite3Fts5ParseNode(
  Fts5Parse *pParse,              /* Parse context */
  int eType,                      /* FTS5_STRING, AND, OR or NOT */
  Fts5ExprNode *pLeft,            /* Left hand child expression */
  Fts5ExprNode *pRight,           /* Right hand child expression */
  Fts5ExprNearset *pNear          /* For STRING expressions, the near cluster */
){
  Fts5ExprNode *pRet = 0;

  if( pParse->rc==SQLITE_OK ){
    int nChild = 0;               /* Number of children of returned node */
    sqlite3_int64 nByte;          /* Bytes of space to allocate for this node */
 
    assert( (eType!=FTS5_STRING && !pNear)
         || (eType==FTS5_STRING && !pLeft && !pRight)
    );
    if( eType==FTS5_STRING && pNear==0 ) return 0;
    if( eType!=FTS5_STRING && pLeft==0 ) return pRight;
    if( eType!=FTS5_STRING && pRight==0 ) return pLeft;

    if( eType==FTS5_STRING 
     && pParse->bPhraseToAnd 
     && pNear->apPhrase[0]->nTerm>1
    ){
      pRet = fts5ParsePhraseToAnd(pParse, pNear);
    }else{
      if( eType==FTS5_NOT ){
        nChild = 2;
      }else if( eType==FTS5_AND || eType==FTS5_OR ){
        nChild = 2;
        if( pLeft->eType==eType ) nChild += pLeft->nChild-1;
        if( pRight->eType==eType ) nChild += pRight->nChild-1;
      }

      nByte = sizeof(Fts5ExprNode) + sizeof(Fts5ExprNode*)*(nChild-1);
      pRet = (Fts5ExprNode*)sqlite3Fts5MallocZero(&pParse->rc, nByte);

      if( pRet ){
        pRet->eType = eType;
        pRet->pNear = pNear;
        fts5ExprAssignXNext(pRet);
        if( eType==FTS5_STRING ){
          int iPhrase;
          for(iPhrase=0; iPhrase<pNear->nPhrase; iPhrase++){
            pNear->apPhrase[iPhrase]->pNode = pRet;
            if( pNear->apPhrase[iPhrase]->nTerm==0 ){
              pRet->xNext = 0;
              pRet->eType = FTS5_EOF;
            }
          }

          if( pParse->pConfig->eDetail!=FTS5_DETAIL_FULL ){
            Fts5ExprPhrase *pPhrase = pNear->apPhrase[0];
            if( pNear->nPhrase!=1 
                || pPhrase->nTerm>1
                || (pPhrase->nTerm>0 && pPhrase->aTerm[0].bFirst)
              ){
              sqlite3Fts5ParseError(pParse, 
                  "fts5: %s queries are not supported (detail!=full)", 
                  pNear->nPhrase==1 ? "phrase": "NEAR"
              );
              sqlite3_free(pRet);
              pRet = 0;
            }
          }
        }else{
          fts5ExprAddChildren(pRet, pLeft);
          fts5ExprAddChildren(pRet, pRight);
        }
      }
    }
  }

  if( pRet==0 ){
    assert( pParse->rc!=SQLITE_OK );
    sqlite3Fts5ParseNodeFree(pLeft);
    sqlite3Fts5ParseNodeFree(pRight);
    sqlite3Fts5ParseNearsetFree(pNear);
  }
  return pRet;
}

static Fts5ExprNode *sqlite3Fts5ParseImplicitAnd(
  Fts5Parse *pParse,              /* Parse context */
  Fts5ExprNode *pLeft,            /* Left hand child expression */
  Fts5ExprNode *pRight            /* Right hand child expression */
){
  Fts5ExprNode *pRet = 0;
  Fts5ExprNode *pPrev;

  if( pParse->rc ){
    sqlite3Fts5ParseNodeFree(pLeft);
    sqlite3Fts5ParseNodeFree(pRight);
  }else{

    assert( pLeft->eType==FTS5_STRING 
        || pLeft->eType==FTS5_TERM
        || pLeft->eType==FTS5_EOF
        || pLeft->eType==FTS5_AND
    );
    assert( pRight->eType==FTS5_STRING 
        || pRight->eType==FTS5_TERM 
        || pRight->eType==FTS5_EOF 
    );

    if( pLeft->eType==FTS5_AND ){
      pPrev = pLeft->apChild[pLeft->nChild-1];
    }else{
      pPrev = pLeft;
    }
    assert( pPrev->eType==FTS5_STRING 
        || pPrev->eType==FTS5_TERM 
        || pPrev->eType==FTS5_EOF 
        );

    if( pRight->eType==FTS5_EOF ){
      assert( pParse->apPhrase[pParse->nPhrase-1]==pRight->pNear->apPhrase[0] );
      sqlite3Fts5ParseNodeFree(pRight);
      pRet = pLeft;
      pParse->nPhrase--;
    }
    else if( pPrev->eType==FTS5_EOF ){
      Fts5ExprPhrase **ap;

      if( pPrev==pLeft ){
        pRet = pRight;
      }else{
        pLeft->apChild[pLeft->nChild-1] = pRight;
        pRet = pLeft;
      }

      ap = &pParse->apPhrase[pParse->nPhrase-1-pRight->pNear->nPhrase];
      assert( ap[0]==pPrev->pNear->apPhrase[0] );
      memmove(ap, &ap[1], sizeof(Fts5ExprPhrase*)*pRight->pNear->nPhrase);
      pParse->nPhrase--;

      sqlite3Fts5ParseNodeFree(pPrev);
    }
    else{
      pRet = sqlite3Fts5ParseNode(pParse, FTS5_AND, pLeft, pRight, 0);
    }
  }

  return pRet;
}

#ifdef SQLITE_TEST
static char *fts5ExprTermPrint(Fts5ExprTerm *pTerm){
  sqlite3_int64 nByte = 0;
  Fts5ExprTerm *p;
  char *zQuoted;

  /* Determine the maximum amount of space required. */
  for(p=pTerm; p; p=p->pSynonym){
    nByte += (int)strlen(pTerm->zTerm) * 2 + 3 + 2;
  }
  zQuoted = sqlite3_malloc64(nByte);

  if( zQuoted ){
    int i = 0;
    for(p=pTerm; p; p=p->pSynonym){
      char *zIn = p->zTerm;
      zQuoted[i++] = '"';
      while( *zIn ){
        if( *zIn=='"' ) zQuoted[i++] = '"';
        zQuoted[i++] = *zIn++;
      }
      zQuoted[i++] = '"';
      if( p->pSynonym ) zQuoted[i++] = '|';
    }
    if( pTerm->bPrefix ){
      zQuoted[i++] = ' ';
      zQuoted[i++] = '*';
    }
    zQuoted[i++] = '\0';
  }
  return zQuoted;
}

static char *fts5PrintfAppend(char *zApp, const char *zFmt, ...){
  char *zNew;
  va_list ap;
  va_start(ap, zFmt);
  zNew = sqlite3_vmprintf(zFmt, ap);
  va_end(ap);
  if( zApp && zNew ){
    char *zNew2 = sqlite3_mprintf("%s%s", zApp, zNew);
    sqlite3_free(zNew);
    zNew = zNew2;
  }
  sqlite3_free(zApp);
  return zNew;
}

/*
** Compose a tcl-readable representation of expression pExpr. Return a 
** pointer to a buffer containing that representation. It is the 
** responsibility of the caller to at some point free the buffer using 
** sqlite3_free().
*/
static char *fts5ExprPrintTcl(
  Fts5Config *pConfig, 
  const char *zNearsetCmd,
  Fts5ExprNode *pExpr
){
  char *zRet = 0;
  if( pExpr->eType==FTS5_STRING || pExpr->eType==FTS5_TERM ){
    Fts5ExprNearset *pNear = pExpr->pNear;
    int i; 
    int iTerm;

    zRet = fts5PrintfAppend(zRet, "%s ", zNearsetCmd);
    if( zRet==0 ) return 0;
    if( pNear->pColset ){
      int *aiCol = pNear->pColset->aiCol;
      int nCol = pNear->pColset->nCol;
      if( nCol==1 ){
        zRet = fts5PrintfAppend(zRet, "-col %d ", aiCol[0]);
      }else{
        zRet = fts5PrintfAppend(zRet, "-col {%d", aiCol[0]);
        for(i=1; i<pNear->pColset->nCol; i++){
          zRet = fts5PrintfAppend(zRet, " %d", aiCol[i]);
        }
        zRet = fts5PrintfAppend(zRet, "} ");
      }
      if( zRet==0 ) return 0;
    }

    if( pNear->nPhrase>1 ){
      zRet = fts5PrintfAppend(zRet, "-near %d ", pNear->nNear);
      if( zRet==0 ) return 0;
    }

    zRet = fts5PrintfAppend(zRet, "--");
    if( zRet==0 ) return 0;

    for(i=0; i<pNear->nPhrase; i++){
      Fts5ExprPhrase *pPhrase = pNear->apPhrase[i];

      zRet = fts5PrintfAppend(zRet, " {");
      for(iTerm=0; zRet && iTerm<pPhrase->nTerm; iTerm++){
        char *zTerm = pPhrase->aTerm[iTerm].zTerm;
        zRet = fts5PrintfAppend(zRet, "%s%s", iTerm==0?"":" ", zTerm);
        if( pPhrase->aTerm[iTerm].bPrefix ){
          zRet = fts5PrintfAppend(zRet, "*");
        }
      }

      if( zRet ) zRet = fts5PrintfAppend(zRet, "}");
      if( zRet==0 ) return 0;
    }

  }else{
    char const *zOp = 0;
    int i;
    switch( pExpr->eType ){
      case FTS5_AND: zOp = "AND"; break;
      case FTS5_NOT: zOp = "NOT"; break;
      default: 
        assert( pExpr->eType==FTS5_OR );
        zOp = "OR"; 
        break;
    }

    zRet = sqlite3_mprintf("%s", zOp);
    for(i=0; zRet && i<pExpr->nChild; i++){
      char *z = fts5ExprPrintTcl(pConfig, zNearsetCmd, pExpr->apChild[i]);
      if( !z ){
        sqlite3_free(zRet);
        zRet = 0;
      }else{
        zRet = fts5PrintfAppend(zRet, " [%z]", z);
      }
    }
  }

  return zRet;
}

static char *fts5ExprPrint(Fts5Config *pConfig, Fts5ExprNode *pExpr){
  char *zRet = 0;
  if( pExpr->eType==0 ){
    return sqlite3_mprintf("\"\"");
  }else
  if( pExpr->eType==FTS5_STRING || pExpr->eType==FTS5_TERM ){
    Fts5ExprNearset *pNear = pExpr->pNear;
    int i; 
    int iTerm;

    if( pNear->pColset ){
      int ii;
      Fts5Colset *pColset = pNear->pColset;
      if( pColset->nCol>1 ) zRet = fts5PrintfAppend(zRet, "{");
      for(ii=0; ii<pColset->nCol; ii++){
        zRet = fts5PrintfAppend(zRet, "%s%s", 
            pConfig->azCol[pColset->aiCol[ii]], ii==pColset->nCol-1 ? "" : " "
        );
      }
      if( zRet ){
        zRet = fts5PrintfAppend(zRet, "%s : ", pColset->nCol>1 ? "}" : "");
      }
      if( zRet==0 ) return 0;
    }

    if( pNear->nPhrase>1 ){
      zRet = fts5PrintfAppend(zRet, "NEAR(");
      if( zRet==0 ) return 0;
    }

    for(i=0; i<pNear->nPhrase; i++){
      Fts5ExprPhrase *pPhrase = pNear->apPhrase[i];
      if( i!=0 ){
        zRet = fts5PrintfAppend(zRet, " ");
        if( zRet==0 ) return 0;
      }
      for(iTerm=0; iTerm<pPhrase->nTerm; iTerm++){
        char *zTerm = fts5ExprTermPrint(&pPhrase->aTerm[iTerm]);
        if( zTerm ){
          zRet = fts5PrintfAppend(zRet, "%s%s", iTerm==0?"":" + ", zTerm);
          sqlite3_free(zTerm);
        }
        if( zTerm==0 || zRet==0 ){
          sqlite3_free(zRet);
          return 0;
        }
      }
    }

    if( pNear->nPhrase>1 ){
      zRet = fts5PrintfAppend(zRet, ", %d)", pNear->nNear);
      if( zRet==0 ) return 0;
    }

  }else{
    char const *zOp = 0;
    int i;

    switch( pExpr->eType ){
      case FTS5_AND: zOp = " AND "; break;
      case FTS5_NOT: zOp = " NOT "; break;
      default:  
        assert( pExpr->eType==FTS5_OR );
        zOp = " OR "; 
        break;
    }

    for(i=0; i<pExpr->nChild; i++){
      char *z = fts5ExprPrint(pConfig, pExpr->apChild[i]);
      if( z==0 ){
        sqlite3_free(zRet);
        zRet = 0;
      }else{
        int e = pExpr->apChild[i]->eType;
        int b = (e!=FTS5_STRING && e!=FTS5_TERM && e!=FTS5_EOF);
        zRet = fts5PrintfAppend(zRet, "%s%s%z%s", 
            (i==0 ? "" : zOp),
            (b?"(":""), z, (b?")":"")
        );
      }
      if( zRet==0 ) break;
    }
  }

  return zRet;
}

/*
** The implementation of user-defined scalar functions fts5_expr() (bTcl==0)
** and fts5_expr_tcl() (bTcl!=0).
*/
static void fts5ExprFunction(
  sqlite3_context *pCtx,          /* Function call context */
  int nArg,                       /* Number of args */
  sqlite3_value **apVal,          /* Function arguments */
  int bTcl
){
  Fts5Global *pGlobal = (Fts5Global*)sqlite3_user_data(pCtx);
  sqlite3 *db = sqlite3_context_db_handle(pCtx);
  const char *zExpr = 0;
  char *zErr = 0;
  Fts5Expr *pExpr = 0;
  int rc;
  int i;

  const char **azConfig;          /* Array of arguments for Fts5Config */
  const char *zNearsetCmd = "nearset";
  int nConfig;                    /* Size of azConfig[] */
  Fts5Config *pConfig = 0;
  int iArg = 1;

  if( nArg<1 ){
    zErr = sqlite3_mprintf("wrong number of arguments to function %s",
        bTcl ? "fts5_expr_tcl" : "fts5_expr"
    );
    sqlite3_result_error(pCtx, zErr, -1);
    sqlite3_free(zErr);
    return;
  }

  if( bTcl && nArg>1 ){
    zNearsetCmd = (const char*)sqlite3_value_text(apVal[1]);
    iArg = 2;
  }

  nConfig = 3 + (nArg-iArg);
  azConfig = (const char**)sqlite3_malloc64(sizeof(char*) * nConfig);
  if( azConfig==0 ){
    sqlite3_result_error_nomem(pCtx);
    return;
  }
  azConfig[0] = 0;
  azConfig[1] = "main";
  azConfig[2] = "tbl";
  for(i=3; iArg<nArg; iArg++){
    const char *z = (const char*)sqlite3_value_text(apVal[iArg]);
    azConfig[i++] = (z ? z : "");
  }

  zExpr = (const char*)sqlite3_value_text(apVal[0]);
  if( zExpr==0 ) zExpr = "";

  rc = sqlite3Fts5ConfigParse(pGlobal, db, nConfig, azConfig, &pConfig, &zErr);
  if( rc==SQLITE_OK ){
    rc = sqlite3Fts5ExprNew(pConfig, 0, pConfig->nCol, zExpr, &pExpr, &zErr);
  }
  if( rc==SQLITE_OK ){
    char *zText;
    if( pExpr->pRoot->xNext==0 ){
      zText = sqlite3_mprintf("");
    }else if( bTcl ){
      zText = fts5ExprPrintTcl(pConfig, zNearsetCmd, pExpr->pRoot);
    }else{
      zText = fts5ExprPrint(pConfig, pExpr->pRoot);
    }
    if( zText==0 ){
      rc = SQLITE_NOMEM;
    }else{
      sqlite3_result_text(pCtx, zText, -1, SQLITE_TRANSIENT);
      sqlite3_free(zText);
    }
  }

  if( rc!=SQLITE_OK ){
    if( zErr ){
      sqlite3_result_error(pCtx, zErr, -1);
      sqlite3_free(zErr);
    }else{
      sqlite3_result_error_code(pCtx, rc);
    }
  }
  sqlite3_free((void *)azConfig);
  sqlite3Fts5ConfigFree(pConfig);
  sqlite3Fts5ExprFree(pExpr);
}

static void fts5ExprFunctionHr(
  sqlite3_context *pCtx,          /* Function call context */
  int nArg,                       /* Number of args */
  sqlite3_value **apVal           /* Function arguments */
){
  fts5ExprFunction(pCtx, nArg, apVal, 0);
}
static void fts5ExprFunctionTcl(
  sqlite3_context *pCtx,          /* Function call context */
  int nArg,                       /* Number of args */
  sqlite3_value **apVal           /* Function arguments */
){
  fts5ExprFunction(pCtx, nArg, apVal, 1);
}

/*
** The implementation of an SQLite user-defined-function that accepts a
** single integer as an argument. If the integer is an alpha-numeric 
** unicode code point, 1 is returned. Otherwise 0.
*/
static void fts5ExprIsAlnum(
  sqlite3_context *pCtx,          /* Function call context */
  int nArg,                       /* Number of args */
  sqlite3_value **apVal           /* Function arguments */
){
  int iCode;
  u8 aArr[32];
  if( nArg!=1 ){
    sqlite3_result_error(pCtx, 
        "wrong number of arguments to function fts5_isalnum", -1
    );
    return;
  }
  memset(aArr, 0, sizeof(aArr));
  sqlite3Fts5UnicodeCatParse("L*", aArr);
  sqlite3Fts5UnicodeCatParse("N*", aArr);
  sqlite3Fts5UnicodeCatParse("Co", aArr);
  iCode = sqlite3_value_int(apVal[0]);
  sqlite3_result_int(pCtx, aArr[sqlite3Fts5UnicodeCategory((u32)iCode)]);
}

static void fts5ExprFold(
  sqlite3_context *pCtx,          /* Function call context */
  int nArg,                       /* Number of args */
  sqlite3_value **apVal           /* Function arguments */
){
  if( nArg!=1 && nArg!=2 ){
    sqlite3_result_error(pCtx, 
        "wrong number of arguments to function fts5_fold", -1
    );
  }else{
    int iCode;
    int bRemoveDiacritics = 0;
    iCode = sqlite3_value_int(apVal[0]);
    if( nArg==2 ) bRemoveDiacritics = sqlite3_value_int(apVal[1]);
    sqlite3_result_int(pCtx, sqlite3Fts5UnicodeFold(iCode, bRemoveDiacritics));
  }
}
#endif /* ifdef SQLITE_TEST */

/*
** This is called during initialization to register the fts5_expr() scalar
** UDF with the SQLite handle passed as the only argument.
*/
static int sqlite3Fts5ExprInit(Fts5Global *pGlobal, sqlite3 *db){
#ifdef SQLITE_TEST
  struct Fts5ExprFunc {
    const char *z;
    void (*x)(sqlite3_context*,int,sqlite3_value**);
  } aFunc[] = {
    { "fts5_expr",     fts5ExprFunctionHr },
    { "fts5_expr_tcl", fts5ExprFunctionTcl },
    { "fts5_isalnum",  fts5ExprIsAlnum },
    { "fts5_fold",     fts5ExprFold },
  };
  int i;
  int rc = SQLITE_OK;
  void *pCtx = (void*)pGlobal;

  for(i=0; rc==SQLITE_OK && i<ArraySize(aFunc); i++){
    struct Fts5ExprFunc *p = &aFunc[i];
    rc = sqlite3_create_function(db, p->z, -1, SQLITE_UTF8, pCtx, p->x, 0, 0);
  }
#else
  int rc = SQLITE_OK;
  UNUSED_PARAM2(pGlobal,db);
#endif

  /* Avoid warnings indicating that sqlite3Fts5ParserTrace() and
  ** sqlite3Fts5ParserFallback() are unused */
#ifndef NDEBUG
  (void)sqlite3Fts5ParserTrace;
#endif
  (void)sqlite3Fts5ParserFallback;

  return rc;
}

/*
** Return the number of phrases in expression pExpr.
*/
static int sqlite3Fts5ExprPhraseCount(Fts5Expr *pExpr){
  return (pExpr ? pExpr->nPhrase : 0);
}

/*
** Return the number of terms in the iPhrase'th phrase in pExpr.
*/
static int sqlite3Fts5ExprPhraseSize(Fts5Expr *pExpr, int iPhrase){
  if( iPhrase<0 || iPhrase>=pExpr->nPhrase ) return 0;
  return pExpr->apExprPhrase[iPhrase]->nTerm;
}

/*
** This function is used to access the current position list for phrase
** iPhrase.
*/
static int sqlite3Fts5ExprPoslist(Fts5Expr *pExpr, int iPhrase, const u8 **pa){
  int nRet;
  Fts5ExprPhrase *pPhrase = pExpr->apExprPhrase[iPhrase];
  Fts5ExprNode *pNode = pPhrase->pNode;
  if( pNode->bEof==0 && pNode->iRowid==pExpr->pRoot->iRowid ){
    *pa = pPhrase->poslist.p;
    nRet = pPhrase->poslist.n;
  }else{
    *pa = 0;
    nRet = 0;
  }
  return nRet;
}

struct Fts5PoslistPopulator {
  Fts5PoslistWriter writer;
  int bOk;                        /* True if ok to populate */
  int bMiss;
};

/*
** Clear the position lists associated with all phrases in the expression
** passed as the first argument. Argument bLive is true if the expression
** might be pointing to a real entry, otherwise it has just been reset.
**
** At present this function is only used for detail=col and detail=none
** fts5 tables. This implies that all phrases must be at most 1 token
** in size, as phrase matches are not supported without detail=full.
*/
static Fts5PoslistPopulator *sqlite3Fts5ExprClearPoslists(Fts5Expr *pExpr, int bLive){
  Fts5PoslistPopulator *pRet;
  pRet = sqlite3_malloc64(sizeof(Fts5PoslistPopulator)*pExpr->nPhrase);
  if( pRet ){
    int i;
    memset(pRet, 0, sizeof(Fts5PoslistPopulator)*pExpr->nPhrase);
    for(i=0; i<pExpr->nPhrase; i++){
      Fts5Buffer *pBuf = &pExpr->apExprPhrase[i]->poslist;
      Fts5ExprNode *pNode = pExpr->apExprPhrase[i]->pNode;
      assert( pExpr->apExprPhrase[i]->nTerm<=1 );
      if( bLive && 
          (pBuf->n==0 || pNode->iRowid!=pExpr->pRoot->iRowid || pNode->bEof)
      ){
        pRet[i].bMiss = 1;
      }else{
        pBuf->n = 0;
      }
    }
  }
  return pRet;
}

struct Fts5ExprCtx {
  Fts5Expr *pExpr;
  Fts5PoslistPopulator *aPopulator;
  i64 iOff;
};
typedef struct Fts5ExprCtx Fts5ExprCtx;

/*
** TODO: Make this more efficient!
*/
static int fts5ExprColsetTest(Fts5Colset *pColset, int iCol){
  int i;
  for(i=0; i<pColset->nCol; i++){
    if( pColset->aiCol[i]==iCol ) return 1;
  }
  return 0;
}

static int fts5ExprPopulatePoslistsCb(
  void *pCtx,                /* Copy of 2nd argument to xTokenize() */
  int tflags,                /* Mask of FTS5_TOKEN_* flags */
  const char *pToken,        /* Pointer to buffer containing token */
  int nToken,                /* Size of token in bytes */
  int iUnused1,              /* Byte offset of token within input text */
  int iUnused2               /* Byte offset of end of token within input text */
){
  Fts5ExprCtx *p = (Fts5ExprCtx*)pCtx;
  Fts5Expr *pExpr = p->pExpr;
  int i;

  UNUSED_PARAM2(iUnused1, iUnused2);

  if( nToken>FTS5_MAX_TOKEN_SIZE ) nToken = FTS5_MAX_TOKEN_SIZE;
  if( (tflags & FTS5_TOKEN_COLOCATED)==0 ) p->iOff++;
  for(i=0; i<pExpr->nPhrase; i++){
    Fts5ExprTerm *pTerm;
    if( p->aPopulator[i].bOk==0 ) continue;
    for(pTerm=&pExpr->apExprPhrase[i]->aTerm[0]; pTerm; pTerm=pTerm->pSynonym){
      int nTerm = (int)strlen(pTerm->zTerm);
      if( (nTerm==nToken || (nTerm<nToken && pTerm->bPrefix))
       && memcmp(pTerm->zTerm, pToken, nTerm)==0
      ){
        int rc = sqlite3Fts5PoslistWriterAppend(
            &pExpr->apExprPhrase[i]->poslist, &p->aPopulator[i].writer, p->iOff
        );
        if( rc ) return rc;
        break;
      }
    }
  }
  return SQLITE_OK;
}

static int sqlite3Fts5ExprPopulatePoslists(
  Fts5Config *pConfig,
  Fts5Expr *pExpr, 
  Fts5PoslistPopulator *aPopulator,
  int iCol, 
  const char *z, int n
){
  int i;
  Fts5ExprCtx sCtx;
  sCtx.pExpr = pExpr;
  sCtx.aPopulator = aPopulator;
  sCtx.iOff = (((i64)iCol) << 32) - 1;

  for(i=0; i<pExpr->nPhrase; i++){
    Fts5ExprNode *pNode = pExpr->apExprPhrase[i]->pNode;
    Fts5Colset *pColset = pNode->pNear->pColset;
    if( (pColset && 0==fts5ExprColsetTest(pColset, iCol)) 
     || aPopulator[i].bMiss
    ){
      aPopulator[i].bOk = 0;
    }else{
      aPopulator[i].bOk = 1;
    }
  }

  return sqlite3Fts5Tokenize(pConfig, 
      FTS5_TOKENIZE_DOCUMENT, z, n, (void*)&sCtx, fts5ExprPopulatePoslistsCb
  );
}

static void fts5ExprClearPoslists(Fts5ExprNode *pNode){
  if( pNode->eType==FTS5_TERM || pNode->eType==FTS5_STRING ){
    pNode->pNear->apPhrase[0]->poslist.n = 0;
  }else{
    int i;
    for(i=0; i<pNode->nChild; i++){
      fts5ExprClearPoslists(pNode->apChild[i]);
    }
  }
}

static int fts5ExprCheckPoslists(Fts5ExprNode *pNode, i64 iRowid){
  pNode->iRowid = iRowid;
  pNode->bEof = 0;
  switch( pNode->eType ){
    case FTS5_TERM:
    case FTS5_STRING:
      return (pNode->pNear->apPhrase[0]->poslist.n>0);

    case FTS5_AND: {
      int i;
      for(i=0; i<pNode->nChild; i++){
        if( fts5ExprCheckPoslists(pNode->apChild[i], iRowid)==0 ){
          fts5ExprClearPoslists(pNode);
          return 0;
        }
      }
      break;
    }

    case FTS5_OR: {
      int i;
      int bRet = 0;
      for(i=0; i<pNode->nChild; i++){
        if( fts5ExprCheckPoslists(pNode->apChild[i], iRowid) ){
          bRet = 1;
        }
      }
      return bRet;
    }

    default: {
      assert( pNode->eType==FTS5_NOT );
      if( 0==fts5ExprCheckPoslists(pNode->apChild[0], iRowid)
          || 0!=fts5ExprCheckPoslists(pNode->apChild[1], iRowid)
        ){
        fts5ExprClearPoslists(pNode);
        return 0;
      }
      break;
    }
  }
  return 1;
}

static void sqlite3Fts5ExprCheckPoslists(Fts5Expr *pExpr, i64 iRowid){
  fts5ExprCheckPoslists(pExpr->pRoot, iRowid);
}

/*
** This function is only called for detail=columns tables. 
*/
static int sqlite3Fts5ExprPhraseCollist(
  Fts5Expr *pExpr, 
  int iPhrase, 
  const u8 **ppCollist, 
  int *pnCollist
){
  Fts5ExprPhrase *pPhrase = pExpr->apExprPhrase[iPhrase];
  Fts5ExprNode *pNode = pPhrase->pNode;
  int rc = SQLITE_OK;

  assert( iPhrase>=0 && iPhrase<pExpr->nPhrase );
  assert( pExpr->pConfig->eDetail==FTS5_DETAIL_COLUMNS );

  if( pNode->bEof==0 
   && pNode->iRowid==pExpr->pRoot->iRowid 
   && pPhrase->poslist.n>0
  ){
    Fts5ExprTerm *pTerm = &pPhrase->aTerm[0];
    if( pTerm->pSynonym ){
      Fts5Buffer *pBuf = (Fts5Buffer*)&pTerm->pSynonym[1];
      rc = fts5ExprSynonymList(
          pTerm, pNode->iRowid, pBuf, (u8**)ppCollist, pnCollist
      );
    }else{
      *ppCollist = pPhrase->aTerm[0].pIter->pData;
      *pnCollist = pPhrase->aTerm[0].pIter->nData;
    }
  }else{
    *ppCollist = 0;
    *pnCollist = 0;
  }

  return rc;
}

#line 1 "fts5_hash.c"
/*
** 2014 August 11
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
******************************************************************************
**
*/



/* #include "fts5Int.h" */

typedef struct Fts5HashEntry Fts5HashEntry;

/*
** This file contains the implementation of an in-memory hash table used
** to accumuluate "term -> doclist" content before it is flused to a level-0
** segment.
*/


struct Fts5Hash {
  int eDetail;                    /* Copy of Fts5Config.eDetail */
  int *pnByte;                    /* Pointer to bytes counter */
  int nEntry;                     /* Number of entries currently in hash */
  int nSlot;                      /* Size of aSlot[] array */
  Fts5HashEntry *pScan;           /* Current ordered scan item */
  Fts5HashEntry **aSlot;          /* Array of hash slots */
};

/*
** Each entry in the hash table is represented by an object of the 
** following type. Each object, its key (a nul-terminated string) and 
** its current data are stored in a single memory allocation. The 
** key immediately follows the object in memory. The position list
** data immediately follows the key data in memory.
**
** The data that follows the key is in a similar, but not identical format
** to the doclist data stored in the database. It is:
**
**   * Rowid, as a varint
**   * Position list, without 0x00 terminator.
**   * Size of previous position list and rowid, as a 4 byte
**     big-endian integer.
**
** iRowidOff:
**   Offset of last rowid written to data area. Relative to first byte of
**   structure.
**
** nData:
**   Bytes of data written since iRowidOff.
*/
struct Fts5HashEntry {
  Fts5HashEntry *pHashNext;       /* Next hash entry with same hash-key */
  Fts5HashEntry *pScanNext;       /* Next entry in sorted order */
  
  int nAlloc;                     /* Total size of allocation */
  int iSzPoslist;                 /* Offset of space for 4-byte poslist size */
  int nData;                      /* Total bytes of data (incl. structure) */
  int nKey;                       /* Length of key in bytes */
  u8 bDel;                        /* Set delete-flag @ iSzPoslist */
  u8 bContent;                    /* Set content-flag (detail=none mode) */
  i16 iCol;                       /* Column of last value written */
  int iPos;                       /* Position of last value written */
  i64 iRowid;                     /* Rowid of last value written */
};

/*
** Eqivalent to:
**
**   char *fts5EntryKey(Fts5HashEntry *pEntry){ return zKey; }
*/
#define fts5EntryKey(p) ( ((char *)(&(p)[1])) )


/*
** Allocate a new hash table.
*/
static int sqlite3Fts5HashNew(Fts5Config *pConfig, Fts5Hash **ppNew, int *pnByte){
  int rc = SQLITE_OK;
  Fts5Hash *pNew;

  *ppNew = pNew = (Fts5Hash*)sqlite3_malloc(sizeof(Fts5Hash));
  if( pNew==0 ){
    rc = SQLITE_NOMEM;
  }else{
    sqlite3_int64 nByte;
    memset(pNew, 0, sizeof(Fts5Hash));
    pNew->pnByte = pnByte;
    pNew->eDetail = pConfig->eDetail;

    pNew->nSlot = 1024;
    nByte = sizeof(Fts5HashEntry*) * pNew->nSlot;
    pNew->aSlot = (Fts5HashEntry**)sqlite3_malloc64(nByte);
    if( pNew->aSlot==0 ){
      sqlite3_free(pNew);
      *ppNew = 0;
      rc = SQLITE_NOMEM;
    }else{
      memset(pNew->aSlot, 0, (size_t)nByte);
    }
  }
  return rc;
}

/*
** Free a hash table object.
*/
static void sqlite3Fts5HashFree(Fts5Hash *pHash){
  if( pHash ){
    sqlite3Fts5HashClear(pHash);
    sqlite3_free(pHash->aSlot);
    sqlite3_free(pHash);
  }
}

/*
** Empty (but do not delete) a hash table.
*/
static void sqlite3Fts5HashClear(Fts5Hash *pHash){
  int i;
  for(i=0; i<pHash->nSlot; i++){
    Fts5HashEntry *pNext;
    Fts5HashEntry *pSlot;
    for(pSlot=pHash->aSlot[i]; pSlot; pSlot=pNext){
      pNext = pSlot->pHashNext;
      sqlite3_free(pSlot);
    }
  }
  memset(pHash->aSlot, 0, pHash->nSlot * sizeof(Fts5HashEntry*));
  pHash->nEntry = 0;
}

static unsigned int fts5HashKey(int nSlot, const u8 *p, int n){
  int i;
  unsigned int h = 13;
  for(i=n-1; i>=0; i--){
    h = (h << 3) ^ h ^ p[i];
  }
  return (h % nSlot);
}

static unsigned int fts5HashKey2(int nSlot, u8 b, const u8 *p, int n){
  int i;
  unsigned int h = 13;
  for(i=n-1; i>=0; i--){
    h = (h << 3) ^ h ^ p[i];
  }
  h = (h << 3) ^ h ^ b;
  return (h % nSlot);
}

/*
** Resize the hash table by doubling the number of slots.
*/
static int fts5HashResize(Fts5Hash *pHash){
  int nNew = pHash->nSlot*2;
  int i;
  Fts5HashEntry **apNew;
  Fts5HashEntry **apOld = pHash->aSlot;

  apNew = (Fts5HashEntry**)sqlite3_malloc64(nNew*sizeof(Fts5HashEntry*));
  if( !apNew ) return SQLITE_NOMEM;
  memset(apNew, 0, nNew*sizeof(Fts5HashEntry*));

  for(i=0; i<pHash->nSlot; i++){
    while( apOld[i] ){
      unsigned int iHash;
      Fts5HashEntry *p = apOld[i];
      apOld[i] = p->pHashNext;
      iHash = fts5HashKey(nNew, (u8*)fts5EntryKey(p),
                          (int)strlen(fts5EntryKey(p)));
      p->pHashNext = apNew[iHash];
      apNew[iHash] = p;
    }
  }

  sqlite3_free(apOld);
  pHash->nSlot = nNew;
  pHash->aSlot = apNew;
  return SQLITE_OK;
}

static int fts5HashAddPoslistSize(
  Fts5Hash *pHash, 
  Fts5HashEntry *p,
  Fts5HashEntry *p2
){
  int nRet = 0;
  if( p->iSzPoslist ){
    u8 *pPtr = p2 ? (u8*)p2 : (u8*)p;
    int nData = p->nData;
    if( pHash->eDetail==FTS5_DETAIL_NONE ){
      assert( nData==p->iSzPoslist );
      if( p->bDel ){
        pPtr[nData++] = 0x00;
        if( p->bContent ){
          pPtr[nData++] = 0x00;
        }
      }
    }else{
      int nSz = (nData - p->iSzPoslist - 1);       /* Size in bytes */
      int nPos = nSz*2 + p->bDel;                     /* Value of nPos field */

      assert( p->bDel==0 || p->bDel==1 );
      if( nPos<=127 ){
        pPtr[p->iSzPoslist] = (u8)nPos;
      }else{
        int nByte = sqlite3Fts5GetVarintLen((u32)nPos);
        memmove(&pPtr[p->iSzPoslist + nByte], &pPtr[p->iSzPoslist + 1], nSz);
        sqlite3Fts5PutVarint(&pPtr[p->iSzPoslist], nPos);
        nData += (nByte-1);
      }
    }

    nRet = nData - p->nData;
    if( p2==0 ){
      p->iSzPoslist = 0;
      p->bDel = 0;
      p->bContent = 0;
      p->nData = nData;
    }
  }
  return nRet;
}

/*
** Add an entry to the in-memory hash table. The key is the concatenation
** of bByte and (pToken/nToken). The value is (iRowid/iCol/iPos).
**
**     (bByte || pToken) -> (iRowid,iCol,iPos)
**
** Or, if iCol is negative, then the value is a delete marker.
*/
static int sqlite3Fts5HashWrite(
  Fts5Hash *pHash,
  i64 iRowid,                     /* Rowid for this entry */
  int iCol,                       /* Column token appears in (-ve -> delete) */
  int iPos,                       /* Position of token within column */
  char bByte,                     /* First byte of token */
  const char *pToken, int nToken  /* Token to add or remove to or from index */
){
  unsigned int iHash;
  Fts5HashEntry *p;
  u8 *pPtr;
  int nIncr = 0;                  /* Amount to increment (*pHash->pnByte) by */
  int bNew;                       /* If non-delete entry should be written */
  
  bNew = (pHash->eDetail==FTS5_DETAIL_FULL);

  /* Attempt to locate an existing hash entry */
  iHash = fts5HashKey2(pHash->nSlot, (u8)bByte, (const u8*)pToken, nToken);
  for(p=pHash->aSlot[iHash]; p; p=p->pHashNext){
    char *zKey = fts5EntryKey(p);
    if( zKey[0]==bByte 
     && p->nKey==nToken
     && memcmp(&zKey[1], pToken, nToken)==0 
    ){
      break;
    }
  }

  /* If an existing hash entry cannot be found, create a new one. */
  if( p==0 ){
    /* Figure out how much space to allocate */
    char *zKey;
    sqlite3_int64 nByte = sizeof(Fts5HashEntry) + (nToken+1) + 1 + 64;
    if( nByte<128 ) nByte = 128;

    /* Grow the Fts5Hash.aSlot[] array if necessary. */
    if( (pHash->nEntry*2)>=pHash->nSlot ){
      int rc = fts5HashResize(pHash);
      if( rc!=SQLITE_OK ) return rc;
      iHash = fts5HashKey2(pHash->nSlot, (u8)bByte, (const u8*)pToken, nToken);
    }

    /* Allocate new Fts5HashEntry and add it to the hash table. */
    p = (Fts5HashEntry*)sqlite3_malloc64(nByte);
    if( !p ) return SQLITE_NOMEM;
    memset(p, 0, sizeof(Fts5HashEntry));
    p->nAlloc = (int)nByte;
    zKey = fts5EntryKey(p);
    zKey[0] = bByte;
    memcpy(&zKey[1], pToken, nToken);
    assert( iHash==fts5HashKey(pHash->nSlot, (u8*)zKey, nToken+1) );
    p->nKey = nToken;
    zKey[nToken+1] = '\0';
    p->nData = nToken+1 + 1 + sizeof(Fts5HashEntry);
    p->pHashNext = pHash->aSlot[iHash];
    pHash->aSlot[iHash] = p;
    pHash->nEntry++;

    /* Add the first rowid field to the hash-entry */
    p->nData += sqlite3Fts5PutVarint(&((u8*)p)[p->nData], iRowid);
    p->iRowid = iRowid;

    p->iSzPoslist = p->nData;
    if( pHash->eDetail!=FTS5_DETAIL_NONE ){
      p->nData += 1;
      p->iCol = (pHash->eDetail==FTS5_DETAIL_FULL ? 0 : -1);
    }

  }else{

    /* Appending to an existing hash-entry. Check that there is enough 
    ** space to append the largest possible new entry. Worst case scenario 
    ** is:
    **
    **     + 9 bytes for a new rowid,
    **     + 4 byte reserved for the "poslist size" varint.
    **     + 1 byte for a "new column" byte,
    **     + 3 bytes for a new column number (16-bit max) as a varint,
    **     + 5 bytes for the new position offset (32-bit max).
    */
    if( (p->nAlloc - p->nData) < (9 + 4 + 1 + 3 + 5) ){
      sqlite3_int64 nNew = p->nAlloc * 2;
      Fts5HashEntry *pNew;
      Fts5HashEntry **pp;
      pNew = (Fts5HashEntry*)sqlite3_realloc64(p, nNew);
      if( pNew==0 ) return SQLITE_NOMEM;
      pNew->nAlloc = (int)nNew;
      for(pp=&pHash->aSlot[iHash]; *pp!=p; pp=&(*pp)->pHashNext);
      *pp = pNew;
      p = pNew;
    }
    nIncr -= p->nData;
  }
  assert( (p->nAlloc - p->nData) >= (9 + 4 + 1 + 3 + 5) );

  pPtr = (u8*)p;

  /* If this is a new rowid, append the 4-byte size field for the previous
  ** entry, and the new rowid for this entry.  */
  if( iRowid!=p->iRowid ){
    u64 iDiff = (u64)iRowid - (u64)p->iRowid;
    fts5HashAddPoslistSize(pHash, p, 0);
    p->nData += sqlite3Fts5PutVarint(&pPtr[p->nData], iDiff);
    p->iRowid = iRowid;
    bNew = 1;
    p->iSzPoslist = p->nData;
    if( pHash->eDetail!=FTS5_DETAIL_NONE ){
      p->nData += 1;
      p->iCol = (pHash->eDetail==FTS5_DETAIL_FULL ? 0 : -1);
      p->iPos = 0;
    }
  }

  if( iCol>=0 ){
    if( pHash->eDetail==FTS5_DETAIL_NONE ){
      p->bContent = 1;
    }else{
      /* Append a new column value, if necessary */
      assert_nc( iCol>=p->iCol );
      if( iCol!=p->iCol ){
        if( pHash->eDetail==FTS5_DETAIL_FULL ){
          pPtr[p->nData++] = 0x01;
          p->nData += sqlite3Fts5PutVarint(&pPtr[p->nData], iCol);
          p->iCol = (i16)iCol;
          p->iPos = 0;
        }else{
          bNew = 1;
          p->iCol = (i16)(iPos = iCol);
        }
      }

      /* Append the new position offset, if necessary */
      if( bNew ){
        p->nData += sqlite3Fts5PutVarint(&pPtr[p->nData], iPos - p->iPos + 2);
        p->iPos = iPos;
      }
    }
  }else{
    /* This is a delete. Set the delete flag. */
    p->bDel = 1;
  }

  nIncr += p->nData;
  *pHash->pnByte += nIncr;
  return SQLITE_OK;
}


/*
** Arguments pLeft and pRight point to linked-lists of hash-entry objects,
** each sorted in key order. This function merges the two lists into a
** single list and returns a pointer to its first element.
*/
static Fts5HashEntry *fts5HashEntryMerge(
  Fts5HashEntry *pLeft,
  Fts5HashEntry *pRight
){
  Fts5HashEntry *p1 = pLeft;
  Fts5HashEntry *p2 = pRight;
  Fts5HashEntry *pRet = 0;
  Fts5HashEntry **ppOut = &pRet;

  while( p1 || p2 ){
    if( p1==0 ){
      *ppOut = p2;
      p2 = 0;
    }else if( p2==0 ){
      *ppOut = p1;
      p1 = 0;
    }else{
      int i = 0;
      char *zKey1 = fts5EntryKey(p1);
      char *zKey2 = fts5EntryKey(p2);
      while( zKey1[i]==zKey2[i] ) i++;

      if( ((u8)zKey1[i])>((u8)zKey2[i]) ){
        /* p2 is smaller */
        *ppOut = p2;
        ppOut = &p2->pScanNext;
        p2 = p2->pScanNext;
      }else{
        /* p1 is smaller */
        *ppOut = p1;
        ppOut = &p1->pScanNext;
        p1 = p1->pScanNext;
      }
      *ppOut = 0;
    }
  }

  return pRet;
}

/*
** Extract all tokens from hash table iHash and link them into a list
** in sorted order. The hash table is cleared before returning. It is
** the responsibility of the caller to free the elements of the returned
** list.
*/
static int fts5HashEntrySort(
  Fts5Hash *pHash, 
  const char *pTerm, int nTerm,   /* Query prefix, if any */
  Fts5HashEntry **ppSorted
){
  const int nMergeSlot = 32;
  Fts5HashEntry **ap;
  Fts5HashEntry *pList;
  int iSlot;
  int i;

  *ppSorted = 0;
  ap = sqlite3_malloc64(sizeof(Fts5HashEntry*) * nMergeSlot);
  if( !ap ) return SQLITE_NOMEM;
  memset(ap, 0, sizeof(Fts5HashEntry*) * nMergeSlot);

  for(iSlot=0; iSlot<pHash->nSlot; iSlot++){
    Fts5HashEntry *pIter;
    for(pIter=pHash->aSlot[iSlot]; pIter; pIter=pIter->pHashNext){
      if( pTerm==0 
       || (pIter->nKey+1>=nTerm && 0==memcmp(fts5EntryKey(pIter), pTerm, nTerm))
      ){
        Fts5HashEntry *pEntry = pIter;
        pEntry->pScanNext = 0;
        for(i=0; ap[i]; i++){
          pEntry = fts5HashEntryMerge(pEntry, ap[i]);
          ap[i] = 0;
        }
        ap[i] = pEntry;
      }
    }
  }

  pList = 0;
  for(i=0; i<nMergeSlot; i++){
    pList = fts5HashEntryMerge(pList, ap[i]);
  }

  pHash->nEntry = 0;
  sqlite3_free(ap);
  *ppSorted = pList;
  return SQLITE_OK;
}

/*
** Query the hash table for a doclist associated with term pTerm/nTerm.
*/
static int sqlite3Fts5HashQuery(
  Fts5Hash *pHash,                /* Hash table to query */
  int nPre,
  const char *pTerm, int nTerm,   /* Query term */
  void **ppOut,                   /* OUT: Pointer to new object */
  int *pnDoclist                  /* OUT: Size of doclist in bytes */
){
  unsigned int iHash = fts5HashKey(pHash->nSlot, (const u8*)pTerm, nTerm);
  char *zKey = 0;
  Fts5HashEntry *p;

  for(p=pHash->aSlot[iHash]; p; p=p->pHashNext){
    zKey = fts5EntryKey(p);
    assert( p->nKey+1==(int)strlen(zKey) );
    if( nTerm==p->nKey+1 && memcmp(zKey, pTerm, nTerm)==0 ) break;
  }

  if( p ){
    int nHashPre = sizeof(Fts5HashEntry) + nTerm + 1;
    int nList = p->nData - nHashPre;
    u8 *pRet = (u8*)(*ppOut = sqlite3_malloc64(nPre + nList + 10));
    if( pRet ){
      Fts5HashEntry *pFaux = (Fts5HashEntry*)&pRet[nPre-nHashPre];
      memcpy(&pRet[nPre], &((u8*)p)[nHashPre], nList);
      nList += fts5HashAddPoslistSize(pHash, p, pFaux);
      *pnDoclist = nList;
    }else{
      *pnDoclist = 0;
      return SQLITE_NOMEM;
    }
  }else{
    *ppOut = 0;
    *pnDoclist = 0;
  }

  return SQLITE_OK;
}

static int sqlite3Fts5HashScanInit(
  Fts5Hash *p,                    /* Hash table to query */
  const char *pTerm, int nTerm    /* Query prefix */
){
  return fts5HashEntrySort(p, pTerm, nTerm, &p->pScan);
}

static void sqlite3Fts5HashScanNext(Fts5Hash *p){
  assert( !sqlite3Fts5HashScanEof(p) );
  p->pScan = p->pScan->pScanNext;
}

static int sqlite3Fts5HashScanEof(Fts5Hash *p){
  return (p->pScan==0);
}

static void sqlite3Fts5HashScanEntry(
  Fts5Hash *pHash,
  const char **pzTerm,            /* OUT: term (nul-terminated) */
  const u8 **ppDoclist,           /* OUT: pointer to doclist */
  int *pnDoclist                  /* OUT: size of doclist in bytes */
){
  Fts5HashEntry *p;
  if( (p = pHash->pScan) ){
    char *zKey = fts5EntryKey(p);
    int nTerm = (int)strlen(zKey);
    fts5HashAddPoslistSize(pHash, p, 0);
    *pzTerm = zKey;
    *ppDoclist = (const u8*)&zKey[nTerm+1];
    *pnDoclist = p->nData - (sizeof(Fts5HashEntry) + nTerm + 1);
  }else{
    *pzTerm = 0;
    *ppDoclist = 0;
    *pnDoclist = 0;
  }
}

#line 1 "fts5_index.c"
/*
** 2014 May 31
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
******************************************************************************
**
** Low level access to the FTS index stored in the database file. The 
** routines in this file file implement all read and write access to the
** %_data table. Other parts of the system access this functionality via
** the interface defined in fts5Int.h.
*/


/* #include "fts5Int.h" */

/*
** Overview:
**
** The %_data table contains all the FTS indexes for an FTS5 virtual table.
** As well as the main term index, there may be up to 31 prefix indexes.
** The format is similar to FTS3/4, except that:
**
**   * all segment b-tree leaf data is stored in fixed size page records 
**     (e.g. 1000 bytes). A single doclist may span multiple pages. Care is 
**     taken to ensure it is possible to iterate in either direction through 
**     the entries in a doclist, or to seek to a specific entry within a 
**     doclist, without loading it into memory.
**
**   * large doclists that span many pages have associated "doclist index"
**     records that contain a copy of the first rowid on each page spanned by
**     the doclist. This is used to speed up seek operations, and merges of
**     large doclists with very small doclists.
**
**   * extra fields in the "structure record" record the state of ongoing
**     incremental merge operations.
**
*/


#define FTS5_OPT_WORK_UNIT  1000  /* Number of leaf pages per optimize step */
#define FTS5_WORK_UNIT      64    /* Number of leaf pages in unit of work */

#define FTS5_MIN_DLIDX_SIZE 4     /* Add dlidx if this many empty pages */

#define FTS5_MAIN_PREFIX '0'

#if FTS5_MAX_PREFIX_INDEXES > 31
# error "FTS5_MAX_PREFIX_INDEXES is too large"
#endif

/*
** Details:
**
** The %_data table managed by this module,
**
**     CREATE TABLE %_data(id INTEGER PRIMARY KEY, block BLOB);
**
** , contains the following 5 types of records. See the comments surrounding
** the FTS5_*_ROWID macros below for a description of how %_data rowids are 
** assigned to each fo them.
**
** 1. Structure Records:
**
**   The set of segments that make up an index - the index structure - are
**   recorded in a single record within the %_data table. The record consists
**   of a single 32-bit configuration cookie value followed by a list of 
**   SQLite varints. If the FTS table features more than one index (because
**   there are one or more prefix indexes), it is guaranteed that all share
**   the same cookie value.
**
**   Immediately following the configuration cookie, the record begins with
**   three varints:
**
**     + number of levels,
**     + total number of segments on all levels,
**     + value of write counter.
**
**   Then, for each level from 0 to nMax:
**
**     + number of input segments in ongoing merge.
**     + total number of segments in level.
**     + for each segment from oldest to newest:
**         + segment id (always > 0)
**         + first leaf page number (often 1, always greater than 0)
**         + final leaf page number
**
** 2. The Averages Record:
**
**   A single record within the %_data table. The data is a list of varints.
**   The first value is the number of rows in the index. Then, for each column
**   from left to right, the total number of tokens in the column for all
**   rows of the table.
**
** 3. Segment leaves:
**
**   TERM/DOCLIST FORMAT:
**
**     Most of each segment leaf is taken up by term/doclist data. The 
**     general format of term/doclist, starting with the first term
**     on the leaf page, is:
**
**         varint : size of first term
**         blob:    first term data
**         doclist: first doclist
**         zero-or-more {
**           varint:  number of bytes in common with previous term
**           varint:  number of bytes of new term data (nNew)
**           blob:    nNew bytes of new term data
**           doclist: next doclist
**         }
**
**     doclist format:
**
**         varint:  first rowid
**         poslist: first poslist
**         zero-or-more {
**           varint:  rowid delta (always > 0)
**           poslist: next poslist
**         }
**
**     poslist format:
**
**         varint: size of poslist in bytes multiplied by 2, not including
**                 this field. Plus 1 if this entry carries the "delete" flag.
**         collist: collist for column 0
**         zero-or-more {
**           0x01 byte
**           varint: column number (I)
**           collist: collist for column I
**         }
**
**     collist format:
**
**         varint: first offset + 2
**         zero-or-more {
**           varint: offset delta + 2
**         }
**
**   PAGE FORMAT
**
**     Each leaf page begins with a 4-byte header containing 2 16-bit 
**     unsigned integer fields in big-endian format. They are:
**
**       * The byte offset of the first rowid on the page, if it exists
**         and occurs before the first term (otherwise 0).
**
**       * The byte offset of the start of the page footer. If the page
**         footer is 0 bytes in size, then this field is the same as the
**         size of the leaf page in bytes.
**
**     The page footer consists of a single varint for each term located
**     on the page. Each varint is the byte offset of the current term
**     within the page, delta-compressed against the previous value. In
**     other words, the first varint in the footer is the byte offset of
**     the first term, the second is the byte offset of the second less that
**     of the first, and so on.
**
**     The term/doclist format described above is accurate if the entire
**     term/doclist data fits on a single leaf page. If this is not the case,
**     the format is changed in two ways:
**
**       + if the first rowid on a page occurs before the first term, it
**         is stored as a literal value:
**
**             varint:  first rowid
**
**       + the first term on each page is stored in the same way as the
**         very first term of the segment:
**
**             varint : size of first term
**             blob:    first term data
**
** 5. Segment doclist indexes:
**
**   Doclist indexes are themselves b-trees, however they usually consist of
**   a single leaf record only. The format of each doclist index leaf page 
**   is:
**
**     * Flags byte. Bits are:
**         0x01: Clear if leaf is also the root page, otherwise set.
**
**     * Page number of fts index leaf page. As a varint.
**
**     * First rowid on page indicated by previous field. As a varint.
**
**     * A list of varints, one for each subsequent termless page. A 
**       positive delta if the termless page contains at least one rowid, 
**       or an 0x00 byte otherwise.
**
**   Internal doclist index nodes are:
**
**     * Flags byte. Bits are:
**         0x01: Clear for root page, otherwise set.
**
**     * Page number of first child page. As a varint.
**
**     * Copy of first rowid on page indicated by previous field. As a varint.
**
**     * A list of delta-encoded varints - the first rowid on each subsequent
**       child page. 
**
*/

/*
** Rowids for the averages and structure records in the %_data table.
*/
#define FTS5_AVERAGES_ROWID     1    /* Rowid used for the averages record */
#define FTS5_STRUCTURE_ROWID   10    /* The structure record */

/*
** Macros determining the rowids used by segment leaves and dlidx leaves
** and nodes. All nodes and leaves are stored in the %_data table with large
** positive rowids.
**
** Each segment has a unique non-zero 16-bit id.
**
** The rowid for each segment leaf is found by passing the segment id and 
** the leaf page number to the FTS5_SEGMENT_ROWID macro. Leaves are numbered
** sequentially starting from 1.
*/
#define FTS5_DATA_ID_B     16     /* Max seg id number 65535 */
#define FTS5_DATA_DLI_B     1     /* Doclist-index flag (1 bit) */
#define FTS5_DATA_HEIGHT_B  5     /* Max dlidx tree height of 32 */
#define FTS5_DATA_PAGE_B   31     /* Max page number of 2147483648 */

#define fts5_dri(segid, dlidx, height, pgno) (                                 \
 ((i64)(segid)  << (FTS5_DATA_PAGE_B+FTS5_DATA_HEIGHT_B+FTS5_DATA_DLI_B)) +    \
 ((i64)(dlidx)  << (FTS5_DATA_PAGE_B + FTS5_DATA_HEIGHT_B)) +                  \
 ((i64)(height) << (FTS5_DATA_PAGE_B)) +                                       \
 ((i64)(pgno))                                                                 \
)

#define FTS5_SEGMENT_ROWID(segid, pgno)       fts5_dri(segid, 0, 0, pgno)
#define FTS5_DLIDX_ROWID(segid, height, pgno) fts5_dri(segid, 1, height, pgno)

#ifdef SQLITE_DEBUG
static int sqlite3Fts5Corrupt() { return SQLITE_CORRUPT_VTAB; }
#endif


/*
** Each time a blob is read from the %_data table, it is padded with this
** many zero bytes. This makes it easier to decode the various record formats
** without overreading if the records are corrupt.
*/
#define FTS5_DATA_ZERO_PADDING 8
#define FTS5_DATA_PADDING 20

typedef struct Fts5Data Fts5Data;
typedef struct Fts5DlidxIter Fts5DlidxIter;
typedef struct Fts5DlidxLvl Fts5DlidxLvl;
typedef struct Fts5DlidxWriter Fts5DlidxWriter;
typedef struct Fts5Iter Fts5Iter;
typedef struct Fts5PageWriter Fts5PageWriter;
typedef struct Fts5SegIter Fts5SegIter;
typedef struct Fts5DoclistIter Fts5DoclistIter;
typedef struct Fts5SegWriter Fts5SegWriter;
typedef struct Fts5Structure Fts5Structure;
typedef struct Fts5StructureLevel Fts5StructureLevel;
typedef struct Fts5StructureSegment Fts5StructureSegment;

struct Fts5Data {
  u8 *p;                          /* Pointer to buffer containing record */
  int nn;                         /* Size of record in bytes */
  int szLeaf;                     /* Size of leaf without page-index */
};

/*
** One object per %_data table.
*/
struct Fts5Index {
  Fts5Config *pConfig;            /* Virtual table configuration */
  char *zDataTbl;                 /* Name of %_data table */
  int nWorkUnit;                  /* Leaf pages in a "unit" of work */

  /*
  ** Variables related to the accumulation of tokens and doclists within the
  ** in-memory hash tables before they are flushed to disk.
  */
  Fts5Hash *pHash;                /* Hash table for in-memory data */
  int nPendingData;               /* Current bytes of pending data */
  i64 iWriteRowid;                /* Rowid for current doc being written */
  int bDelete;                    /* Current write is a delete */

  /* Error state. */
  int rc;                         /* Current error code */

  /* State used by the fts5DataXXX() functions. */
  sqlite3_blob *pReader;          /* RO incr-blob open on %_data table */
  sqlite3_stmt *pWriter;          /* "INSERT ... %_data VALUES(?,?)" */
  sqlite3_stmt *pDeleter;         /* "DELETE FROM %_data ... id>=? AND id<=?" */
  sqlite3_stmt *pIdxWriter;       /* "INSERT ... %_idx VALUES(?,?,?,?)" */
  sqlite3_stmt *pIdxDeleter;      /* "DELETE FROM %_idx WHERE segid=?" */
  sqlite3_stmt *pIdxSelect;
  int nRead;                      /* Total number of blocks read */

  sqlite3_stmt *pDataVersion;
  i64 iStructVersion;             /* data_version when pStruct read */
  Fts5Structure *pStruct;         /* Current db structure (or NULL) */
};

struct Fts5DoclistIter {
  u8 *aEof;                       /* Pointer to 1 byte past end of doclist */

  /* Output variables. aPoslist==0 at EOF */
  i64 iRowid;
  u8 *aPoslist;
  int nPoslist;
  int nSize;
};

/*
** The contents of the "structure" record for each index are represented
** using an Fts5Structure record in memory. Which uses instances of the 
** other Fts5StructureXXX types as components.
*/
struct Fts5StructureSegment {
  int iSegid;                     /* Segment id */
  int pgnoFirst;                  /* First leaf page number in segment */
  int pgnoLast;                   /* Last leaf page number in segment */
};
struct Fts5StructureLevel {
  int nMerge;                     /* Number of segments in incr-merge */
  int nSeg;                       /* Total number of segments on level */
  Fts5StructureSegment *aSeg;     /* Array of segments. aSeg[0] is oldest. */
};
struct Fts5Structure {
  int nRef;                       /* Object reference count */
  u64 nWriteCounter;              /* Total leaves written to level 0 */
  int nSegment;                   /* Total segments in this structure */
  int nLevel;                     /* Number of levels in this index */
  Fts5StructureLevel aLevel[1];   /* Array of nLevel level objects */
};

/*
** An object of type Fts5SegWriter is used to write to segments.
*/
struct Fts5PageWriter {
  int pgno;                       /* Page number for this page */
  int iPrevPgidx;                 /* Previous value written into pgidx */
  Fts5Buffer buf;                 /* Buffer containing leaf data */
  Fts5Buffer pgidx;               /* Buffer containing page-index */
  Fts5Buffer term;                /* Buffer containing previous term on page */
};
struct Fts5DlidxWriter {
  int pgno;                       /* Page number for this page */
  int bPrevValid;                 /* True if iPrev is valid */
  i64 iPrev;                      /* Previous rowid value written to page */
  Fts5Buffer buf;                 /* Buffer containing page data */
};
struct Fts5SegWriter {
  int iSegid;                     /* Segid to write to */
  Fts5PageWriter writer;          /* PageWriter object */
  i64 iPrevRowid;                 /* Previous rowid written to current leaf */
  u8 bFirstRowidInDoclist;        /* True if next rowid is first in doclist */
  u8 bFirstRowidInPage;           /* True if next rowid is first in page */
  /* TODO1: Can use (writer.pgidx.n==0) instead of bFirstTermInPage */
  u8 bFirstTermInPage;            /* True if next term will be first in leaf */
  int nLeafWritten;               /* Number of leaf pages written */
  int nEmpty;                     /* Number of contiguous term-less nodes */

  int nDlidx;                     /* Allocated size of aDlidx[] array */
  Fts5DlidxWriter *aDlidx;        /* Array of Fts5DlidxWriter objects */

  /* Values to insert into the %_idx table */
  Fts5Buffer btterm;              /* Next term to insert into %_idx table */
  int iBtPage;                    /* Page number corresponding to btterm */
};

typedef struct Fts5CResult Fts5CResult;
struct Fts5CResult {
  u16 iFirst;                     /* aSeg[] index of firstest iterator */
  u8 bTermEq;                     /* True if the terms are equal */
};

/*
** Object for iterating through a single segment, visiting each term/rowid
** pair in the segment.
**
** pSeg:
**   The segment to iterate through.
**
** iLeafPgno:
**   Current leaf page number within segment.
**
** iLeafOffset:
**   Byte offset within the current leaf that is the first byte of the 
**   position list data (one byte passed the position-list size field).
**   rowid field of the current entry. Usually this is the size field of the
**   position list data. The exception is if the rowid for the current entry 
**   is the last thing on the leaf page.
**
** pLeaf:
**   Buffer containing current leaf page data. Set to NULL at EOF.
**
** iTermLeafPgno, iTermLeafOffset:
**   Leaf page number containing the last term read from the segment. And
**   the offset immediately following the term data.
**
** flags:
**   Mask of FTS5_SEGITER_XXX values. Interpreted as follows:
**
**   FTS5_SEGITER_ONETERM:
**     If set, set the iterator to point to EOF after the current doclist 
**     has been exhausted. Do not proceed to the next term in the segment.
**
**   FTS5_SEGITER_REVERSE:
**     This flag is only ever set if FTS5_SEGITER_ONETERM is also set. If
**     it is set, iterate through rowid in descending order instead of the
**     default ascending order.
**
** iRowidOffset/nRowidOffset/aRowidOffset:
**     These are used if the FTS5_SEGITER_REVERSE flag is set.
**
**     For each rowid on the page corresponding to the current term, the
**     corresponding aRowidOffset[] entry is set to the byte offset of the
**     start of the "position-list-size" field within the page.
**
** iTermIdx:
**     Index of current term on iTermLeafPgno.
*/
struct Fts5SegIter {
  Fts5StructureSegment *pSeg;     /* Segment to iterate through */
  int flags;                      /* Mask of configuration flags */
  int iLeafPgno;                  /* Current leaf page number */
  Fts5Data *pLeaf;                /* Current leaf data */
  Fts5Data *pNextLeaf;            /* Leaf page (iLeafPgno+1) */
  i64 iLeafOffset;                /* Byte offset within current leaf */

  /* Next method */
  void (*xNext)(Fts5Index*, Fts5SegIter*, int*);

  /* The page and offset from which the current term was read. The offset 
  ** is the offset of the first rowid in the current doclist.  */
  int iTermLeafPgno;
  int iTermLeafOffset;

  int iPgidxOff;                  /* Next offset in pgidx */
  int iEndofDoclist;

  /* The following are only used if the FTS5_SEGITER_REVERSE flag is set. */
  int iRowidOffset;               /* Current entry in aRowidOffset[] */
  int nRowidOffset;               /* Allocated size of aRowidOffset[] array */
  int *aRowidOffset;              /* Array of offset to rowid fields */

  Fts5DlidxIter *pDlidx;          /* If there is a doclist-index */

  /* Variables populated based on current entry. */
  Fts5Buffer term;                /* Current term */
  i64 iRowid;                     /* Current rowid */
  int nPos;                       /* Number of bytes in current position list */
  u8 bDel;                        /* True if the delete flag is set */
};

/*
** Argument is a pointer to an Fts5Data structure that contains a 
** leaf page.
*/
#define ASSERT_SZLEAF_OK(x) assert( \
    (x)->szLeaf==(x)->nn || (x)->szLeaf==fts5GetU16(&(x)->p[2]) \
)

#define FTS5_SEGITER_ONETERM 0x01
#define FTS5_SEGITER_REVERSE 0x02

/* 
** Argument is a pointer to an Fts5Data structure that contains a leaf
** page. This macro evaluates to true if the leaf contains no terms, or
** false if it contains at least one term.
*/
#define fts5LeafIsTermless(x) ((x)->szLeaf >= (x)->nn)

#define fts5LeafTermOff(x, i) (fts5GetU16(&(x)->p[(x)->szLeaf + (i)*2]))

#define fts5LeafFirstRowidOff(x) (fts5GetU16((x)->p))

/*
** Object for iterating through the merged results of one or more segments,
** visiting each term/rowid pair in the merged data.
**
** nSeg is always a power of two greater than or equal to the number of
** segments that this object is merging data from. Both the aSeg[] and
** aFirst[] arrays are sized at nSeg entries. The aSeg[] array is padded
** with zeroed objects - these are handled as if they were iterators opened
** on empty segments.
**
** The results of comparing segments aSeg[N] and aSeg[N+1], where N is an
** even number, is stored in aFirst[(nSeg+N)/2]. The "result" of the 
** comparison in this context is the index of the iterator that currently
** points to the smaller term/rowid combination. Iterators at EOF are
** considered to be greater than all other iterators.
**
** aFirst[1] contains the index in aSeg[] of the iterator that points to
** the smallest key overall. aFirst[0] is unused. 
**
** poslist:
**   Used by sqlite3Fts5IterPoslist() when the poslist needs to be buffered.
**   There is no way to tell if this is populated or not.
*/
struct Fts5Iter {
  Fts5IndexIter base;             /* Base class containing output vars */

  Fts5Index *pIndex;              /* Index that owns this iterator */
  Fts5Buffer poslist;             /* Buffer containing current poslist */
  Fts5Colset *pColset;            /* Restrict matches to these columns */

  /* Invoked to set output variables. */
  void (*xSetOutputs)(Fts5Iter*, Fts5SegIter*);

  int nSeg;                       /* Size of aSeg[] array */
  int bRev;                       /* True to iterate in reverse order */
  u8 bSkipEmpty;                  /* True to skip deleted entries */

  i64 iSwitchRowid;               /* Firstest rowid of other than aFirst[1] */
  Fts5CResult *aFirst;            /* Current merge state (see above) */
  Fts5SegIter aSeg[1];            /* Array of segment iterators */
};


/*
** An instance of the following type is used to iterate through the contents
** of a doclist-index record.
**
** pData:
**   Record containing the doclist-index data.
**
** bEof:
**   Set to true once iterator has reached EOF.
**
** iOff:
**   Set to the current offset within record pData.
*/
struct Fts5DlidxLvl {
  Fts5Data *pData;              /* Data for current page of this level */
  int iOff;                     /* Current offset into pData */
  int bEof;                     /* At EOF already */
  int iFirstOff;                /* Used by reverse iterators */

  /* Output variables */
  int iLeafPgno;                /* Page number of current leaf page */
  i64 iRowid;                   /* First rowid on leaf iLeafPgno */
};
struct Fts5DlidxIter {
  int nLvl;
  int iSegid;
  Fts5DlidxLvl aLvl[1];
};

static void fts5PutU16(u8 *aOut, u16 iVal){
  aOut[0] = (iVal>>8);
  aOut[1] = (iVal&0xFF);
}

static u16 fts5GetU16(const u8 *aIn){
  return ((u16)aIn[0] << 8) + aIn[1];
} 

/*
** Allocate and return a buffer at least nByte bytes in size.
**
** If an OOM error is encountered, return NULL and set the error code in
** the Fts5Index handle passed as the first argument.
*/
static void *fts5IdxMalloc(Fts5Index *p, sqlite3_int64 nByte){
  return sqlite3Fts5MallocZero(&p->rc, nByte);
}

/*
** Compare the contents of the pLeft buffer with the pRight/nRight blob.
**
** Return -ve if pLeft is smaller than pRight, 0 if they are equal or
** +ve if pRight is smaller than pLeft. In other words:
**
**     res = *pLeft - *pRight
*/
#ifdef SQLITE_DEBUG
static int fts5BufferCompareBlob(
  Fts5Buffer *pLeft,              /* Left hand side of comparison */
  const u8 *pRight, int nRight    /* Right hand side of comparison */
){
  int nCmp = MIN(pLeft->n, nRight);
  int res = memcmp(pLeft->p, pRight, nCmp);
  return (res==0 ? (pLeft->n - nRight) : res);
}
#endif

/*
** Compare the contents of the two buffers using memcmp(). If one buffer
** is a prefix of the other, it is considered the lesser.
**
** Return -ve if pLeft is smaller than pRight, 0 if they are equal or
** +ve if pRight is smaller than pLeft. In other words:
**
**     res = *pLeft - *pRight
*/
static int fts5BufferCompare(Fts5Buffer *pLeft, Fts5Buffer *pRight){
  int nCmp, res;
  nCmp = MIN(pLeft->n, pRight->n);
  assert( nCmp<=0 || pLeft->p!=0 );
  assert( nCmp<=0 || pRight->p!=0 );
  res = fts5Memcmp(pLeft->p, pRight->p, nCmp);
  return (res==0 ? (pLeft->n - pRight->n) : res);
}

static int fts5LeafFirstTermOff(Fts5Data *pLeaf){
  int ret;
  fts5GetVarint32(&pLeaf->p[pLeaf->szLeaf], ret);
  return ret;
}

/*
** Close the read-only blob handle, if it is open.
*/
static void sqlite3Fts5IndexCloseReader(Fts5Index *p){
  if( p->pReader ){
    sqlite3_blob *pReader = p->pReader;
    p->pReader = 0;
    sqlite3_blob_close(pReader);
  }
}

/*
** Retrieve a record from the %_data table.
**
** If an error occurs, NULL is returned and an error left in the 
** Fts5Index object.
*/
static Fts5Data *fts5DataRead(Fts5Index *p, i64 iRowid){
  Fts5Data *pRet = 0;
  if( p->rc==SQLITE_OK ){
    int rc = SQLITE_OK;

    if( p->pReader ){
      /* This call may return SQLITE_ABORT if there has been a savepoint
      ** rollback since it was last used. In this case a new blob handle
      ** is required.  */
      sqlite3_blob *pBlob = p->pReader;
      p->pReader = 0;
      rc = sqlite3_blob_reopen(pBlob, iRowid);
      assert( p->pReader==0 );
      p->pReader = pBlob;
      if( rc!=SQLITE_OK ){
        sqlite3Fts5IndexCloseReader(p);
      }
      if( rc==SQLITE_ABORT ) rc = SQLITE_OK;
    }

    /* If the blob handle is not open at this point, open it and seek 
    ** to the requested entry.  */
    if( p->pReader==0 && rc==SQLITE_OK ){
      Fts5Config *pConfig = p->pConfig;
      rc = sqlite3_blob_open(pConfig->db, 
          pConfig->zDb, p->zDataTbl, "block", iRowid, 0, &p->pReader
      );
    }

    /* If either of the sqlite3_blob_open() or sqlite3_blob_reopen() calls
    ** above returned SQLITE_ERROR, return SQLITE_CORRUPT_VTAB instead.
    ** All the reasons those functions might return SQLITE_ERROR - missing
    ** table, missing row, non-blob/text in block column - indicate 
    ** backing store corruption.  */
    if( rc==SQLITE_ERROR ) rc = FTS5_CORRUPT;

    if( rc==SQLITE_OK ){
      u8 *aOut = 0;               /* Read blob data into this buffer */
      int nByte = sqlite3_blob_bytes(p->pReader);
      sqlite3_int64 nAlloc = sizeof(Fts5Data) + nByte + FTS5_DATA_PADDING;
      pRet = (Fts5Data*)sqlite3_malloc64(nAlloc);
      if( pRet ){
        pRet->nn = nByte;
        aOut = pRet->p = (u8*)&pRet[1];
      }else{
        rc = SQLITE_NOMEM;
      }

      if( rc==SQLITE_OK ){
        rc = sqlite3_blob_read(p->pReader, aOut, nByte, 0);
      }
      if( rc!=SQLITE_OK ){
        sqlite3_free(pRet);
        pRet = 0;
      }else{
        /* TODO1: Fix this */
        pRet->p[nByte] = 0x00;
        pRet->p[nByte+1] = 0x00;
        pRet->szLeaf = fts5GetU16(&pRet->p[2]);
      }
    }
    p->rc = rc;
    p->nRead++;
  }

  assert( (pRet==0)==(p->rc!=SQLITE_OK) );
  return pRet;
}


/*
** Release a reference to data record returned by an earlier call to
** fts5DataRead().
*/
static void fts5DataRelease(Fts5Data *pData){
  sqlite3_free(pData);
}

static Fts5Data *fts5LeafRead(Fts5Index *p, i64 iRowid){
  Fts5Data *pRet = fts5DataRead(p, iRowid);
  if( pRet ){
    if( pRet->nn<4 || pRet->szLeaf>pRet->nn ){
      p->rc = FTS5_CORRUPT;
      fts5DataRelease(pRet);
      pRet = 0;
    }
  }
  return pRet;
}

static int fts5IndexPrepareStmt(
  Fts5Index *p,
  sqlite3_stmt **ppStmt,
  char *zSql
){
  if( p->rc==SQLITE_OK ){
    if( zSql ){
      p->rc = sqlite3_prepare_v3(p->pConfig->db, zSql, -1,
          SQLITE_PREPARE_PERSISTENT|SQLITE_PREPARE_NO_VTAB,
          ppStmt, 0);
    }else{
      p->rc = SQLITE_NOMEM;
    }
  }
  sqlite3_free(zSql);
  return p->rc;
}


/*
** INSERT OR REPLACE a record into the %_data table.
*/
static void fts5DataWrite(Fts5Index *p, i64 iRowid, const u8 *pData, int nData){
  if( p->rc!=SQLITE_OK ) return;

  if( p->pWriter==0 ){
    Fts5Config *pConfig = p->pConfig;
    fts5IndexPrepareStmt(p, &p->pWriter, sqlite3_mprintf(
          "REPLACE INTO '%q'.'%q_data'(id, block) VALUES(?,?)", 
          pConfig->zDb, pConfig->zName
    ));
    if( p->rc ) return;
  }

  sqlite3_bind_int64(p->pWriter, 1, iRowid);
  sqlite3_bind_blob(p->pWriter, 2, pData, nData, SQLITE_STATIC);
  sqlite3_step(p->pWriter);
  p->rc = sqlite3_reset(p->pWriter);
  sqlite3_bind_null(p->pWriter, 2);
}

/*
** Execute the following SQL:
**
**     DELETE FROM %_data WHERE id BETWEEN $iFirst AND $iLast
*/
static void fts5DataDelete(Fts5Index *p, i64 iFirst, i64 iLast){
  if( p->rc!=SQLITE_OK ) return;

  if( p->pDeleter==0 ){
    Fts5Config *pConfig = p->pConfig;
    char *zSql = sqlite3_mprintf(
        "DELETE FROM '%q'.'%q_data' WHERE id>=? AND id<=?", 
          pConfig->zDb, pConfig->zName
    );
    if( fts5IndexPrepareStmt(p, &p->pDeleter, zSql) ) return;
  }

  sqlite3_bind_int64(p->pDeleter, 1, iFirst);
  sqlite3_bind_int64(p->pDeleter, 2, iLast);
  sqlite3_step(p->pDeleter);
  p->rc = sqlite3_reset(p->pDeleter);
}

/*
** Remove all records associated with segment iSegid.
*/
static void fts5DataRemoveSegment(Fts5Index *p, int iSegid){
  i64 iFirst = FTS5_SEGMENT_ROWID(iSegid, 0);
  i64 iLast = FTS5_SEGMENT_ROWID(iSegid+1, 0)-1;
  fts5DataDelete(p, iFirst, iLast);
  if( p->pIdxDeleter==0 ){
    Fts5Config *pConfig = p->pConfig;
    fts5IndexPrepareStmt(p, &p->pIdxDeleter, sqlite3_mprintf(
          "DELETE FROM '%q'.'%q_idx' WHERE segid=?",
          pConfig->zDb, pConfig->zName
    ));
  }
  if( p->rc==SQLITE_OK ){
    sqlite3_bind_int(p->pIdxDeleter, 1, iSegid);
    sqlite3_step(p->pIdxDeleter);
    p->rc = sqlite3_reset(p->pIdxDeleter);
  }
}

/*
** Release a reference to an Fts5Structure object returned by an earlier 
** call to fts5StructureRead() or fts5StructureDecode().
*/
static void fts5StructureRelease(Fts5Structure *pStruct){
  if( pStruct && 0>=(--pStruct->nRef) ){
    int i;
    assert( pStruct->nRef==0 );
    for(i=0; i<pStruct->nLevel; i++){
      sqlite3_free(pStruct->aLevel[i].aSeg);
    }
    sqlite3_free(pStruct);
  }
}

static void fts5StructureRef(Fts5Structure *pStruct){
  pStruct->nRef++;
}

static void *sqlite3Fts5StructureRef(Fts5Index *p){
  fts5StructureRef(p->pStruct);
  return (void*)p->pStruct;
}
static void sqlite3Fts5StructureRelease(void *p){
  if( p ){
    fts5StructureRelease((Fts5Structure*)p);
  }
}
static int sqlite3Fts5StructureTest(Fts5Index *p, void *pStruct){
  if( p->pStruct!=(Fts5Structure*)pStruct ){
    return SQLITE_ABORT;
  }
  return SQLITE_OK;
}

/*
** Ensure that structure object (*pp) is writable.
**
** This function is a no-op if (*pRc) is not SQLITE_OK when it is called. If
** an error occurs, (*pRc) is set to an SQLite error code before returning.
*/
static void fts5StructureMakeWritable(int *pRc, Fts5Structure **pp){
  Fts5Structure *p = *pp;
  if( *pRc==SQLITE_OK && p->nRef>1 ){
    i64 nByte = sizeof(Fts5Structure)+(p->nLevel-1)*sizeof(Fts5StructureLevel);
    Fts5Structure *pNew;
    pNew = (Fts5Structure*)sqlite3Fts5MallocZero(pRc, nByte);
    if( pNew ){
      int i;
      memcpy(pNew, p, nByte);
      for(i=0; i<p->nLevel; i++) pNew->aLevel[i].aSeg = 0;
      for(i=0; i<p->nLevel; i++){
        Fts5StructureLevel *pLvl = &pNew->aLevel[i];
        nByte = sizeof(Fts5StructureSegment) * pNew->aLevel[i].nSeg;
        pLvl->aSeg = (Fts5StructureSegment*)sqlite3Fts5MallocZero(pRc, nByte);
        if( pLvl->aSeg==0 ){
          for(i=0; i<p->nLevel; i++){
            sqlite3_free(pNew->aLevel[i].aSeg);
          }
          sqlite3_free(pNew);
          return;
        }
        memcpy(pLvl->aSeg, p->aLevel[i].aSeg, nByte);
      }
      p->nRef--;
      pNew->nRef = 1;
    }
    *pp = pNew;
  }
}

/*
** Deserialize and return the structure record currently stored in serialized
** form within buffer pData/nData.
**
** The Fts5Structure.aLevel[] and each Fts5StructureLevel.aSeg[] array
** are over-allocated by one slot. This allows the structure contents
** to be more easily edited.
**
** If an error occurs, *ppOut is set to NULL and an SQLite error code
** returned. Otherwise, *ppOut is set to point to the new object and
** SQLITE_OK returned.
*/
static int fts5StructureDecode(
  const u8 *pData,                /* Buffer containing serialized structure */
  int nData,                      /* Size of buffer pData in bytes */
  int *piCookie,                  /* Configuration cookie value */
  Fts5Structure **ppOut           /* OUT: Deserialized object */
){
  int rc = SQLITE_OK;
  int i = 0;
  int iLvl;
  int nLevel = 0;
  int nSegment = 0;
  sqlite3_int64 nByte;            /* Bytes of space to allocate at pRet */
  Fts5Structure *pRet = 0;        /* Structure object to return */

  /* Grab the cookie value */
  if( piCookie ) *piCookie = sqlite3Fts5Get32(pData);
  i = 4;

  /* Read the total number of levels and segments from the start of the
  ** structure record.  */
  i += fts5GetVarint32(&pData[i], nLevel);
  i += fts5GetVarint32(&pData[i], nSegment);
  if( nLevel>FTS5_MAX_SEGMENT   || nLevel<0
   || nSegment>FTS5_MAX_SEGMENT || nSegment<0
  ){
    return FTS5_CORRUPT;
  }
  nByte = (
      sizeof(Fts5Structure) +                    /* Main structure */
      sizeof(Fts5StructureLevel) * (nLevel-1)    /* aLevel[] array */
  );
  pRet = (Fts5Structure*)sqlite3Fts5MallocZero(&rc, nByte);

  if( pRet ){
    pRet->nRef = 1;
    pRet->nLevel = nLevel;
    pRet->nSegment = nSegment;
    i += sqlite3Fts5GetVarint(&pData[i], &pRet->nWriteCounter);

    for(iLvl=0; rc==SQLITE_OK && iLvl<nLevel; iLvl++){
      Fts5StructureLevel *pLvl = &pRet->aLevel[iLvl];
      int nTotal = 0;
      int iSeg;

      if( i>=nData ){
        rc = FTS5_CORRUPT;
      }else{
        i += fts5GetVarint32(&pData[i], pLvl->nMerge);
        i += fts5GetVarint32(&pData[i], nTotal);
        if( nTotal<pLvl->nMerge ) rc = FTS5_CORRUPT;
        pLvl->aSeg = (Fts5StructureSegment*)sqlite3Fts5MallocZero(&rc, 
            nTotal * sizeof(Fts5StructureSegment)
        );
        nSegment -= nTotal;
      }

      if( rc==SQLITE_OK ){
        pLvl->nSeg = nTotal;
        for(iSeg=0; iSeg<nTotal; iSeg++){
          Fts5StructureSegment *pSeg = &pLvl->aSeg[iSeg];
          if( i>=nData ){
            rc = FTS5_CORRUPT;
            break;
          }
          i += fts5GetVarint32(&pData[i], pSeg->iSegid);
          i += fts5GetVarint32(&pData[i], pSeg->pgnoFirst);
          i += fts5GetVarint32(&pData[i], pSeg->pgnoLast);
          if( pSeg->pgnoLast<pSeg->pgnoFirst ){
            rc = FTS5_CORRUPT;
            break;
          }
        }
        if( iLvl>0 && pLvl[-1].nMerge && nTotal==0 ) rc = FTS5_CORRUPT;
        if( iLvl==nLevel-1 && pLvl->nMerge ) rc = FTS5_CORRUPT;
      }
    }
    if( nSegment!=0 && rc==SQLITE_OK ) rc = FTS5_CORRUPT;

    if( rc!=SQLITE_OK ){
      fts5StructureRelease(pRet);
      pRet = 0;
    }
  }

  *ppOut = pRet;
  return rc;
}

/*
** Add a level to the Fts5Structure.aLevel[] array of structure object
** (*ppStruct).
*/
static void fts5StructureAddLevel(int *pRc, Fts5Structure **ppStruct){
  fts5StructureMakeWritable(pRc, ppStruct);
  if( *pRc==SQLITE_OK ){
    Fts5Structure *pStruct = *ppStruct;
    int nLevel = pStruct->nLevel;
    sqlite3_int64 nByte = (
        sizeof(Fts5Structure) +                  /* Main structure */
        sizeof(Fts5StructureLevel) * (nLevel+1)  /* aLevel[] array */
    );

    pStruct = sqlite3_realloc64(pStruct, nByte);
    if( pStruct ){
      memset(&pStruct->aLevel[nLevel], 0, sizeof(Fts5StructureLevel));
      pStruct->nLevel++;
      *ppStruct = pStruct;
    }else{
      *pRc = SQLITE_NOMEM;
    }
  }
}

/*
** Extend level iLvl so that there is room for at least nExtra more
** segments.
*/
static void fts5StructureExtendLevel(
  int *pRc, 
  Fts5Structure *pStruct, 
  int iLvl, 
  int nExtra, 
  int bInsert
){
  if( *pRc==SQLITE_OK ){
    Fts5StructureLevel *pLvl = &pStruct->aLevel[iLvl];
    Fts5StructureSegment *aNew;
    sqlite3_int64 nByte;

    nByte = (pLvl->nSeg + nExtra) * sizeof(Fts5StructureSegment);
    aNew = sqlite3_realloc64(pLvl->aSeg, nByte);
    if( aNew ){
      if( bInsert==0 ){
        memset(&aNew[pLvl->nSeg], 0, sizeof(Fts5StructureSegment) * nExtra);
      }else{
        int nMove = pLvl->nSeg * sizeof(Fts5StructureSegment);
        memmove(&aNew[nExtra], aNew, nMove);
        memset(aNew, 0, sizeof(Fts5StructureSegment) * nExtra);
      }
      pLvl->aSeg = aNew;
    }else{
      *pRc = SQLITE_NOMEM;
    }
  }
}

static Fts5Structure *fts5StructureReadUncached(Fts5Index *p){
  Fts5Structure *pRet = 0;
  Fts5Config *pConfig = p->pConfig;
  int iCookie;                    /* Configuration cookie */
  Fts5Data *pData;

  pData = fts5DataRead(p, FTS5_STRUCTURE_ROWID);
  if( p->rc==SQLITE_OK ){
    /* TODO: Do we need this if the leaf-index is appended? Probably... */
    memset(&pData->p[pData->nn], 0, FTS5_DATA_PADDING);
    p->rc = fts5StructureDecode(pData->p, pData->nn, &iCookie, &pRet);
    if( p->rc==SQLITE_OK && (pConfig->pgsz==0 || pConfig->iCookie!=iCookie) ){
      p->rc = sqlite3Fts5ConfigLoad(pConfig, iCookie);
    }
    fts5DataRelease(pData);
    if( p->rc!=SQLITE_OK ){
      fts5StructureRelease(pRet);
      pRet = 0;
    }
  }

  return pRet;
}

static i64 fts5IndexDataVersion(Fts5Index *p){
  i64 iVersion = 0;

  if( p->rc==SQLITE_OK ){
    if( p->pDataVersion==0 ){
      p->rc = fts5IndexPrepareStmt(p, &p->pDataVersion, 
          sqlite3_mprintf("PRAGMA %Q.data_version", p->pConfig->zDb)
          );
      if( p->rc ) return 0;
    }

    if( SQLITE_ROW==sqlite3_step(p->pDataVersion) ){
      iVersion = sqlite3_column_int64(p->pDataVersion, 0);
    }
    p->rc = sqlite3_reset(p->pDataVersion);
  }

  return iVersion;
}

/*
** Read, deserialize and return the structure record.
**
** The Fts5Structure.aLevel[] and each Fts5StructureLevel.aSeg[] array
** are over-allocated as described for function fts5StructureDecode() 
** above.
**
** If an error occurs, NULL is returned and an error code left in the
** Fts5Index handle. If an error has already occurred when this function
** is called, it is a no-op.
*/
static Fts5Structure *fts5StructureRead(Fts5Index *p){

  if( p->pStruct==0 ){
    p->iStructVersion = fts5IndexDataVersion(p);
    if( p->rc==SQLITE_OK ){
      p->pStruct = fts5StructureReadUncached(p);
    }
  }

#if 0
  else{
    Fts5Structure *pTest = fts5StructureReadUncached(p);
    if( pTest ){
      int i, j;
      assert_nc( p->pStruct->nSegment==pTest->nSegment );
      assert_nc( p->pStruct->nLevel==pTest->nLevel );
      for(i=0; i<pTest->nLevel; i++){
        assert_nc( p->pStruct->aLevel[i].nMerge==pTest->aLevel[i].nMerge );
        assert_nc( p->pStruct->aLevel[i].nSeg==pTest->aLevel[i].nSeg );
        for(j=0; j<pTest->aLevel[i].nSeg; j++){
          Fts5StructureSegment *p1 = &pTest->aLevel[i].aSeg[j];
          Fts5StructureSegment *p2 = &p->pStruct->aLevel[i].aSeg[j];
          assert_nc( p1->iSegid==p2->iSegid );
          assert_nc( p1->pgnoFirst==p2->pgnoFirst );
          assert_nc( p1->pgnoLast==p2->pgnoLast );
        }
      }
      fts5StructureRelease(pTest);
    }
  }
#endif

  if( p->rc!=SQLITE_OK ) return 0;
  assert( p->iStructVersion!=0 );
  assert( p->pStruct!=0 );
  fts5StructureRef(p->pStruct);
  return p->pStruct;
}

static void fts5StructureInvalidate(Fts5Index *p){
  if( p->pStruct ){
    fts5StructureRelease(p->pStruct);
    p->pStruct = 0;
  }
}

/*
** Return the total number of segments in index structure pStruct. This
** function is only ever used as part of assert() conditions.
*/
#ifdef SQLITE_DEBUG
static int fts5StructureCountSegments(Fts5Structure *pStruct){
  int nSegment = 0;               /* Total number of segments */
  if( pStruct ){
    int iLvl;                     /* Used to iterate through levels */
    for(iLvl=0; iLvl<pStruct->nLevel; iLvl++){
      nSegment += pStruct->aLevel[iLvl].nSeg;
    }
  }

  return nSegment;
}
#endif

#define fts5BufferSafeAppendBlob(pBuf, pBlob, nBlob) {     \
  assert( (pBuf)->nSpace>=((pBuf)->n+nBlob) );             \
  memcpy(&(pBuf)->p[(pBuf)->n], pBlob, nBlob);             \
  (pBuf)->n += nBlob;                                      \
}

#define fts5BufferSafeAppendVarint(pBuf, iVal) {                \
  (pBuf)->n += sqlite3Fts5PutVarint(&(pBuf)->p[(pBuf)->n], (iVal));  \
  assert( (pBuf)->nSpace>=(pBuf)->n );                          \
}


/*
** Serialize and store the "structure" record.
**
** If an error occurs, leave an error code in the Fts5Index object. If an
** error has already occurred, this function is a no-op.
*/
static void fts5StructureWrite(Fts5Index *p, Fts5Structure *pStruct){
  if( p->rc==SQLITE_OK ){
    Fts5Buffer buf;               /* Buffer to serialize record into */
    int iLvl;                     /* Used to iterate through levels */
    int iCookie;                  /* Cookie value to store */

    assert( pStruct->nSegment==fts5StructureCountSegments(pStruct) );
    memset(&buf, 0, sizeof(Fts5Buffer));

    /* Append the current configuration cookie */
    iCookie = p->pConfig->iCookie;
    if( iCookie<0 ) iCookie = 0;

    if( 0==sqlite3Fts5BufferSize(&p->rc, &buf, 4+9+9+9) ){
      sqlite3Fts5Put32(buf.p, iCookie);
      buf.n = 4;
      fts5BufferSafeAppendVarint(&buf, pStruct->nLevel);
      fts5BufferSafeAppendVarint(&buf, pStruct->nSegment);
      fts5BufferSafeAppendVarint(&buf, (i64)pStruct->nWriteCounter);
    }

    for(iLvl=0; iLvl<pStruct->nLevel; iLvl++){
      int iSeg;                     /* Used to iterate through segments */
      Fts5StructureLevel *pLvl = &pStruct->aLevel[iLvl];
      fts5BufferAppendVarint(&p->rc, &buf, pLvl->nMerge);
      fts5BufferAppendVarint(&p->rc, &buf, pLvl->nSeg);
      assert( pLvl->nMerge<=pLvl->nSeg );

      for(iSeg=0; iSeg<pLvl->nSeg; iSeg++){
        fts5BufferAppendVarint(&p->rc, &buf, pLvl->aSeg[iSeg].iSegid);
        fts5BufferAppendVarint(&p->rc, &buf, pLvl->aSeg[iSeg].pgnoFirst);
        fts5BufferAppendVarint(&p->rc, &buf, pLvl->aSeg[iSeg].pgnoLast);
      }
    }

    fts5DataWrite(p, FTS5_STRUCTURE_ROWID, buf.p, buf.n);
    fts5BufferFree(&buf);
  }
}

#if 0
static void fts5DebugStructure(int*,Fts5Buffer*,Fts5Structure*);
static void fts5PrintStructure(const char *zCaption, Fts5Structure *pStruct){
  int rc = SQLITE_OK;
  Fts5Buffer buf;
  memset(&buf, 0, sizeof(buf));
  fts5DebugStructure(&rc, &buf, pStruct);
  fprintf(stdout, "%s: %s\n", zCaption, buf.p);
  fflush(stdout);
  fts5BufferFree(&buf);
}
#else
# define fts5PrintStructure(x,y)
#endif

static int fts5SegmentSize(Fts5StructureSegment *pSeg){
  return 1 + pSeg->pgnoLast - pSeg->pgnoFirst;
}

/*
** Return a copy of index structure pStruct. Except, promote as many 
** segments as possible to level iPromote. If an OOM occurs, NULL is 
** returned.
*/
static void fts5StructurePromoteTo(
  Fts5Index *p,
  int iPromote,
  int szPromote,
  Fts5Structure *pStruct
){
  int il, is;
  Fts5StructureLevel *pOut = &pStruct->aLevel[iPromote];

  if( pOut->nMerge==0 ){
    for(il=iPromote+1; il<pStruct->nLevel; il++){
      Fts5StructureLevel *pLvl = &pStruct->aLevel[il];
      if( pLvl->nMerge ) return;
      for(is=pLvl->nSeg-1; is>=0; is--){
        int sz = fts5SegmentSize(&pLvl->aSeg[is]);
        if( sz>szPromote ) return;
        fts5StructureExtendLevel(&p->rc, pStruct, iPromote, 1, 1);
        if( p->rc ) return;
        memcpy(pOut->aSeg, &pLvl->aSeg[is], sizeof(Fts5StructureSegment));
        pOut->nSeg++;
        pLvl->nSeg--;
      }
    }
  }
}

/*
** A new segment has just been written to level iLvl of index structure
** pStruct. This function determines if any segments should be promoted
** as a result. Segments are promoted in two scenarios:
**
**   a) If the segment just written is smaller than one or more segments
**      within the previous populated level, it is promoted to the previous
**      populated level.
**
**   b) If the segment just written is larger than the newest segment on
**      the next populated level, then that segment, and any other adjacent
**      segments that are also smaller than the one just written, are 
**      promoted. 
**
** If one or more segments are promoted, the structure object is updated
** to reflect this.
*/
static void fts5StructurePromote(
  Fts5Index *p,                   /* FTS5 backend object */
  int iLvl,                       /* Index level just updated */
  Fts5Structure *pStruct          /* Index structure */
){
  if( p->rc==SQLITE_OK ){
    int iTst;
    int iPromote = -1;
    int szPromote = 0;            /* Promote anything this size or smaller */
    Fts5StructureSegment *pSeg;   /* Segment just written */
    int szSeg;                    /* Size of segment just written */
    int nSeg = pStruct->aLevel[iLvl].nSeg;

    if( nSeg==0 ) return;
    pSeg = &pStruct->aLevel[iLvl].aSeg[pStruct->aLevel[iLvl].nSeg-1];
    szSeg = (1 + pSeg->pgnoLast - pSeg->pgnoFirst);

    /* Check for condition (a) */
    for(iTst=iLvl-1; iTst>=0 && pStruct->aLevel[iTst].nSeg==0; iTst--);
    if( iTst>=0 ){
      int i;
      int szMax = 0;
      Fts5StructureLevel *pTst = &pStruct->aLevel[iTst];
      assert( pTst->nMerge==0 );
      for(i=0; i<pTst->nSeg; i++){
        int sz = pTst->aSeg[i].pgnoLast - pTst->aSeg[i].pgnoFirst + 1;
        if( sz>szMax ) szMax = sz;
      }
      if( szMax>=szSeg ){
        /* Condition (a) is true. Promote the newest segment on level 
        ** iLvl to level iTst.  */
        iPromote = iTst;
        szPromote = szMax;
      }
    }

    /* If condition (a) is not met, assume (b) is true. StructurePromoteTo()
    ** is a no-op if it is not.  */
    if( iPromote<0 ){
      iPromote = iLvl;
      szPromote = szSeg;
    }
    fts5StructurePromoteTo(p, iPromote, szPromote, pStruct);
  }
}


/*
** Advance the iterator passed as the only argument. If the end of the 
** doclist-index page is reached, return non-zero.
*/
static int fts5DlidxLvlNext(Fts5DlidxLvl *pLvl){
  Fts5Data *pData = pLvl->pData;

  if( pLvl->iOff==0 ){
    assert( pLvl->bEof==0 );
    pLvl->iOff = 1;
    pLvl->iOff += fts5GetVarint32(&pData->p[1], pLvl->iLeafPgno);
    pLvl->iOff += fts5GetVarint(&pData->p[pLvl->iOff], (u64*)&pLvl->iRowid);
    pLvl->iFirstOff = pLvl->iOff;
  }else{
    int iOff;
    for(iOff=pLvl->iOff; iOff<pData->nn; iOff++){
      if( pData->p[iOff] ) break; 
    }

    if( iOff<pData->nn ){
      i64 iVal;
      pLvl->iLeafPgno += (iOff - pLvl->iOff) + 1;
      iOff += fts5GetVarint(&pData->p[iOff], (u64*)&iVal);
      pLvl->iRowid += iVal;
      pLvl->iOff = iOff;
    }else{
      pLvl->bEof = 1;
    }
  }

  return pLvl->bEof;
}

/*
** Advance the iterator passed as the only argument.
*/
static int fts5DlidxIterNextR(Fts5Index *p, Fts5DlidxIter *pIter, int iLvl){
  Fts5DlidxLvl *pLvl = &pIter->aLvl[iLvl];

  assert( iLvl<pIter->nLvl );
  if( fts5DlidxLvlNext(pLvl) ){
    if( (iLvl+1) < pIter->nLvl ){
      fts5DlidxIterNextR(p, pIter, iLvl+1);
      if( pLvl[1].bEof==0 ){
        fts5DataRelease(pLvl->pData);
        memset(pLvl, 0, sizeof(Fts5DlidxLvl));
        pLvl->pData = fts5DataRead(p, 
            FTS5_DLIDX_ROWID(pIter->iSegid, iLvl, pLvl[1].iLeafPgno)
        );
        if( pLvl->pData ) fts5DlidxLvlNext(pLvl);
      }
    }
  }

  return pIter->aLvl[0].bEof;
}
static int fts5DlidxIterNext(Fts5Index *p, Fts5DlidxIter *pIter){
  return fts5DlidxIterNextR(p, pIter, 0);
}

/*
** The iterator passed as the first argument has the following fields set
** as follows. This function sets up the rest of the iterator so that it
** points to the first rowid in the doclist-index.
**
**   pData:
**     pointer to doclist-index record, 
**
** When this function is called pIter->iLeafPgno is the page number the
** doclist is associated with (the one featuring the term).
*/
static int fts5DlidxIterFirst(Fts5DlidxIter *pIter){
  int i;
  for(i=0; i<pIter->nLvl; i++){
    fts5DlidxLvlNext(&pIter->aLvl[i]);
  }
  return pIter->aLvl[0].bEof;
}


static int fts5DlidxIterEof(Fts5Index *p, Fts5DlidxIter *pIter){
  return p->rc!=SQLITE_OK || pIter->aLvl[0].bEof;
}

static void fts5DlidxIterLast(Fts5Index *p, Fts5DlidxIter *pIter){
  int i;

  /* Advance each level to the last entry on the last page */
  for(i=pIter->nLvl-1; p->rc==SQLITE_OK && i>=0; i--){
    Fts5DlidxLvl *pLvl = &pIter->aLvl[i];
    while( fts5DlidxLvlNext(pLvl)==0 );
    pLvl->bEof = 0;

    if( i>0 ){
      Fts5DlidxLvl *pChild = &pLvl[-1];
      fts5DataRelease(pChild->pData);
      memset(pChild, 0, sizeof(Fts5DlidxLvl));
      pChild->pData = fts5DataRead(p, 
          FTS5_DLIDX_ROWID(pIter->iSegid, i-1, pLvl->iLeafPgno)
      );
    }
  }
}

/*
** Move the iterator passed as the only argument to the previous entry.
*/
static int fts5DlidxLvlPrev(Fts5DlidxLvl *pLvl){
  int iOff = pLvl->iOff;

  assert( pLvl->bEof==0 );
  if( iOff<=pLvl->iFirstOff ){
    pLvl->bEof = 1;
  }else{
    u8 *a = pLvl->pData->p;
    i64 iVal;
    int iLimit;
    int ii;
    int nZero = 0;

    /* Currently iOff points to the first byte of a varint. This block 
    ** decrements iOff until it points to the first byte of the previous 
    ** varint. Taking care not to read any memory locations that occur
    ** before the buffer in memory.  */
    iLimit = (iOff>9 ? iOff-9 : 0);
    for(iOff--; iOff>iLimit; iOff--){
      if( (a[iOff-1] & 0x80)==0 ) break;
    }

    fts5GetVarint(&a[iOff], (u64*)&iVal);
    pLvl->iRowid -= iVal;
    pLvl->iLeafPgno--;

    /* Skip backwards past any 0x00 varints. */
    for(ii=iOff-1; ii>=pLvl->iFirstOff && a[ii]==0x00; ii--){
      nZero++;
    }
    if( ii>=pLvl->iFirstOff && (a[ii] & 0x80) ){
      /* The byte immediately before the last 0x00 byte has the 0x80 bit
      ** set. So the last 0x00 is only a varint 0 if there are 8 more 0x80
      ** bytes before a[ii]. */
      int bZero = 0;              /* True if last 0x00 counts */
      if( (ii-8)>=pLvl->iFirstOff ){
        int j;
        for(j=1; j<=8 && (a[ii-j] & 0x80); j++);
        bZero = (j>8);
      }
      if( bZero==0 ) nZero--;
    }
    pLvl->iLeafPgno -= nZero;
    pLvl->iOff = iOff - nZero;
  }

  return pLvl->bEof;
}

static int fts5DlidxIterPrevR(Fts5Index *p, Fts5DlidxIter *pIter, int iLvl){
  Fts5DlidxLvl *pLvl = &pIter->aLvl[iLvl];

  assert( iLvl<pIter->nLvl );
  if( fts5DlidxLvlPrev(pLvl) ){
    if( (iLvl+1) < pIter->nLvl ){
      fts5DlidxIterPrevR(p, pIter, iLvl+1);
      if( pLvl[1].bEof==0 ){
        fts5DataRelease(pLvl->pData);
        memset(pLvl, 0, sizeof(Fts5DlidxLvl));
        pLvl->pData = fts5DataRead(p, 
            FTS5_DLIDX_ROWID(pIter->iSegid, iLvl, pLvl[1].iLeafPgno)
        );
        if( pLvl->pData ){
          while( fts5DlidxLvlNext(pLvl)==0 );
          pLvl->bEof = 0;
        }
      }
    }
  }

  return pIter->aLvl[0].bEof;
}
static int fts5DlidxIterPrev(Fts5Index *p, Fts5DlidxIter *pIter){
  return fts5DlidxIterPrevR(p, pIter, 0);
}

/*
** Free a doclist-index iterator object allocated by fts5DlidxIterInit().
*/
static void fts5DlidxIterFree(Fts5DlidxIter *pIter){
  if( pIter ){
    int i;
    for(i=0; i<pIter->nLvl; i++){
      fts5DataRelease(pIter->aLvl[i].pData);
    }
    sqlite3_free(pIter);
  }
}

static Fts5DlidxIter *fts5DlidxIterInit(
  Fts5Index *p,                   /* Fts5 Backend to iterate within */
  int bRev,                       /* True for ORDER BY ASC */
  int iSegid,                     /* Segment id */
  int iLeafPg                     /* Leaf page number to load dlidx for */
){
  Fts5DlidxIter *pIter = 0;
  int i;
  int bDone = 0;

  for(i=0; p->rc==SQLITE_OK && bDone==0; i++){
    sqlite3_int64 nByte = sizeof(Fts5DlidxIter) + i * sizeof(Fts5DlidxLvl);
    Fts5DlidxIter *pNew;

    pNew = (Fts5DlidxIter*)sqlite3_realloc64(pIter, nByte);
    if( pNew==0 ){
      p->rc = SQLITE_NOMEM;
    }else{
      i64 iRowid = FTS5_DLIDX_ROWID(iSegid, i, iLeafPg);
      Fts5DlidxLvl *pLvl = &pNew->aLvl[i];
      pIter = pNew;
      memset(pLvl, 0, sizeof(Fts5DlidxLvl));
      pLvl->pData = fts5DataRead(p, iRowid);
      if( pLvl->pData && (pLvl->pData->p[0] & 0x0001)==0 ){
        bDone = 1;
      }
      pIter->nLvl = i+1;
    }
  }

  if( p->rc==SQLITE_OK ){
    pIter->iSegid = iSegid;
    if( bRev==0 ){
      fts5DlidxIterFirst(pIter);
    }else{
      fts5DlidxIterLast(p, pIter);
    }
  }

  if( p->rc!=SQLITE_OK ){
    fts5DlidxIterFree(pIter);
    pIter = 0;
  }

  return pIter;
}

static i64 fts5DlidxIterRowid(Fts5DlidxIter *pIter){
  return pIter->aLvl[0].iRowid;
}
static int fts5DlidxIterPgno(Fts5DlidxIter *pIter){
  return pIter->aLvl[0].iLeafPgno;
}

/*
** Load the next leaf page into the segment iterator.
*/
static void fts5SegIterNextPage(
  Fts5Index *p,                   /* FTS5 backend object */
  Fts5SegIter *pIter              /* Iterator to advance to next page */
){
  Fts5Data *pLeaf;
  Fts5StructureSegment *pSeg = pIter->pSeg;
  fts5DataRelease(pIter->pLeaf);
  pIter->iLeafPgno++;
  if( pIter->pNextLeaf ){
    pIter->pLeaf = pIter->pNextLeaf;
    pIter->pNextLeaf = 0;
  }else if( pIter->iLeafPgno<=pSeg->pgnoLast ){
    pIter->pLeaf = fts5LeafRead(p, 
        FTS5_SEGMENT_ROWID(pSeg->iSegid, pIter->iLeafPgno)
    );
  }else{
    pIter->pLeaf = 0;
  }
  pLeaf = pIter->pLeaf;

  if( pLeaf ){
    pIter->iPgidxOff = pLeaf->szLeaf;
    if( fts5LeafIsTermless(pLeaf) ){
      pIter->iEndofDoclist = pLeaf->nn+1;
    }else{
      pIter->iPgidxOff += fts5GetVarint32(&pLeaf->p[pIter->iPgidxOff],
          pIter->iEndofDoclist
      );
    }
  }
}

/*
** Argument p points to a buffer containing a varint to be interpreted as a
** position list size field. Read the varint and return the number of bytes
** read. Before returning, set *pnSz to the number of bytes in the position
** list, and *pbDel to true if the delete flag is set, or false otherwise.
*/
static int fts5GetPoslistSize(const u8 *p, int *pnSz, int *pbDel){
  int nSz;
  int n = 0;
  fts5FastGetVarint32(p, n, nSz);
  assert_nc( nSz>=0 );
  *pnSz = nSz/2;
  *pbDel = nSz & 0x0001;
  return n;
}

/*
** Fts5SegIter.iLeafOffset currently points to the first byte of a
** position-list size field. Read the value of the field and store it
** in the following variables:
**
**   Fts5SegIter.nPos
**   Fts5SegIter.bDel
**
** Leave Fts5SegIter.iLeafOffset pointing to the first byte of the 
** position list content (if any).
*/
static void fts5SegIterLoadNPos(Fts5Index *p, Fts5SegIter *pIter){
  if( p->rc==SQLITE_OK ){
    int iOff = pIter->iLeafOffset;  /* Offset to read at */
    ASSERT_SZLEAF_OK(pIter->pLeaf);
    if( p->pConfig->eDetail==FTS5_DETAIL_NONE ){
      int iEod = MIN(pIter->iEndofDoclist, pIter->pLeaf->szLeaf);
      pIter->bDel = 0;
      pIter->nPos = 1;
      if( iOff<iEod && pIter->pLeaf->p[iOff]==0 ){
        pIter->bDel = 1;
        iOff++;
        if( iOff<iEod && pIter->pLeaf->p[iOff]==0 ){
          pIter->nPos = 1;
          iOff++;
        }else{
          pIter->nPos = 0;
        }
      }
    }else{
      int nSz;
      fts5FastGetVarint32(pIter->pLeaf->p, iOff, nSz);
      pIter->bDel = (nSz & 0x0001);
      pIter->nPos = nSz>>1;
      assert_nc( pIter->nPos>=0 );
    }
    pIter->iLeafOffset = iOff;
  }
}

static void fts5SegIterLoadRowid(Fts5Index *p, Fts5SegIter *pIter){
  u8 *a = pIter->pLeaf->p;        /* Buffer to read data from */
  i64 iOff = pIter->iLeafOffset;

  ASSERT_SZLEAF_OK(pIter->pLeaf);
  if( iOff>=pIter->pLeaf->szLeaf ){
    fts5SegIterNextPage(p, pIter);
    if( pIter->pLeaf==0 ){
      if( p->rc==SQLITE_OK ) p->rc = FTS5_CORRUPT;
      return;
    }
    iOff = 4;
    a = pIter->pLeaf->p;
  }
  iOff += sqlite3Fts5GetVarint(&a[iOff], (u64*)&pIter->iRowid);
  pIter->iLeafOffset = iOff;
}

/*
** Fts5SegIter.iLeafOffset currently points to the first byte of the 
** "nSuffix" field of a term. Function parameter nKeep contains the value
** of the "nPrefix" field (if there was one - it is passed 0 if this is
** the first term in the segment).
**
** This function populates:
**
**   Fts5SegIter.term
**   Fts5SegIter.rowid
**
** accordingly and leaves (Fts5SegIter.iLeafOffset) set to the content of
** the first position list. The position list belonging to document 
** (Fts5SegIter.iRowid).
*/
static void fts5SegIterLoadTerm(Fts5Index *p, Fts5SegIter *pIter, int nKeep){
  u8 *a = pIter->pLeaf->p;        /* Buffer to read data from */
  i64 iOff = pIter->iLeafOffset;  /* Offset to read at */
  int nNew;                       /* Bytes of new data */

  iOff += fts5GetVarint32(&a[iOff], nNew);
  if( iOff+nNew>pIter->pLeaf->szLeaf || nKeep>pIter->term.n || nNew==0 ){
    p->rc = FTS5_CORRUPT;
    return;
  }
  pIter->term.n = nKeep;
  fts5BufferAppendBlob(&p->rc, &pIter->term, nNew, &a[iOff]);
  assert( pIter->term.n<=pIter->term.nSpace );
  iOff += nNew;
  pIter->iTermLeafOffset = iOff;
  pIter->iTermLeafPgno = pIter->iLeafPgno;
  pIter->iLeafOffset = iOff;

  if( pIter->iPgidxOff>=pIter->pLeaf->nn ){
    pIter->iEndofDoclist = pIter->pLeaf->nn+1;
  }else{
    int nExtra;
    pIter->iPgidxOff += fts5GetVarint32(&a[pIter->iPgidxOff], nExtra);
    pIter->iEndofDoclist += nExtra;
  }

  fts5SegIterLoadRowid(p, pIter);
}

static void fts5SegIterNext(Fts5Index*, Fts5SegIter*, int*);
static void fts5SegIterNext_Reverse(Fts5Index*, Fts5SegIter*, int*);
static void fts5SegIterNext_None(Fts5Index*, Fts5SegIter*, int*);

static void fts5SegIterSetNext(Fts5Index *p, Fts5SegIter *pIter){
  if( pIter->flags & FTS5_SEGITER_REVERSE ){
    pIter->xNext = fts5SegIterNext_Reverse;
  }else if( p->pConfig->eDetail==FTS5_DETAIL_NONE ){
    pIter->xNext = fts5SegIterNext_None;
  }else{
    pIter->xNext = fts5SegIterNext;
  }
}

/*
** Initialize the iterator object pIter to iterate through the entries in
** segment pSeg. The iterator is left pointing to the first entry when 
** this function returns.
**
** If an error occurs, Fts5Index.rc is set to an appropriate error code. If 
** an error has already occurred when this function is called, it is a no-op.
*/
static void fts5SegIterInit(
  Fts5Index *p,                   /* FTS index object */
  Fts5StructureSegment *pSeg,     /* Description of segment */
  Fts5SegIter *pIter              /* Object to populate */
){
  if( pSeg->pgnoFirst==0 ){
    /* This happens if the segment is being used as an input to an incremental
    ** merge and all data has already been "trimmed". See function
    ** fts5TrimSegments() for details. In this case leave the iterator empty.
    ** The caller will see the (pIter->pLeaf==0) and assume the iterator is
    ** at EOF already. */
    assert( pIter->pLeaf==0 );
    return;
  }

  if( p->rc==SQLITE_OK ){
    memset(pIter, 0, sizeof(*pIter));
    fts5SegIterSetNext(p, pIter);
    pIter->pSeg = pSeg;
    pIter->iLeafPgno = pSeg->pgnoFirst-1;
    fts5SegIterNextPage(p, pIter);
  }

  if( p->rc==SQLITE_OK ){
    pIter->iLeafOffset = 4;
    assert( pIter->pLeaf!=0 );
    assert_nc( pIter->pLeaf->nn>4 );
    assert_nc( fts5LeafFirstTermOff(pIter->pLeaf)==4 );
    pIter->iPgidxOff = pIter->pLeaf->szLeaf+1;
    fts5SegIterLoadTerm(p, pIter, 0);
    fts5SegIterLoadNPos(p, pIter);
  }
}

/*
** This function is only ever called on iterators created by calls to
** Fts5IndexQuery() with the FTS5INDEX_QUERY_DESC flag set.
**
** The iterator is in an unusual state when this function is called: the
** Fts5SegIter.iLeafOffset variable is set to the offset of the start of
** the position-list size field for the first relevant rowid on the page.
** Fts5SegIter.rowid is set, but nPos and bDel are not.
**
** This function advances the iterator so that it points to the last 
** relevant rowid on the page and, if necessary, initializes the 
** aRowidOffset[] and iRowidOffset variables. At this point the iterator
** is in its regular state - Fts5SegIter.iLeafOffset points to the first
** byte of the position list content associated with said rowid.
*/
static void fts5SegIterReverseInitPage(Fts5Index *p, Fts5SegIter *pIter){
  int eDetail = p->pConfig->eDetail;
  int n = pIter->pLeaf->szLeaf;
  int i = pIter->iLeafOffset;
  u8 *a = pIter->pLeaf->p;
  int iRowidOffset = 0;

  if( n>pIter->iEndofDoclist ){
    n = pIter->iEndofDoclist;
  }

  ASSERT_SZLEAF_OK(pIter->pLeaf);
  while( 1 ){
    u64 iDelta = 0;

    if( eDetail==FTS5_DETAIL_NONE ){
      /* todo */
      if( i<n && a[i]==0 ){
        i++;
        if( i<n && a[i]==0 ) i++;
      }
    }else{
      int nPos;
      int bDummy;
      i += fts5GetPoslistSize(&a[i], &nPos, &bDummy);
      i += nPos;
    }
    if( i>=n ) break;
    i += fts5GetVarint(&a[i], &iDelta);
    pIter->iRowid += iDelta;

    /* If necessary, grow the pIter->aRowidOffset[] array. */
    if( iRowidOffset>=pIter->nRowidOffset ){
      int nNew = pIter->nRowidOffset + 8;
      int *aNew = (int*)sqlite3_realloc64(pIter->aRowidOffset,nNew*sizeof(int));
      if( aNew==0 ){
        p->rc = SQLITE_NOMEM;
        break;
      }
      pIter->aRowidOffset = aNew;
      pIter->nRowidOffset = nNew;
    }

    pIter->aRowidOffset[iRowidOffset++] = pIter->iLeafOffset;
    pIter->iLeafOffset = i;
  }
  pIter->iRowidOffset = iRowidOffset;
  fts5SegIterLoadNPos(p, pIter);
}

/*
**
*/
static void fts5SegIterReverseNewPage(Fts5Index *p, Fts5SegIter *pIter){
  assert( pIter->flags & FTS5_SEGITER_REVERSE );
  assert( pIter->flags & FTS5_SEGITER_ONETERM );

  fts5DataRelease(pIter->pLeaf);
  pIter->pLeaf = 0;
  while( p->rc==SQLITE_OK && pIter->iLeafPgno>pIter->iTermLeafPgno ){
    Fts5Data *pNew;
    pIter->iLeafPgno--;
    pNew = fts5DataRead(p, FTS5_SEGMENT_ROWID(
          pIter->pSeg->iSegid, pIter->iLeafPgno
    ));
    if( pNew ){
      /* iTermLeafOffset may be equal to szLeaf if the term is the last
      ** thing on the page - i.e. the first rowid is on the following page.
      ** In this case leave pIter->pLeaf==0, this iterator is at EOF. */
      if( pIter->iLeafPgno==pIter->iTermLeafPgno ){
        assert( pIter->pLeaf==0 );
        if( pIter->iTermLeafOffset<pNew->szLeaf ){
          pIter->pLeaf = pNew;
          pIter->iLeafOffset = pIter->iTermLeafOffset;
        }
      }else{
        int iRowidOff;
        iRowidOff = fts5LeafFirstRowidOff(pNew);
        if( iRowidOff ){
          if( iRowidOff>=pNew->szLeaf ){
            p->rc = FTS5_CORRUPT;
          }else{
            pIter->pLeaf = pNew;
            pIter->iLeafOffset = iRowidOff;
          }
        }
      }

      if( pIter->pLeaf ){
        u8 *a = &pIter->pLeaf->p[pIter->iLeafOffset];
        pIter->iLeafOffset += fts5GetVarint(a, (u64*)&pIter->iRowid);
        break;
      }else{
        fts5DataRelease(pNew);
      }
    }
  }

  if( pIter->pLeaf ){
    pIter->iEndofDoclist = pIter->pLeaf->nn+1;
    fts5SegIterReverseInitPage(p, pIter);
  }
}

/*
** Return true if the iterator passed as the second argument currently
** points to a delete marker. A delete marker is an entry with a 0 byte
** position-list.
*/
static int fts5MultiIterIsEmpty(Fts5Index *p, Fts5Iter *pIter){
  Fts5SegIter *pSeg = &pIter->aSeg[pIter->aFirst[1].iFirst];
  return (p->rc==SQLITE_OK && pSeg->pLeaf && pSeg->nPos==0);
}

/*
** Advance iterator pIter to the next entry.
**
** This version of fts5SegIterNext() is only used by reverse iterators.
*/
static void fts5SegIterNext_Reverse(
  Fts5Index *p,                   /* FTS5 backend object */
  Fts5SegIter *pIter,             /* Iterator to advance */
  int *pbUnused                   /* Unused */
){
  assert( pIter->flags & FTS5_SEGITER_REVERSE );
  assert( pIter->pNextLeaf==0 );
  UNUSED_PARAM(pbUnused);

  if( pIter->iRowidOffset>0 ){
    u8 *a = pIter->pLeaf->p;
    int iOff;
    u64 iDelta;

    pIter->iRowidOffset--;
    pIter->iLeafOffset = pIter->aRowidOffset[pIter->iRowidOffset];
    fts5SegIterLoadNPos(p, pIter);
    iOff = pIter->iLeafOffset;
    if( p->pConfig->eDetail!=FTS5_DETAIL_NONE ){
      iOff += pIter->nPos;
    }
    fts5GetVarint(&a[iOff], &iDelta);
    pIter->iRowid -= iDelta;
  }else{
    fts5SegIterReverseNewPage(p, pIter);
  }
}

/*
** Advance iterator pIter to the next entry.
**
** This version of fts5SegIterNext() is only used if detail=none and the
** iterator is not a reverse direction iterator.
*/
static void fts5SegIterNext_None(
  Fts5Index *p,                   /* FTS5 backend object */
  Fts5SegIter *pIter,             /* Iterator to advance */
  int *pbNewTerm                  /* OUT: Set for new term */
){
  int iOff;

  assert( p->rc==SQLITE_OK );
  assert( (pIter->flags & FTS5_SEGITER_REVERSE)==0 );
  assert( p->pConfig->eDetail==FTS5_DETAIL_NONE );

  ASSERT_SZLEAF_OK(pIter->pLeaf);
  iOff = pIter->iLeafOffset;

  /* Next entry is on the next page */
  if( pIter->pSeg && iOff>=pIter->pLeaf->szLeaf ){
    fts5SegIterNextPage(p, pIter);
    if( p->rc || pIter->pLeaf==0 ) return;
    pIter->iRowid = 0;
    iOff = 4;
  }

  if( iOff<pIter->iEndofDoclist ){
    /* Next entry is on the current page */
    i64 iDelta;
    iOff += sqlite3Fts5GetVarint(&pIter->pLeaf->p[iOff], (u64*)&iDelta);
    pIter->iLeafOffset = iOff;
    pIter->iRowid += iDelta;
  }else if( (pIter->flags & FTS5_SEGITER_ONETERM)==0 ){
    if( pIter->pSeg ){
      int nKeep = 0;
      if( iOff!=fts5LeafFirstTermOff(pIter->pLeaf) ){
        iOff += fts5GetVarint32(&pIter->pLeaf->p[iOff], nKeep);
      }
      pIter->iLeafOffset = iOff;
      fts5SegIterLoadTerm(p, pIter, nKeep);
    }else{
      const u8 *pList = 0;
      const char *zTerm = 0;
      int nList;
      sqlite3Fts5HashScanNext(p->pHash);
      sqlite3Fts5HashScanEntry(p->pHash, &zTerm, &pList, &nList);
      if( pList==0 ) goto next_none_eof;
      pIter->pLeaf->p = (u8*)pList;
      pIter->pLeaf->nn = nList;
      pIter->pLeaf->szLeaf = nList;
      pIter->iEndofDoclist = nList;
      sqlite3Fts5BufferSet(&p->rc,&pIter->term, (int)strlen(zTerm), (u8*)zTerm);
      pIter->iLeafOffset = fts5GetVarint(pList, (u64*)&pIter->iRowid);
    }

    if( pbNewTerm ) *pbNewTerm = 1;
  }else{
    goto next_none_eof;
  }

  fts5SegIterLoadNPos(p, pIter);

  return;
 next_none_eof:
  fts5DataRelease(pIter->pLeaf);
  pIter->pLeaf = 0;
}


/*
** Advance iterator pIter to the next entry. 
**
** If an error occurs, Fts5Index.rc is set to an appropriate error code. It 
** is not considered an error if the iterator reaches EOF. If an error has 
** already occurred when this function is called, it is a no-op.
*/
static void fts5SegIterNext(
  Fts5Index *p,                   /* FTS5 backend object */
  Fts5SegIter *pIter,             /* Iterator to advance */
  int *pbNewTerm                  /* OUT: Set for new term */
){
  Fts5Data *pLeaf = pIter->pLeaf;
  int iOff;
  int bNewTerm = 0;
  int nKeep = 0;
  u8 *a;
  int n;

  assert( pbNewTerm==0 || *pbNewTerm==0 );
  assert( p->pConfig->eDetail!=FTS5_DETAIL_NONE );

  /* Search for the end of the position list within the current page. */
  a = pLeaf->p;
  n = pLeaf->szLeaf;

  ASSERT_SZLEAF_OK(pLeaf);
  iOff = pIter->iLeafOffset + pIter->nPos;

  if( iOff<n ){
    /* The next entry is on the current page. */
    assert_nc( iOff<=pIter->iEndofDoclist );
    if( iOff>=pIter->iEndofDoclist ){
      bNewTerm = 1;
      if( iOff!=fts5LeafFirstTermOff(pLeaf) ){
        iOff += fts5GetVarint32(&a[iOff], nKeep);
      }
    }else{
      u64 iDelta;
      iOff += sqlite3Fts5GetVarint(&a[iOff], &iDelta);
      pIter->iRowid += iDelta;
      assert_nc( iDelta>0 );
    }
    pIter->iLeafOffset = iOff;

  }else if( pIter->pSeg==0 ){
    const u8 *pList = 0;
    const char *zTerm = 0;
    int nList = 0;
    assert( (pIter->flags & FTS5_SEGITER_ONETERM) || pbNewTerm );
    if( 0==(pIter->flags & FTS5_SEGITER_ONETERM) ){
      sqlite3Fts5HashScanNext(p->pHash);
      sqlite3Fts5HashScanEntry(p->pHash, &zTerm, &pList, &nList);
    }
    if( pList==0 ){
      fts5DataRelease(pIter->pLeaf);
      pIter->pLeaf = 0;
    }else{
      pIter->pLeaf->p = (u8*)pList;
      pIter->pLeaf->nn = nList;
      pIter->pLeaf->szLeaf = nList;
      pIter->iEndofDoclist = nList+1;
      sqlite3Fts5BufferSet(&p->rc, &pIter->term, (int)strlen(zTerm),
          (u8*)zTerm);
      pIter->iLeafOffset = fts5GetVarint(pList, (u64*)&pIter->iRowid);
      *pbNewTerm = 1;
    }
  }else{
    iOff = 0;
    /* Next entry is not on the current page */
    while( iOff==0 ){
      fts5SegIterNextPage(p, pIter);
      pLeaf = pIter->pLeaf;
      if( pLeaf==0 ) break;
      ASSERT_SZLEAF_OK(pLeaf);
      if( (iOff = fts5LeafFirstRowidOff(pLeaf)) && iOff<pLeaf->szLeaf ){
        iOff += sqlite3Fts5GetVarint(&pLeaf->p[iOff], (u64*)&pIter->iRowid);
        pIter->iLeafOffset = iOff;

        if( pLeaf->nn>pLeaf->szLeaf ){
          pIter->iPgidxOff = pLeaf->szLeaf + fts5GetVarint32(
              &pLeaf->p[pLeaf->szLeaf], pIter->iEndofDoclist
          );
        }
      }
      else if( pLeaf->nn>pLeaf->szLeaf ){
        pIter->iPgidxOff = pLeaf->szLeaf + fts5GetVarint32(
            &pLeaf->p[pLeaf->szLeaf], iOff
        );
        pIter->iLeafOffset = iOff;
        pIter->iEndofDoclist = iOff;
        bNewTerm = 1;
      }
      assert_nc( iOff<pLeaf->szLeaf );
      if( iOff>pLeaf->szLeaf ){
        p->rc = FTS5_CORRUPT;
        return;
      }
    }
  }

  /* Check if the iterator is now at EOF. If so, return early. */
  if( pIter->pLeaf ){
    if( bNewTerm ){
      if( pIter->flags & FTS5_SEGITER_ONETERM ){
        fts5DataRelease(pIter->pLeaf);
        pIter->pLeaf = 0;
      }else{
        fts5SegIterLoadTerm(p, pIter, nKeep);
        fts5SegIterLoadNPos(p, pIter);
        if( pbNewTerm ) *pbNewTerm = 1;
      }
    }else{
      /* The following could be done by calling fts5SegIterLoadNPos(). But
      ** this block is particularly performance critical, so equivalent
      ** code is inlined.  */
      int nSz;
      assert_nc( pIter->iLeafOffset<=pIter->pLeaf->nn );
      fts5FastGetVarint32(pIter->pLeaf->p, pIter->iLeafOffset, nSz);
      pIter->bDel = (nSz & 0x0001);
      pIter->nPos = nSz>>1;
      assert_nc( pIter->nPos>=0 );
    }
  }
}

#define SWAPVAL(T, a, b) { T tmp; tmp=a; a=b; b=tmp; }

#define fts5IndexSkipVarint(a, iOff) {            \
  int iEnd = iOff+9;                              \
  while( (a[iOff++] & 0x80) && iOff<iEnd );       \
}

/*
** Iterator pIter currently points to the first rowid in a doclist. This
** function sets the iterator up so that iterates in reverse order through
** the doclist.
*/
static void fts5SegIterReverse(Fts5Index *p, Fts5SegIter *pIter){
  Fts5DlidxIter *pDlidx = pIter->pDlidx;
  Fts5Data *pLast = 0;
  int pgnoLast = 0;

  if( pDlidx ){
    int iSegid = pIter->pSeg->iSegid;
    pgnoLast = fts5DlidxIterPgno(pDlidx);
    pLast = fts5LeafRead(p, FTS5_SEGMENT_ROWID(iSegid, pgnoLast));
  }else{
    Fts5Data *pLeaf = pIter->pLeaf;         /* Current leaf data */

    /* Currently, Fts5SegIter.iLeafOffset points to the first byte of
    ** position-list content for the current rowid. Back it up so that it
    ** points to the start of the position-list size field. */
    int iPoslist;
    if( pIter->iTermLeafPgno==pIter->iLeafPgno ){
      iPoslist = pIter->iTermLeafOffset;
    }else{
      iPoslist = 4;
    }
    fts5IndexSkipVarint(pLeaf->p, iPoslist);
    pIter->iLeafOffset = iPoslist;

    /* If this condition is true then the largest rowid for the current
    ** term may not be stored on the current page. So search forward to
    ** see where said rowid really is.  */
    if( pIter->iEndofDoclist>=pLeaf->szLeaf ){
      int pgno;
      Fts5StructureSegment *pSeg = pIter->pSeg;

      /* The last rowid in the doclist may not be on the current page. Search
      ** forward to find the page containing the last rowid.  */
      for(pgno=pIter->iLeafPgno+1; !p->rc && pgno<=pSeg->pgnoLast; pgno++){
        i64 iAbs = FTS5_SEGMENT_ROWID(pSeg->iSegid, pgno);
        Fts5Data *pNew = fts5LeafRead(p, iAbs);
        if( pNew ){
          int iRowid, bTermless;
          iRowid = fts5LeafFirstRowidOff(pNew);
          bTermless = fts5LeafIsTermless(pNew);
          if( iRowid ){
            SWAPVAL(Fts5Data*, pNew, pLast);
            pgnoLast = pgno;
          }
          fts5DataRelease(pNew);
          if( bTermless==0 ) break;
        }
      }
    }
  }

  /* If pLast is NULL at this point, then the last rowid for this doclist
  ** lies on the page currently indicated by the iterator. In this case 
  ** pIter->iLeafOffset is already set to point to the position-list size
  ** field associated with the first relevant rowid on the page.
  **
  ** Or, if pLast is non-NULL, then it is the page that contains the last
  ** rowid. In this case configure the iterator so that it points to the
  ** first rowid on this page.
  */
  if( pLast ){
    int iOff;
    fts5DataRelease(pIter->pLeaf);
    pIter->pLeaf = pLast;
    pIter->iLeafPgno = pgnoLast;
    iOff = fts5LeafFirstRowidOff(pLast);
    if( iOff>pLast->szLeaf ){
      p->rc = FTS5_CORRUPT;
      return;
    }
    iOff += fts5GetVarint(&pLast->p[iOff], (u64*)&pIter->iRowid);
    pIter->iLeafOffset = iOff;

    if( fts5LeafIsTermless(pLast) ){
      pIter->iEndofDoclist = pLast->nn+1;
    }else{
      pIter->iEndofDoclist = fts5LeafFirstTermOff(pLast);
    }
  }

  fts5SegIterReverseInitPage(p, pIter);
}

/*
** Iterator pIter currently points to the first rowid of a doclist.
** There is a doclist-index associated with the final term on the current 
** page. If the current term is the last term on the page, load the 
** doclist-index from disk and initialize an iterator at (pIter->pDlidx).
*/
static void fts5SegIterLoadDlidx(Fts5Index *p, Fts5SegIter *pIter){
  int iSeg = pIter->pSeg->iSegid;
  int bRev = (pIter->flags & FTS5_SEGITER_REVERSE);
  Fts5Data *pLeaf = pIter->pLeaf; /* Current leaf data */

  assert( pIter->flags & FTS5_SEGITER_ONETERM );
  assert( pIter->pDlidx==0 );

  /* Check if the current doclist ends on this page. If it does, return
  ** early without loading the doclist-index (as it belongs to a different
  ** term. */
  if( pIter->iTermLeafPgno==pIter->iLeafPgno 
   && pIter->iEndofDoclist<pLeaf->szLeaf 
  ){
    return;
  }

  pIter->pDlidx = fts5DlidxIterInit(p, bRev, iSeg, pIter->iTermLeafPgno);
}

/*
** The iterator object passed as the second argument currently contains
** no valid values except for the Fts5SegIter.pLeaf member variable. This
** function searches the leaf page for a term matching (pTerm/nTerm).
**
** If the specified term is found on the page, then the iterator is left
** pointing to it. If argument bGe is zero and the term is not found,
** the iterator is left pointing at EOF.
**
** If bGe is non-zero and the specified term is not found, then the
** iterator is left pointing to the smallest term in the segment that
** is larger than the specified term, even if this term is not on the
** current page.
*/
static void fts5LeafSeek(
  Fts5Index *p,                   /* Leave any error code here */
  int bGe,                        /* True for a >= search */
  Fts5SegIter *pIter,             /* Iterator to seek */
  const u8 *pTerm, int nTerm      /* Term to search for */
){
  u32 iOff;
  const u8 *a = pIter->pLeaf->p;
  u32 n = (u32)pIter->pLeaf->nn;

  u32 nMatch = 0;
  u32 nKeep = 0;
  u32 nNew = 0;
  u32 iTermOff;
  u32 iPgidx;                     /* Current offset in pgidx */
  int bEndOfPage = 0;

  assert( p->rc==SQLITE_OK );

  iPgidx = (u32)pIter->pLeaf->szLeaf;
  iPgidx += fts5GetVarint32(&a[iPgidx], iTermOff);
  iOff = iTermOff;
  if( iOff>n ){
    p->rc = FTS5_CORRUPT;
    return;
  }

  while( 1 ){

    /* Figure out how many new bytes are in this term */
    fts5FastGetVarint32(a, iOff, nNew);
    if( nKeep<nMatch ){
      goto search_failed;
    }

    assert( nKeep>=nMatch );
    if( nKeep==nMatch ){
      u32 nCmp;
      u32 i;
      nCmp = (u32)MIN(nNew, nTerm-nMatch);
      for(i=0; i<nCmp; i++){
        if( a[iOff+i]!=pTerm[nMatch+i] ) break;
      }
      nMatch += i;

      if( (u32)nTerm==nMatch ){
        if( i==nNew ){
          goto search_success;
        }else{
          goto search_failed;
        }
      }else if( i<nNew && a[iOff+i]>pTerm[nMatch] ){
        goto search_failed;
      }
    }

    if( iPgidx>=n ){
      bEndOfPage = 1;
      break;
    }

    iPgidx += fts5GetVarint32(&a[iPgidx], nKeep);
    iTermOff += nKeep;
    iOff = iTermOff;

    if( iOff>=n ){
      p->rc = FTS5_CORRUPT;
      return;
    }

    /* Read the nKeep field of the next term. */
    fts5FastGetVarint32(a, iOff, nKeep);
  }

 search_failed:
  if( bGe==0 ){
    fts5DataRelease(pIter->pLeaf);
    pIter->pLeaf = 0;
    return;
  }else if( bEndOfPage ){
    do {
      fts5SegIterNextPage(p, pIter);
      if( pIter->pLeaf==0 ) return;
      a = pIter->pLeaf->p;
      if( fts5LeafIsTermless(pIter->pLeaf)==0 ){
        iPgidx = (u32)pIter->pLeaf->szLeaf;
        iPgidx += fts5GetVarint32(&pIter->pLeaf->p[iPgidx], iOff);
        if( iOff<4 || (i64)iOff>=pIter->pLeaf->szLeaf ){
          p->rc = FTS5_CORRUPT;
          return;
        }else{
          nKeep = 0;
          iTermOff = iOff;
          n = (u32)pIter->pLeaf->nn;
          iOff += fts5GetVarint32(&a[iOff], nNew);
          break;
        }
      }
    }while( 1 );
  }

 search_success:
  if( (i64)iOff+nNew>n || nNew<1 ){
    p->rc = FTS5_CORRUPT;
    return;
  }
  pIter->iLeafOffset = iOff + nNew;
  pIter->iTermLeafOffset = pIter->iLeafOffset;
  pIter->iTermLeafPgno = pIter->iLeafPgno;

  fts5BufferSet(&p->rc, &pIter->term, nKeep, pTerm);
  fts5BufferAppendBlob(&p->rc, &pIter->term, nNew, &a[iOff]);

  if( iPgidx>=n ){
    pIter->iEndofDoclist = pIter->pLeaf->nn+1;
  }else{
    int nExtra;
    iPgidx += fts5GetVarint32(&a[iPgidx], nExtra);
    pIter->iEndofDoclist = iTermOff + nExtra;
  }
  pIter->iPgidxOff = iPgidx;

  fts5SegIterLoadRowid(p, pIter);
  fts5SegIterLoadNPos(p, pIter);
}

static sqlite3_stmt *fts5IdxSelectStmt(Fts5Index *p){
  if( p->pIdxSelect==0 ){
    Fts5Config *pConfig = p->pConfig;
    fts5IndexPrepareStmt(p, &p->pIdxSelect, sqlite3_mprintf(
          "SELECT pgno FROM '%q'.'%q_idx' WHERE "
          "segid=? AND term<=? ORDER BY term DESC LIMIT 1",
          pConfig->zDb, pConfig->zName
    ));
  }
  return p->pIdxSelect;
}

/*
** Initialize the object pIter to point to term pTerm/nTerm within segment
** pSeg. If there is no such term in the index, the iterator is set to EOF.
**
** If an error occurs, Fts5Index.rc is set to an appropriate error code. If 
** an error has already occurred when this function is called, it is a no-op.
*/
static void fts5SegIterSeekInit(
  Fts5Index *p,                   /* FTS5 backend */
  const u8 *pTerm, int nTerm,     /* Term to seek to */
  int flags,                      /* Mask of FTS5INDEX_XXX flags */
  Fts5StructureSegment *pSeg,     /* Description of segment */
  Fts5SegIter *pIter              /* Object to populate */
){
  int iPg = 1;
  int bGe = (flags & FTS5INDEX_QUERY_SCAN);
  int bDlidx = 0;                 /* True if there is a doclist-index */
  sqlite3_stmt *pIdxSelect = 0;

  assert( bGe==0 || (flags & FTS5INDEX_QUERY_DESC)==0 );
  assert( pTerm && nTerm );
  memset(pIter, 0, sizeof(*pIter));
  pIter->pSeg = pSeg;

  /* This block sets stack variable iPg to the leaf page number that may
  ** contain term (pTerm/nTerm), if it is present in the segment. */
  pIdxSelect = fts5IdxSelectStmt(p);
  if( p->rc ) return;
  sqlite3_bind_int(pIdxSelect, 1, pSeg->iSegid);
  sqlite3_bind_blob(pIdxSelect, 2, pTerm, nTerm, SQLITE_STATIC);
  if( SQLITE_ROW==sqlite3_step(pIdxSelect) ){
    i64 val = sqlite3_column_int(pIdxSelect, 0);
    iPg = (int)(val>>1);
    bDlidx = (val & 0x0001);
  }
  p->rc = sqlite3_reset(pIdxSelect);
  sqlite3_bind_null(pIdxSelect, 2);

  if( iPg<pSeg->pgnoFirst ){
    iPg = pSeg->pgnoFirst;
    bDlidx = 0;
  }

  pIter->iLeafPgno = iPg - 1;
  fts5SegIterNextPage(p, pIter);

  if( pIter->pLeaf ){
    fts5LeafSeek(p, bGe, pIter, pTerm, nTerm);
  }

  if( p->rc==SQLITE_OK && bGe==0 ){
    pIter->flags |= FTS5_SEGITER_ONETERM;
    if( pIter->pLeaf ){
      if( flags & FTS5INDEX_QUERY_DESC ){
        pIter->flags |= FTS5_SEGITER_REVERSE;
      }
      if( bDlidx ){
        fts5SegIterLoadDlidx(p, pIter);
      }
      if( flags & FTS5INDEX_QUERY_DESC ){
        fts5SegIterReverse(p, pIter);
      }
    }
  }

  fts5SegIterSetNext(p, pIter);

  /* Either:
  **
  **   1) an error has occurred, or
  **   2) the iterator points to EOF, or
  **   3) the iterator points to an entry with term (pTerm/nTerm), or
  **   4) the FTS5INDEX_QUERY_SCAN flag was set and the iterator points
  **      to an entry with a term greater than or equal to (pTerm/nTerm).
  */
  assert_nc( p->rc!=SQLITE_OK                                       /* 1 */
   || pIter->pLeaf==0                                               /* 2 */
   || fts5BufferCompareBlob(&pIter->term, pTerm, nTerm)==0          /* 3 */
   || (bGe && fts5BufferCompareBlob(&pIter->term, pTerm, nTerm)>0)  /* 4 */
  );
}

/*
** Initialize the object pIter to point to term pTerm/nTerm within the
** in-memory hash table. If there is no such term in the hash-table, the 
** iterator is set to EOF.
**
** If an error occurs, Fts5Index.rc is set to an appropriate error code. If 
** an error has already occurred when this function is called, it is a no-op.
*/
static void fts5SegIterHashInit(
  Fts5Index *p,                   /* FTS5 backend */
  const u8 *pTerm, int nTerm,     /* Term to seek to */
  int flags,                      /* Mask of FTS5INDEX_XXX flags */
  Fts5SegIter *pIter              /* Object to populate */
){
  int nList = 0;
  const u8 *z = 0;
  int n = 0;
  Fts5Data *pLeaf = 0;

  assert( p->pHash );
  assert( p->rc==SQLITE_OK );

  if( pTerm==0 || (flags & FTS5INDEX_QUERY_SCAN) ){
    const u8 *pList = 0;

    p->rc = sqlite3Fts5HashScanInit(p->pHash, (const char*)pTerm, nTerm);
    sqlite3Fts5HashScanEntry(p->pHash, (const char**)&z, &pList, &nList);
    n = (z ? (int)strlen((const char*)z) : 0);
    if( pList ){
      pLeaf = fts5IdxMalloc(p, sizeof(Fts5Data));
      if( pLeaf ){
        pLeaf->p = (u8*)pList;
      }
    }
  }else{
    p->rc = sqlite3Fts5HashQuery(p->pHash, sizeof(Fts5Data), 
        (const char*)pTerm, nTerm, (void**)&pLeaf, &nList
    );
    if( pLeaf ){
      pLeaf->p = (u8*)&pLeaf[1];
    }
    z = pTerm;
    n = nTerm;
    pIter->flags |= FTS5_SEGITER_ONETERM;
  }

  if( pLeaf ){
    sqlite3Fts5BufferSet(&p->rc, &pIter->term, n, z);
    pLeaf->nn = pLeaf->szLeaf = nList;
    pIter->pLeaf = pLeaf;
    pIter->iLeafOffset = fts5GetVarint(pLeaf->p, (u64*)&pIter->iRowid);
    pIter->iEndofDoclist = pLeaf->nn;

    if( flags & FTS5INDEX_QUERY_DESC ){
      pIter->flags |= FTS5_SEGITER_REVERSE;
      fts5SegIterReverseInitPage(p, pIter);
    }else{
      fts5SegIterLoadNPos(p, pIter);
    }
  }

  fts5SegIterSetNext(p, pIter);
}

/*
** Zero the iterator passed as the only argument.
*/
static void fts5SegIterClear(Fts5SegIter *pIter){
  fts5BufferFree(&pIter->term);
  fts5DataRelease(pIter->pLeaf);
  fts5DataRelease(pIter->pNextLeaf);
  fts5DlidxIterFree(pIter->pDlidx);
  sqlite3_free(pIter->aRowidOffset);
  memset(pIter, 0, sizeof(Fts5SegIter));
}

#ifdef SQLITE_DEBUG

/*
** This function is used as part of the big assert() procedure implemented by
** fts5AssertMultiIterSetup(). It ensures that the result currently stored
** in *pRes is the correct result of comparing the current positions of the
** two iterators.
*/
static void fts5AssertComparisonResult(
  Fts5Iter *pIter, 
  Fts5SegIter *p1,
  Fts5SegIter *p2,
  Fts5CResult *pRes
){
  int i1 = p1 - pIter->aSeg;
  int i2 = p2 - pIter->aSeg;

  if( p1->pLeaf || p2->pLeaf ){
    if( p1->pLeaf==0 ){
      assert( pRes->iFirst==i2 );
    }else if( p2->pLeaf==0 ){
      assert( pRes->iFirst==i1 );
    }else{
      int nMin = MIN(p1->term.n, p2->term.n);
      int res = fts5Memcmp(p1->term.p, p2->term.p, nMin);
      if( res==0 ) res = p1->term.n - p2->term.n;

      if( res==0 ){
        assert( pRes->bTermEq==1 );
        assert( p1->iRowid!=p2->iRowid );
        res = ((p1->iRowid > p2->iRowid)==pIter->bRev) ? -1 : 1;
      }else{
        assert( pRes->bTermEq==0 );
      }

      if( res<0 ){
        assert( pRes->iFirst==i1 );
      }else{
        assert( pRes->iFirst==i2 );
      }
    }
  }
}

/*
** This function is a no-op unless SQLITE_DEBUG is defined when this module
** is compiled. In that case, this function is essentially an assert() 
** statement used to verify that the contents of the pIter->aFirst[] array
** are correct.
*/
static void fts5AssertMultiIterSetup(Fts5Index *p, Fts5Iter *pIter){
  if( p->rc==SQLITE_OK ){
    Fts5SegIter *pFirst = &pIter->aSeg[ pIter->aFirst[1].iFirst ];
    int i;

    assert( (pFirst->pLeaf==0)==pIter->base.bEof );

    /* Check that pIter->iSwitchRowid is set correctly. */
    for(i=0; i<pIter->nSeg; i++){
      Fts5SegIter *p1 = &pIter->aSeg[i];
      assert( p1==pFirst 
           || p1->pLeaf==0 
           || fts5BufferCompare(&pFirst->term, &p1->term) 
           || p1->iRowid==pIter->iSwitchRowid
           || (p1->iRowid<pIter->iSwitchRowid)==pIter->bRev
      );
    }

    for(i=0; i<pIter->nSeg; i+=2){
      Fts5SegIter *p1 = &pIter->aSeg[i];
      Fts5SegIter *p2 = &pIter->aSeg[i+1];
      Fts5CResult *pRes = &pIter->aFirst[(pIter->nSeg + i) / 2];
      fts5AssertComparisonResult(pIter, p1, p2, pRes);
    }

    for(i=1; i<(pIter->nSeg / 2); i+=2){
      Fts5SegIter *p1 = &pIter->aSeg[ pIter->aFirst[i*2].iFirst ];
      Fts5SegIter *p2 = &pIter->aSeg[ pIter->aFirst[i*2+1].iFirst ];
      Fts5CResult *pRes = &pIter->aFirst[i];
      fts5AssertComparisonResult(pIter, p1, p2, pRes);
    }
  }
}
#else
# define fts5AssertMultiIterSetup(x,y)
#endif

/*
** Do the comparison necessary to populate pIter->aFirst[iOut].
**
** If the returned value is non-zero, then it is the index of an entry
** in the pIter->aSeg[] array that is (a) not at EOF, and (b) pointing
** to a key that is a duplicate of another, higher priority, 
** segment-iterator in the pSeg->aSeg[] array.
*/
static int fts5MultiIterDoCompare(Fts5Iter *pIter, int iOut){
  int i1;                         /* Index of left-hand Fts5SegIter */
  int i2;                         /* Index of right-hand Fts5SegIter */
  int iRes;
  Fts5SegIter *p1;                /* Left-hand Fts5SegIter */
  Fts5SegIter *p2;                /* Right-hand Fts5SegIter */
  Fts5CResult *pRes = &pIter->aFirst[iOut];

  assert( iOut<pIter->nSeg && iOut>0 );
  assert( pIter->bRev==0 || pIter->bRev==1 );

  if( iOut>=(pIter->nSeg/2) ){
    i1 = (iOut - pIter->nSeg/2) * 2;
    i2 = i1 + 1;
  }else{
    i1 = pIter->aFirst[iOut*2].iFirst;
    i2 = pIter->aFirst[iOut*2+1].iFirst;
  }
  p1 = &pIter->aSeg[i1];
  p2 = &pIter->aSeg[i2];

  pRes->bTermEq = 0;
  if( p1->pLeaf==0 ){           /* If p1 is at EOF */
    iRes = i2;
  }else if( p2->pLeaf==0 ){     /* If p2 is at EOF */
    iRes = i1;
  }else{
    int res = fts5BufferCompare(&p1->term, &p2->term);
    if( res==0 ){
      assert_nc( i2>i1 );
      assert_nc( i2!=0 );
      pRes->bTermEq = 1;
      if( p1->iRowid==p2->iRowid ){
        p1->bDel = p2->bDel;
        return i2;
      }
      res = ((p1->iRowid > p2->iRowid)==pIter->bRev) ? -1 : +1;
    }
    assert( res!=0 );
    if( res<0 ){
      iRes = i1;
    }else{
      iRes = i2;
    }
  }

  pRes->iFirst = (u16)iRes;
  return 0;
}

/*
** Move the seg-iter so that it points to the first rowid on page iLeafPgno.
** It is an error if leaf iLeafPgno does not exist or contains no rowids.
*/
static void fts5SegIterGotoPage(
  Fts5Index *p,                   /* FTS5 backend object */
  Fts5SegIter *pIter,             /* Iterator to advance */
  int iLeafPgno
){
  assert( iLeafPgno>pIter->iLeafPgno );

  if( iLeafPgno>pIter->pSeg->pgnoLast ){
    p->rc = FTS5_CORRUPT;
  }else{
    fts5DataRelease(pIter->pNextLeaf);
    pIter->pNextLeaf = 0;
    pIter->iLeafPgno = iLeafPgno-1;
    fts5SegIterNextPage(p, pIter);
    assert( p->rc!=SQLITE_OK || pIter->iLeafPgno==iLeafPgno );

    if( p->rc==SQLITE_OK && ALWAYS(pIter->pLeaf!=0) ){
      int iOff;
      u8 *a = pIter->pLeaf->p;
      int n = pIter->pLeaf->szLeaf;

      iOff = fts5LeafFirstRowidOff(pIter->pLeaf);
      if( iOff<4 || iOff>=n ){
        p->rc = FTS5_CORRUPT;
      }else{
        iOff += fts5GetVarint(&a[iOff], (u64*)&pIter->iRowid);
        pIter->iLeafOffset = iOff;
        fts5SegIterLoadNPos(p, pIter);
      }
    }
  }
}

/*
** Advance the iterator passed as the second argument until it is at or 
** past rowid iFrom. Regardless of the value of iFrom, the iterator is
** always advanced at least once.
*/
static void fts5SegIterNextFrom(
  Fts5Index *p,                   /* FTS5 backend object */
  Fts5SegIter *pIter,             /* Iterator to advance */
  i64 iMatch                      /* Advance iterator at least this far */
){
  int bRev = (pIter->flags & FTS5_SEGITER_REVERSE);
  Fts5DlidxIter *pDlidx = pIter->pDlidx;
  int iLeafPgno = pIter->iLeafPgno;
  int bMove = 1;

  assert( pIter->flags & FTS5_SEGITER_ONETERM );
  assert( pIter->pDlidx );
  assert( pIter->pLeaf );

  if( bRev==0 ){
    while( !fts5DlidxIterEof(p, pDlidx) && iMatch>fts5DlidxIterRowid(pDlidx) ){
      iLeafPgno = fts5DlidxIterPgno(pDlidx);
      fts5DlidxIterNext(p, pDlidx);
    }
    assert_nc( iLeafPgno>=pIter->iLeafPgno || p->rc );
    if( iLeafPgno>pIter->iLeafPgno ){
      fts5SegIterGotoPage(p, pIter, iLeafPgno);
      bMove = 0;
    }
  }else{
    assert( pIter->pNextLeaf==0 );
    assert( iMatch<pIter->iRowid );
    while( !fts5DlidxIterEof(p, pDlidx) && iMatch<fts5DlidxIterRowid(pDlidx) ){
      fts5DlidxIterPrev(p, pDlidx);
    }
    iLeafPgno = fts5DlidxIterPgno(pDlidx);

    assert( fts5DlidxIterEof(p, pDlidx) || iLeafPgno<=pIter->iLeafPgno );

    if( iLeafPgno<pIter->iLeafPgno ){
      pIter->iLeafPgno = iLeafPgno+1;
      fts5SegIterReverseNewPage(p, pIter);
      bMove = 0;
    }
  }

  do{
    if( bMove && p->rc==SQLITE_OK ) pIter->xNext(p, pIter, 0);
    if( pIter->pLeaf==0 ) break;
    if( bRev==0 && pIter->iRowid>=iMatch ) break;
    if( bRev!=0 && pIter->iRowid<=iMatch ) break;
    bMove = 1;
  }while( p->rc==SQLITE_OK );
}


/*
** Free the iterator object passed as the second argument.
*/
static void fts5MultiIterFree(Fts5Iter *pIter){
  if( pIter ){
    int i;
    for(i=0; i<pIter->nSeg; i++){
      fts5SegIterClear(&pIter->aSeg[i]);
    }
    fts5BufferFree(&pIter->poslist);
    sqlite3_free(pIter);
  }
}

static void fts5MultiIterAdvanced(
  Fts5Index *p,                   /* FTS5 backend to iterate within */
  Fts5Iter *pIter,                /* Iterator to update aFirst[] array for */
  int iChanged,                   /* Index of sub-iterator just advanced */
  int iMinset                     /* Minimum entry in aFirst[] to set */
){
  int i;
  for(i=(pIter->nSeg+iChanged)/2; i>=iMinset && p->rc==SQLITE_OK; i=i/2){
    int iEq;
    if( (iEq = fts5MultiIterDoCompare(pIter, i)) ){
      Fts5SegIter *pSeg = &pIter->aSeg[iEq];
      assert( p->rc==SQLITE_OK );
      pSeg->xNext(p, pSeg, 0);
      i = pIter->nSeg + iEq;
    }
  }
}

/*
** Sub-iterator iChanged of iterator pIter has just been advanced. It still
** points to the same term though - just a different rowid. This function
** attempts to update the contents of the pIter->aFirst[] accordingly.
** If it does so successfully, 0 is returned. Otherwise 1.
**
** If non-zero is returned, the caller should call fts5MultiIterAdvanced()
** on the iterator instead. That function does the same as this one, except
** that it deals with more complicated cases as well.
*/ 
static int fts5MultiIterAdvanceRowid(
  Fts5Iter *pIter,                /* Iterator to update aFirst[] array for */
  int iChanged,                   /* Index of sub-iterator just advanced */
  Fts5SegIter **ppFirst
){
  Fts5SegIter *pNew = &pIter->aSeg[iChanged];

  if( pNew->iRowid==pIter->iSwitchRowid
   || (pNew->iRowid<pIter->iSwitchRowid)==pIter->bRev
  ){
    int i;
    Fts5SegIter *pOther = &pIter->aSeg[iChanged ^ 0x0001];
    pIter->iSwitchRowid = pIter->bRev ? SMALLEST_INT64 : LARGEST_INT64;
    for(i=(pIter->nSeg+iChanged)/2; 1; i=i/2){
      Fts5CResult *pRes = &pIter->aFirst[i];

      assert( pNew->pLeaf );
      assert( pRes->bTermEq==0 || pOther->pLeaf );

      if( pRes->bTermEq ){
        if( pNew->iRowid==pOther->iRowid ){
          return 1;
        }else if( (pOther->iRowid>pNew->iRowid)==pIter->bRev ){
          pIter->iSwitchRowid = pOther->iRowid;
          pNew = pOther;
        }else if( (pOther->iRowid>pIter->iSwitchRowid)==pIter->bRev ){
          pIter->iSwitchRowid = pOther->iRowid;
        }
      }
      pRes->iFirst = (u16)(pNew - pIter->aSeg);
      if( i==1 ) break;

      pOther = &pIter->aSeg[ pIter->aFirst[i ^ 0x0001].iFirst ];
    }
  }

  *ppFirst = pNew;
  return 0;
}

/*
** Set the pIter->bEof variable based on the state of the sub-iterators.
*/
static void fts5MultiIterSetEof(Fts5Iter *pIter){
  Fts5SegIter *pSeg = &pIter->aSeg[ pIter->aFirst[1].iFirst ];
  pIter->base.bEof = pSeg->pLeaf==0;
  pIter->iSwitchRowid = pSeg->iRowid;
}

/*
** Move the iterator to the next entry. 
**
** If an error occurs, an error code is left in Fts5Index.rc. It is not 
** considered an error if the iterator reaches EOF, or if it is already at 
** EOF when this function is called.
*/
static void fts5MultiIterNext(
  Fts5Index *p, 
  Fts5Iter *pIter,
  int bFrom,                      /* True if argument iFrom is valid */
  i64 iFrom                       /* Advance at least as far as this */
){
  int bUseFrom = bFrom;
  assert( pIter->base.bEof==0 );
  while( p->rc==SQLITE_OK ){
    int iFirst = pIter->aFirst[1].iFirst;
    int bNewTerm = 0;
    Fts5SegIter *pSeg = &pIter->aSeg[iFirst];
    assert( p->rc==SQLITE_OK );
    if( bUseFrom && pSeg->pDlidx ){
      fts5SegIterNextFrom(p, pSeg, iFrom);
    }else{
      pSeg->xNext(p, pSeg, &bNewTerm);
    }

    if( pSeg->pLeaf==0 || bNewTerm 
     || fts5MultiIterAdvanceRowid(pIter, iFirst, &pSeg)
    ){
      fts5MultiIterAdvanced(p, pIter, iFirst, 1);
      fts5MultiIterSetEof(pIter);
      pSeg = &pIter->aSeg[pIter->aFirst[1].iFirst];
      if( pSeg->pLeaf==0 ) return;
    }

    fts5AssertMultiIterSetup(p, pIter);
    assert( pSeg==&pIter->aSeg[pIter->aFirst[1].iFirst] && pSeg->pLeaf );
    if( pIter->bSkipEmpty==0 || pSeg->nPos ){
      pIter->xSetOutputs(pIter, pSeg);
      return;
    }
    bUseFrom = 0;
  }
}

static void fts5MultiIterNext2(
  Fts5Index *p, 
  Fts5Iter *pIter,
  int *pbNewTerm                  /* OUT: True if *might* be new term */
){
  assert( pIter->bSkipEmpty );
  if( p->rc==SQLITE_OK ){
    *pbNewTerm = 0;
    do{
      int iFirst = pIter->aFirst[1].iFirst;
      Fts5SegIter *pSeg = &pIter->aSeg[iFirst];
      int bNewTerm = 0;

      assert( p->rc==SQLITE_OK );
      pSeg->xNext(p, pSeg, &bNewTerm);
      if( pSeg->pLeaf==0 || bNewTerm 
       || fts5MultiIterAdvanceRowid(pIter, iFirst, &pSeg)
      ){
        fts5MultiIterAdvanced(p, pIter, iFirst, 1);
        fts5MultiIterSetEof(pIter);
        *pbNewTerm = 1;
      }
      fts5AssertMultiIterSetup(p, pIter);

    }while( fts5MultiIterIsEmpty(p, pIter) );
  }
}

static void fts5IterSetOutputs_Noop(Fts5Iter *pUnused1, Fts5SegIter *pUnused2){
  UNUSED_PARAM2(pUnused1, pUnused2);
}

static Fts5Iter *fts5MultiIterAlloc(
  Fts5Index *p,                   /* FTS5 backend to iterate within */
  int nSeg
){
  Fts5Iter *pNew;
  int nSlot;                      /* Power of two >= nSeg */

  for(nSlot=2; nSlot<nSeg; nSlot=nSlot*2);
  pNew = fts5IdxMalloc(p, 
      sizeof(Fts5Iter) +                  /* pNew */
      sizeof(Fts5SegIter) * (nSlot-1) +   /* pNew->aSeg[] */
      sizeof(Fts5CResult) * nSlot         /* pNew->aFirst[] */
  );
  if( pNew ){
    pNew->nSeg = nSlot;
    pNew->aFirst = (Fts5CResult*)&pNew->aSeg[nSlot];
    pNew->pIndex = p;
    pNew->xSetOutputs = fts5IterSetOutputs_Noop;
  }
  return pNew;
}

static void fts5PoslistCallback(
  Fts5Index *pUnused, 
  void *pContext, 
  const u8 *pChunk, int nChunk
){
  UNUSED_PARAM(pUnused);
  assert_nc( nChunk>=0 );
  if( nChunk>0 ){
    fts5BufferSafeAppendBlob((Fts5Buffer*)pContext, pChunk, nChunk);
  }
}

typedef struct PoslistCallbackCtx PoslistCallbackCtx;
struct PoslistCallbackCtx {
  Fts5Buffer *pBuf;               /* Append to this buffer */
  Fts5Colset *pColset;            /* Restrict matches to this column */
  int eState;                     /* See above */
};

typedef struct PoslistOffsetsCtx PoslistOffsetsCtx;
struct PoslistOffsetsCtx {
  Fts5Buffer *pBuf;               /* Append to this buffer */
  Fts5Colset *pColset;            /* Restrict matches to this column */
  int iRead;
  int iWrite;
};

/*
** TODO: Make this more efficient!
*/
static int fts5IndexColsetTest(Fts5Colset *pColset, int iCol){
  int i;
  for(i=0; i<pColset->nCol; i++){
    if( pColset->aiCol[i]==iCol ) return 1;
  }
  return 0;
}

static void fts5PoslistOffsetsCallback(
  Fts5Index *pUnused, 
  void *pContext, 
  const u8 *pChunk, int nChunk
){
  PoslistOffsetsCtx *pCtx = (PoslistOffsetsCtx*)pContext;
  UNUSED_PARAM(pUnused);
  assert_nc( nChunk>=0 );
  if( nChunk>0 ){
    int i = 0;
    while( i<nChunk ){
      int iVal;
      i += fts5GetVarint32(&pChunk[i], iVal);
      iVal += pCtx->iRead - 2;
      pCtx->iRead = iVal;
      if( fts5IndexColsetTest(pCtx->pColset, iVal) ){
        fts5BufferSafeAppendVarint(pCtx->pBuf, iVal + 2 - pCtx->iWrite);
        pCtx->iWrite = iVal;
      }
    }
  }
}

static void fts5PoslistFilterCallback(
  Fts5Index *pUnused,
  void *pContext, 
  const u8 *pChunk, int nChunk
){
  PoslistCallbackCtx *pCtx = (PoslistCallbackCtx*)pContext;
  UNUSED_PARAM(pUnused);
  assert_nc( nChunk>=0 );
  if( nChunk>0 ){
    /* Search through to find the first varint with value 1. This is the
    ** start of the next columns hits. */
    int i = 0;
    int iStart = 0;

    if( pCtx->eState==2 ){
      int iCol;
      fts5FastGetVarint32(pChunk, i, iCol);
      if( fts5IndexColsetTest(pCtx->pColset, iCol) ){
        pCtx->eState = 1;
        fts5BufferSafeAppendVarint(pCtx->pBuf, 1);
      }else{
        pCtx->eState = 0;
      }
    }

    do {
      while( i<nChunk && pChunk[i]!=0x01 ){
        while( pChunk[i] & 0x80 ) i++;
        i++;
      }
      if( pCtx->eState ){
        fts5BufferSafeAppendBlob(pCtx->pBuf, &pChunk[iStart], i-iStart);
      }
      if( i<nChunk ){
        int iCol;
        iStart = i;
        i++;
        if( i>=nChunk ){
          pCtx->eState = 2;
        }else{
          fts5FastGetVarint32(pChunk, i, iCol);
          pCtx->eState = fts5IndexColsetTest(pCtx->pColset, iCol);
          if( pCtx->eState ){
            fts5BufferSafeAppendBlob(pCtx->pBuf, &pChunk[iStart], i-iStart);
            iStart = i;
          }
        }
      }
    }while( i<nChunk );
  }
}

static void fts5ChunkIterate(
  Fts5Index *p,                   /* Index object */
  Fts5SegIter *pSeg,              /* Poslist of this iterator */
  void *pCtx,                     /* Context pointer for xChunk callback */
  void (*xChunk)(Fts5Index*, void*, const u8*, int)
){
  int nRem = pSeg->nPos;          /* Number of bytes still to come */
  Fts5Data *pData = 0;
  u8 *pChunk = &pSeg->pLeaf->p[pSeg->iLeafOffset];
  int nChunk = MIN(nRem, pSeg->pLeaf->szLeaf - pSeg->iLeafOffset);
  int pgno = pSeg->iLeafPgno;
  int pgnoSave = 0;

  /* This function does not work with detail=none databases. */
  assert( p->pConfig->eDetail!=FTS5_DETAIL_NONE );

  if( (pSeg->flags & FTS5_SEGITER_REVERSE)==0 ){
    pgnoSave = pgno+1;
  }

  while( 1 ){
    xChunk(p, pCtx, pChunk, nChunk);
    nRem -= nChunk;
    fts5DataRelease(pData);
    if( nRem<=0 ){
      break;
    }else if( pSeg->pSeg==0 ){
      p->rc = FTS5_CORRUPT;
      return;
    }else{
      pgno++;
      pData = fts5LeafRead(p, FTS5_SEGMENT_ROWID(pSeg->pSeg->iSegid, pgno));
      if( pData==0 ) break;
      pChunk = &pData->p[4];
      nChunk = MIN(nRem, pData->szLeaf - 4);
      if( pgno==pgnoSave ){
        assert( pSeg->pNextLeaf==0 );
        pSeg->pNextLeaf = pData;
        pData = 0;
      }
    }
  }
}

/*
** Iterator pIter currently points to a valid entry (not EOF). This
** function appends the position list data for the current entry to
** buffer pBuf. It does not make a copy of the position-list size
** field.
*/
static void fts5SegiterPoslist(
  Fts5Index *p,
  Fts5SegIter *pSeg,
  Fts5Colset *pColset,
  Fts5Buffer *pBuf
){
  assert( pBuf!=0 );
  assert( pSeg!=0 );
  if( 0==fts5BufferGrow(&p->rc, pBuf, pSeg->nPos+FTS5_DATA_ZERO_PADDING) ){
    assert( pBuf->p!=0 );
    assert( pBuf->nSpace >= pBuf->n+pSeg->nPos+FTS5_DATA_ZERO_PADDING );
    memset(&pBuf->p[pBuf->n+pSeg->nPos], 0, FTS5_DATA_ZERO_PADDING);
    if( pColset==0 ){
      fts5ChunkIterate(p, pSeg, (void*)pBuf, fts5PoslistCallback);
    }else{
      if( p->pConfig->eDetail==FTS5_DETAIL_FULL ){
        PoslistCallbackCtx sCtx;
        sCtx.pBuf = pBuf;
        sCtx.pColset = pColset;
        sCtx.eState = fts5IndexColsetTest(pColset, 0);
        assert( sCtx.eState==0 || sCtx.eState==1 );
        fts5ChunkIterate(p, pSeg, (void*)&sCtx, fts5PoslistFilterCallback);
      }else{
        PoslistOffsetsCtx sCtx;
        memset(&sCtx, 0, sizeof(sCtx));
        sCtx.pBuf = pBuf;
        sCtx.pColset = pColset;
        fts5ChunkIterate(p, pSeg, (void*)&sCtx, fts5PoslistOffsetsCallback);
      }
    }
  }
}

/*
** Parameter pPos points to a buffer containing a position list, size nPos.
** This function filters it according to pColset (which must be non-NULL)
** and sets pIter->base.pData/nData to point to the new position list.
** If memory is required for the new position list, use buffer pIter->poslist.
** Or, if the new position list is a contiguous subset of the input, set
** pIter->base.pData/nData to point directly to it.
**
** This function is a no-op if *pRc is other than SQLITE_OK when it is
** called. If an OOM error is encountered, *pRc is set to SQLITE_NOMEM
** before returning.
*/
static void fts5IndexExtractColset(
  int *pRc,
  Fts5Colset *pColset,            /* Colset to filter on */
  const u8 *pPos, int nPos,       /* Position list */
  Fts5Iter *pIter
){
  if( *pRc==SQLITE_OK ){
    const u8 *p = pPos;
    const u8 *aCopy = p;
    const u8 *pEnd = &p[nPos];    /* One byte past end of position list */
    int i = 0;
    int iCurrent = 0;

    if( pColset->nCol>1 && sqlite3Fts5BufferSize(pRc, &pIter->poslist, nPos) ){
      return;
    }

    while( 1 ){
      while( pColset->aiCol[i]<iCurrent ){
        i++;
        if( i==pColset->nCol ){
          pIter->base.pData = pIter->poslist.p;
          pIter->base.nData = pIter->poslist.n;
          return;
        }
      }

      /* Advance pointer p until it points to pEnd or an 0x01 byte that is
      ** not part of a varint */
      while( p<pEnd && *p!=0x01 ){
        while( *p++ & 0x80 );
      }

      if( pColset->aiCol[i]==iCurrent ){
        if( pColset->nCol==1 ){
          pIter->base.pData = aCopy;
          pIter->base.nData = p-aCopy;
          return;
        }
        fts5BufferSafeAppendBlob(&pIter->poslist, aCopy, p-aCopy);
      }
      if( p>=pEnd ){
        pIter->base.pData = pIter->poslist.p;
        pIter->base.nData = pIter->poslist.n;
        return;
      }
      aCopy = p++;
      iCurrent = *p++;
      if( iCurrent & 0x80 ){
        p--;
        p += fts5GetVarint32(p, iCurrent);
      }
    }
  }

}

/*
** xSetOutputs callback used by detail=none tables.
*/
static void fts5IterSetOutputs_None(Fts5Iter *pIter, Fts5SegIter *pSeg){
  assert( pIter->pIndex->pConfig->eDetail==FTS5_DETAIL_NONE );
  pIter->base.iRowid = pSeg->iRowid;
  pIter->base.nData = pSeg->nPos;
}

/*
** xSetOutputs callback used by detail=full and detail=col tables when no
** column filters are specified.
*/
static void fts5IterSetOutputs_Nocolset(Fts5Iter *pIter, Fts5SegIter *pSeg){
  pIter->base.iRowid = pSeg->iRowid;
  pIter->base.nData = pSeg->nPos;

  assert( pIter->pIndex->pConfig->eDetail!=FTS5_DETAIL_NONE );
  assert( pIter->pColset==0 );

  if( pSeg->iLeafOffset+pSeg->nPos<=pSeg->pLeaf->szLeaf ){
    /* All data is stored on the current page. Populate the output 
    ** variables to point into the body of the page object. */
    pIter->base.pData = &pSeg->pLeaf->p[pSeg->iLeafOffset];
  }else{
    /* The data is distributed over two or more pages. Copy it into the
    ** Fts5Iter.poslist buffer and then set the output pointer to point
    ** to this buffer.  */
    fts5BufferZero(&pIter->poslist);
    fts5SegiterPoslist(pIter->pIndex, pSeg, 0, &pIter->poslist);
    pIter->base.pData = pIter->poslist.p;
  }
}

/*
** xSetOutputs callback used when the Fts5Colset object has nCol==0 (match
** against no columns at all).
*/
static void fts5IterSetOutputs_ZeroColset(Fts5Iter *pIter, Fts5SegIter *pSeg){
  UNUSED_PARAM(pSeg);
  pIter->base.nData = 0;
}

/*
** xSetOutputs callback used by detail=col when there is a column filter
** and there are 100 or more columns. Also called as a fallback from
** fts5IterSetOutputs_Col100 if the column-list spans more than one page.
*/
static void fts5IterSetOutputs_Col(Fts5Iter *pIter, Fts5SegIter *pSeg){
  fts5BufferZero(&pIter->poslist);
  fts5SegiterPoslist(pIter->pIndex, pSeg, pIter->pColset, &pIter->poslist);
  pIter->base.iRowid = pSeg->iRowid;
  pIter->base.pData = pIter->poslist.p;
  pIter->base.nData = pIter->poslist.n;
}

/*
** xSetOutputs callback used when: 
**
**   * detail=col,
**   * there is a column filter, and
**   * the table contains 100 or fewer columns. 
**
** The last point is to ensure all column numbers are stored as 
** single-byte varints.
*/
static void fts5IterSetOutputs_Col100(Fts5Iter *pIter, Fts5SegIter *pSeg){

  assert( pIter->pIndex->pConfig->eDetail==FTS5_DETAIL_COLUMNS );
  assert( pIter->pColset );

  if( pSeg->iLeafOffset+pSeg->nPos>pSeg->pLeaf->szLeaf ){
    fts5IterSetOutputs_Col(pIter, pSeg);
  }else{
    u8 *a = (u8*)&pSeg->pLeaf->p[pSeg->iLeafOffset];
    u8 *pEnd = (u8*)&a[pSeg->nPos]; 
    int iPrev = 0;
    int *aiCol = pIter->pColset->aiCol;
    int *aiColEnd = &aiCol[pIter->pColset->nCol];

    u8 *aOut = pIter->poslist.p;
    int iPrevOut = 0;

    pIter->base.iRowid = pSeg->iRowid;

    while( a<pEnd ){
      iPrev += (int)a++[0] - 2;
      while( *aiCol<iPrev ){
        aiCol++;
        if( aiCol==aiColEnd ) goto setoutputs_col_out;
      }
      if( *aiCol==iPrev ){
        *aOut++ = (u8)((iPrev - iPrevOut) + 2);
        iPrevOut = iPrev;
      }
    }

setoutputs_col_out:
    pIter->base.pData = pIter->poslist.p;
    pIter->base.nData = aOut - pIter->poslist.p;
  }
}

/*
** xSetOutputs callback used by detail=full when there is a column filter.
*/
static void fts5IterSetOutputs_Full(Fts5Iter *pIter, Fts5SegIter *pSeg){
  Fts5Colset *pColset = pIter->pColset;
  pIter->base.iRowid = pSeg->iRowid;

  assert( pIter->pIndex->pConfig->eDetail==FTS5_DETAIL_FULL );
  assert( pColset );

  if( pSeg->iLeafOffset+pSeg->nPos<=pSeg->pLeaf->szLeaf ){
    /* All data is stored on the current page. Populate the output 
    ** variables to point into the body of the page object. */
    const u8 *a = &pSeg->pLeaf->p[pSeg->iLeafOffset];
    int *pRc = &pIter->pIndex->rc;
    fts5BufferZero(&pIter->poslist);
    fts5IndexExtractColset(pRc, pColset, a, pSeg->nPos, pIter);
  }else{
    /* The data is distributed over two or more pages. Copy it into the
    ** Fts5Iter.poslist buffer and then set the output pointer to point
    ** to this buffer.  */
    fts5BufferZero(&pIter->poslist);
    fts5SegiterPoslist(pIter->pIndex, pSeg, pColset, &pIter->poslist);
    pIter->base.pData = pIter->poslist.p;
    pIter->base.nData = pIter->poslist.n;
  }
}

static void fts5IterSetOutputCb(int *pRc, Fts5Iter *pIter){
  assert( pIter!=0 || (*pRc)!=SQLITE_OK );
  if( *pRc==SQLITE_OK ){
    Fts5Config *pConfig = pIter->pIndex->pConfig;
    if( pConfig->eDetail==FTS5_DETAIL_NONE ){
      pIter->xSetOutputs = fts5IterSetOutputs_None;
    }

    else if( pIter->pColset==0 ){
      pIter->xSetOutputs = fts5IterSetOutputs_Nocolset;
    }

    else if( pIter->pColset->nCol==0 ){
      pIter->xSetOutputs = fts5IterSetOutputs_ZeroColset;
    }

    else if( pConfig->eDetail==FTS5_DETAIL_FULL ){
      pIter->xSetOutputs = fts5IterSetOutputs_Full;
    }

    else{
      assert( pConfig->eDetail==FTS5_DETAIL_COLUMNS );
      if( pConfig->nCol<=100 ){
        pIter->xSetOutputs = fts5IterSetOutputs_Col100;
        sqlite3Fts5BufferSize(pRc, &pIter->poslist, pConfig->nCol);
      }else{
        pIter->xSetOutputs = fts5IterSetOutputs_Col;
      }
    }
  }
}


/*
** Allocate a new Fts5Iter object.
**
** The new object will be used to iterate through data in structure pStruct.
** If iLevel is -ve, then all data in all segments is merged. Or, if iLevel
** is zero or greater, data from the first nSegment segments on level iLevel
** is merged.
**
** The iterator initially points to the first term/rowid entry in the 
** iterated data.
*/
static void fts5MultiIterNew(
  Fts5Index *p,                   /* FTS5 backend to iterate within */
  Fts5Structure *pStruct,         /* Structure of specific index */
  int flags,                      /* FTS5INDEX_QUERY_XXX flags */
  Fts5Colset *pColset,            /* Colset to filter on (or NULL) */
  const u8 *pTerm, int nTerm,     /* Term to seek to (or NULL/0) */
  int iLevel,                     /* Level to iterate (-1 for all) */
  int nSegment,                   /* Number of segments to merge (iLevel>=0) */
  Fts5Iter **ppOut                /* New object */
){
  int nSeg = 0;                   /* Number of segment-iters in use */
  int iIter = 0;                  /* */
  int iSeg;                       /* Used to iterate through segments */
  Fts5StructureLevel *pLvl;
  Fts5Iter *pNew;

  assert( (pTerm==0 && nTerm==0) || iLevel<0 );

  /* Allocate space for the new multi-seg-iterator. */
  if( p->rc==SQLITE_OK ){
    if( iLevel<0 ){
      assert( pStruct->nSegment==fts5StructureCountSegments(pStruct) );
      nSeg = pStruct->nSegment;
      nSeg += (p->pHash ? 1 : 0);
    }else{
      nSeg = MIN(pStruct->aLevel[iLevel].nSeg, nSegment);
    }
  }
  *ppOut = pNew = fts5MultiIterAlloc(p, nSeg);
  if( pNew==0 ){
    assert( p->rc!=SQLITE_OK );
    goto fts5MultiIterNew_post_check;
  }
  pNew->bRev = (0!=(flags & FTS5INDEX_QUERY_DESC));
  pNew->bSkipEmpty = (0!=(flags & FTS5INDEX_QUERY_SKIPEMPTY));
  pNew->pColset = pColset;
  if( (flags & FTS5INDEX_QUERY_NOOUTPUT)==0 ){
    fts5IterSetOutputCb(&p->rc, pNew);
  }

  /* Initialize each of the component segment iterators. */
  if( p->rc==SQLITE_OK ){
    if( iLevel<0 ){
      Fts5StructureLevel *pEnd = &pStruct->aLevel[pStruct->nLevel];
      if( p->pHash ){
        /* Add a segment iterator for the current contents of the hash table. */
        Fts5SegIter *pIter = &pNew->aSeg[iIter++];
        fts5SegIterHashInit(p, pTerm, nTerm, flags, pIter);
      }
      for(pLvl=&pStruct->aLevel[0]; pLvl<pEnd; pLvl++){
        for(iSeg=pLvl->nSeg-1; iSeg>=0; iSeg--){
          Fts5StructureSegment *pSeg = &pLvl->aSeg[iSeg];
          Fts5SegIter *pIter = &pNew->aSeg[iIter++];
          if( pTerm==0 ){
            fts5SegIterInit(p, pSeg, pIter);
          }else{
            fts5SegIterSeekInit(p, pTerm, nTerm, flags, pSeg, pIter);
          }
        }
      }
    }else{
      pLvl = &pStruct->aLevel[iLevel];
      for(iSeg=nSeg-1; iSeg>=0; iSeg--){
        fts5SegIterInit(p, &pLvl->aSeg[iSeg], &pNew->aSeg[iIter++]);
      }
    }
    assert( iIter==nSeg );
  }

  /* If the above was successful, each component iterators now points 
  ** to the first entry in its segment. In this case initialize the 
  ** aFirst[] array. Or, if an error has occurred, free the iterator
  ** object and set the output variable to NULL.  */
  if( p->rc==SQLITE_OK ){
    for(iIter=pNew->nSeg-1; iIter>0; iIter--){
      int iEq;
      if( (iEq = fts5MultiIterDoCompare(pNew, iIter)) ){
        Fts5SegIter *pSeg = &pNew->aSeg[iEq];
        if( p->rc==SQLITE_OK ) pSeg->xNext(p, pSeg, 0);
        fts5MultiIterAdvanced(p, pNew, iEq, iIter);
      }
    }
    fts5MultiIterSetEof(pNew);
    fts5AssertMultiIterSetup(p, pNew);

    if( pNew->bSkipEmpty && fts5MultiIterIsEmpty(p, pNew) ){
      fts5MultiIterNext(p, pNew, 0, 0);
    }else if( pNew->base.bEof==0 ){
      Fts5SegIter *pSeg = &pNew->aSeg[pNew->aFirst[1].iFirst];
      pNew->xSetOutputs(pNew, pSeg);
    }

  }else{
    fts5MultiIterFree(pNew);
    *ppOut = 0;
  }

fts5MultiIterNew_post_check:
  assert( (*ppOut)!=0 || p->rc!=SQLITE_OK );
  return;
}

/*
** Create an Fts5Iter that iterates through the doclist provided
** as the second argument.
*/
static void fts5MultiIterNew2(
  Fts5Index *p,                   /* FTS5 backend to iterate within */
  Fts5Data *pData,                /* Doclist to iterate through */
  int bDesc,                      /* True for descending rowid order */
  Fts5Iter **ppOut                /* New object */
){
  Fts5Iter *pNew;
  pNew = fts5MultiIterAlloc(p, 2);
  if( pNew ){
    Fts5SegIter *pIter = &pNew->aSeg[1];

    pIter->flags = FTS5_SEGITER_ONETERM;
    if( pData->szLeaf>0 ){
      pIter->pLeaf = pData;
      pIter->iLeafOffset = fts5GetVarint(pData->p, (u64*)&pIter->iRowid);
      pIter->iEndofDoclist = pData->nn;
      pNew->aFirst[1].iFirst = 1;
      if( bDesc ){
        pNew->bRev = 1;
        pIter->flags |= FTS5_SEGITER_REVERSE;
        fts5SegIterReverseInitPage(p, pIter);
      }else{
        fts5SegIterLoadNPos(p, pIter);
      }
      pData = 0;
    }else{
      pNew->base.bEof = 1;
    }
    fts5SegIterSetNext(p, pIter);

    *ppOut = pNew;
  }

  fts5DataRelease(pData);
}

/*
** Return true if the iterator is at EOF or if an error has occurred. 
** False otherwise.
*/
static int fts5MultiIterEof(Fts5Index *p, Fts5Iter *pIter){
  assert( pIter!=0 || p->rc!=SQLITE_OK );
  assert( p->rc!=SQLITE_OK
      || (pIter->aSeg[ pIter->aFirst[1].iFirst ].pLeaf==0)==pIter->base.bEof 
  );
  return (p->rc || pIter->base.bEof);
}

/*
** Return the rowid of the entry that the iterator currently points
** to. If the iterator points to EOF when this function is called the
** results are undefined.
*/
static i64 fts5MultiIterRowid(Fts5Iter *pIter){
  assert( pIter->aSeg[ pIter->aFirst[1].iFirst ].pLeaf );
  return pIter->aSeg[ pIter->aFirst[1].iFirst ].iRowid;
}

/*
** Move the iterator to the next entry at or following iMatch.
*/
static void fts5MultiIterNextFrom(
  Fts5Index *p, 
  Fts5Iter *pIter, 
  i64 iMatch
){
  while( 1 ){
    i64 iRowid;
    fts5MultiIterNext(p, pIter, 1, iMatch);
    if( fts5MultiIterEof(p, pIter) ) break;
    iRowid = fts5MultiIterRowid(pIter);
    if( pIter->bRev==0 && iRowid>=iMatch ) break;
    if( pIter->bRev!=0 && iRowid<=iMatch ) break;
  }
}

/*
** Return a pointer to a buffer containing the term associated with the 
** entry that the iterator currently points to.
*/
static const u8 *fts5MultiIterTerm(Fts5Iter *pIter, int *pn){
  Fts5SegIter *p = &pIter->aSeg[ pIter->aFirst[1].iFirst ];
  *pn = p->term.n;
  return p->term.p;
}

/*
** Allocate a new segment-id for the structure pStruct. The new segment
** id must be between 1 and 65335 inclusive, and must not be used by 
** any currently existing segment. If a free segment id cannot be found,
** SQLITE_FULL is returned.
**
** If an error has already occurred, this function is a no-op. 0 is 
** returned in this case.
*/
static int fts5AllocateSegid(Fts5Index *p, Fts5Structure *pStruct){
  int iSegid = 0;

  if( p->rc==SQLITE_OK ){
    if( pStruct->nSegment>=FTS5_MAX_SEGMENT ){
      p->rc = SQLITE_FULL;
    }else{
      /* FTS5_MAX_SEGMENT is currently defined as 2000. So the following
      ** array is 63 elements, or 252 bytes, in size.  */
      u32 aUsed[(FTS5_MAX_SEGMENT+31) / 32];
      int iLvl, iSeg;
      int i;
      u32 mask;
      memset(aUsed, 0, sizeof(aUsed));
      for(iLvl=0; iLvl<pStruct->nLevel; iLvl++){
        for(iSeg=0; iSeg<pStruct->aLevel[iLvl].nSeg; iSeg++){
          int iId = pStruct->aLevel[iLvl].aSeg[iSeg].iSegid;
          if( iId<=FTS5_MAX_SEGMENT && iId>0 ){
            aUsed[(iId-1) / 32] |= (u32)1 << ((iId-1) % 32);
          }
        }
      }

      for(i=0; aUsed[i]==0xFFFFFFFF; i++);
      mask = aUsed[i];
      for(iSegid=0; mask & ((u32)1 << iSegid); iSegid++);
      iSegid += 1 + i*32;

#ifdef SQLITE_DEBUG
      for(iLvl=0; iLvl<pStruct->nLevel; iLvl++){
        for(iSeg=0; iSeg<pStruct->aLevel[iLvl].nSeg; iSeg++){
          assert_nc( iSegid!=pStruct->aLevel[iLvl].aSeg[iSeg].iSegid );
        }
      }
      assert_nc( iSegid>0 && iSegid<=FTS5_MAX_SEGMENT );

      {
        sqlite3_stmt *pIdxSelect = fts5IdxSelectStmt(p);
        if( p->rc==SQLITE_OK ){
          u8 aBlob[2] = {0xff, 0xff};
          sqlite3_bind_int(pIdxSelect, 1, iSegid);
          sqlite3_bind_blob(pIdxSelect, 2, aBlob, 2, SQLITE_STATIC);
          assert_nc( sqlite3_step(pIdxSelect)!=SQLITE_ROW );
          p->rc = sqlite3_reset(pIdxSelect);
          sqlite3_bind_null(pIdxSelect, 2);
        }
      }
#endif
    }
  }

  return iSegid;
}

/*
** Discard all data currently cached in the hash-tables.
*/
static void fts5IndexDiscardData(Fts5Index *p){
  assert( p->pHash || p->nPendingData==0 );
  if( p->pHash ){
    sqlite3Fts5HashClear(p->pHash);
    p->nPendingData = 0;
  }
}

/*
** Return the size of the prefix, in bytes, that buffer 
** (pNew/<length-unknown>) shares with buffer (pOld/nOld).
**
** Buffer (pNew/<length-unknown>) is guaranteed to be greater 
** than buffer (pOld/nOld).
*/
static int fts5PrefixCompress(int nOld, const u8 *pOld, const u8 *pNew){
  int i;
  for(i=0; i<nOld; i++){
    if( pOld[i]!=pNew[i] ) break;
  }
  return i;
}

static void fts5WriteDlidxClear(
  Fts5Index *p, 
  Fts5SegWriter *pWriter,
  int bFlush                      /* If true, write dlidx to disk */
){
  int i;
  assert( bFlush==0 || (pWriter->nDlidx>0 && pWriter->aDlidx[0].buf.n>0) );
  for(i=0; i<pWriter->nDlidx; i++){
    Fts5DlidxWriter *pDlidx = &pWriter->aDlidx[i];
    if( pDlidx->buf.n==0 ) break;
    if( bFlush ){
      assert( pDlidx->pgno!=0 );
      fts5DataWrite(p, 
          FTS5_DLIDX_ROWID(pWriter->iSegid, i, pDlidx->pgno),
          pDlidx->buf.p, pDlidx->buf.n
      );
    }
    sqlite3Fts5BufferZero(&pDlidx->buf);
    pDlidx->bPrevValid = 0;
  }
}

/*
** Grow the pWriter->aDlidx[] array to at least nLvl elements in size.
** Any new array elements are zeroed before returning.
*/
static int fts5WriteDlidxGrow(
  Fts5Index *p,
  Fts5SegWriter *pWriter,
  int nLvl
){
  if( p->rc==SQLITE_OK && nLvl>=pWriter->nDlidx ){
    Fts5DlidxWriter *aDlidx = (Fts5DlidxWriter*)sqlite3_realloc64(
        pWriter->aDlidx, sizeof(Fts5DlidxWriter) * nLvl
    );
    if( aDlidx==0 ){
      p->rc = SQLITE_NOMEM;
    }else{
      size_t nByte = sizeof(Fts5DlidxWriter) * (nLvl - pWriter->nDlidx);
      memset(&aDlidx[pWriter->nDlidx], 0, nByte);
      pWriter->aDlidx = aDlidx;
      pWriter->nDlidx = nLvl;
    }
  }
  return p->rc;
}

/*
** If the current doclist-index accumulating in pWriter->aDlidx[] is large
** enough, flush it to disk and return 1. Otherwise discard it and return
** zero.
*/
static int fts5WriteFlushDlidx(Fts5Index *p, Fts5SegWriter *pWriter){
  int bFlag = 0;

  /* If there were FTS5_MIN_DLIDX_SIZE or more empty leaf pages written
  ** to the database, also write the doclist-index to disk.  */
  if( pWriter->aDlidx[0].buf.n>0 && pWriter->nEmpty>=FTS5_MIN_DLIDX_SIZE ){
    bFlag = 1;
  }
  fts5WriteDlidxClear(p, pWriter, bFlag);
  pWriter->nEmpty = 0;
  return bFlag;
}

/*
** This function is called whenever processing of the doclist for the 
** last term on leaf page (pWriter->iBtPage) is completed. 
**
** The doclist-index for that term is currently stored in-memory within the
** Fts5SegWriter.aDlidx[] array. If it is large enough, this function
** writes it out to disk. Or, if it is too small to bother with, discards
** it.
**
** Fts5SegWriter.btterm currently contains the first term on page iBtPage.
*/
static void fts5WriteFlushBtree(Fts5Index *p, Fts5SegWriter *pWriter){
  int bFlag;

  assert( pWriter->iBtPage || pWriter->nEmpty==0 );
  if( pWriter->iBtPage==0 ) return;
  bFlag = fts5WriteFlushDlidx(p, pWriter);

  if( p->rc==SQLITE_OK ){
    const char *z = (pWriter->btterm.n>0?(const char*)pWriter->btterm.p:"");
    /* The following was already done in fts5WriteInit(): */
    /* sqlite3_bind_int(p->pIdxWriter, 1, pWriter->iSegid); */
    sqlite3_bind_blob(p->pIdxWriter, 2, z, pWriter->btterm.n, SQLITE_STATIC);
    sqlite3_bind_int64(p->pIdxWriter, 3, bFlag + ((i64)pWriter->iBtPage<<1));
    sqlite3_step(p->pIdxWriter);
    p->rc = sqlite3_reset(p->pIdxWriter);
    sqlite3_bind_null(p->pIdxWriter, 2);
  }
  pWriter->iBtPage = 0;
}

/*
** This is called once for each leaf page except the first that contains
** at least one term. Argument (nTerm/pTerm) is the split-key - a term that
** is larger than all terms written to earlier leaves, and equal to or
** smaller than the first term on the new leaf.
**
** If an error occurs, an error code is left in Fts5Index.rc. If an error
** has already occurred when this function is called, it is a no-op.
*/
static void fts5WriteBtreeTerm(
  Fts5Index *p,                   /* FTS5 backend object */
  Fts5SegWriter *pWriter,         /* Writer object */
  int nTerm, const u8 *pTerm      /* First term on new page */
){
  fts5WriteFlushBtree(p, pWriter);
  if( p->rc==SQLITE_OK ){
    fts5BufferSet(&p->rc, &pWriter->btterm, nTerm, pTerm);
    pWriter->iBtPage = pWriter->writer.pgno;
  }
}

/*
** This function is called when flushing a leaf page that contains no
** terms at all to disk.
*/
static void fts5WriteBtreeNoTerm(
  Fts5Index *p,                   /* FTS5 backend object */
  Fts5SegWriter *pWriter          /* Writer object */
){
  /* If there were no rowids on the leaf page either and the doclist-index
  ** has already been started, append an 0x00 byte to it.  */
  if( pWriter->bFirstRowidInPage && pWriter->aDlidx[0].buf.n>0 ){
    Fts5DlidxWriter *pDlidx = &pWriter->aDlidx[0];
    assert( pDlidx->bPrevValid );
    sqlite3Fts5BufferAppendVarint(&p->rc, &pDlidx->buf, 0);
  }

  /* Increment the "number of sequential leaves without a term" counter. */
  pWriter->nEmpty++;
}

static i64 fts5DlidxExtractFirstRowid(Fts5Buffer *pBuf){
  i64 iRowid;
  int iOff;

  iOff = 1 + fts5GetVarint(&pBuf->p[1], (u64*)&iRowid);
  fts5GetVarint(&pBuf->p[iOff], (u64*)&iRowid);
  return iRowid;
}

/*
** Rowid iRowid has just been appended to the current leaf page. It is the
** first on the page. This function appends an appropriate entry to the current
** doclist-index.
*/
static void fts5WriteDlidxAppend(
  Fts5Index *p, 
  Fts5SegWriter *pWriter, 
  i64 iRowid
){
  int i;
  int bDone = 0;

  for(i=0; p->rc==SQLITE_OK && bDone==0; i++){
    i64 iVal;
    Fts5DlidxWriter *pDlidx = &pWriter->aDlidx[i];

    if( pDlidx->buf.n>=p->pConfig->pgsz ){
      /* The current doclist-index page is full. Write it to disk and push
      ** a copy of iRowid (which will become the first rowid on the next
      ** doclist-index leaf page) up into the next level of the b-tree 
      ** hierarchy. If the node being flushed is currently the root node,
      ** also push its first rowid upwards. */
      pDlidx->buf.p[0] = 0x01;    /* Not the root node */
      fts5DataWrite(p, 
          FTS5_DLIDX_ROWID(pWriter->iSegid, i, pDlidx->pgno),
          pDlidx->buf.p, pDlidx->buf.n
      );
      fts5WriteDlidxGrow(p, pWriter, i+2);
      pDlidx = &pWriter->aDlidx[i];
      if( p->rc==SQLITE_OK && pDlidx[1].buf.n==0 ){
        i64 iFirst = fts5DlidxExtractFirstRowid(&pDlidx->buf);

        /* This was the root node. Push its first rowid up to the new root. */
        pDlidx[1].pgno = pDlidx->pgno;
        sqlite3Fts5BufferAppendVarint(&p->rc, &pDlidx[1].buf, 0);
        sqlite3Fts5BufferAppendVarint(&p->rc, &pDlidx[1].buf, pDlidx->pgno);
        sqlite3Fts5BufferAppendVarint(&p->rc, &pDlidx[1].buf, iFirst);
        pDlidx[1].bPrevValid = 1;
        pDlidx[1].iPrev = iFirst;
      }

      sqlite3Fts5BufferZero(&pDlidx->buf);
      pDlidx->bPrevValid = 0;
      pDlidx->pgno++;
    }else{
      bDone = 1;
    }

    if( pDlidx->bPrevValid ){
      iVal = iRowid - pDlidx->iPrev;
    }else{
      i64 iPgno = (i==0 ? pWriter->writer.pgno : pDlidx[-1].pgno);
      assert( pDlidx->buf.n==0 );
      sqlite3Fts5BufferAppendVarint(&p->rc, &pDlidx->buf, !bDone);
      sqlite3Fts5BufferAppendVarint(&p->rc, &pDlidx->buf, iPgno);
      iVal = iRowid;
    }

    sqlite3Fts5BufferAppendVarint(&p->rc, &pDlidx->buf, iVal);
    pDlidx->bPrevValid = 1;
    pDlidx->iPrev = iRowid;
  }
}

static void fts5WriteFlushLeaf(Fts5Index *p, Fts5SegWriter *pWriter){
  static const u8 zero[] = { 0x00, 0x00, 0x00, 0x00 };
  Fts5PageWriter *pPage = &pWriter->writer;
  i64 iRowid;

  assert( (pPage->pgidx.n==0)==(pWriter->bFirstTermInPage) );

  /* Set the szLeaf header field. */
  assert( 0==fts5GetU16(&pPage->buf.p[2]) );
  fts5PutU16(&pPage->buf.p[2], (u16)pPage->buf.n);

  if( pWriter->bFirstTermInPage ){
    /* No term was written to this page. */
    assert( pPage->pgidx.n==0 );
    fts5WriteBtreeNoTerm(p, pWriter);
  }else{
    /* Append the pgidx to the page buffer. Set the szLeaf header field. */
    fts5BufferAppendBlob(&p->rc, &pPage->buf, pPage->pgidx.n, pPage->pgidx.p);
  }

  /* Write the page out to disk */
  iRowid = FTS5_SEGMENT_ROWID(pWriter->iSegid, pPage->pgno);
  fts5DataWrite(p, iRowid, pPage->buf.p, pPage->buf.n);

  /* Initialize the next page. */
  fts5BufferZero(&pPage->buf);
  fts5BufferZero(&pPage->pgidx);
  fts5BufferAppendBlob(&p->rc, &pPage->buf, 4, zero);
  pPage->iPrevPgidx = 0;
  pPage->pgno++;

  /* Increase the leaves written counter */
  pWriter->nLeafWritten++;

  /* The new leaf holds no terms or rowids */
  pWriter->bFirstTermInPage = 1;
  pWriter->bFirstRowidInPage = 1;
}

/*
** Append term pTerm/nTerm to the segment being written by the writer passed
** as the second argument.
**
** If an error occurs, set the Fts5Index.rc error code. If an error has 
** already occurred, this function is a no-op.
*/
static void fts5WriteAppendTerm(
  Fts5Index *p, 
  Fts5SegWriter *pWriter,
  int nTerm, const u8 *pTerm 
){
  int nPrefix;                    /* Bytes of prefix compression for term */
  Fts5PageWriter *pPage = &pWriter->writer;
  Fts5Buffer *pPgidx = &pWriter->writer.pgidx;
  int nMin = MIN(pPage->term.n, nTerm);

  assert( p->rc==SQLITE_OK );
  assert( pPage->buf.n>=4 );
  assert( pPage->buf.n>4 || pWriter->bFirstTermInPage );

  /* If the current leaf page is full, flush it to disk. */
  if( (pPage->buf.n + pPgidx->n + nTerm + 2)>=p->pConfig->pgsz ){
    if( pPage->buf.n>4 ){
      fts5WriteFlushLeaf(p, pWriter);
      if( p->rc!=SQLITE_OK ) return;
    }
    fts5BufferGrow(&p->rc, &pPage->buf, nTerm+FTS5_DATA_PADDING);
  }
  
  /* TODO1: Updating pgidx here. */
  pPgidx->n += sqlite3Fts5PutVarint(
      &pPgidx->p[pPgidx->n], pPage->buf.n - pPage->iPrevPgidx
  );
  pPage->iPrevPgidx = pPage->buf.n;
#if 0
  fts5PutU16(&pPgidx->p[pPgidx->n], pPage->buf.n);
  pPgidx->n += 2;
#endif

  if( pWriter->bFirstTermInPage ){
    nPrefix = 0;
    if( pPage->pgno!=1 ){
      /* This is the first term on a leaf that is not the leftmost leaf in
      ** the segment b-tree. In this case it is necessary to add a term to
      ** the b-tree hierarchy that is (a) larger than the largest term 
      ** already written to the segment and (b) smaller than or equal to
      ** this term. In other words, a prefix of (pTerm/nTerm) that is one
      ** byte longer than the longest prefix (pTerm/nTerm) shares with the
      ** previous term. 
      **
      ** Usually, the previous term is available in pPage->term. The exception
      ** is if this is the first term written in an incremental-merge step.
      ** In this case the previous term is not available, so just write a
      ** copy of (pTerm/nTerm) into the parent node. This is slightly
      ** inefficient, but still correct.  */
      int n = nTerm;
      if( pPage->term.n ){
        n = 1 + fts5PrefixCompress(nMin, pPage->term.p, pTerm);
      }
      fts5WriteBtreeTerm(p, pWriter, n, pTerm);
      if( p->rc!=SQLITE_OK ) return;
      pPage = &pWriter->writer;
    }
  }else{
    nPrefix = fts5PrefixCompress(nMin, pPage->term.p, pTerm);
    fts5BufferAppendVarint(&p->rc, &pPage->buf, nPrefix);
  }

  /* Append the number of bytes of new data, then the term data itself
  ** to the page. */
  fts5BufferAppendVarint(&p->rc, &pPage->buf, nTerm - nPrefix);
  fts5BufferAppendBlob(&p->rc, &pPage->buf, nTerm - nPrefix, &pTerm[nPrefix]);

  /* Update the Fts5PageWriter.term field. */
  fts5BufferSet(&p->rc, &pPage->term, nTerm, pTerm);
  pWriter->bFirstTermInPage = 0;

  pWriter->bFirstRowidInPage = 0;
  pWriter->bFirstRowidInDoclist = 1;

  assert( p->rc || (pWriter->nDlidx>0 && pWriter->aDlidx[0].buf.n==0) );
  pWriter->aDlidx[0].pgno = pPage->pgno;
}

/*
** Append a rowid and position-list size field to the writers output. 
*/
static void fts5WriteAppendRowid(
  Fts5Index *p, 
  Fts5SegWriter *pWriter,
  i64 iRowid
){
  if( p->rc==SQLITE_OK ){
    Fts5PageWriter *pPage = &pWriter->writer;

    if( (pPage->buf.n + pPage->pgidx.n)>=p->pConfig->pgsz ){
      fts5WriteFlushLeaf(p, pWriter);
    }

    /* If this is to be the first rowid written to the page, set the 
    ** rowid-pointer in the page-header. Also append a value to the dlidx
    ** buffer, in case a doclist-index is required.  */
    if( pWriter->bFirstRowidInPage ){
      fts5PutU16(pPage->buf.p, (u16)pPage->buf.n);
      fts5WriteDlidxAppend(p, pWriter, iRowid);
    }

    /* Write the rowid. */
    if( pWriter->bFirstRowidInDoclist || pWriter->bFirstRowidInPage ){
      fts5BufferAppendVarint(&p->rc, &pPage->buf, iRowid);
    }else{
      assert_nc( p->rc || iRowid>pWriter->iPrevRowid );
      fts5BufferAppendVarint(&p->rc, &pPage->buf, 
          (u64)iRowid - (u64)pWriter->iPrevRowid
      );
    }
    pWriter->iPrevRowid = iRowid;
    pWriter->bFirstRowidInDoclist = 0;
    pWriter->bFirstRowidInPage = 0;
  }
}

static void fts5WriteAppendPoslistData(
  Fts5Index *p, 
  Fts5SegWriter *pWriter, 
  const u8 *aData, 
  int nData
){
  Fts5PageWriter *pPage = &pWriter->writer;
  const u8 *a = aData;
  int n = nData;
  
  assert( p->pConfig->pgsz>0 );
  while( p->rc==SQLITE_OK 
     && (pPage->buf.n + pPage->pgidx.n + n)>=p->pConfig->pgsz 
  ){
    int nReq = p->pConfig->pgsz - pPage->buf.n - pPage->pgidx.n;
    int nCopy = 0;
    while( nCopy<nReq ){
      i64 dummy;
      nCopy += fts5GetVarint(&a[nCopy], (u64*)&dummy);
    }
    fts5BufferAppendBlob(&p->rc, &pPage->buf, nCopy, a);
    a += nCopy;
    n -= nCopy;
    fts5WriteFlushLeaf(p, pWriter);
  }
  if( n>0 ){
    fts5BufferAppendBlob(&p->rc, &pPage->buf, n, a);
  }
}

/*
** Flush any data cached by the writer object to the database. Free any
** allocations associated with the writer.
*/
static void fts5WriteFinish(
  Fts5Index *p, 
  Fts5SegWriter *pWriter,         /* Writer object */
  int *pnLeaf                     /* OUT: Number of leaf pages in b-tree */
){
  int i;
  Fts5PageWriter *pLeaf = &pWriter->writer;
  if( p->rc==SQLITE_OK ){
    assert( pLeaf->pgno>=1 );
    if( pLeaf->buf.n>4 ){
      fts5WriteFlushLeaf(p, pWriter);
    }
    *pnLeaf = pLeaf->pgno-1;
    if( pLeaf->pgno>1 ){
      fts5WriteFlushBtree(p, pWriter);
    }
  }
  fts5BufferFree(&pLeaf->term);
  fts5BufferFree(&pLeaf->buf);
  fts5BufferFree(&pLeaf->pgidx);
  fts5BufferFree(&pWriter->btterm);

  for(i=0; i<pWriter->nDlidx; i++){
    sqlite3Fts5BufferFree(&pWriter->aDlidx[i].buf);
  }
  sqlite3_free(pWriter->aDlidx);
}

static void fts5WriteInit(
  Fts5Index *p, 
  Fts5SegWriter *pWriter, 
  int iSegid
){
  const int nBuffer = p->pConfig->pgsz + FTS5_DATA_PADDING;

  memset(pWriter, 0, sizeof(Fts5SegWriter));
  pWriter->iSegid = iSegid;

  fts5WriteDlidxGrow(p, pWriter, 1);
  pWriter->writer.pgno = 1;
  pWriter->bFirstTermInPage = 1;
  pWriter->iBtPage = 1;

  assert( pWriter->writer.buf.n==0 );
  assert( pWriter->writer.pgidx.n==0 );

  /* Grow the two buffers to pgsz + padding bytes in size. */
  sqlite3Fts5BufferSize(&p->rc, &pWriter->writer.pgidx, nBuffer);
  sqlite3Fts5BufferSize(&p->rc, &pWriter->writer.buf, nBuffer);

  if( p->pIdxWriter==0 ){
    Fts5Config *pConfig = p->pConfig;
    fts5IndexPrepareStmt(p, &p->pIdxWriter, sqlite3_mprintf(
          "INSERT INTO '%q'.'%q_idx'(segid,term,pgno) VALUES(?,?,?)", 
          pConfig->zDb, pConfig->zName
    ));
  }

  if( p->rc==SQLITE_OK ){
    /* Initialize the 4-byte leaf-page header to 0x00. */
    memset(pWriter->writer.buf.p, 0, 4);
    pWriter->writer.buf.n = 4;

    /* Bind the current output segment id to the index-writer. This is an
    ** optimization over binding the same value over and over as rows are
    ** inserted into %_idx by the current writer.  */
    sqlite3_bind_int(p->pIdxWriter, 1, pWriter->iSegid);
  }
}

/*
** Iterator pIter was used to iterate through the input segments of on an
** incremental merge operation. This function is called if the incremental
** merge step has finished but the input has not been completely exhausted.
*/
static void fts5TrimSegments(Fts5Index *p, Fts5Iter *pIter){
  int i;
  Fts5Buffer buf;
  memset(&buf, 0, sizeof(Fts5Buffer));
  for(i=0; i<pIter->nSeg && p->rc==SQLITE_OK; i++){
    Fts5SegIter *pSeg = &pIter->aSeg[i];
    if( pSeg->pSeg==0 ){
      /* no-op */
    }else if( pSeg->pLeaf==0 ){
      /* All keys from this input segment have been transfered to the output.
      ** Set both the first and last page-numbers to 0 to indicate that the
      ** segment is now empty. */
      pSeg->pSeg->pgnoLast = 0;
      pSeg->pSeg->pgnoFirst = 0;
    }else{
      int iOff = pSeg->iTermLeafOffset;     /* Offset on new first leaf page */
      i64 iLeafRowid;
      Fts5Data *pData;
      int iId = pSeg->pSeg->iSegid;
      u8 aHdr[4] = {0x00, 0x00, 0x00, 0x00};

      iLeafRowid = FTS5_SEGMENT_ROWID(iId, pSeg->iTermLeafPgno);
      pData = fts5LeafRead(p, iLeafRowid);
      if( pData ){
        if( iOff>pData->szLeaf ){
          /* This can occur if the pages that the segments occupy overlap - if
          ** a single page has been assigned to more than one segment. In
          ** this case a prior iteration of this loop may have corrupted the
          ** segment currently being trimmed.  */
          p->rc = FTS5_CORRUPT;
        }else{
          fts5BufferZero(&buf);
          fts5BufferGrow(&p->rc, &buf, pData->nn);
          fts5BufferAppendBlob(&p->rc, &buf, sizeof(aHdr), aHdr);
          fts5BufferAppendVarint(&p->rc, &buf, pSeg->term.n);
          fts5BufferAppendBlob(&p->rc, &buf, pSeg->term.n, pSeg->term.p);
          fts5BufferAppendBlob(&p->rc, &buf, pData->szLeaf-iOff,&pData->p[iOff]);
          if( p->rc==SQLITE_OK ){
            /* Set the szLeaf field */
            fts5PutU16(&buf.p[2], (u16)buf.n);
          }

          /* Set up the new page-index array */
          fts5BufferAppendVarint(&p->rc, &buf, 4);
          if( pSeg->iLeafPgno==pSeg->iTermLeafPgno 
           && pSeg->iEndofDoclist<pData->szLeaf
           && pSeg->iPgidxOff<=pData->nn
          ){
            int nDiff = pData->szLeaf - pSeg->iEndofDoclist;
            fts5BufferAppendVarint(&p->rc, &buf, buf.n - 1 - nDiff - 4);
            fts5BufferAppendBlob(&p->rc, &buf, 
                pData->nn - pSeg->iPgidxOff, &pData->p[pSeg->iPgidxOff]
            );
          }

          pSeg->pSeg->pgnoFirst = pSeg->iTermLeafPgno;
          fts5DataDelete(p, FTS5_SEGMENT_ROWID(iId, 1), iLeafRowid);
          fts5DataWrite(p, iLeafRowid, buf.p, buf.n);
        }
        fts5DataRelease(pData);
      }
    }
  }
  fts5BufferFree(&buf);
}

static void fts5MergeChunkCallback(
  Fts5Index *p, 
  void *pCtx, 
  const u8 *pChunk, int nChunk
){
  Fts5SegWriter *pWriter = (Fts5SegWriter*)pCtx;
  fts5WriteAppendPoslistData(p, pWriter, pChunk, nChunk);
}

/*
**
*/
static void fts5IndexMergeLevel(
  Fts5Index *p,                   /* FTS5 backend object */
  Fts5Structure **ppStruct,       /* IN/OUT: Stucture of index */
  int iLvl,                       /* Level to read input from */
  int *pnRem                      /* Write up to this many output leaves */
){
  Fts5Structure *pStruct = *ppStruct;
  Fts5StructureLevel *pLvl = &pStruct->aLevel[iLvl];
  Fts5StructureLevel *pLvlOut;
  Fts5Iter *pIter = 0;       /* Iterator to read input data */
  int nRem = pnRem ? *pnRem : 0;  /* Output leaf pages left to write */
  int nInput;                     /* Number of input segments */
  Fts5SegWriter writer;           /* Writer object */
  Fts5StructureSegment *pSeg;     /* Output segment */
  Fts5Buffer term;
  int bOldest;                    /* True if the output segment is the oldest */
  int eDetail = p->pConfig->eDetail;
  const int flags = FTS5INDEX_QUERY_NOOUTPUT;
  int bTermWritten = 0;           /* True if current term already output */

  assert( iLvl<pStruct->nLevel );
  assert( pLvl->nMerge<=pLvl->nSeg );

  memset(&writer, 0, sizeof(Fts5SegWriter));
  memset(&term, 0, sizeof(Fts5Buffer));
  if( pLvl->nMerge ){
    pLvlOut = &pStruct->aLevel[iLvl+1];
    assert( pLvlOut->nSeg>0 );
    nInput = pLvl->nMerge;
    pSeg = &pLvlOut->aSeg[pLvlOut->nSeg-1];

    fts5WriteInit(p, &writer, pSeg->iSegid);
    writer.writer.pgno = pSeg->pgnoLast+1;
    writer.iBtPage = 0;
  }else{
    int iSegid = fts5AllocateSegid(p, pStruct);

    /* Extend the Fts5Structure object as required to ensure the output
    ** segment exists. */
    if( iLvl==pStruct->nLevel-1 ){
      fts5StructureAddLevel(&p->rc, ppStruct);
      pStruct = *ppStruct;
    }
    fts5StructureExtendLevel(&p->rc, pStruct, iLvl+1, 1, 0);
    if( p->rc ) return;
    pLvl = &pStruct->aLevel[iLvl];
    pLvlOut = &pStruct->aLevel[iLvl+1];

    fts5WriteInit(p, &writer, iSegid);

    /* Add the new segment to the output level */
    pSeg = &pLvlOut->aSeg[pLvlOut->nSeg];
    pLvlOut->nSeg++;
    pSeg->pgnoFirst = 1;
    pSeg->iSegid = iSegid;
    pStruct->nSegment++;

    /* Read input from all segments in the input level */
    nInput = pLvl->nSeg;
  }
  bOldest = (pLvlOut->nSeg==1 && pStruct->nLevel==iLvl+2);

  assert( iLvl>=0 );
  for(fts5MultiIterNew(p, pStruct, flags, 0, 0, 0, iLvl, nInput, &pIter);
      fts5MultiIterEof(p, pIter)==0;
      fts5MultiIterNext(p, pIter, 0, 0)
  ){
    Fts5SegIter *pSegIter = &pIter->aSeg[ pIter->aFirst[1].iFirst ];
    int nPos;                     /* position-list size field value */
    int nTerm;
    const u8 *pTerm;

    pTerm = fts5MultiIterTerm(pIter, &nTerm);
    if( nTerm!=term.n || fts5Memcmp(pTerm, term.p, nTerm) ){
      if( pnRem && writer.nLeafWritten>nRem ){
        break;
      }
      fts5BufferSet(&p->rc, &term, nTerm, pTerm);
      bTermWritten =0;
    }

    /* Check for key annihilation. */
    if( pSegIter->nPos==0 && (bOldest || pSegIter->bDel==0) ) continue;

    if( p->rc==SQLITE_OK && bTermWritten==0 ){
      /* This is a new term. Append a term to the output segment. */
      fts5WriteAppendTerm(p, &writer, nTerm, pTerm);
      bTermWritten = 1;
    }

    /* Append the rowid to the output */
    /* WRITEPOSLISTSIZE */
    fts5WriteAppendRowid(p, &writer, fts5MultiIterRowid(pIter));

    if( eDetail==FTS5_DETAIL_NONE ){
      if( pSegIter->bDel ){
        fts5BufferAppendVarint(&p->rc, &writer.writer.buf, 0);
        if( pSegIter->nPos>0 ){
          fts5BufferAppendVarint(&p->rc, &writer.writer.buf, 0);
        }
      }
    }else{
      /* Append the position-list data to the output */
      nPos = pSegIter->nPos*2 + pSegIter->bDel;
      fts5BufferAppendVarint(&p->rc, &writer.writer.buf, nPos);
      fts5ChunkIterate(p, pSegIter, (void*)&writer, fts5MergeChunkCallback);
    }
  }

  /* Flush the last leaf page to disk. Set the output segment b-tree height
  ** and last leaf page number at the same time.  */
  fts5WriteFinish(p, &writer, &pSeg->pgnoLast);

  assert( pIter!=0 || p->rc!=SQLITE_OK );
  if( fts5MultiIterEof(p, pIter) ){
    int i;

    /* Remove the redundant segments from the %_data table */
    for(i=0; i<nInput; i++){
      fts5DataRemoveSegment(p, pLvl->aSeg[i].iSegid);
    }

    /* Remove the redundant segments from the input level */
    if( pLvl->nSeg!=nInput ){
      int nMove = (pLvl->nSeg - nInput) * sizeof(Fts5StructureSegment);
      memmove(pLvl->aSeg, &pLvl->aSeg[nInput], nMove);
    }
    pStruct->nSegment -= nInput;
    pLvl->nSeg -= nInput;
    pLvl->nMerge = 0;
    if( pSeg->pgnoLast==0 ){
      pLvlOut->nSeg--;
      pStruct->nSegment--;
    }
  }else{
    assert( pSeg->pgnoLast>0 );
    fts5TrimSegments(p, pIter);
    pLvl->nMerge = nInput;
  }

  fts5MultiIterFree(pIter);
  fts5BufferFree(&term);
  if( pnRem ) *pnRem -= writer.nLeafWritten;
}

/*
** Do up to nPg pages of automerge work on the index.
**
** Return true if any changes were actually made, or false otherwise.
*/
static int fts5IndexMerge(
  Fts5Index *p,                   /* FTS5 backend object */
  Fts5Structure **ppStruct,       /* IN/OUT: Current structure of index */
  int nPg,                        /* Pages of work to do */
  int nMin                        /* Minimum number of segments to merge */
){
  int nRem = nPg;
  int bRet = 0;
  Fts5Structure *pStruct = *ppStruct;
  while( nRem>0 && p->rc==SQLITE_OK ){
    int iLvl;                   /* To iterate through levels */
    int iBestLvl = 0;           /* Level offering the most input segments */
    int nBest = 0;              /* Number of input segments on best level */

    /* Set iBestLvl to the level to read input segments from. */
    assert( pStruct->nLevel>0 );
    for(iLvl=0; iLvl<pStruct->nLevel; iLvl++){
      Fts5StructureLevel *pLvl = &pStruct->aLevel[iLvl];
      if( pLvl->nMerge ){
        if( pLvl->nMerge>nBest ){
          iBestLvl = iLvl;
          nBest = pLvl->nMerge;
        }
        break;
      }
      if( pLvl->nSeg>nBest ){
        nBest = pLvl->nSeg;
        iBestLvl = iLvl;
      }
    }

    /* If nBest is still 0, then the index must be empty. */
#ifdef SQLITE_DEBUG
    for(iLvl=0; nBest==0 && iLvl<pStruct->nLevel; iLvl++){
      assert( pStruct->aLevel[iLvl].nSeg==0 );
    }
#endif

    if( nBest<nMin && pStruct->aLevel[iBestLvl].nMerge==0 ){
      break;
    }
    bRet = 1;
    fts5IndexMergeLevel(p, &pStruct, iBestLvl, &nRem);
    if( p->rc==SQLITE_OK && pStruct->aLevel[iBestLvl].nMerge==0 ){
      fts5StructurePromote(p, iBestLvl+1, pStruct);
    }
  }
  *ppStruct = pStruct;
  return bRet;
}

/*
** A total of nLeaf leaf pages of data has just been flushed to a level-0
** segment. This function updates the write-counter accordingly and, if
** necessary, performs incremental merge work.
**
** If an error occurs, set the Fts5Index.rc error code. If an error has 
** already occurred, this function is a no-op.
*/
static void fts5IndexAutomerge(
  Fts5Index *p,                   /* FTS5 backend object */
  Fts5Structure **ppStruct,       /* IN/OUT: Current structure of index */
  int nLeaf                       /* Number of output leaves just written */
){
  if( p->rc==SQLITE_OK && p->pConfig->nAutomerge>0 && ALWAYS((*ppStruct)!=0) ){
    Fts5Structure *pStruct = *ppStruct;
    u64 nWrite;                   /* Initial value of write-counter */
    int nWork;                    /* Number of work-quanta to perform */
    int nRem;                     /* Number of leaf pages left to write */

    /* Update the write-counter. While doing so, set nWork. */
    nWrite = pStruct->nWriteCounter;
    nWork = (int)(((nWrite + nLeaf) / p->nWorkUnit) - (nWrite / p->nWorkUnit));
    pStruct->nWriteCounter += nLeaf;
    nRem = (int)(p->nWorkUnit * nWork * pStruct->nLevel);

    fts5IndexMerge(p, ppStruct, nRem, p->pConfig->nAutomerge);
  }
}

static void fts5IndexCrisismerge(
  Fts5Index *p,                   /* FTS5 backend object */
  Fts5Structure **ppStruct        /* IN/OUT: Current structure of index */
){
  const int nCrisis = p->pConfig->nCrisisMerge;
  Fts5Structure *pStruct = *ppStruct;
  int iLvl = 0;

  assert( p->rc!=SQLITE_OK || pStruct->nLevel>0 );
  while( p->rc==SQLITE_OK && pStruct->aLevel[iLvl].nSeg>=nCrisis ){
    fts5IndexMergeLevel(p, &pStruct, iLvl, 0);
    assert( p->rc!=SQLITE_OK || pStruct->nLevel>(iLvl+1) );
    fts5StructurePromote(p, iLvl+1, pStruct);
    iLvl++;
  }
  *ppStruct = pStruct;
}

static int fts5IndexReturn(Fts5Index *p){
  int rc = p->rc;
  p->rc = SQLITE_OK;
  return rc;
}

typedef struct Fts5FlushCtx Fts5FlushCtx;
struct Fts5FlushCtx {
  Fts5Index *pIdx;
  Fts5SegWriter writer; 
};

/*
** Buffer aBuf[] contains a list of varints, all small enough to fit
** in a 32-bit integer. Return the size of the largest prefix of this 
** list nMax bytes or less in size.
*/
static int fts5PoslistPrefix(const u8 *aBuf, int nMax){
  int ret;
  u32 dummy;
  ret = fts5GetVarint32(aBuf, dummy);
  if( ret<nMax ){
    while( 1 ){
      int i = fts5GetVarint32(&aBuf[ret], dummy);
      if( (ret + i) > nMax ) break;
      ret += i;
    }
  }
  return ret;
}

/*
** Flush the contents of in-memory hash table iHash to a new level-0 
** segment on disk. Also update the corresponding structure record.
**
** If an error occurs, set the Fts5Index.rc error code. If an error has 
** already occurred, this function is a no-op.
*/
static void fts5FlushOneHash(Fts5Index *p){
  Fts5Hash *pHash = p->pHash;
  Fts5Structure *pStruct;
  int iSegid;
  int pgnoLast = 0;                 /* Last leaf page number in segment */

  /* Obtain a reference to the index structure and allocate a new segment-id
  ** for the new level-0 segment.  */
  pStruct = fts5StructureRead(p);
  iSegid = fts5AllocateSegid(p, pStruct);
  fts5StructureInvalidate(p);

  if( iSegid ){
    const int pgsz = p->pConfig->pgsz;
    int eDetail = p->pConfig->eDetail;
    Fts5StructureSegment *pSeg;   /* New segment within pStruct */
    Fts5Buffer *pBuf;             /* Buffer in which to assemble leaf page */
    Fts5Buffer *pPgidx;           /* Buffer in which to assemble pgidx */

    Fts5SegWriter writer;
    fts5WriteInit(p, &writer, iSegid);

    pBuf = &writer.writer.buf;
    pPgidx = &writer.writer.pgidx;

    /* fts5WriteInit() should have initialized the buffers to (most likely)
    ** the maximum space required. */
    assert( p->rc || pBuf->nSpace>=(pgsz + FTS5_DATA_PADDING) );
    assert( p->rc || pPgidx->nSpace>=(pgsz + FTS5_DATA_PADDING) );

    /* Begin scanning through hash table entries. This loop runs once for each
    ** term/doclist currently stored within the hash table. */
    if( p->rc==SQLITE_OK ){
      p->rc = sqlite3Fts5HashScanInit(pHash, 0, 0);
    }
    while( p->rc==SQLITE_OK && 0==sqlite3Fts5HashScanEof(pHash) ){
      const char *zTerm;          /* Buffer containing term */
      const u8 *pDoclist;         /* Pointer to doclist for this term */
      int nDoclist;               /* Size of doclist in bytes */

      /* Write the term for this entry to disk. */
      sqlite3Fts5HashScanEntry(pHash, &zTerm, &pDoclist, &nDoclist);
      fts5WriteAppendTerm(p, &writer, (int)strlen(zTerm), (const u8*)zTerm);
      if( p->rc!=SQLITE_OK ) break;

      assert( writer.bFirstRowidInPage==0 );
      if( pgsz>=(pBuf->n + pPgidx->n + nDoclist + 1) ){
        /* The entire doclist will fit on the current leaf. */
        fts5BufferSafeAppendBlob(pBuf, pDoclist, nDoclist);
      }else{
        i64 iRowid = 0;
        u64 iDelta = 0;
        int iOff = 0;

        /* The entire doclist will not fit on this leaf. The following 
        ** loop iterates through the poslists that make up the current 
        ** doclist.  */
        while( p->rc==SQLITE_OK && iOff<nDoclist ){
          iOff += fts5GetVarint(&pDoclist[iOff], &iDelta);
          iRowid += iDelta;
          
          if( writer.bFirstRowidInPage ){
            fts5PutU16(&pBuf->p[0], (u16)pBuf->n);   /* first rowid on page */
            pBuf->n += sqlite3Fts5PutVarint(&pBuf->p[pBuf->n], iRowid);
            writer.bFirstRowidInPage = 0;
            fts5WriteDlidxAppend(p, &writer, iRowid);
            if( p->rc!=SQLITE_OK ) break;
          }else{
            pBuf->n += sqlite3Fts5PutVarint(&pBuf->p[pBuf->n], iDelta);
          }
          assert( pBuf->n<=pBuf->nSpace );

          if( eDetail==FTS5_DETAIL_NONE ){
            if( iOff<nDoclist && pDoclist[iOff]==0 ){
              pBuf->p[pBuf->n++] = 0;
              iOff++;
              if( iOff<nDoclist && pDoclist[iOff]==0 ){
                pBuf->p[pBuf->n++] = 0;
                iOff++;
              }
            }
            if( (pBuf->n + pPgidx->n)>=pgsz ){
              fts5WriteFlushLeaf(p, &writer);
            }
          }else{
            int bDummy;
            int nPos;
            int nCopy = fts5GetPoslistSize(&pDoclist[iOff], &nPos, &bDummy);
            nCopy += nPos;
            if( (pBuf->n + pPgidx->n + nCopy) <= pgsz ){
              /* The entire poslist will fit on the current leaf. So copy
              ** it in one go. */
              fts5BufferSafeAppendBlob(pBuf, &pDoclist[iOff], nCopy);
            }else{
              /* The entire poslist will not fit on this leaf. So it needs
              ** to be broken into sections. The only qualification being
              ** that each varint must be stored contiguously.  */
              const u8 *pPoslist = &pDoclist[iOff];
              int iPos = 0;
              while( p->rc==SQLITE_OK ){
                int nSpace = pgsz - pBuf->n - pPgidx->n;
                int n = 0;
                if( (nCopy - iPos)<=nSpace ){
                  n = nCopy - iPos;
                }else{
                  n = fts5PoslistPrefix(&pPoslist[iPos], nSpace);
                }
                assert( n>0 );
                fts5BufferSafeAppendBlob(pBuf, &pPoslist[iPos], n);
                iPos += n;
                if( (pBuf->n + pPgidx->n)>=pgsz ){
                  fts5WriteFlushLeaf(p, &writer);
                }
                if( iPos>=nCopy ) break;
              }
            }
            iOff += nCopy;
          }
        }
      }

      /* TODO2: Doclist terminator written here. */
      /* pBuf->p[pBuf->n++] = '\0'; */
      assert( pBuf->n<=pBuf->nSpace );
      if( p->rc==SQLITE_OK ) sqlite3Fts5HashScanNext(pHash);
    }
    sqlite3Fts5HashClear(pHash);
    fts5WriteFinish(p, &writer, &pgnoLast);

    /* Update the Fts5Structure. It is written back to the database by the
    ** fts5StructureRelease() call below.  */
    if( pStruct->nLevel==0 ){
      fts5StructureAddLevel(&p->rc, &pStruct);
    }
    fts5StructureExtendLevel(&p->rc, pStruct, 0, 1, 0);
    if( p->rc==SQLITE_OK ){
      pSeg = &pStruct->aLevel[0].aSeg[ pStruct->aLevel[0].nSeg++ ];
      pSeg->iSegid = iSegid;
      pSeg->pgnoFirst = 1;
      pSeg->pgnoLast = pgnoLast;
      pStruct->nSegment++;
    }
    fts5StructurePromote(p, 0, pStruct);
  }

  fts5IndexAutomerge(p, &pStruct, pgnoLast);
  fts5IndexCrisismerge(p, &pStruct);
  fts5StructureWrite(p, pStruct);
  fts5StructureRelease(pStruct);
}

/*
** Flush any data stored in the in-memory hash tables to the database.
*/
static void fts5IndexFlush(Fts5Index *p){
  /* Unless it is empty, flush the hash table to disk */
  if( p->nPendingData ){
    assert( p->pHash );
    p->nPendingData = 0;
    fts5FlushOneHash(p);
  }
}

static Fts5Structure *fts5IndexOptimizeStruct(
  Fts5Index *p, 
  Fts5Structure *pStruct
){
  Fts5Structure *pNew = 0;
  sqlite3_int64 nByte = sizeof(Fts5Structure);
  int nSeg = pStruct->nSegment;
  int i;

  /* Figure out if this structure requires optimization. A structure does
  ** not require optimization if either:
  **
  **  + it consists of fewer than two segments, or 
  **  + all segments are on the same level, or
  **  + all segments except one are currently inputs to a merge operation.
  **
  ** In the first case, return NULL. In the second, increment the ref-count
  ** on *pStruct and return a copy of the pointer to it.
  */
  if( nSeg<2 ) return 0;
  for(i=0; i<pStruct->nLevel; i++){
    int nThis = pStruct->aLevel[i].nSeg;
    if( nThis==nSeg || (nThis==nSeg-1 && pStruct->aLevel[i].nMerge==nThis) ){
      fts5StructureRef(pStruct);
      return pStruct;
    }
    assert( pStruct->aLevel[i].nMerge<=nThis );
  }

  nByte += (pStruct->nLevel+1) * sizeof(Fts5StructureLevel);
  pNew = (Fts5Structure*)sqlite3Fts5MallocZero(&p->rc, nByte);

  if( pNew ){
    Fts5StructureLevel *pLvl;
    nByte = nSeg * sizeof(Fts5StructureSegment);
    pNew->nLevel = pStruct->nLevel+1;
    pNew->nRef = 1;
    pNew->nWriteCounter = pStruct->nWriteCounter;
    pLvl = &pNew->aLevel[pStruct->nLevel];
    pLvl->aSeg = (Fts5StructureSegment*)sqlite3Fts5MallocZero(&p->rc, nByte);
    if( pLvl->aSeg ){
      int iLvl, iSeg;
      int iSegOut = 0;
      /* Iterate through all segments, from oldest to newest. Add them to
      ** the new Fts5Level object so that pLvl->aSeg[0] is the oldest
      ** segment in the data structure.  */
      for(iLvl=pStruct->nLevel-1; iLvl>=0; iLvl--){
        for(iSeg=0; iSeg<pStruct->aLevel[iLvl].nSeg; iSeg++){
          pLvl->aSeg[iSegOut] = pStruct->aLevel[iLvl].aSeg[iSeg];
          iSegOut++;
        }
      }
      pNew->nSegment = pLvl->nSeg = nSeg;
    }else{
      sqlite3_free(pNew);
      pNew = 0;
    }
  }

  return pNew;
}

static int sqlite3Fts5IndexOptimize(Fts5Index *p){
  Fts5Structure *pStruct;
  Fts5Structure *pNew = 0;

  assert( p->rc==SQLITE_OK );
  fts5IndexFlush(p);
  pStruct = fts5StructureRead(p);
  fts5StructureInvalidate(p);

  if( pStruct ){
    pNew = fts5IndexOptimizeStruct(p, pStruct);
  }
  fts5StructureRelease(pStruct);

  assert( pNew==0 || pNew->nSegment>0 );
  if( pNew ){
    int iLvl;
    for(iLvl=0; pNew->aLevel[iLvl].nSeg==0; iLvl++){}
    while( p->rc==SQLITE_OK && pNew->aLevel[iLvl].nSeg>0 ){
      int nRem = FTS5_OPT_WORK_UNIT;
      fts5IndexMergeLevel(p, &pNew, iLvl, &nRem);
    }

    fts5StructureWrite(p, pNew);
    fts5StructureRelease(pNew);
  }

  return fts5IndexReturn(p); 
}

/*
** This is called to implement the special "VALUES('merge', $nMerge)"
** INSERT command.
*/
static int sqlite3Fts5IndexMerge(Fts5Index *p, int nMerge){
  Fts5Structure *pStruct = fts5StructureRead(p);
  if( pStruct ){
    int nMin = p->pConfig->nUsermerge;
    fts5StructureInvalidate(p);
    if( nMerge<0 ){
      Fts5Structure *pNew = fts5IndexOptimizeStruct(p, pStruct);
      fts5StructureRelease(pStruct);
      pStruct = pNew;
      nMin = 2;
      nMerge = nMerge*-1;
    }
    if( pStruct && pStruct->nLevel ){
      if( fts5IndexMerge(p, &pStruct, nMerge, nMin) ){
        fts5StructureWrite(p, pStruct);
      }
    }
    fts5StructureRelease(pStruct);
  }
  return fts5IndexReturn(p);
}

static void fts5AppendRowid(
  Fts5Index *p,
  u64 iDelta,
  Fts5Iter *pUnused,
  Fts5Buffer *pBuf
){
  UNUSED_PARAM(pUnused);
  fts5BufferAppendVarint(&p->rc, pBuf, iDelta);
}

static void fts5AppendPoslist(
  Fts5Index *p,
  u64 iDelta,
  Fts5Iter *pMulti,
  Fts5Buffer *pBuf
){
  int nData = pMulti->base.nData;
  int nByte = nData + 9 + 9 + FTS5_DATA_ZERO_PADDING;
  assert( nData>0 );
  if( p->rc==SQLITE_OK && 0==fts5BufferGrow(&p->rc, pBuf, nByte) ){
    fts5BufferSafeAppendVarint(pBuf, iDelta);
    fts5BufferSafeAppendVarint(pBuf, nData*2);
    fts5BufferSafeAppendBlob(pBuf, pMulti->base.pData, nData);
    memset(&pBuf->p[pBuf->n], 0, FTS5_DATA_ZERO_PADDING);
  }
}


static void fts5DoclistIterNext(Fts5DoclistIter *pIter){
  u8 *p = pIter->aPoslist + pIter->nSize + pIter->nPoslist;

  assert( pIter->aPoslist || (p==0 && pIter->aPoslist==0) );
  if( p>=pIter->aEof ){
    pIter->aPoslist = 0;
  }else{
    i64 iDelta;

    p += fts5GetVarint(p, (u64*)&iDelta);
    pIter->iRowid += iDelta;

    /* Read position list size */
    if( p[0] & 0x80 ){
      int nPos;
      pIter->nSize = fts5GetVarint32(p, nPos);
      pIter->nPoslist = (nPos>>1);
    }else{
      pIter->nPoslist = ((int)(p[0])) >> 1;
      pIter->nSize = 1;
    }

    pIter->aPoslist = p;
    if( &pIter->aPoslist[pIter->nPoslist]>pIter->aEof ){
      pIter->aPoslist = 0;
    }
  }
}

static void fts5DoclistIterInit(
  Fts5Buffer *pBuf, 
  Fts5DoclistIter *pIter
){
  memset(pIter, 0, sizeof(*pIter));
  if( pBuf->n>0 ){
    pIter->aPoslist = pBuf->p;
    pIter->aEof = &pBuf->p[pBuf->n];
    fts5DoclistIterNext(pIter);
  }
}

#if 0
/*
** Append a doclist to buffer pBuf.
**
** This function assumes that space within the buffer has already been
** allocated.
*/
static void fts5MergeAppendDocid(
  Fts5Buffer *pBuf,               /* Buffer to write to */
  i64 *piLastRowid,               /* IN/OUT: Previous rowid written (if any) */
  i64 iRowid                      /* Rowid to append */
){
  assert( pBuf->n!=0 || (*piLastRowid)==0 );
  fts5BufferSafeAppendVarint(pBuf, iRowid - *piLastRowid);
  *piLastRowid = iRowid;
}
#endif

#define fts5MergeAppendDocid(pBuf, iLastRowid, iRowid) {                 \
  assert( (pBuf)->n!=0 || (iLastRowid)==0 );                             \
  fts5BufferSafeAppendVarint((pBuf), (u64)(iRowid) - (u64)(iLastRowid)); \
  (iLastRowid) = (iRowid);                                               \
}

/*
** Swap the contents of buffer *p1 with that of *p2.
*/
static void fts5BufferSwap(Fts5Buffer *p1, Fts5Buffer *p2){
  Fts5Buffer tmp = *p1;
  *p1 = *p2;
  *p2 = tmp;
}

static void fts5NextRowid(Fts5Buffer *pBuf, int *piOff, i64 *piRowid){
  int i = *piOff;
  if( i>=pBuf->n ){
    *piOff = -1;
  }else{
    u64 iVal;
    *piOff = i + sqlite3Fts5GetVarint(&pBuf->p[i], &iVal);
    *piRowid += iVal;
  }
}

/*
** This is the equivalent of fts5MergePrefixLists() for detail=none mode.
** In this case the buffers consist of a delta-encoded list of rowids only.
*/
static void fts5MergeRowidLists(
  Fts5Index *p,                   /* FTS5 backend object */
  Fts5Buffer *p1,                 /* First list to merge */
  int nBuf,                       /* Number of entries in apBuf[] */
  Fts5Buffer *aBuf                /* Array of other lists to merge into p1 */
){
  int i1 = 0;
  int i2 = 0;
  i64 iRowid1 = 0;
  i64 iRowid2 = 0;
  i64 iOut = 0;
  Fts5Buffer *p2 = &aBuf[0];
  Fts5Buffer out;

  (void)nBuf;
  memset(&out, 0, sizeof(out));
  assert( nBuf==1 );
  sqlite3Fts5BufferSize(&p->rc, &out, p1->n + p2->n);
  if( p->rc ) return;

  fts5NextRowid(p1, &i1, &iRowid1);
  fts5NextRowid(p2, &i2, &iRowid2);
  while( i1>=0 || i2>=0 ){
    if( i1>=0 && (i2<0 || iRowid1<iRowid2) ){
      assert( iOut==0 || iRowid1>iOut );
      fts5BufferSafeAppendVarint(&out, iRowid1 - iOut);
      iOut = iRowid1;
      fts5NextRowid(p1, &i1, &iRowid1);
    }else{
      assert( iOut==0 || iRowid2>iOut );
      fts5BufferSafeAppendVarint(&out, iRowid2 - iOut);
      iOut = iRowid2;
      if( i1>=0 && iRowid1==iRowid2 ){
        fts5NextRowid(p1, &i1, &iRowid1);
      }
      fts5NextRowid(p2, &i2, &iRowid2);
    }
  }

  fts5BufferSwap(&out, p1);
  fts5BufferFree(&out);
}

typedef struct PrefixMerger PrefixMerger;
struct PrefixMerger {
  Fts5DoclistIter iter;           /* Doclist iterator */
  i64 iPos;                       /* For iterating through a position list */
  int iOff;
  u8 *aPos;
  PrefixMerger *pNext;            /* Next in docid/poslist order */
};

static void fts5PrefixMergerInsertByRowid(
  PrefixMerger **ppHead, 
  PrefixMerger *p
){
  if( p->iter.aPoslist ){
    PrefixMerger **pp = ppHead;
    while( *pp && p->iter.iRowid>(*pp)->iter.iRowid ){
      pp = &(*pp)->pNext;
    }
    p->pNext = *pp;
    *pp = p;
  }
}

static void fts5PrefixMergerInsertByPosition(
  PrefixMerger **ppHead, 
  PrefixMerger *p
){
  if( p->iPos>=0 ){
    PrefixMerger **pp = ppHead;
    while( *pp && p->iPos>(*pp)->iPos ){
      pp = &(*pp)->pNext;
    }
    p->pNext = *pp;
    *pp = p;
  }
}


/*
** Array aBuf[] contains nBuf doclists. These are all merged in with the
** doclist in buffer p1.
*/
static void fts5MergePrefixLists(
  Fts5Index *p,                   /* FTS5 backend object */
  Fts5Buffer *p1,                 /* First list to merge */
  int nBuf,                       /* Number of buffers in array aBuf[] */
  Fts5Buffer *aBuf                /* Other lists to merge in */ 
){
#define fts5PrefixMergerNextPosition(p) \
  sqlite3Fts5PoslistNext64((p)->aPos,(p)->iter.nPoslist,&(p)->iOff,&(p)->iPos)
#define FTS5_MERGE_NLIST 16
  PrefixMerger aMerger[FTS5_MERGE_NLIST];
  PrefixMerger *pHead = 0;
  int i;
  int nOut = 0;
  Fts5Buffer out = {0, 0, 0};
  Fts5Buffer tmp = {0, 0, 0};
  i64 iLastRowid = 0;

  /* Initialize a doclist-iterator for each input buffer. Arrange them in
  ** a linked-list starting at pHead in ascending order of rowid. Avoid
  ** linking any iterators already at EOF into the linked list at all. */ 
  assert( nBuf+1<=sizeof(aMerger)/sizeof(aMerger[0]) );
  memset(aMerger, 0, sizeof(PrefixMerger)*(nBuf+1));
  pHead = &aMerger[nBuf];
  fts5DoclistIterInit(p1, &pHead->iter);
  for(i=0; i<nBuf; i++){
    fts5DoclistIterInit(&aBuf[i], &aMerger[i].iter);
    fts5PrefixMergerInsertByRowid(&pHead, &aMerger[i]);
    nOut += aBuf[i].n;
  }
  if( nOut==0 ) return;
  nOut += p1->n + 9 + 10*nBuf;

  /* The maximum size of the output is equal to the sum of the
  ** input sizes + 1 varint (9 bytes). The extra varint is because if the
  ** first rowid in one input is a large negative number, and the first in
  ** the other a non-negative number, the delta for the non-negative
  ** number will be larger on disk than the literal integer value
  ** was.  
  **
  ** Or, if the input position-lists are corrupt, then the output might
  ** include up to (nBuf+1) extra 10-byte positions created by interpreting -1
  ** (the value PoslistNext64() uses for EOF) as a position and appending
  ** it to the output. This can happen at most once for each input 
  ** position-list, hence (nBuf+1) 10 byte paddings.  */
  if( sqlite3Fts5BufferSize(&p->rc, &out, nOut) ) return;

  while( pHead ){
    fts5MergeAppendDocid(&out, iLastRowid, pHead->iter.iRowid);

    if( pHead->pNext && iLastRowid==pHead->pNext->iter.iRowid ){
      /* Merge data from two or more poslists */
      i64 iPrev = 0;
      int nTmp = FTS5_DATA_ZERO_PADDING;
      int nMerge = 0;
      PrefixMerger *pSave = pHead;
      PrefixMerger *pThis = 0;
      int nTail = 0;

      pHead = 0;
      while( pSave && pSave->iter.iRowid==iLastRowid ){
        PrefixMerger *pNext = pSave->pNext;
        pSave->iOff = 0;
        pSave->iPos = 0;
        pSave->aPos = &pSave->iter.aPoslist[pSave->iter.nSize];
        fts5PrefixMergerNextPosition(pSave);
        nTmp += pSave->iter.nPoslist + 10;
        nMerge++;
        fts5PrefixMergerInsertByPosition(&pHead, pSave);
        pSave = pNext;
      }

      if( pHead==0 || pHead->pNext==0 ){
        p->rc = FTS5_CORRUPT;
        break;
      }

      /* See the earlier comment in this function for an explanation of why
      ** corrupt input position lists might cause the output to consume
      ** at most nMerge*10 bytes of unexpected space. */
      if( sqlite3Fts5BufferSize(&p->rc, &tmp, nTmp+nMerge*10) ){
        break;
      }
      fts5BufferZero(&tmp);

      pThis = pHead;
      pHead = pThis->pNext;
      sqlite3Fts5PoslistSafeAppend(&tmp, &iPrev, pThis->iPos);
      fts5PrefixMergerNextPosition(pThis);
      fts5PrefixMergerInsertByPosition(&pHead, pThis);

      while( pHead->pNext ){
        pThis = pHead;
        if( pThis->iPos!=iPrev ){
          sqlite3Fts5PoslistSafeAppend(&tmp, &iPrev, pThis->iPos);
        }
        fts5PrefixMergerNextPosition(pThis);
        pHead = pThis->pNext;
        fts5PrefixMergerInsertByPosition(&pHead, pThis);
      }

      if( pHead->iPos!=iPrev ){
        sqlite3Fts5PoslistSafeAppend(&tmp, &iPrev, pHead->iPos);
      }
      nTail = pHead->iter.nPoslist - pHead->iOff;

      /* WRITEPOSLISTSIZE */
      assert_nc( tmp.n+nTail<=nTmp );
      assert( tmp.n+nTail<=nTmp+nMerge*10 );
      if( tmp.n+nTail>nTmp-FTS5_DATA_ZERO_PADDING ){
        if( p->rc==SQLITE_OK ) p->rc = FTS5_CORRUPT;
        break;
      }
      fts5BufferSafeAppendVarint(&out, (tmp.n+nTail) * 2);
      fts5BufferSafeAppendBlob(&out, tmp.p, tmp.n);
      if( nTail>0 ){
        fts5BufferSafeAppendBlob(&out, &pHead->aPos[pHead->iOff], nTail);
      }

      pHead = pSave;
      for(i=0; i<nBuf+1; i++){
        PrefixMerger *pX = &aMerger[i];
        if( pX->iter.aPoslist && pX->iter.iRowid==iLastRowid ){
          fts5DoclistIterNext(&pX->iter);
          fts5PrefixMergerInsertByRowid(&pHead, pX);
        }
      }

    }else{
      /* Copy poslist from pHead to output */
      PrefixMerger *pThis = pHead;
      Fts5DoclistIter *pI = &pThis->iter;
      fts5BufferSafeAppendBlob(&out, pI->aPoslist, pI->nPoslist+pI->nSize);
      fts5DoclistIterNext(pI);
      pHead = pThis->pNext;
      fts5PrefixMergerInsertByRowid(&pHead, pThis);
    }
  }

  fts5BufferFree(p1);
  fts5BufferFree(&tmp);
  memset(&out.p[out.n], 0, FTS5_DATA_ZERO_PADDING);
  *p1 = out;
}

static void fts5SetupPrefixIter(
  Fts5Index *p,                   /* Index to read from */
  int bDesc,                      /* True for "ORDER BY rowid DESC" */
  int iIdx,                       /* Index to scan for data */
  u8 *pToken,                     /* Buffer containing prefix to match */
  int nToken,                     /* Size of buffer pToken in bytes */
  Fts5Colset *pColset,            /* Restrict matches to these columns */
  Fts5Iter **ppIter          /* OUT: New iterator */
){
  Fts5Structure *pStruct;
  Fts5Buffer *aBuf;
  int nBuf = 32;
  int nMerge = 1;

  void (*xMerge)(Fts5Index*, Fts5Buffer*, int, Fts5Buffer*);
  void (*xAppend)(Fts5Index*, u64, Fts5Iter*, Fts5Buffer*);
  if( p->pConfig->eDetail==FTS5_DETAIL_NONE ){
    xMerge = fts5MergeRowidLists;
    xAppend = fts5AppendRowid;
  }else{
    nMerge = FTS5_MERGE_NLIST-1;
    nBuf = nMerge*8;   /* Sufficient to merge (16^8)==(2^32) lists */
    xMerge = fts5MergePrefixLists;
    xAppend = fts5AppendPoslist;
  }

  aBuf = (Fts5Buffer*)fts5IdxMalloc(p, sizeof(Fts5Buffer)*nBuf);
  pStruct = fts5StructureRead(p);

  if( aBuf && pStruct ){
    const int flags = FTS5INDEX_QUERY_SCAN 
                    | FTS5INDEX_QUERY_SKIPEMPTY 
                    | FTS5INDEX_QUERY_NOOUTPUT;
    int i;
    i64 iLastRowid = 0;
    Fts5Iter *p1 = 0;     /* Iterator used to gather data from index */
    Fts5Data *pData;
    Fts5Buffer doclist;
    int bNewTerm = 1;

    memset(&doclist, 0, sizeof(doclist));
    if( iIdx!=0 ){
      int dummy = 0;
      const int f2 = FTS5INDEX_QUERY_SKIPEMPTY|FTS5INDEX_QUERY_NOOUTPUT;
      pToken[0] = FTS5_MAIN_PREFIX;
      fts5MultiIterNew(p, pStruct, f2, pColset, pToken, nToken, -1, 0, &p1);
      fts5IterSetOutputCb(&p->rc, p1);
      for(;
        fts5MultiIterEof(p, p1)==0;
        fts5MultiIterNext2(p, p1, &dummy)
      ){
        Fts5SegIter *pSeg = &p1->aSeg[ p1->aFirst[1].iFirst ];
        p1->xSetOutputs(p1, pSeg);
        if( p1->base.nData ){
          xAppend(p, (u64)p1->base.iRowid-(u64)iLastRowid, p1, &doclist);
          iLastRowid = p1->base.iRowid;
        }
      }
      fts5MultiIterFree(p1);
    }

    pToken[0] = FTS5_MAIN_PREFIX + iIdx;
    fts5MultiIterNew(p, pStruct, flags, pColset, pToken, nToken, -1, 0, &p1);
    fts5IterSetOutputCb(&p->rc, p1);
    for( /* no-op */ ;
        fts5MultiIterEof(p, p1)==0;
        fts5MultiIterNext2(p, p1, &bNewTerm)
    ){
      Fts5SegIter *pSeg = &p1->aSeg[ p1->aFirst[1].iFirst ];
      int nTerm = pSeg->term.n;
      const u8 *pTerm = pSeg->term.p;
      p1->xSetOutputs(p1, pSeg);

      assert_nc( memcmp(pToken, pTerm, MIN(nToken, nTerm))<=0 );
      if( bNewTerm ){
        if( nTerm<nToken || memcmp(pToken, pTerm, nToken) ) break;
      }

      if( p1->base.nData==0 ) continue;

      if( p1->base.iRowid<=iLastRowid && doclist.n>0 ){
        for(i=0; p->rc==SQLITE_OK && doclist.n; i++){
          int i1 = i*nMerge;
          int iStore;
          assert( i1+nMerge<=nBuf );
          for(iStore=i1; iStore<i1+nMerge; iStore++){
            if( aBuf[iStore].n==0 ){
              fts5BufferSwap(&doclist, &aBuf[iStore]);
              fts5BufferZero(&doclist);
              break;
            }
          }
          if( iStore==i1+nMerge ){
            xMerge(p, &doclist, nMerge, &aBuf[i1]);
            for(iStore=i1; iStore<i1+nMerge; iStore++){
              fts5BufferZero(&aBuf[iStore]);
            }
          }
        }
        iLastRowid = 0;
      }

      xAppend(p, (u64)p1->base.iRowid-(u64)iLastRowid, p1, &doclist);
      iLastRowid = p1->base.iRowid;
    }

    assert( (nBuf%nMerge)==0 );
    for(i=0; i<nBuf; i+=nMerge){
      int iFree;
      if( p->rc==SQLITE_OK ){
        xMerge(p, &doclist, nMerge, &aBuf[i]);
      }
      for(iFree=i; iFree<i+nMerge; iFree++){
        fts5BufferFree(&aBuf[iFree]);
      }
    }
    fts5MultiIterFree(p1);

    pData = fts5IdxMalloc(p, sizeof(Fts5Data)+doclist.n+FTS5_DATA_ZERO_PADDING);
    if( pData ){
      pData->p = (u8*)&pData[1];
      pData->nn = pData->szLeaf = doclist.n;
      if( doclist.n ) memcpy(pData->p, doclist.p, doclist.n);
      fts5MultiIterNew2(p, pData, bDesc, ppIter);
    }
    fts5BufferFree(&doclist);
  }

  fts5StructureRelease(pStruct);
  sqlite3_free(aBuf);
}


/*
** Indicate that all subsequent calls to sqlite3Fts5IndexWrite() pertain
** to the document with rowid iRowid.
*/
static int sqlite3Fts5IndexBeginWrite(Fts5Index *p, int bDelete, i64 iRowid){
  assert( p->rc==SQLITE_OK );

  /* Allocate the hash table if it has not already been allocated */
  if( p->pHash==0 ){
    p->rc = sqlite3Fts5HashNew(p->pConfig, &p->pHash, &p->nPendingData);
  }

  /* Flush the hash table to disk if required */
  if( iRowid<p->iWriteRowid 
   || (iRowid==p->iWriteRowid && p->bDelete==0)
   || (p->nPendingData > p->pConfig->nHashSize) 
  ){
    fts5IndexFlush(p);
  }

  p->iWriteRowid = iRowid;
  p->bDelete = bDelete;
  return fts5IndexReturn(p);
}

/*
** Commit data to disk.
*/
static int sqlite3Fts5IndexSync(Fts5Index *p){
  assert( p->rc==SQLITE_OK );
  fts5IndexFlush(p);
  sqlite3Fts5IndexCloseReader(p);
  return fts5IndexReturn(p);
}

/*
** Discard any data stored in the in-memory hash tables. Do not write it
** to the database. Additionally, assume that the contents of the %_data
** table may have changed on disk. So any in-memory caches of %_data 
** records must be invalidated.
*/
static int sqlite3Fts5IndexRollback(Fts5Index *p){
  sqlite3Fts5IndexCloseReader(p);
  fts5IndexDiscardData(p);
  fts5StructureInvalidate(p);
  /* assert( p->rc==SQLITE_OK ); */
  return SQLITE_OK;
}

/*
** The %_data table is completely empty when this function is called. This
** function populates it with the initial structure objects for each index,
** and the initial version of the "averages" record (a zero-byte blob).
*/
static int sqlite3Fts5IndexReinit(Fts5Index *p){
  Fts5Structure s;
  fts5StructureInvalidate(p);
  fts5IndexDiscardData(p);
  memset(&s, 0, sizeof(Fts5Structure));
  fts5DataWrite(p, FTS5_AVERAGES_ROWID, (const u8*)"", 0);
  fts5StructureWrite(p, &s);
  return fts5IndexReturn(p);
}

/*
** Open a new Fts5Index handle. If the bCreate argument is true, create
** and initialize the underlying %_data table.
**
** If successful, set *pp to point to the new object and return SQLITE_OK.
** Otherwise, set *pp to NULL and return an SQLite error code.
*/
static int sqlite3Fts5IndexOpen(
  Fts5Config *pConfig, 
  int bCreate, 
  Fts5Index **pp,
  char **pzErr
){
  int rc = SQLITE_OK;
  Fts5Index *p;                   /* New object */

  *pp = p = (Fts5Index*)sqlite3Fts5MallocZero(&rc, sizeof(Fts5Index));
  if( rc==SQLITE_OK ){
    p->pConfig = pConfig;
    p->nWorkUnit = FTS5_WORK_UNIT;
    p->zDataTbl = sqlite3Fts5Mprintf(&rc, "%s_data", pConfig->zName);
    if( p->zDataTbl && bCreate ){
      rc = sqlite3Fts5CreateTable(
          pConfig, "data", "id INTEGER PRIMARY KEY, block BLOB", 0, pzErr
      );
      if( rc==SQLITE_OK ){
        rc = sqlite3Fts5CreateTable(pConfig, "idx", 
            "segid, term, pgno, PRIMARY KEY(segid, term)", 
            1, pzErr
        );
      }
      if( rc==SQLITE_OK ){
        rc = sqlite3Fts5IndexReinit(p);
      }
    }
  }

  assert( rc!=SQLITE_OK || p->rc==SQLITE_OK );
  if( rc ){
    sqlite3Fts5IndexClose(p);
    *pp = 0;
  }
  return rc;
}

/*
** Close a handle opened by an earlier call to sqlite3Fts5IndexOpen().
*/
static int sqlite3Fts5IndexClose(Fts5Index *p){
  int rc = SQLITE_OK;
  if( p ){
    assert( p->pReader==0 );
    fts5StructureInvalidate(p);
    sqlite3_finalize(p->pWriter);
    sqlite3_finalize(p->pDeleter);
    sqlite3_finalize(p->pIdxWriter);
    sqlite3_finalize(p->pIdxDeleter);
    sqlite3_finalize(p->pIdxSelect);
    sqlite3_finalize(p->pDataVersion);
    sqlite3Fts5HashFree(p->pHash);
    sqlite3_free(p->zDataTbl);
    sqlite3_free(p);
  }
  return rc;
}

/*
** Argument p points to a buffer containing utf-8 text that is n bytes in 
** size. Return the number of bytes in the nChar character prefix of the
** buffer, or 0 if there are less than nChar characters in total.
*/
static int sqlite3Fts5IndexCharlenToBytelen(
  const char *p, 
  int nByte, 
  int nChar
){
  int n = 0;
  int i;
  for(i=0; i<nChar; i++){
    if( n>=nByte ) return 0;      /* Input contains fewer than nChar chars */
    if( (unsigned char)p[n++]>=0xc0 ){
      if( n>=nByte ) return 0;
      while( (p[n] & 0xc0)==0x80 ){
        n++;
        if( n>=nByte ){
          if( i+1==nChar ) break;
          return 0;
        }
      }
    }
  }
  return n;
}

/*
** pIn is a UTF-8 encoded string, nIn bytes in size. Return the number of
** unicode characters in the string.
*/
static int fts5IndexCharlen(const char *pIn, int nIn){
  int nChar = 0;            
  int i = 0;
  while( i<nIn ){
    if( (unsigned char)pIn[i++]>=0xc0 ){
      while( i<nIn && (pIn[i] & 0xc0)==0x80 ) i++;
    }
    nChar++;
  }
  return nChar;
}

/*
** Insert or remove data to or from the index. Each time a document is 
** added to or removed from the index, this function is called one or more
** times.
**
** For an insert, it must be called once for each token in the new document.
** If the operation is a delete, it must be called (at least) once for each
** unique token in the document with an iCol value less than zero. The iPos
** argument is ignored for a delete.
*/
static int sqlite3Fts5IndexWrite(
  Fts5Index *p,                   /* Index to write to */
  int iCol,                       /* Column token appears in (-ve -> delete) */
  int iPos,                       /* Position of token within column */
  const char *pToken, int nToken  /* Token to add or remove to or from index */
){
  int i;                          /* Used to iterate through indexes */
  int rc = SQLITE_OK;             /* Return code */
  Fts5Config *pConfig = p->pConfig;

  assert( p->rc==SQLITE_OK );
  assert( (iCol<0)==p->bDelete );

  /* Add the entry to the main terms index. */
  rc = sqlite3Fts5HashWrite(
      p->pHash, p->iWriteRowid, iCol, iPos, FTS5_MAIN_PREFIX, pToken, nToken
  );

  for(i=0; i<pConfig->nPrefix && rc==SQLITE_OK; i++){
    const int nChar = pConfig->aPrefix[i];
    int nByte = sqlite3Fts5IndexCharlenToBytelen(pToken, nToken, nChar);
    if( nByte ){
      rc = sqlite3Fts5HashWrite(p->pHash, 
          p->iWriteRowid, iCol, iPos, (char)(FTS5_MAIN_PREFIX+i+1), pToken,
          nByte
      );
    }
  }

  return rc;
}

/*
** Open a new iterator to iterate though all rowid that match the 
** specified token or token prefix.
*/
static int sqlite3Fts5IndexQuery(
  Fts5Index *p,                   /* FTS index to query */
  const char *pToken, int nToken, /* Token (or prefix) to query for */
  int flags,                      /* Mask of FTS5INDEX_QUERY_X flags */
  Fts5Colset *pColset,            /* Match these columns only */
  Fts5IndexIter **ppIter          /* OUT: New iterator object */
){
  Fts5Config *pConfig = p->pConfig;
  Fts5Iter *pRet = 0;
  Fts5Buffer buf = {0, 0, 0};

  /* If the QUERY_SCAN flag is set, all other flags must be clear. */
  assert( (flags & FTS5INDEX_QUERY_SCAN)==0 || flags==FTS5INDEX_QUERY_SCAN );

  /* [jart] what is with these gcc11 warnings */
  void *MemCpy(void *, const void *, size_t) asm("memcpy");

  if( sqlite3Fts5BufferSize(&p->rc, &buf, nToken+1)==0 ){
    int iIdx = 0;                 /* Index to search */
    int iPrefixIdx = 0;           /* +1 prefix index */
    if( nToken ) MemCpy(&buf.p[1], pToken, nToken);

    /* Figure out which index to search and set iIdx accordingly. If this
    ** is a prefix query for which there is no prefix index, set iIdx to
    ** greater than pConfig->nPrefix to indicate that the query will be
    ** satisfied by scanning multiple terms in the main index.
    **
    ** If the QUERY_TEST_NOIDX flag was specified, then this must be a
    ** prefix-query. Instead of using a prefix-index (if one exists), 
    ** evaluate the prefix query using the main FTS index. This is used
    ** for internal sanity checking by the integrity-check in debug 
    ** mode only.  */
#ifdef SQLITE_DEBUG
    if( pConfig->bPrefixIndex==0 || (flags & FTS5INDEX_QUERY_TEST_NOIDX) ){
      assert( flags & FTS5INDEX_QUERY_PREFIX );
      iIdx = 1+pConfig->nPrefix;
    }else
#endif
    if( flags & FTS5INDEX_QUERY_PREFIX ){
      int nChar = fts5IndexCharlen(pToken, nToken);
      for(iIdx=1; iIdx<=pConfig->nPrefix; iIdx++){
        int nIdxChar = pConfig->aPrefix[iIdx-1];
        if( nIdxChar==nChar ) break;
        if( nIdxChar==nChar+1 ) iPrefixIdx = iIdx;
      }
    }

    if( iIdx<=pConfig->nPrefix ){
      /* Straight index lookup */
      Fts5Structure *pStruct = fts5StructureRead(p);
      buf.p[0] = (u8)(FTS5_MAIN_PREFIX + iIdx);
      if( pStruct ){
        fts5MultiIterNew(p, pStruct, flags | FTS5INDEX_QUERY_SKIPEMPTY, 
            pColset, buf.p, nToken+1, -1, 0, &pRet
        );
        fts5StructureRelease(pStruct);
      }
    }else{
      /* Scan multiple terms in the main index */
      int bDesc = (flags & FTS5INDEX_QUERY_DESC)!=0;
      fts5SetupPrefixIter(p, bDesc, iPrefixIdx, buf.p, nToken+1, pColset,&pRet);
      if( pRet==0 ){
        assert( p->rc!=SQLITE_OK );
      }else{
        assert( pRet->pColset==0 );
        fts5IterSetOutputCb(&p->rc, pRet);
        if( p->rc==SQLITE_OK ){
          Fts5SegIter *pSeg = &pRet->aSeg[pRet->aFirst[1].iFirst];
          if( pSeg->pLeaf ) pRet->xSetOutputs(pRet, pSeg);
        }
      }
    }

    if( p->rc ){
      sqlite3Fts5IterClose((Fts5IndexIter*)pRet);
      pRet = 0;
      sqlite3Fts5IndexCloseReader(p);
    }

    *ppIter = (Fts5IndexIter*)pRet;
    sqlite3Fts5BufferFree(&buf);
  }
  return fts5IndexReturn(p);
}

/*
** Return true if the iterator passed as the only argument is at EOF.
*/
/*
** Move to the next matching rowid. 
*/
static int sqlite3Fts5IterNext(Fts5IndexIter *pIndexIter){
  Fts5Iter *pIter = (Fts5Iter*)pIndexIter;
  assert( pIter->pIndex->rc==SQLITE_OK );
  fts5MultiIterNext(pIter->pIndex, pIter, 0, 0);
  return fts5IndexReturn(pIter->pIndex);
}

/*
** Move to the next matching term/rowid. Used by the fts5vocab module.
*/
static int sqlite3Fts5IterNextScan(Fts5IndexIter *pIndexIter){
  Fts5Iter *pIter = (Fts5Iter*)pIndexIter;
  Fts5Index *p = pIter->pIndex;

  assert( pIter->pIndex->rc==SQLITE_OK );

  fts5MultiIterNext(p, pIter, 0, 0);
  if( p->rc==SQLITE_OK ){
    Fts5SegIter *pSeg = &pIter->aSeg[ pIter->aFirst[1].iFirst ];
    if( pSeg->pLeaf && pSeg->term.p[0]!=FTS5_MAIN_PREFIX ){
      fts5DataRelease(pSeg->pLeaf);
      pSeg->pLeaf = 0;
      pIter->base.bEof = 1;
    }
  }

  return fts5IndexReturn(pIter->pIndex);
}

/*
** Move to the next matching rowid that occurs at or after iMatch. The
** definition of "at or after" depends on whether this iterator iterates
** in ascending or descending rowid order.
*/
static int sqlite3Fts5IterNextFrom(Fts5IndexIter *pIndexIter, i64 iMatch){
  Fts5Iter *pIter = (Fts5Iter*)pIndexIter;
  fts5MultiIterNextFrom(pIter->pIndex, pIter, iMatch);
  return fts5IndexReturn(pIter->pIndex);
}

/*
** Return the current term.
*/
static const char *sqlite3Fts5IterTerm(Fts5IndexIter *pIndexIter, int *pn){
  int n;
  const char *z = (const char*)fts5MultiIterTerm((Fts5Iter*)pIndexIter, &n);
  assert_nc( z || n<=1 );
  *pn = n-1;
  return (z ? &z[1] : 0);
}

/*
** Close an iterator opened by an earlier call to sqlite3Fts5IndexQuery().
*/
static void sqlite3Fts5IterClose(Fts5IndexIter *pIndexIter){
  if( pIndexIter ){
    Fts5Iter *pIter = (Fts5Iter*)pIndexIter;
    Fts5Index *pIndex = pIter->pIndex;
    fts5MultiIterFree(pIter);
    sqlite3Fts5IndexCloseReader(pIndex);
  }
}

/*
** Read and decode the "averages" record from the database. 
**
** Parameter anSize must point to an array of size nCol, where nCol is
** the number of user defined columns in the FTS table.
*/
static int sqlite3Fts5IndexGetAverages(Fts5Index *p, i64 *pnRow, i64 *anSize){
  int nCol = p->pConfig->nCol;
  Fts5Data *pData;

  *pnRow = 0;
  memset(anSize, 0, sizeof(i64) * nCol);
  pData = fts5DataRead(p, FTS5_AVERAGES_ROWID);
  if( p->rc==SQLITE_OK && pData->nn ){
    int i = 0;
    int iCol;
    i += fts5GetVarint(&pData->p[i], (u64*)pnRow);
    for(iCol=0; i<pData->nn && iCol<nCol; iCol++){
      i += fts5GetVarint(&pData->p[i], (u64*)&anSize[iCol]);
    }
  }

  fts5DataRelease(pData);
  return fts5IndexReturn(p);
}

/*
** Replace the current "averages" record with the contents of the buffer 
** supplied as the second argument.
*/
static int sqlite3Fts5IndexSetAverages(Fts5Index *p, const u8 *pData, int nData){
  assert( p->rc==SQLITE_OK );
  fts5DataWrite(p, FTS5_AVERAGES_ROWID, pData, nData);
  return fts5IndexReturn(p);
}

/*
** Return the total number of blocks this module has read from the %_data
** table since it was created.
*/
static int sqlite3Fts5IndexReads(Fts5Index *p){
  return p->nRead;
}

/*
** Set the 32-bit cookie value stored at the start of all structure 
** records to the value passed as the second argument.
**
** Return SQLITE_OK if successful, or an SQLite error code if an error
** occurs.
*/
static int sqlite3Fts5IndexSetCookie(Fts5Index *p, int iNew){
  int rc;                              /* Return code */
  Fts5Config *pConfig = p->pConfig;    /* Configuration object */
  u8 aCookie[4];                       /* Binary representation of iNew */
  sqlite3_blob *pBlob = 0;

  assert( p->rc==SQLITE_OK );
  sqlite3Fts5Put32(aCookie, iNew);

  rc = sqlite3_blob_open(pConfig->db, pConfig->zDb, p->zDataTbl, 
      "block", FTS5_STRUCTURE_ROWID, 1, &pBlob
  );
  if( rc==SQLITE_OK ){
    sqlite3_blob_write(pBlob, aCookie, 4, 0);
    rc = sqlite3_blob_close(pBlob);
  }

  return rc;
}

static int sqlite3Fts5IndexLoadConfig(Fts5Index *p){
  Fts5Structure *pStruct;
  pStruct = fts5StructureRead(p);
  fts5StructureRelease(pStruct);
  return fts5IndexReturn(p);
}


/*************************************************************************
**************************************************************************
** Below this point is the implementation of the integrity-check 
** functionality.
*/

/*
** Return a simple checksum value based on the arguments.
*/
static u64 sqlite3Fts5IndexEntryCksum(
  i64 iRowid, 
  int iCol, 
  int iPos, 
  int iIdx,
  const char *pTerm,
  int nTerm
){
  int i;
  u64 ret = iRowid;
  ret += (ret<<3) + iCol;
  ret += (ret<<3) + iPos;
  if( iIdx>=0 ) ret += (ret<<3) + (FTS5_MAIN_PREFIX + iIdx);
  for(i=0; i<nTerm; i++) ret += (ret<<3) + pTerm[i];
  return ret;
}

#ifdef SQLITE_DEBUG
/*
** This function is purely an internal test. It does not contribute to 
** FTS functionality, or even the integrity-check, in any way.
**
** Instead, it tests that the same set of pgno/rowid combinations are 
** visited regardless of whether the doclist-index identified by parameters
** iSegid/iLeaf is iterated in forwards or reverse order.
*/
static void fts5TestDlidxReverse(
  Fts5Index *p, 
  int iSegid,                     /* Segment id to load from */
  int iLeaf                       /* Load doclist-index for this leaf */
){
  Fts5DlidxIter *pDlidx = 0;
  u64 cksum1 = 13;
  u64 cksum2 = 13;

  for(pDlidx=fts5DlidxIterInit(p, 0, iSegid, iLeaf);
      fts5DlidxIterEof(p, pDlidx)==0;
      fts5DlidxIterNext(p, pDlidx)
  ){
    i64 iRowid = fts5DlidxIterRowid(pDlidx);
    int pgno = fts5DlidxIterPgno(pDlidx);
    assert( pgno>iLeaf );
    cksum1 += iRowid + ((i64)pgno<<32);
  }
  fts5DlidxIterFree(pDlidx);
  pDlidx = 0;

  for(pDlidx=fts5DlidxIterInit(p, 1, iSegid, iLeaf);
      fts5DlidxIterEof(p, pDlidx)==0;
      fts5DlidxIterPrev(p, pDlidx)
  ){
    i64 iRowid = fts5DlidxIterRowid(pDlidx);
    int pgno = fts5DlidxIterPgno(pDlidx);
    assert( fts5DlidxIterPgno(pDlidx)>iLeaf );
    cksum2 += iRowid + ((i64)pgno<<32);
  }
  fts5DlidxIterFree(pDlidx);
  pDlidx = 0;

  if( p->rc==SQLITE_OK && cksum1!=cksum2 ) p->rc = FTS5_CORRUPT;
}

static int fts5QueryCksum(
  Fts5Index *p,                   /* Fts5 index object */
  int iIdx,
  const char *z,                  /* Index key to query for */
  int n,                          /* Size of index key in bytes */
  int flags,                      /* Flags for Fts5IndexQuery */
  u64 *pCksum                     /* IN/OUT: Checksum value */
){
  int eDetail = p->pConfig->eDetail;
  u64 cksum = *pCksum;
  Fts5IndexIter *pIter = 0;
  int rc = sqlite3Fts5IndexQuery(p, z, n, flags, 0, &pIter);

  while( rc==SQLITE_OK && ALWAYS(pIter!=0) && 0==sqlite3Fts5IterEof(pIter) ){
    i64 rowid = pIter->iRowid;

    if( eDetail==FTS5_DETAIL_NONE ){
      cksum ^= sqlite3Fts5IndexEntryCksum(rowid, 0, 0, iIdx, z, n);
    }else{
      Fts5PoslistReader sReader;
      for(sqlite3Fts5PoslistReaderInit(pIter->pData, pIter->nData, &sReader);
          sReader.bEof==0;
          sqlite3Fts5PoslistReaderNext(&sReader)
      ){
        int iCol = FTS5_POS2COLUMN(sReader.iPos);
        int iOff = FTS5_POS2OFFSET(sReader.iPos);
        cksum ^= sqlite3Fts5IndexEntryCksum(rowid, iCol, iOff, iIdx, z, n);
      }
    }
    if( rc==SQLITE_OK ){
      rc = sqlite3Fts5IterNext(pIter);
    }
  }
  sqlite3Fts5IterClose(pIter);

  *pCksum = cksum;
  return rc;
}

/*
** Check if buffer z[], size n bytes, contains as series of valid utf-8
** encoded codepoints. If so, return 0. Otherwise, if the buffer does not
** contain valid utf-8, return non-zero.
*/
static int fts5TestUtf8(const char *z, int n){
  int i = 0;
  assert_nc( n>0 );
  while( i<n ){
    if( (z[i] & 0x80)==0x00 ){
      i++;
    }else
    if( (z[i] & 0xE0)==0xC0 ){
      if( i+1>=n || (z[i+1] & 0xC0)!=0x80 ) return 1;
      i += 2;
    }else
    if( (z[i] & 0xF0)==0xE0 ){
      if( i+2>=n || (z[i+1] & 0xC0)!=0x80 || (z[i+2] & 0xC0)!=0x80 ) return 1;
      i += 3;
    }else
    if( (z[i] & 0xF8)==0xF0 ){
      if( i+3>=n || (z[i+1] & 0xC0)!=0x80 || (z[i+2] & 0xC0)!=0x80 ) return 1;
      if( (z[i+2] & 0xC0)!=0x80 ) return 1;
      i += 3;
    }else{
      return 1;
    }
  }

  return 0;
}

/*
** This function is also purely an internal test. It does not contribute to 
** FTS functionality, or even the integrity-check, in any way.
*/
static void fts5TestTerm(
  Fts5Index *p, 
  Fts5Buffer *pPrev,              /* Previous term */
  const char *z, int n,           /* Possibly new term to test */
  u64 expected,
  u64 *pCksum
){
  int rc = p->rc;
  if( pPrev->n==0 ){
    fts5BufferSet(&rc, pPrev, n, (const u8*)z);
  }else
  if( rc==SQLITE_OK && (pPrev->n!=n || memcmp(pPrev->p, z, n)) ){
    u64 cksum3 = *pCksum;
    const char *zTerm = (const char*)&pPrev->p[1];  /* term sans prefix-byte */
    int nTerm = pPrev->n-1;            /* Size of zTerm in bytes */
    int iIdx = (pPrev->p[0] - FTS5_MAIN_PREFIX);
    int flags = (iIdx==0 ? 0 : FTS5INDEX_QUERY_PREFIX);
    u64 ck1 = 0;
    u64 ck2 = 0;

    /* Check that the results returned for ASC and DESC queries are
    ** the same. If not, call this corruption.  */
    rc = fts5QueryCksum(p, iIdx, zTerm, nTerm, flags, &ck1);
    if( rc==SQLITE_OK ){
      int f = flags|FTS5INDEX_QUERY_DESC;
      rc = fts5QueryCksum(p, iIdx, zTerm, nTerm, f, &ck2);
    }
    if( rc==SQLITE_OK && ck1!=ck2 ) rc = FTS5_CORRUPT;

    /* If this is a prefix query, check that the results returned if the
    ** the index is disabled are the same. In both ASC and DESC order. 
    **
    ** This check may only be performed if the hash table is empty. This
    ** is because the hash table only supports a single scan query at
    ** a time, and the multi-iter loop from which this function is called
    ** is already performing such a scan. 
    **
    ** Also only do this if buffer zTerm contains nTerm bytes of valid
    ** utf-8. Otherwise, the last part of the buffer contents might contain
    ** a non-utf-8 sequence that happens to be a prefix of a valid utf-8
    ** character stored in the main fts index, which will cause the
    ** test to fail.  */
    if( p->nPendingData==0 && 0==fts5TestUtf8(zTerm, nTerm) ){
      if( iIdx>0 && rc==SQLITE_OK ){
        int f = flags|FTS5INDEX_QUERY_TEST_NOIDX;
        ck2 = 0;
        rc = fts5QueryCksum(p, iIdx, zTerm, nTerm, f, &ck2);
        if( rc==SQLITE_OK && ck1!=ck2 ) rc = FTS5_CORRUPT;
      }
      if( iIdx>0 && rc==SQLITE_OK ){
        int f = flags|FTS5INDEX_QUERY_TEST_NOIDX|FTS5INDEX_QUERY_DESC;
        ck2 = 0;
        rc = fts5QueryCksum(p, iIdx, zTerm, nTerm, f, &ck2);
        if( rc==SQLITE_OK && ck1!=ck2 ) rc = FTS5_CORRUPT;
      }
    }

    cksum3 ^= ck1;
    fts5BufferSet(&rc, pPrev, n, (const u8*)z);

    if( rc==SQLITE_OK && cksum3!=expected ){
      rc = FTS5_CORRUPT;
    }
    *pCksum = cksum3;
  }
  p->rc = rc;
}
 
#else
# define fts5TestDlidxReverse(x,y,z)
# define fts5TestTerm(u,v,w,x,y,z)
#endif

/*
** Check that:
**
**   1) All leaves of pSeg between iFirst and iLast (inclusive) exist and
**      contain zero terms.
**   2) All leaves of pSeg between iNoRowid and iLast (inclusive) exist and
**      contain zero rowids.
*/
static void fts5IndexIntegrityCheckEmpty(
  Fts5Index *p,
  Fts5StructureSegment *pSeg,     /* Segment to check internal consistency */
  int iFirst,
  int iNoRowid,
  int iLast
){
  int i;

  /* Now check that the iter.nEmpty leaves following the current leaf
  ** (a) exist and (b) contain no terms. */
  for(i=iFirst; p->rc==SQLITE_OK && i<=iLast; i++){
    Fts5Data *pLeaf = fts5DataRead(p, FTS5_SEGMENT_ROWID(pSeg->iSegid, i));
    if( pLeaf ){
      if( !fts5LeafIsTermless(pLeaf) ) p->rc = FTS5_CORRUPT;
      if( i>=iNoRowid && 0!=fts5LeafFirstRowidOff(pLeaf) ) p->rc = FTS5_CORRUPT;
    }
    fts5DataRelease(pLeaf);
  }
}

static void fts5IntegrityCheckPgidx(Fts5Index *p, Fts5Data *pLeaf){
  int iTermOff = 0;
  int ii;

  Fts5Buffer buf1 = {0,0,0};
  Fts5Buffer buf2 = {0,0,0};

  ii = pLeaf->szLeaf;
  while( ii<pLeaf->nn && p->rc==SQLITE_OK ){
    int res;
    int iOff;
    int nIncr;

    ii += fts5GetVarint32(&pLeaf->p[ii], nIncr);
    iTermOff += nIncr;
    iOff = iTermOff;

    if( iOff>=pLeaf->szLeaf ){
      p->rc = FTS5_CORRUPT;
    }else if( iTermOff==nIncr ){
      int nByte;
      iOff += fts5GetVarint32(&pLeaf->p[iOff], nByte);
      if( (iOff+nByte)>pLeaf->szLeaf ){
        p->rc = FTS5_CORRUPT;
      }else{
        fts5BufferSet(&p->rc, &buf1, nByte, &pLeaf->p[iOff]);
      }
    }else{
      int nKeep, nByte;
      iOff += fts5GetVarint32(&pLeaf->p[iOff], nKeep);
      iOff += fts5GetVarint32(&pLeaf->p[iOff], nByte);
      if( nKeep>buf1.n || (iOff+nByte)>pLeaf->szLeaf ){
        p->rc = FTS5_CORRUPT;
      }else{
        buf1.n = nKeep;
        fts5BufferAppendBlob(&p->rc, &buf1, nByte, &pLeaf->p[iOff]);
      }

      if( p->rc==SQLITE_OK ){
        res = fts5BufferCompare(&buf1, &buf2);
        if( res<=0 ) p->rc = FTS5_CORRUPT;
      }
    }
    fts5BufferSet(&p->rc, &buf2, buf1.n, buf1.p);
  }

  fts5BufferFree(&buf1);
  fts5BufferFree(&buf2);
}

static void fts5IndexIntegrityCheckSegment(
  Fts5Index *p,                   /* FTS5 backend object */
  Fts5StructureSegment *pSeg      /* Segment to check internal consistency */
){
  Fts5Config *pConfig = p->pConfig;
  sqlite3_stmt *pStmt = 0;
  int rc2;
  int iIdxPrevLeaf = pSeg->pgnoFirst-1;
  int iDlidxPrevLeaf = pSeg->pgnoLast;

  if( pSeg->pgnoFirst==0 ) return;

  fts5IndexPrepareStmt(p, &pStmt, sqlite3_mprintf(
      "SELECT segid, term, (pgno>>1), (pgno&1) FROM %Q.'%q_idx' WHERE segid=%d "
      "ORDER BY 1, 2",
      pConfig->zDb, pConfig->zName, pSeg->iSegid
  ));

  /* Iterate through the b-tree hierarchy.  */
  while( p->rc==SQLITE_OK && SQLITE_ROW==sqlite3_step(pStmt) ){
    i64 iRow;                     /* Rowid for this leaf */
    Fts5Data *pLeaf;              /* Data for this leaf */

    const char *zIdxTerm = (const char*)sqlite3_column_blob(pStmt, 1);
    int nIdxTerm = sqlite3_column_bytes(pStmt, 1);
    int iIdxLeaf = sqlite3_column_int(pStmt, 2);
    int bIdxDlidx = sqlite3_column_int(pStmt, 3);

    /* If the leaf in question has already been trimmed from the segment, 
    ** ignore this b-tree entry. Otherwise, load it into memory. */
    if( iIdxLeaf<pSeg->pgnoFirst ) continue;
    iRow = FTS5_SEGMENT_ROWID(pSeg->iSegid, iIdxLeaf);
    pLeaf = fts5LeafRead(p, iRow);
    if( pLeaf==0 ) break;

    /* Check that the leaf contains at least one term, and that it is equal
    ** to or larger than the split-key in zIdxTerm.  Also check that if there
    ** is also a rowid pointer within the leaf page header, it points to a
    ** location before the term.  */
    if( pLeaf->nn<=pLeaf->szLeaf ){
      p->rc = FTS5_CORRUPT;
    }else{
      int iOff;                   /* Offset of first term on leaf */
      int iRowidOff;              /* Offset of first rowid on leaf */
      int nTerm;                  /* Size of term on leaf in bytes */
      int res;                    /* Comparison of term and split-key */

      iOff = fts5LeafFirstTermOff(pLeaf);
      iRowidOff = fts5LeafFirstRowidOff(pLeaf);
      if( iRowidOff>=iOff || iOff>=pLeaf->szLeaf ){
        p->rc = FTS5_CORRUPT;
      }else{
        iOff += fts5GetVarint32(&pLeaf->p[iOff], nTerm);
        res = fts5Memcmp(&pLeaf->p[iOff], zIdxTerm, MIN(nTerm, nIdxTerm));
        if( res==0 ) res = nTerm - nIdxTerm;
        if( res<0 ) p->rc = FTS5_CORRUPT;
      }

      fts5IntegrityCheckPgidx(p, pLeaf);
    }
    fts5DataRelease(pLeaf);
    if( p->rc ) break;

    /* Now check that the iter.nEmpty leaves following the current leaf
    ** (a) exist and (b) contain no terms. */
    fts5IndexIntegrityCheckEmpty(
        p, pSeg, iIdxPrevLeaf+1, iDlidxPrevLeaf+1, iIdxLeaf-1
    );
    if( p->rc ) break;

    /* If there is a doclist-index, check that it looks right. */
    if( bIdxDlidx ){
      Fts5DlidxIter *pDlidx = 0;  /* For iterating through doclist index */
      int iPrevLeaf = iIdxLeaf;
      int iSegid = pSeg->iSegid;
      int iPg = 0;
      i64 iKey;

      for(pDlidx=fts5DlidxIterInit(p, 0, iSegid, iIdxLeaf);
          fts5DlidxIterEof(p, pDlidx)==0;
          fts5DlidxIterNext(p, pDlidx)
      ){

        /* Check any rowid-less pages that occur before the current leaf. */
        for(iPg=iPrevLeaf+1; iPg<fts5DlidxIterPgno(pDlidx); iPg++){
          iKey = FTS5_SEGMENT_ROWID(iSegid, iPg);
          pLeaf = fts5DataRead(p, iKey);
          if( pLeaf ){
            if( fts5LeafFirstRowidOff(pLeaf)!=0 ) p->rc = FTS5_CORRUPT;
            fts5DataRelease(pLeaf);
          }
        }
        iPrevLeaf = fts5DlidxIterPgno(pDlidx);

        /* Check that the leaf page indicated by the iterator really does
        ** contain the rowid suggested by the same. */
        iKey = FTS5_SEGMENT_ROWID(iSegid, iPrevLeaf);
        pLeaf = fts5DataRead(p, iKey);
        if( pLeaf ){
          i64 iRowid;
          int iRowidOff = fts5LeafFirstRowidOff(pLeaf);
          ASSERT_SZLEAF_OK(pLeaf);
          if( iRowidOff>=pLeaf->szLeaf ){
            p->rc = FTS5_CORRUPT;
          }else{
            fts5GetVarint(&pLeaf->p[iRowidOff], (u64*)&iRowid);
            if( iRowid!=fts5DlidxIterRowid(pDlidx) ) p->rc = FTS5_CORRUPT;
          }
          fts5DataRelease(pLeaf);
        }
      }

      iDlidxPrevLeaf = iPg;
      fts5DlidxIterFree(pDlidx);
      fts5TestDlidxReverse(p, iSegid, iIdxLeaf);
    }else{
      iDlidxPrevLeaf = pSeg->pgnoLast;
      /* TODO: Check there is no doclist index */
    }

    iIdxPrevLeaf = iIdxLeaf;
  }

  rc2 = sqlite3_finalize(pStmt);
  if( p->rc==SQLITE_OK ) p->rc = rc2;

  /* Page iter.iLeaf must now be the rightmost leaf-page in the segment */
#if 0
  if( p->rc==SQLITE_OK && iter.iLeaf!=pSeg->pgnoLast ){
    p->rc = FTS5_CORRUPT;
  }
#endif
}


/*
** Run internal checks to ensure that the FTS index (a) is internally 
** consistent and (b) contains entries for which the XOR of the checksums
** as calculated by sqlite3Fts5IndexEntryCksum() is cksum.
**
** Return SQLITE_CORRUPT if any of the internal checks fail, or if the
** checksum does not match. Return SQLITE_OK if all checks pass without
** error, or some other SQLite error code if another error (e.g. OOM)
** occurs.
*/
static int sqlite3Fts5IndexIntegrityCheck(Fts5Index *p, u64 cksum, int bUseCksum){
  int eDetail = p->pConfig->eDetail;
  u64 cksum2 = 0;                 /* Checksum based on contents of indexes */
  Fts5Buffer poslist = {0,0,0};   /* Buffer used to hold a poslist */
  Fts5Iter *pIter;                /* Used to iterate through entire index */
  Fts5Structure *pStruct;         /* Index structure */
  int iLvl, iSeg;

#ifdef SQLITE_DEBUG
  /* Used by extra internal tests only run if NDEBUG is not defined */
  u64 cksum3 = 0;                 /* Checksum based on contents of indexes */
  Fts5Buffer term = {0,0,0};      /* Buffer used to hold most recent term */
#endif
  const int flags = FTS5INDEX_QUERY_NOOUTPUT;
  
  /* Load the FTS index structure */
  pStruct = fts5StructureRead(p);
  if( pStruct==0 ){
    assert( p->rc!=SQLITE_OK );
    return fts5IndexReturn(p);
  }

  /* Check that the internal nodes of each segment match the leaves */
  for(iLvl=0; iLvl<pStruct->nLevel; iLvl++){
    for(iSeg=0; iSeg<pStruct->aLevel[iLvl].nSeg; iSeg++){
      Fts5StructureSegment *pSeg = &pStruct->aLevel[iLvl].aSeg[iSeg];
      fts5IndexIntegrityCheckSegment(p, pSeg);
    }
  }

  /* The cksum argument passed to this function is a checksum calculated
  ** based on all expected entries in the FTS index (including prefix index
  ** entries). This block checks that a checksum calculated based on the
  ** actual contents of FTS index is identical.
  **
  ** Two versions of the same checksum are calculated. The first (stack
  ** variable cksum2) based on entries extracted from the full-text index
  ** while doing a linear scan of each individual index in turn. 
  **
  ** As each term visited by the linear scans, a separate query for the
  ** same term is performed. cksum3 is calculated based on the entries
  ** extracted by these queries.
  */
  for(fts5MultiIterNew(p, pStruct, flags, 0, 0, 0, -1, 0, &pIter);
      fts5MultiIterEof(p, pIter)==0;
      fts5MultiIterNext(p, pIter, 0, 0)
  ){
    int n;                      /* Size of term in bytes */
    i64 iPos = 0;               /* Position read from poslist */
    int iOff = 0;               /* Offset within poslist */
    i64 iRowid = fts5MultiIterRowid(pIter);
    char *z = (char*)fts5MultiIterTerm(pIter, &n);

    /* If this is a new term, query for it. Update cksum3 with the results. */
    fts5TestTerm(p, &term, z, n, cksum2, &cksum3);
    if( p->rc ) break;

    if( eDetail==FTS5_DETAIL_NONE ){
      if( 0==fts5MultiIterIsEmpty(p, pIter) ){
        cksum2 ^= sqlite3Fts5IndexEntryCksum(iRowid, 0, 0, -1, z, n);
      }
    }else{
      poslist.n = 0;
      fts5SegiterPoslist(p, &pIter->aSeg[pIter->aFirst[1].iFirst], 0, &poslist);
      fts5BufferAppendBlob(&p->rc, &poslist, 4, (const u8*)"\0\0\0\0");
      while( 0==sqlite3Fts5PoslistNext64(poslist.p, poslist.n, &iOff, &iPos) ){
        int iCol = FTS5_POS2COLUMN(iPos);
        int iTokOff = FTS5_POS2OFFSET(iPos);
        cksum2 ^= sqlite3Fts5IndexEntryCksum(iRowid, iCol, iTokOff, -1, z, n);
      }
    }
  }
  fts5TestTerm(p, &term, 0, 0, cksum2, &cksum3);

  fts5MultiIterFree(pIter);
  if( p->rc==SQLITE_OK && bUseCksum && cksum!=cksum2 ) p->rc = FTS5_CORRUPT;

  fts5StructureRelease(pStruct);
#ifdef SQLITE_DEBUG
  fts5BufferFree(&term);
#endif
  fts5BufferFree(&poslist);
  return fts5IndexReturn(p);
}

/*************************************************************************
**************************************************************************
** Below this point is the implementation of the fts5_decode() scalar
** function only.
*/

#ifdef SQLITE_TEST
/*
** Decode a segment-data rowid from the %_data table. This function is
** the opposite of macro FTS5_SEGMENT_ROWID().
*/
static void fts5DecodeRowid(
  i64 iRowid,                     /* Rowid from %_data table */
  int *piSegid,                   /* OUT: Segment id */
  int *pbDlidx,                   /* OUT: Dlidx flag */
  int *piHeight,                  /* OUT: Height */
  int *piPgno                     /* OUT: Page number */
){
  *piPgno = (int)(iRowid & (((i64)1 << FTS5_DATA_PAGE_B) - 1));
  iRowid >>= FTS5_DATA_PAGE_B;

  *piHeight = (int)(iRowid & (((i64)1 << FTS5_DATA_HEIGHT_B) - 1));
  iRowid >>= FTS5_DATA_HEIGHT_B;

  *pbDlidx = (int)(iRowid & 0x0001);
  iRowid >>= FTS5_DATA_DLI_B;

  *piSegid = (int)(iRowid & (((i64)1 << FTS5_DATA_ID_B) - 1));
}
#endif /* SQLITE_TEST */

#ifdef SQLITE_TEST
static void fts5DebugRowid(int *pRc, Fts5Buffer *pBuf, i64 iKey){
  int iSegid, iHeight, iPgno, bDlidx;       /* Rowid compenents */
  fts5DecodeRowid(iKey, &iSegid, &bDlidx, &iHeight, &iPgno);

  if( iSegid==0 ){
    if( iKey==FTS5_AVERAGES_ROWID ){
      sqlite3Fts5BufferAppendPrintf(pRc, pBuf, "{averages} ");
    }else{
      sqlite3Fts5BufferAppendPrintf(pRc, pBuf, "{structure}");
    }
  }
  else{
    sqlite3Fts5BufferAppendPrintf(pRc, pBuf, "{%ssegid=%d h=%d pgno=%d}",
        bDlidx ? "dlidx " : "", iSegid, iHeight, iPgno
    );
  }
}
#endif /* SQLITE_TEST */

#ifdef SQLITE_TEST
static void fts5DebugStructure(
  int *pRc,                       /* IN/OUT: error code */
  Fts5Buffer *pBuf,
  Fts5Structure *p
){
  int iLvl, iSeg;                 /* Iterate through levels, segments */

  for(iLvl=0; iLvl<p->nLevel; iLvl++){
    Fts5StructureLevel *pLvl = &p->aLevel[iLvl];
    sqlite3Fts5BufferAppendPrintf(pRc, pBuf, 
        " {lvl=%d nMerge=%d nSeg=%d", iLvl, pLvl->nMerge, pLvl->nSeg
    );
    for(iSeg=0; iSeg<pLvl->nSeg; iSeg++){
      Fts5StructureSegment *pSeg = &pLvl->aSeg[iSeg];
      sqlite3Fts5BufferAppendPrintf(pRc, pBuf, " {id=%d leaves=%d..%d}", 
          pSeg->iSegid, pSeg->pgnoFirst, pSeg->pgnoLast
      );
    }
    sqlite3Fts5BufferAppendPrintf(pRc, pBuf, "}");
  }
}
#endif /* SQLITE_TEST */

#ifdef SQLITE_TEST
/*
** This is part of the fts5_decode() debugging aid.
**
** Arguments pBlob/nBlob contain a serialized Fts5Structure object. This
** function appends a human-readable representation of the same object
** to the buffer passed as the second argument. 
*/
static void fts5DecodeStructure(
  int *pRc,                       /* IN/OUT: error code */
  Fts5Buffer *pBuf,
  const u8 *pBlob, int nBlob
){
  int rc;                         /* Return code */
  Fts5Structure *p = 0;           /* Decoded structure object */

  rc = fts5StructureDecode(pBlob, nBlob, 0, &p);
  if( rc!=SQLITE_OK ){
    *pRc = rc;
    return;
  }

  fts5DebugStructure(pRc, pBuf, p);
  fts5StructureRelease(p);
}
#endif /* SQLITE_TEST */

#ifdef SQLITE_TEST
/*
** This is part of the fts5_decode() debugging aid.
**
** Arguments pBlob/nBlob contain an "averages" record. This function 
** appends a human-readable representation of record to the buffer passed 
** as the second argument. 
*/
static void fts5DecodeAverages(
  int *pRc,                       /* IN/OUT: error code */
  Fts5Buffer *pBuf,
  const u8 *pBlob, int nBlob
){
  int i = 0;
  const char *zSpace = "";

  while( i<nBlob ){
    u64 iVal;
    i += sqlite3Fts5GetVarint(&pBlob[i], &iVal);
    sqlite3Fts5BufferAppendPrintf(pRc, pBuf, "%s%d", zSpace, (int)iVal);
    zSpace = " ";
  }
}
#endif /* SQLITE_TEST */

#ifdef SQLITE_TEST
/*
** Buffer (a/n) is assumed to contain a list of serialized varints. Read
** each varint and append its string representation to buffer pBuf. Return
** after either the input buffer is exhausted or a 0 value is read.
**
** The return value is the number of bytes read from the input buffer.
*/
static int fts5DecodePoslist(int *pRc, Fts5Buffer *pBuf, const u8 *a, int n){
  int iOff = 0;
  while( iOff<n ){
    int iVal;
    iOff += fts5GetVarint32(&a[iOff], iVal);
    sqlite3Fts5BufferAppendPrintf(pRc, pBuf, " %d", iVal);
  }
  return iOff;
}
#endif /* SQLITE_TEST */

#ifdef SQLITE_TEST
/*
** The start of buffer (a/n) contains the start of a doclist. The doclist
** may or may not finish within the buffer. This function appends a text
** representation of the part of the doclist that is present to buffer
** pBuf. 
**
** The return value is the number of bytes read from the input buffer.
*/
static int fts5DecodeDoclist(int *pRc, Fts5Buffer *pBuf, const u8 *a, int n){
  i64 iDocid = 0;
  int iOff = 0;

  if( n>0 ){
    iOff = sqlite3Fts5GetVarint(a, (u64*)&iDocid);
    sqlite3Fts5BufferAppendPrintf(pRc, pBuf, " id=%lld", iDocid);
  }
  while( iOff<n ){
    int nPos;
    int bDel;
    iOff += fts5GetPoslistSize(&a[iOff], &nPos, &bDel);
    sqlite3Fts5BufferAppendPrintf(pRc, pBuf, " nPos=%d%s", nPos, bDel?"*":"");
    iOff += fts5DecodePoslist(pRc, pBuf, &a[iOff], MIN(n-iOff, nPos));
    if( iOff<n ){
      i64 iDelta;
      iOff += sqlite3Fts5GetVarint(&a[iOff], (u64*)&iDelta);
      iDocid += iDelta;
      sqlite3Fts5BufferAppendPrintf(pRc, pBuf, " id=%lld", iDocid);
    }
  }

  return iOff;
}
#endif /* SQLITE_TEST */

#ifdef SQLITE_TEST
/*
** This function is part of the fts5_decode() debugging function. It is 
** only ever used with detail=none tables.
**
** Buffer (pData/nData) contains a doclist in the format used by detail=none
** tables. This function appends a human-readable version of that list to
** buffer pBuf.
**
** If *pRc is other than SQLITE_OK when this function is called, it is a
** no-op. If an OOM or other error occurs within this function, *pRc is
** set to an SQLite error code before returning. The final state of buffer
** pBuf is undefined in this case.
*/
static void fts5DecodeRowidList(
  int *pRc,                       /* IN/OUT: Error code */
  Fts5Buffer *pBuf,               /* Buffer to append text to */
  const u8 *pData, int nData      /* Data to decode list-of-rowids from */
){
  int i = 0;
  i64 iRowid = 0;

  while( i<nData ){
    const char *zApp = "";
    u64 iVal;
    i += sqlite3Fts5GetVarint(&pData[i], &iVal);
    iRowid += iVal;

    if( i<nData && pData[i]==0x00 ){
      i++;
      if( i<nData && pData[i]==0x00 ){
        i++;
        zApp = "+";
      }else{
        zApp = "*";
      }
    }

    sqlite3Fts5BufferAppendPrintf(pRc, pBuf, " %lld%s", iRowid, zApp);
  }
}
#endif /* SQLITE_TEST */

#ifdef SQLITE_TEST
/*
** The implementation of user-defined scalar function fts5_decode().
*/
static void fts5DecodeFunction(
  sqlite3_context *pCtx,          /* Function call context */
  int nArg,                       /* Number of args (always 2) */
  sqlite3_value **apVal           /* Function arguments */
){
  i64 iRowid;                     /* Rowid for record being decoded */
  int iSegid,iHeight,iPgno,bDlidx;/* Rowid components */
  const u8 *aBlob; int n;         /* Record to decode */
  u8 *a = 0;
  Fts5Buffer s;                   /* Build up text to return here */
  int rc = SQLITE_OK;             /* Return code */
  sqlite3_int64 nSpace = 0;
  int eDetailNone = (sqlite3_user_data(pCtx)!=0);

  assert( nArg==2 );
  UNUSED_PARAM(nArg);
  memset(&s, 0, sizeof(Fts5Buffer));
  iRowid = sqlite3_value_int64(apVal[0]);

  /* Make a copy of the second argument (a blob) in aBlob[]. The aBlob[]
  ** copy is followed by FTS5_DATA_ZERO_PADDING 0x00 bytes, which prevents
  ** buffer overreads even if the record is corrupt.  */
  n = sqlite3_value_bytes(apVal[1]);
  aBlob = sqlite3_value_blob(apVal[1]);
  nSpace = n + FTS5_DATA_ZERO_PADDING;
  a = (u8*)sqlite3Fts5MallocZero(&rc, nSpace);
  if( a==0 ) goto decode_out;
  if( n>0 ) memcpy(a, aBlob, n);

  fts5DecodeRowid(iRowid, &iSegid, &bDlidx, &iHeight, &iPgno);

  fts5DebugRowid(&rc, &s, iRowid);
  if( bDlidx ){
    Fts5Data dlidx;
    Fts5DlidxLvl lvl;

    dlidx.p = a;
    dlidx.nn = n;

    memset(&lvl, 0, sizeof(Fts5DlidxLvl));
    lvl.pData = &dlidx;
    lvl.iLeafPgno = iPgno;

    for(fts5DlidxLvlNext(&lvl); lvl.bEof==0; fts5DlidxLvlNext(&lvl)){
      sqlite3Fts5BufferAppendPrintf(&rc, &s, 
          " %d(%lld)", lvl.iLeafPgno, lvl.iRowid
      );
    }
  }else if( iSegid==0 ){
    if( iRowid==FTS5_AVERAGES_ROWID ){
      fts5DecodeAverages(&rc, &s, a, n);
    }else{
      fts5DecodeStructure(&rc, &s, a, n);
    }
  }else if( eDetailNone ){
    Fts5Buffer term;              /* Current term read from page */
    int szLeaf;
    int iPgidxOff = szLeaf = fts5GetU16(&a[2]);
    int iTermOff;
    int nKeep = 0;
    int iOff;

    memset(&term, 0, sizeof(Fts5Buffer));

    /* Decode any entries that occur before the first term. */
    if( szLeaf<n ){
      iPgidxOff += fts5GetVarint32(&a[iPgidxOff], iTermOff);
    }else{
      iTermOff = szLeaf;
    }
    fts5DecodeRowidList(&rc, &s, &a[4], iTermOff-4);

    iOff = iTermOff;
    while( iOff<szLeaf ){
      int nAppend;

      /* Read the term data for the next term*/
      iOff += fts5GetVarint32(&a[iOff], nAppend);
      term.n = nKeep;
      fts5BufferAppendBlob(&rc, &term, nAppend, &a[iOff]);
      sqlite3Fts5BufferAppendPrintf(
          &rc, &s, " term=%.*s", term.n, (const char*)term.p
      );
      iOff += nAppend;

      /* Figure out where the doclist for this term ends */
      if( iPgidxOff<n ){
        int nIncr;
        iPgidxOff += fts5GetVarint32(&a[iPgidxOff], nIncr);
        iTermOff += nIncr;
      }else{
        iTermOff = szLeaf;
      }

      fts5DecodeRowidList(&rc, &s, &a[iOff], iTermOff-iOff);
      iOff = iTermOff;
      if( iOff<szLeaf ){
        iOff += fts5GetVarint32(&a[iOff], nKeep);
      }
    }

    fts5BufferFree(&term);
  }else{
    Fts5Buffer term;              /* Current term read from page */
    int szLeaf;                   /* Offset of pgidx in a[] */
    int iPgidxOff;
    int iPgidxPrev = 0;           /* Previous value read from pgidx */
    int iTermOff = 0;
    int iRowidOff = 0;
    int iOff;
    int nDoclist;

    memset(&term, 0, sizeof(Fts5Buffer));

    if( n<4 ){
      sqlite3Fts5BufferSet(&rc, &s, 7, (const u8*)"corrupt");
      goto decode_out;
    }else{
      iRowidOff = fts5GetU16(&a[0]);
      iPgidxOff = szLeaf = fts5GetU16(&a[2]);
      if( iPgidxOff<n ){
        fts5GetVarint32(&a[iPgidxOff], iTermOff);
      }else if( iPgidxOff>n ){
        rc = FTS5_CORRUPT;
        goto decode_out;
      }
    }

    /* Decode the position list tail at the start of the page */
    if( iRowidOff!=0 ){
      iOff = iRowidOff;
    }else if( iTermOff!=0 ){
      iOff = iTermOff;
    }else{
      iOff = szLeaf;
    }
    if( iOff>n ){
      rc = FTS5_CORRUPT;
      goto decode_out;
    }
    fts5DecodePoslist(&rc, &s, &a[4], iOff-4);

    /* Decode any more doclist data that appears on the page before the
    ** first term. */
    nDoclist = (iTermOff ? iTermOff : szLeaf) - iOff;
    if( nDoclist+iOff>n ){
      rc = FTS5_CORRUPT;
      goto decode_out;
    }
    fts5DecodeDoclist(&rc, &s, &a[iOff], nDoclist);

    while( iPgidxOff<n && rc==SQLITE_OK ){
      int bFirst = (iPgidxOff==szLeaf);     /* True for first term on page */
      int nByte;                            /* Bytes of data */
      int iEnd;
      
      iPgidxOff += fts5GetVarint32(&a[iPgidxOff], nByte);
      iPgidxPrev += nByte;
      iOff = iPgidxPrev;

      if( iPgidxOff<n ){
        fts5GetVarint32(&a[iPgidxOff], nByte);
        iEnd = iPgidxPrev + nByte;
      }else{
        iEnd = szLeaf;
      }
      if( iEnd>szLeaf ){
        rc = FTS5_CORRUPT;
        break;
      }

      if( bFirst==0 ){
        iOff += fts5GetVarint32(&a[iOff], nByte);
        if( nByte>term.n ){
          rc = FTS5_CORRUPT;
          break;
        }
        term.n = nByte;
      }
      iOff += fts5GetVarint32(&a[iOff], nByte);
      if( iOff+nByte>n ){
        rc = FTS5_CORRUPT;
        break;
      }
      fts5BufferAppendBlob(&rc, &term, nByte, &a[iOff]);
      iOff += nByte;

      sqlite3Fts5BufferAppendPrintf(
          &rc, &s, " term=%.*s", term.n, (const char*)term.p
      );
      iOff += fts5DecodeDoclist(&rc, &s, &a[iOff], iEnd-iOff);
    }

    fts5BufferFree(&term);
  }
  
 decode_out:
  sqlite3_free(a);
  if( rc==SQLITE_OK ){
    sqlite3_result_text(pCtx, (const char*)s.p, s.n, SQLITE_TRANSIENT);
  }else{
    sqlite3_result_error_code(pCtx, rc);
  }
  fts5BufferFree(&s);
}
#endif /* SQLITE_TEST */

#ifdef SQLITE_TEST 
/*
** The implementation of user-defined scalar function fts5_rowid().
*/
static void fts5RowidFunction(
  sqlite3_context *pCtx,          /* Function call context */
  int nArg,                       /* Number of args (always 2) */
  sqlite3_value **apVal           /* Function arguments */
){
  const char *zArg;
  if( nArg==0 ){
    sqlite3_result_error(pCtx, "should be: fts5_rowid(subject, ....)", -1);
  }else{
    zArg = (const char*)sqlite3_value_text(apVal[0]);
    if( 0==sqlite3_stricmp(zArg, "segment") ){
      i64 iRowid;
      int segid, pgno;
      if( nArg!=3 ){
        sqlite3_result_error(pCtx, 
            "should be: fts5_rowid('segment', segid, pgno))", -1
        );
      }else{
        segid = sqlite3_value_int(apVal[1]);
        pgno = sqlite3_value_int(apVal[2]);
        iRowid = FTS5_SEGMENT_ROWID(segid, pgno);
        sqlite3_result_int64(pCtx, iRowid);
      }
    }else{
      sqlite3_result_error(pCtx, 
        "first arg to fts5_rowid() must be 'segment'" , -1
      );
    }
  }
}
#endif /* SQLITE_TEST */

/*
** This is called as part of registering the FTS5 module with database
** connection db. It registers several user-defined scalar functions useful
** with FTS5.
**
** If successful, SQLITE_OK is returned. If an error occurs, some other
** SQLite error code is returned instead.
*/
static int sqlite3Fts5IndexInit(sqlite3 *db){
#ifdef SQLITE_TEST
  int rc = sqlite3_create_function(
      db, "fts5_decode", 2, SQLITE_UTF8, 0, fts5DecodeFunction, 0, 0
  );

  if( rc==SQLITE_OK ){
    rc = sqlite3_create_function(
        db, "fts5_decode_none", 2, 
        SQLITE_UTF8, (void*)db, fts5DecodeFunction, 0, 0
    );
  }

  if( rc==SQLITE_OK ){
    rc = sqlite3_create_function(
        db, "fts5_rowid", -1, SQLITE_UTF8, 0, fts5RowidFunction, 0, 0
    );
  }
  return rc;
#else
  return SQLITE_OK;
  UNUSED_PARAM(db);
#endif
}


static int sqlite3Fts5IndexReset(Fts5Index *p){
  assert( p->pStruct==0 || p->iStructVersion!=0 );
  if( fts5IndexDataVersion(p)!=p->iStructVersion ){
    fts5StructureInvalidate(p);
  }
  return fts5IndexReturn(p);
}

#line 1 "fts5_main.c"
/*
** 2014 Jun 09
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
******************************************************************************
**
** This is an SQLite module implementing full-text search.
*/


/* #include "fts5Int.h" */

/*
** This variable is set to false when running tests for which the on disk
** structures should not be corrupt. Otherwise, true. If it is false, extra
** assert() conditions in the fts5 code are activated - conditions that are
** only true if it is guaranteed that the fts5 database is not corrupt.
*/
#ifdef SQLITE_DEBUG
int sqlite3_fts5_may_be_corrupt = 1;
#endif


typedef struct Fts5Auxdata Fts5Auxdata;
typedef struct Fts5Auxiliary Fts5Auxiliary;
typedef struct Fts5Cursor Fts5Cursor;
typedef struct Fts5FullTable Fts5FullTable;
typedef struct Fts5Sorter Fts5Sorter;
typedef struct Fts5TokenizerModule Fts5TokenizerModule;

/*
** NOTES ON TRANSACTIONS: 
**
** SQLite invokes the following virtual table methods as transactions are 
** opened and closed by the user:
**
**     xBegin():    Start of a new transaction.
**     xSync():     Initial part of two-phase commit.
**     xCommit():   Final part of two-phase commit.
**     xRollback(): Rollback the transaction.
**
** Anything that is required as part of a commit that may fail is performed
** in the xSync() callback. Current versions of SQLite ignore any errors 
** returned by xCommit().
**
** And as sub-transactions are opened/closed:
**
**     xSavepoint(int S):  Open savepoint S.
**     xRelease(int S):    Commit and close savepoint S.
**     xRollbackTo(int S): Rollback to start of savepoint S.
**
** During a write-transaction the fts5_index.c module may cache some data 
** in-memory. It is flushed to disk whenever xSync(), xRelease() or
** xSavepoint() is called. And discarded whenever xRollback() or xRollbackTo() 
** is called.
**
** Additionally, if SQLITE_DEBUG is defined, an instance of the following
** structure is used to record the current transaction state. This information
** is not required, but it is used in the assert() statements executed by
** function fts5CheckTransactionState() (see below).
*/
struct Fts5TransactionState {
  int eState;                     /* 0==closed, 1==open, 2==synced */
  int iSavepoint;                 /* Number of open savepoints (0 -> none) */
};

/*
** A single object of this type is allocated when the FTS5 module is 
** registered with a database handle. It is used to store pointers to
** all registered FTS5 extensions - tokenizers and auxiliary functions.
*/
struct Fts5Global {
  fts5_api api;                   /* User visible part of object (see fts5.h) */
  sqlite3 *db;                    /* Associated database connection */ 
  i64 iNextId;                    /* Used to allocate unique cursor ids */
  Fts5Auxiliary *pAux;            /* First in list of all aux. functions */
  Fts5TokenizerModule *pTok;      /* First in list of all tokenizer modules */
  Fts5TokenizerModule *pDfltTok;  /* Default tokenizer module */
  Fts5Cursor *pCsr;               /* First in list of all open cursors */
};

/*
** Each auxiliary function registered with the FTS5 module is represented
** by an object of the following type. All such objects are stored as part
** of the Fts5Global.pAux list.
*/
struct Fts5Auxiliary {
  Fts5Global *pGlobal;            /* Global context for this function */
  char *zFunc;                    /* Function name (nul-terminated) */
  void *pUserData;                /* User-data pointer */
  fts5_extension_function xFunc;  /* Callback function */
  void (*xDestroy)(void*);        /* Destructor function */
  Fts5Auxiliary *pNext;           /* Next registered auxiliary function */
};

/*
** Each tokenizer module registered with the FTS5 module is represented
** by an object of the following type. All such objects are stored as part
** of the Fts5Global.pTok list.
*/
struct Fts5TokenizerModule {
  char *zName;                    /* Name of tokenizer */
  void *pUserData;                /* User pointer passed to xCreate() */
  fts5_tokenizer x;               /* Tokenizer functions */
  void (*xDestroy)(void*);        /* Destructor function */
  Fts5TokenizerModule *pNext;     /* Next registered tokenizer module */
};

struct Fts5FullTable {
  Fts5Table p;                    /* Public class members from fts5Int.h */
  Fts5Storage *pStorage;          /* Document store */
  Fts5Global *pGlobal;            /* Global (connection wide) data */
  Fts5Cursor *pSortCsr;           /* Sort data from this cursor */
#ifdef SQLITE_DEBUG
  struct Fts5TransactionState ts;
#endif
};

struct Fts5MatchPhrase {
  Fts5Buffer *pPoslist;           /* Pointer to current poslist */
  int nTerm;                      /* Size of phrase in terms */
};

/*
** pStmt:
**   SELECT rowid, <fts> FROM <fts> ORDER BY +rank;
**
** aIdx[]:
**   There is one entry in the aIdx[] array for each phrase in the query,
**   the value of which is the offset within aPoslist[] following the last 
**   byte of the position list for the corresponding phrase.
*/
struct Fts5Sorter {
  sqlite3_stmt *pStmt;
  i64 iRowid;                     /* Current rowid */
  const u8 *aPoslist;             /* Position lists for current row */
  int nIdx;                       /* Number of entries in aIdx[] */
  int aIdx[1];                    /* Offsets into aPoslist for current row */
};


/*
** Virtual-table cursor object.
**
** iSpecial:
**   If this is a 'special' query (refer to function fts5SpecialMatch()), 
**   then this variable contains the result of the query. 
**
** iFirstRowid, iLastRowid:
**   These variables are only used for FTS5_PLAN_MATCH cursors. Assuming the
**   cursor iterates in ascending order of rowids, iFirstRowid is the lower
**   limit of rowids to return, and iLastRowid the upper. In other words, the
**   WHERE clause in the user's query might have been:
**
**       <tbl> MATCH <expr> AND rowid BETWEEN $iFirstRowid AND $iLastRowid
**
**   If the cursor iterates in descending order of rowid, iFirstRowid
**   is the upper limit (i.e. the "first" rowid visited) and iLastRowid
**   the lower.
*/
struct Fts5Cursor {
  sqlite3_vtab_cursor base;       /* Base class used by SQLite core */
  Fts5Cursor *pNext;              /* Next cursor in Fts5Cursor.pCsr list */
  int *aColumnSize;               /* Values for xColumnSize() */
  i64 iCsrId;                     /* Cursor id */

  /* Zero from this point onwards on cursor reset */
  int ePlan;                      /* FTS5_PLAN_XXX value */
  int bDesc;                      /* True for "ORDER BY rowid DESC" queries */
  i64 iFirstRowid;                /* Return no rowids earlier than this */
  i64 iLastRowid;                 /* Return no rowids later than this */
  sqlite3_stmt *pStmt;            /* Statement used to read %_content */
  Fts5Expr *pExpr;                /* Expression for MATCH queries */
  Fts5Sorter *pSorter;            /* Sorter for "ORDER BY rank" queries */
  int csrflags;                   /* Mask of cursor flags (see below) */
  i64 iSpecial;                   /* Result of special query */

  /* "rank" function. Populated on demand from vtab.xColumn(). */
  char *zRank;                    /* Custom rank function */
  char *zRankArgs;                /* Custom rank function args */
  Fts5Auxiliary *pRank;           /* Rank callback (or NULL) */
  int nRankArg;                   /* Number of trailing arguments for rank() */
  sqlite3_value **apRankArg;      /* Array of trailing arguments */
  sqlite3_stmt *pRankArgStmt;     /* Origin of objects in apRankArg[] */

  /* Auxiliary data storage */
  Fts5Auxiliary *pAux;            /* Currently executing extension function */
  Fts5Auxdata *pAuxdata;          /* First in linked list of saved aux-data */

  /* Cache used by auxiliary functions xInst() and xInstCount() */
  Fts5PoslistReader *aInstIter;   /* One for each phrase */
  int nInstAlloc;                 /* Size of aInst[] array (entries / 3) */
  int nInstCount;                 /* Number of phrase instances */
  int *aInst;                     /* 3 integers per phrase instance */
};

/*
** Bits that make up the "idxNum" parameter passed indirectly by 
** xBestIndex() to xFilter().
*/
#define FTS5_BI_MATCH        0x0001         /* <tbl> MATCH ? */
#define FTS5_BI_RANK         0x0002         /* rank MATCH ? */
#define FTS5_BI_ROWID_EQ     0x0004         /* rowid == ? */
#define FTS5_BI_ROWID_LE     0x0008         /* rowid <= ? */
#define FTS5_BI_ROWID_GE     0x0010         /* rowid >= ? */

#define FTS5_BI_ORDER_RANK   0x0020
#define FTS5_BI_ORDER_ROWID  0x0040
#define FTS5_BI_ORDER_DESC   0x0080

/*
** Values for Fts5Cursor.csrflags
*/
#define FTS5CSR_EOF               0x01
#define FTS5CSR_REQUIRE_CONTENT   0x02
#define FTS5CSR_REQUIRE_DOCSIZE   0x04
#define FTS5CSR_REQUIRE_INST      0x08
#define FTS5CSR_FREE_ZRANK        0x10
#define FTS5CSR_REQUIRE_RESEEK    0x20
#define FTS5CSR_REQUIRE_POSLIST   0x40

#define BitFlagAllTest(x,y) (((x) & (y))==(y))
#define BitFlagTest(x,y)    (((x) & (y))!=0)


/*
** Macros to Set(), Clear() and Test() cursor flags.
*/
#define CsrFlagSet(pCsr, flag)   ((pCsr)->csrflags |= (flag))
#define CsrFlagClear(pCsr, flag) ((pCsr)->csrflags &= ~(flag))
#define CsrFlagTest(pCsr, flag)  ((pCsr)->csrflags & (flag))

struct Fts5Auxdata {
  Fts5Auxiliary *pAux;            /* Extension to which this belongs */
  void *pPtr;                     /* Pointer value */
  void(*xDelete)(void*);          /* Destructor */
  Fts5Auxdata *pNext;             /* Next object in linked list */
};

#ifdef SQLITE_DEBUG
#define FTS5_BEGIN      1
#define FTS5_SYNC       2
#define FTS5_COMMIT     3
#define FTS5_ROLLBACK   4
#define FTS5_SAVEPOINT  5
#define FTS5_RELEASE    6
#define FTS5_ROLLBACKTO 7
static void fts5CheckTransactionState(Fts5FullTable *p, int op, int iSavepoint){
  switch( op ){
    case FTS5_BEGIN:
      assert( p->ts.eState==0 );
      p->ts.eState = 1;
      p->ts.iSavepoint = -1;
      break;

    case FTS5_SYNC:
      assert( p->ts.eState==1 );
      p->ts.eState = 2;
      break;

    case FTS5_COMMIT:
      assert( p->ts.eState==2 );
      p->ts.eState = 0;
      break;

    case FTS5_ROLLBACK:
      assert( p->ts.eState==1 || p->ts.eState==2 || p->ts.eState==0 );
      p->ts.eState = 0;
      break;

    case FTS5_SAVEPOINT:
      assert( p->ts.eState==1 );
      assert( iSavepoint>=0 );
      assert( iSavepoint>=p->ts.iSavepoint );
      p->ts.iSavepoint = iSavepoint;
      break;
      
    case FTS5_RELEASE:
      assert( p->ts.eState==1 );
      assert( iSavepoint>=0 );
      assert( iSavepoint<=p->ts.iSavepoint );
      p->ts.iSavepoint = iSavepoint-1;
      break;

    case FTS5_ROLLBACKTO:
      assert( p->ts.eState==1 );
      assert( iSavepoint>=-1 );
      /* The following assert() can fail if another vtab strikes an error
      ** within an xSavepoint() call then SQLite calls xRollbackTo() - without
      ** having called xSavepoint() on this vtab.  */
      /* assert( iSavepoint<=p->ts.iSavepoint ); */
      p->ts.iSavepoint = iSavepoint;
      break;
  }
}
#else
# define fts5CheckTransactionState(x,y,z)
#endif

/*
** Return true if pTab is a contentless table.
*/
static int fts5IsContentless(Fts5FullTable *pTab){
  return pTab->p.pConfig->eContent==FTS5_CONTENT_NONE;
}

/*
** Delete a virtual table handle allocated by fts5InitVtab(). 
*/
static void fts5FreeVtab(Fts5FullTable *pTab){
  if( pTab ){
    sqlite3Fts5IndexClose(pTab->p.pIndex);
    sqlite3Fts5StorageClose(pTab->pStorage);
    sqlite3Fts5ConfigFree(pTab->p.pConfig);
    sqlite3_free(pTab);
  }
}

/*
** The xDisconnect() virtual table method.
*/
static int fts5DisconnectMethod(sqlite3_vtab *pVtab){
  fts5FreeVtab((Fts5FullTable*)pVtab);
  return SQLITE_OK;
}

/*
** The xDestroy() virtual table method.
*/
static int fts5DestroyMethod(sqlite3_vtab *pVtab){
  Fts5Table *pTab = (Fts5Table*)pVtab;
  int rc = sqlite3Fts5DropAll(pTab->pConfig);
  if( rc==SQLITE_OK ){
    fts5FreeVtab((Fts5FullTable*)pVtab);
  }
  return rc;
}

/*
** This function is the implementation of both the xConnect and xCreate
** methods of the FTS3 virtual table.
**
** The argv[] array contains the following:
**
**   argv[0]   -> module name  ("fts5")
**   argv[1]   -> database name
**   argv[2]   -> table name
**   argv[...] -> "column name" and other module argument fields.
*/
static int fts5InitVtab(
  int bCreate,                    /* True for xCreate, false for xConnect */
  sqlite3 *db,                    /* The SQLite database connection */
  void *pAux,                     /* Hash table containing tokenizers */
  int argc,                       /* Number of elements in argv array */
  const char * const *argv,       /* xCreate/xConnect argument array */
  sqlite3_vtab **ppVTab,          /* Write the resulting vtab structure here */
  char **pzErr                    /* Write any error message here */
){
  Fts5Global *pGlobal = (Fts5Global*)pAux;
  const char **azConfig = (const char**)argv;
  int rc = SQLITE_OK;             /* Return code */
  Fts5Config *pConfig = 0;        /* Results of parsing argc/argv */
  Fts5FullTable *pTab = 0;        /* New virtual table object */

  /* Allocate the new vtab object and parse the configuration */
  pTab = (Fts5FullTable*)sqlite3Fts5MallocZero(&rc, sizeof(Fts5FullTable));
  if( rc==SQLITE_OK ){
    rc = sqlite3Fts5ConfigParse(pGlobal, db, argc, azConfig, &pConfig, pzErr);
    assert( (rc==SQLITE_OK && *pzErr==0) || pConfig==0 );
  }
  if( rc==SQLITE_OK ){
    pTab->p.pConfig = pConfig;
    pTab->pGlobal = pGlobal;
  }

  /* Open the index sub-system */
  if( rc==SQLITE_OK ){
    rc = sqlite3Fts5IndexOpen(pConfig, bCreate, &pTab->p.pIndex, pzErr);
  }

  /* Open the storage sub-system */
  if( rc==SQLITE_OK ){
    rc = sqlite3Fts5StorageOpen(
        pConfig, pTab->p.pIndex, bCreate, &pTab->pStorage, pzErr
    );
  }

  /* Call sqlite3_declare_vtab() */
  if( rc==SQLITE_OK ){
    rc = sqlite3Fts5ConfigDeclareVtab(pConfig);
  }

  /* Load the initial configuration */
  if( rc==SQLITE_OK ){
    assert( pConfig->pzErrmsg==0 );
    pConfig->pzErrmsg = pzErr;
    rc = sqlite3Fts5IndexLoadConfig(pTab->p.pIndex);
    sqlite3Fts5IndexRollback(pTab->p.pIndex);
    pConfig->pzErrmsg = 0;
  }

  if( rc!=SQLITE_OK ){
    fts5FreeVtab(pTab);
    pTab = 0;
  }else if( bCreate ){
    fts5CheckTransactionState(pTab, FTS5_BEGIN, 0);
  }
  *ppVTab = (sqlite3_vtab*)pTab;
  return rc;
}

/*
** The xConnect() and xCreate() methods for the virtual table. All the
** work is done in function fts5InitVtab().
*/
static int fts5ConnectMethod(
  sqlite3 *db,                    /* Database connection */
  void *pAux,                     /* Pointer to tokenizer hash table */
  int argc,                       /* Number of elements in argv array */
  const char * const *argv,       /* xCreate/xConnect argument array */
  sqlite3_vtab **ppVtab,          /* OUT: New sqlite3_vtab object */
  char **pzErr                    /* OUT: sqlite3_malloc'd error message */
){
  return fts5InitVtab(0, db, pAux, argc, argv, ppVtab, pzErr);
}
static int fts5CreateMethod(
  sqlite3 *db,                    /* Database connection */
  void *pAux,                     /* Pointer to tokenizer hash table */
  int argc,                       /* Number of elements in argv array */
  const char * const *argv,       /* xCreate/xConnect argument array */
  sqlite3_vtab **ppVtab,          /* OUT: New sqlite3_vtab object */
  char **pzErr                    /* OUT: sqlite3_malloc'd error message */
){
  return fts5InitVtab(1, db, pAux, argc, argv, ppVtab, pzErr);
}

/*
** The different query plans.
*/
#define FTS5_PLAN_MATCH          1       /* (<tbl> MATCH ?) */
#define FTS5_PLAN_SOURCE         2       /* A source cursor for SORTED_MATCH */
#define FTS5_PLAN_SPECIAL        3       /* An internal query */
#define FTS5_PLAN_SORTED_MATCH   4       /* (<tbl> MATCH ? ORDER BY rank) */
#define FTS5_PLAN_SCAN           5       /* No usable constraint */
#define FTS5_PLAN_ROWID          6       /* (rowid = ?) */

/*
** Set the SQLITE_INDEX_SCAN_UNIQUE flag in pIdxInfo->flags. Unless this
** extension is currently being used by a version of SQLite too old to
** support index-info flags. In that case this function is a no-op.
*/
static void fts5SetUniqueFlag(sqlite3_index_info *pIdxInfo){
#if SQLITE_VERSION_NUMBER>=3008012
#ifndef SQLITE_CORE
  if( sqlite3_libversion_number()>=3008012 )
#endif
  {
    pIdxInfo->idxFlags |= SQLITE_INDEX_SCAN_UNIQUE;
  }
#endif
}

static int fts5UsePatternMatch(
  Fts5Config *pConfig, 
  struct sqlite3_index_constraint *p
){
  assert( FTS5_PATTERN_GLOB==SQLITE_INDEX_CONSTRAINT_GLOB );
  assert( FTS5_PATTERN_LIKE==SQLITE_INDEX_CONSTRAINT_LIKE );
  if( pConfig->ePattern==FTS5_PATTERN_GLOB && p->op==FTS5_PATTERN_GLOB ){
    return 1;
  }
  if( pConfig->ePattern==FTS5_PATTERN_LIKE 
   && (p->op==FTS5_PATTERN_LIKE || p->op==FTS5_PATTERN_GLOB)
  ){
    return 1;
  }
  return 0;
}

/*
** Implementation of the xBestIndex method for FTS5 tables. Within the 
** WHERE constraint, it searches for the following:
**
**   1. A MATCH constraint against the table column.
**   2. A MATCH constraint against the "rank" column.
**   3. A MATCH constraint against some other column.
**   4. An == constraint against the rowid column.
**   5. A < or <= constraint against the rowid column.
**   6. A > or >= constraint against the rowid column.
**
** Within the ORDER BY, the following are supported:
**
**   5. ORDER BY rank [ASC|DESC]
**   6. ORDER BY rowid [ASC|DESC]
**
** Information for the xFilter call is passed via both the idxNum and 
** idxStr variables. Specifically, idxNum is a bitmask of the following
** flags used to encode the ORDER BY clause:
**
**     FTS5_BI_ORDER_RANK
**     FTS5_BI_ORDER_ROWID
**     FTS5_BI_ORDER_DESC
**
** idxStr is used to encode data from the WHERE clause. For each argument
** passed to the xFilter method, the following is appended to idxStr:
**
**   Match against table column:            "m"
**   Match against rank column:             "r"
**   Match against other column:            "M<column-number>"
**   LIKE  against other column:            "L<column-number>"
**   GLOB  against other column:            "G<column-number>"
**   Equality constraint against the rowid: "="
**   A < or <= against the rowid:           "<"
**   A > or >= against the rowid:           ">"
**
** This function ensures that there is at most one "r" or "=". And that if
** there exists an "=" then there is no "<" or ">".
**
** Costs are assigned as follows:
**
**  a) If an unusable MATCH operator is present in the WHERE clause, the
**     cost is unconditionally set to 1e50 (a really big number).
**
**  a) If a MATCH operator is present, the cost depends on the other
**     constraints also present. As follows:
**
**       * No other constraints:         cost=1000.0
**       * One rowid range constraint:   cost=750.0
**       * Both rowid range constraints: cost=500.0
**       * An == rowid constraint:       cost=100.0
**
**  b) Otherwise, if there is no MATCH:
**
**       * No other constraints:         cost=1000000.0
**       * One rowid range constraint:   cost=750000.0
**       * Both rowid range constraints: cost=250000.0
**       * An == rowid constraint:       cost=10.0
**
** Costs are not modified by the ORDER BY clause.
*/
static int fts5BestIndexMethod(sqlite3_vtab *pVTab, sqlite3_index_info *pInfo){
  Fts5Table *pTab = (Fts5Table*)pVTab;
  Fts5Config *pConfig = pTab->pConfig;
  const int nCol = pConfig->nCol;
  int idxFlags = 0;               /* Parameter passed through to xFilter() */
  int i;

  char *idxStr;
  int iIdxStr = 0;
  int iCons = 0;

  int bSeenEq = 0;
  int bSeenGt = 0;
  int bSeenLt = 0;
  int bSeenMatch = 0;
  int bSeenRank = 0;


  assert( SQLITE_INDEX_CONSTRAINT_EQ<SQLITE_INDEX_CONSTRAINT_MATCH );
  assert( SQLITE_INDEX_CONSTRAINT_GT<SQLITE_INDEX_CONSTRAINT_MATCH );
  assert( SQLITE_INDEX_CONSTRAINT_LE<SQLITE_INDEX_CONSTRAINT_MATCH );
  assert( SQLITE_INDEX_CONSTRAINT_GE<SQLITE_INDEX_CONSTRAINT_MATCH );
  assert( SQLITE_INDEX_CONSTRAINT_LE<SQLITE_INDEX_CONSTRAINT_MATCH );

  if( pConfig->bLock ){
    pTab->base.zErrMsg = sqlite3_mprintf(
        "recursively defined fts5 content table"
    );
    return SQLITE_ERROR;
  }

  idxStr = (char*)sqlite3_malloc(pInfo->nConstraint * 8 + 1);
  if( idxStr==0 ) return SQLITE_NOMEM;
  pInfo->idxStr = idxStr;
  pInfo->needToFreeIdxStr = 1;

  for(i=0; i<pInfo->nConstraint; i++){
    struct sqlite3_index_constraint *p = &pInfo->aConstraint[i];
    int iCol = p->iColumn;
    if( p->op==SQLITE_INDEX_CONSTRAINT_MATCH
     || (p->op==SQLITE_INDEX_CONSTRAINT_EQ && iCol>=nCol)
    ){
      /* A MATCH operator or equivalent */
      if( p->usable==0 || iCol<0 ){
        /* As there exists an unusable MATCH constraint this is an 
        ** unusable plan. Set a prohibitively high cost. */
        pInfo->estimatedCost = 1e50;
        assert( iIdxStr < pInfo->nConstraint*6 + 1 );
        idxStr[iIdxStr] = 0;
        return SQLITE_OK;
      }else{
        if( iCol==nCol+1 ){
          if( bSeenRank ) continue;
          idxStr[iIdxStr++] = 'r';
          bSeenRank = 1;
        }else if( iCol>=0 ){
          bSeenMatch = 1;
          idxStr[iIdxStr++] = 'M';
          sqlite3_snprintf(6, &idxStr[iIdxStr], "%d", iCol);
          idxStr += strlen(&idxStr[iIdxStr]);
          assert( idxStr[iIdxStr]=='\0' );
        }
        pInfo->aConstraintUsage[i].argvIndex = ++iCons;
        pInfo->aConstraintUsage[i].omit = 1;
      }
    }else if( p->usable ){
      if( iCol>=0 && iCol<nCol && fts5UsePatternMatch(pConfig, p) ){
        assert( p->op==FTS5_PATTERN_LIKE || p->op==FTS5_PATTERN_GLOB );
        idxStr[iIdxStr++] = p->op==FTS5_PATTERN_LIKE ? 'L' : 'G';
        sqlite3_snprintf(6, &idxStr[iIdxStr], "%d", iCol);
        idxStr += strlen(&idxStr[iIdxStr]);
        pInfo->aConstraintUsage[i].argvIndex = ++iCons;
        assert( idxStr[iIdxStr]=='\0' );
      }else if( bSeenEq==0 && p->op==SQLITE_INDEX_CONSTRAINT_EQ && iCol<0 ){
        idxStr[iIdxStr++] = '=';
        bSeenEq = 1;
        pInfo->aConstraintUsage[i].argvIndex = ++iCons;
      }
    }
  }

  if( bSeenEq==0 ){
    for(i=0; i<pInfo->nConstraint; i++){
      struct sqlite3_index_constraint *p = &pInfo->aConstraint[i];
      if( p->iColumn<0 && p->usable ){
        int op = p->op;
        if( op==SQLITE_INDEX_CONSTRAINT_LT || op==SQLITE_INDEX_CONSTRAINT_LE ){
          if( bSeenLt ) continue;
          idxStr[iIdxStr++] = '<';
          pInfo->aConstraintUsage[i].argvIndex = ++iCons;
          bSeenLt = 1;
        }else
        if( op==SQLITE_INDEX_CONSTRAINT_GT || op==SQLITE_INDEX_CONSTRAINT_GE ){
          if( bSeenGt ) continue;
          idxStr[iIdxStr++] = '>';
          pInfo->aConstraintUsage[i].argvIndex = ++iCons;
          bSeenGt = 1;
        }
      }
    }
  }
  idxStr[iIdxStr] = '\0';

  /* Set idxFlags flags for the ORDER BY clause */
  if( pInfo->nOrderBy==1 ){
    int iSort = pInfo->aOrderBy[0].iColumn;
    if( iSort==(pConfig->nCol+1) && bSeenMatch ){
      idxFlags |= FTS5_BI_ORDER_RANK;
    }else if( iSort==-1 ){
      idxFlags |= FTS5_BI_ORDER_ROWID;
    }
    if( BitFlagTest(idxFlags, FTS5_BI_ORDER_RANK|FTS5_BI_ORDER_ROWID) ){
      pInfo->orderByConsumed = 1;
      if( pInfo->aOrderBy[0].desc ){
        idxFlags |= FTS5_BI_ORDER_DESC;
      }
    }
  }

  /* Calculate the estimated cost based on the flags set in idxFlags. */
  if( bSeenEq ){
    pInfo->estimatedCost = bSeenMatch ? 100.0 : 10.0;
    if( bSeenMatch==0 ) fts5SetUniqueFlag(pInfo);
  }else if( bSeenLt && bSeenGt ){
    pInfo->estimatedCost = bSeenMatch ? 500.0 : 250000.0;
  }else if( bSeenLt || bSeenGt ){
    pInfo->estimatedCost = bSeenMatch ? 750.0 : 750000.0;
  }else{
    pInfo->estimatedCost = bSeenMatch ? 1000.0 : 1000000.0;
  }

  pInfo->idxNum = idxFlags;
  return SQLITE_OK;
}

static int fts5NewTransaction(Fts5FullTable *pTab){
  Fts5Cursor *pCsr;
  for(pCsr=pTab->pGlobal->pCsr; pCsr; pCsr=pCsr->pNext){
    if( pCsr->base.pVtab==(sqlite3_vtab*)pTab ) return SQLITE_OK;
  }
  return sqlite3Fts5StorageReset(pTab->pStorage);
}

/*
** Implementation of xOpen method.
*/
static int fts5OpenMethod(sqlite3_vtab *pVTab, sqlite3_vtab_cursor **ppCsr){
  Fts5FullTable *pTab = (Fts5FullTable*)pVTab;
  Fts5Config *pConfig = pTab->p.pConfig;
  Fts5Cursor *pCsr = 0;           /* New cursor object */
  sqlite3_int64 nByte;            /* Bytes of space to allocate */
  int rc;                         /* Return code */

  rc = fts5NewTransaction(pTab);
  if( rc==SQLITE_OK ){
    nByte = sizeof(Fts5Cursor) + pConfig->nCol * sizeof(int);
    pCsr = (Fts5Cursor*)sqlite3_malloc64(nByte);
    if( pCsr ){
      Fts5Global *pGlobal = pTab->pGlobal;
      memset(pCsr, 0, (size_t)nByte);
      pCsr->aColumnSize = (int*)&pCsr[1];
      pCsr->pNext = pGlobal->pCsr;
      pGlobal->pCsr = pCsr;
      pCsr->iCsrId = ++pGlobal->iNextId;
    }else{
      rc = SQLITE_NOMEM;
    }
  }
  *ppCsr = (sqlite3_vtab_cursor*)pCsr;
  return rc;
}

static int fts5StmtType(Fts5Cursor *pCsr){
  if( pCsr->ePlan==FTS5_PLAN_SCAN ){
    return (pCsr->bDesc) ? FTS5_STMT_SCAN_DESC : FTS5_STMT_SCAN_ASC;
  }
  return FTS5_STMT_LOOKUP;
}

/*
** This function is called after the cursor passed as the only argument
** is moved to point at a different row. It clears all cached data 
** specific to the previous row stored by the cursor object.
*/
static void fts5CsrNewrow(Fts5Cursor *pCsr){
  CsrFlagSet(pCsr, 
      FTS5CSR_REQUIRE_CONTENT 
    | FTS5CSR_REQUIRE_DOCSIZE 
    | FTS5CSR_REQUIRE_INST 
    | FTS5CSR_REQUIRE_POSLIST 
  );
}

static void fts5FreeCursorComponents(Fts5Cursor *pCsr){
  Fts5FullTable *pTab = (Fts5FullTable*)(pCsr->base.pVtab);
  Fts5Auxdata *pData;
  Fts5Auxdata *pNext;

  sqlite3_free(pCsr->aInstIter);
  sqlite3_free(pCsr->aInst);
  if( pCsr->pStmt ){
    int eStmt = fts5StmtType(pCsr);
    sqlite3Fts5StorageStmtRelease(pTab->pStorage, eStmt, pCsr->pStmt);
  }
  if( pCsr->pSorter ){
    Fts5Sorter *pSorter = pCsr->pSorter;
    sqlite3_finalize(pSorter->pStmt);
    sqlite3_free(pSorter);
  }

  if( pCsr->ePlan!=FTS5_PLAN_SOURCE ){
    sqlite3Fts5ExprFree(pCsr->pExpr);
  }

  for(pData=pCsr->pAuxdata; pData; pData=pNext){
    pNext = pData->pNext;
    if( pData->xDelete ) pData->xDelete(pData->pPtr);
    sqlite3_free(pData);
  }

  sqlite3_finalize(pCsr->pRankArgStmt);
  sqlite3_free(pCsr->apRankArg);

  if( CsrFlagTest(pCsr, FTS5CSR_FREE_ZRANK) ){
    sqlite3_free(pCsr->zRank);
    sqlite3_free(pCsr->zRankArgs);
  }

  sqlite3Fts5IndexCloseReader(pTab->p.pIndex);
  memset(&pCsr->ePlan, 0, sizeof(Fts5Cursor) - ((u8*)&pCsr->ePlan - (u8*)pCsr));
}


/*
** Close the cursor.  For additional information see the documentation
** on the xClose method of the virtual table interface.
*/
static int fts5CloseMethod(sqlite3_vtab_cursor *pCursor){
  if( pCursor ){
    Fts5FullTable *pTab = (Fts5FullTable*)(pCursor->pVtab);
    Fts5Cursor *pCsr = (Fts5Cursor*)pCursor;
    Fts5Cursor **pp;

    fts5FreeCursorComponents(pCsr);
    /* Remove the cursor from the Fts5Global.pCsr list */
    for(pp=&pTab->pGlobal->pCsr; (*pp)!=pCsr; pp=&(*pp)->pNext);
    *pp = pCsr->pNext;

    sqlite3_free(pCsr);
  }
  return SQLITE_OK;
}

static int fts5SorterNext(Fts5Cursor *pCsr){
  Fts5Sorter *pSorter = pCsr->pSorter;
  int rc;

  rc = sqlite3_step(pSorter->pStmt);
  if( rc==SQLITE_DONE ){
    rc = SQLITE_OK;
    CsrFlagSet(pCsr, FTS5CSR_EOF|FTS5CSR_REQUIRE_CONTENT);
  }else if( rc==SQLITE_ROW ){
    const u8 *a;
    const u8 *aBlob;
    int nBlob;
    int i;
    int iOff = 0;
    rc = SQLITE_OK;

    pSorter->iRowid = sqlite3_column_int64(pSorter->pStmt, 0);
    nBlob = sqlite3_column_bytes(pSorter->pStmt, 1);
    aBlob = a = sqlite3_column_blob(pSorter->pStmt, 1);

    /* nBlob==0 in detail=none mode. */
    if( nBlob>0 ){
      for(i=0; i<(pSorter->nIdx-1); i++){
        int iVal;
        a += fts5GetVarint32(a, iVal);
        iOff += iVal;
        pSorter->aIdx[i] = iOff;
      }
      pSorter->aIdx[i] = &aBlob[nBlob] - a;
      pSorter->aPoslist = a;
    }

    fts5CsrNewrow(pCsr);
  }

  return rc;
}


/*
** Set the FTS5CSR_REQUIRE_RESEEK flag on all FTS5_PLAN_MATCH cursors 
** open on table pTab.
*/
static void fts5TripCursors(Fts5FullTable *pTab){
  Fts5Cursor *pCsr;
  for(pCsr=pTab->pGlobal->pCsr; pCsr; pCsr=pCsr->pNext){
    if( pCsr->ePlan==FTS5_PLAN_MATCH
     && pCsr->base.pVtab==(sqlite3_vtab*)pTab 
    ){
      CsrFlagSet(pCsr, FTS5CSR_REQUIRE_RESEEK);
    }
  }
}

/*
** If the REQUIRE_RESEEK flag is set on the cursor passed as the first
** argument, close and reopen all Fts5IndexIter iterators that the cursor 
** is using. Then attempt to move the cursor to a rowid equal to or laster
** (in the cursors sort order - ASC or DESC) than the current rowid. 
**
** If the new rowid is not equal to the old, set output parameter *pbSkip
** to 1 before returning. Otherwise, leave it unchanged.
**
** Return SQLITE_OK if successful or if no reseek was required, or an 
** error code if an error occurred.
*/
static int fts5CursorReseek(Fts5Cursor *pCsr, int *pbSkip){
  int rc = SQLITE_OK;
  assert( *pbSkip==0 );
  if( CsrFlagTest(pCsr, FTS5CSR_REQUIRE_RESEEK) ){
    Fts5FullTable *pTab = (Fts5FullTable*)(pCsr->base.pVtab);
    int bDesc = pCsr->bDesc;
    i64 iRowid = sqlite3Fts5ExprRowid(pCsr->pExpr);

    rc = sqlite3Fts5ExprFirst(pCsr->pExpr, pTab->p.pIndex, iRowid, bDesc);
    if( rc==SQLITE_OK &&  iRowid!=sqlite3Fts5ExprRowid(pCsr->pExpr) ){
      *pbSkip = 1;
    }

    CsrFlagClear(pCsr, FTS5CSR_REQUIRE_RESEEK);
    fts5CsrNewrow(pCsr);
    if( sqlite3Fts5ExprEof(pCsr->pExpr) ){
      CsrFlagSet(pCsr, FTS5CSR_EOF);
      *pbSkip = 1;
    }
  }
  return rc;
}


/*
** Advance the cursor to the next row in the table that matches the 
** search criteria.
**
** Return SQLITE_OK if nothing goes wrong.  SQLITE_OK is returned
** even if we reach end-of-file.  The fts5EofMethod() will be called
** subsequently to determine whether or not an EOF was hit.
*/
static int fts5NextMethod(sqlite3_vtab_cursor *pCursor){
  Fts5Cursor *pCsr = (Fts5Cursor*)pCursor;
  int rc;

  assert( (pCsr->ePlan<3)==
          (pCsr->ePlan==FTS5_PLAN_MATCH || pCsr->ePlan==FTS5_PLAN_SOURCE) 
  );
  assert( !CsrFlagTest(pCsr, FTS5CSR_EOF) );

  if( pCsr->ePlan<3 ){
    int bSkip = 0;
    if( (rc = fts5CursorReseek(pCsr, &bSkip)) || bSkip ) return rc;
    rc = sqlite3Fts5ExprNext(pCsr->pExpr, pCsr->iLastRowid);
    CsrFlagSet(pCsr, sqlite3Fts5ExprEof(pCsr->pExpr));
    fts5CsrNewrow(pCsr);
  }else{
    switch( pCsr->ePlan ){
      case FTS5_PLAN_SPECIAL: {
        CsrFlagSet(pCsr, FTS5CSR_EOF);
        rc = SQLITE_OK;
        break;
      }
  
      case FTS5_PLAN_SORTED_MATCH: {
        rc = fts5SorterNext(pCsr);
        break;
      }
  
      default: {
        Fts5Config *pConfig = ((Fts5Table*)pCursor->pVtab)->pConfig;
        pConfig->bLock++;
        rc = sqlite3_step(pCsr->pStmt);
        pConfig->bLock--;
        if( rc!=SQLITE_ROW ){
          CsrFlagSet(pCsr, FTS5CSR_EOF);
          rc = sqlite3_reset(pCsr->pStmt);
          if( rc!=SQLITE_OK ){
            pCursor->pVtab->zErrMsg = sqlite3_mprintf(
                "%s", sqlite3_errmsg(pConfig->db)
            );
          }
        }else{
          rc = SQLITE_OK;
        }
        break;
      }
    }
  }
  
  return rc;
}


static int fts5PrepareStatement(
  sqlite3_stmt **ppStmt,
  Fts5Config *pConfig, 
  const char *zFmt,
  ...
){
  sqlite3_stmt *pRet = 0;
  int rc;
  char *zSql;
  va_list ap;

  va_start(ap, zFmt);
  zSql = sqlite3_vmprintf(zFmt, ap);
  if( zSql==0 ){
    rc = SQLITE_NOMEM; 
  }else{
    rc = sqlite3_prepare_v3(pConfig->db, zSql, -1, 
                            SQLITE_PREPARE_PERSISTENT, &pRet, 0);
    if( rc!=SQLITE_OK ){
      *pConfig->pzErrmsg = sqlite3_mprintf("%s", sqlite3_errmsg(pConfig->db));
    }
    sqlite3_free(zSql);
  }

  va_end(ap);
  *ppStmt = pRet;
  return rc;
} 

static int fts5CursorFirstSorted(
  Fts5FullTable *pTab, 
  Fts5Cursor *pCsr, 
  int bDesc
){
  Fts5Config *pConfig = pTab->p.pConfig;
  Fts5Sorter *pSorter;
  int nPhrase;
  sqlite3_int64 nByte;
  int rc;
  const char *zRank = pCsr->zRank;
  const char *zRankArgs = pCsr->zRankArgs;
  
  nPhrase = sqlite3Fts5ExprPhraseCount(pCsr->pExpr);
  nByte = sizeof(Fts5Sorter) + sizeof(int) * (nPhrase-1);
  pSorter = (Fts5Sorter*)sqlite3_malloc64(nByte);
  if( pSorter==0 ) return SQLITE_NOMEM;
  memset(pSorter, 0, (size_t)nByte);
  pSorter->nIdx = nPhrase;

  /* TODO: It would be better to have some system for reusing statement
  ** handles here, rather than preparing a new one for each query. But that
  ** is not possible as SQLite reference counts the virtual table objects.
  ** And since the statement required here reads from this very virtual 
  ** table, saving it creates a circular reference.
  **
  ** If SQLite a built-in statement cache, this wouldn't be a problem. */
  rc = fts5PrepareStatement(&pSorter->pStmt, pConfig,
      "SELECT rowid, rank FROM %Q.%Q ORDER BY %s(\"%w\"%s%s) %s",
      pConfig->zDb, pConfig->zName, zRank, pConfig->zName,
      (zRankArgs ? ", " : ""),
      (zRankArgs ? zRankArgs : ""),
      bDesc ? "DESC" : "ASC"
  );

  pCsr->pSorter = pSorter;
  if( rc==SQLITE_OK ){
    assert( pTab->pSortCsr==0 );
    pTab->pSortCsr = pCsr;
    rc = fts5SorterNext(pCsr);
    pTab->pSortCsr = 0;
  }

  if( rc!=SQLITE_OK ){
    sqlite3_finalize(pSorter->pStmt);
    sqlite3_free(pSorter);
    pCsr->pSorter = 0;
  }

  return rc;
}

static int fts5CursorFirst(Fts5FullTable *pTab, Fts5Cursor *pCsr, int bDesc){
  int rc;
  Fts5Expr *pExpr = pCsr->pExpr;
  rc = sqlite3Fts5ExprFirst(pExpr, pTab->p.pIndex, pCsr->iFirstRowid, bDesc);
  if( sqlite3Fts5ExprEof(pExpr) ){
    CsrFlagSet(pCsr, FTS5CSR_EOF);
  }
  fts5CsrNewrow(pCsr);
  return rc;
}

/*
** Process a "special" query. A special query is identified as one with a
** MATCH expression that begins with a '*' character. The remainder of
** the text passed to the MATCH operator are used as  the special query
** parameters.
*/
static int fts5SpecialMatch(
  Fts5FullTable *pTab, 
  Fts5Cursor *pCsr, 
  const char *zQuery
){
  int rc = SQLITE_OK;             /* Return code */
  const char *z = zQuery;         /* Special query text */
  int n;                          /* Number of bytes in text at z */

  while( z[0]==' ' ) z++;
  for(n=0; z[n] && z[n]!=' '; n++);

  assert( pTab->p.base.zErrMsg==0 );
  pCsr->ePlan = FTS5_PLAN_SPECIAL;

  if( n==5 && 0==sqlite3_strnicmp("reads", z, n) ){
    pCsr->iSpecial = sqlite3Fts5IndexReads(pTab->p.pIndex);
  }
  else if( n==2 && 0==sqlite3_strnicmp("id", z, n) ){
    pCsr->iSpecial = pCsr->iCsrId;
  }
  else{
    /* An unrecognized directive. Return an error message. */
    pTab->p.base.zErrMsg = sqlite3_mprintf("unknown special query: %.*s", n, z);
    rc = SQLITE_ERROR;
  }

  return rc;
}

/*
** Search for an auxiliary function named zName that can be used with table
** pTab. If one is found, return a pointer to the corresponding Fts5Auxiliary
** structure. Otherwise, if no such function exists, return NULL.
*/
static Fts5Auxiliary *fts5FindAuxiliary(Fts5FullTable *pTab, const char *zName){
  Fts5Auxiliary *pAux;

  for(pAux=pTab->pGlobal->pAux; pAux; pAux=pAux->pNext){
    if( sqlite3_stricmp(zName, pAux->zFunc)==0 ) return pAux;
  }

  /* No function of the specified name was found. Return 0. */
  return 0;
}


static int fts5FindRankFunction(Fts5Cursor *pCsr){
  Fts5FullTable *pTab = (Fts5FullTable*)(pCsr->base.pVtab);
  Fts5Config *pConfig = pTab->p.pConfig;
  int rc = SQLITE_OK;
  Fts5Auxiliary *pAux = 0;
  const char *zRank = pCsr->zRank;
  const char *zRankArgs = pCsr->zRankArgs;

  if( zRankArgs ){
    char *zSql = sqlite3Fts5Mprintf(&rc, "SELECT %s", zRankArgs);
    if( zSql ){
      sqlite3_stmt *pStmt = 0;
      rc = sqlite3_prepare_v3(pConfig->db, zSql, -1,
                              SQLITE_PREPARE_PERSISTENT, &pStmt, 0);
      sqlite3_free(zSql);
      assert( rc==SQLITE_OK || pCsr->pRankArgStmt==0 );
      if( rc==SQLITE_OK ){
        if( SQLITE_ROW==sqlite3_step(pStmt) ){
          sqlite3_int64 nByte;
          pCsr->nRankArg = sqlite3_column_count(pStmt);
          nByte = sizeof(sqlite3_value*)*pCsr->nRankArg;
          pCsr->apRankArg = (sqlite3_value**)sqlite3Fts5MallocZero(&rc, nByte);
          if( rc==SQLITE_OK ){
            int i;
            for(i=0; i<pCsr->nRankArg; i++){
              pCsr->apRankArg[i] = sqlite3_column_value(pStmt, i);
            }
          }
          pCsr->pRankArgStmt = pStmt;
        }else{
          rc = sqlite3_finalize(pStmt);
          assert( rc!=SQLITE_OK );
        }
      }
    }
  }

  if( rc==SQLITE_OK ){
    pAux = fts5FindAuxiliary(pTab, zRank);
    if( pAux==0 ){
      assert( pTab->p.base.zErrMsg==0 );
      pTab->p.base.zErrMsg = sqlite3_mprintf("no such function: %s", zRank);
      rc = SQLITE_ERROR;
    }
  }

  pCsr->pRank = pAux;
  return rc;
}


static int fts5CursorParseRank(
  Fts5Config *pConfig,
  Fts5Cursor *pCsr, 
  sqlite3_value *pRank
){
  int rc = SQLITE_OK;
  if( pRank ){
    const char *z = (const char*)sqlite3_value_text(pRank);
    char *zRank = 0;
    char *zRankArgs = 0;

    if( z==0 ){
      if( sqlite3_value_type(pRank)==SQLITE_NULL ) rc = SQLITE_ERROR;
    }else{
      rc = sqlite3Fts5ConfigParseRank(z, &zRank, &zRankArgs);
    }
    if( rc==SQLITE_OK ){
      pCsr->zRank = zRank;
      pCsr->zRankArgs = zRankArgs;
      CsrFlagSet(pCsr, FTS5CSR_FREE_ZRANK);
    }else if( rc==SQLITE_ERROR ){
      pCsr->base.pVtab->zErrMsg = sqlite3_mprintf(
          "parse error in rank function: %s", z
      );
    }
  }else{
    if( pConfig->zRank ){
      pCsr->zRank = (char*)pConfig->zRank;
      pCsr->zRankArgs = (char*)pConfig->zRankArgs;
    }else{
      pCsr->zRank = (char*)FTS5_DEFAULT_RANK;
      pCsr->zRankArgs = 0;
    }
  }
  return rc;
}

static i64 fts5GetRowidLimit(sqlite3_value *pVal, i64 iDefault){
  if( pVal ){
    int eType = sqlite3_value_numeric_type(pVal);
    if( eType==SQLITE_INTEGER ){
      return sqlite3_value_int64(pVal);
    }
  }
  return iDefault;
}

/*
** This is the xFilter interface for the virtual table.  See
** the virtual table xFilter method documentation for additional
** information.
** 
** There are three possible query strategies:
**
**   1. Full-text search using a MATCH operator.
**   2. A by-rowid lookup.
**   3. A full-table scan.
*/
static int fts5FilterMethod(
  sqlite3_vtab_cursor *pCursor,   /* The cursor used for this query */
  int idxNum,                     /* Strategy index */
  const char *idxStr,             /* Unused */
  int nVal,                       /* Number of elements in apVal */
  sqlite3_value **apVal           /* Arguments for the indexing scheme */
){
  Fts5FullTable *pTab = (Fts5FullTable*)(pCursor->pVtab);
  Fts5Config *pConfig = pTab->p.pConfig;
  Fts5Cursor *pCsr = (Fts5Cursor*)pCursor;
  int rc = SQLITE_OK;             /* Error code */
  int bDesc;                      /* True if ORDER BY [rank|rowid] DESC */
  int bOrderByRank;               /* True if ORDER BY rank */
  sqlite3_value *pRank = 0;       /* rank MATCH ? expression (or NULL) */
  sqlite3_value *pRowidEq = 0;    /* rowid = ? expression (or NULL) */
  sqlite3_value *pRowidLe = 0;    /* rowid <= ? expression (or NULL) */
  sqlite3_value *pRowidGe = 0;    /* rowid >= ? expression (or NULL) */
  int iCol;                       /* Column on LHS of MATCH operator */
  char **pzErrmsg = pConfig->pzErrmsg;
  int i;
  int iIdxStr = 0;
  Fts5Expr *pExpr = 0;

  if( pConfig->bLock ){
    pTab->p.base.zErrMsg = sqlite3_mprintf(
        "recursively defined fts5 content table"
    );
    return SQLITE_ERROR;
  }

  if( pCsr->ePlan ){
    fts5FreeCursorComponents(pCsr);
    memset(&pCsr->ePlan, 0, sizeof(Fts5Cursor) - ((u8*)&pCsr->ePlan-(u8*)pCsr));
  }

  assert( pCsr->pStmt==0 );
  assert( pCsr->pExpr==0 );
  assert( pCsr->csrflags==0 );
  assert( pCsr->pRank==0 );
  assert( pCsr->zRank==0 );
  assert( pCsr->zRankArgs==0 );
  assert( pTab->pSortCsr==0 || nVal==0 );

  assert( pzErrmsg==0 || pzErrmsg==&pTab->p.base.zErrMsg );
  pConfig->pzErrmsg = &pTab->p.base.zErrMsg;

  /* Decode the arguments passed through to this function. */
  for(i=0; i<nVal; i++){
    switch( idxStr[iIdxStr++] ){
      case 'r':
        pRank = apVal[i];
        break;
      case 'M': {
        const char *zText = (const char*)sqlite3_value_text(apVal[i]);
        if( zText==0 ) zText = "";
        iCol = 0;
        do{
          iCol = iCol*10 + (idxStr[iIdxStr]-'0');
          iIdxStr++;
        }while( idxStr[iIdxStr]>='0' && idxStr[iIdxStr]<='9' );

        if( zText[0]=='*' ){
          /* The user has issued a query of the form "MATCH '*...'". This
          ** indicates that the MATCH expression is not a full text query,
          ** but a request for an internal parameter.  */
          rc = fts5SpecialMatch(pTab, pCsr, &zText[1]);
          goto filter_out;
        }else{
          char **pzErr = &pTab->p.base.zErrMsg;
          rc = sqlite3Fts5ExprNew(pConfig, 0, iCol, zText, &pExpr, pzErr);
          if( rc==SQLITE_OK ){
            rc = sqlite3Fts5ExprAnd(&pCsr->pExpr, pExpr);
            pExpr = 0;
          }
          if( rc!=SQLITE_OK ) goto filter_out;
        }

        break;
      }
      case 'L':
      case 'G': {
        int bGlob = (idxStr[iIdxStr-1]=='G');
        const char *zText = (const char*)sqlite3_value_text(apVal[i]);
        iCol = 0;
        do{
          iCol = iCol*10 + (idxStr[iIdxStr]-'0');
          iIdxStr++;
        }while( idxStr[iIdxStr]>='0' && idxStr[iIdxStr]<='9' );
        if( zText ){
          rc = sqlite3Fts5ExprPattern(pConfig, bGlob, iCol, zText, &pExpr);
        }
        if( rc==SQLITE_OK ){
          rc = sqlite3Fts5ExprAnd(&pCsr->pExpr, pExpr);
          pExpr = 0;
        }
        if( rc!=SQLITE_OK ) goto filter_out;
        break;
      }
      case '=':
        pRowidEq = apVal[i];
        break;
      case '<':
        pRowidLe = apVal[i];
        break;
      default: assert( idxStr[iIdxStr-1]=='>' );
        pRowidGe = apVal[i];
        break;
    }
  }
  bOrderByRank = ((idxNum & FTS5_BI_ORDER_RANK) ? 1 : 0);
  pCsr->bDesc = bDesc = ((idxNum & FTS5_BI_ORDER_DESC) ? 1 : 0);

  /* Set the cursor upper and lower rowid limits. Only some strategies 
  ** actually use them. This is ok, as the xBestIndex() method leaves the
  ** sqlite3_index_constraint.omit flag clear for range constraints
  ** on the rowid field.  */
  if( pRowidEq ){
    pRowidLe = pRowidGe = pRowidEq;
  }
  if( bDesc ){
    pCsr->iFirstRowid = fts5GetRowidLimit(pRowidLe, LARGEST_INT64);
    pCsr->iLastRowid = fts5GetRowidLimit(pRowidGe, SMALLEST_INT64);
  }else{
    pCsr->iLastRowid = fts5GetRowidLimit(pRowidLe, LARGEST_INT64);
    pCsr->iFirstRowid = fts5GetRowidLimit(pRowidGe, SMALLEST_INT64);
  }

  if( pTab->pSortCsr ){
    /* If pSortCsr is non-NULL, then this call is being made as part of 
    ** processing for a "... MATCH <expr> ORDER BY rank" query (ePlan is
    ** set to FTS5_PLAN_SORTED_MATCH). pSortCsr is the cursor that will
    ** return results to the user for this query. The current cursor 
    ** (pCursor) is used to execute the query issued by function 
    ** fts5CursorFirstSorted() above.  */
    assert( pRowidEq==0 && pRowidLe==0 && pRowidGe==0 && pRank==0 );
    assert( nVal==0 && bOrderByRank==0 && bDesc==0 );
    assert( pCsr->iLastRowid==LARGEST_INT64 );
    assert( pCsr->iFirstRowid==SMALLEST_INT64 );
    if( pTab->pSortCsr->bDesc ){
      pCsr->iLastRowid = pTab->pSortCsr->iFirstRowid;
      pCsr->iFirstRowid = pTab->pSortCsr->iLastRowid;
    }else{
      pCsr->iLastRowid = pTab->pSortCsr->iLastRowid;
      pCsr->iFirstRowid = pTab->pSortCsr->iFirstRowid;
    }
    pCsr->ePlan = FTS5_PLAN_SOURCE;
    pCsr->pExpr = pTab->pSortCsr->pExpr;
    rc = fts5CursorFirst(pTab, pCsr, bDesc);
  }else if( pCsr->pExpr ){
    rc = fts5CursorParseRank(pConfig, pCsr, pRank);
    if( rc==SQLITE_OK ){
      if( bOrderByRank ){
        pCsr->ePlan = FTS5_PLAN_SORTED_MATCH;
        rc = fts5CursorFirstSorted(pTab, pCsr, bDesc);
      }else{
        pCsr->ePlan = FTS5_PLAN_MATCH;
        rc = fts5CursorFirst(pTab, pCsr, bDesc);
      }
    }
  }else if( pConfig->zContent==0 ){
    *pConfig->pzErrmsg = sqlite3_mprintf(
        "%s: table does not support scanning", pConfig->zName
    );
    rc = SQLITE_ERROR;
  }else{
    /* This is either a full-table scan (ePlan==FTS5_PLAN_SCAN) or a lookup
    ** by rowid (ePlan==FTS5_PLAN_ROWID).  */
    pCsr->ePlan = (pRowidEq ? FTS5_PLAN_ROWID : FTS5_PLAN_SCAN);
    rc = sqlite3Fts5StorageStmt(
        pTab->pStorage, fts5StmtType(pCsr), &pCsr->pStmt, &pTab->p.base.zErrMsg
    );
    if( rc==SQLITE_OK ){
      if( pRowidEq!=0 ){
        assert( pCsr->ePlan==FTS5_PLAN_ROWID );
        sqlite3_bind_value(pCsr->pStmt, 1, pRowidEq);
      }else{
        sqlite3_bind_int64(pCsr->pStmt, 1, pCsr->iFirstRowid);
        sqlite3_bind_int64(pCsr->pStmt, 2, pCsr->iLastRowid);
      }
      rc = fts5NextMethod(pCursor);
    }
  }

 filter_out:
  sqlite3Fts5ExprFree(pExpr);
  pConfig->pzErrmsg = pzErrmsg;
  return rc;
}

/* 
** This is the xEof method of the virtual table. SQLite calls this 
** routine to find out if it has reached the end of a result set.
*/
static int fts5EofMethod(sqlite3_vtab_cursor *pCursor){
  Fts5Cursor *pCsr = (Fts5Cursor*)pCursor;
  return (CsrFlagTest(pCsr, FTS5CSR_EOF) ? 1 : 0);
}

/*
** Return the rowid that the cursor currently points to.
*/
static i64 fts5CursorRowid(Fts5Cursor *pCsr){
  assert( pCsr->ePlan==FTS5_PLAN_MATCH 
       || pCsr->ePlan==FTS5_PLAN_SORTED_MATCH 
       || pCsr->ePlan==FTS5_PLAN_SOURCE 
  );
  if( pCsr->pSorter ){
    return pCsr->pSorter->iRowid;
  }else{
    return sqlite3Fts5ExprRowid(pCsr->pExpr);
  }
}

/* 
** This is the xRowid method. The SQLite core calls this routine to
** retrieve the rowid for the current row of the result set. fts5
** exposes %_content.rowid as the rowid for the virtual table. The
** rowid should be written to *pRowid.
*/
static int fts5RowidMethod(sqlite3_vtab_cursor *pCursor, sqlite_int64 *pRowid){
  Fts5Cursor *pCsr = (Fts5Cursor*)pCursor;
  int ePlan = pCsr->ePlan;
  
  assert( CsrFlagTest(pCsr, FTS5CSR_EOF)==0 );
  switch( ePlan ){
    case FTS5_PLAN_SPECIAL:
      *pRowid = 0;
      break;

    case FTS5_PLAN_SOURCE:
    case FTS5_PLAN_MATCH:
    case FTS5_PLAN_SORTED_MATCH:
      *pRowid = fts5CursorRowid(pCsr);
      break;

    default:
      *pRowid = sqlite3_column_int64(pCsr->pStmt, 0);
      break;
  }

  return SQLITE_OK;
}

/*
** If the cursor requires seeking (bSeekRequired flag is set), seek it.
** Return SQLITE_OK if no error occurs, or an SQLite error code otherwise.
**
** If argument bErrormsg is true and an error occurs, an error message may
** be left in sqlite3_vtab.zErrMsg.
*/
static int fts5SeekCursor(Fts5Cursor *pCsr, int bErrormsg){
  int rc = SQLITE_OK;

  /* If the cursor does not yet have a statement handle, obtain one now. */ 
  if( pCsr->pStmt==0 ){
    Fts5FullTable *pTab = (Fts5FullTable*)(pCsr->base.pVtab);
    int eStmt = fts5StmtType(pCsr);
    rc = sqlite3Fts5StorageStmt(
        pTab->pStorage, eStmt, &pCsr->pStmt, (bErrormsg?&pTab->p.base.zErrMsg:0)
    );
    assert( rc!=SQLITE_OK || pTab->p.base.zErrMsg==0 );
    assert( CsrFlagTest(pCsr, FTS5CSR_REQUIRE_CONTENT) );
  }

  if( rc==SQLITE_OK && CsrFlagTest(pCsr, FTS5CSR_REQUIRE_CONTENT) ){
    Fts5Table *pTab = (Fts5Table*)(pCsr->base.pVtab);
    assert( pCsr->pExpr );
    sqlite3_reset(pCsr->pStmt);
    sqlite3_bind_int64(pCsr->pStmt, 1, fts5CursorRowid(pCsr));
    pTab->pConfig->bLock++;
    rc = sqlite3_step(pCsr->pStmt);
    pTab->pConfig->bLock--;
    if( rc==SQLITE_ROW ){
      rc = SQLITE_OK;
      CsrFlagClear(pCsr, FTS5CSR_REQUIRE_CONTENT);
    }else{
      rc = sqlite3_reset(pCsr->pStmt);
      if( rc==SQLITE_OK ){
        rc = FTS5_CORRUPT;
      }else if( pTab->pConfig->pzErrmsg ){
        *pTab->pConfig->pzErrmsg = sqlite3_mprintf(
            "%s", sqlite3_errmsg(pTab->pConfig->db)
        );
      }
    }
  }
  return rc;
}

static void fts5SetVtabError(Fts5FullTable *p, const char *zFormat, ...){
  va_list ap;                     /* ... printf arguments */
  va_start(ap, zFormat);
  assert( p->p.base.zErrMsg==0 );
  p->p.base.zErrMsg = sqlite3_vmprintf(zFormat, ap);
  va_end(ap);
}

/*
** This function is called to handle an FTS INSERT command. In other words,
** an INSERT statement of the form:
**
**     INSERT INTO fts(fts) VALUES($pCmd)
**     INSERT INTO fts(fts, rank) VALUES($pCmd, $pVal)
**
** Argument pVal is the value assigned to column "fts" by the INSERT 
** statement. This function returns SQLITE_OK if successful, or an SQLite
** error code if an error occurs.
**
** The commands implemented by this function are documented in the "Special
** INSERT Directives" section of the documentation. It should be updated if
** more commands are added to this function.
*/
static int fts5SpecialInsert(
  Fts5FullTable *pTab,            /* Fts5 table object */
  const char *zCmd,               /* Text inserted into table-name column */
  sqlite3_value *pVal             /* Value inserted into rank column */
){
  Fts5Config *pConfig = pTab->p.pConfig;
  int rc = SQLITE_OK;
  int bError = 0;

  if( 0==sqlite3_stricmp("delete-all", zCmd) ){
    if( pConfig->eContent==FTS5_CONTENT_NORMAL ){
      fts5SetVtabError(pTab, 
          "'delete-all' may only be used with a "
          "contentless or external content fts5 table"
      );
      rc = SQLITE_ERROR;
    }else{
      rc = sqlite3Fts5StorageDeleteAll(pTab->pStorage);
    }
  }else if( 0==sqlite3_stricmp("rebuild", zCmd) ){
    if( pConfig->eContent==FTS5_CONTENT_NONE ){
      fts5SetVtabError(pTab, 
          "'rebuild' may not be used with a contentless fts5 table"
      );
      rc = SQLITE_ERROR;
    }else{
      rc = sqlite3Fts5StorageRebuild(pTab->pStorage);
    }
  }else if( 0==sqlite3_stricmp("optimize", zCmd) ){
    rc = sqlite3Fts5StorageOptimize(pTab->pStorage);
  }else if( 0==sqlite3_stricmp("merge", zCmd) ){
    int nMerge = sqlite3_value_int(pVal);
    rc = sqlite3Fts5StorageMerge(pTab->pStorage, nMerge);
  }else if( 0==sqlite3_stricmp("integrity-check", zCmd) ){
    int iArg = sqlite3_value_int(pVal);
    rc = sqlite3Fts5StorageIntegrity(pTab->pStorage, iArg);
#ifdef SQLITE_DEBUG
  }else if( 0==sqlite3_stricmp("prefix-index", zCmd) ){
    pConfig->bPrefixIndex = sqlite3_value_int(pVal);
#endif
  }else{
    rc = sqlite3Fts5IndexLoadConfig(pTab->p.pIndex);
    if( rc==SQLITE_OK ){
      rc = sqlite3Fts5ConfigSetValue(pTab->p.pConfig, zCmd, pVal, &bError);
    }
    if( rc==SQLITE_OK ){
      if( bError ){
        rc = SQLITE_ERROR;
      }else{
        rc = sqlite3Fts5StorageConfigValue(pTab->pStorage, zCmd, pVal, 0);
      }
    }
  }
  return rc;
}

static int fts5SpecialDelete(
  Fts5FullTable *pTab, 
  sqlite3_value **apVal
){
  int rc = SQLITE_OK;
  int eType1 = sqlite3_value_type(apVal[1]);
  if( eType1==SQLITE_INTEGER ){
    sqlite3_int64 iDel = sqlite3_value_int64(apVal[1]);
    rc = sqlite3Fts5StorageDelete(pTab->pStorage, iDel, &apVal[2]);
  }
  return rc;
}

static void fts5StorageInsert(
  int *pRc, 
  Fts5FullTable *pTab, 
  sqlite3_value **apVal, 
  i64 *piRowid
){
  int rc = *pRc;
  if( rc==SQLITE_OK ){
    rc = sqlite3Fts5StorageContentInsert(pTab->pStorage, apVal, piRowid);
  }
  if( rc==SQLITE_OK ){
    rc = sqlite3Fts5StorageIndexInsert(pTab->pStorage, apVal, *piRowid);
  }
  *pRc = rc;
}

/* 
** This function is the implementation of the xUpdate callback used by 
** FTS3 virtual tables. It is invoked by SQLite each time a row is to be
** inserted, updated or deleted.
**
** A delete specifies a single argument - the rowid of the row to remove.
** 
** Update and insert operations pass:
**
**   1. The "old" rowid, or NULL.
**   2. The "new" rowid.
**   3. Values for each of the nCol matchable columns.
**   4. Values for the two hidden columns (<tablename> and "rank").
*/
static int fts5UpdateMethod(
  sqlite3_vtab *pVtab,            /* Virtual table handle */
  int nArg,                       /* Size of argument array */
  sqlite3_value **apVal,          /* Array of arguments */
  sqlite_int64 *pRowid            /* OUT: The affected (or effected) rowid */
){
  Fts5FullTable *pTab = (Fts5FullTable*)pVtab;
  Fts5Config *pConfig = pTab->p.pConfig;
  int eType0;                     /* value_type() of apVal[0] */
  int rc = SQLITE_OK;             /* Return code */

  /* A transaction must be open when this is called. */
  assert( pTab->ts.eState==1 );

  assert( pVtab->zErrMsg==0 );
  assert( nArg==1 || nArg==(2+pConfig->nCol+2) );
  assert( sqlite3_value_type(apVal[0])==SQLITE_INTEGER 
       || sqlite3_value_type(apVal[0])==SQLITE_NULL 
  );
  assert( pTab->p.pConfig->pzErrmsg==0 );
  pTab->p.pConfig->pzErrmsg = &pTab->p.base.zErrMsg;

  /* Put any active cursors into REQUIRE_SEEK state. */
  fts5TripCursors(pTab);

  eType0 = sqlite3_value_type(apVal[0]);
  if( eType0==SQLITE_NULL 
   && sqlite3_value_type(apVal[2+pConfig->nCol])!=SQLITE_NULL 
  ){
    /* A "special" INSERT op. These are handled separately. */
    const char *z = (const char*)sqlite3_value_text(apVal[2+pConfig->nCol]);
    if( pConfig->eContent!=FTS5_CONTENT_NORMAL 
      && 0==sqlite3_stricmp("delete", z) 
    ){
      rc = fts5SpecialDelete(pTab, apVal);
    }else{
      rc = fts5SpecialInsert(pTab, z, apVal[2 + pConfig->nCol + 1]);
    }
  }else{
    /* A regular INSERT, UPDATE or DELETE statement. The trick here is that
    ** any conflict on the rowid value must be detected before any 
    ** modifications are made to the database file. There are 4 cases:
    **
    **   1) DELETE
    **   2) UPDATE (rowid not modified)
    **   3) UPDATE (rowid modified)
    **   4) INSERT
    **
    ** Cases 3 and 4 may violate the rowid constraint.
    */
    int eConflict = SQLITE_ABORT;
    if( pConfig->eContent==FTS5_CONTENT_NORMAL ){
      eConflict = sqlite3_vtab_on_conflict(pConfig->db);
    }

    assert( eType0==SQLITE_INTEGER || eType0==SQLITE_NULL );
    assert( nArg!=1 || eType0==SQLITE_INTEGER );

    /* Filter out attempts to run UPDATE or DELETE on contentless tables.
    ** This is not suported.  */
    if( eType0==SQLITE_INTEGER && fts5IsContentless(pTab) ){
      pTab->p.base.zErrMsg = sqlite3_mprintf(
          "cannot %s contentless fts5 table: %s", 
          (nArg>1 ? "UPDATE" : "DELETE from"), pConfig->zName
      );
      rc = SQLITE_ERROR;
    }

    /* DELETE */
    else if( nArg==1 ){
      i64 iDel = sqlite3_value_int64(apVal[0]);  /* Rowid to delete */
      rc = sqlite3Fts5StorageDelete(pTab->pStorage, iDel, 0);
    }

    /* INSERT or UPDATE */
    else{
      int eType1 = sqlite3_value_numeric_type(apVal[1]);

      if( eType1!=SQLITE_INTEGER && eType1!=SQLITE_NULL ){
        rc = SQLITE_MISMATCH;
      }

      else if( eType0!=SQLITE_INTEGER ){     
        /* If this is a REPLACE, first remove the current entry (if any) */
        if( eConflict==SQLITE_REPLACE && eType1==SQLITE_INTEGER ){
          i64 iNew = sqlite3_value_int64(apVal[1]);  /* Rowid to delete */
          rc = sqlite3Fts5StorageDelete(pTab->pStorage, iNew, 0);
        }
        fts5StorageInsert(&rc, pTab, apVal, pRowid);
      }

      /* UPDATE */
      else{
        i64 iOld = sqlite3_value_int64(apVal[0]);  /* Old rowid */
        i64 iNew = sqlite3_value_int64(apVal[1]);  /* New rowid */
        if( eType1==SQLITE_INTEGER && iOld!=iNew ){
          if( eConflict==SQLITE_REPLACE ){
            rc = sqlite3Fts5StorageDelete(pTab->pStorage, iOld, 0);
            if( rc==SQLITE_OK ){
              rc = sqlite3Fts5StorageDelete(pTab->pStorage, iNew, 0);
            }
            fts5StorageInsert(&rc, pTab, apVal, pRowid);
          }else{
            rc = sqlite3Fts5StorageContentInsert(pTab->pStorage, apVal, pRowid);
            if( rc==SQLITE_OK ){
              rc = sqlite3Fts5StorageDelete(pTab->pStorage, iOld, 0);
            }
            if( rc==SQLITE_OK ){
              rc = sqlite3Fts5StorageIndexInsert(pTab->pStorage, apVal,*pRowid);
            }
          }
        }else{
          rc = sqlite3Fts5StorageDelete(pTab->pStorage, iOld, 0);
          fts5StorageInsert(&rc, pTab, apVal, pRowid);
        }
      }
    }
  }

  pTab->p.pConfig->pzErrmsg = 0;
  return rc;
}

/*
** Implementation of xSync() method. 
*/
static int fts5SyncMethod(sqlite3_vtab *pVtab){
  int rc;
  Fts5FullTable *pTab = (Fts5FullTable*)pVtab;
  fts5CheckTransactionState(pTab, FTS5_SYNC, 0);
  pTab->p.pConfig->pzErrmsg = &pTab->p.base.zErrMsg;
  fts5TripCursors(pTab);
  rc = sqlite3Fts5StorageSync(pTab->pStorage);
  pTab->p.pConfig->pzErrmsg = 0;
  return rc;
}

/*
** Implementation of xBegin() method. 
*/
static int fts5BeginMethod(sqlite3_vtab *pVtab){
  fts5CheckTransactionState((Fts5FullTable*)pVtab, FTS5_BEGIN, 0);
  fts5NewTransaction((Fts5FullTable*)pVtab);
  return SQLITE_OK;
}

/*
** Implementation of xCommit() method. This is a no-op. The contents of
** the pending-terms hash-table have already been flushed into the database
** by fts5SyncMethod().
*/
static int fts5CommitMethod(sqlite3_vtab *pVtab){
  UNUSED_PARAM(pVtab);  /* Call below is a no-op for NDEBUG builds */
  fts5CheckTransactionState((Fts5FullTable*)pVtab, FTS5_COMMIT, 0);
  return SQLITE_OK;
}

/*
** Implementation of xRollback(). Discard the contents of the pending-terms
** hash-table. Any changes made to the database are reverted by SQLite.
*/
static int fts5RollbackMethod(sqlite3_vtab *pVtab){
  int rc;
  Fts5FullTable *pTab = (Fts5FullTable*)pVtab;
  fts5CheckTransactionState(pTab, FTS5_ROLLBACK, 0);
  rc = sqlite3Fts5StorageRollback(pTab->pStorage);
  return rc;
}

static int fts5CsrPoslist(Fts5Cursor*, int, const u8**, int*);

static void *fts5ApiUserData(Fts5Context *pCtx){
  Fts5Cursor *pCsr = (Fts5Cursor*)pCtx;
  return pCsr->pAux->pUserData;
}

static int fts5ApiColumnCount(Fts5Context *pCtx){
  Fts5Cursor *pCsr = (Fts5Cursor*)pCtx;
  return ((Fts5Table*)(pCsr->base.pVtab))->pConfig->nCol;
}

static int fts5ApiColumnTotalSize(
  Fts5Context *pCtx, 
  int iCol, 
  sqlite3_int64 *pnToken
){
  Fts5Cursor *pCsr = (Fts5Cursor*)pCtx;
  Fts5FullTable *pTab = (Fts5FullTable*)(pCsr->base.pVtab);
  return sqlite3Fts5StorageSize(pTab->pStorage, iCol, pnToken);
}

static int fts5ApiRowCount(Fts5Context *pCtx, i64 *pnRow){
  Fts5Cursor *pCsr = (Fts5Cursor*)pCtx;
  Fts5FullTable *pTab = (Fts5FullTable*)(pCsr->base.pVtab);
  return sqlite3Fts5StorageRowCount(pTab->pStorage, pnRow);
}

static int fts5ApiTokenize(
  Fts5Context *pCtx, 
  const char *pText, int nText, 
  void *pUserData,
  int (*xToken)(void*, int, const char*, int, int, int)
){
  Fts5Cursor *pCsr = (Fts5Cursor*)pCtx;
  Fts5Table *pTab = (Fts5Table*)(pCsr->base.pVtab);
  return sqlite3Fts5Tokenize(
      pTab->pConfig, FTS5_TOKENIZE_AUX, pText, nText, pUserData, xToken
  );
}

static int fts5ApiPhraseCount(Fts5Context *pCtx){
  Fts5Cursor *pCsr = (Fts5Cursor*)pCtx;
  return sqlite3Fts5ExprPhraseCount(pCsr->pExpr);
}

static int fts5ApiPhraseSize(Fts5Context *pCtx, int iPhrase){
  Fts5Cursor *pCsr = (Fts5Cursor*)pCtx;
  return sqlite3Fts5ExprPhraseSize(pCsr->pExpr, iPhrase);
}

static int fts5ApiColumnText(
  Fts5Context *pCtx, 
  int iCol, 
  const char **pz, 
  int *pn
){
  int rc = SQLITE_OK;
  Fts5Cursor *pCsr = (Fts5Cursor*)pCtx;
  if( fts5IsContentless((Fts5FullTable*)(pCsr->base.pVtab)) 
   || pCsr->ePlan==FTS5_PLAN_SPECIAL 
  ){
    *pz = 0;
    *pn = 0;
  }else{
    rc = fts5SeekCursor(pCsr, 0);
    if( rc==SQLITE_OK ){
      *pz = (const char*)sqlite3_column_text(pCsr->pStmt, iCol+1);
      *pn = sqlite3_column_bytes(pCsr->pStmt, iCol+1);
    }
  }
  return rc;
}

static int fts5CsrPoslist(
  Fts5Cursor *pCsr, 
  int iPhrase, 
  const u8 **pa,
  int *pn
){
  Fts5Config *pConfig = ((Fts5Table*)(pCsr->base.pVtab))->pConfig;
  int rc = SQLITE_OK;
  int bLive = (pCsr->pSorter==0);

  if( CsrFlagTest(pCsr, FTS5CSR_REQUIRE_POSLIST) ){

    if( pConfig->eDetail!=FTS5_DETAIL_FULL ){
      Fts5PoslistPopulator *aPopulator;
      int i;
      aPopulator = sqlite3Fts5ExprClearPoslists(pCsr->pExpr, bLive);
      if( aPopulator==0 ) rc = SQLITE_NOMEM;
      for(i=0; i<pConfig->nCol && rc==SQLITE_OK; i++){
        int n; const char *z;
        rc = fts5ApiColumnText((Fts5Context*)pCsr, i, &z, &n);
        if( rc==SQLITE_OK ){
          rc = sqlite3Fts5ExprPopulatePoslists(
              pConfig, pCsr->pExpr, aPopulator, i, z, n
          );
        }
      }
      sqlite3_free(aPopulator);

      if( pCsr->pSorter ){
        sqlite3Fts5ExprCheckPoslists(pCsr->pExpr, pCsr->pSorter->iRowid);
      }
    }
    CsrFlagClear(pCsr, FTS5CSR_REQUIRE_POSLIST);
  }

  if( pCsr->pSorter && pConfig->eDetail==FTS5_DETAIL_FULL ){
    Fts5Sorter *pSorter = pCsr->pSorter;
    int i1 = (iPhrase==0 ? 0 : pSorter->aIdx[iPhrase-1]);
    *pn = pSorter->aIdx[iPhrase] - i1;
    *pa = &pSorter->aPoslist[i1];
  }else{
    *pn = sqlite3Fts5ExprPoslist(pCsr->pExpr, iPhrase, pa);
  }

  return rc;
}

/*
** Ensure that the Fts5Cursor.nInstCount and aInst[] variables are populated
** correctly for the current view. Return SQLITE_OK if successful, or an
** SQLite error code otherwise.
*/
static int fts5CacheInstArray(Fts5Cursor *pCsr){
  int rc = SQLITE_OK;
  Fts5PoslistReader *aIter;       /* One iterator for each phrase */
  int nIter;                      /* Number of iterators/phrases */
  int nCol = ((Fts5Table*)pCsr->base.pVtab)->pConfig->nCol;
  
  nIter = sqlite3Fts5ExprPhraseCount(pCsr->pExpr);
  if( pCsr->aInstIter==0 ){
    sqlite3_int64 nByte = sizeof(Fts5PoslistReader) * nIter;
    pCsr->aInstIter = (Fts5PoslistReader*)sqlite3Fts5MallocZero(&rc, nByte);
  }
  aIter = pCsr->aInstIter;

  if( aIter ){
    int nInst = 0;                /* Number instances seen so far */
    int i;

    /* Initialize all iterators */
    for(i=0; i<nIter && rc==SQLITE_OK; i++){
      const u8 *a;
      int n; 
      rc = fts5CsrPoslist(pCsr, i, &a, &n);
      if( rc==SQLITE_OK ){
        sqlite3Fts5PoslistReaderInit(a, n, &aIter[i]);
      }
    }

    if( rc==SQLITE_OK ){
      while( 1 ){
        int *aInst;
        int iBest = -1;
        for(i=0; i<nIter; i++){
          if( (aIter[i].bEof==0) 
              && (iBest<0 || aIter[i].iPos<aIter[iBest].iPos) 
            ){
            iBest = i;
          }
        }
        if( iBest<0 ) break;

        nInst++;
        if( nInst>=pCsr->nInstAlloc ){
          int nNewSize = pCsr->nInstAlloc ? pCsr->nInstAlloc*2 : 32;
          aInst = (int*)sqlite3_realloc64(
              pCsr->aInst, nNewSize*sizeof(int)*3
              );
          if( aInst ){
            pCsr->aInst = aInst;
            pCsr->nInstAlloc = nNewSize;
          }else{
            nInst--;
            rc = SQLITE_NOMEM;
            break;
          }
        }

        aInst = &pCsr->aInst[3 * (nInst-1)];
        aInst[0] = iBest;
        aInst[1] = FTS5_POS2COLUMN(aIter[iBest].iPos);
        aInst[2] = FTS5_POS2OFFSET(aIter[iBest].iPos);
        if( aInst[1]<0 || aInst[1]>=nCol ){
          rc = FTS5_CORRUPT;
          break;
        }
        sqlite3Fts5PoslistReaderNext(&aIter[iBest]);
      }
    }

    pCsr->nInstCount = nInst;
    CsrFlagClear(pCsr, FTS5CSR_REQUIRE_INST);
  }
  return rc;
}

static int fts5ApiInstCount(Fts5Context *pCtx, int *pnInst){
  Fts5Cursor *pCsr = (Fts5Cursor*)pCtx;
  int rc = SQLITE_OK;
  if( CsrFlagTest(pCsr, FTS5CSR_REQUIRE_INST)==0 
   || SQLITE_OK==(rc = fts5CacheInstArray(pCsr)) ){
    *pnInst = pCsr->nInstCount;
  }
  return rc;
}

static int fts5ApiInst(
  Fts5Context *pCtx, 
  int iIdx, 
  int *piPhrase, 
  int *piCol, 
  int *piOff
){
  Fts5Cursor *pCsr = (Fts5Cursor*)pCtx;
  int rc = SQLITE_OK;
  if( CsrFlagTest(pCsr, FTS5CSR_REQUIRE_INST)==0 
   || SQLITE_OK==(rc = fts5CacheInstArray(pCsr)) 
  ){
    if( iIdx<0 || iIdx>=pCsr->nInstCount ){
      rc = SQLITE_RANGE;
#if 0
    }else if( fts5IsOffsetless((Fts5Table*)pCsr->base.pVtab) ){
      *piPhrase = pCsr->aInst[iIdx*3];
      *piCol = pCsr->aInst[iIdx*3 + 2];
      *piOff = -1;
#endif
    }else{
      *piPhrase = pCsr->aInst[iIdx*3];
      *piCol = pCsr->aInst[iIdx*3 + 1];
      *piOff = pCsr->aInst[iIdx*3 + 2];
    }
  }
  return rc;
}

static sqlite3_int64 fts5ApiRowid(Fts5Context *pCtx){
  return fts5CursorRowid((Fts5Cursor*)pCtx);
}

static int fts5ColumnSizeCb(
  void *pContext,                 /* Pointer to int */
  int tflags,
  const char *pUnused,            /* Buffer containing token */
  int nUnused,                    /* Size of token in bytes */
  int iUnused1,                   /* Start offset of token */
  int iUnused2                    /* End offset of token */
){
  int *pCnt = (int*)pContext;
  UNUSED_PARAM2(pUnused, nUnused);
  UNUSED_PARAM2(iUnused1, iUnused2);
  if( (tflags & FTS5_TOKEN_COLOCATED)==0 ){
    (*pCnt)++;
  }
  return SQLITE_OK;
}

static int fts5ApiColumnSize(Fts5Context *pCtx, int iCol, int *pnToken){
  Fts5Cursor *pCsr = (Fts5Cursor*)pCtx;
  Fts5FullTable *pTab = (Fts5FullTable*)(pCsr->base.pVtab);
  Fts5Config *pConfig = pTab->p.pConfig;
  int rc = SQLITE_OK;

  if( CsrFlagTest(pCsr, FTS5CSR_REQUIRE_DOCSIZE) ){
    if( pConfig->bColumnsize ){
      i64 iRowid = fts5CursorRowid(pCsr);
      rc = sqlite3Fts5StorageDocsize(pTab->pStorage, iRowid, pCsr->aColumnSize);
    }else if( pConfig->zContent==0 ){
      int i;
      for(i=0; i<pConfig->nCol; i++){
        if( pConfig->abUnindexed[i]==0 ){
          pCsr->aColumnSize[i] = -1;
        }
      }
    }else{
      int i;
      for(i=0; rc==SQLITE_OK && i<pConfig->nCol; i++){
        if( pConfig->abUnindexed[i]==0 ){
          const char *z; int n;
          void *p = (void*)(&pCsr->aColumnSize[i]);
          pCsr->aColumnSize[i] = 0;
          rc = fts5ApiColumnText(pCtx, i, &z, &n);
          if( rc==SQLITE_OK ){
            rc = sqlite3Fts5Tokenize(
                pConfig, FTS5_TOKENIZE_AUX, z, n, p, fts5ColumnSizeCb
            );
          }
        }
      }
    }
    CsrFlagClear(pCsr, FTS5CSR_REQUIRE_DOCSIZE);
  }
  if( iCol<0 ){
    int i;
    *pnToken = 0;
    for(i=0; i<pConfig->nCol; i++){
      *pnToken += pCsr->aColumnSize[i];
    }
  }else if( iCol<pConfig->nCol ){
    *pnToken = pCsr->aColumnSize[iCol];
  }else{
    *pnToken = 0;
    rc = SQLITE_RANGE;
  }
  return rc;
}

/*
** Implementation of the xSetAuxdata() method.
*/
static int fts5ApiSetAuxdata(
  Fts5Context *pCtx,              /* Fts5 context */
  void *pPtr,                     /* Pointer to save as auxdata */
  void(*xDelete)(void*)           /* Destructor for pPtr (or NULL) */
){
  Fts5Cursor *pCsr = (Fts5Cursor*)pCtx;
  Fts5Auxdata *pData;

  /* Search through the cursors list of Fts5Auxdata objects for one that
  ** corresponds to the currently executing auxiliary function.  */
  for(pData=pCsr->pAuxdata; pData; pData=pData->pNext){
    if( pData->pAux==pCsr->pAux ) break;
  }

  if( pData ){
    if( pData->xDelete ){
      pData->xDelete(pData->pPtr);
    }
  }else{
    int rc = SQLITE_OK;
    pData = (Fts5Auxdata*)sqlite3Fts5MallocZero(&rc, sizeof(Fts5Auxdata));
    if( pData==0 ){
      if( xDelete ) xDelete(pPtr);
      return rc;
    }
    pData->pAux = pCsr->pAux;
    pData->pNext = pCsr->pAuxdata;
    pCsr->pAuxdata = pData;
  }

  pData->xDelete = xDelete;
  pData->pPtr = pPtr;
  return SQLITE_OK;
}

static void *fts5ApiGetAuxdata(Fts5Context *pCtx, int bClear){
  Fts5Cursor *pCsr = (Fts5Cursor*)pCtx;
  Fts5Auxdata *pData;
  void *pRet = 0;

  for(pData=pCsr->pAuxdata; pData; pData=pData->pNext){
    if( pData->pAux==pCsr->pAux ) break;
  }

  if( pData ){
    pRet = pData->pPtr;
    if( bClear ){
      pData->pPtr = 0;
      pData->xDelete = 0;
    }
  }

  return pRet;
}

static void fts5ApiPhraseNext(
  Fts5Context *pUnused, 
  Fts5PhraseIter *pIter, 
  int *piCol, int *piOff
){
  UNUSED_PARAM(pUnused);
  if( pIter->a>=pIter->b ){
    *piCol = -1;
    *piOff = -1;
  }else{
    int iVal;
    pIter->a += fts5GetVarint32(pIter->a, iVal);
    if( iVal==1 ){
      pIter->a += fts5GetVarint32(pIter->a, iVal);
      *piCol = iVal;
      *piOff = 0;
      pIter->a += fts5GetVarint32(pIter->a, iVal);
    }
    *piOff += (iVal-2);
  }
}

static int fts5ApiPhraseFirst(
  Fts5Context *pCtx, 
  int iPhrase, 
  Fts5PhraseIter *pIter, 
  int *piCol, int *piOff
){
  Fts5Cursor *pCsr = (Fts5Cursor*)pCtx;
  int n;
  int rc = fts5CsrPoslist(pCsr, iPhrase, &pIter->a, &n);
  if( rc==SQLITE_OK ){
    assert( pIter->a || n==0 );
    pIter->b = (pIter->a ? &pIter->a[n] : 0);
    *piCol = 0;
    *piOff = 0;
    fts5ApiPhraseNext(pCtx, pIter, piCol, piOff);
  }
  return rc;
}

static void fts5ApiPhraseNextColumn(
  Fts5Context *pCtx, 
  Fts5PhraseIter *pIter, 
  int *piCol
){
  Fts5Cursor *pCsr = (Fts5Cursor*)pCtx;
  Fts5Config *pConfig = ((Fts5Table*)(pCsr->base.pVtab))->pConfig;

  if( pConfig->eDetail==FTS5_DETAIL_COLUMNS ){
    if( pIter->a>=pIter->b ){
      *piCol = -1;
    }else{
      int iIncr;
      pIter->a += fts5GetVarint32(&pIter->a[0], iIncr);
      *piCol += (iIncr-2);
    }
  }else{
    while( 1 ){
      int dummy;
      if( pIter->a>=pIter->b ){
        *piCol = -1;
        return;
      }
      if( pIter->a[0]==0x01 ) break;
      pIter->a += fts5GetVarint32(pIter->a, dummy);
    }
    pIter->a += 1 + fts5GetVarint32(&pIter->a[1], *piCol);
  }
}

static int fts5ApiPhraseFirstColumn(
  Fts5Context *pCtx, 
  int iPhrase, 
  Fts5PhraseIter *pIter, 
  int *piCol
){
  int rc = SQLITE_OK;
  Fts5Cursor *pCsr = (Fts5Cursor*)pCtx;
  Fts5Config *pConfig = ((Fts5Table*)(pCsr->base.pVtab))->pConfig;

  if( pConfig->eDetail==FTS5_DETAIL_COLUMNS ){
    Fts5Sorter *pSorter = pCsr->pSorter;
    int n;
    if( pSorter ){
      int i1 = (iPhrase==0 ? 0 : pSorter->aIdx[iPhrase-1]);
      n = pSorter->aIdx[iPhrase] - i1;
      pIter->a = &pSorter->aPoslist[i1];
    }else{
      rc = sqlite3Fts5ExprPhraseCollist(pCsr->pExpr, iPhrase, &pIter->a, &n);
    }
    if( rc==SQLITE_OK ){
      assert( pIter->a || n==0 );
      pIter->b = (pIter->a ? &pIter->a[n] : 0);
      *piCol = 0;
      fts5ApiPhraseNextColumn(pCtx, pIter, piCol);
    }
  }else{
    int n;
    rc = fts5CsrPoslist(pCsr, iPhrase, &pIter->a, &n);
    if( rc==SQLITE_OK ){
      assert( pIter->a || n==0 );
      pIter->b = (pIter->a ? &pIter->a[n] : 0);
      if( n<=0 ){
        *piCol = -1;
      }else if( pIter->a[0]==0x01 ){
        pIter->a += 1 + fts5GetVarint32(&pIter->a[1], *piCol);
      }else{
        *piCol = 0;
      }
    }
  }

  return rc;
}


static int fts5ApiQueryPhrase(Fts5Context*, int, void*, 
    int(*)(const Fts5ExtensionApi*, Fts5Context*, void*)
);

static const Fts5ExtensionApi sFts5Api = {
  2,                            /* iVersion */
  fts5ApiUserData,
  fts5ApiColumnCount,
  fts5ApiRowCount,
  fts5ApiColumnTotalSize,
  fts5ApiTokenize,
  fts5ApiPhraseCount,
  fts5ApiPhraseSize,
  fts5ApiInstCount,
  fts5ApiInst,
  fts5ApiRowid,
  fts5ApiColumnText,
  fts5ApiColumnSize,
  fts5ApiQueryPhrase,
  fts5ApiSetAuxdata,
  fts5ApiGetAuxdata,
  fts5ApiPhraseFirst,
  fts5ApiPhraseNext,
  fts5ApiPhraseFirstColumn,
  fts5ApiPhraseNextColumn,
};

/*
** Implementation of API function xQueryPhrase().
*/
static int fts5ApiQueryPhrase(
  Fts5Context *pCtx, 
  int iPhrase, 
  void *pUserData,
  int(*xCallback)(const Fts5ExtensionApi*, Fts5Context*, void*)
){
  Fts5Cursor *pCsr = (Fts5Cursor*)pCtx;
  Fts5FullTable *pTab = (Fts5FullTable*)(pCsr->base.pVtab);
  int rc;
  Fts5Cursor *pNew = 0;

  rc = fts5OpenMethod(pCsr->base.pVtab, (sqlite3_vtab_cursor**)&pNew);
  if( rc==SQLITE_OK ){
    pNew->ePlan = FTS5_PLAN_MATCH;
    pNew->iFirstRowid = SMALLEST_INT64;
    pNew->iLastRowid = LARGEST_INT64;
    pNew->base.pVtab = (sqlite3_vtab*)pTab;
    rc = sqlite3Fts5ExprClonePhrase(pCsr->pExpr, iPhrase, &pNew->pExpr);
  }

  if( rc==SQLITE_OK ){
    for(rc = fts5CursorFirst(pTab, pNew, 0);
        rc==SQLITE_OK && CsrFlagTest(pNew, FTS5CSR_EOF)==0;
        rc = fts5NextMethod((sqlite3_vtab_cursor*)pNew)
    ){
      rc = xCallback(&sFts5Api, (Fts5Context*)pNew, pUserData);
      if( rc!=SQLITE_OK ){
        if( rc==SQLITE_DONE ) rc = SQLITE_OK;
        break;
      }
    }
  }

  fts5CloseMethod((sqlite3_vtab_cursor*)pNew);
  return rc;
}

static void fts5ApiInvoke(
  Fts5Auxiliary *pAux,
  Fts5Cursor *pCsr,
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
){
  assert( pCsr->pAux==0 );
  pCsr->pAux = pAux;
  pAux->xFunc(&sFts5Api, (Fts5Context*)pCsr, context, argc, argv);
  pCsr->pAux = 0;
}

static Fts5Cursor *fts5CursorFromCsrid(Fts5Global *pGlobal, i64 iCsrId){
  Fts5Cursor *pCsr;
  for(pCsr=pGlobal->pCsr; pCsr; pCsr=pCsr->pNext){
    if( pCsr->iCsrId==iCsrId ) break;
  }
  return pCsr;
}

static void fts5ApiCallback(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
){

  Fts5Auxiliary *pAux;
  Fts5Cursor *pCsr;
  i64 iCsrId;

  assert( argc>=1 );
  pAux = (Fts5Auxiliary*)sqlite3_user_data(context);
  iCsrId = sqlite3_value_int64(argv[0]);

  pCsr = fts5CursorFromCsrid(pAux->pGlobal, iCsrId);
  if( pCsr==0 || pCsr->ePlan==0 ){
    char *zErr = sqlite3_mprintf("no such cursor: %lld", iCsrId);
    sqlite3_result_error(context, zErr, -1);
    sqlite3_free(zErr);
  }else{
    fts5ApiInvoke(pAux, pCsr, context, argc-1, &argv[1]);
  }
}


/*
** Given cursor id iId, return a pointer to the corresponding Fts5Table 
** object. Or NULL If the cursor id does not exist.
*/
static Fts5Table *sqlite3Fts5TableFromCsrid(
  Fts5Global *pGlobal,            /* FTS5 global context for db handle */
  i64 iCsrId                      /* Id of cursor to find */
){
  Fts5Cursor *pCsr;
  pCsr = fts5CursorFromCsrid(pGlobal, iCsrId);
  if( pCsr ){
    return (Fts5Table*)pCsr->base.pVtab;
  }
  return 0;
}

/*
** Return a "position-list blob" corresponding to the current position of
** cursor pCsr via sqlite3_result_blob(). A position-list blob contains
** the current position-list for each phrase in the query associated with
** cursor pCsr.
**
** A position-list blob begins with (nPhrase-1) varints, where nPhrase is
** the number of phrases in the query. Following the varints are the
** concatenated position lists for each phrase, in order.
**
** The first varint (if it exists) contains the size of the position list
** for phrase 0. The second (same disclaimer) contains the size of position
** list 1. And so on. There is no size field for the final position list,
** as it can be derived from the total size of the blob.
*/
static int fts5PoslistBlob(sqlite3_context *pCtx, Fts5Cursor *pCsr){
  int i;
  int rc = SQLITE_OK;
  int nPhrase = sqlite3Fts5ExprPhraseCount(pCsr->pExpr);
  Fts5Buffer val;

  memset(&val, 0, sizeof(Fts5Buffer));
  switch( ((Fts5Table*)(pCsr->base.pVtab))->pConfig->eDetail ){
    case FTS5_DETAIL_FULL:

      /* Append the varints */
      for(i=0; i<(nPhrase-1); i++){
        const u8 *dummy;
        int nByte = sqlite3Fts5ExprPoslist(pCsr->pExpr, i, &dummy);
        sqlite3Fts5BufferAppendVarint(&rc, &val, nByte);
      }

      /* Append the position lists */
      for(i=0; i<nPhrase; i++){
        const u8 *pPoslist;
        int nPoslist;
        nPoslist = sqlite3Fts5ExprPoslist(pCsr->pExpr, i, &pPoslist);
        sqlite3Fts5BufferAppendBlob(&rc, &val, nPoslist, pPoslist);
      }
      break;

    case FTS5_DETAIL_COLUMNS:

      /* Append the varints */
      for(i=0; rc==SQLITE_OK && i<(nPhrase-1); i++){
        const u8 *dummy;
        int nByte;
        rc = sqlite3Fts5ExprPhraseCollist(pCsr->pExpr, i, &dummy, &nByte);
        sqlite3Fts5BufferAppendVarint(&rc, &val, nByte);
      }

      /* Append the position lists */
      for(i=0; rc==SQLITE_OK && i<nPhrase; i++){
        const u8 *pPoslist;
        int nPoslist;
        rc = sqlite3Fts5ExprPhraseCollist(pCsr->pExpr, i, &pPoslist, &nPoslist);
        sqlite3Fts5BufferAppendBlob(&rc, &val, nPoslist, pPoslist);
      }
      break;

    default:
      break;
  }

  sqlite3_result_blob(pCtx, val.p, val.n, sqlite3_free);
  return rc;
}

/* 
** This is the xColumn method, called by SQLite to request a value from
** the row that the supplied cursor currently points to.
*/
static int fts5ColumnMethod(
  sqlite3_vtab_cursor *pCursor,   /* Cursor to retrieve value from */
  sqlite3_context *pCtx,          /* Context for sqlite3_result_xxx() calls */
  int iCol                        /* Index of column to read value from */
){
  Fts5FullTable *pTab = (Fts5FullTable*)(pCursor->pVtab);
  Fts5Config *pConfig = pTab->p.pConfig;
  Fts5Cursor *pCsr = (Fts5Cursor*)pCursor;
  int rc = SQLITE_OK;
  
  assert( CsrFlagTest(pCsr, FTS5CSR_EOF)==0 );

  if( pCsr->ePlan==FTS5_PLAN_SPECIAL ){
    if( iCol==pConfig->nCol ){
      sqlite3_result_int64(pCtx, pCsr->iSpecial);
    }
  }else

  if( iCol==pConfig->nCol ){
    /* User is requesting the value of the special column with the same name
    ** as the table. Return the cursor integer id number. This value is only
    ** useful in that it may be passed as the first argument to an FTS5
    ** auxiliary function.  */
    sqlite3_result_int64(pCtx, pCsr->iCsrId);
  }else if( iCol==pConfig->nCol+1 ){

    /* The value of the "rank" column. */
    if( pCsr->ePlan==FTS5_PLAN_SOURCE ){
      fts5PoslistBlob(pCtx, pCsr);
    }else if( 
        pCsr->ePlan==FTS5_PLAN_MATCH
     || pCsr->ePlan==FTS5_PLAN_SORTED_MATCH
    ){
      if( pCsr->pRank || SQLITE_OK==(rc = fts5FindRankFunction(pCsr)) ){
        fts5ApiInvoke(pCsr->pRank, pCsr, pCtx, pCsr->nRankArg, pCsr->apRankArg);
      }
    }
  }else if( !fts5IsContentless(pTab) ){
    pConfig->pzErrmsg = &pTab->p.base.zErrMsg;
    rc = fts5SeekCursor(pCsr, 1);
    if( rc==SQLITE_OK ){
      sqlite3_result_value(pCtx, sqlite3_column_value(pCsr->pStmt, iCol+1));
    }
    pConfig->pzErrmsg = 0;
  }
  return rc;
}


/*
** This routine implements the xFindFunction method for the FTS3
** virtual table.
*/
static int fts5FindFunctionMethod(
  sqlite3_vtab *pVtab,            /* Virtual table handle */
  int nUnused,                    /* Number of SQL function arguments */
  const char *zName,              /* Name of SQL function */
  void (**pxFunc)(sqlite3_context*,int,sqlite3_value**), /* OUT: Result */
  void **ppArg                    /* OUT: User data for *pxFunc */
){
  Fts5FullTable *pTab = (Fts5FullTable*)pVtab;
  Fts5Auxiliary *pAux;

  UNUSED_PARAM(nUnused);
  pAux = fts5FindAuxiliary(pTab, zName);
  if( pAux ){
    *pxFunc = fts5ApiCallback;
    *ppArg = (void*)pAux;
    return 1;
  }

  /* No function of the specified name was found. Return 0. */
  return 0;
}

/*
** Implementation of FTS5 xRename method. Rename an fts5 table.
*/
static int fts5RenameMethod(
  sqlite3_vtab *pVtab,            /* Virtual table handle */
  const char *zName               /* New name of table */
){
  Fts5FullTable *pTab = (Fts5FullTable*)pVtab;
  return sqlite3Fts5StorageRename(pTab->pStorage, zName);
}

static int sqlite3Fts5FlushToDisk(Fts5Table *pTab){
  fts5TripCursors((Fts5FullTable*)pTab);
  return sqlite3Fts5StorageSync(((Fts5FullTable*)pTab)->pStorage);
}

/*
** The xSavepoint() method.
**
** Flush the contents of the pending-terms table to disk.
*/
static int fts5SavepointMethod(sqlite3_vtab *pVtab, int iSavepoint){
  UNUSED_PARAM(iSavepoint);  /* Call below is a no-op for NDEBUG builds */
  fts5CheckTransactionState((Fts5FullTable*)pVtab, FTS5_SAVEPOINT, iSavepoint);
  return sqlite3Fts5FlushToDisk((Fts5Table*)pVtab);
}

/*
** The xRelease() method.
**
** This is a no-op.
*/
static int fts5ReleaseMethod(sqlite3_vtab *pVtab, int iSavepoint){
  UNUSED_PARAM(iSavepoint);  /* Call below is a no-op for NDEBUG builds */
  fts5CheckTransactionState((Fts5FullTable*)pVtab, FTS5_RELEASE, iSavepoint);
  return sqlite3Fts5FlushToDisk((Fts5Table*)pVtab);
}

/*
** The xRollbackTo() method.
**
** Discard the contents of the pending terms table.
*/
static int fts5RollbackToMethod(sqlite3_vtab *pVtab, int iSavepoint){
  Fts5FullTable *pTab = (Fts5FullTable*)pVtab;
  UNUSED_PARAM(iSavepoint);  /* Call below is a no-op for NDEBUG builds */
  fts5CheckTransactionState(pTab, FTS5_ROLLBACKTO, iSavepoint);
  fts5TripCursors(pTab);
  return sqlite3Fts5StorageRollback(pTab->pStorage);
}

/*
** Register a new auxiliary function with global context pGlobal.
*/
static int fts5CreateAux(
  fts5_api *pApi,                 /* Global context (one per db handle) */
  const char *zName,              /* Name of new function */
  void *pUserData,                /* User data for aux. function */
  fts5_extension_function xFunc,  /* Aux. function implementation */
  void(*xDestroy)(void*)          /* Destructor for pUserData */
){
  Fts5Global *pGlobal = (Fts5Global*)pApi;
  int rc = sqlite3_overload_function(pGlobal->db, zName, -1);
  if( rc==SQLITE_OK ){
    Fts5Auxiliary *pAux;
    sqlite3_int64 nName;            /* Size of zName in bytes, including \0 */
    sqlite3_int64 nByte;            /* Bytes of space to allocate */

    nName = strlen(zName) + 1;
    nByte = sizeof(Fts5Auxiliary) + nName;
    pAux = (Fts5Auxiliary*)sqlite3_malloc64(nByte);
    if( pAux ){
      memset(pAux, 0, (size_t)nByte);
      pAux->zFunc = (char*)&pAux[1];
      memcpy(pAux->zFunc, zName, nName);
      pAux->pGlobal = pGlobal;
      pAux->pUserData = pUserData;
      pAux->xFunc = xFunc;
      pAux->xDestroy = xDestroy;
      pAux->pNext = pGlobal->pAux;
      pGlobal->pAux = pAux;
    }else{
      rc = SQLITE_NOMEM;
    }
  }

  return rc;
}

/*
** Register a new tokenizer. This is the implementation of the 
** fts5_api.xCreateTokenizer() method.
*/
static int fts5CreateTokenizer(
  fts5_api *pApi,                 /* Global context (one per db handle) */
  const char *zName,              /* Name of new function */
  void *pUserData,                /* User data for aux. function */
  fts5_tokenizer *pTokenizer,     /* Tokenizer implementation */
  void(*xDestroy)(void*)          /* Destructor for pUserData */
){
  Fts5Global *pGlobal = (Fts5Global*)pApi;
  Fts5TokenizerModule *pNew;
  sqlite3_int64 nName;            /* Size of zName and its \0 terminator */
  sqlite3_int64 nByte;            /* Bytes of space to allocate */
  int rc = SQLITE_OK;

  nName = strlen(zName) + 1;
  nByte = sizeof(Fts5TokenizerModule) + nName;
  pNew = (Fts5TokenizerModule*)sqlite3_malloc64(nByte);
  if( pNew ){
    memset(pNew, 0, (size_t)nByte);
    pNew->zName = (char*)&pNew[1];
    memcpy(pNew->zName, zName, nName);
    pNew->pUserData = pUserData;
    pNew->x = *pTokenizer;
    pNew->xDestroy = xDestroy;
    pNew->pNext = pGlobal->pTok;
    pGlobal->pTok = pNew;
    if( pNew->pNext==0 ){
      pGlobal->pDfltTok = pNew;
    }
  }else{
    rc = SQLITE_NOMEM;
  }

  return rc;
}

static Fts5TokenizerModule *fts5LocateTokenizer(
  Fts5Global *pGlobal, 
  const char *zName
){
  Fts5TokenizerModule *pMod = 0;

  if( zName==0 ){
    pMod = pGlobal->pDfltTok;
  }else{
    for(pMod=pGlobal->pTok; pMod; pMod=pMod->pNext){
      if( sqlite3_stricmp(zName, pMod->zName)==0 ) break;
    }
  }

  return pMod;
}

/*
** Find a tokenizer. This is the implementation of the 
** fts5_api.xFindTokenizer() method.
*/
static int fts5FindTokenizer(
  fts5_api *pApi,                 /* Global context (one per db handle) */
  const char *zName,              /* Name of new function */
  void **ppUserData,
  fts5_tokenizer *pTokenizer      /* Populate this object */
){
  int rc = SQLITE_OK;
  Fts5TokenizerModule *pMod;

  pMod = fts5LocateTokenizer((Fts5Global*)pApi, zName);
  if( pMod ){
    *pTokenizer = pMod->x;
    *ppUserData = pMod->pUserData;
  }else{
    memset(pTokenizer, 0, sizeof(fts5_tokenizer));
    rc = SQLITE_ERROR;
  }

  return rc;
}

static int sqlite3Fts5GetTokenizer(
  Fts5Global *pGlobal, 
  const char **azArg,
  int nArg,
  Fts5Config *pConfig,
  char **pzErr
){
  Fts5TokenizerModule *pMod;
  int rc = SQLITE_OK;

  pMod = fts5LocateTokenizer(pGlobal, nArg==0 ? 0 : azArg[0]);
  if( pMod==0 ){
    assert( nArg>0 );
    rc = SQLITE_ERROR;
    *pzErr = sqlite3_mprintf("no such tokenizer: %s", azArg[0]);
  }else{
    rc = pMod->x.xCreate(
        pMod->pUserData, (azArg?&azArg[1]:0), (nArg?nArg-1:0), &pConfig->pTok
    );
    pConfig->pTokApi = &pMod->x;
    if( rc!=SQLITE_OK ){
      if( pzErr ) *pzErr = sqlite3_mprintf("error in tokenizer constructor");
    }else{
      pConfig->ePattern = sqlite3Fts5TokenizerPattern(
          pMod->x.xCreate, pConfig->pTok
      );
    }
  }

  if( rc!=SQLITE_OK ){
    pConfig->pTokApi = 0;
    pConfig->pTok = 0;
  }

  return rc;
}

static void fts5ModuleDestroy(void *pCtx){
  Fts5TokenizerModule *pTok, *pNextTok;
  Fts5Auxiliary *pAux, *pNextAux;
  Fts5Global *pGlobal = (Fts5Global*)pCtx;

  for(pAux=pGlobal->pAux; pAux; pAux=pNextAux){
    pNextAux = pAux->pNext;
    if( pAux->xDestroy ) pAux->xDestroy(pAux->pUserData);
    sqlite3_free(pAux);
  }

  for(pTok=pGlobal->pTok; pTok; pTok=pNextTok){
    pNextTok = pTok->pNext;
    if( pTok->xDestroy ) pTok->xDestroy(pTok->pUserData);
    sqlite3_free(pTok);
  }

  sqlite3_free(pGlobal);
}

static void fts5Fts5Func(
  sqlite3_context *pCtx,          /* Function call context */
  int nArg,                       /* Number of args */
  sqlite3_value **apArg           /* Function arguments */
){
  Fts5Global *pGlobal = (Fts5Global*)sqlite3_user_data(pCtx);
  fts5_api **ppApi;
  UNUSED_PARAM(nArg);
  assert( nArg==1 );
  ppApi = (fts5_api**)sqlite3_value_pointer(apArg[0], "fts5_api_ptr");
  if( ppApi ) *ppApi = &pGlobal->api;
}

/*
** Implementation of fts5_source_id() function.
*/
static void fts5SourceIdFunc(
  sqlite3_context *pCtx,          /* Function call context */
  int nArg,                       /* Number of args */
  sqlite3_value **apUnused        /* Function arguments */
){
  assert( nArg==0 );
  UNUSED_PARAM2(nArg, apUnused);
  sqlite3_result_text(pCtx, "fts5: 2022-11-16 12:10:08 89c459e766ea7e9165d0beeb124708b955a4950d0f4792f457465d71b158d318", -1, SQLITE_TRANSIENT);
}

/*
** Return true if zName is the extension on one of the shadow tables used
** by this module.
*/
static int fts5ShadowName(const char *zName){
  static const char *azName[] = {
    "config", "content", "data", "docsize", "idx"
  };
  unsigned int i;
  for(i=0; i<sizeof(azName)/sizeof(azName[0]); i++){
    if( sqlite3_stricmp(zName, azName[i])==0 ) return 1;
  }
  return 0;
}

static int fts5Init(sqlite3 *db){
  static const sqlite3_module fts5Mod = {
    /* iVersion      */ 3,
    /* xCreate       */ fts5CreateMethod,
    /* xConnect      */ fts5ConnectMethod,
    /* xBestIndex    */ fts5BestIndexMethod,
    /* xDisconnect   */ fts5DisconnectMethod,
    /* xDestroy      */ fts5DestroyMethod,
    /* xOpen         */ fts5OpenMethod,
    /* xClose        */ fts5CloseMethod,
    /* xFilter       */ fts5FilterMethod,
    /* xNext         */ fts5NextMethod,
    /* xEof          */ fts5EofMethod,
    /* xColumn       */ fts5ColumnMethod,
    /* xRowid        */ fts5RowidMethod,
    /* xUpdate       */ fts5UpdateMethod,
    /* xBegin        */ fts5BeginMethod,
    /* xSync         */ fts5SyncMethod,
    /* xCommit       */ fts5CommitMethod,
    /* xRollback     */ fts5RollbackMethod,
    /* xFindFunction */ fts5FindFunctionMethod,
    /* xRename       */ fts5RenameMethod,
    /* xSavepoint    */ fts5SavepointMethod,
    /* xRelease      */ fts5ReleaseMethod,
    /* xRollbackTo   */ fts5RollbackToMethod,
    /* xShadowName   */ fts5ShadowName
  };

  int rc;
  Fts5Global *pGlobal = 0;

  pGlobal = (Fts5Global*)sqlite3_malloc(sizeof(Fts5Global));
  if( pGlobal==0 ){
    rc = SQLITE_NOMEM;
  }else{
    void *p = (void*)pGlobal;
    memset(pGlobal, 0, sizeof(Fts5Global));
    pGlobal->db = db;
    pGlobal->api.iVersion = 2;
    pGlobal->api.xCreateFunction = fts5CreateAux;
    pGlobal->api.xCreateTokenizer = fts5CreateTokenizer;
    pGlobal->api.xFindTokenizer = fts5FindTokenizer;
    rc = sqlite3_create_module_v2(db, "fts5", &fts5Mod, p, fts5ModuleDestroy);
    if( rc==SQLITE_OK ) rc = sqlite3Fts5IndexInit(db);
    if( rc==SQLITE_OK ) rc = sqlite3Fts5ExprInit(pGlobal, db);
    if( rc==SQLITE_OK ) rc = sqlite3Fts5AuxInit(&pGlobal->api);
    if( rc==SQLITE_OK ) rc = sqlite3Fts5TokenizerInit(&pGlobal->api);
    if( rc==SQLITE_OK ) rc = sqlite3Fts5VocabInit(pGlobal, db);
    if( rc==SQLITE_OK ){
      rc = sqlite3_create_function(
          db, "fts5", 1, SQLITE_UTF8, p, fts5Fts5Func, 0, 0
      );
    }
    if( rc==SQLITE_OK ){
      rc = sqlite3_create_function(
          db, "fts5_source_id", 0, SQLITE_UTF8, p, fts5SourceIdFunc, 0, 0
      );
    }
  }

  /* If SQLITE_FTS5_ENABLE_TEST_MI is defined, assume that the file
  ** fts5_test_mi.c is compiled and linked into the executable. And call
  ** its entry point to enable the matchinfo() demo.  */
#ifdef SQLITE_FTS5_ENABLE_TEST_MI
  if( rc==SQLITE_OK ){
    extern int sqlite3Fts5TestRegisterMatchinfo(sqlite3*);
    rc = sqlite3Fts5TestRegisterMatchinfo(db);
  }
#endif

  return rc;
}

/*
** The following functions are used to register the module with SQLite. If
** this module is being built as part of the SQLite core (SQLITE_CORE is
** defined), then sqlite3_open() will call sqlite3Fts5Init() directly.
**
** Or, if this module is being built as a loadable extension, 
** sqlite3Fts5Init() is omitted and the two standard entry points
** sqlite3_fts_init() and sqlite3_fts5_init() defined instead.
*/
#ifndef SQLITE_CORE
#ifdef _WIN32
__declspec(dllexport)
#endif
int sqlite3_fts_init(
  sqlite3 *db,
  char **pzErrMsg,
  const sqlite3_api_routines *pApi
){
  SQLITE_EXTENSION_INIT2(pApi);
  (void)pzErrMsg;  /* Unused parameter */
  return fts5Init(db);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
int sqlite3_fts5_init(
  sqlite3 *db,
  char **pzErrMsg,
  const sqlite3_api_routines *pApi
){
  SQLITE_EXTENSION_INIT2(pApi);
  (void)pzErrMsg;  /* Unused parameter */
  return fts5Init(db);
}
#else
int sqlite3Fts5Init(sqlite3 *db){
  return fts5Init(db);
}
#endif

#line 1 "fts5_storage.c"
/*
** 2014 May 31
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
******************************************************************************
**
*/



/* #include "fts5Int.h" */

struct Fts5Storage {
  Fts5Config *pConfig;
  Fts5Index *pIndex;
  int bTotalsValid;               /* True if nTotalRow/aTotalSize[] are valid */
  i64 nTotalRow;                  /* Total number of rows in FTS table */
  i64 *aTotalSize;                /* Total sizes of each column */ 
  sqlite3_stmt *aStmt[11];
};


#if FTS5_STMT_SCAN_ASC!=0 
# error "FTS5_STMT_SCAN_ASC mismatch" 
#endif
#if FTS5_STMT_SCAN_DESC!=1 
# error "FTS5_STMT_SCAN_DESC mismatch" 
#endif
#if FTS5_STMT_LOOKUP!=2
# error "FTS5_STMT_LOOKUP mismatch" 
#endif

#define FTS5_STMT_INSERT_CONTENT  3
#define FTS5_STMT_REPLACE_CONTENT 4
#define FTS5_STMT_DELETE_CONTENT  5
#define FTS5_STMT_REPLACE_DOCSIZE  6
#define FTS5_STMT_DELETE_DOCSIZE  7
#define FTS5_STMT_LOOKUP_DOCSIZE  8
#define FTS5_STMT_REPLACE_CONFIG 9
#define FTS5_STMT_SCAN 10

/*
** Prepare the two insert statements - Fts5Storage.pInsertContent and
** Fts5Storage.pInsertDocsize - if they have not already been prepared.
** Return SQLITE_OK if successful, or an SQLite error code if an error
** occurs.
*/
static int fts5StorageGetStmt(
  Fts5Storage *p,                 /* Storage handle */
  int eStmt,                      /* FTS5_STMT_XXX constant */
  sqlite3_stmt **ppStmt,          /* OUT: Prepared statement handle */
  char **pzErrMsg                 /* OUT: Error message (if any) */
){
  int rc = SQLITE_OK;

  /* If there is no %_docsize table, there should be no requests for 
  ** statements to operate on it.  */
  assert( p->pConfig->bColumnsize || (
        eStmt!=FTS5_STMT_REPLACE_DOCSIZE 
     && eStmt!=FTS5_STMT_DELETE_DOCSIZE 
     && eStmt!=FTS5_STMT_LOOKUP_DOCSIZE 
  ));

  assert( eStmt>=0 && eStmt<ArraySize(p->aStmt) );
  if( p->aStmt[eStmt]==0 ){
    const char *azStmt[] = {
      "SELECT %s FROM %s T WHERE T.%Q >= ? AND T.%Q <= ? ORDER BY T.%Q ASC",
      "SELECT %s FROM %s T WHERE T.%Q <= ? AND T.%Q >= ? ORDER BY T.%Q DESC",
      "SELECT %s FROM %s T WHERE T.%Q=?",               /* LOOKUP  */

      "INSERT INTO %Q.'%q_content' VALUES(%s)",         /* INSERT_CONTENT  */
      "REPLACE INTO %Q.'%q_content' VALUES(%s)",        /* REPLACE_CONTENT */
      "DELETE FROM %Q.'%q_content' WHERE id=?",         /* DELETE_CONTENT  */
      "REPLACE INTO %Q.'%q_docsize' VALUES(?,?)",       /* REPLACE_DOCSIZE  */
      "DELETE FROM %Q.'%q_docsize' WHERE id=?",         /* DELETE_DOCSIZE  */

      "SELECT sz FROM %Q.'%q_docsize' WHERE id=?",      /* LOOKUP_DOCSIZE  */

      "REPLACE INTO %Q.'%q_config' VALUES(?,?)",        /* REPLACE_CONFIG */
      "SELECT %s FROM %s AS T",                         /* SCAN */
    };
    Fts5Config *pC = p->pConfig;
    char *zSql = 0;

    switch( eStmt ){
      case FTS5_STMT_SCAN:
        zSql = sqlite3_mprintf(azStmt[eStmt], 
            pC->zContentExprlist, pC->zContent
        );
        break;

      case FTS5_STMT_SCAN_ASC:
      case FTS5_STMT_SCAN_DESC:
        zSql = sqlite3_mprintf(azStmt[eStmt], pC->zContentExprlist, 
            pC->zContent, pC->zContentRowid, pC->zContentRowid,
            pC->zContentRowid
        );
        break;

      case FTS5_STMT_LOOKUP:
        zSql = sqlite3_mprintf(azStmt[eStmt], 
            pC->zContentExprlist, pC->zContent, pC->zContentRowid
        );
        break;

      case FTS5_STMT_INSERT_CONTENT: 
      case FTS5_STMT_REPLACE_CONTENT: {
        int nCol = pC->nCol + 1;
        char *zBind;
        int i;

        zBind = sqlite3_malloc64(1 + nCol*2);
        if( zBind ){
          for(i=0; i<nCol; i++){
            zBind[i*2] = '?';
            zBind[i*2 + 1] = ',';
          }
          zBind[i*2-1] = '\0';
          zSql = sqlite3_mprintf(azStmt[eStmt], pC->zDb, pC->zName, zBind);
          sqlite3_free(zBind);
        }
        break;
      }

      default:
        zSql = sqlite3_mprintf(azStmt[eStmt], pC->zDb, pC->zName);
        break;
    }

    if( zSql==0 ){
      rc = SQLITE_NOMEM;
    }else{
      int f = SQLITE_PREPARE_PERSISTENT;
      if( eStmt>FTS5_STMT_LOOKUP ) f |= SQLITE_PREPARE_NO_VTAB;
      p->pConfig->bLock++;
      rc = sqlite3_prepare_v3(pC->db, zSql, -1, f, &p->aStmt[eStmt], 0);
      p->pConfig->bLock--;
      sqlite3_free(zSql);
      if( rc!=SQLITE_OK && pzErrMsg ){
        *pzErrMsg = sqlite3_mprintf("%s", sqlite3_errmsg(pC->db));
      }
    }
  }

  *ppStmt = p->aStmt[eStmt];
  sqlite3_reset(*ppStmt);
  return rc;
}


static int fts5ExecPrintf(
  sqlite3 *db,
  char **pzErr,
  const char *zFormat,
  ...
){
  int rc;
  va_list ap;                     /* ... printf arguments */
  char *zSql;

  va_start(ap, zFormat);
  zSql = sqlite3_vmprintf(zFormat, ap);

  if( zSql==0 ){
    rc = SQLITE_NOMEM;
  }else{
    rc = sqlite3_exec(db, zSql, 0, 0, pzErr);
    sqlite3_free(zSql);
  }

  va_end(ap);
  return rc;
}

/*
** Drop all shadow tables. Return SQLITE_OK if successful or an SQLite error
** code otherwise.
*/
static int sqlite3Fts5DropAll(Fts5Config *pConfig){
  int rc = fts5ExecPrintf(pConfig->db, 0, 
      "DROP TABLE IF EXISTS %Q.'%q_data';"
      "DROP TABLE IF EXISTS %Q.'%q_idx';"
      "DROP TABLE IF EXISTS %Q.'%q_config';",
      pConfig->zDb, pConfig->zName,
      pConfig->zDb, pConfig->zName,
      pConfig->zDb, pConfig->zName
  );
  if( rc==SQLITE_OK && pConfig->bColumnsize ){
    rc = fts5ExecPrintf(pConfig->db, 0, 
        "DROP TABLE IF EXISTS %Q.'%q_docsize';",
        pConfig->zDb, pConfig->zName
    );
  }
  if( rc==SQLITE_OK && pConfig->eContent==FTS5_CONTENT_NORMAL ){
    rc = fts5ExecPrintf(pConfig->db, 0, 
        "DROP TABLE IF EXISTS %Q.'%q_content';",
        pConfig->zDb, pConfig->zName
    );
  }
  return rc;
}

static void fts5StorageRenameOne(
  Fts5Config *pConfig,            /* Current FTS5 configuration */
  int *pRc,                       /* IN/OUT: Error code */
  const char *zTail,              /* Tail of table name e.g. "data", "config" */
  const char *zName               /* New name of FTS5 table */
){
  if( *pRc==SQLITE_OK ){
    *pRc = fts5ExecPrintf(pConfig->db, 0, 
        "ALTER TABLE %Q.'%q_%s' RENAME TO '%q_%s';",
        pConfig->zDb, pConfig->zName, zTail, zName, zTail
    );
  }
}

static int sqlite3Fts5StorageRename(Fts5Storage *pStorage, const char *zName){
  Fts5Config *pConfig = pStorage->pConfig;
  int rc = sqlite3Fts5StorageSync(pStorage);

  fts5StorageRenameOne(pConfig, &rc, "data", zName);
  fts5StorageRenameOne(pConfig, &rc, "idx", zName);
  fts5StorageRenameOne(pConfig, &rc, "config", zName);
  if( pConfig->bColumnsize ){
    fts5StorageRenameOne(pConfig, &rc, "docsize", zName);
  }
  if( pConfig->eContent==FTS5_CONTENT_NORMAL ){
    fts5StorageRenameOne(pConfig, &rc, "content", zName);
  }
  return rc;
}

/*
** Create the shadow table named zPost, with definition zDefn. Return
** SQLITE_OK if successful, or an SQLite error code otherwise.
*/
static int sqlite3Fts5CreateTable(
  Fts5Config *pConfig,            /* FTS5 configuration */
  const char *zPost,              /* Shadow table to create (e.g. "content") */
  const char *zDefn,              /* Columns etc. for shadow table */
  int bWithout,                   /* True for without rowid */
  char **pzErr                    /* OUT: Error message */
){
  int rc;
  char *zErr = 0;

  rc = fts5ExecPrintf(pConfig->db, &zErr, "CREATE TABLE %Q.'%q_%q'(%s)%s",
      pConfig->zDb, pConfig->zName, zPost, zDefn, 
#ifndef SQLITE_FTS5_NO_WITHOUT_ROWID
      bWithout?" WITHOUT ROWID":
#endif
      ""
  );
  if( zErr ){
    *pzErr = sqlite3_mprintf(
        "fts5: error creating shadow table %q_%s: %s", 
        pConfig->zName, zPost, zErr
    );
    sqlite3_free(zErr);
  }

  return rc;
}

/*
** Open a new Fts5Index handle. If the bCreate argument is true, create
** and initialize the underlying tables 
**
** If successful, set *pp to point to the new object and return SQLITE_OK.
** Otherwise, set *pp to NULL and return an SQLite error code.
*/
static int sqlite3Fts5StorageOpen(
  Fts5Config *pConfig, 
  Fts5Index *pIndex, 
  int bCreate, 
  Fts5Storage **pp,
  char **pzErr                    /* OUT: Error message */
){
  int rc = SQLITE_OK;
  Fts5Storage *p;                 /* New object */
  sqlite3_int64 nByte;            /* Bytes of space to allocate */

  nByte = sizeof(Fts5Storage)               /* Fts5Storage object */
        + pConfig->nCol * sizeof(i64);      /* Fts5Storage.aTotalSize[] */
  *pp = p = (Fts5Storage*)sqlite3_malloc64(nByte);
  if( !p ) return SQLITE_NOMEM;

  memset(p, 0, (size_t)nByte);
  p->aTotalSize = (i64*)&p[1];
  p->pConfig = pConfig;
  p->pIndex = pIndex;

  if( bCreate ){
    if( pConfig->eContent==FTS5_CONTENT_NORMAL ){
      int nDefn = 32 + pConfig->nCol*10;
      char *zDefn = sqlite3_malloc64(32 + (sqlite3_int64)pConfig->nCol * 10);
      if( zDefn==0 ){
        rc = SQLITE_NOMEM;
      }else{
        int i;
        int iOff;
        sqlite3_snprintf(nDefn, zDefn, "id INTEGER PRIMARY KEY");
        iOff = (int)strlen(zDefn);
        for(i=0; i<pConfig->nCol; i++){
          sqlite3_snprintf(nDefn-iOff, &zDefn[iOff], ", c%d", i);
          iOff += (int)strlen(&zDefn[iOff]);
        }
        rc = sqlite3Fts5CreateTable(pConfig, "content", zDefn, 0, pzErr);
      }
      sqlite3_free(zDefn);
    }

    if( rc==SQLITE_OK && pConfig->bColumnsize ){
      rc = sqlite3Fts5CreateTable(
          pConfig, "docsize", "id INTEGER PRIMARY KEY, sz BLOB", 0, pzErr
      );
    }
    if( rc==SQLITE_OK ){
      rc = sqlite3Fts5CreateTable(
          pConfig, "config", "k PRIMARY KEY, v", 1, pzErr
      );
    }
    if( rc==SQLITE_OK ){
      rc = sqlite3Fts5StorageConfigValue(p, "version", 0, FTS5_CURRENT_VERSION);
    }
  }

  if( rc ){
    sqlite3Fts5StorageClose(p);
    *pp = 0;
  }
  return rc;
}

/*
** Close a handle opened by an earlier call to sqlite3Fts5StorageOpen().
*/
static int sqlite3Fts5StorageClose(Fts5Storage *p){
  int rc = SQLITE_OK;
  if( p ){
    int i;

    /* Finalize all SQL statements */
    for(i=0; i<ArraySize(p->aStmt); i++){
      sqlite3_finalize(p->aStmt[i]);
    }

    sqlite3_free(p);
  }
  return rc;
}

typedef struct Fts5InsertCtx Fts5InsertCtx;
struct Fts5InsertCtx {
  Fts5Storage *pStorage;
  int iCol;
  int szCol;                      /* Size of column value in tokens */
};

/*
** Tokenization callback used when inserting tokens into the FTS index.
*/
static int fts5StorageInsertCallback(
  void *pContext,                 /* Pointer to Fts5InsertCtx object */
  int tflags,
  const char *pToken,             /* Buffer containing token */
  int nToken,                     /* Size of token in bytes */
  int iUnused1,                   /* Start offset of token */
  int iUnused2                    /* End offset of token */
){
  Fts5InsertCtx *pCtx = (Fts5InsertCtx*)pContext;
  Fts5Index *pIdx = pCtx->pStorage->pIndex;
  UNUSED_PARAM2(iUnused1, iUnused2);
  if( nToken>FTS5_MAX_TOKEN_SIZE ) nToken = FTS5_MAX_TOKEN_SIZE;
  if( (tflags & FTS5_TOKEN_COLOCATED)==0 || pCtx->szCol==0 ){
    pCtx->szCol++;
  }
  return sqlite3Fts5IndexWrite(pIdx, pCtx->iCol, pCtx->szCol-1, pToken, nToken);
}

/*
** If a row with rowid iDel is present in the %_content table, add the
** delete-markers to the FTS index necessary to delete it. Do not actually
** remove the %_content row at this time though.
*/
static int fts5StorageDeleteFromIndex(
  Fts5Storage *p, 
  i64 iDel, 
  sqlite3_value **apVal
){
  Fts5Config *pConfig = p->pConfig;
  sqlite3_stmt *pSeek = 0;        /* SELECT to read row iDel from %_data */
  int rc;                         /* Return code */
  int rc2;                        /* sqlite3_reset() return code */
  int iCol;
  Fts5InsertCtx ctx;

  if( apVal==0 ){
    rc = fts5StorageGetStmt(p, FTS5_STMT_LOOKUP, &pSeek, 0);
    if( rc!=SQLITE_OK ) return rc;
    sqlite3_bind_int64(pSeek, 1, iDel);
    if( sqlite3_step(pSeek)!=SQLITE_ROW ){
      return sqlite3_reset(pSeek);
    }
  }

  ctx.pStorage = p;
  ctx.iCol = -1;
  rc = sqlite3Fts5IndexBeginWrite(p->pIndex, 1, iDel);
  for(iCol=1; rc==SQLITE_OK && iCol<=pConfig->nCol; iCol++){
    if( pConfig->abUnindexed[iCol-1]==0 ){
      const char *zText;
      int nText;
      assert( pSeek==0 || apVal==0 );
      assert( pSeek!=0 || apVal!=0 );
      if( pSeek ){
        zText = (const char*)sqlite3_column_text(pSeek, iCol);
        nText = sqlite3_column_bytes(pSeek, iCol);
      }else if( ALWAYS(apVal) ){
        zText = (const char*)sqlite3_value_text(apVal[iCol-1]);
        nText = sqlite3_value_bytes(apVal[iCol-1]);
      }else{
        continue;
      }
      ctx.szCol = 0;
      rc = sqlite3Fts5Tokenize(pConfig, FTS5_TOKENIZE_DOCUMENT, 
          zText, nText, (void*)&ctx, fts5StorageInsertCallback
      );
      p->aTotalSize[iCol-1] -= (i64)ctx.szCol;
      if( p->aTotalSize[iCol-1]<0 ){
        rc = FTS5_CORRUPT;
      }
    }
  }
  if( rc==SQLITE_OK && p->nTotalRow<1 ){
    rc = FTS5_CORRUPT;
  }else{
    p->nTotalRow--;
  }

  rc2 = sqlite3_reset(pSeek);
  if( rc==SQLITE_OK ) rc = rc2;
  return rc;
}


/*
** Insert a record into the %_docsize table. Specifically, do:
**
**   INSERT OR REPLACE INTO %_docsize(id, sz) VALUES(iRowid, pBuf);
**
** If there is no %_docsize table (as happens if the columnsize=0 option
** is specified when the FTS5 table is created), this function is a no-op.
*/
static int fts5StorageInsertDocsize(
  Fts5Storage *p,                 /* Storage module to write to */
  i64 iRowid,                     /* id value */
  Fts5Buffer *pBuf                /* sz value */
){
  int rc = SQLITE_OK;
  if( p->pConfig->bColumnsize ){
    sqlite3_stmt *pReplace = 0;
    rc = fts5StorageGetStmt(p, FTS5_STMT_REPLACE_DOCSIZE, &pReplace, 0);
    if( rc==SQLITE_OK ){
      sqlite3_bind_int64(pReplace, 1, iRowid);
      sqlite3_bind_blob(pReplace, 2, pBuf->p, pBuf->n, SQLITE_STATIC);
      sqlite3_step(pReplace);
      rc = sqlite3_reset(pReplace);
      sqlite3_bind_null(pReplace, 2);
    }
  }
  return rc;
}

/*
** Load the contents of the "averages" record from disk into the 
** p->nTotalRow and p->aTotalSize[] variables. If successful, and if
** argument bCache is true, set the p->bTotalsValid flag to indicate
** that the contents of aTotalSize[] and nTotalRow are valid until
** further notice.
**
** Return SQLITE_OK if successful, or an SQLite error code if an error
** occurs.
*/
static int fts5StorageLoadTotals(Fts5Storage *p, int bCache){
  int rc = SQLITE_OK;
  if( p->bTotalsValid==0 ){
    rc = sqlite3Fts5IndexGetAverages(p->pIndex, &p->nTotalRow, p->aTotalSize);
    p->bTotalsValid = bCache;
  }
  return rc;
}

/*
** Store the current contents of the p->nTotalRow and p->aTotalSize[] 
** variables in the "averages" record on disk.
**
** Return SQLITE_OK if successful, or an SQLite error code if an error
** occurs.
*/
static int fts5StorageSaveTotals(Fts5Storage *p){
  int nCol = p->pConfig->nCol;
  int i;
  Fts5Buffer buf;
  int rc = SQLITE_OK;
  memset(&buf, 0, sizeof(buf));

  sqlite3Fts5BufferAppendVarint(&rc, &buf, p->nTotalRow);
  for(i=0; i<nCol; i++){
    sqlite3Fts5BufferAppendVarint(&rc, &buf, p->aTotalSize[i]);
  }
  if( rc==SQLITE_OK ){
    rc = sqlite3Fts5IndexSetAverages(p->pIndex, buf.p, buf.n);
  }
  sqlite3_free(buf.p);

  return rc;
}

/*
** Remove a row from the FTS table.
*/
static int sqlite3Fts5StorageDelete(Fts5Storage *p, i64 iDel, sqlite3_value **apVal){
  Fts5Config *pConfig = p->pConfig;
  int rc;
  sqlite3_stmt *pDel = 0;

  assert( pConfig->eContent!=FTS5_CONTENT_NORMAL || apVal==0 );
  rc = fts5StorageLoadTotals(p, 1);

  /* Delete the index records */
  if( rc==SQLITE_OK ){
    rc = fts5StorageDeleteFromIndex(p, iDel, apVal);
  }

  /* Delete the %_docsize record */
  if( rc==SQLITE_OK && pConfig->bColumnsize ){
    rc = fts5StorageGetStmt(p, FTS5_STMT_DELETE_DOCSIZE, &pDel, 0);
    if( rc==SQLITE_OK ){
      sqlite3_bind_int64(pDel, 1, iDel);
      sqlite3_step(pDel);
      rc = sqlite3_reset(pDel);
    }
  }

  /* Delete the %_content record */
  if( pConfig->eContent==FTS5_CONTENT_NORMAL ){
    if( rc==SQLITE_OK ){
      rc = fts5StorageGetStmt(p, FTS5_STMT_DELETE_CONTENT, &pDel, 0);
    }
    if( rc==SQLITE_OK ){
      sqlite3_bind_int64(pDel, 1, iDel);
      sqlite3_step(pDel);
      rc = sqlite3_reset(pDel);
    }
  }

  return rc;
}

/*
** Delete all entries in the FTS5 index.
*/
static int sqlite3Fts5StorageDeleteAll(Fts5Storage *p){
  Fts5Config *pConfig = p->pConfig;
  int rc;

  p->bTotalsValid = 0;

  /* Delete the contents of the %_data and %_docsize tables. */
  rc = fts5ExecPrintf(pConfig->db, 0,
      "DELETE FROM %Q.'%q_data';" 
      "DELETE FROM %Q.'%q_idx';",
      pConfig->zDb, pConfig->zName,
      pConfig->zDb, pConfig->zName
  );
  if( rc==SQLITE_OK && pConfig->bColumnsize ){
    rc = fts5ExecPrintf(pConfig->db, 0,
        "DELETE FROM %Q.'%q_docsize';",
        pConfig->zDb, pConfig->zName
    );
  }

  /* Reinitialize the %_data table. This call creates the initial structure
  ** and averages records.  */
  if( rc==SQLITE_OK ){
    rc = sqlite3Fts5IndexReinit(p->pIndex);
  }
  if( rc==SQLITE_OK ){
    rc = sqlite3Fts5StorageConfigValue(p, "version", 0, FTS5_CURRENT_VERSION);
  }
  return rc;
}

static int sqlite3Fts5StorageRebuild(Fts5Storage *p){
  Fts5Buffer buf = {0,0,0};
  Fts5Config *pConfig = p->pConfig;
  sqlite3_stmt *pScan = 0;
  Fts5InsertCtx ctx;
  int rc, rc2;

  memset(&ctx, 0, sizeof(Fts5InsertCtx));
  ctx.pStorage = p;
  rc = sqlite3Fts5StorageDeleteAll(p);
  if( rc==SQLITE_OK ){
    rc = fts5StorageLoadTotals(p, 1);
  }

  if( rc==SQLITE_OK ){
    rc = fts5StorageGetStmt(p, FTS5_STMT_SCAN, &pScan, 0);
  }

  while( rc==SQLITE_OK && SQLITE_ROW==sqlite3_step(pScan) ){
    i64 iRowid = sqlite3_column_int64(pScan, 0);

    sqlite3Fts5BufferZero(&buf);
    rc = sqlite3Fts5IndexBeginWrite(p->pIndex, 0, iRowid);
    for(ctx.iCol=0; rc==SQLITE_OK && ctx.iCol<pConfig->nCol; ctx.iCol++){
      ctx.szCol = 0;
      if( pConfig->abUnindexed[ctx.iCol]==0 ){
        const char *zText = (const char*)sqlite3_column_text(pScan, ctx.iCol+1);
        int nText = sqlite3_column_bytes(pScan, ctx.iCol+1);
        rc = sqlite3Fts5Tokenize(pConfig, 
            FTS5_TOKENIZE_DOCUMENT,
            zText, nText,
            (void*)&ctx,
            fts5StorageInsertCallback
        );
      }
      sqlite3Fts5BufferAppendVarint(&rc, &buf, ctx.szCol);
      p->aTotalSize[ctx.iCol] += (i64)ctx.szCol;
    }
    p->nTotalRow++;

    if( rc==SQLITE_OK ){
      rc = fts5StorageInsertDocsize(p, iRowid, &buf);
    }
  }
  sqlite3_free(buf.p);
  rc2 = sqlite3_reset(pScan);
  if( rc==SQLITE_OK ) rc = rc2;

  /* Write the averages record */
  if( rc==SQLITE_OK ){
    rc = fts5StorageSaveTotals(p);
  }
  return rc;
}

static int sqlite3Fts5StorageOptimize(Fts5Storage *p){
  return sqlite3Fts5IndexOptimize(p->pIndex);
}

static int sqlite3Fts5StorageMerge(Fts5Storage *p, int nMerge){
  return sqlite3Fts5IndexMerge(p->pIndex, nMerge);
}

static int sqlite3Fts5StorageReset(Fts5Storage *p){
  return sqlite3Fts5IndexReset(p->pIndex);
}

/*
** Allocate a new rowid. This is used for "external content" tables when
** a NULL value is inserted into the rowid column. The new rowid is allocated
** by inserting a dummy row into the %_docsize table. The dummy will be
** overwritten later.
**
** If the %_docsize table does not exist, SQLITE_MISMATCH is returned. In
** this case the user is required to provide a rowid explicitly.
*/
static int fts5StorageNewRowid(Fts5Storage *p, i64 *piRowid){
  int rc = SQLITE_MISMATCH;
  if( p->pConfig->bColumnsize ){
    sqlite3_stmt *pReplace = 0;
    rc = fts5StorageGetStmt(p, FTS5_STMT_REPLACE_DOCSIZE, &pReplace, 0);
    if( rc==SQLITE_OK ){
      sqlite3_bind_null(pReplace, 1);
      sqlite3_bind_null(pReplace, 2);
      sqlite3_step(pReplace);
      rc = sqlite3_reset(pReplace);
    }
    if( rc==SQLITE_OK ){
      *piRowid = sqlite3_last_insert_rowid(p->pConfig->db);
    }
  }
  return rc;
}

/*
** Insert a new row into the FTS content table.
*/
static int sqlite3Fts5StorageContentInsert(
  Fts5Storage *p, 
  sqlite3_value **apVal, 
  i64 *piRowid
){
  Fts5Config *pConfig = p->pConfig;
  int rc = SQLITE_OK;

  /* Insert the new row into the %_content table. */
  if( pConfig->eContent!=FTS5_CONTENT_NORMAL ){
    if( sqlite3_value_type(apVal[1])==SQLITE_INTEGER ){
      *piRowid = sqlite3_value_int64(apVal[1]);
    }else{
      rc = fts5StorageNewRowid(p, piRowid);
    }
  }else{
    sqlite3_stmt *pInsert = 0;    /* Statement to write %_content table */
    int i;                        /* Counter variable */
    rc = fts5StorageGetStmt(p, FTS5_STMT_INSERT_CONTENT, &pInsert, 0);
    for(i=1; rc==SQLITE_OK && i<=pConfig->nCol+1; i++){
      rc = sqlite3_bind_value(pInsert, i, apVal[i]);
    }
    if( rc==SQLITE_OK ){
      sqlite3_step(pInsert);
      rc = sqlite3_reset(pInsert);
    }
    *piRowid = sqlite3_last_insert_rowid(pConfig->db);
  }

  return rc;
}

/*
** Insert new entries into the FTS index and %_docsize table.
*/
static int sqlite3Fts5StorageIndexInsert(
  Fts5Storage *p, 
  sqlite3_value **apVal, 
  i64 iRowid
){
  Fts5Config *pConfig = p->pConfig;
  int rc = SQLITE_OK;             /* Return code */
  Fts5InsertCtx ctx;              /* Tokenization callback context object */
  Fts5Buffer buf;                 /* Buffer used to build up %_docsize blob */

  memset(&buf, 0, sizeof(Fts5Buffer));
  ctx.pStorage = p;
  rc = fts5StorageLoadTotals(p, 1);

  if( rc==SQLITE_OK ){
    rc = sqlite3Fts5IndexBeginWrite(p->pIndex, 0, iRowid);
  }
  for(ctx.iCol=0; rc==SQLITE_OK && ctx.iCol<pConfig->nCol; ctx.iCol++){
    ctx.szCol = 0;
    if( pConfig->abUnindexed[ctx.iCol]==0 ){
      const char *zText = (const char*)sqlite3_value_text(apVal[ctx.iCol+2]);
      int nText = sqlite3_value_bytes(apVal[ctx.iCol+2]);
      rc = sqlite3Fts5Tokenize(pConfig, 
          FTS5_TOKENIZE_DOCUMENT,
          zText, nText,
          (void*)&ctx,
          fts5StorageInsertCallback
      );
    }
    sqlite3Fts5BufferAppendVarint(&rc, &buf, ctx.szCol);
    p->aTotalSize[ctx.iCol] += (i64)ctx.szCol;
  }
  p->nTotalRow++;

  /* Write the %_docsize record */
  if( rc==SQLITE_OK ){
    rc = fts5StorageInsertDocsize(p, iRowid, &buf);
  }
  sqlite3_free(buf.p);

  return rc;
}

static int fts5StorageCount(Fts5Storage *p, const char *zSuffix, i64 *pnRow){
  Fts5Config *pConfig = p->pConfig;
  char *zSql;
  int rc;

  zSql = sqlite3_mprintf("SELECT count(*) FROM %Q.'%q_%s'", 
      pConfig->zDb, pConfig->zName, zSuffix
  );
  if( zSql==0 ){
    rc = SQLITE_NOMEM;
  }else{
    sqlite3_stmt *pCnt = 0;
    rc = sqlite3_prepare_v2(pConfig->db, zSql, -1, &pCnt, 0);
    if( rc==SQLITE_OK ){
      if( SQLITE_ROW==sqlite3_step(pCnt) ){
        *pnRow = sqlite3_column_int64(pCnt, 0);
      }
      rc = sqlite3_finalize(pCnt);
    }
  }

  sqlite3_free(zSql);
  return rc;
}

/*
** Context object used by sqlite3Fts5StorageIntegrity().
*/
typedef struct Fts5IntegrityCtx Fts5IntegrityCtx;
struct Fts5IntegrityCtx {
  i64 iRowid;
  int iCol;
  int szCol;
  u64 cksum;
  Fts5Termset *pTermset;
  Fts5Config *pConfig;
};


/*
** Tokenization callback used by integrity check.
*/
static int fts5StorageIntegrityCallback(
  void *pContext,                 /* Pointer to Fts5IntegrityCtx object */
  int tflags,
  const char *pToken,             /* Buffer containing token */
  int nToken,                     /* Size of token in bytes */
  int iUnused1,                   /* Start offset of token */
  int iUnused2                    /* End offset of token */
){
  Fts5IntegrityCtx *pCtx = (Fts5IntegrityCtx*)pContext;
  Fts5Termset *pTermset = pCtx->pTermset;
  int bPresent;
  int ii;
  int rc = SQLITE_OK;
  int iPos;
  int iCol;

  UNUSED_PARAM2(iUnused1, iUnused2);
  if( nToken>FTS5_MAX_TOKEN_SIZE ) nToken = FTS5_MAX_TOKEN_SIZE;

  if( (tflags & FTS5_TOKEN_COLOCATED)==0 || pCtx->szCol==0 ){
    pCtx->szCol++;
  }

  switch( pCtx->pConfig->eDetail ){
    case FTS5_DETAIL_FULL:
      iPos = pCtx->szCol-1;
      iCol = pCtx->iCol;
      break;

    case FTS5_DETAIL_COLUMNS:
      iPos = pCtx->iCol;
      iCol = 0;
      break;

    default:
      assert( pCtx->pConfig->eDetail==FTS5_DETAIL_NONE );
      iPos = 0;
      iCol = 0;
      break;
  }

  rc = sqlite3Fts5TermsetAdd(pTermset, 0, pToken, nToken, &bPresent);
  if( rc==SQLITE_OK && bPresent==0 ){
    pCtx->cksum ^= sqlite3Fts5IndexEntryCksum(
        pCtx->iRowid, iCol, iPos, 0, pToken, nToken
    );
  }

  for(ii=0; rc==SQLITE_OK && ii<pCtx->pConfig->nPrefix; ii++){
    const int nChar = pCtx->pConfig->aPrefix[ii];
    int nByte = sqlite3Fts5IndexCharlenToBytelen(pToken, nToken, nChar);
    if( nByte ){
      rc = sqlite3Fts5TermsetAdd(pTermset, ii+1, pToken, nByte, &bPresent);
      if( bPresent==0 ){
        pCtx->cksum ^= sqlite3Fts5IndexEntryCksum(
            pCtx->iRowid, iCol, iPos, ii+1, pToken, nByte
        );
      }
    }
  }

  return rc;
}

/*
** Check that the contents of the FTS index match that of the %_content
** table. Return SQLITE_OK if they do, or SQLITE_CORRUPT if not. Return
** some other SQLite error code if an error occurs while attempting to
** determine this.
*/
static int sqlite3Fts5StorageIntegrity(Fts5Storage *p, int iArg){
  Fts5Config *pConfig = p->pConfig;
  int rc = SQLITE_OK;             /* Return code */
  int *aColSize;                  /* Array of size pConfig->nCol */
  i64 *aTotalSize;                /* Array of size pConfig->nCol */
  Fts5IntegrityCtx ctx;
  sqlite3_stmt *pScan;
  int bUseCksum;

  memset(&ctx, 0, sizeof(Fts5IntegrityCtx));
  ctx.pConfig = p->pConfig;
  aTotalSize = (i64*)sqlite3_malloc64(pConfig->nCol*(sizeof(int)+sizeof(i64)));
  if( !aTotalSize ) return SQLITE_NOMEM;
  aColSize = (int*)&aTotalSize[pConfig->nCol];
  memset(aTotalSize, 0, sizeof(i64) * pConfig->nCol);

  bUseCksum = (pConfig->eContent==FTS5_CONTENT_NORMAL
           || (pConfig->eContent==FTS5_CONTENT_EXTERNAL && iArg)
  );
  if( bUseCksum ){
    /* Generate the expected index checksum based on the contents of the
    ** %_content table. This block stores the checksum in ctx.cksum. */
    rc = fts5StorageGetStmt(p, FTS5_STMT_SCAN, &pScan, 0);
    if( rc==SQLITE_OK ){
      int rc2;
      while( SQLITE_ROW==sqlite3_step(pScan) ){
        int i;
        ctx.iRowid = sqlite3_column_int64(pScan, 0);
        ctx.szCol = 0;
        if( pConfig->bColumnsize ){
          rc = sqlite3Fts5StorageDocsize(p, ctx.iRowid, aColSize);
        }
        if( rc==SQLITE_OK && pConfig->eDetail==FTS5_DETAIL_NONE ){
          rc = sqlite3Fts5TermsetNew(&ctx.pTermset);
        }
        for(i=0; rc==SQLITE_OK && i<pConfig->nCol; i++){
          if( pConfig->abUnindexed[i] ) continue;
          ctx.iCol = i;
          ctx.szCol = 0;
          if( pConfig->eDetail==FTS5_DETAIL_COLUMNS ){
            rc = sqlite3Fts5TermsetNew(&ctx.pTermset);
          }
          if( rc==SQLITE_OK ){
            const char *zText = (const char*)sqlite3_column_text(pScan, i+1);
            int nText = sqlite3_column_bytes(pScan, i+1);
            rc = sqlite3Fts5Tokenize(pConfig, 
                FTS5_TOKENIZE_DOCUMENT,
                zText, nText,
                (void*)&ctx,
                fts5StorageIntegrityCallback
            );
          }
          if( rc==SQLITE_OK && pConfig->bColumnsize && ctx.szCol!=aColSize[i] ){
            rc = FTS5_CORRUPT;
          }
          aTotalSize[i] += ctx.szCol;
          if( pConfig->eDetail==FTS5_DETAIL_COLUMNS ){
            sqlite3Fts5TermsetFree(ctx.pTermset);
            ctx.pTermset = 0;
          }
        }
        sqlite3Fts5TermsetFree(ctx.pTermset);
        ctx.pTermset = 0;
  
        if( rc!=SQLITE_OK ) break;
      }
      rc2 = sqlite3_reset(pScan);
      if( rc==SQLITE_OK ) rc = rc2;
    }

    /* Test that the "totals" (sometimes called "averages") record looks Ok */
    if( rc==SQLITE_OK ){
      int i;
      rc = fts5StorageLoadTotals(p, 0);
      for(i=0; rc==SQLITE_OK && i<pConfig->nCol; i++){
        if( p->aTotalSize[i]!=aTotalSize[i] ) rc = FTS5_CORRUPT;
      }
    }

    /* Check that the %_docsize and %_content tables contain the expected
    ** number of rows.  */
    if( rc==SQLITE_OK && pConfig->eContent==FTS5_CONTENT_NORMAL ){
      i64 nRow = 0;
      rc = fts5StorageCount(p, "content", &nRow);
      if( rc==SQLITE_OK && nRow!=p->nTotalRow ) rc = FTS5_CORRUPT;
    }
    if( rc==SQLITE_OK && pConfig->bColumnsize ){
      i64 nRow = 0;
      rc = fts5StorageCount(p, "docsize", &nRow);
      if( rc==SQLITE_OK && nRow!=p->nTotalRow ) rc = FTS5_CORRUPT;
    }
  }

  /* Pass the expected checksum down to the FTS index module. It will
  ** verify, amongst other things, that it matches the checksum generated by
  ** inspecting the index itself.  */
  if( rc==SQLITE_OK ){
    rc = sqlite3Fts5IndexIntegrityCheck(p->pIndex, ctx.cksum, bUseCksum);
  }

  sqlite3_free(aTotalSize);
  return rc;
}

/*
** Obtain an SQLite statement handle that may be used to read data from the
** %_content table.
*/
static int sqlite3Fts5StorageStmt(
  Fts5Storage *p, 
  int eStmt, 
  sqlite3_stmt **pp, 
  char **pzErrMsg
){
  int rc;
  assert( eStmt==FTS5_STMT_SCAN_ASC 
       || eStmt==FTS5_STMT_SCAN_DESC
       || eStmt==FTS5_STMT_LOOKUP
  );
  rc = fts5StorageGetStmt(p, eStmt, pp, pzErrMsg);
  if( rc==SQLITE_OK ){
    assert( p->aStmt[eStmt]==*pp );
    p->aStmt[eStmt] = 0;
  }
  return rc;
}

/*
** Release an SQLite statement handle obtained via an earlier call to
** sqlite3Fts5StorageStmt(). The eStmt parameter passed to this function
** must match that passed to the sqlite3Fts5StorageStmt() call.
*/
static void sqlite3Fts5StorageStmtRelease(
  Fts5Storage *p, 
  int eStmt, 
  sqlite3_stmt *pStmt
){
  assert( eStmt==FTS5_STMT_SCAN_ASC
       || eStmt==FTS5_STMT_SCAN_DESC
       || eStmt==FTS5_STMT_LOOKUP
  );
  if( p->aStmt[eStmt]==0 ){
    sqlite3_reset(pStmt);
    p->aStmt[eStmt] = pStmt;
  }else{
    sqlite3_finalize(pStmt);
  }
}

static int fts5StorageDecodeSizeArray(
  int *aCol, int nCol,            /* Array to populate */
  const u8 *aBlob, int nBlob      /* Record to read varints from */
){
  int i;
  int iOff = 0;
  for(i=0; i<nCol; i++){
    if( iOff>=nBlob ) return 1;
    iOff += fts5GetVarint32(&aBlob[iOff], aCol[i]);
  }
  return (iOff!=nBlob);
}

/*
** Argument aCol points to an array of integers containing one entry for
** each table column. This function reads the %_docsize record for the
** specified rowid and populates aCol[] with the results.
**
** An SQLite error code is returned if an error occurs, or SQLITE_OK
** otherwise.
*/
static int sqlite3Fts5StorageDocsize(Fts5Storage *p, i64 iRowid, int *aCol){
  int nCol = p->pConfig->nCol;    /* Number of user columns in table */
  sqlite3_stmt *pLookup = 0;      /* Statement to query %_docsize */
  int rc;                         /* Return Code */

  assert( p->pConfig->bColumnsize );
  rc = fts5StorageGetStmt(p, FTS5_STMT_LOOKUP_DOCSIZE, &pLookup, 0);
  if( pLookup ){
    int bCorrupt = 1;
    assert( rc==SQLITE_OK );
    sqlite3_bind_int64(pLookup, 1, iRowid);
    if( SQLITE_ROW==sqlite3_step(pLookup) ){
      const u8 *aBlob = sqlite3_column_blob(pLookup, 0);
      int nBlob = sqlite3_column_bytes(pLookup, 0);
      if( 0==fts5StorageDecodeSizeArray(aCol, nCol, aBlob, nBlob) ){
        bCorrupt = 0;
      }
    }
    rc = sqlite3_reset(pLookup);
    if( bCorrupt && rc==SQLITE_OK ){
      rc = FTS5_CORRUPT;
    }
  }else{
    assert( rc!=SQLITE_OK );
  }

  return rc;
}

static int sqlite3Fts5StorageSize(Fts5Storage *p, int iCol, i64 *pnToken){
  int rc = fts5StorageLoadTotals(p, 0);
  if( rc==SQLITE_OK ){
    *pnToken = 0;
    if( iCol<0 ){
      int i;
      for(i=0; i<p->pConfig->nCol; i++){
        *pnToken += p->aTotalSize[i];
      }
    }else if( iCol<p->pConfig->nCol ){
      *pnToken = p->aTotalSize[iCol];
    }else{
      rc = SQLITE_RANGE;
    }
  }
  return rc;
}

static int sqlite3Fts5StorageRowCount(Fts5Storage *p, i64 *pnRow){
  int rc = fts5StorageLoadTotals(p, 0);
  if( rc==SQLITE_OK ){
    /* nTotalRow being zero does not necessarily indicate a corrupt 
    ** database - it might be that the FTS5 table really does contain zero
    ** rows. However this function is only called from the xRowCount() API,
    ** and there is no way for that API to be invoked if the table contains
    ** no rows. Hence the FTS5_CORRUPT return.  */
    *pnRow = p->nTotalRow;
    if( p->nTotalRow<=0 ) rc = FTS5_CORRUPT;
  }
  return rc;
}

/*
** Flush any data currently held in-memory to disk.
*/
static int sqlite3Fts5StorageSync(Fts5Storage *p){
  int rc = SQLITE_OK;
  i64 iLastRowid = sqlite3_last_insert_rowid(p->pConfig->db);
  if( p->bTotalsValid ){
    rc = fts5StorageSaveTotals(p);
    p->bTotalsValid = 0;
  }
  if( rc==SQLITE_OK ){
    rc = sqlite3Fts5IndexSync(p->pIndex);
  }
  sqlite3_set_last_insert_rowid(p->pConfig->db, iLastRowid);
  return rc;
}

static int sqlite3Fts5StorageRollback(Fts5Storage *p){
  p->bTotalsValid = 0;
  return sqlite3Fts5IndexRollback(p->pIndex);
}

static int sqlite3Fts5StorageConfigValue(
  Fts5Storage *p, 
  const char *z,
  sqlite3_value *pVal,
  int iVal
){
  sqlite3_stmt *pReplace = 0;
  int rc = fts5StorageGetStmt(p, FTS5_STMT_REPLACE_CONFIG, &pReplace, 0);
  if( rc==SQLITE_OK ){
    sqlite3_bind_text(pReplace, 1, z, -1, SQLITE_STATIC);
    if( pVal ){
      sqlite3_bind_value(pReplace, 2, pVal);
    }else{
      sqlite3_bind_int(pReplace, 2, iVal);
    }
    sqlite3_step(pReplace);
    rc = sqlite3_reset(pReplace);
    sqlite3_bind_null(pReplace, 1);
  }
  if( rc==SQLITE_OK && pVal ){
    int iNew = p->pConfig->iCookie + 1;
    rc = sqlite3Fts5IndexSetCookie(p->pIndex, iNew);
    if( rc==SQLITE_OK ){
      p->pConfig->iCookie = iNew;
    }
  }
  return rc;
}

#line 1 "fts5_tokenize.c"
/*
** 2014 May 31
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
******************************************************************************
*/


/* #include "fts5Int.h" */

/**************************************************************************
** Start of ascii tokenizer implementation.
*/

/*
** For tokenizers with no "unicode" modifier, the set of token characters
** is the same as the set of ASCII range alphanumeric characters. 
*/
static unsigned char aAsciiTokenChar[128] = {
  0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,   /* 0x00..0x0F */
  0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,   /* 0x10..0x1F */
  0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,   /* 0x20..0x2F */
  1, 1, 1, 1, 1, 1, 1, 1,   1, 1, 0, 0, 0, 0, 0, 0,   /* 0x30..0x3F */
  0, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 1, 1, 1, 1, 1,   /* 0x40..0x4F */
  1, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 0, 0, 0, 0, 0,   /* 0x50..0x5F */
  0, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 1, 1, 1, 1, 1,   /* 0x60..0x6F */
  1, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 0, 0, 0, 0, 0,   /* 0x70..0x7F */
};

typedef struct AsciiTokenizer AsciiTokenizer;
struct AsciiTokenizer {
  unsigned char aTokenChar[128];
};

static void fts5AsciiAddExceptions(
  AsciiTokenizer *p, 
  const char *zArg, 
  int bTokenChars
){
  int i;
  for(i=0; zArg[i]; i++){
    if( (zArg[i] & 0x80)==0 ){
      p->aTokenChar[(int)zArg[i]] = (unsigned char)bTokenChars;
    }
  }
}

/*
** Delete a "ascii" tokenizer.
*/
static void fts5AsciiDelete(Fts5Tokenizer *p){
  sqlite3_free(p);
}

/*
** Create an "ascii" tokenizer.
*/
static int fts5AsciiCreate(
  void *pUnused, 
  const char **azArg, int nArg,
  Fts5Tokenizer **ppOut
){
  int rc = SQLITE_OK;
  AsciiTokenizer *p = 0;
  UNUSED_PARAM(pUnused);
  if( nArg%2 ){
    rc = SQLITE_ERROR;
  }else{
    p = sqlite3_malloc(sizeof(AsciiTokenizer));
    if( p==0 ){
      rc = SQLITE_NOMEM;
    }else{
      int i;
      memset(p, 0, sizeof(AsciiTokenizer));
      memcpy(p->aTokenChar, aAsciiTokenChar, sizeof(aAsciiTokenChar));
      for(i=0; rc==SQLITE_OK && i<nArg; i+=2){
        const char *zArg = azArg[i+1];
        if( 0==sqlite3_stricmp(azArg[i], "tokenchars") ){
          fts5AsciiAddExceptions(p, zArg, 1);
        }else
        if( 0==sqlite3_stricmp(azArg[i], "separators") ){
          fts5AsciiAddExceptions(p, zArg, 0);
        }else{
          rc = SQLITE_ERROR;
        }
      }
      if( rc!=SQLITE_OK ){
        fts5AsciiDelete((Fts5Tokenizer*)p);
        p = 0;
      }
    }
  }

  *ppOut = (Fts5Tokenizer*)p;
  return rc;
}


static void asciiFold(char *aOut, const char *aIn, int nByte){
  int i;
  for(i=0; i<nByte; i++){
    char c = aIn[i];
    if( c>='A' && c<='Z' ) c += 32;
    aOut[i] = c;
  }
}

/*
** Tokenize some text using the ascii tokenizer.
*/
static int fts5AsciiTokenize(
  Fts5Tokenizer *pTokenizer,
  void *pCtx,
  int iUnused,
  const char *pText, int nText,
  int (*xToken)(void*, int, const char*, int nToken, int iStart, int iEnd)
){
  AsciiTokenizer *p = (AsciiTokenizer*)pTokenizer;
  int rc = SQLITE_OK;
  int ie;
  int is = 0;

  char aFold[64];
  int nFold = sizeof(aFold);
  char *pFold = aFold;
  unsigned char *a = p->aTokenChar;

  UNUSED_PARAM(iUnused);

  while( is<nText && rc==SQLITE_OK ){
    int nByte;

    /* Skip any leading divider characters. */
    while( is<nText && ((pText[is]&0x80)==0 && a[(int)pText[is]]==0) ){
      is++;
    }
    if( is==nText ) break;

    /* Count the token characters */
    ie = is+1;
    while( ie<nText && ((pText[ie]&0x80) || a[(int)pText[ie]] ) ){
      ie++;
    }

    /* Fold to lower case */
    nByte = ie-is;
    if( nByte>nFold ){
      if( pFold!=aFold ) sqlite3_free(pFold);
      pFold = sqlite3_malloc64((sqlite3_int64)nByte*2);
      if( pFold==0 ){
        rc = SQLITE_NOMEM;
        break;
      }
      nFold = nByte*2;
    }
    asciiFold(pFold, &pText[is], nByte);

    /* Invoke the token callback */
    rc = xToken(pCtx, 0, pFold, nByte, is, ie);
    is = ie+1;
  }
  
  if( pFold!=aFold ) sqlite3_free(pFold);
  if( rc==SQLITE_DONE ) rc = SQLITE_OK;
  return rc;
}

/**************************************************************************
** Start of unicode61 tokenizer implementation.
*/


/*
** The following two macros - READ_UTF8 and WRITE_UTF8 - have been copied
** from the sqlite3 source file utf.c. If this file is compiled as part
** of the amalgamation, they are not required.
*/
#ifndef SQLITE_AMALGAMATION

static const unsigned char sqlite3Utf8Trans1[] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
  0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x00, 0x00,
};

#define READ_UTF8(zIn, zTerm, c)                           \
  c = *(zIn++);                                            \
  if( c>=0xc0 ){                                           \
    c = sqlite3Utf8Trans1[c-0xc0];                         \
    while( zIn!=zTerm && (*zIn & 0xc0)==0x80 ){            \
      c = (c<<6) + (0x3f & *(zIn++));                      \
    }                                                      \
    if( c<0x80                                             \
        || (c&0xFFFFF800)==0xD800                          \
        || (c&0xFFFFFFFE)==0xFFFE ){  c = 0xFFFD; }        \
  }


#define WRITE_UTF8(zOut, c) {                          \
  if( c<0x00080 ){                                     \
    *zOut++ = (unsigned char)(c&0xFF);                 \
  }                                                    \
  else if( c<0x00800 ){                                \
    *zOut++ = 0xC0 + (unsigned char)((c>>6)&0x1F);     \
    *zOut++ = 0x80 + (unsigned char)(c & 0x3F);        \
  }                                                    \
  else if( c<0x10000 ){                                \
    *zOut++ = 0xE0 + (unsigned char)((c>>12)&0x0F);    \
    *zOut++ = 0x80 + (unsigned char)((c>>6) & 0x3F);   \
    *zOut++ = 0x80 + (unsigned char)(c & 0x3F);        \
  }else{                                               \
    *zOut++ = 0xF0 + (unsigned char)((c>>18) & 0x07);  \
    *zOut++ = 0x80 + (unsigned char)((c>>12) & 0x3F);  \
    *zOut++ = 0x80 + (unsigned char)((c>>6) & 0x3F);   \
    *zOut++ = 0x80 + (unsigned char)(c & 0x3F);        \
  }                                                    \
}

#endif /* ifndef SQLITE_AMALGAMATION */

typedef struct Unicode61Tokenizer Unicode61Tokenizer;
struct Unicode61Tokenizer {
  unsigned char aTokenChar[128];  /* ASCII range token characters */
  char *aFold;                    /* Buffer to fold text into */
  int nFold;                      /* Size of aFold[] in bytes */
  int eRemoveDiacritic;           /* True if remove_diacritics=1 is set */
  int nException;
  int *aiException;

  unsigned char aCategory[32];    /* True for token char categories */
};

/* Values for eRemoveDiacritic (must match internals of fts5_unicode2.c) */
#define FTS5_REMOVE_DIACRITICS_NONE    0
#define FTS5_REMOVE_DIACRITICS_SIMPLE  1
#define FTS5_REMOVE_DIACRITICS_COMPLEX 2

static int fts5UnicodeAddExceptions(
  Unicode61Tokenizer *p,          /* Tokenizer object */
  const char *z,                  /* Characters to treat as exceptions */
  int bTokenChars                 /* 1 for 'tokenchars', 0 for 'separators' */
){
  int rc = SQLITE_OK;
  int n = (int)strlen(z);
  int *aNew;

  if( n>0 ){
    aNew = (int*)sqlite3_realloc64(p->aiException,
                                   (n+p->nException)*sizeof(int));
    if( aNew ){
      int nNew = p->nException;
      const unsigned char *zCsr = (const unsigned char*)z;
      const unsigned char *zTerm = (const unsigned char*)&z[n];
      while( zCsr<zTerm ){
        u32 iCode;
        int bToken;
        READ_UTF8(zCsr, zTerm, iCode);
        if( iCode<128 ){
          p->aTokenChar[iCode] = (unsigned char)bTokenChars;
        }else{
          bToken = p->aCategory[sqlite3Fts5UnicodeCategory(iCode)];
          assert( (bToken==0 || bToken==1) ); 
          assert( (bTokenChars==0 || bTokenChars==1) );
          if( bToken!=bTokenChars && sqlite3Fts5UnicodeIsdiacritic(iCode)==0 ){
            int i;
            for(i=0; i<nNew; i++){
              if( (u32)aNew[i]>iCode ) break;
            }
            memmove(&aNew[i+1], &aNew[i], (nNew-i)*sizeof(int));
            aNew[i] = iCode;
            nNew++;
          }
        }
      }
      p->aiException = aNew;
      p->nException = nNew;
    }else{
      rc = SQLITE_NOMEM;
    }
  }

  return rc;
}

/*
** Return true if the p->aiException[] array contains the value iCode.
*/
static int fts5UnicodeIsException(Unicode61Tokenizer *p, int iCode){
  if( p->nException>0 ){
    int *a = p->aiException;
    int iLo = 0;
    int iHi = p->nException-1;

    while( iHi>=iLo ){
      int iTest = (iHi + iLo) / 2;
      if( iCode==a[iTest] ){
        return 1;
      }else if( iCode>a[iTest] ){
        iLo = iTest+1;
      }else{
        iHi = iTest-1;
      }
    }
  }

  return 0;
}

/*
** Delete a "unicode61" tokenizer.
*/
static void fts5UnicodeDelete(Fts5Tokenizer *pTok){
  if( pTok ){
    Unicode61Tokenizer *p = (Unicode61Tokenizer*)pTok;
    sqlite3_free(p->aiException);
    sqlite3_free(p->aFold);
    sqlite3_free(p);
  }
  return;
}

static int unicodeSetCategories(Unicode61Tokenizer *p, const char *zCat){
  const char *z = zCat;

  while( *z ){
    while( *z==' ' || *z=='\t' ) z++;
    if( *z && sqlite3Fts5UnicodeCatParse(z, p->aCategory) ){
      return SQLITE_ERROR;
    }
    while( *z!=' ' && *z!='\t' && *z!='\0' ) z++;
  }

  sqlite3Fts5UnicodeAscii(p->aCategory, p->aTokenChar);
  return SQLITE_OK;
}

/*
** Create a "unicode61" tokenizer.
*/
static int fts5UnicodeCreate(
  void *pUnused, 
  const char **azArg, int nArg,
  Fts5Tokenizer **ppOut
){
  int rc = SQLITE_OK;             /* Return code */
  Unicode61Tokenizer *p = 0;      /* New tokenizer object */ 

  UNUSED_PARAM(pUnused);

  if( nArg%2 ){
    rc = SQLITE_ERROR;
  }else{
    p = (Unicode61Tokenizer*)sqlite3_malloc(sizeof(Unicode61Tokenizer));
    if( p ){
      const char *zCat = "L* N* Co";
      int i;
      memset(p, 0, sizeof(Unicode61Tokenizer));

      p->eRemoveDiacritic = FTS5_REMOVE_DIACRITICS_SIMPLE;
      p->nFold = 64;
      p->aFold = sqlite3_malloc64(p->nFold * sizeof(char));
      if( p->aFold==0 ){
        rc = SQLITE_NOMEM;
      }

      /* Search for a "categories" argument */
      for(i=0; rc==SQLITE_OK && i<nArg; i+=2){
        if( 0==sqlite3_stricmp(azArg[i], "categories") ){
          zCat = azArg[i+1];
        }
      }

      if( rc==SQLITE_OK ){
        rc = unicodeSetCategories(p, zCat);
      }

      for(i=0; rc==SQLITE_OK && i<nArg; i+=2){
        const char *zArg = azArg[i+1];
        if( 0==sqlite3_stricmp(azArg[i], "remove_diacritics") ){
          if( (zArg[0]!='0' && zArg[0]!='1' && zArg[0]!='2') || zArg[1] ){
            rc = SQLITE_ERROR;
          }else{
            p->eRemoveDiacritic = (zArg[0] - '0');
            assert( p->eRemoveDiacritic==FTS5_REMOVE_DIACRITICS_NONE
                 || p->eRemoveDiacritic==FTS5_REMOVE_DIACRITICS_SIMPLE
                 || p->eRemoveDiacritic==FTS5_REMOVE_DIACRITICS_COMPLEX
            );
          }
        }else
        if( 0==sqlite3_stricmp(azArg[i], "tokenchars") ){
          rc = fts5UnicodeAddExceptions(p, zArg, 1);
        }else
        if( 0==sqlite3_stricmp(azArg[i], "separators") ){
          rc = fts5UnicodeAddExceptions(p, zArg, 0);
        }else
        if( 0==sqlite3_stricmp(azArg[i], "categories") ){
          /* no-op */
        }else{
          rc = SQLITE_ERROR;
        }
      }

    }else{
      rc = SQLITE_NOMEM;
    }
    if( rc!=SQLITE_OK ){
      fts5UnicodeDelete((Fts5Tokenizer*)p);
      p = 0;
    }
    *ppOut = (Fts5Tokenizer*)p;
  }
  return rc;
}

/*
** Return true if, for the purposes of tokenizing with the tokenizer
** passed as the first argument, codepoint iCode is considered a token 
** character (not a separator).
*/
static int fts5UnicodeIsAlnum(Unicode61Tokenizer *p, int iCode){
  return (
    p->aCategory[sqlite3Fts5UnicodeCategory((u32)iCode)]
    ^ fts5UnicodeIsException(p, iCode)
  );
}

static int fts5UnicodeTokenize(
  Fts5Tokenizer *pTokenizer,
  void *pCtx,
  int iUnused,
  const char *pText, int nText,
  int (*xToken)(void*, int, const char*, int nToken, int iStart, int iEnd)
){
  Unicode61Tokenizer *p = (Unicode61Tokenizer*)pTokenizer;
  int rc = SQLITE_OK;
  unsigned char *a = p->aTokenChar;

  unsigned char *zTerm = (unsigned char*)&pText[nText];
  unsigned char *zCsr = (unsigned char *)pText;

  /* Output buffer */
  char *aFold = p->aFold;
  int nFold = p->nFold;
  const char *pEnd = &aFold[nFold-6];

  UNUSED_PARAM(iUnused);

  /* Each iteration of this loop gobbles up a contiguous run of separators,
  ** then the next token.  */
  while( rc==SQLITE_OK ){
    u32 iCode;                    /* non-ASCII codepoint read from input */
    char *zOut = aFold;
    int is;
    int ie;

    /* Skip any separator characters. */
    while( 1 ){
      if( zCsr>=zTerm ) goto tokenize_done;
      if( *zCsr & 0x80 ) {
        /* A character outside of the ascii range. Skip past it if it is
        ** a separator character. Or break out of the loop if it is not. */
        is = zCsr - (unsigned char*)pText;
        READ_UTF8(zCsr, zTerm, iCode);
        if( fts5UnicodeIsAlnum(p, iCode) ){
          goto non_ascii_tokenchar;
        }
      }else{
        if( a[*zCsr] ){
          is = zCsr - (unsigned char*)pText;
          goto ascii_tokenchar;
        }
        zCsr++;
      }
    }

    /* Run through the tokenchars. Fold them into the output buffer along
    ** the way.  */
    while( zCsr<zTerm ){

      /* Grow the output buffer so that there is sufficient space to fit the
      ** largest possible utf-8 character.  */
      if( zOut>pEnd ){
        aFold = sqlite3_malloc64((sqlite3_int64)nFold*2);
        if( aFold==0 ){
          rc = SQLITE_NOMEM;
          goto tokenize_done;
        }
        zOut = &aFold[zOut - p->aFold];
        memcpy(aFold, p->aFold, nFold);
        sqlite3_free(p->aFold);
        p->aFold = aFold;
        p->nFold = nFold = nFold*2;
        pEnd = &aFold[nFold-6];
      }

      if( *zCsr & 0x80 ){
        /* An non-ascii-range character. Fold it into the output buffer if
        ** it is a token character, or break out of the loop if it is not. */
        READ_UTF8(zCsr, zTerm, iCode);
        if( fts5UnicodeIsAlnum(p,iCode)||sqlite3Fts5UnicodeIsdiacritic(iCode) ){
 non_ascii_tokenchar:
          iCode = sqlite3Fts5UnicodeFold(iCode, p->eRemoveDiacritic);
          if( iCode ) WRITE_UTF8(zOut, iCode);
        }else{
          break;
        }
      }else if( a[*zCsr]==0 ){
        /* An ascii-range separator character. End of token. */
        break; 
      }else{
 ascii_tokenchar:
        if( *zCsr>='A' && *zCsr<='Z' ){
          *zOut++ = *zCsr + 32;
        }else{
          *zOut++ = *zCsr;
        }
        zCsr++;
      }
      ie = zCsr - (unsigned char*)pText;
    }

    /* Invoke the token callback */
    rc = xToken(pCtx, 0, aFold, zOut-aFold, is, ie); 
  }
  
 tokenize_done:
  if( rc==SQLITE_DONE ) rc = SQLITE_OK;
  return rc;
}

/**************************************************************************
** Start of porter stemmer implementation.
*/

/* Any tokens larger than this (in bytes) are passed through without
** stemming. */
#define FTS5_PORTER_MAX_TOKEN 64

typedef struct PorterTokenizer PorterTokenizer;
struct PorterTokenizer {
  fts5_tokenizer tokenizer;       /* Parent tokenizer module */
  Fts5Tokenizer *pTokenizer;      /* Parent tokenizer instance */
  char aBuf[FTS5_PORTER_MAX_TOKEN + 64];
};

/*
** Delete a "porter" tokenizer.
*/
static void fts5PorterDelete(Fts5Tokenizer *pTok){
  if( pTok ){
    PorterTokenizer *p = (PorterTokenizer*)pTok;
    if( p->pTokenizer ){
      p->tokenizer.xDelete(p->pTokenizer);
    }
    sqlite3_free(p);
  }
}

/*
** Create a "porter" tokenizer.
*/
static int fts5PorterCreate(
  void *pCtx, 
  const char **azArg, int nArg,
  Fts5Tokenizer **ppOut
){
  fts5_api *pApi = (fts5_api*)pCtx;
  int rc = SQLITE_OK;
  PorterTokenizer *pRet;
  void *pUserdata = 0;
  const char *zBase = "unicode61";

  if( nArg>0 ){
    zBase = azArg[0];
  }

  pRet = (PorterTokenizer*)sqlite3_malloc(sizeof(PorterTokenizer));
  if( pRet ){
    memset(pRet, 0, sizeof(PorterTokenizer));
    rc = pApi->xFindTokenizer(pApi, zBase, &pUserdata, &pRet->tokenizer);
  }else{
    rc = SQLITE_NOMEM;
  }
  if( rc==SQLITE_OK ){
    int nArg2 = (nArg>0 ? nArg-1 : 0);
    const char **azArg2 = (nArg2 ? &azArg[1] : 0);
    rc = pRet->tokenizer.xCreate(pUserdata, azArg2, nArg2, &pRet->pTokenizer);
  }

  if( rc!=SQLITE_OK ){
    fts5PorterDelete((Fts5Tokenizer*)pRet);
    pRet = 0;
  }
  *ppOut = (Fts5Tokenizer*)pRet;
  return rc;
}

typedef struct PorterContext PorterContext;
struct PorterContext {
  void *pCtx;
  int (*xToken)(void*, int, const char*, int, int, int);
  char *aBuf;
};

typedef struct PorterRule PorterRule;
struct PorterRule {
  const char *zSuffix;
  int nSuffix;
  int (*xCond)(char *zStem, int nStem);
  const char *zOutput;
  int nOutput;
};

#if 0
static int fts5PorterApply(char *aBuf, int *pnBuf, PorterRule *aRule){
  int ret = -1;
  int nBuf = *pnBuf;
  PorterRule *p;

  for(p=aRule; p->zSuffix; p++){
    assert( strlen(p->zSuffix)==p->nSuffix );
    assert( strlen(p->zOutput)==p->nOutput );
    if( nBuf<p->nSuffix ) continue;
    if( 0==memcmp(&aBuf[nBuf - p->nSuffix], p->zSuffix, p->nSuffix) ) break;
  }

  if( p->zSuffix ){
    int nStem = nBuf - p->nSuffix;
    if( p->xCond==0 || p->xCond(aBuf, nStem) ){
      memcpy(&aBuf[nStem], p->zOutput, p->nOutput);
      *pnBuf = nStem + p->nOutput;
      ret = p - aRule;
    }
  }

  return ret;
}
#endif

static int fts5PorterIsVowel(char c, int bYIsVowel){
  return (
      c=='a' || c=='e' || c=='i' || c=='o' || c=='u' || (bYIsVowel && c=='y')
  );
}

static int fts5PorterGobbleVC(char *zStem, int nStem, int bPrevCons){
  int i;
  int bCons = bPrevCons;

  /* Scan for a vowel */
  for(i=0; i<nStem; i++){
    if( 0==(bCons = !fts5PorterIsVowel(zStem[i], bCons)) ) break;
  }

  /* Scan for a consonent */
  for(i++; i<nStem; i++){
    if( (bCons = !fts5PorterIsVowel(zStem[i], bCons)) ) return i+1;
  }
  return 0;
}

/* porter rule condition: (m > 0) */
static int fts5Porter_MGt0(char *zStem, int nStem){
  return !!fts5PorterGobbleVC(zStem, nStem, 0);
}

/* porter rule condition: (m > 1) */
static int fts5Porter_MGt1(char *zStem, int nStem){
  int n;
  n = fts5PorterGobbleVC(zStem, nStem, 0);
  if( n && fts5PorterGobbleVC(&zStem[n], nStem-n, 1) ){
    return 1;
  }
  return 0;
}

/* porter rule condition: (m = 1) */
static int fts5Porter_MEq1(char *zStem, int nStem){
  int n;
  n = fts5PorterGobbleVC(zStem, nStem, 0);
  if( n && 0==fts5PorterGobbleVC(&zStem[n], nStem-n, 1) ){
    return 1;
  }
  return 0;
}

/* porter rule condition: (*o) */
static int fts5Porter_Ostar(char *zStem, int nStem){
  if( zStem[nStem-1]=='w' || zStem[nStem-1]=='x' || zStem[nStem-1]=='y' ){
    return 0;
  }else{
    int i;
    int mask = 0;
    int bCons = 0;
    for(i=0; i<nStem; i++){
      bCons = !fts5PorterIsVowel(zStem[i], bCons);
      assert( bCons==0 || bCons==1 );
      mask = (mask << 1) + bCons;
    }
    return ((mask & 0x0007)==0x0005);
  }
}

/* porter rule condition: (m > 1 and (*S or *T)) */
static int fts5Porter_MGt1_and_S_or_T(char *zStem, int nStem){
  assert( nStem>0 );
  return (zStem[nStem-1]=='s' || zStem[nStem-1]=='t') 
      && fts5Porter_MGt1(zStem, nStem);
}

/* porter rule condition: (*v*) */
static int fts5Porter_Vowel(char *zStem, int nStem){
  int i;
  for(i=0; i<nStem; i++){
    if( fts5PorterIsVowel(zStem[i], i>0) ){
      return 1;
    }
  }
  return 0;
}


/**************************************************************************
***************************************************************************
** GENERATED CODE STARTS HERE (mkportersteps.tcl)
*/

static int fts5PorterStep4(char *aBuf, int *pnBuf){
  int ret = 0;
  int nBuf = *pnBuf;
  switch( aBuf[nBuf-2] ){
    
    case 'a': 
      if( nBuf>2 && 0==memcmp("al", &aBuf[nBuf-2], 2) ){
        if( fts5Porter_MGt1(aBuf, nBuf-2) ){
          *pnBuf = nBuf - 2;
        }
      }
      break;
  
    case 'c': 
      if( nBuf>4 && 0==memcmp("ance", &aBuf[nBuf-4], 4) ){
        if( fts5Porter_MGt1(aBuf, nBuf-4) ){
          *pnBuf = nBuf - 4;
        }
      }else if( nBuf>4 && 0==memcmp("ence", &aBuf[nBuf-4], 4) ){
        if( fts5Porter_MGt1(aBuf, nBuf-4) ){
          *pnBuf = nBuf - 4;
        }
      }
      break;
  
    case 'e': 
      if( nBuf>2 && 0==memcmp("er", &aBuf[nBuf-2], 2) ){
        if( fts5Porter_MGt1(aBuf, nBuf-2) ){
          *pnBuf = nBuf - 2;
        }
      }
      break;
  
    case 'i': 
      if( nBuf>2 && 0==memcmp("ic", &aBuf[nBuf-2], 2) ){
        if( fts5Porter_MGt1(aBuf, nBuf-2) ){
          *pnBuf = nBuf - 2;
        }
      }
      break;
  
    case 'l': 
      if( nBuf>4 && 0==memcmp("able", &aBuf[nBuf-4], 4) ){
        if( fts5Porter_MGt1(aBuf, nBuf-4) ){
          *pnBuf = nBuf - 4;
        }
      }else if( nBuf>4 && 0==memcmp("ible", &aBuf[nBuf-4], 4) ){
        if( fts5Porter_MGt1(aBuf, nBuf-4) ){
          *pnBuf = nBuf - 4;
        }
      }
      break;
  
    case 'n': 
      if( nBuf>3 && 0==memcmp("ant", &aBuf[nBuf-3], 3) ){
        if( fts5Porter_MGt1(aBuf, nBuf-3) ){
          *pnBuf = nBuf - 3;
        }
      }else if( nBuf>5 && 0==memcmp("ement", &aBuf[nBuf-5], 5) ){
        if( fts5Porter_MGt1(aBuf, nBuf-5) ){
          *pnBuf = nBuf - 5;
        }
      }else if( nBuf>4 && 0==memcmp("ment", &aBuf[nBuf-4], 4) ){
        if( fts5Porter_MGt1(aBuf, nBuf-4) ){
          *pnBuf = nBuf - 4;
        }
      }else if( nBuf>3 && 0==memcmp("ent", &aBuf[nBuf-3], 3) ){
        if( fts5Porter_MGt1(aBuf, nBuf-3) ){
          *pnBuf = nBuf - 3;
        }
      }
      break;
  
    case 'o': 
      if( nBuf>3 && 0==memcmp("ion", &aBuf[nBuf-3], 3) ){
        if( fts5Porter_MGt1_and_S_or_T(aBuf, nBuf-3) ){
          *pnBuf = nBuf - 3;
        }
      }else if( nBuf>2 && 0==memcmp("ou", &aBuf[nBuf-2], 2) ){
        if( fts5Porter_MGt1(aBuf, nBuf-2) ){
          *pnBuf = nBuf - 2;
        }
      }
      break;
  
    case 's': 
      if( nBuf>3 && 0==memcmp("ism", &aBuf[nBuf-3], 3) ){
        if( fts5Porter_MGt1(aBuf, nBuf-3) ){
          *pnBuf = nBuf - 3;
        }
      }
      break;
  
    case 't': 
      if( nBuf>3 && 0==memcmp("ate", &aBuf[nBuf-3], 3) ){
        if( fts5Porter_MGt1(aBuf, nBuf-3) ){
          *pnBuf = nBuf - 3;
        }
      }else if( nBuf>3 && 0==memcmp("iti", &aBuf[nBuf-3], 3) ){
        if( fts5Porter_MGt1(aBuf, nBuf-3) ){
          *pnBuf = nBuf - 3;
        }
      }
      break;
  
    case 'u': 
      if( nBuf>3 && 0==memcmp("ous", &aBuf[nBuf-3], 3) ){
        if( fts5Porter_MGt1(aBuf, nBuf-3) ){
          *pnBuf = nBuf - 3;
        }
      }
      break;
  
    case 'v': 
      if( nBuf>3 && 0==memcmp("ive", &aBuf[nBuf-3], 3) ){
        if( fts5Porter_MGt1(aBuf, nBuf-3) ){
          *pnBuf = nBuf - 3;
        }
      }
      break;
  
    case 'z': 
      if( nBuf>3 && 0==memcmp("ize", &aBuf[nBuf-3], 3) ){
        if( fts5Porter_MGt1(aBuf, nBuf-3) ){
          *pnBuf = nBuf - 3;
        }
      }
      break;
  
  }
  return ret;
}
  

static int fts5PorterStep1B2(char *aBuf, int *pnBuf){
  int ret = 0;
  int nBuf = *pnBuf;
  switch( aBuf[nBuf-2] ){
    
    case 'a': 
      if( nBuf>2 && 0==memcmp("at", &aBuf[nBuf-2], 2) ){
        memcpy(&aBuf[nBuf-2], "ate", 3);
        *pnBuf = nBuf - 2 + 3;
        ret = 1;
      }
      break;
  
    case 'b': 
      if( nBuf>2 && 0==memcmp("bl", &aBuf[nBuf-2], 2) ){
        memcpy(&aBuf[nBuf-2], "ble", 3);
        *pnBuf = nBuf - 2 + 3;
        ret = 1;
      }
      break;
  
    case 'i': 
      if( nBuf>2 && 0==memcmp("iz", &aBuf[nBuf-2], 2) ){
        memcpy(&aBuf[nBuf-2], "ize", 3);
        *pnBuf = nBuf - 2 + 3;
        ret = 1;
      }
      break;
  
  }
  return ret;
}
  

static int fts5PorterStep2(char *aBuf, int *pnBuf){
  int ret = 0;
  int nBuf = *pnBuf;
  switch( aBuf[nBuf-2] ){
    
    case 'a': 
      if( nBuf>7 && 0==memcmp("ational", &aBuf[nBuf-7], 7) ){
        if( fts5Porter_MGt0(aBuf, nBuf-7) ){
          memcpy(&aBuf[nBuf-7], "ate", 3);
          *pnBuf = nBuf - 7 + 3;
        }
      }else if( nBuf>6 && 0==memcmp("tional", &aBuf[nBuf-6], 6) ){
        if( fts5Porter_MGt0(aBuf, nBuf-6) ){
          memcpy(&aBuf[nBuf-6], "tion", 4);
          *pnBuf = nBuf - 6 + 4;
        }
      }
      break;
  
    case 'c': 
      if( nBuf>4 && 0==memcmp("enci", &aBuf[nBuf-4], 4) ){
        if( fts5Porter_MGt0(aBuf, nBuf-4) ){
          memcpy(&aBuf[nBuf-4], "ence", 4);
          *pnBuf = nBuf - 4 + 4;
        }
      }else if( nBuf>4 && 0==memcmp("anci", &aBuf[nBuf-4], 4) ){
        if( fts5Porter_MGt0(aBuf, nBuf-4) ){
          memcpy(&aBuf[nBuf-4], "ance", 4);
          *pnBuf = nBuf - 4 + 4;
        }
      }
      break;
  
    case 'e': 
      if( nBuf>4 && 0==memcmp("izer", &aBuf[nBuf-4], 4) ){
        if( fts5Porter_MGt0(aBuf, nBuf-4) ){
          memcpy(&aBuf[nBuf-4], "ize", 3);
          *pnBuf = nBuf - 4 + 3;
        }
      }
      break;
  
    case 'g': 
      if( nBuf>4 && 0==memcmp("logi", &aBuf[nBuf-4], 4) ){
        if( fts5Porter_MGt0(aBuf, nBuf-4) ){
          memcpy(&aBuf[nBuf-4], "log", 3);
          *pnBuf = nBuf - 4 + 3;
        }
      }
      break;
  
    case 'l': 
      if( nBuf>3 && 0==memcmp("bli", &aBuf[nBuf-3], 3) ){
        if( fts5Porter_MGt0(aBuf, nBuf-3) ){
          memcpy(&aBuf[nBuf-3], "ble", 3);
          *pnBuf = nBuf - 3 + 3;
        }
      }else if( nBuf>4 && 0==memcmp("alli", &aBuf[nBuf-4], 4) ){
        if( fts5Porter_MGt0(aBuf, nBuf-4) ){
          memcpy(&aBuf[nBuf-4], "al", 2);
          *pnBuf = nBuf - 4 + 2;
        }
      }else if( nBuf>5 && 0==memcmp("entli", &aBuf[nBuf-5], 5) ){
        if( fts5Porter_MGt0(aBuf, nBuf-5) ){
          memcpy(&aBuf[nBuf-5], "ent", 3);
          *pnBuf = nBuf - 5 + 3;
        }
      }else if( nBuf>3 && 0==memcmp("eli", &aBuf[nBuf-3], 3) ){
        if( fts5Porter_MGt0(aBuf, nBuf-3) ){
          memcpy(&aBuf[nBuf-3], "e", 1);
          *pnBuf = nBuf - 3 + 1;
        }
      }else if( nBuf>5 && 0==memcmp("ousli", &aBuf[nBuf-5], 5) ){
        if( fts5Porter_MGt0(aBuf, nBuf-5) ){
          memcpy(&aBuf[nBuf-5], "ous", 3);
          *pnBuf = nBuf - 5 + 3;
        }
      }
      break;
  
    case 'o': 
      if( nBuf>7 && 0==memcmp("ization", &aBuf[nBuf-7], 7) ){
        if( fts5Porter_MGt0(aBuf, nBuf-7) ){
          memcpy(&aBuf[nBuf-7], "ize", 3);
          *pnBuf = nBuf - 7 + 3;
        }
      }else if( nBuf>5 && 0==memcmp("ation", &aBuf[nBuf-5], 5) ){
        if( fts5Porter_MGt0(aBuf, nBuf-5) ){
          memcpy(&aBuf[nBuf-5], "ate", 3);
          *pnBuf = nBuf - 5 + 3;
        }
      }else if( nBuf>4 && 0==memcmp("ator", &aBuf[nBuf-4], 4) ){
        if( fts5Porter_MGt0(aBuf, nBuf-4) ){
          memcpy(&aBuf[nBuf-4], "ate", 3);
          *pnBuf = nBuf - 4 + 3;
        }
      }
      break;
  
    case 's': 
      if( nBuf>5 && 0==memcmp("alism", &aBuf[nBuf-5], 5) ){
        if( fts5Porter_MGt0(aBuf, nBuf-5) ){
          memcpy(&aBuf[nBuf-5], "al", 2);
          *pnBuf = nBuf - 5 + 2;
        }
      }else if( nBuf>7 && 0==memcmp("iveness", &aBuf[nBuf-7], 7) ){
        if( fts5Porter_MGt0(aBuf, nBuf-7) ){
          memcpy(&aBuf[nBuf-7], "ive", 3);
          *pnBuf = nBuf - 7 + 3;
        }
      }else if( nBuf>7 && 0==memcmp("fulness", &aBuf[nBuf-7], 7) ){
        if( fts5Porter_MGt0(aBuf, nBuf-7) ){
          memcpy(&aBuf[nBuf-7], "ful", 3);
          *pnBuf = nBuf - 7 + 3;
        }
      }else if( nBuf>7 && 0==memcmp("ousness", &aBuf[nBuf-7], 7) ){
        if( fts5Porter_MGt0(aBuf, nBuf-7) ){
          memcpy(&aBuf[nBuf-7], "ous", 3);
          *pnBuf = nBuf - 7 + 3;
        }
      }
      break;
  
    case 't': 
      if( nBuf>5 && 0==memcmp("aliti", &aBuf[nBuf-5], 5) ){
        if( fts5Porter_MGt0(aBuf, nBuf-5) ){
          memcpy(&aBuf[nBuf-5], "al", 2);
          *pnBuf = nBuf - 5 + 2;
        }
      }else if( nBuf>5 && 0==memcmp("iviti", &aBuf[nBuf-5], 5) ){
        if( fts5Porter_MGt0(aBuf, nBuf-5) ){
          memcpy(&aBuf[nBuf-5], "ive", 3);
          *pnBuf = nBuf - 5 + 3;
        }
      }else if( nBuf>6 && 0==memcmp("biliti", &aBuf[nBuf-6], 6) ){
        if( fts5Porter_MGt0(aBuf, nBuf-6) ){
          memcpy(&aBuf[nBuf-6], "ble", 3);
          *pnBuf = nBuf - 6 + 3;
        }
      }
      break;
  
  }
  return ret;
}
  

static int fts5PorterStep3(char *aBuf, int *pnBuf){
  int ret = 0;
  int nBuf = *pnBuf;
  switch( aBuf[nBuf-2] ){
    
    case 'a': 
      if( nBuf>4 && 0==memcmp("ical", &aBuf[nBuf-4], 4) ){
        if( fts5Porter_MGt0(aBuf, nBuf-4) ){
          memcpy(&aBuf[nBuf-4], "ic", 2);
          *pnBuf = nBuf - 4 + 2;
        }
      }
      break;
  
    case 's': 
      if( nBuf>4 && 0==memcmp("ness", &aBuf[nBuf-4], 4) ){
        if( fts5Porter_MGt0(aBuf, nBuf-4) ){
          *pnBuf = nBuf - 4;
        }
      }
      break;
  
    case 't': 
      if( nBuf>5 && 0==memcmp("icate", &aBuf[nBuf-5], 5) ){
        if( fts5Porter_MGt0(aBuf, nBuf-5) ){
          memcpy(&aBuf[nBuf-5], "ic", 2);
          *pnBuf = nBuf - 5 + 2;
        }
      }else if( nBuf>5 && 0==memcmp("iciti", &aBuf[nBuf-5], 5) ){
        if( fts5Porter_MGt0(aBuf, nBuf-5) ){
          memcpy(&aBuf[nBuf-5], "ic", 2);
          *pnBuf = nBuf - 5 + 2;
        }
      }
      break;
  
    case 'u': 
      if( nBuf>3 && 0==memcmp("ful", &aBuf[nBuf-3], 3) ){
        if( fts5Porter_MGt0(aBuf, nBuf-3) ){
          *pnBuf = nBuf - 3;
        }
      }
      break;
  
    case 'v': 
      if( nBuf>5 && 0==memcmp("ative", &aBuf[nBuf-5], 5) ){
        if( fts5Porter_MGt0(aBuf, nBuf-5) ){
          *pnBuf = nBuf - 5;
        }
      }
      break;
  
    case 'z': 
      if( nBuf>5 && 0==memcmp("alize", &aBuf[nBuf-5], 5) ){
        if( fts5Porter_MGt0(aBuf, nBuf-5) ){
          memcpy(&aBuf[nBuf-5], "al", 2);
          *pnBuf = nBuf - 5 + 2;
        }
      }
      break;
  
  }
  return ret;
}
  

static int fts5PorterStep1B(char *aBuf, int *pnBuf){
  int ret = 0;
  int nBuf = *pnBuf;
  switch( aBuf[nBuf-2] ){
    
    case 'e': 
      if( nBuf>3 && 0==memcmp("eed", &aBuf[nBuf-3], 3) ){
        if( fts5Porter_MGt0(aBuf, nBuf-3) ){
          memcpy(&aBuf[nBuf-3], "ee", 2);
          *pnBuf = nBuf - 3 + 2;
        }
      }else if( nBuf>2 && 0==memcmp("ed", &aBuf[nBuf-2], 2) ){
        if( fts5Porter_Vowel(aBuf, nBuf-2) ){
          *pnBuf = nBuf - 2;
          ret = 1;
        }
      }
      break;
  
    case 'n': 
      if( nBuf>3 && 0==memcmp("ing", &aBuf[nBuf-3], 3) ){
        if( fts5Porter_Vowel(aBuf, nBuf-3) ){
          *pnBuf = nBuf - 3;
          ret = 1;
        }
      }
      break;
  
  }
  return ret;
}
  
/* 
** GENERATED CODE ENDS HERE (mkportersteps.tcl)
***************************************************************************
**************************************************************************/

static void fts5PorterStep1A(char *aBuf, int *pnBuf){
  int nBuf = *pnBuf;
  if( aBuf[nBuf-1]=='s' ){
    if( aBuf[nBuf-2]=='e' ){
      if( (nBuf>4 && aBuf[nBuf-4]=='s' && aBuf[nBuf-3]=='s') 
       || (nBuf>3 && aBuf[nBuf-3]=='i' )
      ){
        *pnBuf = nBuf-2;
      }else{
        *pnBuf = nBuf-1;
      }
    }
    else if( aBuf[nBuf-2]!='s' ){
      *pnBuf = nBuf-1;
    }
  }
}

static int fts5PorterCb(
  void *pCtx, 
  int tflags,
  const char *pToken, 
  int nToken, 
  int iStart, 
  int iEnd
){
  PorterContext *p = (PorterContext*)pCtx;

  char *aBuf;
  int nBuf;

  if( nToken>FTS5_PORTER_MAX_TOKEN || nToken<3 ) goto pass_through;
  aBuf = p->aBuf;
  nBuf = nToken;
  memcpy(aBuf, pToken, nBuf);

  /* Step 1. */
  fts5PorterStep1A(aBuf, &nBuf);
  if( fts5PorterStep1B(aBuf, &nBuf) ){
    if( fts5PorterStep1B2(aBuf, &nBuf)==0 ){
      char c = aBuf[nBuf-1];
      if( fts5PorterIsVowel(c, 0)==0 
       && c!='l' && c!='s' && c!='z' && c==aBuf[nBuf-2] 
      ){
        nBuf--;
      }else if( fts5Porter_MEq1(aBuf, nBuf) && fts5Porter_Ostar(aBuf, nBuf) ){
        aBuf[nBuf++] = 'e';
      }
    }
  }

  /* Step 1C. */
  if( aBuf[nBuf-1]=='y' && fts5Porter_Vowel(aBuf, nBuf-1) ){
    aBuf[nBuf-1] = 'i';
  }

  /* Steps 2 through 4. */
  fts5PorterStep2(aBuf, &nBuf);
  fts5PorterStep3(aBuf, &nBuf);
  fts5PorterStep4(aBuf, &nBuf);

  /* Step 5a. */
  assert( nBuf>0 );
  if( aBuf[nBuf-1]=='e' ){
    if( fts5Porter_MGt1(aBuf, nBuf-1) 
     || (fts5Porter_MEq1(aBuf, nBuf-1) && !fts5Porter_Ostar(aBuf, nBuf-1))
    ){
      nBuf--;
    }
  }

  /* Step 5b. */
  if( nBuf>1 && aBuf[nBuf-1]=='l' 
   && aBuf[nBuf-2]=='l' && fts5Porter_MGt1(aBuf, nBuf-1) 
  ){
    nBuf--;
  }

  return p->xToken(p->pCtx, tflags, aBuf, nBuf, iStart, iEnd);

 pass_through:
  return p->xToken(p->pCtx, tflags, pToken, nToken, iStart, iEnd);
}

/*
** Tokenize using the porter tokenizer.
*/
static int fts5PorterTokenize(
  Fts5Tokenizer *pTokenizer,
  void *pCtx,
  int flags,
  const char *pText, int nText,
  int (*xToken)(void*, int, const char*, int nToken, int iStart, int iEnd)
){
  PorterTokenizer *p = (PorterTokenizer*)pTokenizer;
  PorterContext sCtx;
  sCtx.xToken = xToken;
  sCtx.pCtx = pCtx;
  sCtx.aBuf = p->aBuf;
  return p->tokenizer.xTokenize(
      p->pTokenizer, (void*)&sCtx, flags, pText, nText, fts5PorterCb
  );
}

/**************************************************************************
** Start of trigram implementation.
*/
typedef struct TrigramTokenizer TrigramTokenizer;
struct TrigramTokenizer {
  int bFold;                      /* True to fold to lower-case */
};

/*
** Free a trigram tokenizer.
*/
static void fts5TriDelete(Fts5Tokenizer *p){
  sqlite3_free(p);
}

/*
** Allocate a trigram tokenizer.
*/
static int fts5TriCreate(
  void *pUnused,
  const char **azArg,
  int nArg,
  Fts5Tokenizer **ppOut
){
  int rc = SQLITE_OK;
  TrigramTokenizer *pNew = (TrigramTokenizer*)sqlite3_malloc(sizeof(*pNew));
  UNUSED_PARAM(pUnused);
  if( pNew==0 ){
    rc = SQLITE_NOMEM;
  }else{
    int i;
    pNew->bFold = 1;
    for(i=0; rc==SQLITE_OK && i<nArg; i+=2){
      const char *zArg = azArg[i+1];
      if( 0==sqlite3_stricmp(azArg[i], "case_sensitive") ){
        if( (zArg[0]!='0' && zArg[0]!='1') || zArg[1] ){
          rc = SQLITE_ERROR;
        }else{
          pNew->bFold = (zArg[0]=='0');
        }
      }else{
        rc = SQLITE_ERROR;
      }
    }
    if( rc!=SQLITE_OK ){
      fts5TriDelete((Fts5Tokenizer*)pNew);
      pNew = 0;
    }
  }
  *ppOut = (Fts5Tokenizer*)pNew;
  return rc;
}

/*
** Trigram tokenizer tokenize routine.
*/
static int fts5TriTokenize(
  Fts5Tokenizer *pTok,
  void *pCtx,
  int unusedFlags,
  const char *pText, int nText,
  int (*xToken)(void*, int, const char*, int, int, int)
){
  TrigramTokenizer *p = (TrigramTokenizer*)pTok;
  int rc = SQLITE_OK;
  char aBuf[32];
  const unsigned char *zIn = (const unsigned char*)pText;
  const unsigned char *zEof = &zIn[nText];
  u32 iCode;

  UNUSED_PARAM(unusedFlags);
  while( 1 ){
    char *zOut = aBuf;
    int iStart = zIn - (const unsigned char*)pText;
    const unsigned char *zNext; 

    READ_UTF8(zIn, zEof, iCode);
    if( iCode==0 ) break;
    zNext = zIn;
    if( zIn<zEof ){
      if( p->bFold ) iCode = sqlite3Fts5UnicodeFold(iCode, 0);
      WRITE_UTF8(zOut, iCode);
      READ_UTF8(zIn, zEof, iCode);
      if( iCode==0 ) break;
    }else{
      break;
    }
    if( zIn<zEof ){
      if( p->bFold ) iCode = sqlite3Fts5UnicodeFold(iCode, 0);
      WRITE_UTF8(zOut, iCode);
      READ_UTF8(zIn, zEof, iCode);
      if( iCode==0 ) break;
      if( p->bFold ) iCode = sqlite3Fts5UnicodeFold(iCode, 0);
      WRITE_UTF8(zOut, iCode);
    }else{
      break;
    }
    rc = xToken(pCtx, 0, aBuf, zOut-aBuf, iStart, iStart + zOut-aBuf);
    if( rc!=SQLITE_OK ) break;
    zIn = zNext;
  }

  return rc;
}

/*
** Argument xCreate is a pointer to a constructor function for a tokenizer.
** pTok is a tokenizer previously created using the same method. This function
** returns one of FTS5_PATTERN_NONE, FTS5_PATTERN_LIKE or FTS5_PATTERN_GLOB
** indicating the style of pattern matching that the tokenizer can support.
** In practice, this is:
**
**     "trigram" tokenizer, case_sensitive=1 - FTS5_PATTERN_GLOB
**     "trigram" tokenizer, case_sensitive=0 (the default) - FTS5_PATTERN_LIKE
**     all other tokenizers - FTS5_PATTERN_NONE
*/
static int sqlite3Fts5TokenizerPattern(
    int (*xCreate)(void*, const char**, int, Fts5Tokenizer**),
    Fts5Tokenizer *pTok
){
  if( xCreate==fts5TriCreate ){
    TrigramTokenizer *p = (TrigramTokenizer*)pTok;
    return p->bFold ? FTS5_PATTERN_LIKE : FTS5_PATTERN_GLOB;
  }
  return FTS5_PATTERN_NONE;
}

/*
** Register all built-in tokenizers with FTS5.
*/
static int sqlite3Fts5TokenizerInit(fts5_api *pApi){
  struct BuiltinTokenizer {
    const char *zName;
    fts5_tokenizer x;
  } aBuiltin[] = {
    { "unicode61", {fts5UnicodeCreate, fts5UnicodeDelete, fts5UnicodeTokenize}},
    { "ascii",     {fts5AsciiCreate, fts5AsciiDelete, fts5AsciiTokenize }},
    { "porter",    {fts5PorterCreate, fts5PorterDelete, fts5PorterTokenize }},
    { "trigram",   {fts5TriCreate, fts5TriDelete, fts5TriTokenize}},
  };
  
  int rc = SQLITE_OK;             /* Return code */
  int i;                          /* To iterate through builtin functions */

  for(i=0; rc==SQLITE_OK && i<ArraySize(aBuiltin); i++){
    rc = pApi->xCreateTokenizer(pApi,
        aBuiltin[i].zName,
        (void*)pApi,
        &aBuiltin[i].x,
        0
    );
  }

  return rc;
}

#line 1 "fts5_unicode2.c"
/*
** 2012-05-25
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
******************************************************************************
*/

/*
** DO NOT EDIT THIS MACHINE GENERATED FILE.
*/


#include "libc/assert.h"



/*
** If the argument is a codepoint corresponding to a lowercase letter
** in the ASCII range with a diacritic added, return the codepoint
** of the ASCII letter only. For example, if passed 235 - "LATIN
** SMALL LETTER E WITH DIAERESIS" - return 65 ("LATIN SMALL LETTER
** E"). The resuls of passing a codepoint that corresponds to an
** uppercase letter are undefined.
*/
static int fts5_remove_diacritic(int c, int bComplex){
  unsigned short aDia[] = {
        0,  1797,  1848,  1859,  1891,  1928,  1940,  1995, 
     2024,  2040,  2060,  2110,  2168,  2206,  2264,  2286, 
     2344,  2383,  2472,  2488,  2516,  2596,  2668,  2732, 
     2782,  2842,  2894,  2954,  2984,  3000,  3028,  3336, 
     3456,  3696,  3712,  3728,  3744,  3766,  3832,  3896, 
     3912,  3928,  3944,  3968,  4008,  4040,  4056,  4106, 
     4138,  4170,  4202,  4234,  4266,  4296,  4312,  4344, 
     4408,  4424,  4442,  4472,  4488,  4504,  6148,  6198, 
     6264,  6280,  6360,  6429,  6505,  6529, 61448, 61468, 
    61512, 61534, 61592, 61610, 61642, 61672, 61688, 61704, 
    61726, 61784, 61800, 61816, 61836, 61880, 61896, 61914, 
    61948, 61998, 62062, 62122, 62154, 62184, 62200, 62218, 
    62252, 62302, 62364, 62410, 62442, 62478, 62536, 62554, 
    62584, 62604, 62640, 62648, 62656, 62664, 62730, 62766, 
    62830, 62890, 62924, 62974, 63032, 63050, 63082, 63118, 
    63182, 63242, 63274, 63310, 63368, 63390, 
  };
#define HIBIT ((unsigned char)0x80)
  unsigned char aChar[] = {
    '\0',      'a',       'c',       'e',       'i',       'n',       
    'o',       'u',       'y',       'y',       'a',       'c',       
    'd',       'e',       'e',       'g',       'h',       'i',       
    'j',       'k',       'l',       'n',       'o',       'r',       
    's',       't',       'u',       'u',       'w',       'y',       
    'z',       'o',       'u',       'a',       'i',       'o',       
    'u',       'u'|HIBIT, 'a'|HIBIT, 'g',       'k',       'o',       
    'o'|HIBIT, 'j',       'g',       'n',       'a'|HIBIT, 'a',       
    'e',       'i',       'o',       'r',       'u',       's',       
    't',       'h',       'a',       'e',       'o'|HIBIT, 'o',       
    'o'|HIBIT, 'y',       '\0',      '\0',      '\0',      '\0',      
    '\0',      '\0',      '\0',      '\0',      'a',       'b',       
    'c'|HIBIT, 'd',       'd',       'e'|HIBIT, 'e',       'e'|HIBIT, 
    'f',       'g',       'h',       'h',       'i',       'i'|HIBIT, 
    'k',       'l',       'l'|HIBIT, 'l',       'm',       'n',       
    'o'|HIBIT, 'p',       'r',       'r'|HIBIT, 'r',       's',       
    's'|HIBIT, 't',       'u',       'u'|HIBIT, 'v',       'w',       
    'w',       'x',       'y',       'z',       'h',       't',       
    'w',       'y',       'a',       'a'|HIBIT, 'a'|HIBIT, 'a'|HIBIT, 
    'e',       'e'|HIBIT, 'e'|HIBIT, 'i',       'o',       'o'|HIBIT, 
    'o'|HIBIT, 'o'|HIBIT, 'u',       'u'|HIBIT, 'u'|HIBIT, 'y',       
  };

  unsigned int key = (((unsigned int)c)<<3) | 0x00000007;
  int iRes = 0;
  int iHi = sizeof(aDia)/sizeof(aDia[0]) - 1;
  int iLo = 0;
  while( iHi>=iLo ){
    int iTest = (iHi + iLo) / 2;
    if( key >= aDia[iTest] ){
      iRes = iTest;
      iLo = iTest+1;
    }else{
      iHi = iTest-1;
    }
  }
  assert( key>=aDia[iRes] );
  if( bComplex==0 && (aChar[iRes] & 0x80) ) return c;
  return (c > (aDia[iRes]>>3) + (aDia[iRes]&0x07)) ? c : ((int)aChar[iRes] & 0x7F);
}


/*
** Return true if the argument interpreted as a unicode codepoint
** is a diacritical modifier character.
*/
static int sqlite3Fts5UnicodeIsdiacritic(int c){
  unsigned int mask0 = 0x08029FDF;
  unsigned int mask1 = 0x000361F8;
  if( c<768 || c>817 ) return 0;
  return (c < 768+32) ?
      (mask0 & ((unsigned int)1 << (c-768))) :
      (mask1 & ((unsigned int)1 << (c-768-32)));
}


/*
** Interpret the argument as a unicode codepoint. If the codepoint
** is an upper case character that has a lower case equivalent,
** return the codepoint corresponding to the lower case version.
** Otherwise, return a copy of the argument.
**
** The results are undefined if the value passed to this function
** is less than zero.
*/
static int sqlite3Fts5UnicodeFold(int c, int eRemoveDiacritic){
  /* Each entry in the following array defines a rule for folding a range
  ** of codepoints to lower case. The rule applies to a range of nRange
  ** codepoints starting at codepoint iCode.
  **
  ** If the least significant bit in flags is clear, then the rule applies
  ** to all nRange codepoints (i.e. all nRange codepoints are upper case and
  ** need to be folded). Or, if it is set, then the rule only applies to
  ** every second codepoint in the range, starting with codepoint C.
  **
  ** The 7 most significant bits in flags are an index into the aiOff[]
  ** array. If a specific codepoint C does require folding, then its lower
  ** case equivalent is ((C + aiOff[flags>>1]) & 0xFFFF).
  **
  ** The contents of this array are generated by parsing the CaseFolding.txt
  ** file distributed as part of the "Unicode Character Database". See
  ** http://www.unicode.org for details.
  */
  static const struct TableEntry {
    unsigned short iCode;
    unsigned char flags;
    unsigned char nRange;
  } aEntry[] = {
    {65, 14, 26},          {181, 64, 1},          {192, 14, 23},
    {216, 14, 7},          {256, 1, 48},          {306, 1, 6},
    {313, 1, 16},          {330, 1, 46},          {376, 116, 1},
    {377, 1, 6},           {383, 104, 1},         {385, 50, 1},
    {386, 1, 4},           {390, 44, 1},          {391, 0, 1},
    {393, 42, 2},          {395, 0, 1},           {398, 32, 1},
    {399, 38, 1},          {400, 40, 1},          {401, 0, 1},
    {403, 42, 1},          {404, 46, 1},          {406, 52, 1},
    {407, 48, 1},          {408, 0, 1},           {412, 52, 1},
    {413, 54, 1},          {415, 56, 1},          {416, 1, 6},
    {422, 60, 1},          {423, 0, 1},           {425, 60, 1},
    {428, 0, 1},           {430, 60, 1},          {431, 0, 1},
    {433, 58, 2},          {435, 1, 4},           {439, 62, 1},
    {440, 0, 1},           {444, 0, 1},           {452, 2, 1},
    {453, 0, 1},           {455, 2, 1},           {456, 0, 1},
    {458, 2, 1},           {459, 1, 18},          {478, 1, 18},
    {497, 2, 1},           {498, 1, 4},           {502, 122, 1},
    {503, 134, 1},         {504, 1, 40},          {544, 110, 1},
    {546, 1, 18},          {570, 70, 1},          {571, 0, 1},
    {573, 108, 1},         {574, 68, 1},          {577, 0, 1},
    {579, 106, 1},         {580, 28, 1},          {581, 30, 1},
    {582, 1, 10},          {837, 36, 1},          {880, 1, 4},
    {886, 0, 1},           {902, 18, 1},          {904, 16, 3},
    {908, 26, 1},          {910, 24, 2},          {913, 14, 17},
    {931, 14, 9},          {962, 0, 1},           {975, 4, 1},
    {976, 140, 1},         {977, 142, 1},         {981, 146, 1},
    {982, 144, 1},         {984, 1, 24},          {1008, 136, 1},
    {1009, 138, 1},        {1012, 130, 1},        {1013, 128, 1},
    {1015, 0, 1},          {1017, 152, 1},        {1018, 0, 1},
    {1021, 110, 3},        {1024, 34, 16},        {1040, 14, 32},
    {1120, 1, 34},         {1162, 1, 54},         {1216, 6, 1},
    {1217, 1, 14},         {1232, 1, 88},         {1329, 22, 38},
    {4256, 66, 38},        {4295, 66, 1},         {4301, 66, 1},
    {7680, 1, 150},        {7835, 132, 1},        {7838, 96, 1},
    {7840, 1, 96},         {7944, 150, 8},        {7960, 150, 6},
    {7976, 150, 8},        {7992, 150, 8},        {8008, 150, 6},
    {8025, 151, 8},        {8040, 150, 8},        {8072, 150, 8},
    {8088, 150, 8},        {8104, 150, 8},        {8120, 150, 2},
    {8122, 126, 2},        {8124, 148, 1},        {8126, 100, 1},
    {8136, 124, 4},        {8140, 148, 1},        {8152, 150, 2},
    {8154, 120, 2},        {8168, 150, 2},        {8170, 118, 2},
    {8172, 152, 1},        {8184, 112, 2},        {8186, 114, 2},
    {8188, 148, 1},        {8486, 98, 1},         {8490, 92, 1},
    {8491, 94, 1},         {8498, 12, 1},         {8544, 8, 16},
    {8579, 0, 1},          {9398, 10, 26},        {11264, 22, 47},
    {11360, 0, 1},         {11362, 88, 1},        {11363, 102, 1},
    {11364, 90, 1},        {11367, 1, 6},         {11373, 84, 1},
    {11374, 86, 1},        {11375, 80, 1},        {11376, 82, 1},
    {11378, 0, 1},         {11381, 0, 1},         {11390, 78, 2},
    {11392, 1, 100},       {11499, 1, 4},         {11506, 0, 1},
    {42560, 1, 46},        {42624, 1, 24},        {42786, 1, 14},
    {42802, 1, 62},        {42873, 1, 4},         {42877, 76, 1},
    {42878, 1, 10},        {42891, 0, 1},         {42893, 74, 1},
    {42896, 1, 4},         {42912, 1, 10},        {42922, 72, 1},
    {65313, 14, 26},       
  };
  static const unsigned short aiOff[] = {
   1,     2,     8,     15,    16,    26,    28,    32,    
   37,    38,    40,    48,    63,    64,    69,    71,    
   79,    80,    116,   202,   203,   205,   206,   207,   
   209,   210,   211,   213,   214,   217,   218,   219,   
   775,   7264,  10792, 10795, 23228, 23256, 30204, 54721, 
   54753, 54754, 54756, 54787, 54793, 54809, 57153, 57274, 
   57921, 58019, 58363, 61722, 65268, 65341, 65373, 65406, 
   65408, 65410, 65415, 65424, 65436, 65439, 65450, 65462, 
   65472, 65476, 65478, 65480, 65482, 65488, 65506, 65511, 
   65514, 65521, 65527, 65528, 65529, 
  };

  int ret = c;

  assert( sizeof(unsigned short)==2 && sizeof(unsigned char)==1 );

  if( c<128 ){
    if( c>='A' && c<='Z' ) ret = c + ('a' - 'A');
  }else if( c<65536 ){
    const struct TableEntry *p;
    int iHi = sizeof(aEntry)/sizeof(aEntry[0]) - 1;
    int iLo = 0;
    int iRes = -1;

    assert( c>aEntry[0].iCode );
    while( iHi>=iLo ){
      int iTest = (iHi + iLo) / 2;
      int cmp = (c - aEntry[iTest].iCode);
      if( cmp>=0 ){
        iRes = iTest;
        iLo = iTest+1;
      }else{
        iHi = iTest-1;
      }
    }

    assert( iRes>=0 && c>=aEntry[iRes].iCode );
    p = &aEntry[iRes];
    if( c<(p->iCode + p->nRange) && 0==(0x01 & p->flags & (p->iCode ^ c)) ){
      ret = (c + (aiOff[p->flags>>1])) & 0x0000FFFF;
      assert( ret>0 );
    }

    if( eRemoveDiacritic ){
      ret = fts5_remove_diacritic(ret, eRemoveDiacritic==2);
    }
  }
  
  else if( c>=66560 && c<66600 ){
    ret = c + 40;
  }

  return ret;
}


static int sqlite3Fts5UnicodeCatParse(const char *zCat, u8 *aArray){ 
  aArray[0] = 1;
  switch( zCat[0] ){
    case 'C':
          switch( zCat[1] ){
            case 'c': aArray[1] = 1; break;
            case 'f': aArray[2] = 1; break;
            case 'n': aArray[3] = 1; break;
            case 's': aArray[4] = 1; break;
            case 'o': aArray[31] = 1; break;
            case '*': 
              aArray[1] = 1;
              aArray[2] = 1;
              aArray[3] = 1;
              aArray[4] = 1;
              aArray[31] = 1;
              break;
            default: return 1;          }
          break;

    case 'L':
          switch( zCat[1] ){
            case 'l': aArray[5] = 1; break;
            case 'm': aArray[6] = 1; break;
            case 'o': aArray[7] = 1; break;
            case 't': aArray[8] = 1; break;
            case 'u': aArray[9] = 1; break;
            case 'C': aArray[30] = 1; break;
            case '*': 
              aArray[5] = 1;
              aArray[6] = 1;
              aArray[7] = 1;
              aArray[8] = 1;
              aArray[9] = 1;
              aArray[30] = 1;
              break;
            default: return 1;          }
          break;

    case 'M':
          switch( zCat[1] ){
            case 'c': aArray[10] = 1; break;
            case 'e': aArray[11] = 1; break;
            case 'n': aArray[12] = 1; break;
            case '*': 
              aArray[10] = 1;
              aArray[11] = 1;
              aArray[12] = 1;
              break;
            default: return 1;          }
          break;

    case 'N':
          switch( zCat[1] ){
            case 'd': aArray[13] = 1; break;
            case 'l': aArray[14] = 1; break;
            case 'o': aArray[15] = 1; break;
            case '*': 
              aArray[13] = 1;
              aArray[14] = 1;
              aArray[15] = 1;
              break;
            default: return 1;          }
          break;

    case 'P':
          switch( zCat[1] ){
            case 'c': aArray[16] = 1; break;
            case 'd': aArray[17] = 1; break;
            case 'e': aArray[18] = 1; break;
            case 'f': aArray[19] = 1; break;
            case 'i': aArray[20] = 1; break;
            case 'o': aArray[21] = 1; break;
            case 's': aArray[22] = 1; break;
            case '*': 
              aArray[16] = 1;
              aArray[17] = 1;
              aArray[18] = 1;
              aArray[19] = 1;
              aArray[20] = 1;
              aArray[21] = 1;
              aArray[22] = 1;
              break;
            default: return 1;          }
          break;

    case 'S':
          switch( zCat[1] ){
            case 'c': aArray[23] = 1; break;
            case 'k': aArray[24] = 1; break;
            case 'm': aArray[25] = 1; break;
            case 'o': aArray[26] = 1; break;
            case '*': 
              aArray[23] = 1;
              aArray[24] = 1;
              aArray[25] = 1;
              aArray[26] = 1;
              break;
            default: return 1;          }
          break;

    case 'Z':
          switch( zCat[1] ){
            case 'l': aArray[27] = 1; break;
            case 'p': aArray[28] = 1; break;
            case 's': aArray[29] = 1; break;
            case '*': 
              aArray[27] = 1;
              aArray[28] = 1;
              aArray[29] = 1;
              break;
            default: return 1;          }
          break;

  }
  return 0;
}

static u16 aFts5UnicodeBlock[] = {
    0,     1471,  1753,  1760,  1760,  1760,  1760,  1760,  1760,  1760,  
    1760,  1760,  1760,  1760,  1760,  1763,  1765,  
  };
static u16 aFts5UnicodeMap[] = {
    0,     32,    33,    36,    37,    40,    41,    42,    43,    44,    
    45,    46,    48,    58,    60,    63,    65,    91,    92,    93,    
    94,    95,    96,    97,    123,   124,   125,   126,   127,   160,   
    161,   162,   166,   167,   168,   169,   170,   171,   172,   173,   
    174,   175,   176,   177,   178,   180,   181,   182,   184,   185,   
    186,   187,   188,   191,   192,   215,   216,   223,   247,   248,   
    256,   312,   313,   329,   330,   377,   383,   385,   387,   388,   
    391,   394,   396,   398,   402,   403,   405,   406,   409,   412,   
    414,   415,   417,   418,   423,   427,   428,   431,   434,   436,   
    437,   440,   442,   443,   444,   446,   448,   452,   453,   454,   
    455,   456,   457,   458,   459,   460,   461,   477,   478,   496,   
    497,   498,   499,   500,   503,   505,   506,   564,   570,   572,   
    573,   575,   577,   580,   583,   584,   592,   660,   661,   688,   
    706,   710,   722,   736,   741,   748,   749,   750,   751,   768,   
    880,   884,   885,   886,   890,   891,   894,   900,   902,   903,   
    904,   908,   910,   912,   913,   931,   940,   975,   977,   978,   
    981,   984,   1008,  1012,  1014,  1015,  1018,  1020,  1021,  1072,  
    1120,  1154,  1155,  1160,  1162,  1217,  1231,  1232,  1329,  1369,  
    1370,  1377,  1417,  1418,  1423,  1425,  1470,  1471,  1472,  1473,  
    1475,  1476,  1478,  1479,  1488,  1520,  1523,  1536,  1542,  1545,  
    1547,  1548,  1550,  1552,  1563,  1566,  1568,  1600,  1601,  1611,  
    1632,  1642,  1646,  1648,  1649,  1748,  1749,  1750,  1757,  1758,  
    1759,  1765,  1767,  1769,  1770,  1774,  1776,  1786,  1789,  1791,  
    1792,  1807,  1808,  1809,  1810,  1840,  1869,  1958,  1969,  1984,  
    1994,  2027,  2036,  2038,  2039,  2042,  2048,  2070,  2074,  2075,  
    2084,  2085,  2088,  2089,  2096,  2112,  2137,  2142,  2208,  2210,  
    2276,  2304,  2307,  2308,  2362,  2363,  2364,  2365,  2366,  2369,  
    2377,  2381,  2382,  2384,  2385,  2392,  2402,  2404,  2406,  2416,  
    2417,  2418,  2425,  2433,  2434,  2437,  2447,  2451,  2474,  2482,  
    2486,  2492,  2493,  2494,  2497,  2503,  2507,  2509,  2510,  2519,  
    2524,  2527,  2530,  2534,  2544,  2546,  2548,  2554,  2555,  2561,  
    2563,  2565,  2575,  2579,  2602,  2610,  2613,  2616,  2620,  2622,  
    2625,  2631,  2635,  2641,  2649,  2654,  2662,  2672,  2674,  2677,  
    2689,  2691,  2693,  2703,  2707,  2730,  2738,  2741,  2748,  2749,  
    2750,  2753,  2759,  2761,  2763,  2765,  2768,  2784,  2786,  2790,  
    2800,  2801,  2817,  2818,  2821,  2831,  2835,  2858,  2866,  2869,  
    2876,  2877,  2878,  2879,  2880,  2881,  2887,  2891,  2893,  2902,  
    2903,  2908,  2911,  2914,  2918,  2928,  2929,  2930,  2946,  2947,  
    2949,  2958,  2962,  2969,  2972,  2974,  2979,  2984,  2990,  3006,  
    3008,  3009,  3014,  3018,  3021,  3024,  3031,  3046,  3056,  3059,  
    3065,  3066,  3073,  3077,  3086,  3090,  3114,  3125,  3133,  3134,  
    3137,  3142,  3146,  3157,  3160,  3168,  3170,  3174,  3192,  3199,  
    3202,  3205,  3214,  3218,  3242,  3253,  3260,  3261,  3262,  3263,  
    3264,  3270,  3271,  3274,  3276,  3285,  3294,  3296,  3298,  3302,  
    3313,  3330,  3333,  3342,  3346,  3389,  3390,  3393,  3398,  3402,  
    3405,  3406,  3415,  3424,  3426,  3430,  3440,  3449,  3450,  3458,  
    3461,  3482,  3507,  3517,  3520,  3530,  3535,  3538,  3542,  3544,  
    3570,  3572,  3585,  3633,  3634,  3636,  3647,  3648,  3654,  3655,  
    3663,  3664,  3674,  3713,  3716,  3719,  3722,  3725,  3732,  3737,  
    3745,  3749,  3751,  3754,  3757,  3761,  3762,  3764,  3771,  3773,  
    3776,  3782,  3784,  3792,  3804,  3840,  3841,  3844,  3859,  3860,  
    3861,  3864,  3866,  3872,  3882,  3892,  3893,  3894,  3895,  3896,  
    3897,  3898,  3899,  3900,  3901,  3902,  3904,  3913,  3953,  3967,  
    3968,  3973,  3974,  3976,  3981,  3993,  4030,  4038,  4039,  4046,  
    4048,  4053,  4057,  4096,  4139,  4141,  4145,  4146,  4152,  4153,  
    4155,  4157,  4159,  4160,  4170,  4176,  4182,  4184,  4186,  4190,  
    4193,  4194,  4197,  4199,  4206,  4209,  4213,  4226,  4227,  4229,  
    4231,  4237,  4238,  4239,  4240,  4250,  4253,  4254,  4256,  4295,  
    4301,  4304,  4347,  4348,  4349,  4682,  4688,  4696,  4698,  4704,  
    4746,  4752,  4786,  4792,  4800,  4802,  4808,  4824,  4882,  4888,  
    4957,  4960,  4969,  4992,  5008,  5024,  5120,  5121,  5741,  5743,  
    5760,  5761,  5787,  5788,  5792,  5867,  5870,  5888,  5902,  5906,  
    5920,  5938,  5941,  5952,  5970,  5984,  5998,  6002,  6016,  6068,  
    6070,  6071,  6078,  6086,  6087,  6089,  6100,  6103,  6104,  6107,  
    6108,  6109,  6112,  6128,  6144,  6150,  6151,  6155,  6158,  6160,  
    6176,  6211,  6212,  6272,  6313,  6314,  6320,  6400,  6432,  6435,  
    6439,  6441,  6448,  6450,  6451,  6457,  6464,  6468,  6470,  6480,  
    6512,  6528,  6576,  6593,  6600,  6608,  6618,  6622,  6656,  6679,  
    6681,  6686,  6688,  6741,  6742,  6743,  6744,  6752,  6753,  6754,  
    6755,  6757,  6765,  6771,  6783,  6784,  6800,  6816,  6823,  6824,  
    6912,  6916,  6917,  6964,  6965,  6966,  6971,  6972,  6973,  6978,  
    6979,  6981,  6992,  7002,  7009,  7019,  7028,  7040,  7042,  7043,  
    7073,  7074,  7078,  7080,  7082,  7083,  7084,  7086,  7088,  7098,  
    7142,  7143,  7144,  7146,  7149,  7150,  7151,  7154,  7164,  7168,  
    7204,  7212,  7220,  7222,  7227,  7232,  7245,  7248,  7258,  7288,  
    7294,  7360,  7376,  7379,  7380,  7393,  7394,  7401,  7405,  7406,  
    7410,  7412,  7413,  7424,  7468,  7531,  7544,  7545,  7579,  7616,  
    7676,  7680,  7830,  7838,  7936,  7944,  7952,  7960,  7968,  7976,  
    7984,  7992,  8000,  8008,  8016,  8025,  8027,  8029,  8031,  8033,  
    8040,  8048,  8064,  8072,  8080,  8088,  8096,  8104,  8112,  8118,  
    8120,  8124,  8125,  8126,  8127,  8130,  8134,  8136,  8140,  8141,  
    8144,  8150,  8152,  8157,  8160,  8168,  8173,  8178,  8182,  8184,  
    8188,  8189,  8192,  8203,  8208,  8214,  8216,  8217,  8218,  8219,  
    8221,  8222,  8223,  8224,  8232,  8233,  8234,  8239,  8240,  8249,  
    8250,  8251,  8255,  8257,  8260,  8261,  8262,  8263,  8274,  8275,  
    8276,  8277,  8287,  8288,  8298,  8304,  8305,  8308,  8314,  8317,  
    8318,  8319,  8320,  8330,  8333,  8334,  8336,  8352,  8400,  8413,  
    8417,  8418,  8421,  8448,  8450,  8451,  8455,  8456,  8458,  8459,  
    8462,  8464,  8467,  8468,  8469,  8470,  8472,  8473,  8478,  8484,  
    8485,  8486,  8487,  8488,  8489,  8490,  8494,  8495,  8496,  8500,  
    8501,  8505,  8506,  8508,  8510,  8512,  8517,  8519,  8522,  8523,  
    8524,  8526,  8527,  8528,  8544,  8579,  8581,  8585,  8592,  8597,  
    8602,  8604,  8608,  8609,  8611,  8612,  8614,  8615,  8622,  8623,  
    8654,  8656,  8658,  8659,  8660,  8661,  8692,  8960,  8968,  8972,  
    8992,  8994,  9001,  9002,  9003,  9084,  9085,  9115,  9140,  9180,  
    9186,  9216,  9280,  9312,  9372,  9450,  9472,  9655,  9656,  9665,  
    9666,  9720,  9728,  9839,  9840,  9985,  10088, 10089, 10090, 10091, 
    10092, 10093, 10094, 10095, 10096, 10097, 10098, 10099, 10100, 10101, 
    10102, 10132, 10176, 10181, 10182, 10183, 10214, 10215, 10216, 10217, 
    10218, 10219, 10220, 10221, 10222, 10223, 10224, 10240, 10496, 10627, 
    10628, 10629, 10630, 10631, 10632, 10633, 10634, 10635, 10636, 10637, 
    10638, 10639, 10640, 10641, 10642, 10643, 10644, 10645, 10646, 10647, 
    10648, 10649, 10712, 10713, 10714, 10715, 10716, 10748, 10749, 10750, 
    11008, 11056, 11077, 11079, 11088, 11264, 11312, 11360, 11363, 11365, 
    11367, 11374, 11377, 11378, 11380, 11381, 11383, 11388, 11390, 11393, 
    11394, 11492, 11493, 11499, 11503, 11506, 11513, 11517, 11518, 11520, 
    11559, 11565, 11568, 11631, 11632, 11647, 11648, 11680, 11688, 11696, 
    11704, 11712, 11720, 11728, 11736, 11744, 11776, 11778, 11779, 11780, 
    11781, 11782, 11785, 11786, 11787, 11788, 11789, 11790, 11799, 11800, 
    11802, 11803, 11804, 11805, 11806, 11808, 11809, 11810, 11811, 11812, 
    11813, 11814, 11815, 11816, 11817, 11818, 11823, 11824, 11834, 11904, 
    11931, 12032, 12272, 12288, 12289, 12292, 12293, 12294, 12295, 12296, 
    12297, 12298, 12299, 12300, 12301, 12302, 12303, 12304, 12305, 12306, 
    12308, 12309, 12310, 12311, 12312, 12313, 12314, 12315, 12316, 12317, 
    12318, 12320, 12321, 12330, 12334, 12336, 12337, 12342, 12344, 12347, 
    12348, 12349, 12350, 12353, 12441, 12443, 12445, 12447, 12448, 12449, 
    12539, 12540, 12543, 12549, 12593, 12688, 12690, 12694, 12704, 12736, 
    12784, 12800, 12832, 12842, 12872, 12880, 12881, 12896, 12928, 12938, 
    12977, 12992, 13056, 13312, 19893, 19904, 19968, 40908, 40960, 40981, 
    40982, 42128, 42192, 42232, 42238, 42240, 42508, 42509, 42512, 42528, 
    42538, 42560, 42606, 42607, 42608, 42611, 42612, 42622, 42623, 42624, 
    42655, 42656, 42726, 42736, 42738, 42752, 42775, 42784, 42786, 42800, 
    42802, 42864, 42865, 42873, 42878, 42888, 42889, 42891, 42896, 42912, 
    43000, 43002, 43003, 43010, 43011, 43014, 43015, 43019, 43020, 43043, 
    43045, 43047, 43048, 43056, 43062, 43064, 43065, 43072, 43124, 43136, 
    43138, 43188, 43204, 43214, 43216, 43232, 43250, 43256, 43259, 43264, 
    43274, 43302, 43310, 43312, 43335, 43346, 43359, 43360, 43392, 43395, 
    43396, 43443, 43444, 43446, 43450, 43452, 43453, 43457, 43471, 43472, 
    43486, 43520, 43561, 43567, 43569, 43571, 43573, 43584, 43587, 43588, 
    43596, 43597, 43600, 43612, 43616, 43632, 43633, 43639, 43642, 43643, 
    43648, 43696, 43697, 43698, 43701, 43703, 43705, 43710, 43712, 43713, 
    43714, 43739, 43741, 43742, 43744, 43755, 43756, 43758, 43760, 43762, 
    43763, 43765, 43766, 43777, 43785, 43793, 43808, 43816, 43968, 44003, 
    44005, 44006, 44008, 44009, 44011, 44012, 44013, 44016, 44032, 55203, 
    55216, 55243, 55296, 56191, 56319, 57343, 57344, 63743, 63744, 64112, 
    64256, 64275, 64285, 64286, 64287, 64297, 64298, 64312, 64318, 64320, 
    64323, 64326, 64434, 64467, 64830, 64831, 64848, 64914, 65008, 65020, 
    65021, 65024, 65040, 65047, 65048, 65049, 65056, 65072, 65073, 65075, 
    65077, 65078, 65079, 65080, 65081, 65082, 65083, 65084, 65085, 65086, 
    65087, 65088, 65089, 65090, 65091, 65092, 65093, 65095, 65096, 65097, 
    65101, 65104, 65108, 65112, 65113, 65114, 65115, 65116, 65117, 65118, 
    65119, 65122, 65123, 65124, 65128, 65129, 65130, 65136, 65142, 65279, 
    65281, 65284, 65285, 65288, 65289, 65290, 65291, 65292, 65293, 65294, 
    65296, 65306, 65308, 65311, 65313, 65339, 65340, 65341, 65342, 65343, 
    65344, 65345, 65371, 65372, 65373, 65374, 65375, 65376, 65377, 65378, 
    65379, 65380, 65382, 65392, 65393, 65438, 65440, 65474, 65482, 65490, 
    65498, 65504, 65506, 65507, 65508, 65509, 65512, 65513, 65517, 65529, 
    65532, 0,     13,    40,    60,    63,    80,    128,   256,   263,   
    311,   320,   373,   377,   394,   400,   464,   509,   640,   672,   
    768,   800,   816,   833,   834,   842,   896,   927,   928,   968,   
    976,   977,   1024,  1064,  1104,  1184,  2048,  2056,  2058,  2103,  
    2108,  2111,  2135,  2136,  2304,  2326,  2335,  2336,  2367,  2432,  
    2494,  2560,  2561,  2565,  2572,  2576,  2581,  2585,  2616,  2623,  
    2624,  2640,  2656,  2685,  2687,  2816,  2873,  2880,  2904,  2912,  
    2936,  3072,  3680,  4096,  4097,  4098,  4099,  4152,  4167,  4178,  
    4198,  4224,  4226,  4227,  4272,  4275,  4279,  4281,  4283,  4285,  
    4286,  4304,  4336,  4352,  4355,  4391,  4396,  4397,  4406,  4416,  
    4480,  4482,  4483,  4531,  4534,  4543,  4545,  4549,  4560,  5760,  
    5803,  5804,  5805,  5806,  5808,  5814,  5815,  5824,  8192,  9216,  
    9328,  12288, 26624, 28416, 28496, 28497, 28559, 28563, 45056, 53248, 
    53504, 53545, 53605, 53607, 53610, 53613, 53619, 53627, 53635, 53637, 
    53644, 53674, 53678, 53760, 53826, 53829, 54016, 54112, 54272, 54298, 
    54324, 54350, 54358, 54376, 54402, 54428, 54430, 54434, 54437, 54441, 
    54446, 54454, 54459, 54461, 54469, 54480, 54506, 54532, 54535, 54541, 
    54550, 54558, 54584, 54587, 54592, 54598, 54602, 54610, 54636, 54662, 
    54688, 54714, 54740, 54766, 54792, 54818, 54844, 54870, 54896, 54922, 
    54952, 54977, 54978, 55003, 55004, 55010, 55035, 55036, 55061, 55062, 
    55068, 55093, 55094, 55119, 55120, 55126, 55151, 55152, 55177, 55178, 
    55184, 55209, 55210, 55235, 55236, 55242, 55246, 60928, 60933, 60961, 
    60964, 60967, 60969, 60980, 60985, 60987, 60994, 60999, 61001, 61003, 
    61005, 61009, 61012, 61015, 61017, 61019, 61021, 61023, 61025, 61028, 
    61031, 61036, 61044, 61049, 61054, 61056, 61067, 61089, 61093, 61099, 
    61168, 61440, 61488, 61600, 61617, 61633, 61649, 61696, 61712, 61744, 
    61808, 61926, 61968, 62016, 62032, 62208, 62256, 62263, 62336, 62368, 
    62406, 62432, 62464, 62528, 62530, 62713, 62720, 62784, 62800, 62971, 
    63045, 63104, 63232, 0,     42710, 42752, 46900, 46912, 47133, 63488, 
    1,     32,    256,   0,     65533, 
  };
static u16 aFts5UnicodeData[] = {
    1025,  61,    117,   55,    117,   54,    50,    53,    57,    53,    
    49,    85,    333,   85,    121,   85,    841,   54,    53,    50,    
    56,    48,    56,    837,   54,    57,    50,    57,    1057,  61,    
    53,    151,   58,    53,    56,    58,    39,    52,    57,    34,    
    58,    56,    58,    57,    79,    56,    37,    85,    56,    47,    
    39,    51,    111,   53,    745,   57,    233,   773,   57,    261,   
    1822,  37,    542,   37,    1534,  222,   69,    73,    37,    126,   
    126,   73,    69,    137,   37,    73,    37,    105,   101,   73,    
    37,    73,    37,    190,   158,   37,    126,   126,   73,    37,    
    126,   94,    37,    39,    94,    69,    135,   41,    40,    37,    
    41,    40,    37,    41,    40,    37,    542,   37,    606,   37,    
    41,    40,    37,    126,   73,    37,    1886,  197,   73,    37,    
    73,    69,    126,   105,   37,    286,   2181,  39,    869,   582,   
    152,   390,   472,   166,   248,   38,    56,    38,    568,   3596,  
    158,   38,    56,    94,    38,    101,   53,    88,    41,    53,    
    105,   41,    73,    37,    553,   297,   1125,  94,    37,    105,   
    101,   798,   133,   94,    57,    126,   94,    37,    1641,  1541,  
    1118,  58,    172,   75,    1790,  478,   37,    2846,  1225,  38,    
    213,   1253,  53,    49,    55,    1452,  49,    44,    53,    76,    
    53,    76,    53,    44,    871,   103,   85,    162,   121,   85,    
    55,    85,    90,    364,   53,    85,    1031,  38,    327,   684,   
    333,   149,   71,    44,    3175,  53,    39,    236,   34,    58,    
    204,   70,    76,    58,    140,   71,    333,   103,   90,    39,    
    469,   34,    39,    44,    967,   876,   2855,  364,   39,    333,   
    1063,  300,   70,    58,    117,   38,    711,   140,   38,    300,   
    38,    108,   38,    172,   501,   807,   108,   53,    39,    359,   
    876,   108,   42,    1735,  44,    42,    44,    39,    106,   268,   
    138,   44,    74,    39,    236,   327,   76,    85,    333,   53,    
    38,    199,   231,   44,    74,    263,   71,    711,   231,   39,    
    135,   44,    39,    106,   140,   74,    74,    44,    39,    42,    
    71,    103,   76,    333,   71,    87,    207,   58,    55,    76,    
    42,    199,   71,    711,   231,   71,    71,    71,    44,    106,   
    76,    76,    108,   44,    135,   39,    333,   76,    103,   44,    
    76,    42,    295,   103,   711,   231,   71,    167,   44,    39,    
    106,   172,   76,    42,    74,    44,    39,    71,    76,    333,   
    53,    55,    44,    74,    263,   71,    711,   231,   71,    167,   
    44,    39,    42,    44,    42,    140,   74,    74,    44,    44,    
    42,    71,    103,   76,    333,   58,    39,    207,   44,    39,    
    199,   103,   135,   71,    39,    71,    71,    103,   391,   74,    
    44,    74,    106,   106,   44,    39,    42,    333,   111,   218,   
    55,    58,    106,   263,   103,   743,   327,   167,   39,    108,   
    138,   108,   140,   76,    71,    71,    76,    333,   239,   58,    
    74,    263,   103,   743,   327,   167,   44,    39,    42,    44,    
    170,   44,    74,    74,    76,    74,    39,    71,    76,    333,   
    71,    74,    263,   103,   1319,  39,    106,   140,   106,   106,   
    44,    39,    42,    71,    76,    333,   207,   58,    199,   74,    
    583,   775,   295,   39,    231,   44,    106,   108,   44,    266,   
    74,    53,    1543,  44,    71,    236,   55,    199,   38,    268,   
    53,    333,   85,    71,    39,    71,    39,    39,    135,   231,   
    103,   39,    39,    71,    135,   44,    71,    204,   76,    39,    
    167,   38,    204,   333,   135,   39,    122,   501,   58,    53,    
    122,   76,    218,   333,   335,   58,    44,    58,    44,    58,    
    44,    54,    50,    54,    50,    74,    263,   1159,  460,   42,    
    172,   53,    76,    167,   364,   1164,  282,   44,    218,   90,    
    181,   154,   85,    1383,  74,    140,   42,    204,   42,    76,    
    74,    76,    39,    333,   213,   199,   74,    76,    135,   108,   
    39,    106,   71,    234,   103,   140,   423,   44,    74,    76,    
    202,   44,    39,    42,    333,   106,   44,    90,    1225,  41,    
    41,    1383,  53,    38,    10631, 135,   231,   39,    135,   1319,  
    135,   1063,  135,   231,   39,    135,   487,   1831,  135,   2151,  
    108,   309,   655,   519,   346,   2727,  49,    19847, 85,    551,   
    61,    839,   54,    50,    2407,  117,   110,   423,   135,   108,   
    583,   108,   85,    583,   76,    423,   103,   76,    1671,  76,    
    42,    236,   266,   44,    74,    364,   117,   38,    117,   55,    
    39,    44,    333,   335,   213,   49,    149,   108,   61,    333,   
    1127,  38,    1671,  1319,  44,    39,    2247,  935,   108,   138,   
    76,    106,   74,    44,    202,   108,   58,    85,    333,   967,   
    167,   1415,  554,   231,   74,    333,   47,    1114,  743,   76,    
    106,   85,    1703,  42,    44,    42,    236,   44,    42,    44,    
    74,    268,   202,   332,   44,    333,   333,   245,   38,    213,   
    140,   42,    1511,  44,    42,    172,   42,    44,    170,   44,    
    74,    231,   333,   245,   346,   300,   314,   76,    42,    967,   
    42,    140,   74,    76,    42,    44,    74,    71,    333,   1415,  
    44,    42,    76,    106,   44,    42,    108,   74,    149,   1159,  
    266,   268,   74,    76,    181,   333,   103,   333,   967,   198,   
    85,    277,   108,   53,    428,   42,    236,   135,   44,    135,   
    74,    44,    71,    1413,  2022,  421,   38,    1093,  1190,  1260,  
    140,   4830,  261,   3166,  261,   265,   197,   201,   261,   265,   
    261,   265,   197,   201,   261,   41,    41,    41,    94,    229,   
    265,   453,   261,   264,   261,   264,   261,   264,   165,   69,    
    137,   40,    56,    37,    120,   101,   69,    137,   40,    120,   
    133,   69,    137,   120,   261,   169,   120,   101,   69,    137,   
    40,    88,    381,   162,   209,   85,    52,    51,    54,    84,    
    51,    54,    52,    277,   59,    60,    162,   61,    309,   52,    
    51,    149,   80,    117,   57,    54,    50,    373,   57,    53,    
    48,    341,   61,    162,   194,   47,    38,    207,   121,   54,    
    50,    38,    335,   121,   54,    50,    422,   855,   428,   139,   
    44,    107,   396,   90,    41,    154,   41,    90,    37,    105,   
    69,    105,   37,    58,    41,    90,    57,    169,   218,   41,    
    58,    41,    58,    41,    58,    137,   58,    37,    137,   37,    
    135,   37,    90,    69,    73,    185,   94,    101,   58,    57,    
    90,    37,    58,    527,   1134,  94,    142,   47,    185,   186,   
    89,    154,   57,    90,    57,    90,    57,    250,   57,    1018,  
    89,    90,    57,    58,    57,    1018,  8601,  282,   153,   666,   
    89,    250,   54,    50,    2618,  57,    986,   825,   1306,  217,   
    602,   1274,  378,   1935,  2522,  719,   5882,  57,    314,   57,    
    1754,  281,   3578,  57,    4634,  3322,  54,    50,    54,    50,    
    54,    50,    54,    50,    54,    50,    54,    50,    54,    50,    
    975,   1434,  185,   54,    50,    1017,  54,    50,    54,    50,    
    54,    50,    54,    50,    54,    50,    537,   8218,  4217,  54,    
    50,    54,    50,    54,    50,    54,    50,    54,    50,    54,    
    50,    54,    50,    54,    50,    54,    50,    54,    50,    54,    
    50,    2041,  54,    50,    54,    50,    1049,  54,    50,    8281,  
    1562,  697,   90,    217,   346,   1513,  1509,  126,   73,    69,    
    254,   105,   37,    94,    37,    94,    165,   70,    105,   37,    
    3166,  37,    218,   158,   108,   94,    149,   47,    85,    1221,  
    37,    37,    1799,  38,    53,    44,    743,   231,   231,   231,   
    231,   231,   231,   231,   231,   1036,  85,    52,    51,    52,    
    51,    117,   52,    51,    53,    52,    51,    309,   49,    85,    
    49,    53,    52,    51,    85,    52,    51,    54,    50,    54,    
    50,    54,    50,    54,    50,    181,   38,    341,   81,    858,   
    2874,  6874,  410,   61,    117,   58,    38,    39,    46,    54,    
    50,    54,    50,    54,    50,    54,    50,    54,    50,    90,    
    54,    50,    54,    50,    54,    50,    54,    50,    49,    54,    
    82,    58,    302,   140,   74,    49,    166,   90,    110,   38,    
    39,    53,    90,    2759,  76,    88,    70,    39,    49,    2887,  
    53,    102,   39,    1319,  3015,  90,    143,   346,   871,   1178,  
    519,   1018,  335,   986,   271,   58,    495,   1050,  335,   1274,  
    495,   2042,  8218,  39,    39,    2074,  39,    39,    679,   38,    
    36583, 1786,  1287,  198,   85,    8583,  38,    117,   519,   333,   
    71,    1502,  39,    44,    107,   53,    332,   53,    38,    798,   
    44,    2247,  334,   76,    213,   760,   294,   88,    478,   69,    
    2014,  38,    261,   190,   350,   38,    88,    158,   158,   382,   
    70,    37,    231,   44,    103,   44,    135,   44,    743,   74,    
    76,    42,    154,   207,   90,    55,    58,    1671,  149,   74,    
    1607,  522,   44,    85,    333,   588,   199,   117,   39,    333,   
    903,   268,   85,    743,   364,   74,    53,    935,   108,   42,    
    1511,  44,    74,    140,   74,    44,    138,   437,   38,    333,   
    85,    1319,  204,   74,    76,    74,    76,    103,   44,    263,   
    44,    42,    333,   149,   519,   38,    199,   122,   39,    42,    
    1543,  44,    39,    108,   71,    76,    167,   76,    39,    44,    
    39,    71,    38,    85,    359,   42,    76,    74,    85,    39,    
    70,    42,    44,    199,   199,   199,   231,   231,   1127,  74,    
    44,    74,    44,    74,    53,    42,    44,    333,   39,    39,    
    743,   1575,  36,    68,    68,    36,    63,    63,    11719, 3399,  
    229,   165,   39,    44,    327,   57,    423,   167,   39,    71,    
    71,    3463,  536,   11623, 54,    50,    2055,  1735,  391,   55,    
    58,    524,   245,   54,    50,    53,    236,   53,    81,    80,    
    54,    50,    54,    50,    54,    50,    54,    50,    54,    50,    
    54,    50,    54,    50,    54,    50,    85,    54,    50,    149,   
    112,   117,   149,   49,    54,    50,    54,    50,    54,    50,    
    117,   57,    49,    121,   53,    55,    85,    167,   4327,  34,    
    117,   55,    117,   54,    50,    53,    57,    53,    49,    85,    
    333,   85,    121,   85,    841,   54,    53,    50,    56,    48,    
    56,    837,   54,    57,    50,    57,    54,    50,    53,    54,    
    50,    85,    327,   38,    1447,  70,    999,   199,   199,   199,   
    103,   87,    57,    56,    58,    87,    58,    153,   90,    98,    
    90,    391,   839,   615,   71,    487,   455,   3943,  117,   1455,  
    314,   1710,  143,   570,   47,    410,   1466,  44,    935,   1575,  
    999,   143,   551,   46,    263,   46,    967,   53,    1159,  263,   
    53,    174,   1289,  1285,  2503,  333,   199,   39,    1415,  71,    
    39,    743,   53,    271,   711,   207,   53,    839,   53,    1799,  
    71,    39,    108,   76,    140,   135,   103,   871,   108,   44,    
    271,   309,   935,   79,    53,    1735,  245,   711,   271,   615,   
    271,   2343,  1007,  42,    44,    42,    1703,  492,   245,   655,   
    333,   76,    42,    1447,  106,   140,   74,    76,    85,    34,    
    149,   807,   333,   108,   1159,  172,   42,    268,   333,   149,   
    76,    42,    1543,  106,   300,   74,    135,   149,   333,   1383,  
    44,    42,    44,    74,    204,   42,    44,    333,   28135, 3182,  
    149,   34279, 18215, 2215,  39,    1482,  140,   422,   71,    7898,  
    1274,  1946,  74,    108,   122,   202,   258,   268,   90,    236,   
    986,   140,   1562,  2138,  108,   58,    2810,  591,   841,   837,   
    841,   229,   581,   841,   837,   41,    73,    41,    73,    137,   
    265,   133,   37,    229,   357,   841,   837,   73,    137,   265,   
    233,   837,   73,    137,   169,   41,    233,   837,   841,   837,   
    841,   837,   841,   837,   841,   837,   841,   837,   841,   901,   
    809,   57,    805,   57,    197,   809,   57,    805,   57,    197,   
    809,   57,    805,   57,    197,   809,   57,    805,   57,    197,   
    809,   57,    805,   57,    197,   94,    1613,  135,   871,   71,    
    39,    39,    327,   135,   39,    39,    39,    39,    39,    39,    
    103,   71,    39,    39,    39,    39,    39,    39,    71,    39,    
    135,   231,   135,   135,   39,    327,   551,   103,   167,   551,   
    89,    1434,  3226,  506,   474,   506,   506,   367,   1018,  1946,  
    1402,  954,   1402,  314,   90,    1082,  218,   2266,  666,   1210,  
    186,   570,   2042,  58,    5850,  154,   2010,  154,   794,   2266,  
    378,   2266,  3738,  39,    39,    39,    39,    39,    39,    17351, 
    34,    3074,  7692,  63,    63,    
  };

static int sqlite3Fts5UnicodeCategory(u32 iCode) { 
  int iRes = -1;
  int iHi;
  int iLo;
  int ret;
  u16 iKey;

  if( iCode>=(1<<20) ){
    return 0;
  }
  iLo = aFts5UnicodeBlock[(iCode>>16)];
  iHi = aFts5UnicodeBlock[1+(iCode>>16)];
  iKey = (iCode & 0xFFFF);
  while( iHi>iLo ){
    int iTest = (iHi + iLo) / 2;
    assert( iTest>=iLo && iTest<iHi );
    if( iKey>=aFts5UnicodeMap[iTest] ){
      iRes = iTest;
      iLo = iTest+1;
    }else{
      iHi = iTest;
    }
  }

  if( iRes<0 ) return 0;
  if( iKey>=(aFts5UnicodeMap[iRes]+(aFts5UnicodeData[iRes]>>5)) ) return 0;
  ret = aFts5UnicodeData[iRes] & 0x1F;
  if( ret!=30 ) return ret;
  return ((iKey - aFts5UnicodeMap[iRes]) & 0x01) ? 5 : 9;
}

static void sqlite3Fts5UnicodeAscii(u8 *aArray, u8 *aAscii){
  int i = 0;
  int iTbl = 0;
  while( i<128 ){
    int bToken = aArray[ aFts5UnicodeData[iTbl] & 0x1F ];
    int n = (aFts5UnicodeData[iTbl] >> 5) + i;
    for(; i<128 && i<n; i++){
      aAscii[i] = (u8)bToken;
    }
    iTbl++;
  }
  aAscii[0] = 0;                  /* 0x00 is never a token character */
}


#line 1 "fts5_varint.c"
/*
** 2015 May 30
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
******************************************************************************
**
** Routines for varint serialization and deserialization.
*/


/* #include "fts5Int.h" */

/*
** This is a copy of the sqlite3GetVarint32() routine from the SQLite core.
** Except, this version does handle the single byte case that the core
** version depends on being handled before its function is called.
*/
static int sqlite3Fts5GetVarint32(const unsigned char *p, u32 *v){
  u32 a,b;

  /* The 1-byte case. Overwhelmingly the most common. */
  a = *p;
  /* a: p0 (unmasked) */
  if (!(a&0x80))
  {
    /* Values between 0 and 127 */
    *v = a;
    return 1;
  }

  /* The 2-byte case */
  p++;
  b = *p;
  /* b: p1 (unmasked) */
  if (!(b&0x80))
  {
    /* Values between 128 and 16383 */
    a &= 0x7f;
    a = a<<7;
    *v = a | b;
    return 2;
  }

  /* The 3-byte case */
  p++;
  a = a<<14;
  a |= *p;
  /* a: p0<<14 | p2 (unmasked) */
  if (!(a&0x80))
  {
    /* Values between 16384 and 2097151 */
    a &= (0x7f<<14)|(0x7f);
    b &= 0x7f;
    b = b<<7;
    *v = a | b;
    return 3;
  }

  /* A 32-bit varint is used to store size information in btrees.
  ** Objects are rarely larger than 2MiB limit of a 3-byte varint.
  ** A 3-byte varint is sufficient, for example, to record the size
  ** of a 1048569-byte BLOB or string.
  **
  ** We only unroll the first 1-, 2-, and 3- byte cases.  The very
  ** rare larger cases can be handled by the slower 64-bit varint
  ** routine.
  */
  {
    u64 v64;
    u8 n;
    p -= 2;
    n = sqlite3Fts5GetVarint(p, &v64);
    *v = ((u32)v64) & 0x7FFFFFFF;
    assert( n>3 && n<=9 );
    return n;
  }
}


/*
** Bitmasks used by sqlite3GetVarint().  These precomputed constants
** are defined here rather than simply putting the constant expressions
** inline in order to work around bugs in the RVT compiler.
**
** SLOT_2_0     A mask for  (0x7f<<14) | 0x7f
**
** SLOT_4_2_0   A mask for  (0x7f<<28) | SLOT_2_0
*/
#define SLOT_2_0     0x001fc07f
#define SLOT_4_2_0   0xf01fc07f

/*
** Read a 64-bit variable-length integer from memory starting at p[0].
** Return the number of bytes read.  The value is stored in *v.
*/
static u8 sqlite3Fts5GetVarint(const unsigned char *p, u64 *v){
  u32 a,b,s;

  a = *p;
  /* a: p0 (unmasked) */
  if (!(a&0x80))
  {
    *v = a;
    return 1;
  }

  p++;
  b = *p;
  /* b: p1 (unmasked) */
  if (!(b&0x80))
  {
    a &= 0x7f;
    a = a<<7;
    a |= b;
    *v = a;
    return 2;
  }

  /* Verify that constants are precomputed correctly */
  assert( SLOT_2_0 == ((0x7f<<14) | (0x7f)) );
  assert( SLOT_4_2_0 == ((0xfU<<28) | (0x7f<<14) | (0x7f)) );

  p++;
  a = a<<14;
  a |= *p;
  /* a: p0<<14 | p2 (unmasked) */
  if (!(a&0x80))
  {
    a &= SLOT_2_0;
    b &= 0x7f;
    b = b<<7;
    a |= b;
    *v = a;
    return 3;
  }

  /* CSE1 from below */
  a &= SLOT_2_0;
  p++;
  b = b<<14;
  b |= *p;
  /* b: p1<<14 | p3 (unmasked) */
  if (!(b&0x80))
  {
    b &= SLOT_2_0;
    /* moved CSE1 up */
    /* a &= (0x7f<<14)|(0x7f); */
    a = a<<7;
    a |= b;
    *v = a;
    return 4;
  }

  /* a: p0<<14 | p2 (masked) */
  /* b: p1<<14 | p3 (unmasked) */
  /* 1:save off p0<<21 | p1<<14 | p2<<7 | p3 (masked) */
  /* moved CSE1 up */
  /* a &= (0x7f<<14)|(0x7f); */
  b &= SLOT_2_0;
  s = a;
  /* s: p0<<14 | p2 (masked) */

  p++;
  a = a<<14;
  a |= *p;
  /* a: p0<<28 | p2<<14 | p4 (unmasked) */
  if (!(a&0x80))
  {
    /* we can skip these cause they were (effectively) done above in calc'ing s */
    /* a &= (0x7f<<28)|(0x7f<<14)|(0x7f); */
    /* b &= (0x7f<<14)|(0x7f); */
    b = b<<7;
    a |= b;
    s = s>>18;
    *v = ((u64)s)<<32 | a;
    return 5;
  }

  /* 2:save off p0<<21 | p1<<14 | p2<<7 | p3 (masked) */
  s = s<<7;
  s |= b;
  /* s: p0<<21 | p1<<14 | p2<<7 | p3 (masked) */

  p++;
  b = b<<14;
  b |= *p;
  /* b: p1<<28 | p3<<14 | p5 (unmasked) */
  if (!(b&0x80))
  {
    /* we can skip this cause it was (effectively) done above in calc'ing s */
    /* b &= (0x7f<<28)|(0x7f<<14)|(0x7f); */
    a &= SLOT_2_0;
    a = a<<7;
    a |= b;
    s = s>>18;
    *v = ((u64)s)<<32 | a;
    return 6;
  }

  p++;
  a = a<<14;
  a |= *p;
  /* a: p2<<28 | p4<<14 | p6 (unmasked) */
  if (!(a&0x80))
  {
    a &= SLOT_4_2_0;
    b &= SLOT_2_0;
    b = b<<7;
    a |= b;
    s = s>>11;
    *v = ((u64)s)<<32 | a;
    return 7;
  }

  /* CSE2 from below */
  a &= SLOT_2_0;
  p++;
  b = b<<14;
  b |= *p;
  /* b: p3<<28 | p5<<14 | p7 (unmasked) */
  if (!(b&0x80))
  {
    b &= SLOT_4_2_0;
    /* moved CSE2 up */
    /* a &= (0x7f<<14)|(0x7f); */
    a = a<<7;
    a |= b;
    s = s>>4;
    *v = ((u64)s)<<32 | a;
    return 8;
  }

  p++;
  a = a<<15;
  a |= *p;
  /* a: p4<<29 | p6<<15 | p8 (unmasked) */

  /* moved CSE2 up */
  /* a &= (0x7f<<29)|(0x7f<<15)|(0xff); */
  b &= SLOT_2_0;
  b = b<<8;
  a |= b;

  s = s<<4;
  b = p[-4];
  b &= 0x7f;
  b = b>>3;
  s |= b;

  *v = ((u64)s)<<32 | a;

  return 9;
}

/*
** The variable-length integer encoding is as follows:
**
** KEY:
**         A = 0xxxxxxx    7 bits of data and one flag bit
**         B = 1xxxxxxx    7 bits of data and one flag bit
**         C = xxxxxxxx    8 bits of data
**
**  7 bits - A
** 14 bits - BA
** 21 bits - BBA
** 28 bits - BBBA
** 35 bits - BBBBA
** 42 bits - BBBBBA
** 49 bits - BBBBBBA
** 56 bits - BBBBBBBA
** 64 bits - BBBBBBBBC
*/

#ifdef SQLITE_NOINLINE
# define FTS5_NOINLINE SQLITE_NOINLINE
#else
# define FTS5_NOINLINE
#endif

/*
** Write a 64-bit variable-length integer to memory starting at p[0].
** The length of data write will be between 1 and 9 bytes.  The number
** of bytes written is returned.
**
** A variable-length integer consists of the lower 7 bits of each byte
** for all bytes that have the 8th bit set and one byte with the 8th
** bit clear.  Except, if we get to the 9th byte, it stores the full
** 8 bits and is the last byte.
*/
static int FTS5_NOINLINE fts5PutVarint64(unsigned char *p, u64 v){
  int i, j, n;
  u8 buf[10];
  if( v & (((u64)0xff000000)<<32) ){
    p[8] = (u8)v;
    v >>= 8;
    for(i=7; i>=0; i--){
      p[i] = (u8)((v & 0x7f) | 0x80);
      v >>= 7;
    }
    return 9;
  }    
  n = 0;
  do{
    buf[n++] = (u8)((v & 0x7f) | 0x80);
    v >>= 7;
  }while( v!=0 );
  buf[0] &= 0x7f;
  assert( n<=9 );
  for(i=0, j=n-1; j>=0; j--, i++){
    p[i] = buf[j];
  }
  return n;
}

static int sqlite3Fts5PutVarint(unsigned char *p, u64 v){
  if( v<=0x7f ){
    p[0] = v&0x7f;
    return 1;
  }
  if( v<=0x3fff ){
    p[0] = ((v>>7)&0x7f)|0x80;
    p[1] = v&0x7f;
    return 2;
  }
  return fts5PutVarint64(p,v);
}


static int sqlite3Fts5GetVarintLen(u32 iVal){
#if 0
  if( iVal<(1 << 7 ) ) return 1;
#endif
  assert( iVal>=(1 << 7) );
  if( iVal<(1 << 14) ) return 2;
  if( iVal<(1 << 21) ) return 3;
  if( iVal<(1 << 28) ) return 4;
  return 5;
}

#line 1 "fts5_vocab.c"
/*
** 2015 May 08
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
******************************************************************************
**
** This is an SQLite virtual table module implementing direct access to an
** existing FTS5 index. The module may create several different types of 
** tables:
**
** col:
**     CREATE TABLE vocab(term, col, doc, cnt, PRIMARY KEY(term, col));
**
**   One row for each term/column combination. The value of $doc is set to
**   the number of fts5 rows that contain at least one instance of term
**   $term within column $col. Field $cnt is set to the total number of 
**   instances of term $term in column $col (in any row of the fts5 table). 
**
** row:
**     CREATE TABLE vocab(term, doc, cnt, PRIMARY KEY(term));
**
**   One row for each term in the database. The value of $doc is set to
**   the number of fts5 rows that contain at least one instance of term
**   $term. Field $cnt is set to the total number of instances of term 
**   $term in the database.
**
** instance:
**     CREATE TABLE vocab(term, doc, col, offset, PRIMARY KEY(<all-fields>));
**
**   One row for each term instance in the database. 
*/


/* #include "fts5Int.h" */


typedef struct Fts5VocabTable Fts5VocabTable;
typedef struct Fts5VocabCursor Fts5VocabCursor;

struct Fts5VocabTable {
  sqlite3_vtab base;
  char *zFts5Tbl;                 /* Name of fts5 table */
  char *zFts5Db;                  /* Db containing fts5 table */
  sqlite3 *db;                    /* Database handle */
  Fts5Global *pGlobal;            /* FTS5 global object for this database */
  int eType;                      /* FTS5_VOCAB_COL, ROW or INSTANCE */
  unsigned bBusy;                 /* True if busy */
};

struct Fts5VocabCursor {
  sqlite3_vtab_cursor base;
  sqlite3_stmt *pStmt;            /* Statement holding lock on pIndex */
  Fts5Table *pFts5;               /* Associated FTS5 table */

  int bEof;                       /* True if this cursor is at EOF */
  Fts5IndexIter *pIter;           /* Term/rowid iterator object */
  void *pStruct;                  /* From sqlite3Fts5StructureRef() */

  int nLeTerm;                    /* Size of zLeTerm in bytes */
  char *zLeTerm;                  /* (term <= $zLeTerm) paramater, or NULL */

  /* These are used by 'col' tables only */
  int iCol;
  i64 *aCnt;
  i64 *aDoc;

  /* Output values used by all tables. */
  i64 rowid;                      /* This table's current rowid value */
  Fts5Buffer term;                /* Current value of 'term' column */

  /* Output values Used by 'instance' tables only */
  i64 iInstPos;
  int iInstOff;
};

#define FTS5_VOCAB_COL      0
#define FTS5_VOCAB_ROW      1
#define FTS5_VOCAB_INSTANCE 2

#define FTS5_VOCAB_COL_SCHEMA  "term, col, doc, cnt"
#define FTS5_VOCAB_ROW_SCHEMA  "term, doc, cnt"
#define FTS5_VOCAB_INST_SCHEMA "term, doc, col, offset"

/*
** Bits for the mask used as the idxNum value by xBestIndex/xFilter.
*/
#define FTS5_VOCAB_TERM_EQ 0x01
#define FTS5_VOCAB_TERM_GE 0x02
#define FTS5_VOCAB_TERM_LE 0x04


/*
** Translate a string containing an fts5vocab table type to an 
** FTS5_VOCAB_XXX constant. If successful, set *peType to the output
** value and return SQLITE_OK. Otherwise, set *pzErr to an error message
** and return SQLITE_ERROR.
*/
static int fts5VocabTableType(const char *zType, char **pzErr, int *peType){
  int rc = SQLITE_OK;
  char *zCopy = sqlite3Fts5Strndup(&rc, zType, -1);
  if( rc==SQLITE_OK ){
    sqlite3Fts5Dequote(zCopy);
    if( sqlite3_stricmp(zCopy, "col")==0 ){
      *peType = FTS5_VOCAB_COL;
    }else

    if( sqlite3_stricmp(zCopy, "row")==0 ){
      *peType = FTS5_VOCAB_ROW;
    }else
    if( sqlite3_stricmp(zCopy, "instance")==0 ){
      *peType = FTS5_VOCAB_INSTANCE;
    }else
    {
      *pzErr = sqlite3_mprintf("fts5vocab: unknown table type: %Q", zCopy);
      rc = SQLITE_ERROR;
    }
    sqlite3_free(zCopy);
  }

  return rc;
}


/*
** The xDisconnect() virtual table method.
*/
static int fts5VocabDisconnectMethod(sqlite3_vtab *pVtab){
  Fts5VocabTable *pTab = (Fts5VocabTable*)pVtab;
  sqlite3_free(pTab);
  return SQLITE_OK;
}

/*
** The xDestroy() virtual table method.
*/
static int fts5VocabDestroyMethod(sqlite3_vtab *pVtab){
  Fts5VocabTable *pTab = (Fts5VocabTable*)pVtab;
  sqlite3_free(pTab);
  return SQLITE_OK;
}

/*
** This function is the implementation of both the xConnect and xCreate
** methods of the FTS3 virtual table.
**
** The argv[] array contains the following:
**
**   argv[0]   -> module name  ("fts5vocab")
**   argv[1]   -> database name
**   argv[2]   -> table name
**
** then:
**
**   argv[3]   -> name of fts5 table
**   argv[4]   -> type of fts5vocab table
**
** or, for tables in the TEMP schema only.
**
**   argv[3]   -> name of fts5 tables database
**   argv[4]   -> name of fts5 table
**   argv[5]   -> type of fts5vocab table
*/
static int fts5VocabInitVtab(
  sqlite3 *db,                    /* The SQLite database connection */
  void *pAux,                     /* Pointer to Fts5Global object */
  int argc,                       /* Number of elements in argv array */
  const char * const *argv,       /* xCreate/xConnect argument array */
  sqlite3_vtab **ppVTab,          /* Write the resulting vtab structure here */
  char **pzErr                    /* Write any error message here */
){
  const char *azSchema[] = { 
    "CREATE TABlE vocab(" FTS5_VOCAB_COL_SCHEMA  ")", 
    "CREATE TABlE vocab(" FTS5_VOCAB_ROW_SCHEMA  ")",
    "CREATE TABlE vocab(" FTS5_VOCAB_INST_SCHEMA ")"
  };

  Fts5VocabTable *pRet = 0;
  int rc = SQLITE_OK;             /* Return code */
  int bDb;

  bDb = (argc==6 && strlen(argv[1])==4 && memcmp("temp", argv[1], 4)==0);

  if( argc!=5 && bDb==0 ){
    *pzErr = sqlite3_mprintf("wrong number of vtable arguments");
    rc = SQLITE_ERROR;
  }else{
    int nByte;                      /* Bytes of space to allocate */
    const char *zDb = bDb ? argv[3] : argv[1];
    const char *zTab = bDb ? argv[4] : argv[3];
    const char *zType = bDb ? argv[5] : argv[4];
    int nDb = (int)strlen(zDb)+1; 
    int nTab = (int)strlen(zTab)+1;
    int eType = 0;
    
    rc = fts5VocabTableType(zType, pzErr, &eType);
    if( rc==SQLITE_OK ){
      assert( eType>=0 && eType<ArraySize(azSchema) );
      rc = sqlite3_declare_vtab(db, azSchema[eType]);
    }

    nByte = sizeof(Fts5VocabTable) + nDb + nTab;
    pRet = sqlite3Fts5MallocZero(&rc, nByte);
    if( pRet ){
      pRet->pGlobal = (Fts5Global*)pAux;
      pRet->eType = eType;
      pRet->db = db;
      pRet->zFts5Tbl = (char*)&pRet[1];
      pRet->zFts5Db = &pRet->zFts5Tbl[nTab];
      memcpy(pRet->zFts5Tbl, zTab, nTab);
      memcpy(pRet->zFts5Db, zDb, nDb);
      sqlite3Fts5Dequote(pRet->zFts5Tbl);
      sqlite3Fts5Dequote(pRet->zFts5Db);
    }
  }

  *ppVTab = (sqlite3_vtab*)pRet;
  return rc;
}


/*
** The xConnect() and xCreate() methods for the virtual table. All the
** work is done in function fts5VocabInitVtab().
*/
static int fts5VocabConnectMethod(
  sqlite3 *db,                    /* Database connection */
  void *pAux,                     /* Pointer to tokenizer hash table */
  int argc,                       /* Number of elements in argv array */
  const char * const *argv,       /* xCreate/xConnect argument array */
  sqlite3_vtab **ppVtab,          /* OUT: New sqlite3_vtab object */
  char **pzErr                    /* OUT: sqlite3_malloc'd error message */
){
  return fts5VocabInitVtab(db, pAux, argc, argv, ppVtab, pzErr);
}
static int fts5VocabCreateMethod(
  sqlite3 *db,                    /* Database connection */
  void *pAux,                     /* Pointer to tokenizer hash table */
  int argc,                       /* Number of elements in argv array */
  const char * const *argv,       /* xCreate/xConnect argument array */
  sqlite3_vtab **ppVtab,          /* OUT: New sqlite3_vtab object */
  char **pzErr                    /* OUT: sqlite3_malloc'd error message */
){
  return fts5VocabInitVtab(db, pAux, argc, argv, ppVtab, pzErr);
}

/* 
** Implementation of the xBestIndex method.
**
** Only constraints of the form:
**
**     term <= ?
**     term == ?
**     term >= ?
**
** are interpreted. Less-than and less-than-or-equal are treated 
** identically, as are greater-than and greater-than-or-equal.
*/
static int fts5VocabBestIndexMethod(
  sqlite3_vtab *pUnused,
  sqlite3_index_info *pInfo
){
  int i;
  int iTermEq = -1;
  int iTermGe = -1;
  int iTermLe = -1;
  int idxNum = 0;
  int nArg = 0;

  UNUSED_PARAM(pUnused);

  for(i=0; i<pInfo->nConstraint; i++){
    struct sqlite3_index_constraint *p = &pInfo->aConstraint[i];
    if( p->usable==0 ) continue;
    if( p->iColumn==0 ){          /* term column */
      if( p->op==SQLITE_INDEX_CONSTRAINT_EQ ) iTermEq = i;
      if( p->op==SQLITE_INDEX_CONSTRAINT_LE ) iTermLe = i;
      if( p->op==SQLITE_INDEX_CONSTRAINT_LT ) iTermLe = i;
      if( p->op==SQLITE_INDEX_CONSTRAINT_GE ) iTermGe = i;
      if( p->op==SQLITE_INDEX_CONSTRAINT_GT ) iTermGe = i;
    }
  }

  if( iTermEq>=0 ){
    idxNum |= FTS5_VOCAB_TERM_EQ;
    pInfo->aConstraintUsage[iTermEq].argvIndex = ++nArg;
    pInfo->estimatedCost = 100;
  }else{
    pInfo->estimatedCost = 1000000;
    if( iTermGe>=0 ){
      idxNum |= FTS5_VOCAB_TERM_GE;
      pInfo->aConstraintUsage[iTermGe].argvIndex = ++nArg;
      pInfo->estimatedCost = pInfo->estimatedCost / 2;
    }
    if( iTermLe>=0 ){
      idxNum |= FTS5_VOCAB_TERM_LE;
      pInfo->aConstraintUsage[iTermLe].argvIndex = ++nArg;
      pInfo->estimatedCost = pInfo->estimatedCost / 2;
    }
  }

  /* This virtual table always delivers results in ascending order of
  ** the "term" column (column 0). So if the user has requested this
  ** specifically - "ORDER BY term" or "ORDER BY term ASC" - set the
  ** sqlite3_index_info.orderByConsumed flag to tell the core the results
  ** are already in sorted order.  */
  if( pInfo->nOrderBy==1 
   && pInfo->aOrderBy[0].iColumn==0 
   && pInfo->aOrderBy[0].desc==0
  ){
    pInfo->orderByConsumed = 1;
  }

  pInfo->idxNum = idxNum;
  return SQLITE_OK;
}

/*
** Implementation of xOpen method.
*/
static int fts5VocabOpenMethod(
  sqlite3_vtab *pVTab, 
  sqlite3_vtab_cursor **ppCsr
){
  Fts5VocabTable *pTab = (Fts5VocabTable*)pVTab;
  Fts5Table *pFts5 = 0;
  Fts5VocabCursor *pCsr = 0;
  int rc = SQLITE_OK;
  sqlite3_stmt *pStmt = 0;
  char *zSql = 0;

  if( pTab->bBusy ){
    pVTab->zErrMsg = sqlite3_mprintf(
       "recursive definition for %s.%s", pTab->zFts5Db, pTab->zFts5Tbl
    );
    return SQLITE_ERROR;
  }
  zSql = sqlite3Fts5Mprintf(&rc,
      "SELECT t.%Q FROM %Q.%Q AS t WHERE t.%Q MATCH '*id'",
      pTab->zFts5Tbl, pTab->zFts5Db, pTab->zFts5Tbl, pTab->zFts5Tbl
  );
  if( zSql ){
    rc = sqlite3_prepare_v2(pTab->db, zSql, -1, &pStmt, 0);
  }
  sqlite3_free(zSql);
  assert( rc==SQLITE_OK || pStmt==0 );
  if( rc==SQLITE_ERROR ) rc = SQLITE_OK;

  pTab->bBusy = 1;
  if( pStmt && sqlite3_step(pStmt)==SQLITE_ROW ){
    i64 iId = sqlite3_column_int64(pStmt, 0);
    pFts5 = sqlite3Fts5TableFromCsrid(pTab->pGlobal, iId);
  }
  pTab->bBusy = 0;

  if( rc==SQLITE_OK ){
    if( pFts5==0 ){
      rc = sqlite3_finalize(pStmt);
      pStmt = 0;
      if( rc==SQLITE_OK ){
        pVTab->zErrMsg = sqlite3_mprintf(
            "no such fts5 table: %s.%s", pTab->zFts5Db, pTab->zFts5Tbl
            );
        rc = SQLITE_ERROR;
      }
    }else{
      rc = sqlite3Fts5FlushToDisk(pFts5);
    }
  }

  if( rc==SQLITE_OK ){
    i64 nByte = pFts5->pConfig->nCol * sizeof(i64)*2 + sizeof(Fts5VocabCursor);
    pCsr = (Fts5VocabCursor*)sqlite3Fts5MallocZero(&rc, nByte);
  }

  if( pCsr ){
    pCsr->pFts5 = pFts5;
    pCsr->pStmt = pStmt;
    pCsr->aCnt = (i64*)&pCsr[1];
    pCsr->aDoc = &pCsr->aCnt[pFts5->pConfig->nCol];
  }else{
    sqlite3_finalize(pStmt);
  }

  *ppCsr = (sqlite3_vtab_cursor*)pCsr;
  return rc;
}

static void fts5VocabResetCursor(Fts5VocabCursor *pCsr){
  pCsr->rowid = 0;
  sqlite3Fts5IterClose(pCsr->pIter);
  sqlite3Fts5StructureRelease(pCsr->pStruct);
  pCsr->pStruct = 0;
  pCsr->pIter = 0;
  sqlite3_free(pCsr->zLeTerm);
  pCsr->nLeTerm = -1;
  pCsr->zLeTerm = 0;
  pCsr->bEof = 0;
}

/*
** Close the cursor.  For additional information see the documentation
** on the xClose method of the virtual table interface.
*/
static int fts5VocabCloseMethod(sqlite3_vtab_cursor *pCursor){
  Fts5VocabCursor *pCsr = (Fts5VocabCursor*)pCursor;
  fts5VocabResetCursor(pCsr);
  sqlite3Fts5BufferFree(&pCsr->term);
  sqlite3_finalize(pCsr->pStmt);
  sqlite3_free(pCsr);
  return SQLITE_OK;
}

static int fts5VocabInstanceNewTerm(Fts5VocabCursor *pCsr){
  int rc = SQLITE_OK;
  
  if( sqlite3Fts5IterEof(pCsr->pIter) ){
    pCsr->bEof = 1;
  }else{
    const char *zTerm;
    int nTerm;
    zTerm = sqlite3Fts5IterTerm(pCsr->pIter, &nTerm);
    if( pCsr->nLeTerm>=0 ){
      int nCmp = MIN(nTerm, pCsr->nLeTerm);
      int bCmp = memcmp(pCsr->zLeTerm, zTerm, nCmp);
      if( bCmp<0 || (bCmp==0 && pCsr->nLeTerm<nTerm) ){
        pCsr->bEof = 1;
      }
    }

    sqlite3Fts5BufferSet(&rc, &pCsr->term, nTerm, (const u8*)zTerm);
  }
  return rc;
}

static int fts5VocabInstanceNext(Fts5VocabCursor *pCsr){
  int eDetail = pCsr->pFts5->pConfig->eDetail;
  int rc = SQLITE_OK;
  Fts5IndexIter *pIter = pCsr->pIter;
  i64 *pp = &pCsr->iInstPos;
  int *po = &pCsr->iInstOff;
  
  assert( sqlite3Fts5IterEof(pIter)==0 );
  assert( pCsr->bEof==0 );
  while( eDetail==FTS5_DETAIL_NONE
      || sqlite3Fts5PoslistNext64(pIter->pData, pIter->nData, po, pp) 
  ){
    pCsr->iInstPos = 0;
    pCsr->iInstOff = 0;

    rc = sqlite3Fts5IterNextScan(pCsr->pIter);
    if( rc==SQLITE_OK ){
      rc = fts5VocabInstanceNewTerm(pCsr);
      if( pCsr->bEof || eDetail==FTS5_DETAIL_NONE ) break;
    }
    if( rc ){
      pCsr->bEof = 1;
      break;
    }
  }

  return rc;
}

/*
** Advance the cursor to the next row in the table.
*/
static int fts5VocabNextMethod(sqlite3_vtab_cursor *pCursor){
  Fts5VocabCursor *pCsr = (Fts5VocabCursor*)pCursor;
  Fts5VocabTable *pTab = (Fts5VocabTable*)pCursor->pVtab;
  int nCol = pCsr->pFts5->pConfig->nCol;
  int rc;

  rc = sqlite3Fts5StructureTest(pCsr->pFts5->pIndex, pCsr->pStruct);
  if( rc!=SQLITE_OK ) return rc;
  pCsr->rowid++;

  if( pTab->eType==FTS5_VOCAB_INSTANCE ){
    return fts5VocabInstanceNext(pCsr);
  }

  if( pTab->eType==FTS5_VOCAB_COL ){
    for(pCsr->iCol++; pCsr->iCol<nCol; pCsr->iCol++){
      if( pCsr->aDoc[pCsr->iCol] ) break;
    }
  }

  if( pTab->eType!=FTS5_VOCAB_COL || pCsr->iCol>=nCol ){
    if( sqlite3Fts5IterEof(pCsr->pIter) ){
      pCsr->bEof = 1;
    }else{
      const char *zTerm;
      int nTerm;

      zTerm = sqlite3Fts5IterTerm(pCsr->pIter, &nTerm);
      assert( nTerm>=0 );
      if( pCsr->nLeTerm>=0 ){
        int nCmp = MIN(nTerm, pCsr->nLeTerm);
        int bCmp = memcmp(pCsr->zLeTerm, zTerm, nCmp);
        if( bCmp<0 || (bCmp==0 && pCsr->nLeTerm<nTerm) ){
          pCsr->bEof = 1;
          return SQLITE_OK;
        }
      }

      sqlite3Fts5BufferSet(&rc, &pCsr->term, nTerm, (const u8*)zTerm);
      memset(pCsr->aCnt, 0, nCol * sizeof(i64));
      memset(pCsr->aDoc, 0, nCol * sizeof(i64));
      pCsr->iCol = 0;

      assert( pTab->eType==FTS5_VOCAB_COL || pTab->eType==FTS5_VOCAB_ROW );
      while( rc==SQLITE_OK ){
        int eDetail = pCsr->pFts5->pConfig->eDetail;
        const u8 *pPos; int nPos;   /* Position list */
        i64 iPos = 0;               /* 64-bit position read from poslist */
        int iOff = 0;               /* Current offset within position list */

        pPos = pCsr->pIter->pData;
        nPos = pCsr->pIter->nData;

        switch( pTab->eType ){
          case FTS5_VOCAB_ROW:
            if( eDetail==FTS5_DETAIL_FULL ){
              while( 0==sqlite3Fts5PoslistNext64(pPos, nPos, &iOff, &iPos) ){
                pCsr->aCnt[0]++;
              }
            }
            pCsr->aDoc[0]++;
            break;

          case FTS5_VOCAB_COL:
            if( eDetail==FTS5_DETAIL_FULL ){
              int iCol = -1;
              while( 0==sqlite3Fts5PoslistNext64(pPos, nPos, &iOff, &iPos) ){
                int ii = FTS5_POS2COLUMN(iPos);
                if( iCol!=ii ){
                  if( ii>=nCol ){
                    rc = FTS5_CORRUPT;
                    break;
                  }
                  pCsr->aDoc[ii]++;
                  iCol = ii;
                }
                pCsr->aCnt[ii]++;
              }
            }else if( eDetail==FTS5_DETAIL_COLUMNS ){
              while( 0==sqlite3Fts5PoslistNext64(pPos, nPos, &iOff,&iPos) ){
                assert_nc( iPos>=0 && iPos<nCol );
                if( iPos>=nCol ){
                  rc = FTS5_CORRUPT;
                  break;
                }
                pCsr->aDoc[iPos]++;
              }
            }else{
              assert( eDetail==FTS5_DETAIL_NONE );
              pCsr->aDoc[0]++;
            }
            break;

          default:
            assert( pTab->eType==FTS5_VOCAB_INSTANCE );
            break;
        }

        if( rc==SQLITE_OK ){
          rc = sqlite3Fts5IterNextScan(pCsr->pIter);
        }
        if( pTab->eType==FTS5_VOCAB_INSTANCE ) break;

        if( rc==SQLITE_OK ){
          zTerm = sqlite3Fts5IterTerm(pCsr->pIter, &nTerm);
          if( nTerm!=pCsr->term.n 
          || (nTerm>0 && memcmp(zTerm, pCsr->term.p, nTerm)) 
          ){
            break;
          }
          if( sqlite3Fts5IterEof(pCsr->pIter) ) break;
        }
      }
    }
  }

  if( rc==SQLITE_OK && pCsr->bEof==0 && pTab->eType==FTS5_VOCAB_COL ){
    for(/* noop */; pCsr->iCol<nCol && pCsr->aDoc[pCsr->iCol]==0; pCsr->iCol++);
    if( pCsr->iCol==nCol ){
      rc = FTS5_CORRUPT;
    }
  }
  return rc;
}

/*
** This is the xFilter implementation for the virtual table.
*/
static int fts5VocabFilterMethod(
  sqlite3_vtab_cursor *pCursor,   /* The cursor used for this query */
  int idxNum,                     /* Strategy index */
  const char *zUnused,            /* Unused */
  int nUnused,                    /* Number of elements in apVal */
  sqlite3_value **apVal           /* Arguments for the indexing scheme */
){
  Fts5VocabTable *pTab = (Fts5VocabTable*)pCursor->pVtab;
  Fts5VocabCursor *pCsr = (Fts5VocabCursor*)pCursor;
  int eType = pTab->eType;
  int rc = SQLITE_OK;

  int iVal = 0;
  int f = FTS5INDEX_QUERY_SCAN;
  const char *zTerm = 0;
  int nTerm = 0;

  sqlite3_value *pEq = 0;
  sqlite3_value *pGe = 0;
  sqlite3_value *pLe = 0;

  UNUSED_PARAM2(zUnused, nUnused);

  fts5VocabResetCursor(pCsr);
  if( idxNum & FTS5_VOCAB_TERM_EQ ) pEq = apVal[iVal++];
  if( idxNum & FTS5_VOCAB_TERM_GE ) pGe = apVal[iVal++];
  if( idxNum & FTS5_VOCAB_TERM_LE ) pLe = apVal[iVal++];

  if( pEq ){
    zTerm = (const char *)sqlite3_value_text(pEq);
    nTerm = sqlite3_value_bytes(pEq);
    f = 0;
  }else{
    if( pGe ){
      zTerm = (const char *)sqlite3_value_text(pGe);
      nTerm = sqlite3_value_bytes(pGe);
    }
    if( pLe ){
      const char *zCopy = (const char *)sqlite3_value_text(pLe);
      if( zCopy==0 ) zCopy = "";
      pCsr->nLeTerm = sqlite3_value_bytes(pLe);
      pCsr->zLeTerm = sqlite3_malloc(pCsr->nLeTerm+1);
      if( pCsr->zLeTerm==0 ){
        rc = SQLITE_NOMEM;
      }else{
        memcpy(pCsr->zLeTerm, zCopy, pCsr->nLeTerm+1);
      }
    }
  }

  if( rc==SQLITE_OK ){
    Fts5Index *pIndex = pCsr->pFts5->pIndex;
    rc = sqlite3Fts5IndexQuery(pIndex, zTerm, nTerm, f, 0, &pCsr->pIter);
    if( rc==SQLITE_OK ){
      pCsr->pStruct = sqlite3Fts5StructureRef(pIndex);
    }
  }
  if( rc==SQLITE_OK && eType==FTS5_VOCAB_INSTANCE ){
    rc = fts5VocabInstanceNewTerm(pCsr);
  }
  if( rc==SQLITE_OK && !pCsr->bEof 
   && (eType!=FTS5_VOCAB_INSTANCE 
    || pCsr->pFts5->pConfig->eDetail!=FTS5_DETAIL_NONE)
  ){
    rc = fts5VocabNextMethod(pCursor);
  }

  return rc;
}

/* 
** This is the xEof method of the virtual table. SQLite calls this 
** routine to find out if it has reached the end of a result set.
*/
static int fts5VocabEofMethod(sqlite3_vtab_cursor *pCursor){
  Fts5VocabCursor *pCsr = (Fts5VocabCursor*)pCursor;
  return pCsr->bEof;
}

static int fts5VocabColumnMethod(
  sqlite3_vtab_cursor *pCursor,   /* Cursor to retrieve value from */
  sqlite3_context *pCtx,          /* Context for sqlite3_result_xxx() calls */
  int iCol                        /* Index of column to read value from */
){
  Fts5VocabCursor *pCsr = (Fts5VocabCursor*)pCursor;
  int eDetail = pCsr->pFts5->pConfig->eDetail;
  int eType = ((Fts5VocabTable*)(pCursor->pVtab))->eType;
  i64 iVal = 0;

  if( iCol==0 ){
    sqlite3_result_text(
        pCtx, (const char*)pCsr->term.p, pCsr->term.n, SQLITE_TRANSIENT
    );
  }else if( eType==FTS5_VOCAB_COL ){
    assert( iCol==1 || iCol==2 || iCol==3 );
    if( iCol==1 ){
      if( eDetail!=FTS5_DETAIL_NONE ){
        const char *z = pCsr->pFts5->pConfig->azCol[pCsr->iCol];
        sqlite3_result_text(pCtx, z, -1, SQLITE_STATIC);
      }
    }else if( iCol==2 ){
      iVal = pCsr->aDoc[pCsr->iCol];
    }else{
      iVal = pCsr->aCnt[pCsr->iCol];
    }
  }else if( eType==FTS5_VOCAB_ROW ){
    assert( iCol==1 || iCol==2 );
    if( iCol==1 ){
      iVal = pCsr->aDoc[0];
    }else{
      iVal = pCsr->aCnt[0];
    }
  }else{
    assert( eType==FTS5_VOCAB_INSTANCE );
    switch( iCol ){
      case 1:
        sqlite3_result_int64(pCtx, pCsr->pIter->iRowid);
        break;
      case 2: {
        int ii = -1;
        if( eDetail==FTS5_DETAIL_FULL ){
          ii = FTS5_POS2COLUMN(pCsr->iInstPos);
        }else if( eDetail==FTS5_DETAIL_COLUMNS ){
          ii = (int)pCsr->iInstPos;
        }
        if( ii>=0 && ii<pCsr->pFts5->pConfig->nCol ){
          const char *z = pCsr->pFts5->pConfig->azCol[ii];
          sqlite3_result_text(pCtx, z, -1, SQLITE_STATIC);
        }
        break;
      }
      default: {
        assert( iCol==3 );
        if( eDetail==FTS5_DETAIL_FULL ){
          int ii = FTS5_POS2OFFSET(pCsr->iInstPos);
          sqlite3_result_int(pCtx, ii);
        }
        break;
      }
    }
  }

  if( iVal>0 ) sqlite3_result_int64(pCtx, iVal);
  return SQLITE_OK;
}

/* 
** This is the xRowid method. The SQLite core calls this routine to
** retrieve the rowid for the current row of the result set. The
** rowid should be written to *pRowid.
*/
static int fts5VocabRowidMethod(
  sqlite3_vtab_cursor *pCursor, 
  sqlite_int64 *pRowid
){
  Fts5VocabCursor *pCsr = (Fts5VocabCursor*)pCursor;
  *pRowid = pCsr->rowid;
  return SQLITE_OK;
}

static int sqlite3Fts5VocabInit(Fts5Global *pGlobal, sqlite3 *db){
  static const sqlite3_module fts5Vocab = {
    /* iVersion      */ 2,
    /* xCreate       */ fts5VocabCreateMethod,
    /* xConnect      */ fts5VocabConnectMethod,
    /* xBestIndex    */ fts5VocabBestIndexMethod,
    /* xDisconnect   */ fts5VocabDisconnectMethod,
    /* xDestroy      */ fts5VocabDestroyMethod,
    /* xOpen         */ fts5VocabOpenMethod,
    /* xClose        */ fts5VocabCloseMethod,
    /* xFilter       */ fts5VocabFilterMethod,
    /* xNext         */ fts5VocabNextMethod,
    /* xEof          */ fts5VocabEofMethod,
    /* xColumn       */ fts5VocabColumnMethod,
    /* xRowid        */ fts5VocabRowidMethod,
    /* xUpdate       */ 0,
    /* xBegin        */ 0,
    /* xSync         */ 0,
    /* xCommit       */ 0,
    /* xRollback     */ 0,
    /* xFindFunction */ 0,
    /* xRename       */ 0,
    /* xSavepoint    */ 0,
    /* xRelease      */ 0,
    /* xRollbackTo   */ 0,
    /* xShadowName   */ 0
  };
  void *p = (void*)pGlobal;

  return sqlite3_create_module_v2(db, "fts5vocab", &fts5Vocab, p, 0);
}


    
#endif /* !defined(SQLITE_CORE) || defined(SQLITE_ENABLE_FTS5) */
