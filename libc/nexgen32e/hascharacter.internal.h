#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_HASCHARACTER_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_HASCHARACTER_H_

forceinline bool HasCharacter(char c, const char *s) {
  unsigned i;
  for (i = 0; s[i]; ++i) {
    if (s[i] == c) {
      return true;
    }
  }
  return false;
}

forceinline bool HasCharacter16(char16_t c, const char16_t *s) {
  unsigned i;
  for (i = 0; s[i]; ++i) {
    if (s[i] == c) {
      return true;
    }
  }
  return false;
}

forceinline bool HasCharacterWide(wchar_t c, const wchar_t *s) {
  unsigned i;
  for (i = 0; s[i]; ++i) {
    if (s[i] == c) {
      return true;
    }
  }
  return false;
}

#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_HASCHARACTER_H_ */
