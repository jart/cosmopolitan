#ifndef COSMOPOLITAN_THIRD_PARTY_AVIR_LANCZOS1_HPP_
#define COSMOPOLITAN_THIRD_PARTY_AVIR_LANCZOS1_HPP_
#include "third_party/avir/lancir.h"

struct Lanczos1Impl {
  Lanczos1Impl() : lanczos{} {
  }
  avir::CLancIR lanczos;
};

#endif /* COSMOPOLITAN_THIRD_PARTY_AVIR_LANCZOS1_HPP_ */
