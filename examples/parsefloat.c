#include <stdio.h>

#define PARSE_AND_PRINT(type, scan_fmt, print_fmt, str)       \
  do {                                                        \
    type val; int ret;                                        \
    ret = sscanf(str, scan_fmt, &val);                        \
    printf("\"%s\" => " print_fmt " = %d\n", str, val, ret);  \
  } while (0)

int main()
{
    PARSE_AND_PRINT(float, "%f", "%f", "0.3715");
    PARSE_AND_PRINT(float, "%f", "%f", ".3715");
    PARSE_AND_PRINT(float, "%f", "%f", "3715");
    PARSE_AND_PRINT(float, "%f", "%f", "111.11");
    PARSE_AND_PRINT(float, "%f", "%f", "-2.22");
    PARSE_AND_PRINT(float, "%f", "%f", "Nan");
    PARSE_AND_PRINT(float, "%f", "%f", "nAn(2)");
    PARSE_AND_PRINT(float, "%f", "%f", "-NAN(_asdfZXCV1234_)");
    PARSE_AND_PRINT(float, "%f", "%f", "-nan");
    PARSE_AND_PRINT(float, "%f", "%f", "+nan");
    PARSE_AND_PRINT(float, "%f", "%f", "inF");
    PARSE_AND_PRINT(float, "%f", "%f", "iNfINiTy");
    PARSE_AND_PRINT(float, "%f", "%f", "+inf");
    PARSE_AND_PRINT(float, "%f", "%f", "-inf");
    PARSE_AND_PRINT(float, "%f", "%f", "0X1.BC70A3D70A3D7P+6");
    PARSE_AND_PRINT(float, "%f", "%f", "1.18973e+4932zzz");
    PARSE_AND_PRINT(float, "%f", "%.10f", "   -0.0000000123junk");
    PARSE_AND_PRINT(float, "%f", "%f", "junk");
    return 0;
}
