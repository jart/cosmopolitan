#include "third_party/chibicc/test/test.h"

int (*__attribute__((__ms_abi__)) NtFoo)(int x, int y);

int main() {
}
