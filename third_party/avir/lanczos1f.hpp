#ifndef COSMOPOLITAN_THIRD_PARTY_AVIR_LANCZOS1F_HPP_
#define COSMOPOLITAN_THIRD_PARTY_AVIR_LANCZOS1F_HPP_
#include "third_party/avir/lancir.h"

struct Lanczos1fImpl {
  Lanczos1fImpl() : lanczos{} {
  }
  avir::CLancIR lanczos;
};

#endif /* COSMOPOLITAN_THIRD_PARTY_AVIR_LANCZOS1F_HPP_ */
