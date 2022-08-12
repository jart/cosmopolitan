#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_PUN_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_PUN_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

union FloatPun {
  float f;
  uint32_t i;
};

union DoublePun {
  double f;
  uint64_t i;
};

union FloatVectorPun {
  union FloatPun u[4];
  float f[4];
};

union DoubleVectorPun {
  union DoublePun u[2];
  double f[2];
};

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_PUN_H_ */
