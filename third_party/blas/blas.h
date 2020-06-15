#ifndef COSMOPOLITAN_THIRD_PARTY_BLAS_BLAS_H_
#define COSMOPOLITAN_THIRD_PARTY_BLAS_BLAS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int dgemm_(char *transa, char *transb, long *m, long *n, long *k, double *alpha,
           double *A /*['N'?k:m][1≤m≤lda]*/, long *lda,
           double *B /*['N'?k:n][1≤n≤ldb]*/, long *ldb, double *beta,
           double *C /*[n][1≤m≤ldc]*/, long *ldc);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_BLAS_BLAS_H_ */
