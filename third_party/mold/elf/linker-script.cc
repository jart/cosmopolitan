// clang-format off
// On Linux, /usr/lib/x86_64-linux-gnu/libc.so is not actually
// a shared object file but an ASCII text file containing a linker
// script to include a "real" libc.so file. Therefore, we need to
// support a (very limited) subset of the linker script language.

#include "third_party/mold/elf/mold.h"

#include "third_party/libcxx/cctype"
#include "third_party/libcxx/iomanip"

namespace mold::elf {

template <typename E>
static thread_local MappedFile<Context<E>> *current_file;

template <typename E>
void read_version_script(Context<E> &ctx, std::span<std::string_view> &tok);

static std::string_view get_line(std::string_view input, const char *pos) {
  assert(input.data() <= pos);
  assert(pos < input.data() + input.size());

  i64 start = input.rfind('\n', pos - input.data());
  if (start == input.npos)
    start = 0;
  else
    start++;

  i64 end = input.find('\n', pos - input.data());
  if (end == input.npos)
    end = input.size();

  return input.substr(start, end - start);
}

template <typename E>
class SyntaxError {
public:
  SyntaxError(Context<E> &ctx, std::string_view errpos) : out(ctx) {
    std::string_view contents = current_file<E>->get_contents();
    std::string_view line = get_line(contents, errpos.data());

    i64 lineno = 1;
    for (i64 i = 0; contents.data() + i < line.data(); i++)
      if (contents[i] == '\n')
        lineno++;

    i64 column = errpos.data() - line.data();

    std::stringstream ss;
    ss << current_file<E>->name << ":" << lineno << ": ";
    i64 indent = (i64)ss.tellp() + strlen("mold: ");
    ss << line << "\n" << std::setw(indent + column) << " " << "^ ";
    out << ss.str();
  }

  template <class T> SyntaxError &operator<<(T &&val) {
    out << std::forward<T>(val);
    return *this;
  }

  [[noreturn]] ~SyntaxError() = default;

  Fatal<Context<E>> out;
};

template <typename E>
static std::vector<std::string_view>
tokenize(Context<E> &ctx, std::string_view input) {
  std::vector<std::string_view> vec;
  while (!input.empty()) {
    if (isspace(input[0])) {
      input = input.substr(1);
      continue;
    }

    if (input.starts_with("/*")) {
      i64 pos = input.find("*/", 2);
      if (pos == std::string_view::npos)
        SyntaxError(ctx, input) << "unclosed comment";
      input = input.substr(pos + 2);
      continue;
    }

    if (input[0] == '#') {
      i64 pos = input.find("\n", 1);
      if (pos == std::string_view::npos)
        break;
      input = input.substr(pos + 1);
      continue;
    }

    if (input[0] == '"') {
      i64 pos = input.find('"', 1);
      if (pos == std::string_view::npos)
        SyntaxError(ctx, input) << "unclosed string literal";
      vec.push_back(input.substr(0, pos + 1));
      input = input.substr(pos + 1);
      continue;
    }

    i64 pos = input.find_first_not_of(
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
      "0123456789_.$/\\~=+[]*?-!^:");

    if (pos == 0)
      pos = 1;
    else if (pos == input.npos)
      pos = input.size();

    vec.push_back(input.substr(0, pos));
    input = input.substr(pos);
  }
  return vec;
}

template <typename E>
static std::span<std::string_view>
skip(Context<E> &ctx, std::span<std::string_view> tok, std::string_view str) {
  if (tok.empty())
    Fatal(ctx) << current_file<E>->name << ": expected '" << str
               << "', but got EOF";
  if (tok[0] != str)
    SyntaxError(ctx, tok[0]) << "expected '" << str << "'";
  return tok.subspan(1);
}

static std::string_view unquote(std::string_view s) {
  if (s.size() > 0 && s[0] == '"') {
    assert(s[s.size() - 1] == '"');
    return s.substr(1, s.size() - 2);
  }
  return s;
}

template <typename E>
static std::span<std::string_view>
read_output_format(Context<E> &ctx, std::span<std::string_view> tok) {
  tok = skip(ctx, tok, "(");
  while (!tok.empty() && tok[0] != ")")
    tok = tok.subspan(1);
  if (tok.empty())
    Fatal(ctx) << current_file<E>->name << ": expected ')', but got EOF";
  return tok.subspan(1);
}

template <typename E>
static bool is_in_sysroot(Context<E> &ctx, std::string path) {
  std::string rel = to_abs_path(path)
                        .lexically_relative(to_abs_path(ctx.arg.sysroot))
                        .string();
  return rel != "." && !rel.starts_with("../");
}

template <typename E>
static MappedFile<Context<E>> *resolve_path(Context<E> &ctx, std::string_view tok) {
  std::string str(unquote(tok));

  // GNU ld prepends the sysroot if a pathname starts with '/' and the
  // script being processed is in the sysroot. We do the same.
  if (str.starts_with('/') && is_in_sysroot(ctx, current_file<E>->name))
    return MappedFile<Context<E>>::must_open(ctx, ctx.arg.sysroot + str);

  if (str.starts_with('=')) {
    std::string path;
    if (ctx.arg.sysroot.empty())
      path = str.substr(1);
    else
      path = ctx.arg.sysroot + str.substr(1);
    return MappedFile<Context<E>>::must_open(ctx, path);
  }

  if (str.starts_with("-l"))
    return find_library(ctx, str.substr(2));

  if (MappedFile<Context<E>> *mf = open_library(ctx, str))
    return mf;

  for (std::string_view dir : ctx.arg.library_paths) {
    std::string path = std::string(dir) + "/" + str;
    if (MappedFile<Context<E>> *mf = open_library(ctx, path))
      return mf;
  }

  SyntaxError(ctx, tok) << "library not found: " << str;
}

template <typename E>
static std::span<std::string_view>
read_group(Context<E> &ctx, std::span<std::string_view> tok) {
  tok = skip(ctx, tok, "(");

  while (!tok.empty() && tok[0] != ")") {
    if (tok[0] == "AS_NEEDED") {
      bool orig = ctx.as_needed;
      ctx.as_needed = true;
      tok = read_group(ctx, tok.subspan(1));
      ctx.as_needed = orig;
      continue;
    }

    MappedFile<Context<E>> *mf = resolve_path(ctx, tok[0]);
    read_file(ctx, mf);
    tok = tok.subspan(1);
  }

  if (tok.empty())
    Fatal(ctx) << current_file<E>->name << ": expected ')', but got EOF";
  return tok.subspan(1);
}

template <typename E>
void parse_linker_script(Context<E> &ctx, MappedFile<Context<E>> *mf) {
  current_file<E> = mf;

  std::vector<std::string_view> vec = tokenize(ctx, mf->get_contents());
  std::span<std::string_view> tok = vec;

  while (!tok.empty()) {
    if (tok[0] == "OUTPUT_FORMAT") {
      tok = read_output_format(ctx, tok.subspan(1));
    } else if (tok[0] == "INPUT" || tok[0] == "GROUP") {
      tok = read_group(ctx, tok.subspan(1));
    } else if (tok[0] == "VERSION") {
      tok = tok.subspan(1);
      tok = skip(ctx, tok, "{");
      read_version_script(ctx, tok);
      tok = skip(ctx, tok, "}");
    } else if (tok.size() > 3 && tok[1] == "=" && tok[3] == ";") {
      ctx.arg.defsyms.emplace_back(get_symbol(ctx, unquote(tok[0])),
                                   get_symbol(ctx, unquote(tok[2])));
      tok = tok.subspan(4);
    } else if (tok[0] == ";") {
      tok = tok.subspan(1);
    } else {
      SyntaxError(ctx, tok[0]) << "unknown linker script token";
    }
  }
}

template <typename E>
std::string_view
get_script_output_type(Context<E> &ctx, MappedFile<Context<E>> *mf) {
  current_file<E> = mf;

  std::vector<std::string_view> vec = tokenize(ctx, mf->get_contents());
  std::span<std::string_view> tok = vec;

  if (tok.size() >= 3 && tok[0] == "OUTPUT_FORMAT" && tok[1] == "(") {
    if (tok[2] == "elf64-x86-64")
      return X86_64::target_name;
    if (tok[2] == "elf32-i386")
      return I386::target_name;
  }

  if (tok.size() >= 3 && (tok[0] == "INPUT" || tok[0] == "GROUP") &&
      tok[1] == "(")
    if (MappedFile<Context<E>> *mf =
        MappedFile<Context<E>>::open(ctx, std::string(unquote(tok[2]))))
      return get_machine_type(ctx, mf);

  return "";
}

static bool read_label(std::span<std::string_view> &tok,
                       std::string label) {
  if (tok.size() >= 1 && tok[0] == label + ":") {
    tok = tok.subspan(1);
    return true;
  }

  if (tok.size() >= 2 && tok[0] == label && tok[1] == ":") {
    tok = tok.subspan(2);
    return true;
  }
  return false;
}

template <typename E>
static void
read_version_script_commands(Context<E> &ctx, std::span<std::string_view> &tok,
                             std::string_view ver_str, u16 ver_idx, bool is_cpp) {
  bool is_global = true;

  while (!tok.empty() && tok[0] != "}") {
    if (read_label(tok, "global")) {
      is_global = true;
      continue;
    }

    if (read_label(tok, "local")) {
      is_global = false;
      continue;
    }

    if (tok[0] == "extern") {
      tok = tok.subspan(1);

      if (!tok.empty() && tok[0] == "\"C\"") {
        tok = tok.subspan(1);
        tok = skip(ctx, tok, "{");
        read_version_script_commands( ctx, tok, ver_str, ver_idx, false);
      } else {
        tok = skip(ctx, tok, "\"C++\"");
        tok = skip(ctx, tok, "{");
        read_version_script_commands(ctx, tok, ver_str, ver_idx, true);
      }

      tok = skip(ctx, tok, "}");
      tok = skip(ctx, tok, ";");
      continue;
    }

    if (tok[0] == "*") {
      ctx.default_version = (is_global ? ver_idx : (u32)VER_NDX_LOCAL);
      ctx.default_version_from_version_script = true;
    } else if (is_global) {
      ctx.version_patterns.push_back({unquote(tok[0]), current_file<E>->name,
                                      ver_str, ver_idx, is_cpp});
    } else {
      ctx.version_patterns.push_back({unquote(tok[0]), current_file<E>->name,
                                      ver_str, VER_NDX_LOCAL, is_cpp});
    }

    tok = tok.subspan(1);

    if (!tok.empty() && tok[0] == "}")
      return;
    tok = skip(ctx, tok, ";");
  }
}

template <typename E>
void read_version_script(Context<E> &ctx, std::span<std::string_view> &tok) {
  u16 next_ver = VER_NDX_LAST_RESERVED + ctx.arg.version_definitions.size() + 1;

  while (!tok.empty() && tok[0] != "}") {
    std::string_view ver_str;
    u16 ver_idx;

    if (tok[0] == "{") {
      ver_str = "global";
      ver_idx = VER_NDX_GLOBAL;
    } else {
      ver_str = tok[0];
      ver_idx = next_ver++;
      ctx.arg.version_definitions.push_back(std::string(tok[0]));
      tok = tok.subspan(1);
    }

    tok = skip(ctx, tok, "{");
    read_version_script_commands(ctx, tok, ver_str, ver_idx, false);
    tok = skip(ctx, tok, "}");
    if (!tok.empty() && tok[0] != ";")
      tok = tok.subspan(1);
    tok = skip(ctx, tok, ";");
  }
}

template <typename E>
void parse_version_script(Context<E> &ctx, MappedFile<Context<E>> *mf) {
  current_file<E> = mf;
  std::vector<std::string_view> vec = tokenize(ctx, mf->get_contents());
  std::span<std::string_view> tok = vec;
  read_version_script(ctx, tok);
  if (!tok.empty())
    SyntaxError(ctx, tok[0]) << "trailing garbage token";
}

template <typename E>
void read_dynamic_list_commands(Context<E> &ctx, std::span<std::string_view> &tok,
                                bool is_cpp) {
  while (!tok.empty() && tok[0] != "}") {
    if (tok[0] == "extern") {
      tok = tok.subspan(1);

      if (!tok.empty() && tok[0] == "\"C\"") {
        tok = tok.subspan(1);
        tok = skip(ctx, tok, "{");
        read_dynamic_list_commands(ctx, tok, false);
      } else {
        tok = skip(ctx, tok, "\"C++\"");
        tok = skip(ctx, tok, "{");
        read_dynamic_list_commands(ctx, tok, true);
      }

      tok = skip(ctx, tok, "}");
      tok = skip(ctx, tok, ";");
      continue;
    }

    if (tok[0] == "*")
      ctx.default_version = VER_NDX_GLOBAL;
    else
      ctx.version_patterns.push_back({unquote(tok[0]), current_file<E>->name,
                                      "global", VER_NDX_GLOBAL, is_cpp});

    tok = skip(ctx, tok.subspan(1), ";");
  }
}

template <typename E>
void parse_dynamic_list(Context<E> &ctx, MappedFile<Context<E>> *mf) {
  current_file<E> = mf;
  std::vector<std::string_view> vec = tokenize(ctx, mf->get_contents());
  std::span<std::string_view> tok = vec;

  tok = skip(ctx, tok, "{");
  read_dynamic_list_commands(ctx, tok, false);
  tok = skip(ctx, tok, "}");
  tok = skip(ctx, tok, ";");

  if (!tok.empty())
    SyntaxError(ctx, tok[0]) << "trailing garbage token";
}

using E = MOLD_TARGET;

template void parse_linker_script(Context<E> &, MappedFile<Context<E>> *);
template std::string_view get_script_output_type(Context<E> &, MappedFile<Context<E>> *);
template void parse_version_script(Context<E> &, MappedFile<Context<E>> *);
template void parse_dynamic_list(Context<E> &, MappedFile<Context<E>> *);

} // namespace mold::elf
