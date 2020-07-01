#ifndef COSMOPOLITAN_THIRD_PARTY_XED_AVX_H_
#define COSMOPOLITAN_THIRD_PARTY_XED_AVX_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

union XedAvxC4Payload1 {
  struct {
    unsigned map : 5;
    unsigned b_inv : 1;
    unsigned x_inv : 1;
    unsigned r_inv : 1;
    unsigned pad : 24;
  } s;
  unsigned u32;
};

union XedAvxC4Payload2 {
  struct {
    unsigned pp : 2;
    unsigned l : 1;
    unsigned vvv210 : 3;
    unsigned v3 : 1;
    unsigned w : 1;
    unsigned pad : 24;
  } s;
  unsigned u32;
};

union XedAvxC5Payload {
  struct {
    unsigned pp : 2;
    unsigned l : 1;
    unsigned vvv210 : 3;
    unsigned v3 : 1;
    unsigned r_inv : 1;
    unsigned pad : 24;
  } s;
  unsigned u32;
};

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_XED_AVX_H_ */
