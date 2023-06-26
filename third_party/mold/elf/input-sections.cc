// clang-format off
#include "third_party/mold/elf/mold.h"

#include "third_party/libcxx/limits"
// MISSING #include <zlib.h>
// MISSING #include <zstd.h>

namespace mold::elf {

typedef enum {
  NONE, ERROR, COPYREL, DYN_COPYREL, PLT, CPLT, DYN_CPLT, DYNREL, BASEREL, IFUNC,
} Action;

template <typename E>
bool CieRecord<E>::equals(const CieRecord<E> &other) const {
  if (get_contents() != other.get_contents())
    return false;

  std::span<const ElfRel<E>> x = get_rels();
  std::span<const ElfRel<E>> y = other.get_rels();
  if (x.size() != y.size())
    return false;

  for (i64 i = 0; i < x.size(); i++) {
    if (x[i].r_offset - input_offset != y[i].r_offset - other.input_offset ||
        x[i].r_type != y[i].r_type ||
        file.symbols[x[i].r_sym] != other.file.symbols[y[i].r_sym] ||
        get_addend(input_section, x[i]) != get_addend(other.input_section, y[i]))
      return false;
  }
  return true;
}

static i64 to_p2align(u64 alignment) {
  if (alignment == 0)
    return 0;
  return std::countr_zero(alignment);
}

template <typename E>
InputSection<E>::InputSection(Context<E> &ctx, ObjectFile<E> &file,
                              std::string_view name, i64 shndx)
  : file(file), shndx(shndx) {
  if (shndx < file.elf_sections.size())
    contents = {(char *)file.mf->data + shdr().sh_offset, (size_t)shdr().sh_size};

  if (shdr().sh_flags & SHF_COMPRESSED) {
    ElfChdr<E> &chdr = *(ElfChdr<E> *)&contents[0];
    sh_size = chdr.ch_size;
    p2align = to_p2align(chdr.ch_addralign);
  } else {
    sh_size = shdr().sh_size;
    p2align = to_p2align(shdr().sh_addralign);
  }

  // Sections may have been compressed. We usually uncompress them
  // directly into the mmap'ed output file, but we want to uncompress
  // early for REL-type ELF types to read relocation addends from
  // section contents. For RELA-type, we don't need to do this because
  // addends are in relocations.
  //
  // SH-4 stores addends to sections despite being RELA, which is a
  // special (and buggy) case.
  if constexpr (!E::is_rela || is_sh4<E>)
    uncompress(ctx);
}

template <typename E>
void InputSection<E>::uncompress(Context<E> &ctx) {
  if (!(shdr().sh_flags & SHF_COMPRESSED) || uncompressed)
    return;

  u8 *buf = new u8[sh_size];
  uncompress_to(ctx, buf);
  contents = std::string_view((char *)buf, sh_size);
  ctx.string_pool.emplace_back(buf);
  uncompressed = true;
}

template <typename E>
void InputSection<E>::uncompress_to(Context<E> &ctx, u8 *buf) {
  if (!(shdr().sh_flags & SHF_COMPRESSED) || uncompressed) {
    memcpy(buf, contents.data(), contents.size());
    return;
  }

  if (contents.size() < sizeof(ElfChdr<E>))
    Fatal(ctx) << *this << ": corrupted compressed section";

  ElfChdr<E> &hdr = *(ElfChdr<E> *)&contents[0];
  std::string_view data = contents.substr(sizeof(ElfChdr<E>));

  switch (hdr.ch_type) {
  case ELFCOMPRESS_ZLIB: {
    unsigned long size = sh_size;
    if (::uncompress(buf, &size, (u8 *)data.data(), data.size()) != Z_OK)
      Fatal(ctx) << *this << ": uncompress failed";
    assert(size == sh_size);
    break;
  }
  case ELFCOMPRESS_ZSTD:
    if (ZSTD_decompress(buf, sh_size, (u8 *)data.data(), data.size()) != sh_size)
      Fatal(ctx) << *this << ": ZSTD_decompress failed";
    break;
  default:
    Fatal(ctx) << *this << ": unsupported compression type: 0x"
               << std::hex << hdr.ch_type;
  }
}

template <typename E>
static Action get_rel_action(Context<E> &ctx, Symbol<E> &sym,
                             const Action table[3][4]) {
  auto get_output_type = [&] {
    if (ctx.arg.shared)
      return 0;
    if (ctx.arg.pie)
      return 1;
    return 2;
  };

  auto get_sym_type = [&] {
    if (sym.is_absolute())
      return 0;
    if (!sym.is_imported)
      return 1;
    if (sym.get_type() != STT_FUNC)
      return 2;
    return 3;
  };

  return table[get_output_type()][get_sym_type()];
}

template <typename E>
static void scan_rel(Context<E> &ctx, InputSection<E> &isec, Symbol<E> &sym,
                     const ElfRel<E> &rel, Action action) {
  bool writable = (isec.shdr().sh_flags & SHF_WRITE);

  auto error = [&] {
    std::string msg = sym.is_absolute() ? "-fno-PIC" : "-fPIC";
    Error(ctx) << isec << ": " << rel << " relocation at offset 0x"
               << std::hex << rel.r_offset << " against symbol `"
               << sym << "' can not be used; recompile with " << msg;
  };

  auto check_textrel = [&] {
    if (!writable) {
      if (ctx.arg.z_text) {
        error();
      } else if (ctx.arg.warn_textrel) {
        Warn(ctx) << isec << ": relocation against symbol `" << sym
                  << "' in read-only section";
      }
      ctx.has_textrel = true;
    }
  };

  auto copyrel = [&] {
    assert(sym.is_imported);
    if (sym.esym().st_visibility == STV_PROTECTED) {
      Error(ctx) << isec
                 << ": cannot make copy relocation for protected symbol '" << sym
                 << "', defined in " << *sym.file << "; recompile with -fPIC";
    }
    sym.flags |= NEEDS_COPYREL;
  };

  auto dynrel = [&] {
    check_textrel();
    isec.file.num_dynrel++;
  };

  switch (action) {
  case NONE:
    break;
  case ERROR:
    error();
    break;
  case COPYREL:
    if (!ctx.arg.z_copyreloc)
      error();
    copyrel();
    break;
  case DYN_COPYREL:
    if (writable || !ctx.arg.z_copyreloc)
      dynrel();
    else
      copyrel();
    break;
  case PLT:
    sym.flags |= NEEDS_PLT;
    break;
  case CPLT:
    sym.flags |= NEEDS_CPLT;
    break;
  case DYN_CPLT:
    if (writable)
      dynrel();
    else
      sym.flags |= NEEDS_CPLT;
    break;
  case DYNREL:
    dynrel();
    break;
  case BASEREL:
    check_textrel();
    if (!isec.is_relr_reloc(ctx, rel))
      isec.file.num_dynrel++;
    break;
  case IFUNC:
    dynrel();
    ctx.num_ifunc_dynrels++;
    break;
  default:
    unreachable();
  }
}

template <typename E>
static Action get_pcrel_action(Context<E> &ctx, Symbol<E> &sym) {
  // This is for PC-relative relocations (e.g. R_X86_64_PC32).
  // We cannot promote them to dynamic relocations because the dynamic
  // linker generally does not support PC-relative relocations.
  constexpr static Action table[3][4] = {
    // Absolute  Local    Imported data  Imported code
    {  ERROR,    NONE,    ERROR,         PLT    },  // Shared object
    {  ERROR,    NONE,    COPYREL,       PLT    },  // Position-independent exec
    {  NONE,     NONE,    COPYREL,       CPLT   },  // Position-dependent exec
  };

  return get_rel_action(ctx, sym, table);
}

template <typename E>
static Action get_absrel_action(Context<E> &ctx, Symbol<E> &sym) {
  // This is a decision table for absolute relocations that is smaller
  // than the word size (e.g. R_X86_64_32). Since the dynamic linker
  // generally does not support dynamic relocations smaller than the
  // word size, we need to report an error if a relocation cannot be
  // resolved at link-time.
  constexpr static Action table[3][4] = {
    // Absolute  Local    Imported data  Imported code
    {  NONE,     ERROR,   ERROR,         ERROR },  // Shared object
    {  NONE,     ERROR,   ERROR,         ERROR },  // Position-independent exec
    {  NONE,     NONE,    COPYREL,       CPLT  },  // Position-dependent exec
  };

  return get_rel_action(ctx, sym, table);
}

template <typename E>
static Action get_dyn_absrel_action(Context<E> &ctx, Symbol<E> &sym) {
  if (sym.is_ifunc())
    return IFUNC;

  // This is a decision table for absolute relocations for the word
  // size data (e.g. R_X86_64_64). Unlike the absrel_table, we can emit
  // a dynamic relocation if we cannot resolve an address at link-time.
  constexpr static Action table[3][4] = {
    // Absolute  Local    Imported data  Imported code
    {  NONE,     BASEREL, DYNREL,        DYNREL   },  // Shared object
    {  NONE,     BASEREL, DYNREL,        DYNREL   },  // Position-independent exec
    {  NONE,     NONE,    DYN_COPYREL,   DYN_CPLT },  // Position-dependent exec
  };

  return get_rel_action(ctx, sym, table);
}

template <typename E>
static Action get_ppc64_toc_action(Context<E> &ctx, Symbol<E> &sym) {
  if (sym.is_ifunc())
    return IFUNC;

  // As a special case, we do not create copy relocations nor canonical
  // PLTs for .toc sections. PPC64's .toc is a compiler-generated
  // GOT-like section, and no user-generated code directly uses values
  // in it.
  constexpr static Action table[3][4] = {
    // Absolute  Local    Imported data  Imported code
    {  NONE,     BASEREL, DYNREL,        DYNREL },  // Shared object
    {  NONE,     BASEREL, DYNREL,        DYNREL },  // Position-independent exec
    {  NONE,     NONE,    DYNREL,        DYNREL },  // Position-dependent exec
  };

  return get_rel_action(ctx, sym, table);
}

template <typename E>
void InputSection<E>::scan_pcrel(Context<E> &ctx, Symbol<E> &sym,
                                 const ElfRel<E> &rel) {
  scan_rel(ctx, *this, sym, rel, get_pcrel_action(ctx, sym));
}

template <typename E>
void InputSection<E>::scan_absrel(Context<E> &ctx, Symbol<E> &sym,
                                  const ElfRel<E> &rel) {
  scan_rel(ctx, *this, sym, rel, get_absrel_action(ctx, sym));
}

template <typename E>
void InputSection<E>::scan_dyn_absrel(Context<E> &ctx, Symbol<E> &sym,
                                      const ElfRel<E> &rel) {
  scan_rel(ctx, *this, sym, rel, get_dyn_absrel_action(ctx, sym));
}

template <typename E>
void InputSection<E>::scan_toc_rel(Context<E> &ctx, Symbol<E> &sym,
                                   const ElfRel<E> &rel) {
  scan_rel(ctx, *this, sym, rel, get_ppc64_toc_action(ctx, sym));
}

template <typename E>
void InputSection<E>::check_tlsle(Context<E> &ctx, Symbol<E> &sym,
                                  const ElfRel<E> &rel) {
  if (ctx.arg.shared)
    Error(ctx) << *this << ": relocation " << rel << " against `" << sym
               << "` can not be used when making a shared object;"
               << " recompile with -fPIC";
}

template <typename E>
static void apply_absrel(Context<E> &ctx, InputSection<E> &isec,
                         Symbol<E> &sym, const ElfRel<E> &rel, u8 *loc,
                         u64 S, i64 A, u64 P, ElfRel<E> *&dynrel,
                         Action action) {
  bool writable = (isec.shdr().sh_flags & SHF_WRITE);

  auto apply_dynrel = [&] {
    *dynrel++ = ElfRel<E>(P, E::R_ABS, sym.get_dynsym_idx(ctx), A);
    if (ctx.arg.apply_dynamic_relocs)
      *(Word<E> *)loc = A;
  };

  switch (action) {
  case COPYREL:
  case CPLT:
  case NONE:
    *(Word<E> *)loc = S + A;
    break;
  case BASEREL:
    if (isec.is_relr_reloc(ctx, rel)) {
      *(Word<E> *)loc = S + A;
    } else {
      *dynrel++ = ElfRel<E>(P, E::R_RELATIVE, 0, S + A);
      if (ctx.arg.apply_dynamic_relocs)
        *(Word<E> *)loc = S + A;
    }
    break;
  case DYN_COPYREL:
    if (writable || !ctx.arg.z_copyreloc)
      apply_dynrel();
    else
      *(Word<E> *)loc = S + A;
    break;
  case DYN_CPLT:
    if (writable)
      apply_dynrel();
    else
      *(Word<E> *)loc = S + A;
    break;
  case DYNREL:
    apply_dynrel();
    break;
  case IFUNC:
    if constexpr (supports_ifunc<E>) {
      u64 addr = sym.get_addr(ctx, NO_PLT) + A;
      *dynrel++ = ElfRel<E>(P, E::R_IRELATIVE, 0, addr);
      if (ctx.arg.apply_dynamic_relocs)
        *(Word<E> *)loc = addr;
    } else {
      unreachable();
    }
    break;
  default:
    unreachable();
  }
}

template <typename E>
void InputSection<E>::apply_dyn_absrel(Context<E> &ctx, Symbol<E> &sym,
                                       const ElfRel<E> &rel, u8 *loc,
                                       u64 S, i64 A, u64 P,
                                       ElfRel<E> *&dynrel) {
  apply_absrel(ctx, *this, sym, rel, loc, S, A, P, dynrel,
               get_dyn_absrel_action(ctx, sym));
}

template <typename E>
void InputSection<E>::apply_toc_rel(Context<E> &ctx, Symbol<E> &sym,
                                    const ElfRel<E> &rel, u8 *loc,
                                    u64 S, i64 A, u64 P,
                                    ElfRel<E> *&dynrel) {
  apply_absrel(ctx, *this, sym, rel, loc, S, A, P, dynrel,
               get_ppc64_toc_action(ctx, sym));
}

template <typename E>
void InputSection<E>::write_to(Context<E> &ctx, u8 *buf) {
  if (shdr().sh_type == SHT_NOBITS || sh_size == 0)
    return;

  // Copy data
  if constexpr (is_riscv<E>)
    copy_contents_riscv(ctx, buf);
  else
    uncompress_to(ctx, buf);

  // Apply relocations
  if (!ctx.arg.relocatable) {
    if (shdr().sh_flags & SHF_ALLOC)
      apply_reloc_alloc(ctx, buf);
    else
      apply_reloc_nonalloc(ctx, buf);
  }
}

// Get the name of a function containin a given offset.
template <typename E>
std::string_view InputSection<E>::get_func_name(Context<E> &ctx, i64 offset) const {
  for (const ElfSym<E> &esym : file.elf_syms) {
    if (esym.st_shndx == shndx && esym.st_type == STT_FUNC &&
        esym.st_value <= offset && offset < esym.st_value + esym.st_size) {
      std::string_view name = file.symbol_strtab.data() + esym.st_name;
      if (ctx.arg.demangle)
        return demangle(name);
      return name;
    }
  }
  return "";
}

// Test if the symbol a given relocation refers to has already been resolved.
// If not, record that error and returns true.
template <typename E>
bool InputSection<E>::record_undef_error(Context<E> &ctx, const ElfRel<E> &rel) {
  // If a relocation refers to a linker-synthesized symbol for a
  // section fragment, it's always been resolved.
  if (file.elf_syms.size() <= rel.r_sym)
    return false;

  Symbol<E> &sym = *file.symbols[rel.r_sym];
  const ElfSym<E> &esym = file.elf_syms[rel.r_sym];

  // If a symbol is defined in a comdat group, and the comdat group is
  // discarded, the symbol may not have an owner. It is technically an
  // violation of the One Definition Rule, so it is a programmer's fault.
  if (!sym.file) {
    Error(ctx) << *this << ": " << sym << " refers to a discarded COMDAT section"
               << " probably due to an ODR violation";
    return true;
  }

  auto record = [&] {
    std::stringstream ss;
    if (std::string_view source = file.get_source_name(); !source.empty())
      ss << ">>> referenced by " << source << "\n";
    else
      ss << ">>> referenced by " << *this << "\n";

    ss << ">>>               " << file;
    if (std::string_view func = get_func_name(ctx, rel.r_offset); !func.empty())
      ss << ":(" << func << ")";

    typename decltype(ctx.undef_errors)::accessor acc;
    ctx.undef_errors.insert(acc, {sym.name(), {}});
    acc->second.push_back(ss.str());
  };

  // A non-weak undefined symbol must be promoted to an imported
  // symbol or resolved to an defined symbol. Otherwise, it's an
  // undefined symbol error.
  //
  // Every ELF file has an absolute local symbol as its first symbol.
  // Referring to that symbol is always valid.
  bool is_undef = esym.is_undef() && !esym.is_weak() && sym.sym_idx;
  if (!sym.is_imported && is_undef && sym.esym().is_undef()) {
    record();
    return true;
  }

  // If a protected/hidden undefined symbol is resolved to other .so,
  // it's handled as if no symbols were found.
  if (sym.file->is_dso &&
      (sym.visibility == STV_PROTECTED || sym.visibility == STV_HIDDEN)) {
    record();
    return true;
  }

  return false;
}

using E = MOLD_TARGET;

template struct CieRecord<E>;
template class InputSection<E>;

} // namespace mold::elf
