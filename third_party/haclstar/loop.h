#ifndef COSMOPOLITAN_THIRD_PARTY_HACLSTAR_LOOP_H_
#define COSMOPOLITAN_THIRD_PARTY_HACLSTAR_LOOP_H_
COSMOPOLITAN_C_START_

#define KRML_UNROLL_MAX 16
#define KRML_LOOP1(i, n, x) \
    { \
        x i += n; \
        (void)i; \
    }
#define KRML_LOOP2(i, n, x) KRML_LOOP1(i, n, x) KRML_LOOP1(i, n, x)
#define KRML_LOOP3(i, n, x) KRML_LOOP2(i, n, x) KRML_LOOP1(i, n, x)
#define KRML_LOOP4(i, n, x) KRML_LOOP2(i, n, x) KRML_LOOP2(i, n, x)
#define KRML_LOOP5(i, n, x) KRML_LOOP4(i, n, x) KRML_LOOP1(i, n, x)
#define KRML_LOOP6(i, n, x) KRML_LOOP4(i, n, x) KRML_LOOP2(i, n, x)
#define KRML_LOOP7(i, n, x) KRML_LOOP4(i, n, x) KRML_LOOP3(i, n, x)
#define KRML_LOOP8(i, n, x) KRML_LOOP4(i, n, x) KRML_LOOP4(i, n, x)
#define KRML_LOOP9(i, n, x) KRML_LOOP8(i, n, x) KRML_LOOP1(i, n, x)
#define KRML_LOOP10(i, n, x) KRML_LOOP8(i, n, x) KRML_LOOP2(i, n, x)
#define KRML_LOOP11(i, n, x) KRML_LOOP8(i, n, x) KRML_LOOP3(i, n, x)
#define KRML_LOOP12(i, n, x) KRML_LOOP8(i, n, x) KRML_LOOP4(i, n, x)
#define KRML_LOOP13(i, n, x) KRML_LOOP8(i, n, x) KRML_LOOP5(i, n, x)
#define KRML_LOOP14(i, n, x) KRML_LOOP8(i, n, x) KRML_LOOP6(i, n, x)
#define KRML_LOOP15(i, n, x) KRML_LOOP8(i, n, x) KRML_LOOP7(i, n, x)
#define KRML_LOOP16(i, n, x) KRML_LOOP8(i, n, x) KRML_LOOP8(i, n, x)
#define KRML_UNROLL_FOR(i, z, n, k, x) \
    do { \
        uint32_t i = z; \
        KRML_LOOP##n(i, k, x) \
    } while (0)
#define KRML_ACTUAL_FOR(i, z, n, k, x) \
    do { \
        for (uint32_t i = z; i < n; i += k) { \
            x \
        } \
    } while (0)
#define KRML_MAYBE_FOR0(i, z, n, k, x)
#define KRML_MAYBE_FOR1(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 1, k, x)
#define KRML_MAYBE_FOR2(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 2, k, x)
#define KRML_MAYBE_FOR3(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 3, k, x)
#define KRML_MAYBE_FOR4(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 4, k, x)
#define KRML_MAYBE_FOR5(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 5, k, x)
#define KRML_MAYBE_FOR6(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 6, k, x)
#define KRML_MAYBE_FOR7(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 7, k, x)
#define KRML_MAYBE_FOR8(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 8, k, x)
#define KRML_MAYBE_FOR9(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 9, k, x)
#define KRML_MAYBE_FOR10(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 10, k, x)
#define KRML_MAYBE_FOR11(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 11, k, x)
#define KRML_MAYBE_FOR12(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 12, k, x)
#define KRML_MAYBE_FOR13(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 13, k, x)
#define KRML_MAYBE_FOR14(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 14, k, x)
#define KRML_MAYBE_FOR15(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 15, k, x)
#define KRML_MAYBE_FOR16(i, z, n, k, x) KRML_UNROLL_FOR(i, z, 16, k, x)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_HACLSTAR_LOOP_H_ */
