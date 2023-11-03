#include <dlfcn.h>

int main(int argc, char *argv[]) {
  return ((int (*)(void *))argv[1])((void *[]){
      dlopen,
      dlsym,
      dlclose,
      dlerror,
  });
}
