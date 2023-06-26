// clang-format off
// This file implements the Identical Code Folding feature which can
// reduce the output file size of a typical program by a few percent.
// ICF identifies read-only input sections that happen to be identical
// and thus can be used interchangeably. ICF leaves one of them and discards
// the others.
//
// ICF is usually used in combination with -ffunction-sections and
// -fdata-sections compiler options, so that object files have one section
// for each function or variable instead of having one large .text or .data.
// The unit of ICF merging is section.
//
// Two sections are considered identical by ICF if they have the exact
// same contents, metadata such as section flags, exception handling
// records, and relocations. The last one is interesting because two
// relocations are considered identical if they point to the _same_
// section in terms of ICF.
//
// To see what that means, consider two sections, A and B, which are
// identical except for one pair of relocations. Say, A has a relocation to
// section C, and B has a relocation to D. In this case, A and B are
// considered identical if C and D are considered identical. C and D can be
// either really the same section or two different sections that are
// considered identical by ICF. Below is an example of such inputs, A, B, C
// and D:
//
//   void A() { C(); }
//   void B() { D(); }
//   void C() { A(); }
//   void D() { B(); }
//
// If we assume A and B are mergeable, we can merge C and D, which makes A
// and B mergeable. There's no contradiction in our assumption, so we can
// conclude that A and B as well as C and D are mergeable.
//
// This problem boils down to one in graph theory. Input to ICF can be
// considered as a directed graph in which vertices are sections and edges
// are relocations. Vertices have labels (section contents, etc.), and so
// are edges (relocation offsets, etc.). Two vertices are considered
// identical if and only if the (possibly infinite) their unfoldings into
// regular trees are equal. Given this formulation, we want to find as
// many identical vertices as possible.
//
// Just like a lot of problems with graph, this problem doesn't have a
// straightforward "optimal" solution, and we need to resort to heuristics.
//
// mold approaches this problem by hashing program trees with increasing depth
// on each iteration.
// For example, when we start, we only hash individual functions with
// their call into other functions omitted. From the second iteration, we
// put the function they call into the hash by appending the hash of those
// functions from the previous iteration. This means that the nth iteration
// hashes call chain up to (n-1) levels deep.
// We use a cryptographic hash function, so the unique number of hashes will
// only monotonically increase as we take into account of deeper trees with
// iterations (otherwise, that means we have found a hash collision). We stop
// when the unique number of hashes stop increasing; this is based on the fact
// that once we observe an iteration with the same amount of unique hashes as
// the previous iteration, it will remain unchanged for further iterations.
// This is provable, but here we omit the proof for brevity.
//
// When compared to other approaches, mold's approach has a relatively cheaper
// cost per iteration, and as a bonus, is highly parallelizable.
// For Chromium, mold's ICF finishes in less than 1 second with 20 threads,
// whereas lld takes 5 seconds and gold takes 50 seconds under the same
// conditions.

#include "third_party/mold/elf/mold.h"
// MISSING #include "../common/sha.h"

#include "third_party/libcxx/array"
#include "third_party/libcxx/cstdio"
// MISSING #include <tbb/concurrent_unordered_map.h>
// MISSING #include <tbb/concurrent_vector.h>
// MISSING #include <tbb/enumerable_thread_specific.h>
// MISSING #include <tbb/parallel_for.h>
// MISSING #include <tbb/parallel_for_each.h>
// MISSING #include <tbb/parallel_sort.h>

static constexpr int64_t HASH_SIZE = 16;

typedef std::array<uint8_t, HASH_SIZE> Digest;

namespace std {
template<> struct hash<Digest> {
  size_t operator()(const Digest &k) const {
    return *(int64_t *)&k[0];
  }
};
}

namespace mold::elf {

template <typename E>
static void uniquify_cies(Context<E> &ctx) {
  Timer t(ctx, "uniquify_cies");
  std::vector<CieRecord<E> *> cies;

  for (ObjectFile<E> *file : ctx.objs) {
    for (CieRecord<E> &cie : file->cies) {
      for (i64 i = 0; i < cies.size(); i++) {
        if (cie.equals(*cies[i])) {
          cie.icf_idx = i;
          goto found;
        }
      }
      cie.icf_idx = cies.size();
      cies.push_back(&cie);
    found:;
    }
  }
}

template <typename E>
static bool is_eligible(Context<E> &ctx, InputSection<E> &isec) {
  const ElfShdr<E> &shdr = isec.shdr();
  std::string_view name = isec.name();

  bool is_alloc = (shdr.sh_flags & SHF_ALLOC);
  bool is_exec = (shdr.sh_flags & SHF_EXECINSTR) ||
                 ctx.arg.ignore_data_address_equality;
  bool is_relro = (name == ".data.rel.ro" ||
                   name.starts_with(".data.rel.ro."));
  bool is_readonly = !(shdr.sh_flags & SHF_WRITE) || is_relro;
  bool is_bss = (shdr.sh_type == SHT_NOBITS);
  bool is_empty = (shdr.sh_size == 0);
  bool is_init = (shdr.sh_type == SHT_INIT_ARRAY || name == ".init");
  bool is_fini = (shdr.sh_type == SHT_FINI_ARRAY || name == ".fini");
  bool is_enumerable = is_c_identifier(name);
  bool is_addr_taken = !ctx.arg.icf_all && isec.address_significant;

  return is_alloc && is_exec && is_readonly && !is_bss && !is_empty &&
         !is_init && !is_fini && !is_enumerable && !is_addr_taken;
}

static Digest digest_final(SHA256Hash &sha) {
  u8 buf[SHA256_SIZE];
  sha.finish(buf);

  Digest digest;
  memcpy(digest.data(), buf, HASH_SIZE);
  return digest;
}

template <typename E>
static bool is_leaf(Context<E> &ctx, InputSection<E> &isec) {
  if (!isec.get_rels(ctx).empty())
    return false;

  for (FdeRecord<E> &fde : isec.get_fdes())
    if (fde.get_rels(isec.file).size() > 1)
      return false;

  return true;
}

template <typename E>
struct LeafHasher {
  size_t operator()(InputSection<E> *isec) const {
    u64 h = hash_string(isec->contents);
    for (FdeRecord<E> &fde : isec->get_fdes()) {
      u64 h2 = hash_string(fde.get_contents(isec->file).substr(8));
      h = combine_hash(h, h2);
    }
    return h;
  }
};

template <typename E>
struct LeafEq {
  bool operator()(InputSection<E> *a, InputSection<E> *b) const {
    if (a->contents != b->contents)
      return false;

    std::span<FdeRecord<E>> x = a->get_fdes();
    std::span<FdeRecord<E>> y = b->get_fdes();

    if (x.size() != y.size())
      return false;

    for (i64 i = 0; i < x.size(); i++)
      if (x[i].get_contents(a->file).substr(8) !=
          y[i].get_contents(b->file).substr(8))
        return false;
    return true;
  }
};

// Early merge of leaf nodes, which can be processed without constructing the
// entire graph. This reduces the vertex count and improves memory efficiency.
template <typename E>
static void merge_leaf_nodes(Context<E> &ctx) {
  Timer t(ctx, "merge_leaf_nodes");

  static Counter eligible("icf_eligibles");
  static Counter non_eligible("icf_non_eligibles");
  static Counter leaf("icf_leaf_nodes");

  tbb::concurrent_unordered_map<InputSection<E> *, InputSection<E> *,
                                LeafHasher<E>, LeafEq<E>> map;

  tbb::parallel_for((i64)0, (i64)ctx.objs.size(), [&](i64 i) {
    for (std::unique_ptr<InputSection<E>> &isec : ctx.objs[i]->sections) {
      if (!isec || !isec->is_alive)
        continue;

      if (!is_eligible(ctx, *isec)) {
        non_eligible++;
        continue;
      }

      if (is_leaf(ctx, *isec)) {
        leaf++;
        isec->icf_leaf = true;
        auto [it, inserted] = map.insert({isec.get(), isec.get()});
        if (!inserted && isec->get_priority() < it->second->get_priority())
          it->second = isec.get();
      } else {
        eligible++;
        isec->icf_eligible = true;
      }
    }
  });

  tbb::parallel_for((i64)0, (i64)ctx.objs.size(), [&](i64 i) {
    for (std::unique_ptr<InputSection<E>> &isec : ctx.objs[i]->sections) {
      if (isec && isec->is_alive && isec->icf_leaf) {
        auto it = map.find(isec.get());
        assert(it != map.end());
        isec->leader = it->second;
      }
    }
  });
}

template <typename E>
static Digest compute_digest(Context<E> &ctx, InputSection<E> &isec) {
  SHA256Hash sha;

  auto hash = [&](auto val) {
    sha.update((u8 *)&val, sizeof(val));
  };

  auto hash_string = [&](std::string_view str) {
    hash(str.size());
    sha.update((u8 *)str.data(), str.size());
  };

  auto hash_symbol = [&](Symbol<E> &sym) {
    InputSection<E> *isec = sym.get_input_section();

    if (!sym.file) {
      hash('1');
      hash((u64)&sym);
    } else if (SectionFragment<E> *frag = sym.get_frag()) {
      hash('2');
      hash((u64)frag);
    } else if (!isec) {
      hash('3');
    } else if (isec->leader) {
      hash('4');
      hash((u64)isec->leader);
    } else if (isec->icf_eligible) {
      hash('5');
    } else {
      hash('6');
      hash((u64)isec);
    }
    hash(sym.value);
  };

  hash_string(isec.contents);
  hash(isec.shdr().sh_flags);
  hash(isec.get_fdes().size());
  hash(isec.get_rels(ctx).size());

  for (FdeRecord<E> &fde : isec.get_fdes()) {
    hash(isec.file.cies[fde.cie_idx].icf_idx);

    // Bytes 0 to 4 contain the length of this record, and
    // bytes 4 to 8 contain an offset to CIE.
    hash_string(fde.get_contents(isec.file).substr(8));

    hash(fde.get_rels(isec.file).size());

    for (const ElfRel<E> &rel : fde.get_rels(isec.file).subspan(1)) {
      hash_symbol(*isec.file.symbols[rel.r_sym]);
      hash(rel.r_type);
      hash(rel.r_offset - fde.input_offset);
      hash(get_addend(isec.file.cies[fde.cie_idx].input_section, rel));
    }
  }

  for (i64 i = 0; i < isec.get_rels(ctx).size(); i++) {
    const ElfRel<E> &rel = isec.get_rels(ctx)[i];
    hash(rel.r_offset);
    hash(rel.r_type);
    hash(get_addend(isec, rel));
    hash_symbol(*isec.file.symbols[rel.r_sym]);
  }

  return digest_final(sha);
}

template <typename E>
static std::vector<InputSection<E> *> gather_sections(Context<E> &ctx) {
  Timer t(ctx, "gather_sections");

  // Count the number of input sections for each input file.
  std::vector<i64> num_sections(ctx.objs.size());

  tbb::parallel_for((i64)0, (i64)ctx.objs.size(), [&](i64 i) {
    for (std::unique_ptr<InputSection<E>> &isec : ctx.objs[i]->sections)
      if (isec && isec->is_alive && isec->icf_eligible)
        num_sections[i]++;
  });

  std::vector<i64> section_indices(ctx.objs.size());
  for (i64 i = 0; i < ctx.objs.size() - 1; i++)
    section_indices[i + 1] = section_indices[i] + num_sections[i];

  std::vector<InputSection<E> *> sections(
    section_indices.back() + num_sections.back());

  // Fill `sections` contents.
  tbb::parallel_for((i64)0, (i64)ctx.objs.size(), [&](i64 i) {
    i64 idx = section_indices[i];
    for (std::unique_ptr<InputSection<E>> &isec : ctx.objs[i]->sections)
      if (isec && isec->is_alive && isec->icf_eligible)
        sections[idx++] = isec.get();
  });

  tbb::parallel_for((i64)0, (i64)sections.size(), [&](i64 i) {
    sections[i]->icf_idx = i;
  });

  return sections;
}

template <typename E>
static std::vector<Digest>
compute_digests(Context<E> &ctx, std::span<InputSection<E> *> sections) {
  Timer t(ctx, "compute_digests");

  std::vector<Digest> digests(sections.size());
  tbb::parallel_for((i64)0, (i64)sections.size(), [&](i64 i) {
    digests[i] = compute_digest(ctx, *sections[i]);
  });
  return digests;
}

// Build a graph, treating every function as a vertex and every function call
// as an edge. See the description at the top for a more detailed formulation.
// We use u32 indices here to improve cache locality.
template <typename E>
static void gather_edges(Context<E> &ctx,
                         std::span<InputSection<E> *> sections,
                         std::vector<u32> &edges,
                         std::vector<u32> &edge_indices) {
  Timer t(ctx, "gather_edges");

  if (sections.empty())
    return;

  std::vector<i64> num_edges(sections.size());
  edge_indices.resize(sections.size());

  tbb::parallel_for((i64)0, (i64)sections.size(), [&](i64 i) {
    InputSection<E> &isec = *sections[i];
    assert(isec.icf_eligible);

    for (i64 j = 0; j < isec.get_rels(ctx).size(); j++) {
      const ElfRel<E> &rel = isec.get_rels(ctx)[j];
      Symbol<E> &sym = *isec.file.symbols[rel.r_sym];
      if (!sym.get_frag())
        if (InputSection<E> *isec = sym.get_input_section())
          if (isec->icf_eligible)
            num_edges[i]++;
    }
  });

  for (i64 i = 0; i < num_edges.size() - 1; i++)
    edge_indices[i + 1] = edge_indices[i] + num_edges[i];

  edges.resize(edge_indices.back() + num_edges.back());

  tbb::parallel_for((i64)0, (i64)num_edges.size(), [&](i64 i) {
    InputSection<E> &isec = *sections[i];
    i64 idx = edge_indices[i];

    for (ElfRel<E> &rel : isec.get_rels(ctx)) {
      Symbol<E> &sym = *isec.file.symbols[rel.r_sym];
      if (InputSection<E> *isec = sym.get_input_section())
        if (isec->icf_eligible)
          edges[idx++] = isec->icf_idx;
  }
  });
}

template <typename E>
static i64 propagate(std::span<std::vector<Digest>> digests,
                     std::span<u32> edges, std::span<u32> edge_indices,
                     bool &slot, BitVector &converged,
                     tbb::affinity_partitioner &ap) {
  static Counter round("icf_round");
  round++;

  i64 num_digests = digests[0].size();
  tbb::enumerable_thread_specific<i64> changed;

  tbb::parallel_for((i64)0, num_digests, [&](i64 i) {
    if (converged.get(i))
      return;

    SHA256Hash sha;
    sha.update(digests[2][i].data(), HASH_SIZE);

    i64 begin = edge_indices[i];
    i64 end = (i + 1 == num_digests) ? edges.size() : edge_indices[i + 1];

    for (i64 j : edges.subspan(begin, end - begin))
      sha.update(digests[slot][j].data(), HASH_SIZE);

    digests[!slot][i] = digest_final(sha);

    if (digests[slot][i] == digests[!slot][i]) {
      // This node has converged. Skip further iterations as it will
      // yield the same hash.
      converged.set(i);
    } else {
      changed.local()++;
    }
  }, ap);

  slot = !slot;
  return changed.combine(std::plus());
}

template <typename E>
static i64 count_num_classes(std::span<Digest> digests,
                             tbb::affinity_partitioner &ap) {
  std::vector<Digest> vec(digests.begin(), digests.end());
  tbb::parallel_sort(vec);

  tbb::enumerable_thread_specific<i64> num_classes;
  tbb::parallel_for((i64)0, (i64)vec.size() - 1, [&](i64 i) {
    if (vec[i] != vec[i + 1])
      num_classes.local()++;
  }, ap);
  return num_classes.combine(std::plus());
}

template <typename E>
static void print_icf_sections(Context<E> &ctx) {
  tbb::concurrent_vector<InputSection<E> *> leaders;
  tbb::concurrent_unordered_multimap<InputSection<E> *, InputSection<E> *> map;

  tbb::parallel_for_each(ctx.objs, [&](ObjectFile<E> *file) {
    for (std::unique_ptr<InputSection<E>> &isec : file->sections) {
      if (isec && isec->is_alive && isec->leader) {
        if (isec.get() == isec->leader)
          leaders.push_back(isec.get());
        else
          map.insert({isec->leader, isec.get()});
      }
    }
  });

  tbb::parallel_sort(leaders.begin(), leaders.end(),
                     [&](InputSection<E> *a, InputSection<E> *b) {
                       return a->get_priority() < b->get_priority();
                     });

  i64 saved_bytes = 0;

  for (InputSection<E> *leader : leaders) {
    auto [begin, end] = map.equal_range(leader);
    if (begin == end)
      continue;

    SyncOut(ctx) << "selected section " << *leader;

    i64 n = 0;
    for (auto it = begin; it != end; it++) {
      SyncOut(ctx) << "  removing identical section " << *it->second;
      n++;
    }
    saved_bytes += leader->contents.size() * n;
  }

  SyncOut(ctx) << "ICF saved " << saved_bytes << " bytes";
}

template <typename E>
void icf_sections(Context<E> &ctx) {
  Timer t(ctx, "icf");
  if (ctx.objs.empty())
    return;

  uniquify_cies(ctx);
  merge_leaf_nodes(ctx);

  // Prepare for the propagation rounds.
  std::vector<InputSection<E> *> sections = gather_sections(ctx);

  // We allocate 3 arrays to store hashes for each vertex.
  //
  // Index 0 and 1 are used for tree hashes from the previous
  // iteration and the current iteration. They switch roles every
  // iteration. See `slot` below.
  //
  // Index 2 stores the initial, single-vertex hash. This is combined
  // with hashes from the connected vertices to form the tree hash
  // described above.
  std::vector<std::vector<Digest>> digests(3);
  digests[0] = compute_digests<E>(ctx, sections);
  digests[1].resize(digests[0].size());
  digests[2] = digests[0];

  std::vector<u32> edges;
  std::vector<u32> edge_indices;
  gather_edges<E>(ctx, sections, edges, edge_indices);

  BitVector converged(digests[0].size());
  bool slot = 0;

  // Execute the propagation rounds until convergence is obtained.
  {
    Timer t(ctx, "propagate");
    tbb::affinity_partitioner ap;

    // A cheap test that the graph hasn't converged yet.
    // The loop after this one uses a strict condition, but it's expensive
    // as it requires sorting the entire hash collection.
    //
    // For nodes that have a cycle in downstream (i.e. recursive
    // functions and functions that calls recursive functions) will always
    // change with the iterations. Nodes that doesn't (i.e. non-recursive
    // functions) will stop changing as soon as the propagation depth reaches
    // the call tree depth.
    // Here, we test whether we have reached sufficient depth for the latter,
    // which is a necessary (but not sufficient) condition for convergence.
    i64 num_changed = -1;
    for (;;) {
      i64 n = propagate<E>(digests, edges, edge_indices, slot, converged, ap);
      if (n == num_changed)
        break;
      num_changed = n;
    }

    // Run the pass until the unique number of hashes stop increasing, at which
    // point we have achieved convergence (proof omitted for brevity).
    i64 num_classes = -1;
    for (;;) {
      // count_num_classes requires sorting which is O(n log n), so do a little
      // more work beforehand to amortize that log factor.
      for (i64 i = 0; i < 10; i++)
        propagate<E>(digests, edges, edge_indices, slot, converged, ap);

      i64 n = count_num_classes<E>(digests[slot], ap);
      if (n == num_classes)
        break;
      num_classes = n;
    }
  }

  // Group sections by SHA digest.
  {
    Timer t(ctx, "group");

    auto *map = new tbb::concurrent_unordered_map<Digest, InputSection<E> *>;
    std::span<Digest> digest = digests[slot];

    tbb::parallel_for((i64)0, (i64)sections.size(), [&](i64 i) {
      InputSection<E> *isec = sections[i];
      auto [it, inserted] = map->insert({digest[i], isec});
      if (!inserted && isec->get_priority() < it->second->get_priority())
        it->second = isec;
    });

    tbb::parallel_for((i64)0, (i64)sections.size(), [&](i64 i) {
      auto it = map->find(digest[i]);
      assert(it != map->end());
      sections[i]->leader = it->second;
    });

    // Since free'ing the map is slow, postpone it.
    ctx.on_exit.push_back([=] { delete map; });
  }

  if (ctx.arg.print_icf_sections)
    print_icf_sections(ctx);

  // Eliminate duplicate sections.
  // Symbols pointing to eliminated sections will be redirected on the fly when
  // exporting to the symtab.
  {
    Timer t(ctx, "sweep");
    static Counter eliminated("icf_eliminated");
    tbb::parallel_for_each(ctx.objs, [](ObjectFile<E> *file) {
      for (std::unique_ptr<InputSection<E>> &isec : file->sections) {
        if (isec && isec->is_alive && isec->is_killed_by_icf()) {
          isec->kill();
          eliminated++;
        }
      }
    });
  }
}

using E = MOLD_TARGET;

template void icf_sections(Context<E> &ctx);

} // namespace mold::elf
