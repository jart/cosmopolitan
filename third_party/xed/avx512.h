#ifndef COSMOPOLITAN_THIRD_PARTY_XED_AVX512_H_
#define COSMOPOLITAN_THIRD_PARTY_XED_AVX512_H_
COSMOPOLITAN_C_START_

union XedAvx512Payload1 {
  struct {
    unsigned map : 4;
    unsigned rr_inv : 1;
    unsigned b_inv : 1;
    unsigned x_inv : 1;
    unsigned r_inv : 1;
    unsigned pad : 24;
  } s;
  unsigned u32;
};

union XedAvx512Payload2 {
  struct {
    unsigned pp : 2;
    unsigned ubit : 1;
    unsigned vexdest210 : 3;
    unsigned vexdest3 : 1;
    unsigned rexw : 1;
    unsigned pad : 24;
  } s;
  unsigned u32;
};

union XedAvx512Payload3 {
  struct {
    unsigned mask : 3;
    unsigned vexdest4p : 1;
    unsigned bcrc : 1;
    unsigned llrc : 2;
    unsigned z : 1;
    unsigned pad : 24;
  } s;
  unsigned u32;
};

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_XED_AVX512_H_ */
