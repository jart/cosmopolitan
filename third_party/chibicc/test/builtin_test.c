#include "libc/math.h"
#include "libc/stdckdint.h"
#include "third_party/chibicc/test/test.h"

#define FPNAN       0
#define FPINFINITE  1
#define FPZERO      2
#define FPSUBNORMAL 3
#define FPNORMAL    4
#define FPCLASSIFY(x) \
  __builtin_fpclassify(FPNAN, FPINFINITE, FPNORMAL, FPSUBNORMAL, FPZERO, x)

#define conceal(x)         \
  ({                       \
    typeof(x) cloak = x;   \
    asm("" : "+r"(cloak)); \
    cloak;                 \
  })

struct frame {
  struct frame *next;
  intptr_t addr;
};

struct frame *frame(void) {
  struct frame *myframe = __builtin_frame_address(0);
  struct frame *parentframe = myframe->next;
  return parentframe;
}

void test_frame_address(void) {
  ASSERT(1, __builtin_frame_address(0) == frame());
}

void test_constant(void) {
  ASSERT(1, __builtin_constant_p(1));
  ASSERT(0, __builtin_constant_p(conceal(1)));
  ASSERT(0, __builtin_constant_p(stdin));
  ASSERT(1, __builtin_constant_p(__builtin_popcount(0b10111011)));
  ASSERT(1, __builtin_constant_p(__builtin_popcountll((size_t)0b10111011)));
}

void test_ignored(void) {
  ASSERT(123, __builtin_assume_aligned(123, 8));
  ASSERT(123, __builtin_assume_aligned(123, 32, 8));
  ASSERT(123, __builtin_expect(123, 0));
}

void __attribute__((__aligned__(16))) test_clz(void) {
  ASSERT(31, __builtin_clz(1));
  ASSERT(63, __builtin_clzl(1));
  ASSERT(63, __builtin_clzll(1));
  ASSERT(25, __builtin_clz(77));
  ASSERT(4, __builtin_clz(0x08000000));
  ASSERT(4, __builtin_clz(0xfff08000000));
  ASSERT(25, __builtin_clz(conceal(77)));
  __builtin_clz(conceal(77));
}

__attribute__((__weak__)) void test_ctz(void) {
  ASSERT(0, __builtin_ctz(1));
  ASSERT(0, __builtin_ctz(77));
  ASSERT(27, __builtin_ctz(0x08000000));
  ASSERT(27, __builtin_ctz(0xffff08000000));
  ASSERT(63, __builtin_ctzl(0x8000000000000000));
  ASSERT(63, __builtin_ctzll(0x8000000000000000));
  ASSERT(0, __builtin_ctz(conceal(77)));
}

void test_ffs(void) {
  ASSERT(0, __builtin_ffs(0));
  ASSERT(1, __builtin_ffs(1));
  ASSERT(1, __builtin_ffs(77));
  ASSERT(28, __builtin_ffs(0x08000000));
  ASSERT(28, __builtin_ffs(0xffff08000000));
  ASSERT(1, __builtin_ffs(conceal(77)));
}

void test_popcnt(void) {
  ASSERT(0, __builtin_popcount(0));
  ASSERT(1, __builtin_popcount(1));
  ASSERT(6, __builtin_popcount(0b10111011));
  ASSERT(6, __builtin_popcountl(0b10111011));
  ASSERT(6, __builtin_popcountll(0xbb00000000000000));
  ASSERT(6, __builtin_popcountl(conceal(0b10111011)));
}

void test_bswap(void) {
  ASSERT(0x3412, __builtin_bswap16(0x1234));
  ASSERT(0x78563412, __builtin_bswap32(0x12345678));
  ASSERT(0xefcdab8967452301, __builtin_bswap64(0x0123456789abcdef));
  ASSERT(0xefcdab89, __builtin_bswap32(0x0123456789abcdef));
  ASSERT(0x78563412, __builtin_bswap32(conceal(0x12345678)));
}

void test_fpclassify(void) {
  float minf = __FLT_MIN__;
  double min = __DBL_MIN__;
  long double minl = __LDBL_MIN__;
  ASSERT(FPZERO, FPCLASSIFY(.0f));
  ASSERT(FPZERO, FPCLASSIFY(.0));
  ASSERT(FPZERO, FPCLASSIFY(.0L));
  ASSERT(FPNORMAL, FPCLASSIFY(1.f));
  ASSERT(FPNORMAL, FPCLASSIFY(1.));
  ASSERT(FPNORMAL, FPCLASSIFY(1.L));
  ASSERT(FPNORMAL, FPCLASSIFY(1.f));
  ASSERT(FPNORMAL, FPCLASSIFY(1.));
  ASSERT(FPNORMAL, FPCLASSIFY(1.L));
  ASSERT(FPNORMAL, FPCLASSIFY(minf));
  ASSERT(FPNORMAL, FPCLASSIFY(min));
  ASSERT(FPNORMAL, FPCLASSIFY(minl));
  ASSERT(FPSUBNORMAL, FPCLASSIFY(minf / 2));
  ASSERT(FPSUBNORMAL, FPCLASSIFY(min / 2));
  ASSERT(FPSUBNORMAL, FPCLASSIFY(minl / 2));
  ASSERT(FPNAN, FPCLASSIFY(__builtin_nanf("")));
  ASSERT(FPNAN, FPCLASSIFY(__builtin_nan("")));
  ASSERT(FPNAN, FPCLASSIFY(__builtin_nanl("")));
}

void test_logb(void) {
  ASSERT(6, __builtin_logbl(123.456));
  ASSERT(logbl(123.456L), __builtin_logbl(123.456L));
  ASSERT(logbl(__LDBL_MIN__), __builtin_logbl(__LDBL_MIN__));
  ASSERT(logbl(__LDBL_MAX__), __builtin_logbl(__LDBL_MAX__));
}

void test_fmax(void) {
  ASSERT(fmaxl(1, 2), __builtin_fmaxl(1, 2));
  ASSERT(2, __builtin_fmaxl(__builtin_nanl(""), 2));
  ASSERT(1, __builtin_fmaxl(1, __builtin_nanl("")));
  ASSERT(2, fmaxl(nanl(""), 2));
  ASSERT(1, fmaxl(1, nanl("")));
  ASSERT(fmaxf(1, 2), __builtin_fmaxf(1, 2));
  ASSERT(2, __builtin_fmaxf(__builtin_nanl(""), 2));
  ASSERT(1, __builtin_fmaxf(1, __builtin_nanl("")));
  ASSERT(2, fmaxf(nanl(""), 2));
  ASSERT(1, fmaxf(1, nanl("")));
  ASSERT(fmax(1, 2), __builtin_fmax(1, 2));
  ASSERT(2, __builtin_fmax(__builtin_nanl(""), 2));
  ASSERT(1, __builtin_fmax(1, __builtin_nanl("")));
  ASSERT(2, fmax(nanl(""), 2));
  ASSERT(1, fmax(1, nanl("")));
}

void test_strlen(void) {
  ASSERT(5, strlen("hello"));
  ASSERT(5, __builtin_strlen("hello"));
}

void test_strchr(void) {
  ASSERT(1, __builtin_strchr("hello", 'z') == NULL);
  ASSERT(0, (strcmp)(__builtin_strchr("hello", 'e'), "ello"));
}

void test_strpbrk(void) {
  ASSERT(1, __builtin_strpbrk("hello", "z") == NULL);
  ASSERT(0, (strcmp)(__builtin_strpbrk("hello", "ze"), "ello"));
}

void test_strstr(void) {
  ASSERT(1, __builtin_strstr("hello", "sup") == NULL);
  ASSERT(0, (strcmp)(__builtin_strstr("hello", "ell"), "ello"));
}

void test_memcpy(void) {
  {
    char x[5] = {4, 3, 2, 1, 0};
    char y[5] = {0, 1, 2, 3, 4};
    char z[5] = {2, 3, 4, 1, 0};
    ASSERT(1, x == (memcpy)(x, y + 2, 3));
    ASSERT(0, memcmp(x, z, 5));
  }
  {
    char x[5] = {4, 3, 2, 1, 0};
    char y[5] = {0, 1, 2, 3, 4};
    char z[5] = {2, 3, 4, 1, 0};
    ASSERT(1, x == __builtin_memcpy(x, y + 2, 3));
    ASSERT(0, memcmp(x, z, 5));
  }
  {
    int n = 3;
    char x[5] = {4, 3, 2, 1, 0};
    char y[5] = {0, 1, 2, 3, 4};
    char z[5] = {2, 3, 4, 1, 0};
    ASSERT(1, x == __builtin_memcpy(x, y + 2, n));
    ASSERT(0, memcmp(x, z, 5));
  }
  {
    char x[5] = {4, 3, 2, 1, 0};
    ASSERT(1, x == __builtin_memcpy(x, x + 1, 4));
    ASSERT(0, memcmp(x, (char[5]){3, 2, 1, 0, 0}, 5));
  }
}

void test_inf(void) {
  ASSERT(0, __builtin_isinf(0));
  ASSERT(0, __builtin_isinf(1));
  ASSERT(1, __builtin_isinf(__builtin_inff()));
  ASSERT(1, __builtin_isinf(-__builtin_inff()));
  ASSERT(1, __builtin_isinf(__builtin_inf()));
  ASSERT(1, __builtin_isinf(-__builtin_inf()));
  ASSERT(1, __builtin_isinf(__builtin_infl()));
  ASSERT(1, __builtin_isinf(-__builtin_infl()));
  ASSERT(1, __builtin_isfinite(0));
  ASSERT(1, __builtin_isfinite(1));
  ASSERT(0, __builtin_isfinite(__builtin_inff()));
  ASSERT(0, __builtin_isfinite(-__builtin_inff()));
  ASSERT(0, __builtin_isfinite(__builtin_inf()));
  ASSERT(0, __builtin_isfinite(-__builtin_inf()));
  ASSERT(0, __builtin_isfinite(__builtin_infl()));
  ASSERT(0, __builtin_isfinite(-__builtin_infl()));
}

void test_signbit(void) {
  ASSERT(0, !!__builtin_signbitf(0));
  ASSERT(0, !!__builtin_signbitf(0.f));
  ASSERT(0, !!__builtin_signbit(0.));
  ASSERT(0, !!__builtin_signbitl(0.L));
  ASSERT(1, !!__builtin_signbitf(-0.f));
  ASSERT(1, !!__builtin_signbit(-0.));
  ASSERT(1, !!__builtin_signbitl(-0.L));
  ASSERT(0, !!__builtin_signbitf(__builtin_nanf("")));
  ASSERT(1, !!__builtin_signbitf(-__builtin_nanf("")));
  ASSERT(0, !!__builtin_signbit(__builtin_nan("")));
  ASSERT(1, !!__builtin_signbit(-__builtin_nan("")));
  ASSERT(0, !!__builtin_signbitl(__builtin_nanl("")));
  ASSERT(1, !!__builtin_signbitl(-__builtin_nanl("")));
  ASSERT(0, !!__builtin_signbitf(__builtin_inff()));
  ASSERT(1, !!__builtin_signbitf(-__builtin_inff()));
  ASSERT(0, !!__builtin_signbit(__builtin_inf()));
  ASSERT(1, !!__builtin_signbit(-__builtin_inf()));
  ASSERT(0, !!__builtin_signbitl(__builtin_infl()));
  ASSERT(1, !!__builtin_signbitl(-__builtin_infl()));
}

void test_nan(void) {
  ASSERT(0, __builtin_isnan(0));
  ASSERT(0, __builtin_isnan(1));
  ASSERT(1, __builtin_isnan(__builtin_nanf("")));
  ASSERT(1, __builtin_isnan(-__builtin_nanf("")));
  ASSERT(1, __builtin_isnan(__builtin_nan("")));
  ASSERT(1, __builtin_isnan(-__builtin_nan("")));
  ASSERT(1, __builtin_isnan(__builtin_nanl("")));
  ASSERT(1, __builtin_isnan(-__builtin_nanl("")));
  ASSERT(0, __builtin_isunordered(0, 0));
  ASSERT(0, __builtin_isunordered(-1, 1));
  ASSERT(1, __builtin_isunordered(0, __builtin_nanf("")));
  ASSERT(1, __builtin_isunordered(__builtin_nanf(""), 0));
  ASSERT(1, __builtin_isunordered(__builtin_nanf(""), __builtin_nanf("")));
}

void test_double(void) { /* TODO */
  /* ASSERT(1, __DBL_MIN__ < 0.0L); */
  /* ASSERT(1, __DBL_MAX__ > 0.0L); */
}

void test_types_compatible_p(void) {
  ASSERT(1, __builtin_types_compatible_p(int, int));
  ASSERT(1, __builtin_types_compatible_p(double, double));
  ASSERT(0, __builtin_types_compatible_p(int, long));
  ASSERT(0, __builtin_types_compatible_p(long, float));
  ASSERT(1, __builtin_types_compatible_p(int *, int *));
  ASSERT(0, __builtin_types_compatible_p(short *, int *));
  ASSERT(0, __builtin_types_compatible_p(int **, int *));
  ASSERT(1, __builtin_types_compatible_p(const int, int));
  ASSERT(0, __builtin_types_compatible_p(unsigned, int));
  ASSERT(1, __builtin_types_compatible_p(signed, int));
  ASSERT(0, __builtin_types_compatible_p(
                struct { int a; }, struct { int a; }));
  ASSERT(1, __builtin_types_compatible_p(int (*)(void), int (*)(void)));
  ASSERT(1, __builtin_types_compatible_p(void (*)(int), void (*)(int)));
  ASSERT(1, __builtin_types_compatible_p(void (*)(int, double),
                                         void (*)(int, double)));
  ASSERT(1, __builtin_types_compatible_p(int (*)(float, double),
                                         int (*)(float, double)));
  ASSERT(0, __builtin_types_compatible_p(int (*)(float, double), int));
  ASSERT(0,
         __builtin_types_compatible_p(int (*)(float, double), int (*)(float)));
  ASSERT(0, __builtin_types_compatible_p(int (*)(float, double),
                                         int (*)(float, double, int)));
  ASSERT(1, __builtin_types_compatible_p(double (*)(...), double (*)(...)));
  ASSERT(0, __builtin_types_compatible_p(double (*)(...), double (*)(void)));
  ASSERT(1, ({
           typedef struct {
             int a;
           } T;
           __builtin_types_compatible_p(T, T);
         }));
  ASSERT(1, ({
           typedef struct {
             int a;
           } T;
           __builtin_types_compatible_p(T, const T);
         }));
  ASSERT(1, ({
           struct {
             int a;
             int b;
           } x;
           __builtin_types_compatible_p(typeof(x.a), typeof(x.b));
         }));
}

void test_offsetof(void) {
  ASSERT(0, ({
           struct T {
             int a;
             int b;
           };
           __builtin_offsetof(struct T, a);
         }));
  ASSERT(4, ({
           struct T {
             int a;
             int b;
           };
           __builtin_offsetof(struct T, b);
         }));
}

int main() {
  test_constant();
  test_frame_address();
  test_types_compatible_p();
  test_clz();
  test_ctz();
  test_ffs();
  test_bswap();
  test_popcnt();
  test_inf();
  test_nan();
  test_double();
  test_fpclassify();
  test_signbit();
  test_memcpy();
  test_offsetof();
  test_ignored();
  test_strlen();
  test_strchr();
  test_strpbrk();
  test_strstr();
  test_logb();
  test_fmax();
  return 0;
}
