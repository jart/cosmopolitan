#ifndef COSMOPOLITAN_TOOL_VIZ_LIB_FORMATSTRINGTABLE_H_
#define COSMOPOLITAN_TOOL_VIZ_LIB_FORMATSTRINGTABLE_H_
COSMOPOLITAN_C_START_

typedef void *StringTableFormatter(long yn, long xn, const char *const[yn][xn],
                                   int (*)(), void *, const char *,
                                   const char *, const char *);

StringTableFormatter FormatStringTable;
StringTableFormatter FormatStringTableBasic;
StringTableFormatter FormatStringTableAsCode;
StringTableFormatter FormatStringTableAsAssembly;
StringTableFormatter FormatStringTableForAssertion;

void *FreeStringTableCells(long yn, long xn, char *[yn][xn]);

void FormatMatrixDouble(long yn, long xn, const double[yn][xn], int (*)(),
                        void *, StringTableFormatter, const char *,
                        const char *, const char *, double, double (*)(double));
void FormatMatrixFloat(long yn, long xn, const float[yn][xn], int (*)(), void *,
                       StringTableFormatter, const char *, const char *,
                       const char *, double, double (*)(double));
void FormatMatrixByte(long yn, long xn, const unsigned char[yn][xn], int (*)(),
                      void *, StringTableFormatter, const char *, const char *,
                      const char *);
void FormatMatrixShort(long yn, long xn, const short[yn][xn], int (*)(), void *,
                       StringTableFormatter, const char *, const char *,
                       const char *);

char *StringifyMatrixDouble(long yn, long xn, const double[yn][xn],
                            StringTableFormatter, const char *, const char *,
                            const char *, double,
                            double (*)(double)) mallocesque;
char *StringifyMatrixFloat(long yn, long xn, const float[yn][xn],
                           StringTableFormatter, const char *, const char *,
                           const char *, double,
                           double (*)(double)) mallocesque;
char *StringifyMatrixByte(long yn, long xn, const unsigned char[yn][xn],
                          StringTableFormatter, const char *, const char *,
                          const char *) mallocesque;
char *StringifyMatrixShort(long yn, long xn, const short[yn][xn],
                           StringTableFormatter, const char *, const char *,
                           const char *) mallocesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TOOL_VIZ_LIB_FORMATSTRINGTABLE_H_ */
