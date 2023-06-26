// clang-format off
#include "third_party/mold/elf/mold.h"
#include "third_party/mold/elf/lto.h"

namespace mold::elf {

template <typename E>
ObjectFile<E> *read_lto_object(Context<E> &ctx, MappedFile<Context<E>> *mf) {
  Fatal(ctx) << "LTO is not supported on Windows";
}

template <typename E>
std::vector<ObjectFile<E> *> do_lto(Context<E> &ctx) {
  return {};
}

template <typename E>
void lto_cleanup(Context<E> &ctx) {}

using E = MOLD_TARGET;

template ObjectFile<E> *read_lto_object(Context<E> &, MappedFile<Context<E>> *);
template std::vector<ObjectFile<E> *> do_lto(Context<E> &);
template void lto_cleanup(Context<E> &);

} // namespace mold::elf
