// clang-format off
#include "third_party/mold/elf/mold.h"
#include "third_party/mold/cmdline.h"

#include "third_party/libcxx/regex"
#include "third_party/libcxx/sstream"
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
#include "third_party/libcxx/unordered_set"

#ifdef _WIN32
# define _isatty isatty
# define STDERR_FILENO (_fileno(stderr))
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
#include "third_party/musl/crypt.h"
#include "third_party/musl/lockf.h"
#endif

namespace mold::elf {

inline const char helpmsg[] = R"(
Options:
  --help                      Report usage information
  -v, --version               Report version information
  -V                          Report version and target information
  -(, --start-group           Ignored
  -), --end-group             Ignored
  -C DIR, --directory DIR     Change to DIR before doing anything
  -E, --export-dynamic        Put symbols in the dynamic symbol table
    --no-export-dynamic
  -F LIBNAME, --filter LIBNAME
                              Set DT_FILTER to the specified value
  -I FILE, --dynamic-linker FILE
                              Set dynamic linker path
    --no-dynamic-linker
  -L DIR, --library-path DIR  Add DIR to library search path
  -M, --print-map             Write map file to stdout
  -N, --omagic                Do not page align data, do not make text readonly
    --no-omagic
  -O NUMBER                   Ignored
  -S, --strip-debug           Strip .debug_* sections
  -T FILE, --script FILE      Read linker script
  -X, --discard-locals        Discard temporary local symbols
  -e SYMBOL, --entry SYMBOL   Set program entry point
  -f SHLIB, --auxiliary SHLIB Set DT_AUXILIARY to the specified value
  -h LIBNAME, --soname LIBNAME
                              Set shared library name
  -l LIBNAME                  Search for a given library
  -m TARGET                   Set target
  -o FILE, --output FILE      Set output filename
  -q, --emit-relocs           Leaves relocation sections in the output
  -r, --relocatable           Generate relocatable output
  -s, --strip-all             Strip .symtab section
  -u SYMBOL, --undefined SYMBOL
                              Force to resolve SYMBOL
  --Bdynamic, --dy            Link against shared libraries (default)
  --Bstatic, --dn, --static   Do not link against shared libraries
  --Bsymbolic                 Bind global symbols locally
  --Bsymbolic-functions       Bind global functions locally
  --Bno-symbolic              Cancel --Bsymbolic and --Bsymbolic-functions
  --Map FILE                  Write map file to a given file
  --Tbss=ADDR                 Set address to .bss
  --Tdata                     Set address to .data
  --Ttext                     Set address to .text
  --allow-multiple-definition Allow multiple definitions
  --apply-dynamic-relocs      Apply link-time values for dynamic relocations (defualt)
    --no-apply-dynamic-relocs
  --as-needed                 Only set DT_NEEDED if used
    --no-as-needed
  --build-id [none,md5,sha1,sha256,uuid,HEXSTRING]
                              Generate build ID
    --no-build-id
  --chroot DIR                Set a given path to root directory
  --color-diagnostics=[auto,always,never]
                              Use colors in diagnostics
  --color-diagnostics         Alias for --color-diagnostics=always
  --compress-debug-sections [none,zlib,zlib-gabi,zstd]
                              Compress .debug_* sections
  --dc                        Ignored
  --dependency-file=FILE      Write Makefile-style dependency rules to FILE
  --defsym=SYMBOL=VALUE       Define a symbol alias
  --demangle                  Demangle C++ symbols in log messages (default)
    --no-demangle
  --enable-new-dtags          Emit DT_RUNPATH for --rpath (default)
    --disable-new-dtags       Emit DT_RPATH for --rpath
  --execute-only              Make executable segments unreadable
  --dp                        Ignored
  --dynamic-list              Read a list of dynamic symbols (implies -Bsymbolic)
  --eh-frame-hdr              Create .eh_frame_hdr section
    --no-eh-frame-hdr
  --exclude-libs LIB,LIB,..   Mark all symbols in given libraries hidden
  --export-dynamic-symbol     Put symbols matching glob in the dynamic symbol table
  --export-dynamic-symbol-list
                              Read a list of dynamic symbols
  --fatal-warnings            Treat warnings as errors
    --no-fatal-warnings       Do not treat warnings as errors (default)
  --fini SYMBOL               Call SYMBOL at unload-time
  --fork                      Spawn a child process (default)
    --no-fork
  --gc-sections               Remove unreferenced sections
    --no-gc-sections
  --gdb-index                 Create .gdb_index for faster gdb startup
  --hash-style [sysv,gnu,both,none]
                              Set hash style
  --icf=[all,safe,none]       Fold identical code
    --no-icf
  --ignore-data-address-equality
                              Allow merging non-executable sections with --icf
  --image-base ADDR           Set the base address to a given value
  --init SYMBOL               Call SYMBOL at load-time
  --no-undefined              Report undefined symbols (even with --shared)
  --noinhibit-exec            Create an output file even if errors occur
  --oformat=binary            Omit ELF, section and program headers
  --pack-dyn-relocs=[relr,none]
                              Pack dynamic relocations
  --package-metadata=STRING   Set a given string to .note.package
  --perf                      Print performance statistics
  --pie, --pic-executable     Create a position independent executable
    --no-pie, --no-pic-executable
  --pop-state                 Restore state of flags governing input file handling
  --print-gc-sections         Print removed unreferenced sections
    --no-print-gc-sections
  --print-icf-sections        Print folded identical sections
    --no-print-icf-sections
  --push-state                Save state of flags governing input file handling
  --quick-exit                Use quick_exit to exit (default)
    --no-quick-exit
  --relax                     Optimize instructions (default)
    --no-relax
  --repro                     Embed input files to .repro section
  --require-defined SYMBOL    Require SYMBOL be defined in the final output
  --retain-symbols-file FILE  Keep only symbols listed in FILE
  --reverse-sections          Reverses input sections in the output file
  --rosegment                 Put read-only non-executable sections in their own segment (default)
    --no-rosegment            Put read-only non-executable sections in an executable segment
  --rpath DIR                 Add DIR to runtime search path
  --rpath-link DIR            Ignored
  --run COMMAND ARG...        Run COMMAND with mold as /usr/bin/ld
  --section-start=SECTION=ADDR Set address to section
  --shared, --Bshareable      Create a share library
  --shuffle-sections[=SEED]   Randomize the output by shuffling input sections
  --sort-common               Ignored
  --sort-section              Ignored
  --spare-dynamic-tags NUMBER Reserve give number of tags in .dynamic section
  --start-lib                 Give following object files in-archive-file semantics
    --end-lib                 End the effect of --start-lib
  --stats                     Print input statistics
  --sysroot DIR               Set target system root directory
  --thread-count COUNT, --threads=COUNT
                              Use COUNT number of threads
  --threads                   Use multiple threads (default)
    --no-threads
  --trace                     Print name of each input file
  --undefined-version         Do not report version scripts that refer undefined symbols
    --no-undefined-version    Report version scripts that refer undefined symbols (default)
  --unique PATTERN            Don't merge input sections that match a given pattern
  --unresolved-symbols [report-all,ignore-all,ignore-in-object-files,ignore-in-shared-libs]
                              How to handle unresolved symbols
  --version-script FILE       Read version script
  --warn-common               Warn about common symbols
    --no-warn-common
  --warn-once                 Only warn once for each undefined symbol
  --warn-shared-textrel       Warn if the output .so needs text relocations
  --warn-textrel              Warn if the output file needs text relocations
  --warn-unresolved-symbols   Report unresolved symbols as warnings
    --error-unresolved-symbols
                              Report unresolved symbols as errors (default)
  --whole-archive             Include all objects from static archives
    --no-whole-archive
  --wrap SYMBOL               Use wrapper function for a given symbol
  -z defs                     Report undefined symbols (even with --shared)
    -z nodefs
  -z common-page-size=VALUE   Ignored
  -z execstack                Require executable stack
    -z noexecstack
  -z execstack-if-needed      Make the stack area execuable if an input file explicitly requests it
  -z initfirst                Mark DSO to be initialized first at runtime
  -z interpose                Mark object to interpose all DSOs but executable
  -z keep-text-section-prefix Keep .text.{hot,unknown,unlikely,startup,exit} as separate sections in the final binary
    -z nokeep-text-section-prefix
  -z lazy                     Enable lazy function resolution (default)
  -z max-page-size=VALUE      Use VALUE as the memory page size
  -z nocopyreloc              Do not create copy relocations
  -z nodefaultlib             Make the dynamic loader to ignore default search paths
  -z nodelete                 Mark DSO non-deletable at runtime
  -z nodlopen                 Mark DSO not available to dlopen
  -z nodump                   Mark DSO not available to dldump
  -z now                      Disable lazy function resolution
  -z origin                   Mark object requiring immediate $ORIGIN processing at runtime
  -z pack-relative-relocs     Alias for --pack-dyn-relocs=relr
    -z nopack-relative-relocs
  -z separate-loadable-segments
                              Separate all loadable segments to different pages
    -z separate-code          Separate code and data into different pages
    -z noseparate-code        Allow overlap in pages
  -z relro                    Make some sections read-only after relocation (default)
    -z norelro
  -z text                     Report error if DT_TEXTREL is set
    -z notext
    -z textoff

mold: supported targets: elf32-i386 elf64-x86-64 elf32-littlearm elf64-littleaarch64 elf32-littleriscv elf32-bigriscv elf64-littleriscv elf64-bigriscv elf32-powerpc elf64-powerpc elf64-powerpc elf64-powerpcle elf64-s390 elf64-sparc elf32-m68k elf32-sh-linux elf64-alpha
mold: supported emulations: elf_i386 elf_x86_64 armelf_linux_eabi aarch64linux aarch64elf elf32lriscv elf32briscv elf64lriscv elf64briscv elf32ppc elf32ppclinux elf64ppc elf64lppc elf64_s390 elf64_sparc m68kelf shlelf_linux elf64alpha)";

static std::vector<std::string> add_dashes(std::string name) {
  // Single-letter option
  if (name.size() == 1)
    return {"-" + name};

  // Multi-letter linker options can be preceded by either a single
  // dash or double dashes except ones starting with "o", which must
  // be preceded by double dashes. For example, "-omagic" is
  // interpreted as "-o magic". If you really want to specify the
  // "omagic" option, you have to pass "--omagic".
  if (name[0] == 'o')
    return {"--" + name};
  return {"-" + name, "--" + name};
}

template <typename E>
static i64 parse_hex(Context<E> &ctx, std::string opt, std::string_view value) {
  auto flags = std::regex_constants::optimize | std::regex_constants::ECMAScript;
  static std::regex re(R"((?:0x|0X)?([0-9a-fA-F]+))", flags);

  std::cmatch m;
  if (!std::regex_match(value.data(), value.data() + value.size(), m, re))
    Fatal(ctx) << "option -" << opt << ": not a hexadecimal number";
  return std::stoul(m[1], nullptr, 16);
}

template <typename E>
static i64 parse_number(Context<E> &ctx, std::string opt,
                        std::string_view value) {
  size_t nread;

  if (value.starts_with('-')) {
    i64 ret = std::stoul(std::string(value.substr(1)), &nread, 0);
    if (value.size() - 1 != nread)
      Fatal(ctx) << "option -" << opt << ": not a number: " << value;
    return -ret;
  }

  i64 ret = std::stoul(std::string(value), &nread, 0);
  if (value.size() != nread)
    Fatal(ctx) << "option -" << opt << ": not a number: " << value;
  return ret;
}

template <typename E>
static std::vector<u8> parse_hex_build_id(Context<E> &ctx, std::string_view arg) {
  auto flags = std::regex_constants::optimize | std::regex_constants::ECMAScript;
  static std::regex re(R"(0[xX]([0-9a-fA-F][0-9a-fA-F])+)", flags);

  if (!std::regex_match(arg.begin(), arg.end(), re))
    Fatal(ctx) << "invalid build-id: " << arg;

  arg = arg.substr(2);

  auto fn = [](char c) {
    if ('0' <= c && c <= '9')
      return c - '0';
    if ('a' <= c && c <= 'f')
      return c - 'a' + 10;
    assert('A' <= c && c <= 'F');
    return c - 'A' + 10;
  };

  std::vector<u8> vec;
  for (i64 i = 0; i < arg.size(); i += 2)
    vec.push_back((fn(arg[i]) << 4) | fn(arg[i + 1]));
  return vec;
}

static std::vector<std::string_view>
split_by_comma_or_colon(std::string_view str) {
  std::vector<std::string_view> vec;

  for (;;) {
    i64 pos = str.find_first_of(",:");
    if (pos == str.npos) {
      vec.push_back(str);
      break;
    }
    vec.push_back(str.substr(0, pos));
    str = str.substr(pos);
  }
  return vec;
}

template <typename E>
static void read_retain_symbols_file(Context<E> &ctx, std::string_view path) {
  MappedFile<Context<E>> *mf =
    MappedFile<Context<E>>::must_open(ctx, std::string(path));
  std::string_view data((char *)mf->data, mf->size);

  ctx.arg.retain_symbols_file.reset(new std::unordered_set<std::string_view>);

  while (!data.empty()) {
    size_t pos = data.find('\n');
    std::string_view name;

    if (pos == data.npos) {
      name = data;
      data = "";
    } else {
      name = data.substr(0, pos);
      data = data.substr(pos + 1);
    }

    name = string_trim(name);
    if (!name.empty())
      ctx.arg.retain_symbols_file->insert(name);
  }
}

static bool is_file(std::string_view path) {
  struct stat st;
  return stat(std::string(path).c_str(), &st) == 0 &&
         (st.st_mode & S_IFMT) != S_IFDIR;
}

template <typename E>
static std::vector<SectionOrder>
parse_section_order(Context<E> &ctx, std::string_view arg) {
  auto flags = std::regex_constants::ECMAScript | std::regex_constants::icase |
               std::regex_constants::optimize;
  static std::regex re1(R"(^\s*(TEXT|DATA|RODATA|BSS)(?:\s|$))", flags);
  static std::regex re2(R"(^\s*([a-zA-Z0-9_.][^\s]*|EHDR|PHDR)(?:\s|$))", flags);
  static std::regex re3(R"(^\s*=(0x[0-9a-f]+|\d+)(?:\s|$))", flags);
  static std::regex re4(R"(^\s*%(0x[0-9a-f]+|\d*)(?:\s|$))", flags);
  static std::regex re5(R"(^\s*!(\S+)(?:\s|$))", flags);

  std::vector<SectionOrder> vec;
  arg = string_trim(arg);

  while (!arg.empty()) {
    SectionOrder ord;
    std::cmatch m;

    if (std::regex_search(arg.data(), arg.data() + arg.size(), m, re1)) {
      ord.type = SectionOrder::GROUP;
      ord.name = m[1].str();
    } else if (std::regex_search(arg.data(), arg.data() + arg.size(), m, re2)) {
      ord.type = SectionOrder::SECTION;
      ord.name = m[1].str();
    } else if (std::regex_search(arg.data(), arg.data() + arg.size(), m, re3)) {
      ord.type = SectionOrder::ADDR;
      std::string s = m[1];
      ord.value = std::stoull(s, nullptr, s.starts_with("0x") ? 16 : 10);
    } else if (std::regex_search(arg.data(), arg.data() + arg.size(), m, re4)) {
      ord.type = SectionOrder::ALIGN;
      std::string s = m[1];
      ord.value = std::stoull(s, nullptr, s.starts_with("0x") ? 16 : 10);
    } else if (std::regex_search(arg.data(), arg.data() + arg.size(), m, re5)) {
      ord.type = SectionOrder::SYMBOL;
      ord.name = m[1].str();
    } else {
      Fatal(ctx) << "--section-order: parse error: " << arg;
    }

    vec.push_back(ord);
    arg = arg.substr(m[0].length());
  }

  bool is_first = true;
  for (SectionOrder &ord : vec) {
    if (ord.type == SectionOrder::SECTION) {
      if (is_first) {
        is_first = false;
      } else if (ord.name == "EHDR")
        Fatal(ctx) << "--section-order: EHDR must be the first "
                   << "section specifier: " << arg;
    }
  }

  return vec;
}

template <typename E>
static std::variant<Symbol<E> *, u64>
parse_defsym_value(Context<E> &ctx, std::string_view s) {
  if (s.starts_with("0x") || s.starts_with("0X")) {
    size_t nread;
    u64 addr = std::stoull(std::string(s), &nread, 16);
    if (s.size() != nread)
      return {};
    return addr;
  }

  if (s.find_first_not_of("0123456789") == s.npos)
    return (u64)std::stoull(std::string(s), nullptr, 10);
  return get_symbol(ctx, s);
}

template <typename E>
std::vector<std::string> parse_nonpositional_args(Context<E> &ctx) {
  std::span<std::string_view> args = ctx.cmdline_args;
  args = args.subspan(1);

  std::vector<std::string> remaining;
  std::string_view arg;

  ctx.page_size = E::page_size;
  ctx.arg.color_diagnostics = isatty(STDERR_FILENO);

  bool version_shown = false;
  bool warn_shared_textrel = false;
  std::optional<SeparateCodeKind> z_separate_code;
  std::optional<bool> z_relro;
  std::unordered_set<std::string_view> rpaths;

  auto add_rpath = [&](std::string_view arg) {
    if (rpaths.insert(arg).second) {
      if (!ctx.arg.rpaths.empty())
        ctx.arg.rpaths += ':';
      ctx.arg.rpaths += arg;
    }
  };

  // RISC-V object files contains lots of local symbols, so by default
  // we discard them. This is compatible with GNU ld.
  if constexpr (is_riscv<E>)
    ctx.arg.discard_locals = true;

  // It looks like the SPARC's dynamic linker takes both RELA's r_addend
  // and the value at the relocated place. So we don't want to write
  // values to relocated places.
  if (is_sparc<E>)
    ctx.arg.apply_dynamic_relocs = false;

  auto read_arg = [&](std::string name) {
    for (std::string opt : add_dashes(name)) {
      if (args[0] == opt) {
        if (args.size() == 1)
          Fatal(ctx) << "option -" << name << ": argument missing";
        arg = args[1];
        args = args.subspan(2);
        return true;
      }

      std::string prefix = (name.size() == 1) ? opt : opt + "=";
      if (args[0].starts_with(prefix)) {
        arg = args[0].substr(prefix.size());
        args = args.subspan(1);
        return true;
      }
    }
    return false;
  };

  auto read_eq = [&](std::string name) {
    for (std::string opt : add_dashes(name)) {
      if (args[0].starts_with(opt + "=")) {
        arg = args[0].substr(opt.size() + 1);
        args = args.subspan(1);
        return true;
      }
    }
    return false;
  };

  auto read_flag = [&](std::string name) {
    for (std::string opt : add_dashes(name)) {
      if (args[0] == opt) {
        args = args.subspan(1);
        return true;
      }
    }
    return false;
  };

  auto read_z_flag = [&](std::string name) {
    if (args.size() >= 2 && args[0] == "-z" && args[1] == name) {
      args = args.subspan(2);
      return true;
    }

    if (!args.empty() && args[0] == "-z" + name) {
      args = args.subspan(1);
      return true;
    }
    return false;
  };

  auto read_z_arg = [&](std::string name) {
    if (args.size() >= 2 && args[0] == "-z" && args[1].starts_with(name + "=")) {
      arg = args[1].substr(name.size() + 1);
      args = args.subspan(2);
      return true;
    }

    if (!args.empty() && args[0].starts_with("-z" + name + "=")) {
      arg = args[0].substr(name.size() + 3);
      args = args.subspan(1);
      return true;
    }
    return false;
  };

  while (!args.empty()) {
    if (read_flag("help")) {
      SyncOut(ctx) << "Usage: " << ctx.cmdline_args[0]
                   << " [options] file...\n" << helpmsg;
      exit(0);
    }

    if (read_arg("o") || read_arg("output")) {
      ctx.arg.output = arg;
    } else if (read_arg("dynamic-linker") || read_arg("I")) {
      ctx.arg.dynamic_linker = arg;
    } else if (read_flag("no-dynamic-linker")) {
      ctx.arg.dynamic_linker = "";
    } else if (read_flag("v")) {
      SyncOut(ctx) << mold_version;
      version_shown = true;
    } else if (read_flag("version")) {
      SyncOut(ctx) << mold_version;
      exit(0);
    } else if (read_flag("V")) {
      SyncOut(ctx) << mold_version
                   << "\n  Supported emulations:\n   elf_x86_64\n   elf_i386\n"
                   << "   aarch64linux\n   armelf_linux_eabi\n   elf64lriscv\n"
                   << "   elf64briscv\n   elf32lriscv\n   elf32briscv\n"
                   << "   elf32ppc\n   elf64ppc\n   elf64lppc\n   elf64_s390\n"
                   << "   elf64_sparc\n   m68kelf\n   shlelf_linux\n   elf64alpha";
      version_shown = true;
    } else if (read_arg("m")) {
      if (arg == "elf_x86_64") {
        ctx.arg.emulation = X86_64::target_name;
      } else if (arg == "elf_i386") {
        ctx.arg.emulation = I386::target_name;
      } else if (arg == "aarch64linux") {
        ctx.arg.emulation = ARM64::target_name;
      } else if (arg == "armelf_linux_eabi") {
        ctx.arg.emulation = ARM32::target_name;
      } else if (arg == "elf64lriscv") {
        ctx.arg.emulation = RV64LE::target_name;
      } else if (arg == "elf64briscv") {
        ctx.arg.emulation = RV64BE::target_name;
      } else if (arg == "elf32lriscv") {
        ctx.arg.emulation = RV32LE::target_name;
      } else if (arg == "elf32briscv") {
        ctx.arg.emulation = RV32BE::target_name;
      } else if (arg == "elf32ppc" || arg == "elf32ppclinux") {
        ctx.arg.emulation = PPC32::target_name;
      } else if (arg == "elf64ppc") {
        ctx.arg.emulation = PPC64V1::target_name;
      } else if (arg == "elf64lppc") {
        ctx.arg.emulation = PPC64V2::target_name;
      } else if (arg == "elf64_s390") {
        ctx.arg.emulation = S390X::target_name;
      } else if (arg == "elf64_sparc") {
        ctx.arg.emulation = SPARC64::target_name;
      } else if (arg == "m68kelf") {
        ctx.arg.emulation = M68K::target_name;
      } else if (arg == "shlelf_linux") {
        ctx.arg.emulation = SH4::target_name;
      } else if (arg == "elf64alpha") {
        ctx.arg.emulation = ALPHA::target_name;
      } else {
        Fatal(ctx) << "unknown -m argument: " << arg;
      }
    } else if (read_flag("end-lib")) {
      remaining.push_back("--end-lib");
    } else if (read_flag("export-dynamic") || read_flag("E")) {
      ctx.arg.export_dynamic = true;
    } else if (read_flag("no-export-dynamic")) {
      ctx.arg.export_dynamic = false;
    } else if (read_flag("Bsymbolic")) {
      ctx.arg.Bsymbolic = true;
    } else if (read_flag("Bsymbolic-functions")) {
      ctx.arg.Bsymbolic_functions = true;
    } else if (read_flag("Bno-symbolic")) {
      ctx.arg.Bsymbolic = false;
      ctx.arg.Bsymbolic_functions = false;
    } else if (read_arg("exclude-libs")) {
      append(ctx.arg.exclude_libs, split_by_comma_or_colon(arg));
    } else if (read_flag("q") || read_flag("emit-relocs")) {
      ctx.arg.emit_relocs = true;
      ctx.arg.discard_locals = false;
    } else if (read_arg("e") || read_arg("entry")) {
      ctx.arg.entry = arg;
    } else if (read_arg("Map")) {
      ctx.arg.Map = arg;
      ctx.arg.print_map = true;
    } else if (read_flag("print-dependencies")) {
      ctx.arg.print_dependencies = true;
    } else if (read_flag("print-map") || read_flag("M")) {
      ctx.arg.print_map = true;
    } else if (read_flag("Bstatic") || read_flag("dn") || read_flag("static")) {
      ctx.arg.is_static = true;
      remaining.push_back("--Bstatic");
    } else if (read_flag("Bdynamic") || read_flag("dy")) {
      ctx.arg.is_static = false;
      remaining.push_back("--Bdynamic");
    } else if (read_flag("shared") || read_flag("Bshareable")) {
      ctx.arg.shared = true;
    } else if (read_arg("spare-dynamic-tags")) {
      ctx.arg.spare_dynamic_tags = parse_number(ctx, "spare-dynamic-tags", arg);
    } else if (read_flag("start-lib")) {
      remaining.push_back("--start-lib");
    } else if (read_flag("start-stop")) {
      ctx.arg.start_stop = true;
    } else if (read_arg("dependency-file")) {
      ctx.arg.dependency_file = arg;
    } else if (read_arg("defsym")) {
      size_t pos = arg.find('=');
      if (pos == arg.npos || pos == arg.size() - 1)
        Fatal(ctx) << "-defsym: syntax error: " << arg;
      ctx.arg.defsyms.emplace_back(get_symbol(ctx, arg.substr(0, pos)),
                                   parse_defsym_value(ctx, arg.substr(pos + 1)));
    } else if (read_flag(":lto-pass2")) {
      ctx.arg.lto_pass2 = true;
    } else if (read_arg(":ignore-ir-file")) {
      ctx.arg.ignore_ir_file.insert(arg);
    } else if (read_flag("demangle")) {
      ctx.arg.demangle = true;
    } else if (read_flag("no-demangle")) {
      ctx.arg.demangle = false;
    } else if (read_flag("default-symver")) {
      ctx.arg.default_symver = true;
    } else if (read_flag("noinhibit-exec")) {
      ctx.arg.noinhibit_exec = true;
    } else if (read_flag("shuffle-sections")) {
      ctx.arg.shuffle_sections = SHUFFLE_SECTIONS_SHUFFLE;
    } else if (read_eq("shuffle-sections")) {
      ctx.arg.shuffle_sections = SHUFFLE_SECTIONS_SHUFFLE;
      ctx.arg.shuffle_sections_seed = parse_number(ctx, "shuffle-sections", arg);
    } else if (read_flag("reverse-sections")) {
      ctx.arg.shuffle_sections = SHUFFLE_SECTIONS_REVERSE;
    } else if (read_flag("rosegment")) {
      ctx.arg.rosegment = true;
    } else if (read_flag("no-rosegment")) {
      ctx.arg.rosegment = false;
    } else if (read_arg("y") || read_arg("trace-symbol")) {
      ctx.arg.trace_symbol.push_back(arg);
    } else if (read_arg("filler")) {
      ctx.arg.filler = parse_hex(ctx, "filler", arg);
    } else if (read_arg("L") || read_arg("library-path")) {
      ctx.arg.library_paths.push_back(std::string(arg));
    } else if (read_arg("sysroot")) {
      ctx.arg.sysroot = arg;
    } else if (read_arg("unique")) {
      std::optional<Glob> pat = Glob::compile(arg);
      if (!pat)
        Fatal(ctx) << "-unique: invalid glob pattern: " << arg;
      ctx.arg.unique = std::move(*pat);
    } else if (read_arg("unresolved-symbols")) {
      if (arg == "report-all" || arg == "ignore-in-shared-libs")
        ctx.arg.unresolved_symbols = UNRESOLVED_ERROR;
      else if (arg == "ignore-all" || arg == "ignore-in-object-files")
        ctx.arg.unresolved_symbols = UNRESOLVED_IGNORE;
      else
        Fatal(ctx) << "unknown --unresolved-symbols argument: " << arg;
    } else if (read_arg("u") || read_arg("undefined")) {
      ctx.arg.undefined.push_back(arg);
    } else if (read_arg("require-defined")) {
      ctx.arg.require_defined.push_back(arg);
    } else if (read_arg("init")) {
      ctx.arg.init = arg;
    } else if (read_arg("fini")) {
      ctx.arg.fini = arg;
    } else if (read_arg("hash-style")) {
      if (arg == "sysv") {
        ctx.arg.hash_style_sysv = true;
        ctx.arg.hash_style_gnu = false;
      } else if (arg == "gnu") {
        ctx.arg.hash_style_sysv = false;
        ctx.arg.hash_style_gnu = true;
      } else if (arg == "both") {
        ctx.arg.hash_style_sysv = true;
        ctx.arg.hash_style_gnu = true;
      } else if (arg == "none") {
        ctx.arg.hash_style_sysv = false;
        ctx.arg.hash_style_gnu = false;
      } else {
        Fatal(ctx) << "invalid --hash-style argument: " << arg;
      }
    } else if (read_arg("soname") ||
               read_arg("h")) {
      ctx.arg.soname = arg;
    } else if (read_flag("allow-multiple-definition")) {
      ctx.arg.allow_multiple_definition = true;
    } else if (read_flag("apply-dynamic-relocs")) {
      ctx.arg.apply_dynamic_relocs = true;
    } else if (read_flag("no-apply-dynamic-relocs")) {
      ctx.arg.apply_dynamic_relocs = false;
    } else if (read_flag("trace")) {
      ctx.arg.trace = true;
    } else if (read_flag("eh-frame-hdr")) {
      ctx.arg.eh_frame_hdr = true;
    } else if (read_flag("no-eh-frame-hdr")) {
      ctx.arg.eh_frame_hdr = false;
    } else if (read_flag("pie") || read_flag("pic-executable")) {
      ctx.arg.pic = true;
      ctx.arg.pie = true;
    } else if (read_flag("no-pie") || read_flag("no-pic-executable") ||
               read_flag("nopie")) {
      ctx.arg.pic = false;
      ctx.arg.pie = false;
    } else if (read_flag("relax")) {
      ctx.arg.relax = true;
    } else if (read_flag("no-relax")) {
      ctx.arg.relax = false;
    } else if (read_flag("gdb-index")) {
      ctx.arg.gdb_index = true;
    } else if (read_flag("no-gdb-index")) {
      ctx.arg.gdb_index = false;
    } else if (read_flag("r") || read_flag("relocatable")) {
      ctx.arg.relocatable = true;
      ctx.arg.emit_relocs = true;
      ctx.arg.discard_locals = false;
    } else if (read_flag("relocatable-merge-sections")) {
      ctx.arg.relocatable_merge_sections = true;
    } else if (read_flag("perf")) {
      ctx.arg.perf = true;
    } else if (read_flag("pack-dyn-relocs=relr")) {
      ctx.arg.pack_dyn_relocs_relr = true;
    } else if (read_flag("pack-dyn-relocs=none")) {
      ctx.arg.pack_dyn_relocs_relr = false;
    } else if (read_arg("package-metadata")) {
      ctx.arg.package_metadata = arg;
    } else if (read_flag("stats")) {
      ctx.arg.stats = true;
      Counter::enabled = true;
    } else if (read_arg("C") || read_arg("directory")) {
      ctx.arg.directory = arg;
    } else if (read_arg("chroot")) {
      ctx.arg.chroot = arg;
    } else if (read_flag("color-diagnostics") ||
               read_flag("color-diagnostics=auto")) {
      ctx.arg.color_diagnostics = isatty(STDERR_FILENO);
    } else if (read_flag("color-diagnostics=always")) {
      ctx.arg.color_diagnostics = true;
    } else if (read_flag("color-diagnostics=never")) {
      ctx.arg.color_diagnostics = false;
    } else if (read_flag("warn-common")) {
      ctx.arg.warn_common = true;
    } else if (read_flag("no-warn-common")) {
      ctx.arg.warn_common = false;
    } else if (read_flag("warn-once")) {
      ctx.arg.warn_once = true;
    } else if (read_flag("warn-shared-textrel")) {
      warn_shared_textrel = true;
    } else if (read_flag("warn-textrel")) {
      ctx.arg.warn_textrel = true;
    } else if (read_flag("enable-new-dtags")) {
      ctx.arg.enable_new_dtags = true;
    } else if (read_flag("disable-new-dtags")) {
      ctx.arg.enable_new_dtags = false;
    } else if (read_flag("execute-only")) {
      ctx.arg.execute_only = true;
    } else if (read_arg("compress-debug-sections")) {
      if (arg == "zlib" || arg == "zlib-gabi")
        ctx.arg.compress_debug_sections = COMPRESS_ZLIB;
      else if (arg == "zstd")
        ctx.arg.compress_debug_sections = COMPRESS_ZSTD;
      else if (arg == "none")
        ctx.arg.compress_debug_sections = COMPRESS_NONE;
      else
        Fatal(ctx) << "invalid --compress-debug-sections argument: " << arg;
    } else if (read_arg("wrap")) {
      ctx.arg.wrap.insert(arg);
    } else if (read_flag("omagic") || read_flag("N")) {
      ctx.arg.omagic = true;
      ctx.arg.is_static = true;
    } else if (read_flag("no-omagic")) {
      ctx.arg.omagic = false;
    } else if (read_arg("oformat")) {
      if (arg != "binary")
        Fatal(ctx) << "-oformat: " << arg << " is not supported";
      ctx.arg.oformat_binary = true;
    } else if (read_arg("retain-symbols-file")) {
      read_retain_symbols_file(ctx, arg);
    } else if (read_arg("section-align")) {
      size_t pos = arg.find('=');
      if (pos == arg.npos || pos == arg.size() - 1)
        Fatal(ctx) << "--section-align: syntax error: " << arg;
      i64 value = parse_number(ctx, "section-align", arg.substr(pos + 1));
      if (!has_single_bit(value))
        Fatal(ctx) << "--section-align=" << arg << ": value must be a power of 2";
      ctx.arg.section_align[arg.substr(0, pos)] = value;
    } else if (read_arg("section-start")) {
      size_t pos = arg.find('=');
      if (pos == arg.npos || pos == arg.size() - 1)
        Fatal(ctx) << "--section-start: syntax error: " << arg;
      ctx.arg.section_start[arg.substr(0, pos)] =
        parse_hex(ctx, "section-start", arg.substr(pos + 1));
    } else if (read_arg("section-order")) {
      ctx.arg.section_order = parse_section_order(ctx, arg);
    } else if (read_arg("Tbss")) {
      ctx.arg.section_start[".bss"] = parse_hex(ctx, "Tbss", arg);
    } else if (read_arg("Tdata")) {
      ctx.arg.section_start[".data"] = parse_hex(ctx, "Tdata", arg);
    } else if (read_arg("Ttext")) {
      ctx.arg.section_start[".text"] = parse_hex(ctx, "Ttext", arg);
    } else if (read_flag("repro")) {
      ctx.arg.repro = true;
    } else if (read_z_flag("now")) {
      ctx.arg.z_now = true;
    } else if (read_z_flag("lazy")) {
      ctx.arg.z_now = false;
    } else if (read_z_flag("cet-report=none")) {
      ctx.arg.z_cet_report = CET_REPORT_NONE;
    } else if (read_z_flag("cet-report=warning")) {
      ctx.arg.z_cet_report = CET_REPORT_WARNING;
    } else if (read_z_flag("cet-report=error")) {
      ctx.arg.z_cet_report = CET_REPORT_ERROR;
    } else if (read_z_flag("execstack")) {
      ctx.arg.z_execstack = true;
    } else if (read_z_flag("execstack-if-needed")) {
      ctx.arg.z_execstack_if_needed = true;
    } else if (read_z_arg("max-page-size")) {
      ctx.page_size = parse_number(ctx, "-z max-page-size", arg);
      if (!has_single_bit(ctx.page_size))
        Fatal(ctx) << "-z max-page-size " << arg << ": value must be a power of 2";
    } else if (read_z_arg("start-stop-visibility")) {
      if (arg != "hidden")
        Fatal(ctx) << "-z start-stop-visibility: unsupported visibility: " << arg;
    } else if (read_z_flag("noexecstack")) {
      ctx.arg.z_execstack = false;
    } else if (read_z_flag("relro")) {
      z_relro = true;
    } else if (read_z_flag("norelro")) {
      z_relro = false;
    } else if (read_z_flag("defs")) {
      ctx.arg.z_defs = true;
    } else if (read_z_flag("undefs")) {
      ctx.arg.z_defs = false;
    } else if (read_z_flag("nodlopen")) {
      ctx.arg.z_dlopen = false;
    } else if (read_z_flag("nodelete")) {
      ctx.arg.z_delete = false;
    } else if (read_z_flag("nocopyreloc")) {
      ctx.arg.z_copyreloc = false;
    } else if (read_z_flag("nodump")) {
      ctx.arg.z_dump = false;
    } else if (read_z_flag("initfirst")) {
      ctx.arg.z_initfirst = true;
    } else if (read_z_flag("interpose")) {
      ctx.arg.z_interpose = true;
    } else if (read_z_flag("ibt")) {
      ctx.arg.z_ibt = true;
    } else if (read_z_flag("ibtplt")) {
    } else if (read_z_flag("muldefs")) {
      ctx.arg.allow_multiple_definition = true;
    } else if (read_z_flag("keep-text-section-prefix")) {
      ctx.arg.z_keep_text_section_prefix = true;
    } else if (read_z_flag("nokeep-text-section-prefix")) {
      ctx.arg.z_keep_text_section_prefix = false;
    } else if (read_z_flag("shstk")) {
      ctx.arg.z_shstk = true;
    } else if (read_z_flag("text")) {
      ctx.arg.z_text = true;
    } else if (read_z_flag("notext") || read_z_flag("textoff")) {
      ctx.arg.z_text = false;
    } else if (read_z_flag("origin")) {
      ctx.arg.z_origin = true;
    } else if (read_z_flag("nodefaultlib")) {
      ctx.arg.z_nodefaultlib = true;
    } else if (read_z_flag("pack-relative-relocs")) {
      ctx.arg.pack_dyn_relocs_relr = true;
    } else if (read_z_flag("nopack-relative-relocs")) {
      ctx.arg.pack_dyn_relocs_relr = false;
    } else if (read_z_flag("separate-loadable-segments")) {
      z_separate_code = SEPARATE_LOADABLE_SEGMENTS;
    } else if (read_z_flag("separate-code")) {
      z_separate_code = SEPARATE_CODE;
    } else if (read_z_flag("noseparate-code")) {
      z_separate_code = NOSEPARATE_CODE;
    } else if (read_z_flag("dynamic-undefined-weak")) {
      ctx.arg.z_dynamic_undefined_weak = true;
    } else if (read_z_flag("nodynamic-undefined-weak")) {
      ctx.arg.z_dynamic_undefined_weak = false;
    } else if (read_flag("no-undefined")) {
      ctx.arg.z_defs = true;
    } else if (read_flag("fatal-warnings")) {
      ctx.arg.fatal_warnings = true;
    } else if (read_flag("no-fatal-warnings")) {
      ctx.arg.fatal_warnings = false;
    } else if (read_flag("fork")) {
      ctx.arg.fork = true;
    } else if (read_flag("no-fork")) {
      ctx.arg.fork = false;
    } else if (read_flag("gc-sections")) {
      ctx.arg.gc_sections = true;
    } else if (read_flag("no-gc-sections")) {
      ctx.arg.gc_sections = false;
    } else if (read_flag("print-gc-sections")) {
      ctx.arg.print_gc_sections = true;
    } else if (read_flag("no-print-gc-sections")) {
      ctx.arg.print_gc_sections = false;
    } else if (read_arg("icf")) {
      if (arg == "all") {
        ctx.arg.icf = true;
        ctx.arg.icf_all = true;
      } else if (arg == "safe") {
        ctx.arg.icf = true;
      } else if (arg == "none") {
        ctx.arg.icf = false;
      } else {
        Fatal(ctx) << "unknown --icf argument: " << arg;
      }
    } else if (read_flag("no-icf")) {
      ctx.arg.icf = false;
    } else if (read_flag("ignore-data-address-equality")) {
      ctx.arg.ignore_data_address_equality = true;
    } else if (read_arg("image-base")) {
      ctx.arg.image_base = parse_number(ctx, "image-base", arg);
    } else if (read_arg("physical-image-base")) {
      ctx.arg.physical_image_base = parse_number(ctx, "physical-image-base", arg);
    } else if (read_flag("print-icf-sections")) {
      ctx.arg.print_icf_sections = true;
    } else if (read_flag("no-print-icf-sections")) {
      ctx.arg.print_icf_sections = false;
    } else if (read_flag("quick-exit")) {
      ctx.arg.quick_exit = true;
    } else if (read_flag("no-quick-exit")) {
      ctx.arg.quick_exit = false;
    } else if (read_arg("plugin")) {
      ctx.arg.plugin = arg;
    } else if (read_arg("plugin-opt")) {
      ctx.arg.plugin_opt.push_back(std::string(arg));
    } else if (read_flag("lto-cs-profile-generate")) {
      ctx.arg.plugin_opt.push_back("cs-profile-generate");
    } else if (read_arg("lto-cs-profile-file")) {
      ctx.arg.plugin_opt.push_back("cs-profile-path=" + std::string(arg));
    } else if (read_flag("lto-debug-pass-manager")) {
      ctx.arg.plugin_opt.push_back("debug-pass-manager");
    } else if (read_flag("disable-verify")) {
      ctx.arg.plugin_opt.push_back("disable-verify");
    } else if (read_flag("lto-emit-asm")) {
      ctx.arg.plugin_opt.push_back("emit-asm");
    } else if (read_arg("thinlto-jobs")) {
      ctx.arg.plugin_opt.push_back("jobs=" + std::string(arg));
    } else if (read_flag("no-legacy-pass-manager")) {
      ctx.arg.plugin_opt.push_back("legacy-pass-manager");
    } else if (read_arg("lto-partitions")) {
      ctx.arg.plugin_opt.push_back("lto-partitions=" + std::string(arg));
    } else if (read_flag("no-lto-legacy-pass-manager")) {
      ctx.arg.plugin_opt.push_back("new-pass-manager");
    } else if (read_arg("lto-obj-path")) {
      ctx.arg.plugin_opt.push_back("obj-path=" + std::string(arg));
    } else if (read_arg("opt-remarks-filename")) {
      ctx.arg.plugin_opt.push_back("opt-remarks-filename=" + std::string(arg));
    } else if (read_arg("opt-remarks-format")) {
      ctx.arg.plugin_opt.push_back("opt-remarks-format=" + std::string(arg));
    } else if (read_arg("opt-remarks-hotness-threshold")) {
      ctx.arg.plugin_opt.push_back("opt-remarks-hotness-threshold=" +
                                   std::string(arg));
    } else if (read_arg("opt-remarks-passes")) {
      ctx.arg.plugin_opt.push_back("opt-remarks-passes=" + std::string(arg));
    } else if (read_flag("opt-remarks-with_hotness")) {
      ctx.arg.plugin_opt.push_back("opt-remarks-with-hotness");
    } else if (args[0].starts_with("-lto-O")) {
      ctx.arg.plugin_opt.push_back("O" + std::string(args[0].substr(6)));
      args = args.subspan(1);
    } else if (args[0].starts_with("--lto-O")) {
      ctx.arg.plugin_opt.push_back("O" + std::string(args[0].substr(7)));
      args = args.subspan(1);
    } else if (read_arg("lto-pseudo-probe-for-profiling")) {
      ctx.arg.plugin_opt.push_back("pseudo-probe-for-profiling=" +
                                   std::string(arg));
    } else if (read_arg("lto-sample-profile")) {
      ctx.arg.plugin_opt.push_back("sample-profile=" + std::string(arg));
    } else if (read_flag("save-temps")) {
      ctx.arg.plugin_opt.push_back("save-temps");
    } else if (read_flag("thinlto-emit-imports-files")) {
      ctx.arg.plugin_opt.push_back("thinlto-emit-imports-files");
    } else if (read_arg("thinlto-index-only")) {
      ctx.arg.plugin_opt.push_back("thinlto-index-only=" + std::string(arg));
    } else if (read_flag("thinlto-index-only")) {
      ctx.arg.plugin_opt.push_back("thinlto-index-only");
    } else if (read_arg("thinlto-object-suffix-replace")) {
      ctx.arg.plugin_opt.push_back("thinlto-object-suffix-replace=" +
                                   std::string(arg));
    } else if (read_arg("thinlto-prefix-replace")) {
      ctx.arg.plugin_opt.push_back("thinlto-prefix-replace=" + std::string(arg));
    } else if (read_arg("thinlto-cache-dir")) {
      ctx.arg.plugin_opt.push_back("cache-dir=" + std::string(arg));
    } else if (read_arg("thinlto-cache-policy")) {
      ctx.arg.plugin_opt.push_back("cache-policy=" + std::string(arg));
    } else if (read_arg("thinlto-jobs")) {
      ctx.arg.plugin_opt.push_back("jobs=" + std::string(arg));
    } else if (read_arg("thread-count")) {
      ctx.arg.thread_count = parse_number(ctx, "thread-count", arg);
    } else if (read_flag("threads")) {
      ctx.arg.thread_count = 0;
    } else if (read_flag("no-threads")) {
      ctx.arg.thread_count = 1;
    } else if (read_eq("threads")) {
      ctx.arg.thread_count = parse_number(ctx, "threads", arg);
    } else if (read_flag("discard-all") || read_flag("x")) {
      ctx.arg.discard_all = true;
    } else if (read_flag("discard-locals") || read_flag("X")) {
      ctx.arg.discard_locals = true;
    } else if (read_flag("strip-all") || read_flag("s")) {
      ctx.arg.strip_all = true;
    } else if (read_flag("strip-debug") || read_flag("S")) {
      ctx.arg.strip_all = true;
    } else if (read_flag("warn-unresolved-symbols")) {
      ctx.arg.unresolved_symbols = UNRESOLVED_WARN;
    } else if (read_flag("error-unresolved-symbols")) {
      ctx.arg.unresolved_symbols = UNRESOLVED_ERROR;
    } else if (read_arg("rpath")) {
      add_rpath(arg);
    } else if (read_arg("R")) {
      if (is_file(arg))
        Fatal(ctx) << "-R" << arg
                   << ": -R as an alias for --just-symbols is not supported";
      add_rpath(arg);
    } else if (read_flag("undefined-version")) {
      ctx.arg.undefined_version = true;
    } else if (read_flag("no-undefined-version")) {
      ctx.arg.undefined_version = false;
    } else if (read_flag("build-id")) {
      ctx.arg.build_id.kind = BuildId::HASH;
      ctx.arg.build_id.hash_size = 20;
    } else if (read_arg("build-id")) {
      if (arg == "none") {
        ctx.arg.build_id.kind = BuildId::NONE;
      } else if (arg == "uuid") {
        ctx.arg.build_id.kind = BuildId::UUID;
      } else if (arg == "md5") {
        ctx.arg.build_id.kind = BuildId::HASH;
        ctx.arg.build_id.hash_size = 16;
      } else if (arg == "sha1") {
        ctx.arg.build_id.kind = BuildId::HASH;
        ctx.arg.build_id.hash_size = 20;
      } else if (arg == "sha256") {
        ctx.arg.build_id.kind = BuildId::HASH;
        ctx.arg.build_id.hash_size = 32;
      } else if (arg.starts_with("0x") || arg.starts_with("0X")) {
        ctx.arg.build_id.kind = BuildId::HEX;
        ctx.arg.build_id.value = parse_hex_build_id(ctx, arg);
      } else {
        Fatal(ctx) << "invalid --build-id argument: " << arg;
      }
    } else if (read_flag("no-build-id")) {
      ctx.arg.build_id.kind = BuildId::NONE;
    } else if (read_arg("format") || read_arg("b")) {
      if (arg == "binary")
        Fatal(ctx)
          << "mold does not support `-b binary`. If you want to convert a"
          << " binary file into an object file, use `objcopy -I binary -O"
          << " default <input-file> <output-file.o>` instead.";
      Fatal(ctx) << "unknown command line option: -b " << arg;
    } else if (read_arg("auxiliary") || read_arg("f")) {
      ctx.arg.auxiliary.push_back(arg);
    } else if (read_arg("filter") || read_arg("F")) {
      ctx.arg.filter.push_back(arg);
    } else if (read_arg("O")) {
    } else if (read_flag("O0")) {
    } else if (read_flag("O1")) {
    } else if (read_flag("O2")) {
    } else if (read_flag("verbose")) {
    } else if (read_flag("color-diagnostics")) {
    } else if (read_flag("eh-frame-hdr")) {
    } else if (read_flag("start-group")) {
    } else if (read_flag("end-group")) {
    } else if (read_flag("(")) {
    } else if (read_flag(")")) {
    } else if (read_flag("fatal-warnings")) {
    } else if (read_flag("enable-new-dtags")) {
    } else if (read_flag("disable-new-dtags")) {
    } else if (read_flag("nostdlib")) {
    } else if (read_flag("allow-shlib-undefined")) {
    } else if (read_flag("no-allow-shlib-undefined")) {
    } else if (read_flag("no-add-needed")) {
    } else if (read_flag("no-call-graph-profile-sort")) {
    } else if (read_flag("no-copy-dt-needed-entries")) {
    } else if (read_arg("sort-section")) {
    } else if (read_flag("sort-common")) {
    } else if (read_flag("dc")) {
    } else if (read_flag("dp")) {
    } else if (read_flag("fix-cortex-a53-835769")) {
    } else if (read_flag("fix-cortex-a53-843419")) {
    } else if (read_flag("EL")) {
    } else if (read_flag("warn-once")) {
    } else if (read_flag("nodefaultlibs")) {
    } else if (read_flag("warn-constructors")) {
    } else if (read_flag("warn-execstack")) {
    } else if (read_flag("no-warn-execstack")) {
    } else if (read_flag("secure-plt")) {
    } else if (read_arg("rpath-link")) {
    } else if (read_z_flag("combreloc")) {
    } else if (read_z_flag("nocombreloc")) {
    } else if (read_z_arg("common-page-size")) {
    } else if (read_flag("no-keep-memory")) {
    } else if (read_arg("max-cache-size")) {
    } else if (read_arg("version-script")) {
      // --version-script, --dynamic-list and --export-dynamic-symbol[-list]
      // are treated as positional arguments even if they are actually not
      // positional. This is because linker scripts (a positional argument)
      // can also specify a version script, and it's better to consolidate
      // parsing in read_input_files. In particular, version scripts can
      // modify ctx.default_version which we initialize *after* parsing
      // non-positional args, so the parsing cannot be done right here.
      remaining.push_back("--version-script=" + std::string(arg));
    } else if (read_arg("dynamic-list")) {
      ctx.arg.Bsymbolic = true;
      remaining.push_back("--dynamic-list=" + std::string(arg));
    } else if (read_arg("export-dynamic-symbol")) {
      remaining.push_back("--export-dynamic-symbol=" + std::string(arg));
    } else if (read_arg("export-dynamic-symbol-list")) {
      remaining.push_back("--export-dynamic-symbol-list=" + std::string(arg));
    } else if (read_flag("as-needed")) {
      remaining.push_back("--as-needed");
    } else if (read_flag("no-as-needed")) {
      remaining.push_back("--no-as-needed");
    } else if (read_flag("whole-archive")) {
      remaining.push_back("--whole-archive");
    } else if (read_flag("no-whole-archive")) {
      remaining.push_back("--no-whole-archive");
    } else if (read_arg("l")) {
      remaining.push_back("-l" + std::string(arg));
    } else if (read_arg("script") || read_arg("T")) {
      remaining.push_back(std::string(arg));
    } else if (read_flag("push-state")) {
      remaining.push_back("--push-state");
    } else if (read_flag("pop-state")) {
      remaining.push_back("--pop-state");
    } else if (args[0].starts_with("-z") && args[0].size() > 2) {
      Warn(ctx) << "unknown command line option: " << args[0];
      args = args.subspan(1);
    } else if (args[0] == "-z" && args.size() >= 2) {
      Warn(ctx) << "unknown command line option: -z " << args[1];
      args = args.subspan(2);
    } else if (args[0] == "-dynamic") {
      Fatal(ctx) << "unknown command line option: -dynamic;"
                 << " -dynamic is a macOS linker's option. If you are trying"
                 << " to build a binary for an Apple platform, you need to use"
                 << " ld64.mold instead of mold or ld.mold.";
    } else {
      if (args[0][0] == '-')
        Fatal(ctx) << "unknown command line option: " << args[0];
      remaining.push_back(std::string(args[0]));
      args = args.subspan(1);
    }
  }

  if (!ctx.arg.sysroot.empty()) {
    for (std::string &path : ctx.arg.library_paths) {
      if (std::string_view(path).starts_with('='))
        path = ctx.arg.sysroot + path.substr(1);
      else if (std::string_view(path).starts_with("$SYSROOT"))
        path = ctx.arg.sysroot + path.substr(8);
    }
  }

  // Clean library paths by removing redundant `/..` and `/.`
  // so that they are easier to read in log messages.
  for (std::string &path : ctx.arg.library_paths)
    path = path_clean(path);

  if (ctx.arg.shared)
    ctx.arg.pic = true;

  if (ctx.arg.pic)
    ctx.arg.image_base = 0;

  if (ctx.arg.retain_symbols_file) {
    ctx.arg.strip_all = false;
    ctx.arg.discard_all = false;
  }

  if (ctx.arg.relocatable)
    ctx.arg.is_static = true;

  // --section-order implies `-z separate-loadable-segments`
  if (z_separate_code)
    ctx.arg.z_separate_code = *z_separate_code;
  else if (!ctx.arg.section_order.empty())
    ctx.arg.z_separate_code = SEPARATE_LOADABLE_SEGMENTS;

  // --section-order implies `-z norelro`
  if (z_relro)
    ctx.arg.z_relro = *z_relro;
  else if (!ctx.arg.section_order.empty())
    ctx.arg.z_relro = false;

  if (!ctx.arg.shared) {
    if (!ctx.arg.filter.empty())
      Fatal(ctx) << "-filter may not be used without -shared";
    if (!ctx.arg.auxiliary.empty())
      Fatal(ctx) << "-auxiliary may not be used without -shared";
  }

  if (!ctx.arg.apply_dynamic_relocs && !E::is_rela)
    Fatal(ctx) << "--no-apply-dynamic-relocs may not be used on "
               << E::target_name;

  if (is_sparc<E> && ctx.arg.apply_dynamic_relocs)
    Fatal(ctx) << "--apply-dynamic-relocs may not be used on SPARC64";

  if (!ctx.arg.section_start.empty() && !ctx.arg.section_order.empty())
    Fatal(ctx) << "--section-start may not be used with --section-order";

  if (ctx.arg.image_base % ctx.page_size)
    Fatal(ctx) << "-image-base must be a multiple of -max-page-size";

  if (ctx.arg.thread_count == 0)
    ctx.arg.thread_count = get_default_thread_count();

  if (char *env = getenv("MOLD_REPRO"); env && env[0])
    ctx.arg.repro = true;

  if (ctx.arg.shared || ctx.arg.export_dynamic)
    ctx.default_version = VER_NDX_GLOBAL;
  else
    ctx.default_version = VER_NDX_LOCAL;

  if (ctx.arg.default_symver) {
    std::string ver = ctx.arg.soname.empty() ?
      filepath(ctx.arg.output).filename().string() : std::string(ctx.arg.soname);
    ctx.arg.version_definitions.push_back(ver);
    ctx.default_version = VER_NDX_LAST_RESERVED + 1;
  }

  if (ctx.arg.shared && warn_shared_textrel)
    ctx.arg.warn_textrel = true;

  ctx.arg.undefined.push_back(ctx.arg.entry);

  // --oformat=binary implies --strip-all because without a section
  // header, there's no way to identify the locations of a symbol
  // table in an output file in the first place.
  if (ctx.arg.oformat_binary)
    ctx.arg.strip_all = true;

  // By default, mold tries to ovewrite to an output file if exists
  // because at least on Linux, writing to an existing file is much
  // faster than creating a fresh file and writing to it.
  //
  // However, if an existing file is in use, writing to it will mess
  // up processes that are executing that file. Linux prevents a write
  // to a running executable file; it returns ETXTBSY on open(2).
  // However, that mechanism doesn't protect .so files. Therefore, we
  // want to disable this optimization if we are creating a shared
  // object file.
  if (ctx.arg.shared)
    ctx.overwrite_output_file = false;

  if (version_shown && remaining.empty())
    exit(0);
  return remaining;
}

using E = MOLD_TARGET;

template std::vector<std::string> parse_nonpositional_args(Context<E> &ctx);

} // namespace mold::elf
