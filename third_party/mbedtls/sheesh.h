#ifndef COSMOPOLITAN_THIRD_PARTY_MBEDTLS_INCLUDE_PSA_SHEESH_H_
#define COSMOPOLITAN_THIRD_PARTY_MBEDTLS_INCLUDE_PSA_SHEESH_H_
#include "third_party/mbedtls/crypto_extra.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

static inline void psa_set_key_type(psa_key_attributes_t *attributes,
                                    psa_key_type_t type) {
  if (!attributes->domain_parameters) {
    attributes->core.type = type;
  } else {
    psa_set_key_domain_parameters(attributes, type, NULL, 0);
  }
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_MBEDTLS_INCLUDE_PSA_SHEESH_H_ */
