#include <stdlib.h>

__static_yoink("__die");
__static_yoink("GetSymbolByAddr");
__static_yoink("malloc_inspect_all");

char *lol(int n) {
  return malloc(n);
}

char *(*pLol)(int) = lol;

int main(int argc, char *argv[]) {
  char *s = pLol(4);
  s[4] = 1;
}
