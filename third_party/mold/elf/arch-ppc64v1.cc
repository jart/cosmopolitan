// clang-format off
// This file contains code for the 64-bit PowerPC ELFv1 ABI that is
// commonly used for big-endian PPC systems. Modern PPC systems that use
// the processor in the little-endian mode use the ELFv2 ABI instead. For
// ELFv2, see arch-ppc64v2.cc.
//
// Even though they are similiar, ELFv1 isn't only different from ELFv2 in
// endianness. The most notable difference is, in ELFv1, a function
// pointer doesn't directly refer to the entry point of a function but
// instead refers to a data structure so-called "function descriptor".
//
// The function descriptor is essentially a pair of a function entry point
// address and a value that should be set to %r2 before calling that
// function. There is also a third member for "the environment pointer for
// languages such as Pascal and PL/1" according to the psABI, but it looks
// like no one acutally uses it. In total, the function descriptor is 24
// bytes long. Here is why we need it.
//
// PPC generally lacks PC-relative data access instructions. Position-
// independent code sets GOT + 0x8000 to %r2 and access global variables
// relative to %r2.
//
// Each ELF file has its own GOT. If a function calls another function in
// the same ELF file, it doesn't have to reset %r2. However, if it is in
// other file (e.g. other .so), it has to set a new value to %r2 so that
// the register contains the callee's GOT + 0x8000.
//
// In this way, you can't call a function just by knowing the function's
// entry point address. You also need to know a proper %r2 value for the
// function. This is why a function pointer refers to a tuple of an
// address and a %r2 value.
//
// If a function call is made through PLT, PLT takes care of restoring %r2.
// Therefore, the caller has to restore %r2 only for function calls
// through function pointers.
//
// .opd (short for "official procedure descriptors") contains function
// descriptors.
//
// You can think OPD as this: even in other targets, a function can have a
// few different addresses for different purposes. It may not only have an
// entry point address but may also have PLT and/or GOT addresses.
// In PPCV1, it may have an OPD address in addition to these. OPD address
// is used for relocations that refers to the address of a function as a
// function pointer.
//
// https://github.com/rui314/psabi/blob/main/ppc64v1.pdf

#include "third_party/mold/elf/mold.h"

#include "third_party/libcxx/algorithm"
// MISSING #include <tbb/parallel_for_each.h>

namespace mold::elf {

using E = PPC64V1;

static u64 lo(u64 x)    { return x & 0xffff; }
static u64 hi(u64 x)    { return x >> 16; }
static u64 ha(u64 x)    { return (x + 0x8000) >> 16; }
static u64 high(u64 x)  { return (x >> 16) & 0xffff; }
static u64 higha(u64 x) { return ((x + 0x8000) >> 16) & 0xffff; }

// .plt is used only for lazy symbol resolution on PPC64. All PLT
// calls are made via range extension thunks even if they are within
// reach. Thunks read addresses from .got.plt and jump there.
// Therefore, once PLT symbols are resolved and final addresses are
// written to .got.plt, thunks just skip .plt and directly jump to the
// resolved addresses.
template <>
void write_plt_header(Context<E> &ctx, u8 *buf) {
  static const ub32 insn[] = {
    0x7d88'02a6, // mflr    r12
    0x429f'0005, // bcl     20, 31, 4 // obtain PC
    0x7d68'02a6, // mflr    r11
    0xe84b'0024, // ld      r2,36(r11)
    0x7d88'03a6, // mtlr    r12
    0x7d62'5a14, // add     r11,r2,r11
    0xe98b'0000, // ld      r12,0(r11)
    0xe84b'0008, // ld      r2,8(r11)
    0x7d89'03a6, // mtctr   r12
    0xe96b'0010, // ld      r11,16(r11)
    0x4e80'0420, // bctr
    // .quad .got.plt - .plt - 8
    0x0000'0000,
    0x0000'0000,
  };

  static_assert(sizeof(insn) == E::plt_hdr_size);
  memcpy(buf, insn, sizeof(insn));
  *(ub64 *)(buf + 44) = ctx.gotplt->shdr.sh_addr - ctx.plt->shdr.sh_addr - 8;
}

template <>
void write_plt_entry(Context<E> &ctx, u8 *buf, Symbol<E> &sym) {
  ub32 *loc = (ub32 *)buf;
  i64 idx = sym.get_plt_idx(ctx);

  // The PPC64 ELFv1 ABI requires PLT entries to be vary in size depending
  // on their indices. Unlike other targets, .got.plt is filled not by us
  // but by the loader, so we don't have a control over where the initial
  // call to the PLT entry jumps to. So we need to strictly follow the PLT
  // section layout as the loader expect it to be.
  if (idx < 0x8000) {
    static const ub32 insn[] = {
      0x3800'0000, // li      r0, PLT_INDEX
      0x4b00'0000, // b       plt0
    };

    memcpy(loc, insn, sizeof(insn));
    loc[0] |= idx;
    loc[1] |= (ctx.plt->shdr.sh_addr - sym.get_plt_addr(ctx) - 4) & 0x00ff'ffff;
  } else {
    static const ub32 insn[] = {
      0x3c00'0000, // lis     r0, PLT_INDEX@high
      0x6000'0000, // ori     r0, r0, PLT_INDEX@lo
      0x4b00'0000, // b       plt0
    };

    memcpy(loc, insn, sizeof(insn));
    loc[0] |= high(idx);
    loc[1] |= lo(idx);
    loc[2] |= (ctx.plt->shdr.sh_addr - sym.get_plt_addr(ctx) - 8) & 0x00ff'ffff;
  }
}

// .plt.got is not necessary on PPC64 because range extension thunks
// directly read GOT entries and jump there.
template <>
void write_pltgot_entry(Context<E> &ctx, u8 *buf, Symbol<E> &sym) {}

template <>
void EhFrameSection<E>::apply_reloc(Context<E> &ctx, const ElfRel<E> &rel,
                                    u64 offset, u64 val) {
  u8 *loc = ctx.buf + this->shdr.sh_offset + offset;

  switch (rel.r_type) {
  case R_NONE:
    break;
  case R_PPC64_ADDR64:
    *(ub64 *)loc = val;
    break;
  case R_PPC64_REL32:
    *(ub32 *)loc = val - this->shdr.sh_addr - offset;
    break;
  case R_PPC64_REL64:
    *(ub64 *)loc = val - this->shdr.sh_addr - offset;
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

    auto check = [&](i64 val, i64 lo, i64 hi) {
      if (val < lo || hi <= val)
        Error(ctx) << *this << ": relocation " << rel << " against "
                   << sym << " out of range: " << val << " is not in ["
                   << lo << ", " << hi << ")";
    };

    u64 S = sym.get_addr(ctx);
    u64 A = rel.r_addend;
    u64 P = get_addr() + rel.r_offset;
    u64 G = sym.get_got_idx(ctx) * sizeof(Word<E>);
    u64 GOT = ctx.got->shdr.sh_addr;
    u64 TOC = ctx.extra.TOC->value;

    switch (rel.r_type) {
    case R_PPC64_ADDR64:
      apply_toc_rel(ctx, sym, rel, loc, S, A, P, dynrel);
      break;
    case R_PPC64_TOC:
      apply_toc_rel(ctx, *ctx.extra.TOC, rel, loc, TOC, A, P, dynrel);
      break;
    case R_PPC64_TOC16_HA:
      *(ub16 *)loc = ha(S + A - TOC);
      break;
    case R_PPC64_TOC16_LO:
      *(ub16 *)loc = lo(S + A - TOC);
      break;
    case R_PPC64_TOC16_DS:
      check(S + A - TOC, -(1 << 15), 1 << 15);
      *(ub16 *)loc |= (S + A - TOC) & 0xfffc;
      break;
    case R_PPC64_TOC16_LO_DS:
      *(ub16 *)loc |= (S + A - TOC) & 0xfffc;
      break;
    case R_PPC64_REL24: {
      i64 val = sym.get_addr(ctx, NO_OPD) + A - P;
      if (sym.has_plt(ctx) || sign_extend(val, 25) != val)
        val = get_thunk_addr(i) + A - P;

      check(val, -(1 << 25), 1 << 25);
      *(ub32 *)loc |= bits(val, 25, 2) << 2;

      // If a callee is an external function, PLT saves %r2 to the
      // caller's r2 save slot. We need to restore it after function
      // return. To do so, there's usually a NOP as a placeholder
      // after a BL. 0x6000'0000 is a NOP.
      if (sym.has_plt(ctx) && *(ub32 *)(loc + 4) == 0x6000'0000)
        *(ub32 *)(loc + 4) = 0xe841'0028; // ld r2, 40(r1)
      break;
    }
    case R_PPC64_REL32:
      *(ub32 *)loc = S + A - P;
      break;
    case R_PPC64_REL64:
      *(ub64 *)loc = S + A - P;
      break;
    case R_PPC64_REL16_HA:
      *(ub16 *)loc = ha(S + A - P);
      break;
    case R_PPC64_REL16_LO:
      *(ub16 *)loc = lo(S + A - P);
      break;
    case R_PPC64_PLT16_HA:
      *(ub16 *)loc = ha(G + GOT - TOC);
      break;
    case R_PPC64_PLT16_HI:
      *(ub16 *)loc = hi(G + GOT - TOC);
      break;
    case R_PPC64_PLT16_LO:
      *(ub16 *)loc = lo(G + GOT - TOC);
      break;
    case R_PPC64_PLT16_LO_DS:
      *(ub16 *)loc |= (G + GOT - TOC) & 0xfffc;
      break;
    case R_PPC64_GOT_TPREL16_HA:
      *(ub16 *)loc = ha(sym.get_gottp_addr(ctx) - TOC);
      break;
    case R_PPC64_GOT_TLSGD16_HA:
      *(ub16 *)loc = ha(sym.get_tlsgd_addr(ctx) - TOC);
      break;
    case R_PPC64_GOT_TLSGD16_LO:
      *(ub16 *)loc = lo(sym.get_tlsgd_addr(ctx) - TOC);
      break;
    case R_PPC64_GOT_TLSLD16_HA:
      *(ub16 *)loc = ha(ctx.got->get_tlsld_addr(ctx) - TOC);
      break;
    case R_PPC64_GOT_TLSLD16_LO:
      *(ub16 *)loc = lo(ctx.got->get_tlsld_addr(ctx) - TOC);
      break;
    case R_PPC64_DTPREL16_HA:
      *(ub16 *)loc = ha(S + A - ctx.dtp_addr);
      break;
    case R_PPC64_DTPREL16_LO:
      *(ub16 *)loc = lo(S + A - ctx.dtp_addr);
      break;
    case R_PPC64_TPREL16_HA:
      *(ub16 *)loc = ha(S + A - ctx.tp_addr);
      break;
    case R_PPC64_TPREL16_LO:
      *(ub16 *)loc = lo(S + A - ctx.tp_addr);
      break;
    case R_PPC64_GOT_TPREL16_LO_DS:
      *(ub16 *)loc |= (sym.get_gottp_addr(ctx) - TOC) & 0xfffc;
      break;
    case R_PPC64_PLTSEQ:
    case R_PPC64_PLTCALL:
    case R_PPC64_TLS:
    case R_PPC64_TLSGD:
    case R_PPC64_TLSLD:
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

    auto check = [&](i64 val, i64 lo, i64 hi) {
      if (val < lo || hi <= val)
        Error(ctx) << *this << ": relocation " << rel << " against "
                   << sym << " out of range: " << val << " is not in ["
                   << lo << ", " << hi << ")";
    };

    SectionFragment<E> *frag;
    i64 frag_addend;
    std::tie(frag, frag_addend) = get_fragment(ctx, rel);

    u64 S = frag ? frag->get_addr(ctx) : sym.get_addr(ctx);
    u64 A = frag ? frag_addend : (i64)rel.r_addend;

    switch (rel.r_type) {
    case R_PPC64_ADDR64:
      if (std::optional<u64> val = get_tombstone(sym, frag))
        *(ub64 *)loc = *val;
      else
        *(ub64 *)loc = S + A;
      break;
    case R_PPC64_ADDR32: {
      i64 val = S + A;
      check(val, 0, 1LL << 32);
      *(ub32 *)loc = val;
      break;
    }
    case R_PPC64_DTPREL64:
      *(ub64 *)loc = S + A - ctx.dtp_addr;
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

  // Scan relocations
  for (i64 i = 0; i < rels.size(); i++) {
    const ElfRel<E> &rel = rels[i];
    if (rel.r_type == R_NONE || record_undef_error(ctx, rel))
      continue;

    Symbol<E> &sym = *file.symbols[rel.r_sym];

    if (sym.is_ifunc())
      sym.flags |= NEEDS_GOT | NEEDS_PLT | NEEDS_PPC_OPD;

    // Any relocation except R_PPC64_REL24 is considered as an
    // address-taking relocation.
    if (rel.r_type != R_PPC64_REL24 && sym.get_type() == STT_FUNC)
      sym.flags |= NEEDS_PPC_OPD;

    switch (rel.r_type) {
    case R_PPC64_ADDR64:
    case R_PPC64_TOC:
      scan_toc_rel(ctx, sym, rel);
      break;
    case R_PPC64_GOT_TPREL16_HA:
      sym.flags |= NEEDS_GOTTP;
      break;
    case R_PPC64_REL24:
      if (sym.is_imported)
        sym.flags |= NEEDS_PLT;
      break;
    case R_PPC64_PLT16_HA:
      sym.flags |= NEEDS_GOT;
      break;
    case R_PPC64_GOT_TLSGD16_HA:
      sym.flags |= NEEDS_TLSGD;
      break;
    case R_PPC64_GOT_TLSLD16_HA:
      ctx.needs_tlsld = true;
      break;
    case R_PPC64_TPREL16_HA:
    case R_PPC64_TPREL16_LO:
      check_tlsle(ctx, sym, rel);
      break;
    case R_PPC64_REL32:
    case R_PPC64_REL64:
    case R_PPC64_TOC16_HA:
    case R_PPC64_TOC16_LO:
    case R_PPC64_TOC16_LO_DS:
    case R_PPC64_TOC16_DS:
    case R_PPC64_REL16_HA:
    case R_PPC64_REL16_LO:
    case R_PPC64_PLT16_HI:
    case R_PPC64_PLT16_LO:
    case R_PPC64_PLT16_LO_DS:
    case R_PPC64_PLTSEQ:
    case R_PPC64_PLTCALL:
    case R_PPC64_GOT_TPREL16_LO_DS:
    case R_PPC64_GOT_TLSGD16_LO:
    case R_PPC64_GOT_TLSLD16_LO:
    case R_PPC64_TLS:
    case R_PPC64_TLSGD:
    case R_PPC64_TLSLD:
    case R_PPC64_DTPREL16_HA:
    case R_PPC64_DTPREL16_LO:
      break;
    default:
      Error(ctx) << *this << ": unknown relocation: " << rel;
    }
  }
}

template <>
void RangeExtensionThunk<E>::copy_buf(Context<E> &ctx) {
  u8 *buf = ctx.buf + output_section.shdr.sh_offset + offset;

  // If the destination is .plt.got, we save the current r2, read an
  // address of a function descriptor from .got, restore %r2 and jump
  // to the function.
  static const ub32 pltgot_thunk[] = {
    // Store the caller's %r2
    0xf841'0028, // std   %r2, 40(%r1)

    // Load an address of a function descriptor
    0x3d82'0000, // addis %r12, %r2,  foo@got@toc@ha
    0xe98c'0000, // ld    %r12, foo@got@toc@lo(%r12)

    // Restore the callee's %r2
    0xe84c'0008, // ld    %r2,  8(%r12)

    // Jump to the function
    0xe98c'0000, // ld    %r12, 0(%r12)
    0x7d89'03a6, // mtctr %r12
    0x4e80'0420, // bctr
  };

  // If the destination is .plt, read a function descriptor from .got.plt.
  static const ub32 plt_thunk[] = {
    // Store the caller's %r2
    0xf841'0028, // std   %r2, 40(%r1)

    // Materialize an address of a function descriptor
    0x3d82'0000, // addis %r12, %r2,  foo@gotplt@toc@ha
    0x398c'0000, // addi  %r12, %r12, foo@gotplt@toc@lo

    // Restore the callee's %r2
    0xe84c'0008, // ld    %r2,  8(%r12)

    // Jump to the function
    0xe98c'0000, // ld    %r12, 0(%r12)
    0x7d89'03a6, // mtctr %r12
    0x4e80'0420, // bctr
  };

  // If the destination is a non-imported function, we directly jump
  // to the function entry address.
  static const ub32 local_thunk[] = {
    0x3d82'0000, // addis r12, r2,  foo@toc@ha
    0x398c'0000, // addi  r12, r12, foo@toc@lo
    0x7d89'03a6, // mtctr r12
    0x4e80'0420, // bctr
    0x6000'0000, // nop
    0x6000'0000, // nop
    0x6000'0000, // nop
  };

  static_assert(E::thunk_size == sizeof(pltgot_thunk));
  static_assert(E::thunk_size == sizeof(plt_thunk));
  static_assert(E::thunk_size == sizeof(local_thunk));

  for (i64 i = 0; i < symbols.size(); i++) {
    Symbol<E> &sym = *symbols[i];
    ub32 *loc = (ub32 *)(buf + i * E::thunk_size);

    if (sym.has_got(ctx)) {
      memcpy(loc, pltgot_thunk, sizeof(pltgot_thunk));
      i64 val = sym.get_got_addr(ctx) - ctx.extra.TOC->value;
      loc[1] |= higha(val);
      loc[2] |= lo(val);
    } else if(sym.has_plt(ctx)) {
      memcpy(loc, plt_thunk, sizeof(plt_thunk));
      i64 val = sym.get_gotplt_addr(ctx) - ctx.extra.TOC->value;
      loc[1] |= higha(val);
      loc[2] |= lo(val);
    } else {
      memcpy(loc, local_thunk, sizeof(local_thunk));
      i64 val = sym.get_addr(ctx, NO_OPD) - ctx.extra.TOC->value;
      loc[0] |= higha(val);
      loc[1] |= lo(val);
    }
  }
}

static InputSection<E> *get_opd_section(ObjectFile<E> &file) {
  for (std::unique_ptr<InputSection<E>> &isec : file.sections)
    if (isec && isec->name() == ".opd")
      return isec.get();
  return nullptr;
}

static ElfRel<E> *
get_relocation_at(Context<E> &ctx, InputSection<E> &isec, i64 offset) {
  std::span<ElfRel<E>> rels = isec.get_rels(ctx);

  auto it = std::lower_bound(rels.begin(), rels.end(), offset,
                             [](const ElfRel<E> &r, i64 offset) {
    return r.r_offset < offset;
  });

  if (it == rels.end())
    return nullptr;
  if (it->r_offset != offset)
    return nullptr;
  return &*it;
}

struct OpdSymbol {
  bool operator<(const OpdSymbol &x) const { return r_offset < x.r_offset; }

  u64 r_offset = 0;
  Symbol<E> *sym = nullptr;
};

static Symbol<E> *
get_opd_sym_at(Context<E> &ctx, std::span<OpdSymbol> syms, u64 offset) {
  auto it = std::lower_bound(syms.begin(), syms.end(), OpdSymbol{offset});
  if (it == syms.end())
    return nullptr;
  if (it->r_offset != offset)
    return nullptr;
  return it->sym;
}

// Compiler creates an .opd entry for each function symbol. The intention
// is to make it possible to create an output .opd section just by linking
// input .opd sections in the same manner as we do to other normal input
// sections.
//
// However, in reality, .opd isn't a normal input section. It needs many
// special treatments as follows:
//
// 1. A function symbol refers to not a .text but an .opd. Its address
//    works fine for address-taking relocations such as R_PPC64_ADDR64.
//    However, R_PPC64_REL24 (which is used for branch instruction) needs
//    a function's real address instead of the function's .opd address.
//    We need to read .opd contents to find out a function entry point
//    address to apply R_PPC64_REL24.
//
// 2. Output .opd entries are needed only for functions whose addresses
//    are taken. Just copying input .opd sections to an output would
//    produces lots of dead .opd entries.
//
// 3. In this design, all function symbols refer to an .opd section, and
//    that doesn't work well with graph traversal optimizations such as
//    garbage collection or identical comdat folding. For example, garbage
//    collector would mark an .opd alive which in turn mark all functions
//    thatare referenced by .opd as alive, effectively keeping all
//    functions as alive.
//
// The problem is that the compiler creates a half-baked .opd section, and
// the linker has to figure out what all these .opd entries and
// relocations are trying to achieve. It's like the compiler would emit a
// half-baked .plt section in an object file and the linker has to deal
// with that. That's not a good design.
//
// So, in this function, we undo what the compiler did to .opd. We remove
// function symbols from .opd and reattach them to their function entry
// points. We also rewrite relocations that directly refer to an input
// .opd  section so that they refer to function symbols instead. We then
// mark input .opd sections as dead.
//
// After this function, we mark symbols with the NEEDS_PPC_OPD flag if the
// symbol needs an .opd entry. We then create an output .opd just like we
// do for .plt or .got.
void ppc64v1_rewrite_opd(Context<E> &ctx) {
  tbb::parallel_for_each(ctx.objs, [&](ObjectFile<E> *file) {
    InputSection<E> *opd = get_opd_section(*file);
    if (!opd)
      return;
    opd->is_alive = false;

    // Move symbols from .opd to .text.
    std::vector<OpdSymbol> opd_syms;

    for (Symbol<E> *sym : file->symbols) {
      if (sym->file != file || sym->get_input_section() != opd)
        continue;

      if (u32 ty = sym->get_type(); ty != STT_FUNC && ty != STT_GNU_IFUNC)
        continue;

      ElfRel<E> *rel = get_relocation_at(ctx, *opd, sym->value);
      if (!rel)
        Fatal(ctx) << *file << ": cannot find a relocation in .opd for "
                   << *sym << " at offset 0x" << std::hex << (u64)sym->value;

      Symbol<E> *sym2 = file->symbols[rel->r_sym];
      if (sym2->get_type() != STT_SECTION)
        Fatal(ctx) << *file << ": bad relocation in .opd referring " << *sym2;

      opd_syms.push_back({sym->value, sym});

      sym->set_input_section(sym2->get_input_section());
      sym->value = rel->r_addend;
    }

    // Sort symbols so that get_opd_sym_at() can do binary search.
    sort(opd_syms);

    // Rewrite relocations so that they directly refer to .opd.
    for (std::unique_ptr<InputSection<E>> &isec : file->sections) {
      if (!isec || !isec->is_alive || isec.get() == opd)
        continue;

      for (ElfRel<E> &r : isec->get_rels(ctx)) {
        Symbol<E> &sym = *file->symbols[r.r_sym];
        if (sym.get_input_section() != opd)
          continue;

        Symbol<E> *real_sym = get_opd_sym_at(ctx, opd_syms, r.r_addend);
        if (!real_sym)
          Fatal(ctx) << *isec << ": cannot find a symbol in .opd for " << r
                     << " at offset 0x" << std::hex << (u64)r.r_addend;

        r.r_sym = real_sym->sym_idx;
        r.r_addend = 0;
      }
    }
  });
}

// When a function is exported, the dynamic symbol for the function should
// refers to the function's .opd entry. This function marks such symbols
// with NEEDS_PPC_OPD.
void ppc64v1_scan_symbols(Context<E> &ctx) {
  tbb::parallel_for_each(ctx.objs, [&](ObjectFile<E> *file) {
    for (Symbol<E> *sym : file->symbols)
      if (sym->file == file && sym->is_exported)
        if (u32 ty = sym->get_type(); ty == STT_FUNC || ty == STT_GNU_IFUNC)
          sym->flags |= NEEDS_PPC_OPD;
  });

  // Functions referenced by the ELF header also have to have .opd entries.
  auto mark = [&](std::string_view name) {
    if (!name.empty())
      if (Symbol<E> &sym = *get_symbol(ctx, name); !sym.is_imported)
        sym.flags |= NEEDS_PPC_OPD;
  };

  mark(ctx.arg.entry);
  mark(ctx.arg.init);
  mark(ctx.arg.fini);
}

void PPC64OpdSection::add_symbol(Context<E> &ctx, Symbol<E> *sym) {
  sym->set_opd_idx(ctx, symbols.size());
  symbols.push_back(sym);
  this->shdr.sh_size += ENTRY_SIZE;
}

i64 PPC64OpdSection::get_reldyn_size(Context<E> &ctx) const {
  if (ctx.arg.pic)
    return symbols.size() * 2;
  return 0;
}

void PPC64OpdSection::copy_buf(Context<E> &ctx) {
  ub64 *buf = (ub64 *)(ctx.buf + this->shdr.sh_offset);

  ElfRel<E> *rel = nullptr;
  if (ctx.arg.pic)
    rel = (ElfRel<E> *)(ctx.buf + ctx.reldyn->shdr.sh_offset + reldyn_offset);

  for (Symbol<E> *sym : symbols) {
    u64 addr = sym->get_addr(ctx, NO_PLT | NO_OPD);
    *buf++ = addr;
    *buf++ = ctx.extra.TOC->value;
    *buf++ = 0;

    if (ctx.arg.pic) {
      u64 loc = sym->get_opd_addr(ctx);
      *rel++ = ElfRel<E>(loc, E::R_RELATIVE, 0, addr);
      *rel++ = ElfRel<E>(loc + 8, E::R_RELATIVE, 0, ctx.extra.TOC->value);
    }
  }
}

} // namespace mold::elf
