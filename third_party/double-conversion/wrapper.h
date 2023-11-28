#ifndef COSMOPOLITAN_THIRD_PARTY_DOUBLE_CONVERSION_WRAPPER_H_
#define COSMOPOLITAN_THIRD_PARTY_DOUBLE_CONVERSION_WRAPPER_H_
COSMOPOLITAN_C_START_

char *DoubleToLua(char[128], double);
char *DoubleToJson(char[128], double);
char *DoubleToEcmascript(char[128], double);
double StringToDouble(const char *, size_t, int *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_DOUBLE_CONVERSION_WRAPPER_H_ */
