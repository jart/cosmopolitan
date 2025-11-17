#define IS_DEFINED

#if 0
#error oh no
#elifdef IS_DEFINED
int need1;
#else
#error oh no
#endif

#if 0
#error oh no
#elifdef IS_NOT_DEFINED
#error oh no
#else
int need2;
#endif

int main(void) {
  return need1 + need2;
}
