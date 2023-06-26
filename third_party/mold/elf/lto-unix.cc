// clang-format off
// This file handles the linker plugin to support LTO (Link-Time
// Optimization).
//
// LTO is a technique to do whole-program optimization to a program. Since
// a linker sees the whole program as opposed to a single compilation
// unit, it in theory can do some optimizations that cannot be done in the
// usual separate compilation model. For example, LTO should be able to
// inline functions that are defined in other compilation unit.
//
// In GCC and Clang, all you have to do to enable LTO is adding the
// `-flto` flag to the compiler and the linker command lines. If `-flto`
// is given, the compiler generates a file that contains not machine code
// but the compiler's IR (intermediate representation). In GCC, the output
// is an ELF file which wraps GCC's IR. In LLVM, it's not even an ELF file
// but just a raw LLVM IR file.
//
// Here is what we have to do if at least one input file is not a usual
// ELF file but an IR object file:
//
//  1. Read symbols both from usual ELF files and from IR object files and
//     resolve symbols as usual.
//
//  2. Pass all IR objects to the compiler backend. The compiler backend
//     compiles the IRs and returns a few big ELF object files as a
//     result.
//
//  3. Parse the returned ELF files and overwrite IR object symbols with
//     the returned ones, discarding IR object files.
//
//  4. Continue the rest of the linking process as usual.
//
// When gcc or clang inovkes ld, they pass `-plugin linker-plugin.so` to
// the linker. The given .so file provides a way to call the compiler
// backend.
//
// The linker plugin API is documented at
// https://gcc.gnu.org/wiki/whopr/driver, though the document is a bit
// outdated.
//
// Frankly, the linker plugin API is peculiar and is not very easy to use.
// For some reason, the API functions don't return the result of a
// function call as a return value but instead calls other function with
// the result as its argument to "return" the result.
//
// For example, the first thing you need to do after dlopen()'ing a linker
// plugin .so is to call `onload` function with a list of callback
// functions. `onload` calls callbacks to notify about the pointers to
// other functions the linker plugin provides. I don't know why `onload`
// can't just return a list of functions or why the linker plugin can't
// define not only `onload` but other functions, but that's what it is.
//
// Here is the steps to use the linker plugin:
//
//  1. dlopen() the linker plugin .so and call `onload` to obtain pointers
//     to other functions provided by the plugin.
//
//  2. Call `claim_file_hook` with an IR object file to read its symbol
//     table. `claim_file_hook` calls the `add_symbols` callback to
//     "return" a list of symbols.
//
//  3. `claim_file_hook` returns LDPT_OK only when the plugin wants to
//     handle a given file. Since we pass only IR object files to the
//     plugin in mold, it always returns LDPT_OK in our case.
//
//  4. Once we made a decision as to which object file to include into the
//     output file, we call `all_symbols_read_hook` to compile IR objects
//     into a few big ELF files. That function calls the `get_symbols`
//     callback to ask us about the symbol resolution results. (The
//     compiler backend needs to know whether an undefined symbol in an IR
//     object was resolved to a regular object file or a shared object to
//     do whole program optimization, for example.)
//
//  5. `all_symbols_read_hook` "returns" the result by calling the
//     `add_input_file` callback. The callback is called with a path to an
//     LTO'ed ELF file. We parse that ELF file and override symbols
//     defined by IR objects with the ELF file's ones.
//
//  6. Lastly, we call `cleanup_hook` to remove temporary files created by
//     the compiler backend.

#include "third_party/mold/elf/mold.h"
#include "third_party/mold/elf/lto.h"

#include "third_party/libcxx/cstdarg"
#include "third_party/libcxx/cstring"
#include "libc/runtime/dlfcn.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/flock.h"
#include "libc/calls/weirdtypes.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/posix.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/consts/splice.h"
#include "third_party/libcxx/sstream"
// MISSING #include <tbb/parallel_for_each.h>
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

#if 0
# define LOG std::cerr
#else
# define LOG std::ostringstream()
#endif

namespace mold::elf {

// Global variables
// We store LTO-related information to global variables,
// as the LTO plugin is not thread-safe by design anyway.

template <typename E> static Context<E> *gctx;
template <typename E> static std::vector<ObjectFile<E> *> lto_objects;

static int phase = 0;
static std::vector<PluginSymbol> plugin_symbols;
static ClaimFileHandler *claim_file_hook;
static AllSymbolsReadHandler *all_symbols_read_hook;
static CleanupHandler *cleanup_hook;
static bool is_gcc_linker_api_v1 = false;

// Event handlers

template <typename E>
static PluginStatus message(PluginLevel level, const char *fmt, ...) {
  LOG << "message\n";
  Context<E> &ctx = *gctx<E>;

  char buf[1000];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);

  switch (level) {
  case LDPL_INFO:
    SyncOut(ctx) << buf;
    break;
  case LDPL_WARNING:
    Warn(ctx) << buf;
    break;
  case LDPL_ERROR:
  case LDPL_FATAL:
    Fatal(ctx) << buf;
  }

  return LDPS_OK;
}

template <typename E>
static PluginStatus register_claim_file_hook(ClaimFileHandler fn) {
  LOG << "register_claim_file_hook\n";
  claim_file_hook = fn;
  return LDPS_OK;
}

template <typename E>
static PluginStatus
register_all_symbols_read_hook(AllSymbolsReadHandler fn) {
  LOG << "register_all_symbols_read_hook\n";
  all_symbols_read_hook = fn;
  return LDPS_OK;
}

template <typename E>
static PluginStatus register_cleanup_hook(CleanupHandler fn) {
  LOG << "register_cleanup_hook\n";
  cleanup_hook = fn;
  return LDPS_OK;
}

static PluginStatus
add_symbols(void *handle, int nsyms, const PluginSymbol *psyms) {
  LOG << "add_symbols: " << nsyms << "\n";
  assert(phase == 1);
  plugin_symbols = {psyms, psyms + nsyms};
  return LDPS_OK;
}

template <typename E>
static PluginStatus add_input_file(const char *path) {
  LOG << "add_input_file: " << path << "\n";

  Context<E> &ctx = *gctx<E>;
  static i64 file_priority = 100;

  MappedFile<Context<E>> *mf = MappedFile<Context<E>>::must_open(ctx, path);

  ObjectFile<E> *file = ObjectFile<E>::create(ctx, mf, "", false);
  ctx.obj_pool.emplace_back(file);
  lto_objects<E>.push_back(file);

  file->priority = file_priority++;
  file->is_alive = true;
  file->parse(ctx);
  file->resolve_symbols(ctx);
  return LDPS_OK;
}

static PluginStatus
get_input_file(const void *handle, struct PluginInputFile *file) {
  LOG << "get_input_file\n";
  return LDPS_OK;
}

template <typename E>
static PluginStatus release_input_file(const void *handle) {
  LOG << "release_input_file\n";

  ObjectFile<E> &file = *(ObjectFile<E> *)handle;
  if (file.mf->fd != -1) {
    close(file.mf->fd);
    file.mf->fd = -1;
  }
  return LDPS_OK;
}

static PluginStatus add_input_library(const char *path) {
  LOG << "add_input_library\n";
  return LDPS_OK;
}

static PluginStatus set_extra_library_path(const char *path) {
  LOG << "set_extra_library_path\n";
  return LDPS_OK;
}

template <typename E>
static PluginStatus get_view(const void *handle, const void **view) {
  LOG << "get_view\n";

  ObjectFile<E> &file = *(ObjectFile<E> *)handle;
  *view = (void *)file.mf->data;
  return LDPS_OK;
}

static PluginStatus
get_input_section_count(const void *handle, int *count) {
  LOG << "get_input_section_count\n";
  return LDPS_OK;
}

static PluginStatus
get_input_section_type(const PluginSection section, int *type) {
  LOG << "get_input_section_type\n";
  return LDPS_OK;
}

static PluginStatus
get_input_section_name(const PluginSection section,
                       char **section_name) {
  LOG << "get_input_section_name\n";
  return LDPS_OK;
}

static PluginStatus
get_input_section_contents(const PluginSection section,
                           const char **section_contents,
                           size_t *len) {
  LOG << "get_input_section_contents\n";
  return LDPS_OK;
}

static PluginStatus
update_section_order(const PluginSection *section_list,
                     int num_sections) {
  LOG << "update_section_order\n";
  return LDPS_OK;
}

static PluginStatus allow_section_ordering() {
  LOG << "allow_section_ordering\n";
  return LDPS_OK;
}

static PluginStatus
get_symbols_v1(const void *handle, int nsyms, PluginSymbol *psyms) {
  unreachable();
}

// get_symbols teaches the LTO plugin as to how we have resolved symbols.
// The plugin uses the symbol resolution info to optimize the program.
//
// For example, if a definition in an IR file is not referenced by
// non-IR objects at all, the plugin may choose to completely inline
// that definition within the IR objects and remove the symbol from the
// LTO result. On the other hand, if a definition is referenced by a
// non-IR object, it has to keep the symbol in the LTO result.
template <typename E>
static PluginStatus
get_symbols(const void *handle, int nsyms, PluginSymbol *psyms, bool is_v2) {
  ObjectFile<E> &file = *(ObjectFile<E> *)handle;
  assert(file.is_lto_obj);

  // If file is an archive member which was not chose to be included in
  // to the final result, we need to make the plugin to ignore all
  // symbols.
  if (!file.is_alive) {
    assert(!is_v2);
    for (int i = 0; i < nsyms; i++)
      psyms[i].resolution = LDPR_PREEMPTED_REG;
    return LDPS_NO_SYMS;
  }

  auto get_resolution = [&](ElfSym<E> &esym, Symbol<E> &sym) {
    if (!sym.file)
      return LDPR_UNDEF;

    if (sym.file == &file) {
      if (sym.referenced_by_regular_obj)
        return LDPR_PREVAILING_DEF;
      if (sym.is_exported)
        return is_v2 ? LDPR_PREVAILING_DEF : LDPR_PREVAILING_DEF_IRONLY_EXP;
      return LDPR_PREVAILING_DEF_IRONLY;
    }

    if (sym.file->is_dso)
      return LDPR_RESOLVED_DYN;

    if (((ObjectFile<E> *)sym.file)->is_lto_obj && !sym.is_wrapped)
      return esym.is_undef() ? LDPR_RESOLVED_IR : LDPR_PREEMPTED_IR;
    return esym.is_undef() ? LDPR_RESOLVED_EXEC : LDPR_PREEMPTED_REG;
  };

  // Set the symbol resolution results to psyms.
  for (i64 i = 0; i < nsyms; i++) {
    ElfSym<E> &esym = file.elf_syms[i + 1];
    Symbol<E> &sym = *file.symbols[i + 1];
    psyms[i].resolution = get_resolution(esym, sym);
  }
  return LDPS_OK;
}

// This function restarts mold itself with `--:lto-pass2` and
// `--:ignore-ir-file` flags. We do this as a workaround for the old
// linker plugins that do not support the get_symbols_v3 API.
//
// get_symbols_v1 and get_symbols_v2 don't provide a way to ignore an
// object file we previously passed to the linker plugin. So we can't
// "unload" object files in archives that we ended up not choosing to
// include into the final output.
//
// As a workaround, we restart the linker with a list of object files
// the linker has to ignore, so that it won't read the object files
// from archives next time.
//
// This is an ugly hack and should be removed once GCC adopts the v3 API.
template <typename E>
static void restart_process(Context<E> &ctx) {
  std::vector<const char *> args;

  for (std::string_view arg : ctx.cmdline_args)
    args.push_back(strdup(std::string(arg).c_str()));

  for (std::unique_ptr<ObjectFile<E>> &file : ctx.obj_pool)
    if (file->is_lto_obj && !file->is_alive)
      args.push_back(strdup(("--:ignore-ir-file=" +
                             file->mf->get_identifier()).c_str()));

  args.push_back("--:lto-pass2");
  args.push_back(nullptr);

  std::cout << std::flush;
  std::cerr << std::flush;

  std::string self = get_self_path();
  execv(self.c_str(), (char * const *)args.data());
  std::cerr << "execv failed: " << errno_string() << "\n";
  _exit(1);
}

template <typename E>
static PluginStatus
get_symbols_v2(const void *handle, int nsyms, PluginSymbol *psyms) {
  LOG << "get_symbols_v2\n";
  return get_symbols<E>(handle, nsyms, psyms, true);
}

template <typename E>
static PluginStatus
get_symbols_v3(const void *handle, int nsyms, PluginSymbol *psyms) {
  LOG << "get_symbols_v3\n";
  return get_symbols<E>(handle, nsyms, psyms, false);
}

static PluginStatus allow_unique_segment_for_sections() {
  LOG << "allow_unique_segment_for_sections\n";
  return LDPS_OK;
}

static PluginStatus
unique_segment_for_sections(const char *segment_name,
                            uint64_t flags,
                            uint64_t align,
                            const PluginSection *section_list,
                            int num_sections) {
  LOG << "unique_segment_for_sections\n";
  return LDPS_OK;
}

static PluginStatus
get_input_section_alignment(const PluginSection section,
                            int *addralign) {
  LOG << "get_input_section_alignment\n";
  return LDPS_OK;
}

static PluginStatus
get_input_section_size(const PluginSection section, uint64_t *size) {
  LOG << "get_input_section_size\n";
  return LDPS_OK;
}

template <typename E>
static PluginStatus
register_new_input_hook(NewInputHandler fn) {
  LOG << "register_new_input_hook\n";
  return LDPS_OK;
}

static PluginStatus
get_wrap_symbols(uint64_t *num_symbols, const char ***wrap_symbols) {
  LOG << "get_wrap_symbols\n";
  return LDPS_OK;
}

template <typename E>
static PluginLinkerAPIVersion
get_api_version(const char *plugin_identifier,
                unsigned plugin_version,
                int minimal_api_supported,
                int maximal_api_supported,
                const char **linker_identifier,
                const char **linker_version) {
  if (LAPI_V1 < minimal_api_supported)
    Fatal(*gctx<E>) << "LTO plugin does not support V0 or V1 API";

  std::string version = mold_version + "\0"s;

  *linker_identifier = "mold";
  *linker_version = version.data();

  if (LAPI_V1 <= maximal_api_supported) {
    is_gcc_linker_api_v1 = true;
    return LAPI_V1;
  }
  return LAPI_V0;
}

template <typename E>
static void load_plugin(Context<E> &ctx) {
  assert(phase == 0);
  phase = 1;
  gctx<E> = &ctx;

  void *handle = dlopen(ctx.arg.plugin.c_str(), RTLD_NOW | RTLD_GLOBAL);
  if (!handle)
    Fatal(ctx) << "could not open plugin file: " << dlerror();

  OnloadFn *onload = (OnloadFn *)dlsym(handle, "onload");
  if (!onload)
    Fatal(ctx) << "failed to load plugin " << ctx.arg.plugin << ": "
               << dlerror();

  auto save = [&](std::string_view str) {
    return save_string(ctx, std::string(str).c_str()).data();
  };

  std::vector<PluginTagValue> tv;
  tv.emplace_back(LDPT_MESSAGE, message<E>);

  if (ctx.arg.shared)
    tv.emplace_back(LDPT_LINKER_OUTPUT, LDPO_DYN);
  else if (ctx.arg.pie)
    tv.emplace_back(LDPT_LINKER_OUTPUT, LDPO_PIE);
  else
    tv.emplace_back(LDPT_LINKER_OUTPUT, LDPO_EXEC);

  for (std::string_view opt : ctx.arg.plugin_opt)
    tv.emplace_back(LDPT_OPTION, save(opt));

  tv.emplace_back(LDPT_REGISTER_CLAIM_FILE_HOOK, register_claim_file_hook<E>);
  tv.emplace_back(LDPT_REGISTER_ALL_SYMBOLS_READ_HOOK,
                  register_all_symbols_read_hook<E>);
  tv.emplace_back(LDPT_REGISTER_CLEANUP_HOOK, register_cleanup_hook<E>);
  tv.emplace_back(LDPT_ADD_SYMBOLS, add_symbols);
  tv.emplace_back(LDPT_GET_SYMBOLS, get_symbols_v1);
  tv.emplace_back(LDPT_ADD_INPUT_FILE, add_input_file<E>);
  tv.emplace_back(LDPT_GET_INPUT_FILE, get_input_file);
  tv.emplace_back(LDPT_RELEASE_INPUT_FILE, release_input_file<E>);
  tv.emplace_back(LDPT_ADD_INPUT_LIBRARY, add_input_library);
  tv.emplace_back(LDPT_OUTPUT_NAME, save(ctx.arg.output));
  tv.emplace_back(LDPT_SET_EXTRA_LIBRARY_PATH, set_extra_library_path);
  tv.emplace_back(LDPT_GET_VIEW, get_view<E>);
  tv.emplace_back(LDPT_GET_INPUT_SECTION_COUNT, get_input_section_count);
  tv.emplace_back(LDPT_GET_INPUT_SECTION_TYPE, get_input_section_type);
  tv.emplace_back(LDPT_GET_INPUT_SECTION_NAME, get_input_section_name);
  tv.emplace_back(LDPT_GET_INPUT_SECTION_CONTENTS, get_input_section_contents);
  tv.emplace_back(LDPT_UPDATE_SECTION_ORDER, update_section_order);
  tv.emplace_back(LDPT_ALLOW_SECTION_ORDERING, allow_section_ordering);
  tv.emplace_back(LDPT_ADD_SYMBOLS_V2, add_symbols);
  tv.emplace_back(LDPT_GET_SYMBOLS_V2, get_symbols_v2<E>);
  tv.emplace_back(LDPT_ALLOW_UNIQUE_SEGMENT_FOR_SECTIONS,
                  allow_unique_segment_for_sections);
  tv.emplace_back(LDPT_UNIQUE_SEGMENT_FOR_SECTIONS, unique_segment_for_sections);
  tv.emplace_back(LDPT_GET_SYMBOLS_V3, get_symbols_v3<E>);
  tv.emplace_back(LDPT_GET_INPUT_SECTION_ALIGNMENT, get_input_section_alignment);
  tv.emplace_back(LDPT_GET_INPUT_SECTION_SIZE, get_input_section_size);
  tv.emplace_back(LDPT_REGISTER_NEW_INPUT_HOOK, register_new_input_hook<E>);
  tv.emplace_back(LDPT_GET_WRAP_SYMBOLS, get_wrap_symbols);
  tv.emplace_back(LDPT_GET_API_VERSION, get_api_version<E>);
  tv.emplace_back(LDPT_NULL, 0);

  [[maybe_unused]] PluginStatus status = onload(tv.data());
  assert(status == LDPS_OK);
}

template <typename E>
static ElfSym<E> to_elf_sym(PluginSymbol &psym) {
  ElfSym<E> esym;
  memset(&esym, 0, sizeof(esym));

  switch (psym.def) {
  case LDPK_DEF:
    esym.st_shndx = SHN_ABS;
    break;
  case LDPK_WEAKDEF:
    esym.st_shndx = SHN_ABS;
    esym.st_bind = STB_WEAK;
    break;
  case LDPK_UNDEF:
    esym.st_shndx = SHN_UNDEF;
    break;
  case LDPK_WEAKUNDEF:
    esym.st_shndx = SHN_UNDEF;
    esym.st_bind = STB_WEAK;
    break;
  case LDPK_COMMON:
    esym.st_shndx = SHN_COMMON;
    break;
  }

  switch (psym.symbol_type) {
  case LDST_UNKNOWN:
    break;
  case LDST_FUNCTION:
    esym.st_type = STT_FUNC;
    break;
  case LDST_VARIABLE:
    esym.st_type = STT_OBJECT;
    break;
  };

  switch (psym.visibility) {
  case LDPV_DEFAULT:
    break;
  case LDPV_PROTECTED:
    esym.st_visibility = STV_PROTECTED;
    break;
  case LDPV_INTERNAL:
    esym.st_visibility = STV_INTERNAL;
    break;
  case LDPV_HIDDEN:
    esym.st_visibility = STV_HIDDEN;
    break;
  }

  esym.st_size = psym.size;
  return esym;
}

// Returns true if a given linker plugin looks like LLVM's one.
// Returns false if it's GCC.
template <typename E>
static bool is_llvm(Context<E> &ctx) {
  return ctx.arg.plugin.ends_with("LLVMgold.so");
}

// Returns true if a given linker plugin supports the get_symbols_v3 API.
// Any version of LLVM and GCC 12 or newer support it.
template <typename E>
static bool supports_v3_api(Context<E> &ctx) {
  return is_gcc_linker_api_v1 || is_llvm(ctx);
}

template <typename E>
ObjectFile<E> *read_lto_object(Context<E> &ctx, MappedFile<Context<E>> *mf) {
  // V0 API's claim_file is not thread-safe.
  static std::mutex mu;
  std::unique_lock lock(mu, std::defer_lock);
  if (!is_gcc_linker_api_v1)
    lock.lock();

  if (ctx.arg.plugin.empty())
    Fatal(ctx) << mf->name << ": don't know how to handle this LTO object file "
               << "because no -plugin option was given. Please make sure you "
               << "added -flto not only for creating object files but also for "
               << "creating the final executable.";

  // dlopen the linker plugin file
  static std::once_flag flag;
  std::call_once(flag, [&] { load_plugin(ctx); });

  // Create mold's object instance
  ObjectFile<E> *obj = new ObjectFile<E>;
  ctx.obj_pool.emplace_back(obj);

  obj->filename = mf->name;
  obj->symbols.push_back(new Symbol<E>);
  obj->first_global = 1;
  obj->is_lto_obj = true;
  obj->mf = mf;

  // Create plugin's object instance
  PluginInputFile file = {};

  MappedFile<Context<E>> *mf2 = mf->parent ? mf->parent : mf;
  file.name = save_string(ctx, mf2->name).data();
  if (mf2->fd == -1)
    mf2->fd = open(file.name, O_RDONLY);
  file.fd = mf2->fd;
  if (file.fd == -1)
    Fatal(ctx) << "cannot open " << file.name << ": " << errno_string();

  if (mf->parent)
    obj->archive_name = mf->parent->name;

  file.offset = mf->get_offset();
  file.filesize = mf->size;
  file.handle = (void *)obj;

  LOG << "read_lto_symbols: "<< mf->name << "\n";

  // claim_file_hook() calls add_symbols() which initializes `plugin_symbols`
  int claimed = false;
  claim_file_hook(&file, &claimed);
  if (!claimed)
    Fatal(ctx) << mf->name << ": not claimed by the LTO plugin;"
               << " please make sure you are using the same compiler of the"
               << " same version for all object files";

  // It looks like GCC doesn't need fd after claim_file_hook() while
  // LLVM needs it and takes the ownership of fd. To prevent "too many
  // open files" issue, we close fd only for GCC. This is ugly, though.
  if (!is_llvm(ctx)) {
    close(mf2->fd);
    mf2->fd = -1;
  }

  // Initialize object symbols
  std::vector<ElfSym<E>> *esyms = new std::vector<ElfSym<E>>(1);

  for (PluginSymbol &psym : plugin_symbols) {
    esyms->push_back(to_elf_sym<E>(psym));
    obj->symbols.push_back(get_symbol(ctx, save_string(ctx, psym.name)));
  }

  obj->elf_syms = *esyms;
  obj->has_symver.resize(esyms->size());
  plugin_symbols.clear();
  return obj;
}

// Entry point
template <typename E>
std::vector<ObjectFile<E> *> do_lto(Context<E> &ctx) {
  Timer t(ctx, "do_lto");

  if (!ctx.arg.lto_pass2 && !supports_v3_api(ctx))
    restart_process(ctx);

  assert(phase == 1);
  phase = 2;

  // Set `referenced_by_regular_obj` bit.
  tbb::parallel_for_each(ctx.objs, [&](ObjectFile<E> *file) {
    if (file->is_lto_obj)
      return;

    for (i64 i = file->first_global; i < file->symbols.size(); i++) {
      Symbol<E> &sym = *file->symbols[i];

      if (sym.file && !sym.file->is_dso &&
          ((ObjectFile<E> *)sym.file)->is_lto_obj) {
        std::scoped_lock lock(sym.mu);
        sym.referenced_by_regular_obj = true;
      }
    }
  });

  // Symbols specified by the --wrap option needs to be visible from
  // regular object files.
  for (std::string_view name : ctx.arg.wrap) {
    get_symbol(ctx, name)->referenced_by_regular_obj = true;

    std::string_view x = save_string(ctx, "__wrap_" + std::string(name));
    std::string_view y = save_string(ctx, "__real_" + std::string(name));

    get_symbol(ctx, x)->referenced_by_regular_obj = true;
    get_symbol(ctx, y)->referenced_by_regular_obj = true;
  }

  // all_symbols_read_hook() calls add_input_file() and add_input_library()
  LOG << "all symbols read\n";
  if (PluginStatus st = all_symbols_read_hook(); st != LDPS_OK)
    Fatal(ctx) << "LTO: all_symbols_read_hook returns " << st;

  return lto_objects<E>;
}

template <typename E>
void lto_cleanup(Context<E> &ctx) {
  Timer t(ctx, "lto_cleanup");

  if (cleanup_hook)
    cleanup_hook();
}

using E = MOLD_TARGET;

template ObjectFile<E> *read_lto_object(Context<E> &, MappedFile<Context<E>> *);
template std::vector<ObjectFile<E> *> do_lto(Context<E> &);
template void lto_cleanup(Context<E> &);

} // namespace mold::elf
