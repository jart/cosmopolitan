#ifndef COSMOPOLITAN_TOOL_BUILD_EMUBIN_METALSHA256_H_
#define COSMOPOLITAN_TOOL_BUILD_EMUBIN_METALSHA256_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct MetalSha256Ctx {
  uint8_t data[64];
  uint32_t datalen;
  uint64_t bitlen;
  uint32_t state[8];
};

void MetalSha256Init(struct MetalSha256Ctx *);
void MetalSha256Update(struct MetalSha256Ctx *, const uint8_t *, long);
void MetalSha256Final(struct MetalSha256Ctx *, uint8_t *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_EMUBIN_METALSHA256_H_ */
