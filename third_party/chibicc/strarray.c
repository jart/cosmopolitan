#include "third_party/chibicc/chibicc.h"

void strarray_push(StringArray *arr, char *s) {
  size_t i;
  if (!arr->data) {
    arr->data = calloc(8, sizeof(char *));
    arr->capacity = 8;
  }
  if (arr->len + 1 == arr->capacity) {
    arr->capacity += arr->capacity >> 1;
    arr->data = realloc(arr->data, arr->capacity * sizeof(*arr->data));
    for (i = arr->len; i < arr->capacity; i++) {
      arr->data[i] = 0;
    }
  }
  arr->data[arr->len++] = s;
}
