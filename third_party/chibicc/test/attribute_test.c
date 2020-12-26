#include "third_party/chibicc/test/test.h"

void doge() __attribute__((__nonnull__));
void cate(char *) __attribute__((__nonnull__(1)));
int var __attribute__((__section__(".data.var")));
int ar[4] __attribute__((__section__(".data.var")));
typedef int int2[2] __attribute__((__aligned__(64)));
typedef int int4[4] __attribute__((__warn_if_not_aligned__(16)));

__attribute__((__nonnull__)) void doge2();
__attribute__((__nonnull__(1))) void cate2(char *);
__attribute__((__section__(".data.var"))) int var2;
__attribute__((__section__(".data.var"))) int ar2[4];

__attribute__((__force_align_arg_pointer__, __no_caller_saved_registers__)) int
main() {
  int2 a;
  ASSERT(64, _Alignof(int2));
  ASSERT(64, _Alignof(a));
  ASSERT(5, ({
           struct {
             char a;
             int b;
           } __attribute__((packed)) x;
           sizeof(x);
         }));
  ASSERT(0, offsetof(
                struct __attribute__((packed)) {
                  char a;
                  int b;
                },
                a));
  ASSERT(1, offsetof(
                struct __attribute__((packed)) {
                  char a;
                  int b;
                },
                b));

  ASSERT(5, ({
           struct __attribute__((packed)) {
             char a;
             int b;
           } x;
           sizeof(x);
         }));
  ASSERT(0, offsetof(
                struct {
                  char a;
                  int b;
                } __attribute__((packed)),
                a));
  ASSERT(1, offsetof(
                struct {
                  char a;
                  int b;
                } __attribute__((packed)),
                b));

  ASSERT(9, ({
           typedef struct {
             char a;
             int b[2];
           } __attribute__((packed)) T;
           sizeof(T);
         }));
  ASSERT(9, ({
           typedef struct __attribute__((packed)) {
             char a;
             int b[2];
           } T;
           sizeof(T);
         }));

  ASSERT(1, offsetof(
                struct __attribute__((packed)) T {
                  char a;
                  int b[2];
                },
                b));
  ASSERT(1, _Alignof(struct __attribute__((packed)) {
           char a;
           int b[2];
         }));

  ASSERT(8, ({
           struct __attribute__((aligned(8))) {
             int a;
           } x;
           _Alignof(x);
         }));
  ASSERT(8, ({
           struct {
             int a;
           } __attribute__((aligned(8))) x;
           _Alignof(x);
         }));

  ASSERT(8, ({
           struct __attribute__((aligned(8), packed)) {
             char a;
             int b;
           } x;
           _Alignof(x);
         }));
  ASSERT(8, ({
           struct {
             char a;
             int b;
           } __attribute__((aligned(8), packed)) x;
           _Alignof(x);
         }));
  ASSERT(1, offsetof(
                struct __attribute__((aligned(8), packed)) {
                  char a;
                  int b;
                },
                b));
  ASSERT(1, offsetof(
                struct {
                  char a;
                  int b;
                } __attribute__((aligned(8), packed)),
                b));

  ASSERT(8, ({
           struct __attribute__((aligned(8))) __attribute__((packed)) {
             char a;
             int b;
           } x;
           _Alignof(x);
         }));
  ASSERT(8, ({
           struct {
             char a;
             int b;
           } __attribute__((aligned(8))) __attribute__((packed)) x;
           _Alignof(x);
         }));
  ASSERT(1, offsetof(
                struct __attribute__((aligned(8))) __attribute__((packed)) {
                  char a;
                  int b;
                },
                b));
  ASSERT(1, offsetof(
                struct {
                  char a;
                  int b;
                } __attribute__((aligned(8))) __attribute__((packed)),
                b));

  ASSERT(8, ({
           struct __attribute__((aligned(8))) {
             char a;
             int b;
           } __attribute__((packed)) x;
           _Alignof(x);
         }));
  ASSERT(1, offsetof(
                struct __attribute__((aligned(8))) {
                  char a;
                  int b;
                } __attribute__((packed)),
                b));

  ASSERT(16, ({
           struct __attribute__((aligned(8 + 8))) {
             char a;
             int b;
           } x;
           _Alignof(x);
         }));

  return 0;
}
