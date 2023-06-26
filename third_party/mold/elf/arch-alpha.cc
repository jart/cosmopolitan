// clang-format off
// Alpha is a 64-bit RISC ISA developed by DEC (Digital Equipment
// Corporation) in the early '90s. It aimed to be an ISA that would last
// 25 years. DEC expected Alpha would become 1000x faster during that time
// span. Since the ISA was developed from scratch for future machines,
// it's 64-bit from the beginning. There's no 32-bit variant.
//
// DEC ported its own Unix (Tru64) to Alpha. Microsoft also ported Windows
// NT to it. But it wasn't a huge commercial success.
//
// DEC was acquired by Compaq in 1997. In the late '90s, Intel and
// Hewlett-Packard were advertising that their upcoming Itanium processor
// would achieve significantly better performance than RISC processors, so
// Compaq decided to discontinue the Alpha processor line to switch to
// Itanium. Itanium resulted in a miserable failure, but it still suceeded
// to wipe out several RISC processors just by promising overly optimistic
// perf numbers. Alpha as an ISA would probably have been fine after 25
// years since its introduction (which is 1992 + 25 = 2017), but the
// company and its market didn't last that long.
//
// From the linker's point of view, there are a few peculiarities in its
// psABI as shown below:
//
//  - Alpha lacks PC-relative memory load/store instructions, so it uses
//    register-relative load/store instructions in position-independent
//    code. Specifically, GP (which is an alias for $r29) is always
//    maintained to refer to .got+0x8000, and global variables' addresses
//    are loaded in a GP-relative manner.
//
//  - It looks like even function addresses are first loaded to register
//    in a GP-relative manner before calling it. We can relax it to
//    convert the instruction sequence with a direct branch instruction,
//    but by default, object files don't use a direct branch to call a
//    function. Therefore, by default, we don't need to create a PLT.
//    Any function call is made by first reading its address from GOT and
//    jump to the address.

#include "third_party/mold/elf/mold.h"

namespace mold::elf {

using E = ALPHA;

// A 32-bit immediate can be materialized in a register with a "load high"
// and a "load low" instruction sequence. The first instruction sets the
// upper 16 bits in a register, and the second one set the lower 16
// bits. When doing so, they sign-extend an immediate.  Therefore, if the
// 15th bit of an immediate happens to be 1, setting a "low half" value
// negates the upper 16 bit values that has already been set in a
// register. To compensate that, we need to add 0x8000 when setting the
// upper 16 bits.
static u32 hi(u32 val) {
  return bits(val + 0x8000, 31, 16);
}

template <>
void write_plt_header(Context<E> &ctx, u8 *buf) {}

template <>
void write_plt_entry(Context<E> &ctx, u8 *buf, Symbol<E> &sym) {}

template <>
void write_pltgot_entry(Context<E> &ctx, u8 *buf, Symbol<E> &sym) {}

template <>
void EhFrameSection<E>::apply_reloc(Context<E> &ctx, const ElfRel<E> &rel,
                                    u64 offset, u64 val) {
  u8 *loc = ctx.buf + this->shdr.sh_offset + offset;

  switch (rel.r_type) {
  case R_NONE:
    break;
  case R_ALPHA_SREL32:
    *(ul32 *)loc = val - this->shdr.sh_addr - offset;
    break;
  default:
    Fatal(ctx) << "unsupported relocation in .eh_frame: " << rel;
  }
}

template <>
void InputSection<E>::apply_reloc_alloc(Context<E> &ctx, u8 *base) {
  std::span<const ElfRel<E>> rels = get_rels(ctx);

  ElfRel<E> *dynrel = nullptr;
  if (ctx.reldyn)
    dynrel = (ElfRel<E> *)(ctx.buf + ctx.reldyn->shdr.sh_offset +
                           file.reldyn_offset + this->reldyn_offset);

  for (i64 i = 0; i < rels.size(); i++) {
    const ElfRel<E> &rel = rels[i];
    if (rel.r_type == R_NONE)
      continue;

    Symbol<E> &sym = *file.symbols[rel.r_sym];
    u8 *loc = base + rel.r_offset;

    u64 S = sym.get_addr(ctx);
    u64 A = rel.r_addend;
    u64 P = get_addr() + rel.r_offset;
    u64 G = sym.get_got_idx(ctx) * sizeof(Word<E>);
    u64 GOT = ctx.got->shdr.sh_addr;
    u64 GP = ctx.got->shdr.sh_addr + 0x8000;

    switch (rel.r_type) {
    case R_ALPHA_REFQUAD:
      apply_dyn_absrel(ctx, sym, rel, loc, S, A, P, dynrel);
      break;
    case R_ALPHA_GPREL32:
      *(ul32 *)loc = S + A - GP;
      break;
    case R_ALPHA_LITERAL:
      if (A)
        *(ul16 *)loc = ctx.extra.got->get_addr(sym, A) - GP;
      else
        *(ul16 *)loc = GOT + G - GP;
      break;
    case R_ALPHA_BRSGP:
      *(ul32 *)loc |= bits(S + A - P - 4, 22, 0);
      break;
    case R_ALPHA_GPDISP:
      *(ul16 *)loc = hi(GP - P);
      *(ul16 *)(loc + A) = GP - P;
      break;
    case R_ALPHA_SREL32:
      *(ul32 *)loc = S + A - P;
      break;
    case R_ALPHA_GPRELHIGH:
      *(ul16 *)loc = hi(S + A - GP);
      break;
    case R_ALPHA_GPRELLOW:
      *(ul16 *)loc = S + A - GP;
      break;
    case R_ALPHA_TLSGD:
      *(ul16 *)loc = sym.get_tlsgd_addr(ctx) - GP;
      break;
    case R_ALPHA_TLSLDM:
      *(ul16 *)loc = ctx.got->get_tlsld_addr(ctx) - GP;
      break;
    case R_ALPHA_DTPRELHI:
      *(ul16 *)loc = hi(S + A - ctx.dtp_addr);
      break;
    case R_ALPHA_DTPRELLO:
      *(ul16 *)loc = S + A - ctx.dtp_addr;
      break;
    case R_ALPHA_GOTTPREL:
      *(ul16 *)loc = sym.get_gottp_addr(ctx) + A - GP;
      break;
    case R_ALPHA_TPRELHI:
      *(ul16 *)loc = hi(S + A - ctx.tp_addr);
      break;
    case R_ALPHA_TPRELLO:
      *(ul16 *)loc = S + A - ctx.tp_addr;
      break;
    case R_ALPHA_LITUSE:
    case R_ALPHA_HINT:
      break;
    default:
      unreachable();
    }
  }
}

template <>
void InputSection<E>::apply_reloc_nonalloc(Context<E> &ctx, u8 *base) {
  std::span<const ElfRel<E>> rels = get_rels(ctx);

  for (i64 i = 0; i < rels.size(); i++) {
    const ElfRel<E> &rel = rels[i];
    if (rel.r_type == R_NONE || record_undef_error(ctx, rel))
      continue;

    Symbol<E> &sym = *file.symbols[rel.r_sym];
    u8 *loc = base + rel.r_offset;

    SectionFragment<E> *frag;
    i64 frag_addend;
    std::tie(frag, frag_addend) = get_fragment(ctx, rel);

    u64 S = frag ? frag->get_addr(ctx) : sym.get_addr(ctx);
    u64 A = frag ? frag_addend : (i64)rel.r_addend;

    switch (rel.r_type) {
    case R_ALPHA_REFLONG:
      if (std::optional<u64> val = get_tombstone(sym, frag))
        *(ul32 *)loc = *val;
      else
        *(ul32 *)loc = S + A;
      break;
    case R_ALPHA_REFQUAD:
      if (std::optional<u64> val = get_tombstone(sym, frag))
        *(ul64 *)loc = *val;
      else
        *(ul64 *)loc = S + A;
      break;
    default:
      Fatal(ctx) << *this << ": invalid relocation for non-allocated sections: "
                 << rel;
    }
  }
}

template <>
void InputSection<E>::scan_relocations(Context<E> &ctx) {
  assert(shdr().sh_flags & SHF_ALLOC);

  this->reldyn_offset = file.num_dynrel * sizeof(ElfRel<E>);
  std::span<const ElfRel<E>> rels = get_rels(ctx);

  for (i64 i = 0; i < rels.size(); i++) {
    const ElfRel<E> &rel = rels[i];
    if (rel.r_type == R_NONE || record_undef_error(ctx, rel))
      continue;

    Symbol<E> &sym = *file.symbols[rel.r_sym];

    if (sym.is_ifunc())
      Error(ctx) << sym << ": GNU ifunc symbol is not supported on Alpha";

    switch (rel.r_type) {
    case R_ALPHA_REFQUAD:
      scan_dyn_absrel(ctx, sym, rel);
      break;
    case R_ALPHA_LITERAL:
      if (rel.r_addend)
        ctx.extra.got->add_symbol(sym, rel.r_addend);
      else
        sym.flags |= NEEDS_GOT;
      break;
    case R_ALPHA_SREL32:
      scan_pcrel(ctx, sym, rel);
      break;
    case R_ALPHA_BRSGP:
      if (sym.is_imported)
        sym.flags |= NEEDS_PLT;
      break;
    case R_ALPHA_TLSGD:
      sym.flags |= NEEDS_TLSGD;
      break;
    case R_ALPHA_TLSLDM:
      ctx.needs_tlsld = true;
      break;
    case R_ALPHA_GOTTPREL:
      sym.flags |= NEEDS_GOTTP;
      break;
    case R_ALPHA_TPRELHI:
    case R_ALPHA_TPRELLO:
      check_tlsle(ctx, sym, rel);
      break;
    case R_ALPHA_GPREL32:
    case R_ALPHA_LITUSE:
    case R_ALPHA_GPDISP:
    case R_ALPHA_HINT:
    case R_ALPHA_GPRELHIGH:
    case R_ALPHA_GPRELLOW:
    case R_ALPHA_DTPRELHI:
    case R_ALPHA_DTPRELLO:
      break;
    default:
      Fatal(ctx) << *this << ": unknown relocation: " << rel;
    }
  }
}

// An R_ALPHA_LITERAL relocation may request the linker to create a GOT
// entry for an external symbol with a non-zero addend. This is an unusual
// request which is not found in any other targets.
//
// Referring an external symbol with a non-zero addend is a bad practice
// because we need to create as many dynamic relocations as the number of
// distinctive addends for the same symbol.
//
// We don't want to mess up the implementation of the common GOT section
// for Alpha. So we create another GOT-like section, .alpha_got. Any GOT
// entry for an R_ALPHA_LITERAL reloc with a non-zero addend is created
// not in .got but in .alpha_got.
//
// Since .alpha_got entries are accessed relative to GP, .alpha_got
// needs to be close enough to .got. It's actually placed next to .got.
void AlphaGotSection::add_symbol(Symbol<E> &sym, i64 addend) {
  assert(addend);
  std::scoped_lock lock(mu);
  entries.push_back({&sym, addend});
}

bool operator<(const AlphaGotSection::Entry &a, const AlphaGotSection::Entry &b) {
  return std::tuple(a.sym->file->priority, a.sym->sym_idx, a.addend) <
         std::tuple(b.sym->file->priority, b.sym->sym_idx, b.addend);
};

u64 AlphaGotSection::get_addr(Symbol<E> &sym, i64 addend) {
  auto it = std::lower_bound(entries.begin(), entries.end(), Entry{&sym, addend});
  assert(it != entries.end());
  return this->shdr.sh_addr + (it - entries.begin()) * sizeof(Word<E>);
}

i64 AlphaGotSection::get_reldyn_size(Context<E> &ctx) const {
  i64 n = 0;
  for (const Entry &e : entries)
    if (e.sym->is_imported || (ctx.arg.pic && !e.sym->is_absolute()))
      n++;
  return n;
}

void AlphaGotSection::finalize() {
  sort(entries);
  remove_duplicates(entries);
  shdr.sh_size = entries.size() * sizeof(Word<E>);
}

void AlphaGotSection::copy_buf(Context<E> &ctx) {
  ElfRel<E> *dynrel = (ElfRel<E> *)(ctx.buf + ctx.reldyn->shdr.sh_offset +
                                    reldyn_offset);

  for (i64 i = 0; i < entries.size(); i++) {
    Entry &e = entries[i];
    u64 P = this->shdr.sh_addr + sizeof(Word<E>) * i;
    ul64 *buf = (ul64 *)(ctx.buf + this->shdr.sh_offset + sizeof(Word<E>) * i);

    if (e.sym->is_imported) {
      *buf = ctx.arg.apply_dynamic_relocs ? e.addend : 0;
      *dynrel++ = ElfRel<E>(P, E::R_ABS, e.sym->get_dynsym_idx(ctx), e.addend);
    } else {
      *buf = e.sym->get_addr(ctx) + e.addend;
      if (ctx.arg.pic && !e.sym->is_absolute())
        *dynrel++ = ElfRel<E>(P, E::R_RELATIVE, 0, *buf);
    }
  }
}

} // namespace mold::elf
