// clang-format off
#include "third_party/mold/common.h"

#include "third_party/libcxx/cstring"

namespace mold {

std::optional<Glob> Glob::compile(std::string_view pat) {
  std::vector<Element> vec;

  while (!pat.empty()) {
    u8 c = pat[0];
    pat = pat.substr(1);

    switch (c) {
    case '[': {
      // Here are a few bracket pattern examples:
      //
      // [abc]: a, b or c
      // [$\]!]: $, ] or !
      // [a-czg-i]: a, b, c, z, g, h, or i
      // [^a-z]: Any character except lowercase letters
      vec.push_back({BRACKET});
      std::bitset<256> &bitset = vec.back().bitset;

      bool negate = false;
      if (!pat.empty() && pat[0] == '^') {
        negate = true;
        pat = pat.substr(1);
      }

      bool closed = false;

      while (!pat.empty()) {
        if (pat[0] == ']') {
          pat = pat.substr(1);
          closed = true;
          break;
        }

        if (pat[0] == '\\') {
          pat = pat.substr(1);
          if (pat.empty())
            return {};
        }

        if (pat.size() >= 3 && pat[1] == '-') {
          u8 start = pat[0];
          u8 end = pat[2];
          pat = pat.substr(3);

          if (end == '\\') {
            if (pat.empty())
              return {};
            end = pat[0];
            pat = pat.substr(1);
          }

          if (end < start)
            return {};

          for (i64 i = start; i <= end; i++)
            bitset[i] = true;
        } else {
          bitset[(u8)pat[0]] = true;
          pat = pat.substr(1);
        }
      }

      if (!closed)
        return {};

      if (negate)
        bitset.flip();
      break;
    }
    case '?':
      vec.push_back({QUESTION});
      break;
    case '*':
      vec.push_back({STAR});
      break;
    default:
      if (vec.empty() || vec.back().kind != STRING)
        vec.push_back({STRING});
      vec.back().str += c;
      break;
    }
  }

  return {Glob{std::move(vec)}};
}

bool Glob::match(std::string_view str) {
  return do_match(str, elements);
}

bool Glob::do_match(std::string_view str, std::span<Element> elements) {
  while (!elements.empty()) {
    Element &e = elements[0];
    elements = elements.subspan(1);

    switch (e.kind) {
    case STRING:
      if (str.empty() || !str.starts_with(e.str))
        return false;
      str = str.substr(e.str.size());
      break;
    case STAR:
      if (elements.empty())
        return true;

      // Patterns like "*foo*bar*" should be much more common than more
      // complex ones like "*foo*[abc]*" or "*foo**?bar*", so we optimize
      // the former case here.
      if (elements[0].kind == STRING) {
        for (;;) {
          size_t pos = str.find(elements[0].str);
          if (pos == str.npos)
            break;
          if (do_match(str.substr(pos + elements[0].str.size()),
                       elements.subspan(1)))
            return true;
          str = str.substr(pos + 1);
        }
        return false;
      }

      // Other cases are handled here.
      for (i64 j = 0; j < str.size(); j++)
        if (do_match(str.substr(j), elements))
          return true;
      return false;
    case QUESTION:
      if (str.empty())
        return false;
      str = str.substr(1);
      break;
    case BRACKET:
      if (str.empty() || !e.bitset[str[0]])
        return false;
      str = str.substr(1);
      break;
    }
  }

  return str.empty();
}

} // namespace mold
