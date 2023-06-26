// clang-format off
// This file implements -r or --relocatable. That option forces the linker
// to combine input object files into another single large object file.
// Since the behavior of the linker when the option is given is quite
// different from that of the normal execution mode, we separate code for
// the feature into this separate file.
//
// The --relocatable option isn't used very often. After all, if you want
// to combine object files into a single file, you could use `ar`.
// However, some programs use it in a creative manner which is hard to be
// substituted with static archives, so we need to support this option in
// the same way as GNU ld does. A notable example is GHC (Glasgow Haskell
// Compiler). GHC has its own dynamic linker which can load a .o file (as
// opposed to a .so) into memory. GHC's module is not a shared object file
// but a combined object file.
//
// There are many different ways to combine object files into a single file.
// The simplest approach would be to just copy all sections from input files
// to an output file as-is with a few exceptions for singleton sections such
// as the symbol table or the string table. That works, but that's not
// compatible with GNU ld.
//
// To be compatible with GNU ld, we need to do the followings:
//
//  - Regular sections containing opaque data (e.g. ".text" or ".data")
//    are just copied as-is. Two sections with the same name are merged.
//
//  - .symtab, .strtab and .shstrtab are merged.
//
//  - COMDAT groups are uniquified.
//
//  - Relocations are copied, but we need to fix symbol indices.

#include "third_party/mold/elf/mold.h"

// MISSING #include <tbb/parallel_for.h>
// MISSING #include <tbb/parallel_for_each.h>

namespace mold::elf {

// Create linker-synthesized sections
template <typename E>
static void r_create_synthetic_sections(Context<E> &ctx) {
  auto push = [&]<typename T>(T *x) {
    ctx.chunks.push_back(x);
    ctx.chunk_pool.emplace_back(x);
    return x;
  };

  ctx.ehdr = push(new OutputEhdr<E>(0));
  ctx.shdr = push(new OutputShdr<E>);
  ctx.eh_frame = push(new EhFrameSection<E>);
  ctx.eh_frame_reloc = push(new EhFrameRelocSection<E>);
  ctx.strtab = push(new StrtabSection<E>);
  ctx.symtab = push(new SymtabSection<E>);
  ctx.shstrtab = push(new ShstrtabSection<E>);
  ctx.note_property = push(new NotePropertySection<E>);
}

// Create SHT_GROUP (i.e. comdat group) sections. We uniquify comdat
// sections by signature. We want to propagate input comdat groups as
// output comdat groups if they are still alive after uniquification.
template <typename E>
static void create_comdat_group_sections(Context<E> &ctx) {
  Timer t(ctx, "create_comdat_group_sections");

  std::vector<std::vector<Chunk<E> *>> vec{ctx.objs.size()};

  tbb::parallel_for((i64)0, (i64)ctx.objs.size(), [&](i64 i) {
    ObjectFile<E> &file = *ctx.objs[i];

    for (ComdatGroupRef<E> &ref : file.comdat_groups) {
      if (ref.group->owner != file.priority)
        continue;

      Symbol<E> *sym = file.symbols[file.elf_sections[ref.sect_idx].sh_info];
      assert(sym);

      std::vector<Chunk<E> *> members;
      for (u32 j : ref.members) {
        const ElfShdr<E> &shdr = file.elf_sections[j];
        if (shdr.sh_type == (E::is_rela ? SHT_RELA : SHT_REL)) {
          InputSection<E> &isec = *file.sections[shdr.sh_info];
          members.push_back(isec.output_section->reloc_sec.get());
        } else {
          InputSection<E> &isec = *file.sections[j];
          members.push_back(isec.output_section);
        }
      }

      vec[i].push_back(new ComdatGroupSection<E>(*sym, std::move(members)));
    }
  });

  for (std::vector<Chunk<E> *> &vec2 : vec) {
    for (Chunk<E> *chunk : vec2) {
      ctx.chunks.push_back(chunk);
      ctx.chunk_pool.emplace_back(chunk);
    }
  }
}

// Unresolved undefined symbols in the -r mode are simply propagated to an
// output file as undefined symbols. This function guarantees that
// unresolved undefined symbols belongs to some input file.
template <typename E>
static void r_claim_unresolved_symbols(Context<E> &ctx) {
  Timer t(ctx, "r_claim_unresolved_symbols");

  tbb::parallel_for_each(ctx.objs, [&](ObjectFile<E> *file) {
    if (!file->is_alive)
      return;

    for (i64 i = file->first_global; i < file->elf_syms.size(); i++) {
      const ElfSym<E> &esym = file->elf_syms[i];
      Symbol<E> &sym = *file->symbols[i];
      if (!esym.is_undef())
        continue;

      std::scoped_lock lock(sym.mu);

      if (sym.file &&
          (!sym.esym().is_undef() || sym.file->priority <= file->priority))
        continue;

      sym.file = file;
      sym.origin = 0;
      sym.value = 0;
      sym.sym_idx = i;
    }
  });
}

// Set output section in-file offsets. Output section memory addresses
// are left as zero.
template <typename E>
static u64 r_set_osec_offsets(Context<E> &ctx) {
  u64 offset = 0;
  for (Chunk<E> *chunk : ctx.chunks) {
    offset = align_to(offset, chunk->shdr.sh_addralign);
    chunk->shdr.sh_offset = offset;
    offset += chunk->shdr.sh_size;
  }
  return offset;
}

template <typename E>
void combine_objects(Context<E> &ctx) {
  compute_merged_section_sizes(ctx);

  create_output_sections(ctx);

  r_create_synthetic_sections(ctx);

  r_claim_unresolved_symbols(ctx);

  compute_section_sizes(ctx);

  sort_output_sections(ctx);

  create_output_symtab(ctx);

  ctx.eh_frame->construct(ctx);

  create_reloc_sections(ctx);

  create_comdat_group_sections(ctx);

  compute_section_headers(ctx);

  i64 filesize = r_set_osec_offsets(ctx);
  ctx.output_file =
    OutputFile<Context<E>>::open(ctx, ctx.arg.output, filesize, 0666);
  ctx.buf = ctx.output_file->buf;

  copy_chunks(ctx);
  clear_padding(ctx);
  ctx.output_file->close(ctx);
  ctx.checkpoint();

  if (ctx.arg.print_map)
    print_map(ctx);

  if (ctx.arg.stats)
    show_stats(ctx);

  if (ctx.arg.perf)
    print_timer_records(ctx.timer_records);

  if (ctx.arg.quick_exit)
    _exit(0);
}

using E = MOLD_TARGET;

template void combine_objects(Context<E> &);

} // namespace mold::elf
