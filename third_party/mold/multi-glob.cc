// clang-format off
// This file implements the Aho-Corasick algorithm to match multiple
// glob patterns to symbol strings as quickly as possible.
//
// Here are some examples of glob patterns:
//
//    qt_private_api_tag*
//    *16QAccessibleCache*
//    *32QAbstractFileIconProviderPrivate*
//    *17QPixmapIconEngine*
//
// `*` is a wildcard that matches any substring. We sometimes have
// hundreds of glob patterns and have to match them against millions
// of symbol strings.
//
// Aho-Corasick cannot handle complex patterns such as `*foo*bar*`.
// We handle such patterns with the Glob class. Glob is relatively
// slow, but complex patterns are rare in practice, so it should be
// OK.

#include "third_party/mold/common.h"

#include "third_party/libcxx/queue"
#include "third_party/libcxx/regex"

namespace mold {

std::optional<u32> MultiGlob::find(std::string_view str) {
  std::call_once(once, [&] { compile(); });
  u32 val = UINT32_MAX;

  if (root) {
    // Match against simple glob patterns
    TrieNode *node = root.get();

    auto walk = [&](u8 c) {
      for (;;) {
        if (node->children[c]) {
          node = node->children[c].get();
          val = std::min(val, node->value);
          return;
        }

        if (!node->suffix_link)
          return;
        node = node->suffix_link;
      }
    };

    walk('\0');
    for (u8 c : str)
      walk(c);
    walk('\0');
  }

  // Match against complex glob patterns
  for (std::pair<Glob, u32> &glob : globs)
    if (glob.first.match(str))
      val = std::min(val, glob.second);

  if (val == UINT32_MAX)
    return {};
  return val;
}

static bool is_simple_pattern(std::string_view pat) {
  static std::regex re(R"(\*?[^*[?]+\*?)", std::regex_constants::optimize);
  return std::regex_match(pat.begin(), pat.end(), re);
}

static std::string handle_stars(std::string_view pat) {
  std::string str(pat);

  // Convert "foo" -> "\0foo\0", "*foo" -> "foo\0", "foo*" -> "\0foo"
  // and "*foo*" -> "foo". Aho-Corasick can do only substring matching,
  // so we use \0 as beginning/end-of-string markers.
  if (str.starts_with('*') && str.ends_with('*'))
    return str.substr(1, str.size() - 2);
  if (str.starts_with('*'))
    return str.substr(1) + "\0"s;
  if (str.ends_with('*'))
    return "\0"s + str.substr(0, str.size() - 1);
  return "\0"s + str + "\0"s;
}

bool MultiGlob::add(std::string_view pat, u32 val) {
  assert(!is_compiled);
  assert(!pat.empty());

  strings.push_back(std::string(pat));

  // Complex glob pattern
  if (!is_simple_pattern(pat)) {
    if (std::optional<Glob> glob = Glob::compile(pat)) {
      globs.push_back({std::move(*glob), val});
      return true;
    }
    return false;
  }

  // Simple glob pattern
  if (!root)
    root.reset(new TrieNode);
  TrieNode *node = root.get();

  for (u8 c : handle_stars(pat)) {
    if (!node->children[c])
      node->children[c].reset(new TrieNode);
    node = node->children[c].get();
  }

  node->value = std::min(node->value, val);
  return true;
}

void MultiGlob::compile() {
  is_compiled = true;
  if (root) {
    fix_suffix_links(*root);
    fix_values();
  }
}

void MultiGlob::fix_suffix_links(TrieNode &node) {
  for (i64 i = 0; i < 256; i++) {
    if (!node.children[i])
      continue;

    TrieNode &child = *node.children[i];

    TrieNode *cur = node.suffix_link;
    for (;;) {
      if (!cur) {
        child.suffix_link = root.get();
        break;
      }

      if (cur->children[i]) {
        child.suffix_link = cur->children[i].get();
        break;
      }

      cur = cur->suffix_link;
    }

    fix_suffix_links(child);
  }
}

void MultiGlob::fix_values() {
  std::queue<TrieNode *> queue;
  queue.push(root.get());

  do {
    TrieNode *node = queue.front();
    queue.pop();

    for (std::unique_ptr<TrieNode> &child : node->children) {
      if (!child)
        continue;
      child->value = std::min(child->value, child->suffix_link->value);
      queue.push(child.get());
    }
  } while (!queue.empty());
}

} // namespace mold
