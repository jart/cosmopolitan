// clang-format off
// RISC instructions are usually up to 4 bytes long, so the immediates of
// their branch instructions are naturally smaller than 32 bits.  This is
// contrary to x86-64 on which branch instructions take 4 bytes immediates
// and can jump to anywhere within PC ± 2 GiB.
//
// In fact, ARM32's branch instructions can jump only within ±16 MiB and
// ARM64's ±128 MiB, for example. If a branch target is further than that,
// we need to let it branch to a linker-synthesized code sequence that
// construct a full 32 bit address in a register and jump there. That
// linker-synthesized code is called "thunk".
//
// The function in this file creates thunks.
//
// Note that although thunks play an important role in an executable, they
// don't take up too much space in it. For example, among the clang-16's
// text segment whose size is ~300 MiB on ARM64, thunks in total occupy
// only ~30 KiB or 0.01%. Of course the number depends on an ISA; we would
// need more thunks on ARM32 whose branch range is shorter than ARM64.
// That said, the total size of thunks still isn't that much. Therefore,
// we don't need to try too hard to reduce thunk size to the absolute
// minimum.

#if MOLD_ARM32 || MOLD_ARM64 || MOLD_PPC32 || MOLD_PPC64V1 || MOLD_PPC64V2

#include "third_party/mold/elf/mold.h"

// MISSING #include <tbb/parallel_for.h>
// MISSING #include <tbb/parallel_for_each.h>

namespace mold::elf {

// Returns a branch reach in bytes for a given target.
template <typename E>
static consteval i64 max_distance() {
  // ARM64's branch has 26 bits immediate. The immediate is padded with
  // implicit two-bit zeros because all instructions are 4 bytes aligned
  // and therefore the least two bits are always zero. So the branch
  // operand is effectively 28 bits long. That means the branch range is
  // [-2^27, 2^27) or PC ± 128 MiB.
  if (is_arm64<E>)
    return 1 << 27;

  // ARM32's Thumb branch has 24 bits immediate, and the instructions are
  // aligned to 2, so it's effectively 25 bits. It's [-2^24, 2^24) or PC ±
  // 16 MiB.
  //
  // ARM32's non-Thumb branches have twice longer range than its Thumb
  // counterparts, but we conservatively use the Thumb's limitation.
  if (is_arm32<E>)
    return 1 << 24;

  // PPC's branch has 24 bits immediate, and the instructions are aligned
  // to 4, therefore the reach is [-2^25, 2^25) or PC ± 32 MiB.
  assert(is_ppc<E>);
  return 1 << 25;
}

// We create thunks for each 12.8/1.6/3.2 MiB code block for
// ARM64/ARM32/PPC, respectively.
template <typename E>
static constexpr i64 batch_size = max_distance<E>() / 10;

// We assume that a single thunk group is smaller than 100 KiB.
static constexpr i64 max_thunk_size = 102400;

// Returns true if a given relocation is of type used for function calls.
template <typename E>
static bool needs_thunk_rel(const ElfRel<E> &r) {
  u32 ty = r.r_type;

  if constexpr (is_arm64<E>) {
    return ty == R_AARCH64_JUMP26 || ty == R_AARCH64_CALL26;
  } else if constexpr (is_arm32<E>) {
    return ty == R_ARM_JUMP24 || ty == R_ARM_THM_JUMP24 ||
           ty == R_ARM_CALL   || ty == R_ARM_THM_CALL   ||
           ty == R_ARM_PLT32;
  } else if constexpr (is_ppc32<E>) {
    return ty == R_PPC_REL24  || ty == R_PPC_PLTREL24 || ty == R_PPC_LOCAL24PC;
  } else {
    static_assert(is_ppc64<E>);
    return ty == R_PPC64_REL24 || ty == R_PPC64_REL24_NOTOC;
  }
}

template <typename E>
static bool is_reachable(Context<E> &ctx, InputSection<E> &isec,
                         Symbol<E> &sym, const ElfRel<E> &rel) {
  // We create thunks with a pessimistic assumption that all
  // out-of-section relocations would be out-of-range.
  InputSection<E> *isec2 = sym.get_input_section();
  if (!isec2 || isec.output_section != isec2->output_section)
    return false;

  // Even if the target is the same section, we branch to its PLT
  // if it has one. So a symbol with a PLT is also considered an
  // out-of-section reference.
  if (sym.has_plt(ctx))
    return false;

  // If the target section is in the same output section but
  // hasn't got any address yet, that's unreacahble.
  if (isec2->offset == -1)
    return false;

  // Thumb and ARM B instructions cannot be converted to BX, so we
  // always have to make them jump to a thunk to switch processor mode
  // even if their destinations are within their ranges.
  if constexpr (is_arm32<E>) {
    bool is_thumb = sym.get_addr(ctx) & 1;
    if ((rel.r_type == R_ARM_THM_JUMP24 && !is_thumb) ||
        (rel.r_type == R_ARM_JUMP24 && is_thumb) ||
        (rel.r_type == R_ARM_PLT32 && is_thumb))
      return false;
  }

  // PowerPC before Power9 lacks PC-relative load/store instructions.
  // Functions compiled for Power9 or earlier assume that r2 points to
  // GOT+0x8000, while those for Power10 uses r2 as a scratch register.
  // We need to a thunk to recompute r2 for interworking.
  if constexpr (is_ppc64v2<E>) {
    if (rel.r_type == R_PPC64_REL24 && !sym.esym().preserves_r2())
      return false;
    if (rel.r_type == R_PPC64_REL24_NOTOC && sym.esym().uses_toc())
      return false;
  }

  // Compute a distance between the relocated place and the symbol
  // and check if they are within reach.
  i64 S = sym.get_addr(ctx, NO_OPD);
  i64 A = get_addend(isec, rel);
  i64 P = isec.get_addr() + rel.r_offset;
  i64 val = S + A - P;
  return -max_distance<E>() <= val && val < max_distance<E>();
}

template <typename E>
static void reset_thunk(RangeExtensionThunk<E> &thunk) {
  for (Symbol<E> *sym : thunk.symbols) {
    sym->extra.thunk_idx = -1;
    sym->extra.thunk_sym_idx = -1;
    sym->flags = 0;
  }
}

// Scan relocations to collect symbols that need thunks.
template <typename E>
static void scan_rels(Context<E> &ctx, InputSection<E> &isec,
                      RangeExtensionThunk<E> &thunk) {
  std::span<const ElfRel<E>> rels = isec.get_rels(ctx);
  std::vector<RangeExtensionRef> &range_extn = isec.extra.range_extn;
  range_extn.resize(rels.size());

  for (i64 i = 0; i < rels.size(); i++) {
    const ElfRel<E> &rel = rels[i];
    if (!needs_thunk_rel(rel))
      continue;

    // Skip if the symbol is undefined. apply_reloc() will report an error.
    Symbol<E> &sym = *isec.file.symbols[rel.r_sym];
    if (!sym.file)
      continue;

    // Skip if the destination is within reach.
    if (is_reachable(ctx, isec, sym, rel))
      continue;

    // This relocation needs a thunk. If the symbol is already in a
    // previous thunk, reuse it.
    if (sym.extra.thunk_idx != -1) {
      range_extn[i].thunk_idx = sym.extra.thunk_idx;
      range_extn[i].sym_idx = sym.extra.thunk_sym_idx;
      continue;
    }

    // Otherwise, add the symbol to the current thunk if it's not
    // added already.
    range_extn[i].thunk_idx = thunk.thunk_idx;
    range_extn[i].sym_idx = -1;

    if (sym.flags.exchange(-1) == 0) {
      std::scoped_lock lock(thunk.mu);
      thunk.symbols.push_back(&sym);
    }
  }
}

template <typename E>
void create_range_extension_thunks(Context<E> &ctx, OutputSection<E> &osec) {
  std::span<InputSection<E> *> m = osec.members;
  if (m.empty())
    return;

  m[0]->offset = 0;

  // Initialize input sections with a dummy offset so that we can
  // distinguish sections that have got an address with the one who
  // haven't.
  tbb::parallel_for((i64)1, (i64)m.size(), [&](i64 i) {
    m[i]->offset = -1;
  });

  // We create thunks from the beginning of the section to the end.
  // We manage progress using four offsets which increase monotonically.
  // The locations they point to are always A <= B <= C <= D.
  //
  // Input sections between B and C are in the current batch.
  //
  // A is the input section with the smallest address than can reach
  // anywhere from the current batch.
  //
  // D is the input section with the largest address such that the thunk
  // is reachable from the current batch if it's inserted right before D.
  //
  //  ................................ <input sections> ............
  //     A    B    C    D
  //                    ^ We insert a thunk for the current batch just before D
  //          <--->       The current batch, which is smaller than batch_size
  //     <-------->       Smaller than max_distance
  //          <-------->  Smaller than max_distance
  //     <------------->  Reachable from the current batch
  i64 a = 0;
  i64 b = 0;
  i64 c = 0;
  i64 d = 0;
  i64 offset = 0;
  i64 thunk_idx = 0;

  while (b < m.size()) {
    // Move D foward as far as we can jump from B to anywhere in a thunk at D.
    while (d < m.size() &&
           align_to(offset, 1 << m[d]->p2align) + m[d]->sh_size + max_thunk_size <
           m[b]->offset + max_distance<E>()) {
      offset = align_to(offset, 1 << m[d]->p2align);
      m[d]->offset = offset;
      offset += m[d]->sh_size;
      d++;
    }

    // Move C forward so that C is apart from B by BATCH_SIZE. We want
    // to make sure that there's at least one section between B and C
    // to ensure progress.
    c = b + 1;
    while (c < m.size() &&
           m[c]->offset + m[c]->sh_size < m[b]->offset + batch_size<E>)
      c++;

    // Move A forward so that A is reachable from C.
    i64 c_offset = (c == m.size()) ? offset : m[c]->offset;
    while (a < m.size() && m[a]->offset + max_distance<E>() < c_offset)
      a++;

    // Erase references to out-of-range thunks.
    while (thunk_idx < osec.thunks.size() &&
           osec.thunks[thunk_idx]->offset < m[a]->offset)
      reset_thunk(*osec.thunks[thunk_idx++]);

    // Create a thunk for input sections between B and C and place it at D.
    offset = align_to(offset, RangeExtensionThunk<E>::alignment);
    RangeExtensionThunk<E> *thunk =
      new RangeExtensionThunk<E>(osec, osec.thunks.size(), offset);
    osec.thunks.emplace_back(thunk);

    // Scan relocations between B and C to collect symbols that need thunks.
    tbb::parallel_for_each(m.begin() + b, m.begin() + c,
                           [&](InputSection<E> *isec) {
      scan_rels(ctx, *isec, *thunk);
    });

    // Now that we know the number of symbols in the thunk, we can compute
    // its size.
    assert(thunk->size() < max_thunk_size);
    offset += thunk->size();

    // Sort symbols added to the thunk to make the output deterministic.
    sort(thunk->symbols, [](Symbol<E> *a, Symbol<E> *b) {
      return std::tuple{a->file->priority, a->sym_idx} <
             std::tuple{b->file->priority, b->sym_idx};
    });

    // Assign offsets within the thunk to the symbols.
    for (i64 i = 0; i < thunk->symbols.size(); i++) {
      Symbol<E> &sym = *thunk->symbols[i];
      sym.extra.thunk_idx = thunk->thunk_idx;
      sym.extra.thunk_sym_idx = i;
    }

    // Scan relocations again to fix symbol offsets in the last thunk.
    tbb::parallel_for_each(m.begin() + b, m.begin() + c,
                           [&](InputSection<E> *isec) {
      std::span<Symbol<E> *> syms = isec->file.symbols;
      std::span<const ElfRel<E>> rels = isec->get_rels(ctx);
      std::span<RangeExtensionRef> range_extn = isec->extra.range_extn;

      for (i64 i = 0; i < rels.size(); i++)
        if (range_extn[i].thunk_idx == thunk->thunk_idx)
          range_extn[i].sym_idx = syms[rels[i].r_sym]->extra.thunk_sym_idx;
    });

    // Move B forward to point to the begining of the next batch.
    b = c;
  }

  while (thunk_idx < osec.thunks.size())
    reset_thunk(*osec.thunks[thunk_idx++]);

  osec.shdr.sh_size = offset;
}

using E = MOLD_TARGET;

static_assert(max_thunk_size / E::thunk_size < INT16_MAX);

template void create_range_extension_thunks(Context<E> &, OutputSection<E> &);

} // namespace mold::elf

#endif
