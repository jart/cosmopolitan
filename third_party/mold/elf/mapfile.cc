// clang-format off
#include "third_party/mold/elf/mold.h"

#include "third_party/libcxx/fstream"
#include "third_party/libcxx/iomanip"
#include "third_party/libcxx/ios"
#include "third_party/libcxx/sstream"
// MISSING #include <tbb/parallel_for_each.h>
#include "third_party/libcxx/unordered_map"

namespace mold::elf {

template <typename E>
using Map =
  tbb::concurrent_hash_map<InputSection<E> *, std::vector<Symbol<E> *>>;

template <typename E>
static std::unique_ptr<std::ofstream> open_output_file(Context<E> &ctx) {
  std::unique_ptr<std::ofstream> file(new std::ofstream);
  file->open(ctx.arg.Map.c_str());
  if (!file->is_open())
    Fatal(ctx) << "cannot open " << ctx.arg.Map << ": " << errno_string();
  return file;
}

template <typename E>
static Map<E> get_map(Context<E> &ctx) {
  Map<E> map;

  tbb::parallel_for_each(ctx.objs, [&](ObjectFile<E> *file) {
    for (Symbol<E> *sym : file->symbols) {
      if (sym->file != file || sym->get_type() == STT_SECTION)
        continue;

      if (InputSection<E> *isec = sym->get_input_section()) {
        assert(file == &isec->file);
        typename Map<E>::accessor acc;
        map.insert(acc, {isec, {}});
        acc->second.push_back(sym);
      }
    }
  });

  if (map.size() <= 1)
    return map;

  tbb::parallel_for(map.range(), [](const typename Map<E>::range_type &range) {
    for (auto it = range.begin(); it != range.end(); it++) {
      std::vector<Symbol<E> *> &vec = it->second;
      sort(vec, [](Symbol<E> *a, Symbol<E> *b) { return a->value < b->value; });
    }
  });
  return map;
}

template <typename E>
void print_map(Context<E> &ctx) {
  std::ostream *out = &std::cout;
  std::unique_ptr<std::ofstream> file;

  if (!ctx.arg.Map.empty()) {
    file = open_output_file(ctx);
    out = file.get();
  }

  // Construct a section-to-symbol map.
  Map<E> map = get_map(ctx);

  // Print a mapfile.
  *out << "               VMA       Size Align Out     In      Symbol\n";

  for (Chunk<E> *osec : ctx.chunks) {
    *out << std::showbase
         << std::setw(18) << std::hex << (u64)osec->shdr.sh_addr << std::dec
         << std::setw(11) << (u64)osec->shdr.sh_size
         << std::setw(6) << (u64)osec->shdr.sh_addralign
         << " " << osec->name << "\n";

    if (osec->kind() != OUTPUT_SECTION)
      continue;

    std::span<InputSection<E> *> members = ((OutputSection<E> *)osec)->members;
    std::vector<std::string> bufs(members.size());

    tbb::parallel_for((i64)0, (i64)members.size(), [&](i64 i) {
      InputSection<E> *mem = members[i];
      std::ostringstream ss;
      opt_demangle = ctx.arg.demangle;
      u64 addr = osec->shdr.sh_addr + mem->offset;

      ss << std::showbase
         << std::setw(18) << std::hex << addr << std::dec
         << std::setw(11) << (u64)mem->sh_size
         << std::setw(6) << (1 << (u64)mem->p2align)
         << "         " << *mem << "\n";

      typename Map<E>::const_accessor acc;
      if (map.find(acc, mem))
        for (Symbol<E> *sym : acc->second)
          ss << std::showbase
             << std::setw(18) << std::hex << sym->get_addr(ctx) << std::dec
             << "          0     0                 "
             << *sym << "\n";

      bufs[i] = ss.str();
    });

    for (std::string &str : bufs)
      *out << str;
  }
}

using E = MOLD_TARGET;

template void print_map(Context<E> &ctx);

} // namespace mold::elf
