#ifndef Py_LIMITED_API
#ifndef PYCTYPE_H
#define PYCTYPE_H
#include "libc/str/str.h"

#define Py_TOLOWER(c) kToLower[255 & (c)]
#define Py_TOUPPER(c) kToUpper[255 & (c)]

forceinline bool Py_ISDIGIT(unsigned char c) {
  return '0' <= c && c <= '9';
}

forceinline bool Py_ISLOWER(unsigned char c) {
  return 'a' <= c && c <= 'z';
}

forceinline bool Py_ISUPPER(unsigned char c) {
  return 'A' <= c && c <= 'Z';
}

forceinline bool Py_ISALPHA(unsigned char c) {
  return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

forceinline bool Py_ISALNUM(unsigned char c) {
  return ('0' <= c && c <= '9') || ('A' <= c && c <= 'Z') ||
         ('a' <= c && c <= 'z');
}

forceinline bool Py_ISSPACE(unsigned char c) {
  return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\f' ||
         c == '\v';
}

forceinline bool Py_ISXDIGIT(unsigned char c) {
  return ('0' <= c && c <= '9') || ('A' <= c && c <= 'F') ||
         ('a' <= c && c <= 'f');
}

#endif /* !PYCTYPE_H */
#endif /* !Py_LIMITED_API */
