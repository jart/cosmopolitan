// clang-format off
#include "third_party/mold/elf/mold.h"
// MISSING #include "../common/archive-file.h"
// MISSING #include "../common/cmdline.h"
// MISSING #include "../common/output-file.h"

#include "third_party/libcxx/cstring"
#include "third_party/libcxx/functional"
#include "third_party/libcxx/iomanip"
#include "third_party/libcxx/map"
#include "third_party/libcxx/regex"
#include "libc/calls/calls.h"
#include "libc/calls/sigtimedwait.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/ss.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/stat.macros.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/weirdtypes.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/consts/utime.h"
#include "libc/time/time.h"
#include "libc/calls/makedev.h"
#include "libc/calls/weirdtypes.h"
#include "libc/thread/thread.h"
#include "libc/calls/typedef/u.h"
#include "libc/calls/weirdtypes.h"
#include "libc/intrin/newbie.h"
#include "libc/sock/select.h"
#include "libc/sysv/consts/endian.h"
// MISSING #include <tbb/global_control.h>
// MISSING #include <tbb/parallel_for_each.h>
#include "third_party/libcxx/unordered_set"

#ifdef _WIN32
// MISSING #include <direct.h>
# define _chdir chdir
#else
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/runtime/pathconf.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/sysconf.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/ok.h"
#include "libc/time/time.h"
#include "third_party/getopt/getopt.h"
#include "third_party/musl/crypt.h"
#include "third_party/musl/lockf.h"
#endif

namespace mold::elf {

// Read the beginning of a given file and returns its machine type
// (e.g. EM_X86_64 or EM_386).
template <typename E>
std::string_view get_machine_type(Context<E> &ctx, MappedFile<Context<E>> *mf) {
  auto get_elf_type = [&](u8 *buf) -> std::string_view {
    bool is_le = (((ElfEhdr<I386> *)buf)->e_ident[EI_DATA] == ELFDATA2LSB);
    bool is_64;
    u32 e_machine;

    if (is_le) {
      auto &ehdr = *(ElfEhdr<I386> *)buf;
      is_64 = (ehdr.e_ident[EI_CLASS] == ELFCLASS64);
      e_machine = ehdr.e_machine;
    } else {
      auto &ehdr = *(ElfEhdr<M68K> *)buf;
      is_64 = (ehdr.e_ident[EI_CLASS] == ELFCLASS64);
      e_machine = ehdr.e_machine;
    }

    switch (e_machine) {
    case EM_386:
      return I386::target_name;
    case EM_X86_64:
      return X86_64::target_name;
    case EM_ARM:
      return ARM32::target_name;
    case EM_AARCH64:
      return ARM64::target_name;
    case EM_RISCV:
      if (is_le)
        return is_64 ? RV64LE::target_name : RV32LE::target_name;
      return is_64 ? RV64BE::target_name : RV32BE::target_name;
    case EM_PPC:
      return PPC32::target_name;
    case EM_PPC64:
      return is_le ? PPC64V2::target_name : PPC64V1::target_name;
    case EM_S390X:
      return S390X::target_name;
    case EM_SPARC64:
      return SPARC64::target_name;
    case EM_68K:
      return M68K::target_name;
    case EM_SH:
      return SH4::target_name;
    case EM_ALPHA:
      return ALPHA::target_name;
    default:
      return "";
    }
  };

  switch (get_file_type(ctx, mf)) {
  case FileType::ELF_OBJ:
  case FileType::ELF_DSO:
  case FileType::GCC_LTO_OBJ:
    return get_elf_type(mf->data);
  case FileType::AR:
    for (MappedFile<Context<E>> *child : read_fat_archive_members(ctx, mf))
      if (get_file_type(ctx, child) == FileType::ELF_OBJ)
        return get_elf_type(child->data);
    return "";
  case FileType::THIN_AR:
    for (MappedFile<Context<E>> *child : read_thin_archive_members(ctx, mf))
      if (get_file_type(ctx, child) == FileType::ELF_OBJ)
        return get_elf_type(child->data);
    return "";
  case FileType::TEXT:
    return get_script_output_type(ctx, mf);
  default:
    return "";
  }
}

template <typename E>
static void
check_file_compatibility(Context<E> &ctx, MappedFile<Context<E>> *mf) {
  std::string_view target = get_machine_type(ctx, mf);
  if (target != ctx.arg.emulation)
    Fatal(ctx) << mf->name << ": incompatible file type: "
               << ctx.arg.emulation << " is expected but got " << target;
}

template <typename E>
static ObjectFile<E> *new_object_file(Context<E> &ctx, MappedFile<Context<E>> *mf,
                                      std::string archive_name) {
  static Counter count("parsed_objs");
  count++;

  check_file_compatibility(ctx, mf);

  bool in_lib = ctx.in_lib || (!archive_name.empty() && !ctx.whole_archive);
  ObjectFile<E> *file = ObjectFile<E>::create(ctx, mf, archive_name, in_lib);
  file->priority = ctx.file_priority++;
  ctx.tg.run([file, &ctx] { file->parse(ctx); });
  if (ctx.arg.trace)
    SyncOut(ctx) << "trace: " << *file;
  return file;
}

template <typename E>
static ObjectFile<E> *new_lto_obj(Context<E> &ctx, MappedFile<Context<E>> *mf,
                                  std::string archive_name) {
  static Counter count("parsed_lto_objs");
  count++;

  if (ctx.arg.ignore_ir_file.count(mf->get_identifier()))
    return nullptr;

  ObjectFile<E> *file = read_lto_object(ctx, mf);
  file->priority = ctx.file_priority++;
  file->archive_name = archive_name;
  file->is_in_lib = ctx.in_lib || (!archive_name.empty() && !ctx.whole_archive);
  file->is_alive = !file->is_in_lib;
  ctx.has_lto_object = true;
  if (ctx.arg.trace)
    SyncOut(ctx) << "trace: " << *file;
  return file;
}

template <typename E>
static SharedFile<E> *
new_shared_file(Context<E> &ctx, MappedFile<Context<E>> *mf) {
  check_file_compatibility(ctx, mf);

  SharedFile<E> *file = SharedFile<E>::create(ctx, mf);
  file->priority = ctx.file_priority++;
  ctx.tg.run([file, &ctx] { file->parse(ctx); });
  if (ctx.arg.trace)
    SyncOut(ctx) << "trace: " << *file;
  return file;
}

template <typename E>
void read_file(Context<E> &ctx, MappedFile<Context<E>> *mf) {
  if (ctx.visited.contains(mf->name))
    return;

  switch (get_file_type(ctx, mf)) {
  case FileType::ELF_OBJ:
    ctx.objs.push_back(new_object_file(ctx, mf, ""));
    return;
  case FileType::ELF_DSO:
    ctx.dsos.push_back(new_shared_file(ctx, mf));
    ctx.visited.insert(mf->name);
    return;
  case FileType::AR:
  case FileType::THIN_AR:
    for (MappedFile<Context<E>> *child : read_archive_members(ctx, mf)) {
      switch (get_file_type(ctx, child)) {
      case FileType::ELF_OBJ:
        ctx.objs.push_back(new_object_file(ctx, child, mf->name));
        break;
      case FileType::GCC_LTO_OBJ:
      case FileType::LLVM_BITCODE:
        if (ObjectFile<E> *file = new_lto_obj(ctx, child, mf->name))
          ctx.objs.push_back(file);
        break;
      case FileType::ELF_DSO:
        Warn(ctx) << mf->name << "(" << child->name
                  << "): shared object file in an archive is ignored";
        break;
      default:
        break;
      }
    }
    ctx.visited.insert(mf->name);
    return;
  case FileType::TEXT:
    parse_linker_script(ctx, mf);
    return;
  case FileType::GCC_LTO_OBJ:
  case FileType::LLVM_BITCODE:
    if (ObjectFile<E> *file = new_lto_obj(ctx, mf, ""))
      ctx.objs.push_back(file);
    return;
  default:
    Fatal(ctx) << mf->name << ": unknown file type";
  }
}

template <typename E>
static std::string_view
deduce_machine_type(Context<E> &ctx, std::span<std::string> args) {
  for (std::string_view arg : args)
    if (!arg.starts_with('-'))
      if (auto *mf = MappedFile<Context<E>>::open(ctx, std::string(arg)))
        if (std::string_view target = get_machine_type(ctx, mf);
            !target.empty())
          return target;
  Fatal(ctx) << "-m option is missing";
}

template <typename E>
MappedFile<Context<E>> *open_library(Context<E> &ctx, std::string path) {
  MappedFile<Context<E>> *mf = MappedFile<Context<E>>::open(ctx, path);
  if (!mf)
    return nullptr;

  std::string_view target = get_machine_type(ctx, mf);
  if (target.empty() || target == E::target_name)
    return mf;
  Warn(ctx) << path << ": skipping incompatible file " << target
            << " " << (int)E::e_machine;
  return nullptr;
}

template <typename E>
MappedFile<Context<E>> *find_library(Context<E> &ctx, std::string name) {
  if (name.starts_with(':')) {
    for (std::string_view dir : ctx.arg.library_paths) {
      std::string path = std::string(dir) + "/" + name.substr(1);
      if (MappedFile<Context<E>> *mf = open_library(ctx, path))
        return mf;
    }
    Fatal(ctx) << "library not found: " << name;
  }

  for (std::string_view dir : ctx.arg.library_paths) {
    std::string stem = std::string(dir) + "/lib" + name;
    if (!ctx.is_static)
      if (MappedFile<Context<E>> *mf = open_library(ctx, stem + ".so"))
        return mf;
    if (MappedFile<Context<E>> *mf = open_library(ctx, stem + ".a"))
      return mf;
  }
  Fatal(ctx) << "library not found: " << name;
}

template <typename E>
MappedFile<Context<E>> *find_from_search_paths(Context<E> &ctx, std::string name) {
  if (MappedFile<Context<E>> *mf = MappedFile<Context<E>>::open(ctx, name))
    return mf;

  for (std::string_view dir : ctx.arg.library_paths)
    if (MappedFile<Context<E>> *mf =
        MappedFile<Context<E>>::open(ctx, std::string(dir) + "/" + name))
      return mf;
  return nullptr;
}

template <typename E>
static void read_input_files(Context<E> &ctx, std::span<std::string> args) {
  Timer t(ctx, "read_input_files");

  std::vector<std::tuple<bool, bool, bool, bool>> state;
  ctx.is_static = ctx.arg.is_static;

  while (!args.empty()) {
    std::string_view arg = args[0];
    args = args.subspan(1);

    if (arg == "--as-needed") {
      ctx.as_needed = true;
    } else if (arg == "--no-as-needed") {
      ctx.as_needed = false;
    } else if (arg == "--whole-archive") {
      ctx.whole_archive = true;
    } else if (arg == "--no-whole-archive") {
      ctx.whole_archive = false;
    } else if (arg == "--Bstatic") {
      ctx.is_static = true;
    } else if (arg == "--Bdynamic") {
      ctx.is_static = false;
    } else if (arg == "--start-lib") {
      ctx.in_lib = true;
    } else if (arg == "--end-lib") {
      ctx.in_lib = false;
    } else if (remove_prefix(arg, "--version-script=")) {
      MappedFile<Context<E>> *mf = find_from_search_paths(ctx, std::string(arg));
      if (!mf)
        Fatal(ctx) << "--version-script: file not found: " << arg;
      parse_version_script(ctx, mf);
    } else if (remove_prefix(arg, "--dynamic-list=")) {
      MappedFile<Context<E>> *mf = find_from_search_paths(ctx, std::string(arg));
      if (!mf)
        Fatal(ctx) << "--dynamic-list: file not found: " << arg;
      parse_dynamic_list(ctx, mf);
    } else if (remove_prefix(arg, "--export-dynamic-symbol=")) {
      if (arg == "*")
        ctx.default_version = VER_NDX_GLOBAL;
      else
        ctx.version_patterns.push_back({arg, "--export-dynamic-symbol",
                                        "global", VER_NDX_GLOBAL, false});
    } else if (remove_prefix(arg, "--export-dynamic-symbol-list=")) {
      MappedFile<Context<E>> *mf = find_from_search_paths(ctx, std::string(arg));
      if (!mf)
        Fatal(ctx) << "--export-dynamic-symbol-list: file not found: " << arg;
      parse_dynamic_list(ctx, mf);
    } else if (arg == "--push-state") {
      state.push_back({ctx.as_needed, ctx.whole_archive, ctx.is_static,
                       ctx.in_lib});
    } else if (arg == "--pop-state") {
      if (state.empty())
        Fatal(ctx) << "no state pushed before popping";
      std::tie(ctx.as_needed, ctx.whole_archive, ctx.is_static, ctx.in_lib) =
        state.back();
      state.pop_back();
    } else if (remove_prefix(arg, "-l")) {
      MappedFile<Context<E>> *mf = find_library(ctx, std::string(arg));
      mf->given_fullpath = false;
      read_file(ctx, mf);
    } else {
      read_file(ctx, MappedFile<Context<E>>::must_open(ctx, std::string(arg)));
    }
  }

  if (ctx.objs.empty())
    Fatal(ctx) << "no input files";

  ctx.tg.wait();
}

// Since elf_main is a template, we can't run it without a type parameter.
// We speculatively run elf_main with X86_64, and if the speculation was
// wrong, re-run it with an actual machine type.
template <typename E>
static int redo_main(int argc, char **argv, std::string_view target) {
  if (target == I386::target_name)
    return elf_main<I386>(argc, argv);
  if (target == ARM64::target_name)
    return elf_main<ARM64>(argc, argv);
  if (target == ARM32::target_name)
    return elf_main<ARM32>(argc, argv);
  if (target == RV64LE::target_name)
    return elf_main<RV64LE>(argc, argv);
  if (target == RV64BE::target_name)
    return elf_main<RV64BE>(argc, argv);
  if (target == RV32LE::target_name)
    return elf_main<RV32LE>(argc, argv);
  if (target == RV32BE::target_name)
    return elf_main<RV32BE>(argc, argv);
  if (target == PPC32::target_name)
    return elf_main<PPC32>(argc, argv);
  if (target == PPC64V1::target_name)
    return elf_main<PPC64V1>(argc, argv);
  if (target == PPC64V2::target_name)
    return elf_main<PPC64V2>(argc, argv);
  if (target == S390X::target_name)
    return elf_main<S390X>(argc, argv);
  if (target == SPARC64::target_name)
    return elf_main<SPARC64>(argc, argv);
  if (target == M68K::target_name)
    return elf_main<M68K>(argc, argv);
  if (target == SH4::target_name)
    return elf_main<SH4>(argc, argv);
  if (target == ALPHA::target_name)
    return elf_main<ALPHA>(argc, argv);
  unreachable();
}

template <typename E>
int elf_main(int argc, char **argv) {
  Context<E> ctx;

  // Process -run option first. process_run_subcommand() does not return.
  if (argc >= 2 && (argv[1] == "-run"sv || argv[1] == "--run"sv)) {
#if defined(_WIN32) || defined(__APPLE__)
    Fatal(ctx) << "-run is supported only on Unix";
#endif
    process_run_subcommand(ctx, argc, argv);
  }

  // Parse non-positional command line options
  ctx.cmdline_args = expand_response_files(ctx, argv);
  std::vector<std::string> file_args = parse_nonpositional_args(ctx);

  // If no -m option is given, deduce it from input files.
  if (ctx.arg.emulation.empty())
    ctx.arg.emulation = deduce_machine_type(ctx, file_args);

  // Redo if -m is not x86-64.
  if constexpr (is_x86_64<E>)
    if (ctx.arg.emulation != X86_64::target_name)
      return redo_main<E>(argc, argv, ctx.arg.emulation);

  Timer t_all(ctx, "all");

  install_signal_handler();

  if (!ctx.arg.directory.empty())
    if (chdir(ctx.arg.directory.c_str()) == -1)
      Fatal(ctx) << "chdir failed: " << ctx.arg.directory
                 << ": " << errno_string();

  // Fork a subprocess unless --no-fork is given.
  std::function<void()> on_complete;

#if !defined(_WIN32) && !defined(__APPLE__)
  if (ctx.arg.fork)
    on_complete = fork_child();
#endif

  acquire_global_lock(ctx);

  tbb::global_control tbb_cont(tbb::global_control::max_allowed_parallelism,
                               ctx.arg.thread_count);

  // Handle --wrap options if any.
  for (std::string_view name : ctx.arg.wrap)
    get_symbol(ctx, name)->is_wrapped = true;

  // Handle --retain-symbols-file options if any.
  if (ctx.arg.retain_symbols_file)
    for (std::string_view name : *ctx.arg.retain_symbols_file)
      get_symbol(ctx, name)->write_to_symtab = true;

  for (std::string_view arg : ctx.arg.trace_symbol)
    get_symbol(ctx, arg)->is_traced = true;

  // Parse input files
  read_input_files(ctx, file_args);

  // Uniquify shared object files by soname
  {
    std::unordered_set<std::string_view> seen;
    std::erase_if(ctx.dsos, [&](SharedFile<E> *file) {
      return !seen.insert(file->soname).second;
    });
  }

  Timer t_total(ctx, "total");
  Timer t_before_copy(ctx, "before_copy");

  // Apply -exclude-libs
  apply_exclude_libs(ctx);

  // Create a dummy file containing linker-synthesized symbols.
  if (!ctx.arg.relocatable)
    create_internal_file(ctx);

  // resolve_symbols is 4 things in 1 phase:
  //
  // - Determine the set of object files to extract from archives.
  // - Remove redundant COMDAT sections (e.g. duplicate inline functions).
  // - Finally, the actual symbol resolution.
  // - LTO, which requires preliminary symbol resolution before running
  //   and a follow-up re-resolution after the LTO objects are emitted.
  //
  // These passes have complex interactions, and unfortunately has to be
  // put together in a single phase.
  resolve_symbols(ctx);

  // "Kill" .eh_frame input sections after symbol resolution.
  kill_eh_frame_sections(ctx);

  // Resolve mergeable section pieces to merge them.
  resolve_section_pieces(ctx);

  // Handle --relocatable. Since the linker's behavior is quite different
  // from the normal one when the option is given, the logic is implemented
  // to a separate file.
  if (ctx.arg.relocatable) {
    combine_objects(ctx);
    return 0;
  }

  // Create .bss sections for common symbols.
  convert_common_symbols(ctx);

  // Apply version scripts.
  apply_version_script(ctx);

  // Parse symbol version suffixes (e.g. "foo@ver1").
  parse_symbol_version(ctx);

  // Set is_imported and is_exported bits for each symbol.
  compute_import_export(ctx);

  // Read address-significant section information.
  if (ctx.arg.icf && !ctx.arg.icf_all)
    mark_addrsig(ctx);

  // Garbage-collect unreachable sections.
  if (ctx.arg.gc_sections)
    gc_sections(ctx);

  // Merge identical read-only sections.
  if (ctx.arg.icf)
    icf_sections(ctx);

  // Compute sizes of sections containing mergeable strings.
  compute_merged_section_sizes(ctx);

  // Create linker-synthesized sections such as .got or .plt.
  create_synthetic_sections(ctx);

  // Make sure that there's no duplicate symbol
  if (!ctx.arg.allow_multiple_definition)
    check_duplicate_symbols(ctx);

  // Warn if symbols with different types are defined under the same name.
  check_symbol_types(ctx);

  if constexpr (is_ppc64v1<E>)
    ppc64v1_rewrite_opd(ctx);

  // Bin input sections into output sections.
  create_output_sections(ctx);

  // Add synthetic symbols such as __ehdr_start or __end.
  add_synthetic_symbols(ctx);

  // Beyond this point, no new files will be added to ctx.objs
  // or ctx.dsos.

  // Handle `-z cet-report`.
  if (ctx.arg.z_cet_report != CET_REPORT_NONE)
    check_cet_errors(ctx);

  // Handle `-z execstack-if-needed`.
  if (ctx.arg.z_execstack_if_needed)
    for (ObjectFile<E> *file : ctx.objs)
      if (file->needs_executable_stack)
        ctx.arg.z_execstack = true;

  // If we are linking a .so file, remaining undefined symbols does
  // not cause a linker error. Instead, they are treated as if they
  // were imported symbols.
  //
  // If we are linking an executable, weak undefs are converted to
  // weakly imported symbols so that they'll have another chance to be
  // resolved.
  claim_unresolved_symbols(ctx);

  // Beyond this point, no new symbols will be added to the result.

  // Handle --print-dependencies
  if (ctx.arg.print_dependencies)
    print_dependencies(ctx);

  // Handle -repro
  if (ctx.arg.repro)
    write_repro_file(ctx);

  // Handle --require-defined
  for (std::string_view name : ctx.arg.require_defined)
    if (!get_symbol(ctx, name)->file)
      Error(ctx) << "--require-defined: undefined symbol: " << name;

  // .init_array and .fini_array contents have to be sorted by
  // a special rule. Sort them.
  sort_init_fini(ctx);

  // Likewise, .ctors and .dtors have to be sorted. They are rare
  // because they are superceded by .init_array/.fini_array, though.
  sort_ctor_dtor(ctx);

  // Handle --shuffle-sections
  if (ctx.arg.shuffle_sections != SHUFFLE_SECTIONS_NONE)
    shuffle_sections(ctx);

  // Copy string referred by .dynamic to .dynstr.
  for (SharedFile<E> *file : ctx.dsos)
    ctx.dynstr->add_string(file->soname);
  for (std::string_view str : ctx.arg.auxiliary)
    ctx.dynstr->add_string(str);
  for (std::string_view str : ctx.arg.filter)
    ctx.dynstr->add_string(str);
  if (!ctx.arg.rpaths.empty())
    ctx.dynstr->add_string(ctx.arg.rpaths);
  if (!ctx.arg.soname.empty())
    ctx.dynstr->add_string(ctx.arg.soname);

  if constexpr (is_ppc64v1<E>)
    ppc64v1_scan_symbols(ctx);

  // Scan relocations to find symbols that need entries in .got, .plt,
  // .got.plt, .dynsym, .dynstr, etc.
  scan_relocations(ctx);

  // Compute sizes of output sections while assigning offsets
  // within an output section to input sections.
  compute_section_sizes(ctx);

  // Sort sections by section attributes so that we'll have to
  // create as few segments as possible.
  sort_output_sections(ctx);

  // If --packed_dyn_relocs=relr was given, base relocations are stored
  // to a .relr.dyn section in a compressed form. Construct a compressed
  // relocations now so that we can fix section sizes and file layout.
  if (ctx.arg.pack_dyn_relocs_relr)
    construct_relr(ctx);

  // Reserve a space for dynamic symbol strings in .dynstr and sort
  // .dynsym contents if necessary. Beyond this point, no symbol will
  // be added to .dynsym.
  ctx.dynsym->finalize(ctx);

  // Print reports about undefined symbols, if needed.
  if (ctx.arg.unresolved_symbols == UNRESOLVED_ERROR)
    report_undef_errors(ctx);

  // Fill .gnu.version_d section contents.
  if (ctx.verdef)
    ctx.verdef->construct(ctx);

  // Fill .gnu.version_r section contents.
  ctx.verneed->construct(ctx);

  // Compute .symtab and .strtab sizes for each file.
  create_output_symtab(ctx);

  // .eh_frame is a special section from the linker's point of view,
  // as its contents are parsed and reconstructed by the linker,
  // unlike other sections that are regarded as opaque bytes.
  // Here, we construct output .eh_frame contents.
  ctx.eh_frame->construct(ctx);

  // Handle --gdb-index.
  if (ctx.arg.gdb_index)
    ctx.gdb_index->construct(ctx);

  // If --emit-relocs is given, we'll copy relocation sections from input
  // files to an output file.
  if (ctx.arg.emit_relocs)
    create_reloc_sections(ctx);

  // Compute the section header values for all sections.
  compute_section_headers(ctx);

  // Assign offsets to output sections
  i64 filesize = set_osec_offsets(ctx);

  // On RISC-V, branches are encode using multiple instructions so
  // that they can jump to anywhere in ±2 GiB by default. They may
  // be replaced with shorter instruction sequences if destinations
  // are close enough. Do this optimization.
  if constexpr (is_riscv<E>)
    filesize = riscv_resize_sections(ctx);

  // At this point, memory layout is fixed.

  // Set actual addresses to linker-synthesized symbols.
  fix_synthetic_symbols(ctx);

  // Beyond this, you can assume that symbol addresses including their
  // GOT or PLT addresses have a correct final value.

  // If --compress-debug-sections is given, compress .debug_* sections
  // using zlib.
  if (ctx.arg.compress_debug_sections != COMPRESS_NONE)
    filesize = compress_debug_sections(ctx);

  // At this point, both memory and file layouts are fixed.

  t_before_copy.stop();

  // Create an output file
  ctx.output_file =
    OutputFile<Context<E>>::open(ctx, ctx.arg.output, filesize, 0777);
  ctx.buf = ctx.output_file->buf;

  Timer t_copy(ctx, "copy");

  // Copy input sections to the output file and apply relocations.
  copy_chunks(ctx);

  // Some part of .gdb_index couldn't be computed until other debug
  // sections are complete. We have complete debug sections now, so
  // write the rest of .gdb_index.
  if (ctx.gdb_index)
    ctx.gdb_index->write_address_areas(ctx);

  // Dynamic linker works better with sorted .rela.dyn section,
  // so we sort them.
  ctx.reldyn->sort(ctx);

  // Zero-clear paddings between sections
  clear_padding(ctx);

  // .note.gnu.build-id section contains a cryptographic hash of the
  // entire output file. Now that we wrote everything except build-id,
  // we can compute it.
  if (ctx.buildid)
    ctx.buildid->write_buildid(ctx);

  t_copy.stop();
  ctx.checkpoint();

  // Close the output file. This is the end of the linker's main job.
  ctx.output_file->close(ctx);

  // Handle --dependency-file
  if (!ctx.arg.dependency_file.empty())
    write_dependency_file(ctx);

  if (ctx.has_lto_object)
    lto_cleanup(ctx);

  t_total.stop();
  t_all.stop();

  if (ctx.arg.print_map)
    print_map(ctx);

  // Show stats numbers
  if (ctx.arg.stats)
    show_stats(ctx);

  if (ctx.arg.perf)
    print_timer_records(ctx.timer_records);

  std::cout << std::flush;
  std::cerr << std::flush;
  if (on_complete)
    on_complete();

  release_global_lock(ctx);

  if (ctx.arg.quick_exit)
    _exit(0);

  for (std::function<void()> &fn : ctx.on_exit)
    fn();
  ctx.checkpoint();
  return 0;
}

using E = MOLD_TARGET;

template void read_file(Context<E> &, MappedFile<Context<E>> *);
template MappedFile<Context<E>> *open_library(Context<E> &, std::string);

#ifdef MOLD_X86_64

extern template int elf_main<I386>(int, char **);
extern template int elf_main<ARM32>(int, char **);
extern template int elf_main<ARM64>(int, char **);
extern template int elf_main<RV32BE>(int, char **);
extern template int elf_main<RV32LE>(int, char **);
extern template int elf_main<RV64LE>(int, char **);
extern template int elf_main<RV64BE>(int, char **);
extern template int elf_main<PPC32>(int, char **);
extern template int elf_main<PPC64V1>(int, char **);
extern template int elf_main<PPC64V2>(int, char **);
extern template int elf_main<S390X>(int, char **);
extern template int elf_main<SPARC64>(int, char **);
extern template int elf_main<M68K>(int, char **);
extern template int elf_main<SH4>(int, char **);
extern template int elf_main<ALPHA>(int, char **);

int main(int argc, char **argv) {
  return elf_main<X86_64>(argc, argv);
}

#else

template int elf_main<E>(int, char **);

#endif

} // namespace mold::elf
