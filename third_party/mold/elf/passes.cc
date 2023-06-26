// clang-format off
#include "third_party/mold/elf/mold.h"

#include "third_party/libcxx/fstream"
#include "third_party/libcxx/functional"
#include "third_party/libcxx/map"
#include "third_party/libcxx/optional"
#include "third_party/libcxx/random"
#include "third_party/libcxx/regex"
// MISSING #include <shared_mutex>
// MISSING #include <tbb/parallel_for_each.h>
// MISSING #include <tbb/parallel_sort.h>
// MISSING #include <tbb/partitioner.h>
#include "third_party/libcxx/unordered_set"

namespace mold::elf {

template <typename E>
void apply_exclude_libs(Context<E> &ctx) {
  Timer t(ctx, "apply_exclude_libs");

  if (ctx.arg.exclude_libs.empty())
    return;

  std::unordered_set<std::string_view> set(ctx.arg.exclude_libs.begin(),
                                           ctx.arg.exclude_libs.end());

  for (ObjectFile<E> *file : ctx.objs) {
    if (!file->archive_name.empty())
      if (set.contains("ALL") ||
          set.contains(filepath(file->archive_name).filename().string()))
        file->exclude_libs = true;
  }
}

template <typename E>
void create_synthetic_sections(Context<E> &ctx) {
  auto push = [&]<typename T>(T *x) {
    ctx.chunks.push_back(x);
    ctx.chunk_pool.emplace_back(x);
    return x;
  };

  if (!ctx.arg.oformat_binary) {
    auto find = [&](std::string_view name) {
      for (SectionOrder &ord : ctx.arg.section_order)
        if (ord.type == SectionOrder::SECTION && ord.name == name)
          return true;
      return false;
    };

    if (ctx.arg.section_order.empty() || find("EHDR"))
      ctx.ehdr = push(new OutputEhdr<E>(SHF_ALLOC));
    else
      ctx.ehdr = push(new OutputEhdr<E>(0));

    if (ctx.arg.section_order.empty() || find("PHDR"))
      ctx.phdr = push(new OutputPhdr<E>(SHF_ALLOC));
    else
      ctx.phdr = push(new OutputPhdr<E>(0));

    ctx.shdr = push(new OutputShdr<E>);
  }

  ctx.got = push(new GotSection<E>);

  if constexpr (!is_sparc<E>)
    ctx.gotplt = push(new GotPltSection<E>);

  ctx.reldyn = push(new RelDynSection<E>);
  ctx.relplt = push(new RelPltSection<E>);

  if (ctx.arg.pack_dyn_relocs_relr)
    ctx.relrdyn = push(new RelrDynSection<E>);

  ctx.strtab = push(new StrtabSection<E>);
  ctx.plt = push(new PltSection<E>);
  ctx.pltgot = push(new PltGotSection<E>);
  ctx.symtab = push(new SymtabSection<E>);
  ctx.dynsym = push(new DynsymSection<E>);
  ctx.dynstr = push(new DynstrSection<E>);
  ctx.eh_frame = push(new EhFrameSection<E>);
  ctx.copyrel = push(new CopyrelSection<E>(false));
  ctx.copyrel_relro = push(new CopyrelSection<E>(true));

  if (!ctx.arg.oformat_binary)
    ctx.shstrtab = push(new ShstrtabSection<E>);

  if (!ctx.arg.dynamic_linker.empty())
    ctx.interp = push(new InterpSection<E>);
  if (ctx.arg.build_id.kind != BuildId::NONE)
    ctx.buildid = push(new BuildIdSection<E>);
  if (ctx.arg.eh_frame_hdr)
    ctx.eh_frame_hdr = push(new EhFrameHdrSection<E>);
  if (ctx.arg.gdb_index)
    ctx.gdb_index = push(new GdbIndexSection<E>);
  if (ctx.arg.z_relro && ctx.arg.section_order.empty() &&
      ctx.arg.z_separate_code != SEPARATE_LOADABLE_SEGMENTS)
    ctx.relro_padding = push(new RelroPaddingSection<E>);
  if (ctx.arg.hash_style_sysv)
    ctx.hash = push(new HashSection<E>);
  if (ctx.arg.hash_style_gnu)
    ctx.gnu_hash = push(new GnuHashSection<E>);
  if (!ctx.arg.version_definitions.empty())
    ctx.verdef = push(new VerdefSection<E>);
  if (ctx.arg.emit_relocs)
    ctx.eh_frame_reloc = push(new EhFrameRelocSection<E>);

  if (ctx.arg.shared || !ctx.dsos.empty() || ctx.arg.pie)
    ctx.dynamic = push(new DynamicSection<E>);

  ctx.versym = push(new VersymSection<E>);
  ctx.verneed = push(new VerneedSection<E>);
  ctx.note_package = push(new NotePackageSection<E>);
  ctx.note_property = push(new NotePropertySection<E>);


  if constexpr (is_ppc64v1<E>)
    ctx.extra.opd = push(new PPC64OpdSection);

  if constexpr (is_sparc<E>) {
    if (ctx.arg.is_static)
      ctx.extra.tls_get_addr_sec = push(new SparcTlsGetAddrSection);
    ctx.extra.tls_get_addr_sym = get_symbol(ctx, "__tls_get_addr");
  }

  if constexpr (is_alpha<E>)
    ctx.extra.got = push(new AlphaGotSection);

  // If .dynamic exists, .dynsym and .dynstr must exist as well
  // since .dynamic refers them.
  if (ctx.dynamic) {
    ctx.dynstr->keep();
    ctx.dynsym->keep();
  }
}

template <typename E>
static void mark_live_objects(Context<E> &ctx) {
  auto mark_symbol = [&](std::string_view name) {
    if (InputFile<E> *file = get_symbol(ctx, name)->file)
      file->is_alive = true;
  };

  for (std::string_view name : ctx.arg.undefined)
    mark_symbol(name);
  for (std::string_view name : ctx.arg.require_defined)
    mark_symbol(name);

  std::vector<InputFile<E> *> roots;

  for (InputFile<E> *file : ctx.objs)
    if (file->is_alive)
      roots.push_back(file);

  for (InputFile<E> *file : ctx.dsos)
    if (file->is_alive)
      roots.push_back(file);

  tbb::parallel_for_each(roots, [&](InputFile<E> *file,
                                    tbb::feeder<InputFile<E> *> &feeder) {
    if (file->is_alive)
      file->mark_live_objects(ctx, [&](InputFile<E> *obj) { feeder.add(obj); });
  });
}

template <typename E>
void do_resolve_symbols(Context<E> &ctx) {
  auto for_each_file = [&](std::function<void(InputFile<E> *)> fn) {
    tbb::parallel_for_each(ctx.objs, fn);
    tbb::parallel_for_each(ctx.dsos, fn);
  };

  // Due to legacy reasons, archive members will only get included in the final
  // binary if they satisfy one of the undefined symbols in a non-archive object
  // file. This is called archive extraction. In finalize_archive_extraction,
  // this is processed as follows:
  //
  // 1. Do preliminary symbol resolution assuming all archive members
  //    are included. This matches the undefined symbols with ones to be
  //    extracted from archives.
  //
  // 2. Do a mark & sweep pass to eliminate unneeded archive members.
  //
  // Note that the symbol resolution inside finalize_archive_extraction uses a
  // different rule. In order to prevent extracting archive members that can be
  // satisfied by either non-archive object files or DSOs, the archive members
  // are given a lower priority. This is not correct for the general case, where
  // *extracted* object files have precedence over DSOs and even non-archive
  // files that are passed earlier in the command line. Hence, the symbol
  // resolution is thrown away once we determine which archive members to
  // extract, and redone later with the formal rule.
  {
    Timer t(ctx, "extract_archive_members");

    // Register symbols
    for_each_file([&](InputFile<E> *file) { file->resolve_symbols(ctx); });

    // Mark reachable objects to decide which files to include into an output.
    // This also merges symbol visibility.
    mark_live_objects(ctx);

    // Cleanup. The rule used for archive extraction isn't accurate for the
    // general case of symbol extraction, so reset the resolution to be redone
    // later.
    for_each_file([](InputFile<E> *file) { file->clear_symbols(); });

    // Now that the symbol references are gone, remove the eliminated files from
    // the file list.
    std::erase_if(ctx.objs, [](InputFile<E> *file) { return !file->is_alive; });
    std::erase_if(ctx.dsos, [](InputFile<E> *file) { return !file->is_alive; });
  }

  // COMDAT elimination needs to happen exactly here.
  //
  // It needs to be after archive extraction, otherwise we might assign COMDAT
  // leader to an archive member that is not supposed to be extracted.
  //
  // It needs to happen before symbol resolution, otherwise we could eliminate
  // a symbol that is already resolved to and cause dangling references.
  {
    Timer t(ctx, "eliminate_comdats");

    tbb::parallel_for_each(ctx.objs, [](ObjectFile<E> *file) {
      for (ComdatGroupRef<E> &ref : file->comdat_groups)
        update_minimum(ref.group->owner, file->priority);
    });

    tbb::parallel_for_each(ctx.objs, [](ObjectFile<E> *file) {
      for (ComdatGroupRef<E> &ref : file->comdat_groups)
        if (ref.group->owner != file->priority)
          for (u32 i : ref.members)
            if (file->sections[i])
              file->sections[i]->kill();
    });
  }

  // Since we have turned on object files live bits, their symbols
  // may now have higher priority than before. So run the symbol
  // resolution pass again to get the final resolution result.
  for_each_file([&](InputFile<E> *file) { file->resolve_symbols(ctx); });
}

template <typename E>
void resolve_symbols(Context<E> &ctx) {
  Timer t(ctx, "resolve_symbols");

  std::vector<ObjectFile<E> *> objs = ctx.objs;
  std::vector<SharedFile<E> *> dsos = ctx.dsos;

  do_resolve_symbols(ctx);

  if (ctx.has_lto_object) {
    // Do link-time optimization. We pass all IR object files to the
    // compiler backend to compile them into a few ELF object files.
    //
    // The compiler backend needs to know how symbols are resolved,
    // so compute symbol visibility, import/export bits, etc early.
    mark_live_objects(ctx);
    apply_version_script(ctx);
    parse_symbol_version(ctx);
    compute_import_export(ctx);

    // Do LTO. It compiles IR object files into a few big ELF files.
    std::vector<ObjectFile<E> *> lto_objs = do_lto(ctx);

    // do_resolve_symbols() have removed unreferenced files. Restore the
    // original files here because some of them may have to be resurrected
    // because they are referenced by the ELF files returned from do_lto().
    ctx.objs = objs;
    ctx.dsos = dsos;

    append(ctx.objs, lto_objs);

    // Redo name resolution from scratch.
    tbb::parallel_for_each(ctx.objs, [&](ObjectFile<E> *file) {
      file->clear_symbols();
    });

    tbb::parallel_for_each(ctx.dsos, [&](SharedFile<E> *file) {
      file->clear_symbols();
    });

    // Remove IR object files.
    for (ObjectFile<E> *file : ctx.objs)
      if (file->is_lto_obj)
        file->is_alive = false;

    std::erase_if(ctx.objs, [](ObjectFile<E> *file) { return file->is_lto_obj; });

    do_resolve_symbols(ctx);
  }
}

// .eh_frame sections are parsed and regenerated by the linker for the purpose
// of deduplication and garbage collection. As such, the input sections should
// not be copied over.
// However, in very rare cases (e.g. GCC CRT compiled with LTO) we might need
// to resolve cross-object .eh_frame section references (they only point to
// begin or end and don't depend on the actual section contents).
// Therefore, the sections are "killed" after symbol resolution as a separate
// pass.
template <typename E>
void kill_eh_frame_sections(Context<E> &ctx) {
  Timer t(ctx, "kill_eh_frame_sections");

  tbb::parallel_for_each(ctx.objs, [&](ObjectFile<E> *file) {
    for (i64 i = 0; i < file->sections.size(); i++) {
      if (std::unique_ptr<InputSection<E>> &isec = file->sections[i]) {
        if (isec && isec->is_alive && isec->name() == ".eh_frame") {
          isec->is_alive = false;
        }
      }
    }
  });
}

template <typename E>
void resolve_section_pieces(Context<E> &ctx) {
  Timer t(ctx, "resolve_section_pieces");

  tbb::parallel_for_each(ctx.objs, [&](ObjectFile<E> *file) {
    file->initialize_mergeable_sections(ctx);
  });

  tbb::parallel_for_each(ctx.objs, [&](ObjectFile<E> *file) {
    file->resolve_section_pieces(ctx);
  });
}

template <typename E>
void convert_common_symbols(Context<E> &ctx) {
  Timer t(ctx, "convert_common_symbols");

  tbb::parallel_for_each(ctx.objs, [&](ObjectFile<E> *file) {
    file->convert_common_symbols(ctx);
  });
}

template <typename E>
static std::string get_cmdline_args(Context<E> &ctx) {
  std::stringstream ss;
  ss << ctx.cmdline_args[1];
  for (i64 i = 2; i < ctx.cmdline_args.size(); i++)
    ss << " " << ctx.cmdline_args[i];
  return ss.str();
}

template <typename E>
void add_comment_string(Context<E> &ctx, std::string str) {
  MergedSection<E> *sec =
    MergedSection<E>::get_instance(ctx, ".comment", SHT_PROGBITS,
                                   SHF_MERGE | SHF_STRINGS);

  std::string_view buf = save_string(ctx, str);
  std::string_view data(buf.data(), buf.size() + 1);
  sec->insert(ctx, data, hash_string(data), 0);
}

template <typename E>
void compute_merged_section_sizes(Context<E> &ctx) {
  Timer t(ctx, "compute_merged_section_sizes");

  // Add an identification string to .comment.
  if (!ctx.arg.oformat_binary)
    add_comment_string(ctx, mold_version);

  // Embed command line arguments for debugging.
  if (char *env = getenv("MOLD_DEBUG"); env && env[0])
    add_comment_string(ctx, "mold command line: " + get_cmdline_args(ctx));

  tbb::parallel_for_each(ctx.merged_sections,
                         [&](std::unique_ptr<MergedSection<E>> &sec) {
    sec->assign_offsets(ctx);
  });
}

template <typename T>
static std::vector<std::span<T>> split(std::vector<T> &input, i64 unit) {
  std::span<T> span(input);
  std::vector<std::span<T>> vec;

  while (span.size() >= unit) {
    vec.push_back(span.subspan(0, unit));
    span = span.subspan(unit);
  }
  if (!span.empty())
    vec.push_back(span);
  return vec;
}

template <typename E>
static u64 canonicalize_type(std::string_view name, u64 type) {
  if (type == SHT_PROGBITS) {
    if (name == ".init_array" || name.starts_with(".init_array."))
      return SHT_INIT_ARRAY;
    if (name == ".fini_array" || name.starts_with(".fini_array."))
      return SHT_FINI_ARRAY;
  }

  if constexpr (is_x86_64<E>)
    if (type == SHT_X86_64_UNWIND)
      return SHT_PROGBITS;
  return type;
}

struct OutputSectionKey {
  std::string_view name;
  u64 type;
  u64 flags;

  bool operator==(const OutputSectionKey &other) const {
    return name == other.name && type == other.type && flags == other.flags;
  }
};

template <typename E>
std::string_view
get_output_name(Context<E> &ctx, std::string_view name, u64 flags) {
  if (ctx.arg.relocatable && !ctx.arg.relocatable_merge_sections)
    return name;
  if (ctx.arg.unique && ctx.arg.unique->match(name))
    return name;
  if (flags & SHF_MERGE)
    return name;

  if constexpr (is_arm32<E>) {
    if (name.starts_with(".ARM.exidx"))
      return ".ARM.exidx";
    if (name.starts_with(".ARM.extab"))
      return ".ARM.extab";
  }

  if constexpr (is_alpha<E>) {
    if (name.starts_with(".sdata."))
      return ".sdata";
    if (name.starts_with(".sbss."))
      return ".sbss";
  }

  if (ctx.arg.z_keep_text_section_prefix) {
    static std::string_view prefixes[] = {
      ".text.hot.", ".text.unknown.", ".text.unlikely.", ".text.startup.",
      ".text.exit."
    };

    for (std::string_view prefix : prefixes) {
      std::string_view stem = prefix.substr(0, prefix.size() - 1);
      if (name == stem || name.starts_with(prefix))
        return stem;
    }
  }

  static std::string_view prefixes[] = {
    ".text.", ".data.rel.ro.", ".data.", ".rodata.", ".bss.rel.ro.", ".bss.",
    ".init_array.", ".fini_array.", ".tbss.", ".tdata.", ".gcc_except_table.",
    ".ctors.", ".dtors.", ".gnu.warning.", ".openbsd.randomdata.",
  };

  for (std::string_view prefix : prefixes) {
    std::string_view stem = prefix.substr(0, prefix.size() - 1);
    if (name == stem || name.starts_with(prefix))
      return stem;
  }

  return name;
}

template <typename E>
static OutputSectionKey
get_output_section_key(Context<E> &ctx, InputSection<E> &isec) {
  const ElfShdr<E> &shdr = isec.shdr();
  std::string_view name = get_output_name(ctx, isec.name(), shdr.sh_flags);
  u64 type = canonicalize_type<E>(name, shdr.sh_type);
  u64 flags = shdr.sh_flags & ~(u64)SHF_COMPRESSED;

  if (!ctx.arg.relocatable)
    flags &= ~(u64)SHF_GROUP & ~(u64)SHF_GNU_RETAIN;

  // .init_array is usually writable. We don't want to create multiple
  // .init_array output sections, so make it always writable.
  // So is .fini_array.
  if (type == SHT_INIT_ARRAY || type == SHT_FINI_ARRAY)
    flags |= SHF_WRITE;
  return {name, type, flags};
}

// Create output sections for input sections.
template <typename E>
void create_output_sections(Context<E> &ctx) {
  Timer t(ctx, "create_output_sections");

  struct Hash {
    size_t operator()(const OutputSectionKey &k) const {
      u64 h = hash_string(k.name);
      h = combine_hash(h, std::hash<u64>{}(k.type));
      h = combine_hash(h, std::hash<u64>{}(k.flags));
      return h;
    }
  };

  std::unordered_map<OutputSectionKey, OutputSection<E> *, Hash> map;
  std::shared_mutex mu;

  // Instantiate output sections
  tbb::parallel_for_each(ctx.objs, [&](ObjectFile<E> *file) {
    // Make a per-thread cache of the main map to avoid lock contention.
    // It makes a noticeable difference if we have millions of input sections.
    decltype(map) cache;
    {
      std::shared_lock lock(mu);
      cache = map;
    }

    for (std::unique_ptr<InputSection<E>> &isec : file->sections) {
      if (!isec || !isec->is_alive)
        continue;

      OutputSectionKey key = get_output_section_key(ctx, *isec);

      if (auto it = cache.find(key); it != cache.end()) {
        isec->output_section = it->second;
        continue;
      }

      auto get_or_insert = [&] {
        {
          std::shared_lock lock(mu);
          if (auto it = map.find(key); it != map.end())
            return it->second;
        }

        std::unique_ptr<OutputSection<E>> osec =
          std::make_unique<OutputSection<E>>(key.name, key.type, key.flags);

        std::unique_lock lock(mu);
        auto [it, inserted] = map.insert({key, osec.get()});
        OutputSection<E> *ret = it->second;
        lock.unlock();

        if (inserted)
          ctx.osec_pool.emplace_back(std::move(osec));
        return ret;
      };

      OutputSection<E> *osec = get_or_insert();
      isec->output_section = osec;
      cache.insert({key, osec});
    }
  });

  // Add input sections to output sections
  for (ObjectFile<E> *file : ctx.objs)
    for (std::unique_ptr<InputSection<E>> &isec : file->sections)
      if (isec && isec->is_alive)
        isec->output_section->members.push_back(isec.get());

  // Add output sections and mergeable sections to ctx.chunks
  std::vector<Chunk<E> *> vec;
  for (std::pair<const OutputSectionKey, OutputSection<E> *> &kv : map)
    vec.push_back(kv.second);

  for (std::unique_ptr<MergedSection<E>> &osec : ctx.merged_sections)
    if (osec->shdr.sh_size)
      vec.push_back(osec.get());

  // Sections are added to the section lists in an arbitrary order
  // because they are created in parallel. Sort them to to make the
  // output deterministic.
  tbb::parallel_sort(vec.begin(), vec.end(), [](Chunk<E> *x, Chunk<E> *y) {
    return std::tuple(x->name, x->shdr.sh_type, x->shdr.sh_flags) <
           std::tuple(y->name, y->shdr.sh_type, y->shdr.sh_flags);
  });

  append(ctx.chunks, vec);
}

// Create a dummy object file containing linker-synthesized
// symbols.
template <typename E>
void create_internal_file(Context<E> &ctx) {
  ObjectFile<E> *obj = new ObjectFile<E>;
  ctx.obj_pool.emplace_back(obj);
  ctx.internal_obj = obj;
  ctx.objs.push_back(obj);

  // Create linker-synthesized symbols.
  ctx.internal_esyms.resize(1);

  obj->symbols.push_back(new Symbol<E>);
  obj->first_global = 1;
  obj->is_alive = true;
  obj->priority = 1;

  auto add = [&](Symbol<E> *sym) {
    obj->symbols.push_back(sym);

    // An actual value will be set to a linker-synthesized symbol by
    // fix_synthetic_symbols(). Until then, `value` doesn't have a valid
    // value. 0xdeadbeef is a unique dummy value to make debugging easier
    // if the field is accidentally used before it gets a valid one.
    sym->value = 0xdeadbeef;

    ElfSym<E> esym;
    memset(&esym, 0, sizeof(esym));
    esym.st_type = STT_NOTYPE;
    esym.st_shndx = SHN_ABS;
    esym.st_bind = STB_GLOBAL;
    esym.st_visibility = STV_DEFAULT;
    ctx.internal_esyms.push_back(esym);
  };

  auto add_undef = [&](Symbol<E> *sym) {
    obj->symbols.push_back(sym);
    sym->value = 0xdeadbeef;

    ElfSym<E> esym;
    memset(&esym, 0, sizeof(esym));
    esym.st_type = STT_NOTYPE;
    esym.st_shndx = SHN_UNDEF;
    esym.st_bind = STB_GLOBAL;
    esym.st_visibility = STV_DEFAULT;
    ctx.internal_esyms.push_back(esym);
  };

  // Add --defsym symbols
  for (i64 i = 0; i < ctx.arg.defsyms.size(); i++) {
    Symbol<E> *sym = ctx.arg.defsyms[i].first;
    std::variant<Symbol<E> *, u64> val = ctx.arg.defsyms[i].second;
    add(sym);

    if (Symbol<E> **ref = std::get_if<Symbol<E> *>(&val)) {
      // Add an undefined symbol to keep a reference to the defsym target.
      // This prevents elimination by e.g. LTO or gc-sections.
      // The undefined symbol will never make to the final object file; we
      // double-check that the defsym target is not undefined in
      // fix_synthetic_symbols.
      add_undef(*ref);
    }
  }

  // Add --section-order symbols
  for (SectionOrder &ord : ctx.arg.section_order)
    if (ord.type == SectionOrder::SYMBOL)
      add(get_symbol(ctx, ord.name));

  obj->elf_syms = ctx.internal_esyms;
  obj->has_symver.resize(ctx.internal_esyms.size() - 1);
}

template <typename E>
static std::optional<std::string>
get_start_stop_name(Context<E> &ctx, Chunk<E> &chunk) {
  if ((chunk.shdr.sh_flags & SHF_ALLOC) && !chunk.name.empty()) {
    if (is_c_identifier(chunk.name))
      return std::string(chunk.name);

    if (ctx.arg.start_stop) {
      auto isalnum = [](char c) {
        return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') ||
               ('0' <= c && c <= '9');
      };

      std::string s{chunk.name};
      if (s.starts_with('.'))
        s = s.substr(1);

      for (i64 i = 0; i < s.size(); i++)
        if (!isalnum(s[i]))
          s[i] = '_';
      return s;
    }
  }

  return {};
}

template <typename E>
void add_synthetic_symbols(Context<E> &ctx) {
  ObjectFile<E> &obj = *ctx.internal_obj;

  auto add = [&](std::string_view name) {
    ElfSym<E> esym;
    memset(&esym, 0, sizeof(esym));
    esym.st_type = STT_NOTYPE;
    esym.st_shndx = SHN_ABS;
    esym.st_bind = STB_GLOBAL;
    esym.st_visibility = STV_HIDDEN;
    ctx.internal_esyms.push_back(esym);

    Symbol<E> *sym = get_symbol(ctx, name);
    sym->value = 0xdeadbeef; // unique dummy value
    obj.symbols.push_back(sym);
    return sym;
  };

  ctx.__ehdr_start = add("__ehdr_start");
  ctx.__init_array_start = add("__init_array_start");
  ctx.__init_array_end = add("__init_array_end");
  ctx.__fini_array_start = add("__fini_array_start");
  ctx.__fini_array_end = add("__fini_array_end");
  ctx.__preinit_array_start = add("__preinit_array_start");
  ctx.__preinit_array_end = add("__preinit_array_end");
  ctx._DYNAMIC = add("_DYNAMIC");
  ctx._GLOBAL_OFFSET_TABLE_ = add("_GLOBAL_OFFSET_TABLE_");
  ctx._PROCEDURE_LINKAGE_TABLE_ = add("_PROCEDURE_LINKAGE_TABLE_");
  ctx.__bss_start = add("__bss_start");
  ctx._end = add("_end");
  ctx._etext = add("_etext");
  ctx._edata = add("_edata");
  ctx.__executable_start = add("__executable_start");

  ctx.__rel_iplt_start =
    add(E::is_rela ? "__rela_iplt_start" : "__rel_iplt_start");
  ctx.__rel_iplt_end =
    add(E::is_rela ? "__rela_iplt_end" : "__rel_iplt_end");

  if (ctx.arg.eh_frame_hdr)
    ctx.__GNU_EH_FRAME_HDR = add("__GNU_EH_FRAME_HDR");

  if (!get_symbol(ctx, "end")->file)
    ctx.end = add("end");
  if (!get_symbol(ctx, "etext")->file)
    ctx.etext = add("etext");
  if (!get_symbol(ctx, "edata")->file)
    ctx.edata = add("edata");
  if (!get_symbol(ctx, "__dso_handle")->file)
    ctx.__dso_handle = add("__dso_handle");

  if constexpr (supports_tlsdesc<E>)
    ctx._TLS_MODULE_BASE_ = add("_TLS_MODULE_BASE_");

  if constexpr (is_riscv<E>)
    if (!ctx.arg.shared)
      ctx.__global_pointer = add("__global_pointer$");

  if constexpr (is_arm32<E>) {
    ctx.__exidx_start = add("__exidx_start");
    ctx.__exidx_end = add("__exidx_end");
  }

  if constexpr (is_ppc64<E>)
    ctx.extra.TOC = add(".TOC.");

  if constexpr (is_ppc32<E>)
    ctx.extra._SDA_BASE_ = add("_SDA_BASE_");

  for (Chunk<E> *chunk : ctx.chunks) {
    if (std::optional<std::string> name = get_start_stop_name(ctx, *chunk)) {
      add(save_string(ctx, "__start_" + *name));
      add(save_string(ctx, "__stop_" + *name));

      if (ctx.arg.physical_image_base) {
        add(save_string(ctx, "__phys_start_" + *name));
        add(save_string(ctx, "__phys_stop_" + *name));
      }
    }
  }

  obj.elf_syms = ctx.internal_esyms;
  obj.has_symver.resize(ctx.internal_esyms.size() - 1);

  obj.resolve_symbols(ctx);

  // Make all synthetic symbols relative ones by associating them to
  // a dummy output section.
  for (Symbol<E> *sym : obj.symbols)
    if (sym->file == &obj)
      sym->set_output_section(ctx.symtab);

  // Handle --defsym symbols.
  for (i64 i = 0; i < ctx.arg.defsyms.size(); i++) {
    Symbol<E> *sym = ctx.arg.defsyms[i].first;
    std::variant<Symbol<E> *, u64> val = ctx.arg.defsyms[i].second;

    Symbol<E> *target = nullptr;
    if (Symbol<E> **ref = std::get_if<Symbol<E> *>(&val))
      target = *ref;

    // If the alias refers another symobl, copy ELF symbol attributes.
    if (target) {
      ElfSym<E> &esym = obj.elf_syms[i + 1];
      esym.st_type = target->esym().st_type;
      if constexpr (is_ppc64v2<E>)
        esym.ppc_local_entry = target->esym().ppc_local_entry;
    }

    // Make the target absolute if necessary.
    if (!target || target->is_absolute())
      sym->origin = 0;
  }

}

template <typename E>
void check_cet_errors(Context<E> &ctx) {
  bool warning = (ctx.arg.z_cet_report == CET_REPORT_WARNING);
  assert(warning || ctx.arg.z_cet_report == CET_REPORT_ERROR);

  auto has_feature = [](ObjectFile<E> *file, u32 feature) {
    return std::any_of(file->gnu_properties.begin(), file->gnu_properties.end(),
                       [&](std::pair<u32, u32> kv) {
      return kv.first == GNU_PROPERTY_X86_FEATURE_1_AND &&
             (kv.second & feature);
    });
  };

  for (ObjectFile<E> *file : ctx.objs) {
    if (file == ctx.internal_obj)
      continue;

    if (!has_feature(file, GNU_PROPERTY_X86_FEATURE_1_IBT)) {
      if (warning)
        Warn(ctx) << *file << ": -cet-report=warning: "
                  << "missing GNU_PROPERTY_X86_FEATURE_1_IBT";
      else
        Error(ctx) << *file << ": -cet-report=error: "
                   << "missing GNU_PROPERTY_X86_FEATURE_1_IBT";
    }

    if (!has_feature(file, GNU_PROPERTY_X86_FEATURE_1_SHSTK)) {
      if (warning)
        Warn(ctx) << *file << ": -cet-report=warning: "
                  << "missing GNU_PROPERTY_X86_FEATURE_1_SHSTK";
      else
        Error(ctx) << *file << ": -cet-report=error: "
                   << "missing GNU_PROPERTY_X86_FEATURE_1_SHSTK";
    }
  }
}

template <typename E>
void print_dependencies(Context<E> &ctx) {
  SyncOut(ctx) <<
R"(# This is an output of the mold linker's --print-dependencies option.
#
# Each line consists of 4 fields, <section1>, <section2>, <symbol-type> and
# <symbol>, separated by tab characters. It indicates that <section1> depends
# on <section2> to use <symbol>. <symbol-type> is either "u" or "w" for
# regular undefined or weak undefined, respectively.
#
# If you want to obtain dependency information per function granularity,
# compile source files with the -ffunction-sections compiler flag.
)";

  auto println = [&](auto &src, Symbol<E> &sym, ElfSym<E> &esym) {
    if (InputSection<E> *isec = sym.get_input_section())
      SyncOut(ctx) << src << "\t" << *isec
                   << "\t" << (esym.is_weak() ? 'w' : 'u')
                   << "\t" << sym;
    else
      SyncOut(ctx) << src << "\t" << *sym.file
                   << "\t" << (esym.is_weak() ? 'w' : 'u')
                   << "\t" << sym;
  };

  for (ObjectFile<E> *file : ctx.objs) {
    for (std::unique_ptr<InputSection<E>> &isec : file->sections) {
      if (!isec)
        continue;

      std::unordered_set<void *> visited;

      for (const ElfRel<E> &r : isec->get_rels(ctx)) {
        if (r.r_type == R_NONE)
          continue;

        ElfSym<E> &esym = file->elf_syms[r.r_sym];
        Symbol<E> &sym = *file->symbols[r.r_sym];

        if (esym.is_undef() && sym.file && sym.file != file &&
            visited.insert((void *)&sym).second)
          println(*isec, sym, esym);
      }
    }
  }

  for (SharedFile<E> *file : ctx.dsos) {
    for (i64 i = file->first_global; i < file->symbols.size(); i++) {
      ElfSym<E> &esym = file->elf_syms[i];
      Symbol<E> &sym = *file->symbols[i];
      if (esym.is_undef() && sym.file && sym.file != file)
        println(*file, sym, esym);
    }
  }
}

template <typename E>
static std::string create_response_file(Context<E> &ctx) {
  std::string buf;
  std::stringstream out;

  std::string cwd = std::filesystem::current_path().string();
  out << "-C " << cwd.substr(1) << "\n";

  if (cwd != "/") {
    out << "--chroot ..";
    i64 depth = std::count(cwd.begin(), cwd.end(), '/');
    for (i64 i = 1; i < depth; i++)
      out << "/..";
    out << "\n";
  }

  for (i64 i = 1; i < ctx.cmdline_args.size(); i++) {
    std::string_view arg = ctx.cmdline_args[i];
    if (arg != "-repro" && arg != "--repro")
      out << arg << "\n";
  }
  return out.str();
}

template <typename E>
void write_repro_file(Context<E> &ctx) {
  std::string path = ctx.arg.output + ".repro.tar";

  std::unique_ptr<TarWriter> tar =
    TarWriter::open(path, filepath(ctx.arg.output).filename().string() + ".repro");
  if (!tar)
    Fatal(ctx) << "cannot open " << path << ": " << errno_string();

  tar->append("response.txt", save_string(ctx, create_response_file(ctx)));
  tar->append("version.txt", save_string(ctx, mold_version + "\n"));

  std::unordered_set<std::string> seen;
  for (std::unique_ptr<MappedFile<Context<E>>> &mf : ctx.mf_pool) {
    if (!mf->parent) {
      std::string path = to_abs_path(mf->name).string();
      if (seen.insert(path).second) {
        // We reopen a file because we may have modified the contents of mf
        // in memory, which is mapped with PROT_WRITE and MAP_PRIVATE.
        MappedFile<Context<E>> *mf2 = MappedFile<Context<E>>::must_open(ctx, path);
        tar->append(path, mf2->get_contents());
        mf2->unmap();
      }
    }
  }
}

template <typename E>
void check_duplicate_symbols(Context<E> &ctx) {
  Timer t(ctx, "check_duplicate_symbols");

  tbb::parallel_for_each(ctx.objs, [&](ObjectFile<E> *file) {
    for (i64 i = file->first_global; i < file->elf_syms.size(); i++) {
      const ElfSym<E> &esym = file->elf_syms[i];
      Symbol<E> &sym = *file->symbols[i];

      // Skip if our symbol is undef or weak
      if (sym.file == file || sym.file == ctx.internal_obj ||
          esym.is_undef() || esym.is_common() || (esym.st_bind == STB_WEAK))
        continue;

      // Skip if our symbol is in a dead section. In most cases, the
      // section has been eliminated due to comdat deduplication.
      if (!esym.is_abs()) {
        InputSection<E> *isec = file->get_section(esym);
        if (!isec || !isec->is_alive)
          continue;
      }

      Error(ctx) << "duplicate symbol: " << *file << ": " << *sym.file
                 << ": " << sym;
    }
  });

  ctx.checkpoint();
}

template <typename E>
void check_symbol_types(Context<E> &ctx) {
  Timer t(ctx, "check_symbol_types");

  auto check = [&](InputFile<E> *file) {
    for (i64 i = file->first_global; i < file->elf_syms.size(); i++) {
      const ElfSym<E> &esym = file->elf_syms[i];
      Symbol<E> &sym = *file->symbols[i];

      if (!sym.file)
        continue;

      u32 x = sym.esym().st_type;
      if (x == STT_GNU_IFUNC)
        x = STT_FUNC;

      u32 y = esym.st_type;
      if (y == STT_GNU_IFUNC)
        y = STT_FUNC;

      if (x != STT_NOTYPE && y != STT_NOTYPE && x != y)
        Warn(ctx) << "symbol type mismatch: " << sym << '\n'
                  << ">>> defined in " << *sym.file << " as "
                  << stt_to_string<E>(sym.esym().st_type) << '\n'
                  << ">>> defined in " << *file << " as "
                  << stt_to_string<E>(esym.st_type);
    }
  };

  tbb::parallel_for_each(ctx.objs, check);
  tbb::parallel_for_each(ctx.dsos, check);
}

template <typename E>
void sort_init_fini(Context<E> &ctx) {
  Timer t(ctx, "sort_init_fini");

  auto get_priority = [](InputSection<E> *isec) {
    static std::regex re(R"(\.(\d+)$)", std::regex_constants::optimize);
    std::string_view name = isec->name();
    std::cmatch m;
    if (std::regex_search(name.data(), name.data() + name.size(), m, re))
      return std::stoi(m[1]);
    return 65536;
  };

  for (Chunk<E> *chunk : ctx.chunks) {
    if (OutputSection<E> *osec = chunk->to_osec()) {
      if (osec->name == ".init_array" || osec->name == ".preinit_array" ||
          osec->name == ".fini_array") {
        if (ctx.arg.shuffle_sections == SHUFFLE_SECTIONS_REVERSE)
          std::reverse(osec->members.begin(), osec->members.end());

        std::unordered_map<InputSection<E> *, i64> map;
        for (InputSection<E> *isec : osec->members)
          map.insert({isec, get_priority(isec)});

        sort(osec->members, [&](InputSection<E> *a, InputSection<E> *b) {
          return map[a] < map[b];
        });
      }
    }
  }
}

template <typename E>
void sort_ctor_dtor(Context<E> &ctx) {
  Timer t(ctx, "sort_ctor_dtor");

  auto get_priority = [](InputSection<E> *isec) {
    auto opts = std::regex_constants::optimize | std::regex_constants::ECMAScript;
    static std::regex re1(R"((?:clang_rt\.)?crtbegin)", opts);
    static std::regex re2(R"((?:clang_rt\.)?crtend)", opts);
    static std::regex re3(R"(\.(\d+)$)", opts);

    // crtbegin.o and crtend.o contain marker symbols such as
    // __CTOR_LIST__ or __DTOR_LIST__. So they have to be at the
    // beginning or end of the section.
    std::smatch m;
    if (std::regex_search(isec->file.filename, m, re1))
      return -2;
    if (std::regex_search(isec->file.filename, m, re2))
      return 65536;

    std::string name(isec->name());
    if (std::regex_search(name, m, re3))
      return std::stoi(m[1]);
    return -1;
  };

  for (Chunk<E> *chunk : ctx.chunks) {
    if (OutputSection<E> *osec = chunk->to_osec()) {
      if (osec->name == ".ctors" || osec->name == ".dtors") {
        if (ctx.arg.shuffle_sections != SHUFFLE_SECTIONS_REVERSE)
          std::reverse(osec->members.begin(), osec->members.end());

        std::unordered_map<InputSection<E> *, i64> map;
        for (InputSection<E> *isec : osec->members)
          map.insert({isec, get_priority(isec)});

        sort(osec->members, [&](InputSection<E> *a, InputSection<E> *b) {
          return map[a] < map[b];
        });
      }
    }
  }
}

template <typename T>
static void shuffle(std::vector<T> &vec, u64 seed) {
  if (vec.empty())
    return;

  // Xorshift random number generator. We use this RNG because it is
  // measurably faster than MT19937.
  auto rand = [&] {
    seed ^= seed << 13;
    seed ^= seed >> 7;
    seed ^= seed << 17;
    return seed;
  };

  // The Fisher-Yates shuffling algorithm.
  //
  // We don't want to use std::shuffle for build reproducibility. That is,
  // std::shuffle's implementation is not guaranteed to be the same across
  // platform, so even though the result is guaranteed to be randomly
  // shuffled, the exact order may be different across implementations.
  //
  // We are not using std::uniform_int_distribution for the same reason.
  for (i64 i = 0; i < vec.size() - 1; i++)
    std::swap(vec[i], vec[i + rand() % (vec.size() - i)]);
}

template <typename E>
void shuffle_sections(Context<E> &ctx) {
  Timer t(ctx, "shuffle_sections");

  auto is_eligible = [](OutputSection<E> &osec) {
    return osec.name != ".init" && osec.name != ".fini" &&
           osec.name != ".ctors" && osec.name != ".dtors" &&
           osec.name != ".init_array" && osec.name != ".preinit_array" &&
           osec.name != ".fini_array";
  };

  switch (ctx.arg.shuffle_sections) {
  case SHUFFLE_SECTIONS_NONE:
    unreachable();
  case SHUFFLE_SECTIONS_SHUFFLE: {
    u64 seed;
    if (ctx.arg.shuffle_sections_seed)
      seed = *ctx.arg.shuffle_sections_seed;
    else
      seed = ((u64)std::random_device()() << 32) | std::random_device()();

    tbb::parallel_for_each(ctx.chunks, [&](Chunk<E> *chunk) {
      if (OutputSection<E> *osec = chunk->to_osec())
        if (is_eligible(*osec))
          shuffle(osec->members, seed + hash_string(osec->name));
    });
    break;
  }
  case SHUFFLE_SECTIONS_REVERSE:
    tbb::parallel_for_each(ctx.chunks, [&](Chunk<E> *chunk) {
      if (OutputSection<E> *osec = chunk->to_osec())
        if (is_eligible(*osec))
          std::reverse(osec->members.begin(), osec->members.end());
    });
    break;
  }
}

template <typename E>
void compute_section_sizes(Context<E> &ctx) {
  Timer t(ctx, "compute_section_sizes");

  struct Group {
    i64 size = 0;
    i64 p2align = 0;
    i64 offset = 0;
    std::span<InputSection<E> *> members;
  };

  tbb::parallel_for_each(ctx.chunks, [&](Chunk<E> *chunk) {
    OutputSection<E> *osec = chunk->to_osec();
    if (!osec)
      return;

    // This pattern will be processed in the next loop.
    if constexpr (needs_thunk<E>)
      if ((osec->shdr.sh_flags & SHF_EXECINSTR) && !ctx.arg.relocatable)
        return;

    // Since one output section may contain millions of input sections,
    // we first split input sections into groups and assign offsets to
    // groups.
    std::vector<Group> groups;
    constexpr i64 group_size = 10000;

    for (std::span<InputSection<E> *> span : split(osec->members, group_size))
      groups.push_back(Group{.members = span});

    tbb::parallel_for_each(groups, [](Group &group) {
      for (InputSection<E> *isec : group.members) {
        group.size = align_to(group.size, 1 << isec->p2align) + isec->sh_size;
        group.p2align = std::max<i64>(group.p2align, isec->p2align);
      }
    });

    i64 offset = 0;
    i64 p2align = 0;

    for (i64 i = 0; i < groups.size(); i++) {
      offset = align_to(offset, 1 << groups[i].p2align);
      groups[i].offset = offset;
      offset += groups[i].size;
      p2align = std::max(p2align, groups[i].p2align);
    }

    osec->shdr.sh_size = offset;
    osec->shdr.sh_addralign = 1 << p2align;

    // Assign offsets to input sections.
    tbb::parallel_for_each(groups, [](Group &group) {
      i64 offset = group.offset;
      for (InputSection<E> *isec : group.members) {
        offset = align_to(offset, 1 << isec->p2align);
        isec->offset = offset;
        offset += isec->sh_size;
      }
    });
  });

  // On ARM32 or ARM64, we may need to create so-called "range extension
  // thunks" to extend branch instructions reach, as they can jump only
  // to ±16 MiB or ±128 MiB, respecitvely.
  //
  // In the following loop, We compute the sizes of sections while
  // inserting thunks. This pass cannot be parallelized. That is,
  // create_range_extension_thunks is parallelized internally, but the
  // function itself is not thread-safe.
  if constexpr (needs_thunk<E>) {
    for (Chunk<E> *chunk : ctx.chunks) {
      OutputSection<E> *osec = chunk->to_osec();
      if (osec && (osec->shdr.sh_flags & SHF_EXECINSTR) && !ctx.arg.relocatable) {
        create_range_extension_thunks(ctx, *osec);

        for (InputSection<E> *isec : osec->members)
          osec->shdr.sh_addralign =
            std::max<u32>(osec->shdr.sh_addralign, 1 << isec->p2align);
      }
    }
  }

  for (Chunk<E> *chunk : ctx.chunks)
    if (OutputSection<E> *osec = chunk->to_osec())
      if (u32 align = ctx.arg.section_align[osec->name])
        osec->shdr.sh_addralign = std::max<u32>(osec->shdr.sh_addralign, align);
}

// Find all unresolved symbols and attach them to the most appropriate files.
// Note that even a symbol that will be reported as an undefined symbol will
// get an owner file in this function. Such symbol will be reported by
// ObjectFile<E>::scan_relocations().
template <typename E>
void claim_unresolved_symbols(Context<E> &ctx) {
  Timer t(ctx, "claim_unresolved_symbols");

  tbb::parallel_for_each(ctx.objs, [&](ObjectFile<E> *file) {
    if (!file->is_alive)
      return;

    for (i64 i = file->first_global; i < file->elf_syms.size(); i++) {
      const ElfSym<E> &esym = file->elf_syms[i];
      Symbol<E> &sym = *file->symbols[i];
      if (!esym.is_undef())
        continue;

      std::scoped_lock lock(sym.mu);

      if (sym.file)
        if (!sym.esym().is_undef() || sym.file->priority <= file->priority)
          continue;

      // If a symbol name is in the form of "foo@version", search for
      // symbol "foo" and check if the symbol has version "version".
      if (file->has_symver.get(i - file->first_global)) {
        std::string_view str = file->symbol_strtab.data() + esym.st_name;
        i64 pos = str.find('@');
        assert(pos != str.npos);

        std::string_view name = str.substr(0, pos);
        std::string_view ver = str.substr(pos + 1);

        Symbol<E> *sym2 = get_symbol(ctx, name);
        if (sym2->file && sym2->file->is_dso && sym2->get_version() == ver) {
          file->symbols[i] = sym2;
          continue;
        }
      }

      auto claim = [&](bool is_imported) {
        if (sym.is_traced)
          SyncOut(ctx) << "trace-symbol: " << *file << ": unresolved"
                       << (esym.is_weak() ? " weak" : "")
                       << " symbol " << sym;

        sym.file = file;
        sym.origin = 0;
        sym.value = 0;
        sym.sym_idx = i;
        sym.is_weak = false;
        sym.is_imported = is_imported;
        sym.is_exported = false;
        sym.ver_idx = is_imported ? 0 : ctx.default_version;
      };

      if (esym.is_undef_weak()) {
        if (ctx.arg.shared && sym.visibility != STV_HIDDEN &&
            ctx.arg.z_dynamic_undefined_weak) {
          // Global weak undefined symbols are promoted to dynamic symbols
          // when linking a DSO unless `-z nodynamic_undefined_weak` was given.
          claim(true);
        } else {
          // Otherwise, weak undefs are converted to absolute symbols with value 0.
          claim(false);
        }
        continue;
      }

      // Traditionally, remaining undefined symbols cause a link failure
      // only when we are creating an executable. Undefined symbols in
      // shared objects are promoted to dynamic symbols, so that they'll
      // get another chance to be resolved at run-time. You can change the
      // behavior by passing `-z defs` to the linker.
      //
      // Even if `-z defs` is given, weak undefined symbols are still
      // promoted to dynamic symbols for compatibility with other linkers.
      // Some major programs, notably Firefox, depend on the behavior
      // (they use this loophole to export symbols from libxul.so).
      if (ctx.arg.shared && sym.visibility != STV_HIDDEN && !ctx.arg.z_defs) {
        claim(true);
        continue;
      }

      // Convert remaining undefined symbols to absolute symbols with value 0.
      claim(false);
    }
  });
}

template <typename E>
void scan_relocations(Context<E> &ctx) {
  Timer t(ctx, "scan_relocations");

  // Scan relocations to find dynamic symbols.
  tbb::parallel_for_each(ctx.objs, [&](ObjectFile<E> *file) {
    file->scan_relocations(ctx);
  });

  // Exit if there was a relocation that refers an undefined symbol.
  ctx.checkpoint();

  // Aggregate dynamic symbols to a single vector.
  std::vector<InputFile<E> *> files;
  append(files, ctx.objs);
  append(files, ctx.dsos);

  std::vector<std::vector<Symbol<E> *>> vec(files.size());

  tbb::parallel_for((i64)0, (i64)files.size(), [&](i64 i) {
    for (Symbol<E> *sym : files[i]->symbols)
      if (sym->file == files[i])
        if (sym->flags || sym->is_imported || sym->is_exported)
          vec[i].push_back(sym);
  });

  std::vector<Symbol<E> *> syms = flatten(vec);
  ctx.symbol_aux.reserve(syms.size());

  // Assign offsets in additional tables for each dynamic symbol.
  for (Symbol<E> *sym : syms) {
    sym->add_aux(ctx);

    if (sym->is_imported || sym->is_exported)
      ctx.dynsym->add_symbol(ctx, sym);

    if (sym->flags & NEEDS_GOT)
      ctx.got->add_got_symbol(ctx, sym);

    if (sym->flags & NEEDS_CPLT) {
      sym->is_canonical = true;

      // A canonical PLT needs to be visible from DSOs.
      sym->is_exported = true;

      // We can't use .plt.got for a canonical PLT because otherwise
      // .plt.got and .got would refer to each other, resulting in an
      // infinite loop at runtime.
      ctx.plt->add_symbol(ctx, sym);
    } else if (sym->flags & NEEDS_PLT) {
      if (sym->flags & NEEDS_GOT)
        ctx.pltgot->add_symbol(ctx, sym);
      else
        ctx.plt->add_symbol(ctx, sym);
    }

    if (sym->flags & NEEDS_GOTTP)
      ctx.got->add_gottp_symbol(ctx, sym);

    if (sym->flags & NEEDS_TLSGD)
      ctx.got->add_tlsgd_symbol(ctx, sym);

    if (sym->flags & NEEDS_TLSDESC)
      ctx.got->add_tlsdesc_symbol(ctx, sym);

    if (sym->flags & NEEDS_COPYREL) {
      if (((SharedFile<E> *)sym->file)->is_readonly(sym))
        ctx.copyrel_relro->add_symbol(ctx, sym);
      else
        ctx.copyrel->add_symbol(ctx, sym);
    }

    if constexpr (is_ppc64v1<E>)
      if (sym->flags & NEEDS_PPC_OPD)
        ctx.extra.opd->add_symbol(ctx, sym);

    sym->flags = 0;
  }

  if (ctx.needs_tlsld)
    ctx.got->add_tlsld(ctx);

  if constexpr (is_alpha<E>)
    ctx.extra.got->finalize();

  if (ctx.has_textrel && ctx.arg.warn_textrel)
    Warn(ctx) << "creating a DT_TEXTREL in an output file";
}

// Report all undefined symbols, grouped by symbol.
template <typename E>
void report_undef_errors(Context<E> &ctx) {
  constexpr i64 max_errors = 3;

  for (auto &pair : ctx.undef_errors) {
    std::string_view sym_name = pair.first;
    std::span<std::string> errors = pair.second;

    if (ctx.arg.demangle)
      sym_name = demangle(sym_name);

    std::stringstream ss;
    ss << "undefined symbol: " << sym_name << "\n";

    for (i64 i = 0; i < errors.size() && i < max_errors; i++)
      ss << errors[i];

    if (errors.size() > max_errors)
      ss << ">>> referenced " << (errors.size() - max_errors) << " more times\n";

    if (ctx.arg.unresolved_symbols == UNRESOLVED_ERROR)
      Error(ctx) << ss.str();
    else if (ctx.arg.unresolved_symbols == UNRESOLVED_WARN)
      Warn(ctx) << ss.str();
  }

  ctx.checkpoint();
}

template <typename E>
void create_reloc_sections(Context<E> &ctx) {
  Timer t(ctx, "create_reloc_sections");

  // Create .rela.* sections
  tbb::parallel_for((i64)0, (i64)ctx.chunks.size(), [&](i64 i) {
    if (OutputSection<E> *osec = ctx.chunks[i]->to_osec())
      osec->reloc_sec.reset(new RelocSection<E>(ctx, *osec));
  });

  for (i64 i = 0, end = ctx.chunks.size(); i < end; i++)
    if (OutputSection<E> *osec = ctx.chunks[i]->to_osec())
      if (RelocSection<E> *x = osec->reloc_sec.get())
        ctx.chunks.push_back(x);
}

// Copy chunks to an output file
template <typename E>
void copy_chunks(Context<E> &ctx) {
  Timer t(ctx, "copy_chunks");

  auto copy = [&](Chunk<E> &chunk) {
    std::string name = chunk.name.empty() ? "(header)" : std::string(chunk.name);
    Timer t2(ctx, name, &t);
    chunk.copy_buf(ctx);
  };

  // For --relocatable and --emit-relocs, we want to copy non-relocation
  // sections first. This is because REL-type relocation sections (as
  // opposed to RELA-type) stores relocation addends to target sections.
  tbb::parallel_for_each(ctx.chunks, [&](Chunk<E> *chunk) {
    if (chunk->shdr.sh_type != (E::is_rela ? SHT_RELA : SHT_REL))
      copy(*chunk);
  });

  tbb::parallel_for_each(ctx.chunks, [&](Chunk<E> *chunk) {
    if (chunk->shdr.sh_type == (E::is_rela ? SHT_RELA : SHT_REL))
      copy(*chunk);
  });

  // Undefined symbols in SHF_ALLOC sections are found by scan_relocations(),
  // but those in non-SHF_ALLOC sections cannot be found until we copy section
  // contents. So we need to call this function again to report possible
  // undefined errors.
  report_undef_errors(ctx);

  if constexpr (is_arm32<E>)
    fixup_arm_exidx_section(ctx);
}

template <typename E>
void construct_relr(Context<E> &ctx) {
  Timer t(ctx, "construct_relr");

  tbb::parallel_for_each(ctx.chunks, [&](Chunk<E> *chunk) {
    if (OutputSection<E> *osec = chunk->to_osec())
      osec->construct_relr(ctx);
  });

  ctx.got->construct_relr(ctx);
}

template <typename E>
void create_output_symtab(Context<E> &ctx) {
  Timer t(ctx, "compute_symtab_size");

  tbb::parallel_for_each(ctx.chunks, [&](Chunk<E> *chunk) {
    chunk->compute_symtab_size(ctx);
  });

  tbb::parallel_for_each(ctx.objs, [&](ObjectFile<E> *file) {
    file->compute_symtab_size(ctx);
  });

  tbb::parallel_for_each(ctx.dsos, [&](SharedFile<E> *file) {
    file->compute_symtab_size(ctx);
  });
}

template <typename E>
void apply_version_script(Context<E> &ctx) {
  Timer t(ctx, "apply_version_script");

  // If all patterns are simple (i.e. not containing any meta-
  // characters and is not a C++ name), we can simply look up
  // symbols.
  auto is_simple = [&] {
    for (VersionPattern &v : ctx.version_patterns)
      if (v.is_cpp || v.pattern.find_first_of("*?[") != v.pattern.npos)
        return false;
    return true;
  };

  if (is_simple()) {
    for (VersionPattern &v : ctx.version_patterns) {
      Symbol<E> *sym = get_symbol(ctx, v.pattern);

      if (!sym->file && !ctx.arg.undefined_version)
        Warn(ctx) << v.source << ": cannot assign version `" << v.ver_str
                  << "` to symbol `" << *sym << "`: symbol not found";

      if (sym->file && !sym->file->is_dso)
        sym->ver_idx = v.ver_idx;
    }
    return;
  }

  // Otherwise, use glob pattern matchers.
  MultiGlob matcher;
  MultiGlob cpp_matcher;

  for (i64 i = 0; i < ctx.version_patterns.size(); i++) {
    VersionPattern &v = ctx.version_patterns[i];
    if (v.is_cpp) {
      if (!cpp_matcher.add(v.pattern, i))
        Fatal(ctx) << "invalid version pattern: " << v.pattern;
    } else {
      if (!matcher.add(v.pattern, i))
        Fatal(ctx) << "invalid version pattern: " << v.pattern;
    }
  }

  tbb::parallel_for_each(ctx.objs, [&](ObjectFile<E> *file) {
    for (Symbol<E> *sym : file->get_global_syms()) {
      if (sym->file != file)
        continue;

      std::string_view name = sym->name();
      i64 match = INT64_MAX;

      if (std::optional<u32> idx = matcher.find(name))
        match = std::min<i64>(match, *idx);

      // Match non-mangled symbols against the C++ pattern as well.
      // Weird, but required to match other linkers' behavior.
      if (!cpp_matcher.empty()) {
        if (std::optional<std::string_view> s = cpp_demangle(name))
          name = *s;
        if (std::optional<u32> idx = cpp_matcher.find(name))
          match = std::min<i64>(match, *idx);
      }

      if (match != INT64_MAX)
        sym->ver_idx = ctx.version_patterns[match].ver_idx;
    }
  });
}

template <typename E>
void parse_symbol_version(Context<E> &ctx) {
  if (!ctx.arg.shared)
    return;

  Timer t(ctx, "parse_symbol_version");

  std::unordered_map<std::string_view, u16> verdefs;
  for (i64 i = 0; i < ctx.arg.version_definitions.size(); i++)
    verdefs[ctx.arg.version_definitions[i]] = i + VER_NDX_LAST_RESERVED + 1;

  tbb::parallel_for_each(ctx.objs, [&](ObjectFile<E> *file) {
    for (i64 i = file->first_global; i < file->elf_syms.size(); i++) {
      // Match VERSION part of symbol foo@VERSION with version definitions.
      if (!file->has_symver.get(i - file->first_global))
        continue;

      Symbol<E> *sym = file->symbols[i];
      if (sym->file != file)
        continue;

      const char *name = file->symbol_strtab.data() + file->elf_syms[i].st_name;
      std::string_view ver = strchr(name, '@') + 1;

      bool is_default = false;
      if (ver.starts_with('@')) {
        is_default = true;
        ver = ver.substr(1);
      }

      auto it = verdefs.find(ver);
      if (it == verdefs.end()) {
        Error(ctx) << *file << ": symbol " << *sym <<  " has undefined version "
                   << ver;
        continue;
      }

      sym->ver_idx = it->second;
      if (!is_default)
        sym->ver_idx |= VERSYM_HIDDEN;

      // If both symbol `foo` and `foo@VERSION` are defined, `foo@VERSION`
      // hides `foo` so that all references to `foo` are resolved to a
      // versioned symbol. Likewise, if `foo@VERSION` and `foo@@VERSION` are
      // defined, the default one takes precedence.
      Symbol<E> *sym2 = get_symbol(ctx, sym->name());
      if (sym2->file == file &&
          !file->has_symver.get(sym2->sym_idx - file->first_global))
        if (sym2->ver_idx == ctx.default_version ||
            (sym2->ver_idx & ~VERSYM_HIDDEN) == (sym->ver_idx & ~VERSYM_HIDDEN))
          sym2->ver_idx = VER_NDX_LOCAL;
    }
  });
}

template <typename E>
void compute_import_export(Context<E> &ctx) {
  Timer t(ctx, "compute_import_export");

  // If we are creating an executable, we want to export symbols referenced
  // by DSOs unless they are explicitly marked as local by a version script.
  if (!ctx.arg.shared) {
    tbb::parallel_for_each(ctx.dsos, [&](SharedFile<E> *file) {
      for (Symbol<E> *sym : file->symbols) {
        if (sym->file && !sym->file->is_dso && sym->visibility != STV_HIDDEN) {
          if (sym->ver_idx != VER_NDX_LOCAL ||
              !ctx.default_version_from_version_script) {
            std::scoped_lock lock(sym->mu);
            sym->is_exported = true;
          }
        }
      }
    });
  }

  // Export symbols that are not hidden or marked as local.
  // We also want to mark imported symbols as such.
  tbb::parallel_for_each(ctx.objs, [&](ObjectFile<E> *file) {
    for (Symbol<E> *sym : file->get_global_syms()) {
      if (!sym->file || sym->visibility == STV_HIDDEN ||
          sym->ver_idx == VER_NDX_LOCAL)
        continue;

      // If we are using a symbol in a DSO, we need to import it at runtime.
      if (sym->file != file && sym->file->is_dso && !sym->is_absolute()) {
        std::scoped_lock lock(sym->mu);
        sym->is_imported = true;
        continue;
      }

      // If we are creating a DSO, all global symbols are exported by default.
      if (sym->file == file) {
        std::scoped_lock lock(sym->mu);
        sym->is_exported = true;

        if (ctx.arg.shared && sym->visibility != STV_PROTECTED &&
            !ctx.arg.Bsymbolic &&
            !(ctx.arg.Bsymbolic_functions && sym->get_type() == STT_FUNC))
          sym->is_imported = true;
      }
    }
  });
}

template <typename E>
void mark_addrsig(Context<E> &ctx) {
  Timer t(ctx, "mark_addrsig");

  tbb::parallel_for_each(ctx.objs, [&](ObjectFile<E> *file) {
    file->mark_addrsig(ctx);
  });
}

template <typename E>
void clear_padding(Context<E> &ctx) {
  Timer t(ctx, "clear_padding");

  auto zero = [&](Chunk<E> *chunk, i64 next_start) {
    i64 pos = chunk->shdr.sh_offset + chunk->shdr.sh_size;
    memset(ctx.buf + pos, 0, next_start - pos);
  };

  std::vector<Chunk<E> *> chunks = ctx.chunks;

  std::erase_if(chunks, [](Chunk<E> *chunk) {
    return chunk->shdr.sh_type == SHT_NOBITS;
  });

  for (i64 i = 1; i < chunks.size(); i++)
    zero(chunks[i - 1], chunks[i]->shdr.sh_offset);
  zero(chunks.back(), ctx.output_file->filesize);
}

// We want to sort output chunks in the following order.
//
//   <ELF header>
//   <program header>
//   .interp
//   .note
//   .hash
//   .gnu.hash
//   .dynsym
//   .dynstr
//   .gnu.version
//   .gnu.version_r
//   .rela.dyn
//   .rela.plt
//   <readonly data>
//   <readonly code>
//   <writable tdata>
//   <writable tbss>
//   <writable RELRO data>
//   .got
//   .toc
//   .alpha_got
//   <writable RELRO bss>
//   .relro_padding
//   <writable non-RELRO data>
//   <writable non-RELRO bss>
//   <non-memory-allocated sections>
//   <section header>
//
// .interp and some other linker-synthesized sections are placed at the
// beginning of a file because they are needed by loader. Especially on
// a hard drive with spinning disks, it is important to read these
// sections in a single seek.
//
// .note sections are also placed at the beginning so that they are
// included in a core crash dump even if it's truncated by ulimit. In
// particular, if .note.gnu.build-id is in a truncated core file, you
// can at least identify which executable has crashed.
//
// A PT_NOTE segment will contain multiple .note sections if exists,
// but there's no way to represent a gap between .note sections.
// Therefore, we sort .note sections by decreasing alignment
// requirement. I believe each .note section size is a multiple of its
// alignment, so by sorting them by alignment, we should be able to
// avoid a gap between .note sections.
//
// .toc is placed right after .got for PPC64. PPC-specific .toc section
// contains data that may be accessed with a 16-bit offset relative to
// %r2. %r2 is set to .got + 32 KiB. Therefore, .toc needs to be within
// [.got, .got + 64 KiB).
//
// Other file layouts are possible, but this layout is chosen to keep
// the number of segments as few as possible.
template <typename E>
void sort_output_sections_regular(Context<E> &ctx) {
  auto get_rank1 = [&](Chunk<E> *chunk) {
    u64 type = chunk->shdr.sh_type;
    u64 flags = chunk->shdr.sh_flags;

    if (chunk == ctx.ehdr)
      return 0;
    if (chunk == ctx.phdr)
      return 1;
    if (chunk == ctx.interp)
      return 2;
    if (type == SHT_NOTE && (flags & SHF_ALLOC))
      return 3;
    if (chunk == ctx.hash)
      return 4;
    if (chunk == ctx.gnu_hash)
      return 5;
    if (chunk == ctx.dynsym)
      return 6;
    if (chunk == ctx.dynstr)
      return 7;
    if (chunk == ctx.versym)
      return 8;
    if (chunk == ctx.verneed)
      return 9;
    if (chunk == ctx.reldyn)
      return 10;
    if (chunk == ctx.relplt)
      return 11;
    if (chunk == ctx.shdr)
      return INT32_MAX;

    bool alloc = (flags & SHF_ALLOC);
    bool writable = (flags & SHF_WRITE);
    bool exec = (flags & SHF_EXECINSTR);
    bool tls = (flags & SHF_TLS);
    bool relro = is_relro(ctx, chunk);
    bool is_bss = (type == SHT_NOBITS);

    return (1 << 10) | (!alloc << 9) | (writable << 8) | (exec << 7) |
           (!tls << 6) | (!relro << 5) | (is_bss << 4);
  };

  auto get_rank2 = [&](Chunk<E> *chunk) -> i64 {
    if (chunk->shdr.sh_type == SHT_NOTE)
      return -chunk->shdr.sh_addralign;

    if (chunk == ctx.got)
      return 1;
    if (chunk->name == ".toc")
      return 2;
    if (chunk->name == ".alpha_got")
      return 3;
    if (chunk == ctx.relro_padding)
      return INT_MAX;
    return 0;
  };

  sort(ctx.chunks, [&](Chunk<E> *a, Chunk<E> *b) {
    // Sort sections by segments
    i64 x = get_rank1(a);
    i64 y = get_rank1(b);
    if (x != y)
      return x < y;

    // Ties are broken by additional rules
    return get_rank2(a) < get_rank2(b);
  });
}

template <typename E>
static std::string_view get_section_order_group(Chunk<E> &chunk) {
  if (chunk.shdr.sh_type == SHT_NOBITS)
    return "BSS";
  if (chunk.shdr.sh_flags & SHF_EXECINSTR)
    return "TEXT";
  if (chunk.shdr.sh_flags & SHF_WRITE)
    return "DATA";
  return "RODATA";
};

// Sort sections according to a --section-order argument.
template <typename E>
void sort_output_sections_by_order(Context<E> &ctx) {
  auto get_rank = [&](Chunk<E> *chunk) -> i64 {
    u64 flags = chunk->shdr.sh_flags;

    if (chunk == ctx.ehdr && !(chunk->shdr.sh_flags & SHF_ALLOC))
      return -2;
    if (chunk == ctx.phdr && !(chunk->shdr.sh_flags & SHF_ALLOC))
      return -1;

    if (chunk == ctx.shdr)
      return INT32_MAX;
    if (!(flags & SHF_ALLOC))
      return INT32_MAX - 1;

    for (i64 i = 0; const SectionOrder &arg : ctx.arg.section_order) {
      if (arg.type == SectionOrder::SECTION && arg.name == chunk->name)
        return i;
      i++;
    }

    std::string_view group = get_section_order_group(*chunk);

    for (i64 i = 0; i < ctx.arg.section_order.size(); i++) {
      SectionOrder arg = ctx.arg.section_order[i];
      if (arg.type == SectionOrder::GROUP && arg.name == group)
        return i;
    }

    Error(ctx) << "--section-order: missing section specification for "
               << chunk->name;
    return 0;
  };

  // It is an error if a section order cannot be determined by a given
  // section order list.
  for (Chunk<E> *chunk : ctx.chunks)
    chunk->sect_order = get_rank(chunk);

  // Sort output sections by --section-order
  sort(ctx.chunks, [&](Chunk<E> *a, Chunk<E> *b) {
    return a->sect_order < b->sect_order;
  });
}

template <typename E>
void sort_output_sections(Context<E> &ctx) {
  if (ctx.arg.section_order.empty())
    sort_output_sections_regular(ctx);
  else
    sort_output_sections_by_order(ctx);
}

template <typename E>
static bool is_tbss(Chunk<E> *chunk) {
  return (chunk->shdr.sh_type == SHT_NOBITS) && (chunk->shdr.sh_flags & SHF_TLS);
}

// This function assigns virtual addresses to output sections. Assigning
// addresses is a bit tricky because we want to pack sections as tightly
// as possible while not violating the constraints imposed by the hardware
// and the OS kernel. Specifically, we need to satisfy the following
// constraints:
//
// - Memory protection (readable, writable and executable) works at page
//   granularity. Therefore, if we want to set different memory attributes
//   to two sections, we need to place them into separate pages.
//
// - The ELF spec requires that a section's file offset is congruent to
//   its virtual address modulo the page size. For example, a section at
//   virtual address 0x401234 on x86-64 (4 KiB, or 0x1000 byte page
//   system) can be at file offset 0x3234 or 0x50234 but not at 0x1000.
//
// We need to insert paddings between sections if we can't satisfy the
// above constraints without them.
//
// We don't want to waste too much memory and disk space for paddings.
// There are a few tricks we can use to minimize paddings as below:
//
// - We want to place sections with the same memory attributes
//   contiguous as possible.
//
// - We can map the same file region to memory more than once. For
//   example, we can write code (with R and X bits) and read-only data
//   (with R bit) adjacent on file and map it twice as the last page of
//   the executable segment and the first page of the read-only data
//   segment. This doesn't save memory but saves disk space.
template <typename E>
static void set_virtual_addresses_regular(Context<E> &ctx) {
  constexpr i64 RELRO = 1LL << 32;

  auto get_flags = [&](Chunk<E> *chunk) {
    i64 flags = to_phdr_flags(ctx, chunk);
    if (is_relro(ctx, chunk))
      return flags | RELRO;
    return flags;
  };

  // Assign virtual addresses
  std::vector<Chunk<E> *> &chunks = ctx.chunks;
  u64 addr = ctx.arg.image_base;

  // TLS chunks alignments are special: in addition to having their virtual
  // addresses aligned, they also have to be aligned when the region of
  // tls_begin is copied to a new thread's storage area. In other words, their
  // offset against tls_begin also has to be aligned.
  //
  // A good way to achieve this is to take the largest alignment requirement
  // of all TLS sections and make tls_begin also aligned to that.
  Chunk<E> *first_tls_chunk = nullptr;
  u64 tls_alignment = 1;
  for (Chunk<E> *chunk : chunks) {
    if (chunk->shdr.sh_flags & SHF_TLS) {
      if (!first_tls_chunk)
        first_tls_chunk = chunk;
      tls_alignment = std::max(tls_alignment, (u64)chunk->shdr.sh_addralign);
    }
  }

  auto alignment = [&](Chunk<E> *chunk) {
    return chunk == first_tls_chunk ? tls_alignment : (u64)chunk->shdr.sh_addralign;
  };

  for (i64 i = 0; i < chunks.size(); i++) {
    if (!(chunks[i]->shdr.sh_flags & SHF_ALLOC))
      continue;

    // .relro_padding is a padding section to extend a PT_GNU_RELRO
    // segment to cover an entire page. Technically, we don't need a
    // .relro_padding section because we can leave a trailing part of a
    // segment an unused space. However, the `strip` command would delete
    // such an unused trailing part and make an executable invalid.
    // So we add a dummy section.
    if (chunks[i] == ctx.relro_padding) {
      chunks[i]->shdr.sh_addr = addr;
      chunks[i]->shdr.sh_size = align_to(addr, ctx.page_size) - addr;
      addr += ctx.page_size;
      continue;
    }

    // Handle --section-start first
    if (auto it = ctx.arg.section_start.find(chunks[i]->name);
        it != ctx.arg.section_start.end()) {
      addr = it->second;
      chunks[i]->shdr.sh_addr = addr;
      addr += chunks[i]->shdr.sh_size;
      continue;
    }

    // Memory protection works at page size granularity. We need to
    // put sections with different memory attributes into different
    // pages. We do it by inserting paddings here.
    if (i > 0 && chunks[i - 1] != ctx.relro_padding) {
      i64 flags1 = get_flags(chunks[i - 1]);
      i64 flags2 = get_flags(chunks[i]);

      if (flags1 != flags2) {
        switch (ctx.arg.z_separate_code) {
        case SEPARATE_LOADABLE_SEGMENTS:
          addr = align_to(addr, ctx.page_size);
          break;
        case SEPARATE_CODE:
          if ((flags1 & PF_X) != (flags2 & PF_X)) {
            addr = align_to(addr, ctx.page_size);
            break;
          }
          [[fallthrough]];
        case NOSEPARATE_CODE:
          if (addr % ctx.page_size != 0)
            addr += ctx.page_size;
          break;
        default:
          unreachable();
        }
      }
    }

    // TLS BSS sections are laid out so that they overlap with the
    // subsequent non-tbss sections. Overlapping is fine because a STT_TLS
    // segment contains an initialization image for newly-created threads,
    // and no one except the runtime reads its contents. Even the runtime
    // doesn't need a BSS part of a TLS initialization image; it just
    // leaves zero-initialized bytes as-is instead of copying zeros.
    // So no one really read tbss at runtime.
    //
    // We can instead allocate a dedicated virtual address space to tbss,
    // but that would be just a waste of the address and disk space.
    if (is_tbss(chunks[i])) {
      u64 addr2 = addr;
      for (;;) {
        addr2 = align_to(addr2, alignment(chunks[i]));
        chunks[i]->shdr.sh_addr = addr2;
        addr2 += chunks[i]->shdr.sh_size;
        if (i + 2 == chunks.size() || !is_tbss(chunks[i + 1]))
          break;
        i++;
      }
      continue;
    }

    addr = align_to(addr, alignment(chunks[i]));
    chunks[i]->shdr.sh_addr = addr;
    addr += chunks[i]->shdr.sh_size;
  }
}

template <typename E>
static void set_virtual_addresses_by_order(Context<E> &ctx) {
  std::vector<Chunk<E> *> &c = ctx.chunks;
  u64 addr = ctx.arg.image_base;
  i64 i = 0;

  while (i < c.size() && !(c[i]->shdr.sh_flags & SHF_ALLOC))
    i++;

  auto assign_addr = [&] {
    if (i != 0) {
      i64 flags1 = to_phdr_flags(ctx, c[i - 1]);
      i64 flags2 = to_phdr_flags(ctx, c[i]);

      // Memory protection works at page size granularity. We need to
      // put sections with different memory attributes into different
      // pages. We do it by inserting paddings here.
      if (flags1 != flags2) {
        switch (ctx.arg.z_separate_code) {
        case SEPARATE_LOADABLE_SEGMENTS:
          addr = align_to(addr, ctx.page_size);
          break;
        case SEPARATE_CODE:
          if ((flags1 & PF_X) != (flags2 & PF_X))
            addr = align_to(addr, ctx.page_size);
          break;
        default:
          break;
        }
      }
    }

    addr = align_to(addr, c[i]->shdr.sh_addralign);
    c[i]->shdr.sh_addr = addr;
    addr += c[i]->shdr.sh_size;

    do {
      i++;
    } while (i < c.size() && !(c[i]->shdr.sh_flags & SHF_ALLOC));
  };

  for (i64 j = 0; j < ctx.arg.section_order.size(); j++) {
    SectionOrder &ord = ctx.arg.section_order[j];
    switch (ord.type) {
    case SectionOrder::SECTION:
      if (i < c.size() && j == c[i]->sect_order)
        assign_addr();
      break;
    case SectionOrder::GROUP:
      while (i < c.size() && j == c[i]->sect_order)
        assign_addr();
      break;
    case SectionOrder::ADDR:
      addr = ord.value;
      break;
    case SectionOrder::ALIGN:
      addr = align_to(addr, ord.value);
      break;
    case SectionOrder::SYMBOL:
      get_symbol(ctx, ord.name)->value = addr;
      break;
    default:
      unreachable();
    }
  }
}

// Returns the smallest integer N that satisfies N >= val and
// N mod align == skew mod align.
//
// Section's file offset must be congruent to its virtual address modulo
// the page size. We use this function to satisfy that requirement.
static u64 align_with_skew(u64 val, u64 align, u64 skew) {
  u64 x = align_down(val, align) + skew % align;
  return (val <= x) ? x : x + align;
}

// Assign file offsets to output sections.
template <typename E>
static i64 set_file_offsets(Context<E> &ctx) {
  std::vector<Chunk<E> *> &chunks = ctx.chunks;
  u64 fileoff = 0;
  i64 i = 0;

  while (i < chunks.size()) {
    Chunk<E> &first = *chunks[i];

    if (!(first.shdr.sh_flags & SHF_ALLOC)) {
      fileoff = align_to(fileoff, first.shdr.sh_addralign);
      first.shdr.sh_offset = fileoff;
      fileoff += first.shdr.sh_size;
      i++;
      continue;
    }

    if (first.shdr.sh_type == SHT_NOBITS) {
      i++;
      continue;
    }

    if (first.shdr.sh_addralign > ctx.page_size)
      fileoff = align_to(fileoff, first.shdr.sh_addralign);
    else
      fileoff = align_with_skew(fileoff, ctx.page_size, first.shdr.sh_addr);

    // Assign ALLOC sections contiguous file offsets as long as they
    // are contiguous in memory.
    for (;;) {
      chunks[i]->shdr.sh_offset =
        fileoff + chunks[i]->shdr.sh_addr - first.shdr.sh_addr;
      i++;

      if (i >= chunks.size() ||
          !(chunks[i]->shdr.sh_flags & SHF_ALLOC) ||
          chunks[i]->shdr.sh_type == SHT_NOBITS)
        break;

      // If --start-section is given, addresses may not increase
      // monotonically.
      if (chunks[i]->shdr.sh_addr < first.shdr.sh_addr)
        break;

      i64 gap_size = chunks[i]->shdr.sh_addr - chunks[i - 1]->shdr.sh_addr -
                     chunks[i - 1]->shdr.sh_size;

      // If --start-section is given, there may be a large gap between
      // sections. We don't want to allocate a disk space for a gap if
      // exists.
      if (gap_size >= ctx.page_size)
        break;
    }

    fileoff = chunks[i - 1]->shdr.sh_offset + chunks[i - 1]->shdr.sh_size;

    while (i < chunks.size() &&
           (chunks[i]->shdr.sh_flags & SHF_ALLOC) &&
           chunks[i]->shdr.sh_type == SHT_NOBITS)
      i++;
  }

  return fileoff;
}

template <typename E>
void compute_section_headers(Context<E> &ctx) {
  // Update sh_size for each chunk.
  for (Chunk<E> *chunk : ctx.chunks)
    chunk->update_shdr(ctx);

  // Remove empty chunks.
  std::erase_if(ctx.chunks, [](Chunk<E> *chunk) {
    return chunk->kind() != OUTPUT_SECTION && chunk->shdr.sh_size == 0;
  });

  // Set section indices.
  i64 shndx = 1;
  for (i64 i = 0; i < ctx.chunks.size(); i++)
    if (ctx.chunks[i]->kind() != HEADER)
      ctx.chunks[i]->shndx = shndx++;

  if (ctx.symtab && SHN_LORESERVE <= shndx) {
    SymtabShndxSection<E> *sec = new SymtabShndxSection<E>;
    sec->shndx = shndx++;
    sec->shdr.sh_link = ctx.symtab->shndx;
    ctx.symtab_shndx = sec;
    ctx.chunks.push_back(sec);
    ctx.chunk_pool.emplace_back(sec);
  }

  if (ctx.shdr)
    ctx.shdr->shdr.sh_size = shndx * sizeof(ElfShdr<E>);

  // Some types of section header refer other section by index.
  // Recompute the section header to fill such fields with correct values.
  for (Chunk<E> *chunk : ctx.chunks)
    chunk->update_shdr(ctx);

  if (ctx.symtab_shndx) {
    i64 symtab_size = ctx.symtab->shdr.sh_size / sizeof(ElfSym<E>);
    ctx.symtab_shndx->shdr.sh_size = symtab_size * 4;
  }
}

// Assign virtual addresses and file offsets to output sections.
template <typename E>
i64 set_osec_offsets(Context<E> &ctx) {
  Timer t(ctx, "set_osec_offsets");

  for (;;) {
    if (ctx.arg.section_order.empty())
      set_virtual_addresses_regular(ctx);
    else
      set_virtual_addresses_by_order(ctx);

    i64 fileoff = set_file_offsets(ctx);

    // Assigning new offsets may change the contents and the length
    // of the program header, so repeat it until converge.
    if (!ctx.phdr)
      return fileoff;

    i64 sz = ctx.phdr->shdr.sh_size;
    ctx.phdr->update_shdr(ctx);
    if (sz == ctx.phdr->shdr.sh_size)
      return fileoff;
  }
}

template <typename E>
static i64 get_num_irelative_relocs(Context<E> &ctx) {
  i64 n = std::count_if(ctx.got->got_syms.begin(), ctx.got->got_syms.end(),
                        [](Symbol<E> *sym) { return sym->is_ifunc(); });
  return n + ctx.num_ifunc_dynrels;
}

template <typename E>
static u64 to_paddr(Context<E> &ctx, u64 vaddr) {
  for (ElfPhdr<E> &phdr : ctx.phdr->phdrs)
    if (phdr.p_type == PT_LOAD)
      if (phdr.p_vaddr <= vaddr && vaddr < phdr.p_vaddr + phdr.p_memsz)
        return phdr.p_paddr + (vaddr - phdr.p_vaddr);
  return 0;
}

template <typename E>
void fix_synthetic_symbols(Context<E> &ctx) {
  auto start = [](Symbol<E> *sym, auto &chunk, i64 bias = 0) {
    if (sym && chunk) {
      sym->set_output_section(chunk);
      sym->value = chunk->shdr.sh_addr + bias;
    }
  };

  auto stop = [](Symbol<E> *sym, auto &chunk) {
    if (sym && chunk) {
      sym->set_output_section(chunk);
      sym->value = chunk->shdr.sh_addr + chunk->shdr.sh_size;
    }
  };

  std::vector<Chunk<E> *> sections;
  for (Chunk<E> *chunk : ctx.chunks)
    if (chunk->kind() != HEADER && (chunk->shdr.sh_flags & SHF_ALLOC))
      sections.push_back(chunk);

  auto find = [&](std::string name) -> Chunk<E> * {
    for (Chunk<E> *chunk : sections)
      if (chunk->name == name)
        return chunk;
    return nullptr;
  };

  // __bss_start
  if (Chunk<E> *chunk = find(".bss"))
    start(ctx.__bss_start, chunk);

  if (ctx.ehdr && (ctx.ehdr->shdr.sh_flags & SHF_ALLOC)) {
    ctx.__ehdr_start->set_output_section(sections[0]);
    ctx.__ehdr_start->value = ctx.ehdr->shdr.sh_addr;
    ctx.__executable_start->set_output_section(sections[0]);
    ctx.__executable_start->value = ctx.ehdr->shdr.sh_addr;
  }

  if (ctx.__dso_handle) {
    ctx.__dso_handle->set_output_section(sections[0]);
    ctx.__dso_handle->value = sections[0]->shdr.sh_addr;
  }

  // __rel_iplt_start and __rel_iplt_end. These symbols need to be
  // defined in a statically-linked non-relocatable executable because
  // such executable lacks the .dynamic section and thus there's no way
  // to find ifunc relocations other than these symbols.
  //
  // We don't want to set values to these symbols if we are creating a
  // static PIE due to a glibc bug. Static PIE has a dynamic section.
  // If we set values to these symbols in a static PIE, glibc attempts
  // to run ifunc initializers twice, with the second attempt with wrong
  // function addresses, causing a segmentation fault.
  if (ctx.reldyn && ctx.arg.is_static && !ctx.arg.pie) {
    stop(ctx.__rel_iplt_start, ctx.reldyn);
    stop(ctx.__rel_iplt_end, ctx.reldyn);

    ctx.__rel_iplt_start->value -=
      get_num_irelative_relocs(ctx) * sizeof(ElfRel<E>);
  }

  // __{init,fini}_array_{start,end}
  for (Chunk<E> *chunk : sections) {
    switch (chunk->shdr.sh_type) {
    case SHT_INIT_ARRAY:
      start(ctx.__init_array_start, chunk);
      stop(ctx.__init_array_end, chunk);
      break;
    case SHT_PREINIT_ARRAY:
      start(ctx.__preinit_array_start, chunk);
      stop(ctx.__preinit_array_end, chunk);
      break;
    case SHT_FINI_ARRAY:
      start(ctx.__fini_array_start, chunk);
      stop(ctx.__fini_array_end, chunk);
      break;
    }
  }

  // _end, _etext, _edata and the like
  for (Chunk<E> *chunk : sections) {
    if (chunk->shdr.sh_flags & SHF_ALLOC) {
      stop(ctx._end, chunk);
      stop(ctx.end, chunk);
    }

    if (chunk->shdr.sh_flags & SHF_EXECINSTR) {
      stop(ctx._etext, chunk);
      stop(ctx.etext, chunk);
    }

    if (chunk->shdr.sh_type != SHT_NOBITS &&
        (chunk->shdr.sh_flags & SHF_ALLOC)) {
      stop(ctx._edata, chunk);
      stop(ctx.edata, chunk);
    }
  }

  // _DYNAMIC
  start(ctx._DYNAMIC, ctx.dynamic);

  // _GLOBAL_OFFSET_TABLE_. I don't know why, but for the sake of
  // compatibility with existing code, it must be set to the beginning of
  // .got.plt instead of .got only on i386 and x86-64.
  if constexpr (is_x86<E>)
    start(ctx._GLOBAL_OFFSET_TABLE_, ctx.gotplt);
  else
    start(ctx._GLOBAL_OFFSET_TABLE_, ctx.got);

  // _PROCEDURE_LINKAGE_TABLE_. We need this on SPARC.
  start(ctx._PROCEDURE_LINKAGE_TABLE_, ctx.plt);

  // _TLS_MODULE_BASE_. This symbol is used to obtain the address of
  // the TLS block in the TLSDESC model. I believe GCC and Clang don't
  // create a reference to it, but Intel compiler seems to be using
  // this symbol.
  if (ctx._TLS_MODULE_BASE_) {
    ctx._TLS_MODULE_BASE_->set_output_section(sections[0]);
    ctx._TLS_MODULE_BASE_->value = ctx.tls_begin;
  }

  // __GNU_EH_FRAME_HDR
  start(ctx.__GNU_EH_FRAME_HDR, ctx.eh_frame_hdr);

  // RISC-V's __global_pointer$
  if (ctx.__global_pointer) {
    if (Chunk<E> *chunk = find(".sdata")) {
      start(ctx.__global_pointer, chunk, 0x800);
    } else {
      ctx.__global_pointer->set_output_section(sections[0]);
      ctx.__global_pointer->value = 0;
    }
  }

  // ARM32's __exidx_{start,end}
  if (ctx.__exidx_start) {
    if (Chunk<E> *chunk = find(".ARM.exidx")) {
      start(ctx.__exidx_start, chunk);
      stop(ctx.__exidx_end, chunk);
    }
  }

  // PPC64's ".TOC." symbol.
  if constexpr (is_ppc64<E>) {
    if (Chunk<E> *chunk = find(".got")) {
      start(ctx.extra.TOC, chunk, 0x8000);
    } else if (Chunk<E> *chunk = find(".toc")) {
      start(ctx.extra.TOC, chunk, 0x8000);
    } else {
      ctx.extra.TOC->set_output_section(sections[0]);
      ctx.extra.TOC->value = 0;
    }
  }

  // __start_ and __stop_ symbols
  for (Chunk<E> *chunk : sections) {
    if (std::optional<std::string> name = get_start_stop_name(ctx, *chunk)) {
      start(get_symbol(ctx, save_string(ctx, "__start_" + *name)), chunk);
      stop(get_symbol(ctx, save_string(ctx, "__stop_" + *name)), chunk);

      if (ctx.arg.physical_image_base) {
        u64 paddr = to_paddr(ctx, chunk->shdr.sh_addr);

        Symbol<E> *x = get_symbol(ctx, save_string(ctx, "__phys_start_" + *name));
        x->set_output_section(chunk);
        x->value = paddr;

        Symbol<E> *y = get_symbol(ctx, save_string(ctx, "__phys_stop_" + *name));
        y->set_output_section(chunk);
        y->value = paddr + chunk->shdr.sh_size;
      }
    }
  }

  // --defsym=sym=value symbols
  for (i64 i = 0; i < ctx.arg.defsyms.size(); i++) {
    Symbol<E> *sym = ctx.arg.defsyms[i].first;
    std::variant<Symbol<E> *, u64> val = ctx.arg.defsyms[i].second;

    if (u64 *addr = std::get_if<u64>(&val)) {
      sym->origin = 0;
      sym->value = *addr;
      continue;
    }

    Symbol<E> *sym2 = std::get<Symbol<E> *>(val);
    if (!sym2->file) {
      Error(ctx) << "--defsym: undefined symbol: " << *sym2;
      continue;
    }

    sym->value = sym2->value;
    sym->origin = sym2->origin;
    sym->visibility = sym2->visibility.load();
  }


  // --section-order symbols
  for (SectionOrder &ord : ctx.arg.section_order)
    if (ord.type == SectionOrder::SYMBOL)
      get_symbol(ctx, ord.name)->set_output_section(sections[0]);
}

template <typename E>
i64 compress_debug_sections(Context<E> &ctx) {
  Timer t(ctx, "compress_debug_sections");

  tbb::parallel_for((i64)0, (i64)ctx.chunks.size(), [&](i64 i) {
    Chunk<E> &chunk = *ctx.chunks[i];

    if ((chunk.shdr.sh_flags & SHF_ALLOC) || chunk.shdr.sh_size == 0 ||
        !chunk.name.starts_with(".debug"))
      return;

    Chunk<E> *comp = new CompressedSection<E>(ctx, chunk);
    ctx.chunk_pool.emplace_back(comp);
    ctx.chunks[i] = comp;
  });

  ctx.shstrtab->update_shdr(ctx);

  if (ctx.ehdr)
    ctx.ehdr->update_shdr(ctx);
  if (ctx.shdr)
    ctx.shdr->update_shdr(ctx);

  return set_osec_offsets(ctx);
}

// Write Makefile-style dependency rules to a file specified by
// --dependency-file. This is analogous to the compiler's -M flag.
template <typename E>
void write_dependency_file(Context<E> &ctx) {
  std::vector<std::string> deps;
  std::unordered_set<std::string> seen;

  for (std::unique_ptr<MappedFile<Context<E>>> &mf : ctx.mf_pool)
    if (!mf->parent)
      if (std::string path = path_clean(mf->name); seen.insert(path).second)
        deps.push_back(path);

  std::ofstream out;
  out.open(ctx.arg.dependency_file);
  if (out.fail())
    Fatal(ctx) << "--dependency-file: cannot open " << ctx.arg.dependency_file
               << ": " << errno_string();

  out << ctx.arg.output << ":";
  for (std::string &s : deps)
    out << " " << s;
  out << "\n";

  for (std::string &s : deps)
    out << "\n" << s << ":\n";
  out.close();
}

template <typename E>
void show_stats(Context<E> &ctx) {
  for (ObjectFile<E> *obj : ctx.objs) {
    static Counter defined("defined_syms");
    defined += obj->first_global - 1;

    static Counter undefined("undefined_syms");
    undefined += obj->symbols.size() - obj->first_global;

    for (std::unique_ptr<InputSection<E>> &sec : obj->sections) {
      if (!sec || !sec->is_alive)
        continue;

      static Counter alloc("reloc_alloc");
      static Counter nonalloc("reloc_nonalloc");

      if (sec->shdr().sh_flags & SHF_ALLOC)
        alloc += sec->get_rels(ctx).size();
      else
        nonalloc += sec->get_rels(ctx).size();
    }

    static Counter comdats("comdats");
    comdats += obj->comdat_groups.size();

    static Counter removed_comdats("removed_comdat_mem");
    for (ComdatGroupRef<E> &ref : obj->comdat_groups)
      if (ref.group->owner != obj->priority)
        removed_comdats += ref.members.size();

    static Counter num_cies("num_cies");
    num_cies += obj->cies.size();

    static Counter num_unique_cies("num_unique_cies");
    for (CieRecord<E> &cie : obj->cies)
      if (cie.is_leader)
        num_unique_cies++;

    static Counter num_fdes("num_fdes");
    num_fdes +=  obj->fdes.size();
  }

  static Counter num_bytes("total_input_bytes");
  for (std::unique_ptr<MappedFile<Context<E>>> &mf : ctx.mf_pool)
    num_bytes += mf->size;

  static Counter num_input_sections("input_sections");
  for (ObjectFile<E> *file : ctx.objs)
    num_input_sections += file->sections.size();

  static Counter num_output_chunks("output_chunks", ctx.chunks.size());
  static Counter num_objs("num_objs", ctx.objs.size());
  static Counter num_dsos("num_dsos", ctx.dsos.size());

  if constexpr (needs_thunk<E>) {
    static Counter thunk_bytes("thunk_bytes");
    for (Chunk<E> *chunk : ctx.chunks)
      if (OutputSection<E> *osec = chunk->to_osec())
        for (std::unique_ptr<RangeExtensionThunk<E>> &thunk : osec->thunks)
          thunk_bytes += thunk->size();
  }

  Counter::print();

  for (std::unique_ptr<MergedSection<E>> &sec : ctx.merged_sections)
    sec->print_stats(ctx);
}

using E = MOLD_TARGET;

template void create_internal_file(Context<E> &);
template void apply_exclude_libs(Context<E> &);
template void create_synthetic_sections(Context<E> &);
template void resolve_symbols(Context<E> &);
template void kill_eh_frame_sections(Context<E> &);
template void resolve_section_pieces(Context<E> &);
template void convert_common_symbols(Context<E> &);
template void compute_merged_section_sizes(Context<E> &);
template void create_output_sections(Context<E> &);
template void add_synthetic_symbols(Context<E> &);
template void check_cet_errors(Context<E> &);
template void print_dependencies(Context<E> &);
template void write_repro_file(Context<E> &);
template void check_duplicate_symbols(Context<E> &);
template void check_symbol_types(Context<E> &);
template void sort_init_fini(Context<E> &);
template void sort_ctor_dtor(Context<E> &);
template void shuffle_sections(Context<E> &);
template void compute_section_sizes(Context<E> &);
template void sort_output_sections(Context<E> &);
template void claim_unresolved_symbols(Context<E> &);
template void scan_relocations(Context<E> &);
template void report_undef_errors(Context<E> &);
template void create_reloc_sections(Context<E> &);
template void copy_chunks(Context<E> &);
template void construct_relr(Context<E> &);
template void create_output_symtab(Context<E> &);
template void apply_version_script(Context<E> &);
template void parse_symbol_version(Context<E> &);
template void compute_import_export(Context<E> &);
template void mark_addrsig(Context<E> &);
template void clear_padding(Context<E> &);
template void compute_section_headers(Context<E> &);
template i64 set_osec_offsets(Context<E> &);
template void fix_synthetic_symbols(Context<E> &);
template i64 compress_debug_sections(Context<E> &);
template void write_dependency_file(Context<E> &);
template void show_stats(Context<E> &);

} // namespace mold::elf
