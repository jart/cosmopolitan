// clang-format off
#pragma once

#include "third_party/mold/integers.h"

#include "third_party/libcxx/array"
#include "third_party/libcxx/atomic"
#include "third_party/libcxx/bit"
#include "third_party/libcxx/bitset"
#include "third_party/libcxx/cassert"
#include "third_party/libcxx/cstdio"
#include "third_party/libcxx/cstring"
#include "third_party/libcxx/unordered_map"
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
#include "third_party/libcxx/filesystem"
#include "third_party/libcxx/iostream"
#include "third_party/libcxx/mutex"
#include "third_party/libcxx/optional"
#include "third_party/libcxx/span"
#include "third_party/libcxx/sstream"
#include "third_party/libcxx/string"
#include "third_party/libcxx/string_view"
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
#include "third_party/libcxx/vector"

#ifdef _WIN32
// MISSING #include <io.h>
#else
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/mlock.h"
#include "libc/sysv/consts/msync.h"
#include "libc/sysv/consts/posix.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/consts/mfd.h"
#include "libc/sysv/consts/mremap.h"
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
#include "libc/isystem/getopt.h"
#include "third_party/musl/crypt.h"
#include "third_party/musl/lockf.h"
#endif

#define XXH_INLINE_ALL 1
#include "third_party/xxhash/xxhash.h"

#ifdef NDEBUG
# define unreachable() __builtin_unreachable()
#else
# define unreachable() assert(0 && "unreachable")
#endif

// __builtin_assume() is supported only by clang, and [[assume]] is
// available only in C++23, so we use this macro when giving a hint to
// the compiler's optimizer what's true.
#define ASSUME(x) do { if (!(x)) __builtin_unreachable(); } while (0)

// This is an assert() that is enabled even in the release build.
#define ASSERT(x)                                       \
  do {                                                  \
    if (!(x)) {                                         \
      std::cerr << "Assertion failed: (" << #x          \
                << "), function " << __FUNCTION__       \
                << ", file " << __FILE__                \
                << ", line " << __LINE__ << ".\n";      \
      std::abort();                                     \
    }                                                   \
  } while (0)

inline uint64_t hash_string(std::string_view str) {
  return XXH3_64bits(str.data(), str.size());
}

class HashCmp {
public:
  static size_t hash(const std::string_view &k) {
    return hash_string(k);
  }

  static bool equal(const std::string_view &k1, const std::string_view &k2) {
    return k1 == k2;
  }
};

namespace mold {

using namespace std::literals::string_literals;
using namespace std::literals::string_view_literals;

template <typename Context> class OutputFile;

inline char *output_tmpfile;
inline thread_local bool opt_demangle;

inline u8 *output_buffer_start = nullptr;
inline u8 *output_buffer_end = nullptr;

inline std::string mold_version;
extern std::string mold_version_string;
extern std::string mold_git_hash;

std::string errno_string();
std::string get_self_path();
void cleanup();
void install_signal_handler();
i64 get_default_thread_count();

static u64 combine_hash(u64 a, u64 b) {
  return a ^ (b + 0x9e3779b9 + (a << 6) + (a >> 2));
}

//
// Error output
//

template <typename Context>
class SyncOut {
public:
  SyncOut(Context &ctx, std::ostream *out = &std::cout) : out(out) {
    opt_demangle = ctx.arg.demangle;
  }

  ~SyncOut() {
    if (out) {
      std::scoped_lock lock(mu);
      *out << ss.str() << "\n";
    }
  }

  template <class T> SyncOut &operator<<(T &&val) {
    if (out)
      ss << std::forward<T>(val);
    return *this;
  }

  static inline std::mutex mu;

private:
  std::ostream *out;
  std::stringstream ss;
};

template <typename Context>
static std::string add_color(Context &ctx, std::string msg) {
  if (ctx.arg.color_diagnostics)
    return "mold: \033[0;1;31m" + msg + ":\033[0m ";
  return "mold: " + msg + ": ";
}

template <typename Context>
class Fatal {
public:
  Fatal(Context &ctx) : out(ctx, &std::cerr) {
    out << add_color(ctx, "fatal");
  }

  [[noreturn]] ~Fatal() {
    out.~SyncOut();
    cleanup();
    _exit(1);
  }

  template <class T> Fatal &operator<<(T &&val) {
    out << std::forward<T>(val);
    return *this;
  }

private:
  SyncOut<Context> out;
};

template <typename Context>
class Error {
public:
  Error(Context &ctx) : out(ctx, &std::cerr) {
    if (ctx.arg.noinhibit_exec) {
      out << add_color(ctx, "warning");
    } else {
      out << add_color(ctx, "error");
      ctx.has_error = true;
    }
  }

  template <class T> Error &operator<<(T &&val) {
    out << std::forward<T>(val);
    return *this;
  }

private:
  SyncOut<Context> out;
};

template <typename Context>
class Warn {
public:
  Warn(Context &ctx)
    : out(ctx, ctx.arg.suppress_warnings ? nullptr : &std::cerr) {
    if (ctx.arg.fatal_warnings) {
      out << add_color(ctx, "error");
      ctx.has_error = true;
    } else {
      out << add_color(ctx, "warning");
    }
  }

  template <class T> Warn &operator<<(T &&val) {
    out << std::forward<T>(val);
    return *this;
  }

private:
  SyncOut<Context> out;
};

//
// Atomics
//

// This is the same as std::atomic except that the default memory
// order is relaxed instead of sequential consistency.
template <typename T>
struct Atomic : std::atomic<T> {
  static constexpr std::memory_order relaxed = std::memory_order_relaxed;

  using std::atomic<T>::atomic;

  Atomic(const Atomic<T> &other) { store(other.load()); }

  Atomic<T> &operator=(const Atomic<T> &other) {
    store(other.load());
    return *this;
  }

  void operator=(T val) { store(val); }
  operator T() const { return load(); }

  void store(T val) { std::atomic<T>::store(val, relaxed); }
  T load() const { return std::atomic<T>::load(relaxed); }
  T exchange(T val) { return std::atomic<T>::exchange(val, relaxed); }
  T operator|=(T val) { return std::atomic<T>::fetch_or(val, relaxed); }
  T operator++() { return std::atomic<T>::fetch_add(1, relaxed) + 1; }
  T operator--() { return std::atomic<T>::fetch_sub(1, relaxed) - 1; }
  T operator++(int) { return std::atomic<T>::fetch_add(1, relaxed); }
  T operator--(int) { return std::atomic<T>::fetch_sub(1, relaxed); }

  bool test_and_set() {
    // A relaxed load + branch (assuming miss) takes only around 20 cycles,
    // while an atomic RMW can easily take hundreds on x86. We note that it's
    // common that another thread beat us in marking, so doing an optimistic
    // early test tends to improve performance in the ~20% ballpark.
    return load() || exchange(true);
  }
};

//
// Bit vector
//

class BitVector {
public:
  BitVector() = default;
  BitVector(u32 size) : vec((size + 7) / 8) {}

  void resize(u32 size) { vec.resize((size + 7) / 8); }
  bool get(u32 idx) const { return vec[idx / 8] & (1 << (idx % 8)); }
  void set(u32 idx) { vec[idx / 8] |= 1 << (idx % 8); }

private:
  std::vector<u8> vec;
};

//
// Utility functions
//

// Some C++ libraries haven't implemented std::has_single_bit yet.
inline bool has_single_bit(u64 val) {
  return std::popcount(val) == 1;
}

// Some C++ libraries haven't implemented std::bit_ceil yet.
inline u64 bit_ceil(u64 val) {
  if (has_single_bit(val))
    return val;
  return 1LL << (64 - std::countl_zero(val));
}

inline u64 align_to(u64 val, u64 align) {
  if (align == 0)
    return val;
  assert(has_single_bit(align));
  return (val + align - 1) & ~(align - 1);
}

inline u64 align_down(u64 val, u64 align) {
  assert(has_single_bit(align));
  return val & ~(align - 1);
}

inline u64 bit(u64 val, i64 pos) {
  return (val >> pos) & 1;
};

// Returns [hi:lo] bits of val.
inline u64 bits(u64 val, u64 hi, u64 lo) {
  return (val >> lo) & ((1LL << (hi - lo + 1)) - 1);
}

inline i64 sign_extend(u64 val, i64 size) {
  return (i64)(val << (63 - size)) >> (63 - size);
};

template <typename T, typename Compare = std::less<T>>
void update_minimum(std::atomic<T> &atomic, u64 new_val, Compare cmp = {}) {
  T old_val = atomic.load(std::memory_order_relaxed);
  while (cmp(new_val, old_val) &&
         !atomic.compare_exchange_weak(old_val, new_val,
                                       std::memory_order_relaxed));
}

template <typename T, typename Compare = std::less<T>>
void update_maximum(std::atomic<T> &atomic, u64 new_val, Compare cmp = {}) {
  T old_val = atomic.load(std::memory_order_relaxed);
  while (cmp(old_val, new_val) &&
         !atomic.compare_exchange_weak(old_val, new_val,
                                       std::memory_order_relaxed));
}

template <typename T, typename U>
inline void append(std::vector<T> &vec1, std::vector<U> vec2) {
  vec1.insert(vec1.end(), vec2.begin(), vec2.end());
}

template <typename T>
inline std::vector<T> flatten(std::vector<std::vector<T>> &vec) {
  i64 size = 0;
  for (std::vector<T> &v : vec)
    size += v.size();

  std::vector<T> ret;
  ret.reserve(size);
  for (std::vector<T> &v : vec)
    append(ret, v);
  return ret;
}

template <typename T>
inline void sort(T &vec) {
  std::stable_sort(vec.begin(), vec.end());
}

template <typename T, typename U>
inline void sort(T &vec, U less) {
  std::stable_sort(vec.begin(), vec.end(), less);
}

template <typename T>
inline void remove_duplicates(std::vector<T> &vec) {
  vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
}

inline i64 write_string(void *buf, std::string_view str) {
  memcpy(buf, str.data(), str.size());
  *((u8 *)buf + str.size()) = '\0';
  return str.size() + 1;
}

template <typename T>
inline i64 write_vector(void *buf, const std::vector<T> &vec) {
  i64 sz = vec.size() * sizeof(T);
  memcpy(buf, vec.data(), sz);
  return sz;
}

inline void encode_uleb(std::vector<u8> &vec, u64 val) {
  do {
    u8 byte = val & 0x7f;
    val >>= 7;
    vec.push_back(val ? (byte | 0x80) : byte);
  } while (val);
}

inline void encode_sleb(std::vector<u8> &vec, i64 val) {
  for (;;) {
    u8 byte = val & 0x7f;
    val >>= 7;

    bool neg = (byte & 0x40);
    if ((val == 0 && !neg) || (val == -1 && neg)) {
      vec.push_back(byte);
      break;
    }
    vec.push_back(byte | 0x80);
  }
}

inline i64 write_uleb(u8 *buf, u64 val) {
  i64 i = 0;
  do {
    u8 byte = val & 0x7f;
    val >>= 7;
    buf[i++] = val ? (byte | 0x80) : byte;
  } while (val);
  return i;
}

inline u64 read_uleb(u8 *&buf) {
  u64 val = 0;
  u8 shift = 0;
  u8 byte;
  do {
    byte = *buf++;
    val |= (byte & 0x7f) << shift;
    shift += 7;
  } while (byte & 0x80);
  return val;
}

inline u64 read_uleb(u8 const*&buf) {
  return read_uleb(const_cast<u8 *&>(buf));
}

inline u64 read_uleb(std::string_view &str) {
  u8 *start = (u8 *)&str[0];
  u8 *ptr = start;
  u64 val = read_uleb(ptr);
  str = str.substr(ptr - start);
  return val;
}

inline i64 uleb_size(u64 val) {
#if __GNUC__
#pragma GCC unroll 8
#endif
  for (int i = 1; i < 9; i++)
    if (val < (1LL << (7 * i)))
      return i;
  return 9;
}

template <typename Context>
std::string_view save_string(Context &ctx, const std::string &str) {
  u8 *buf = new u8[str.size() + 1];
  memcpy(buf, str.data(), str.size());
  buf[str.size()] = '\0';
  ctx.string_pool.push_back(std::unique_ptr<u8[]>(buf));
  return {(char *)buf, str.size()};
}

inline bool remove_prefix(std::string_view &s, std::string_view prefix) {
  if (s.starts_with(prefix)) {
    s = s.substr(prefix.size());
    return true;
  }
  return false;
}

//
// Concurrent Map
//

// This is an implementation of a fast concurrent hash map. Unlike
// ordinary hash tables, this impl just aborts if it becomes full.
// So you need to give a correct estimation of the final size before
// using it. We use this hash map to uniquify pieces of data in
// mergeable sections.
template <typename T>
class ConcurrentMap {
public:
  ConcurrentMap() {}

  ConcurrentMap(i64 nbuckets) {
    resize(nbuckets);
  }

  ~ConcurrentMap() {
    if (keys) {
      free((void *)keys);
      free((void *)key_sizes);
      free((void *)values);
    }
  }

  void resize(i64 nbuckets) {
    this->~ConcurrentMap();

    nbuckets = std::max<i64>(MIN_NBUCKETS, bit_ceil(nbuckets));

    this->nbuckets = nbuckets;
    keys = (std::atomic<const char *> *)calloc(nbuckets, sizeof(char *));
    key_sizes = (u32 *)malloc(nbuckets * sizeof(u32));
    values = (T *)malloc(nbuckets * sizeof(T));
  }

  std::pair<T *, bool> insert(std::string_view key, u64 hash, const T &val) {
    if (!keys)
      return {nullptr, false};

    assert(has_single_bit(nbuckets));
    i64 idx = hash & (nbuckets - 1);
    i64 retry = 0;

    while (retry < MAX_RETRY) {
      const char *ptr = keys[idx].load(std::memory_order_acquire);
      if (ptr == marker) {
        pause();
        continue;
      }

      if (ptr == nullptr) {
        if (!keys[idx].compare_exchange_weak(ptr, marker,
                                             std::memory_order_acquire))
          continue;
        new (values + idx) T(val);
        key_sizes[idx] = key.size();
        keys[idx].store(key.data(), std::memory_order_release);
        return {values + idx, true};
      }

      if (key.size() == key_sizes[idx] &&
          memcmp(ptr, key.data(), key_sizes[idx]) == 0)
        return {values + idx, false};

      u64 mask = nbuckets / NUM_SHARDS - 1;
      idx = (idx & ~mask) | ((idx + 1) & mask);
      retry++;
    }

    assert(false && "ConcurrentMap is full");
    return {nullptr, false};
  }

  const char *get_key(i64 idx) {
    return keys[idx].load(std::memory_order_relaxed);
  }

  static constexpr i64 MIN_NBUCKETS = 2048;
  static constexpr i64 NUM_SHARDS = 16;
  static constexpr i64 MAX_RETRY = 128;

  i64 nbuckets = 0;
  u32 *key_sizes = nullptr;
  T *values = nullptr;

private:
  static void pause() {
#if defined(__x86_64__)
    asm volatile("pause");
#elif defined(__aarch64__)
    asm volatile("yield");
#endif
  }

private:
  std::atomic<const char *> *keys = nullptr;
  static constexpr const char *marker = "marker";
};

//
// output-file.h
//

template <typename Context>
class OutputFile {
public:
  static std::unique_ptr<OutputFile<Context>>
  open(Context &ctx, std::string path, i64 filesize, i64 perm);

  virtual void close(Context &ctx) = 0;
  virtual ~OutputFile() = default;

  u8 *buf = nullptr;
  std::string path;
  i64 filesize;
  bool is_mmapped;
  bool is_unmapped = false;

protected:
  OutputFile(std::string path, i64 filesize, bool is_mmapped)
    : path(path), filesize(filesize), is_mmapped(is_mmapped) {}
};

//
// hyperloglog.cc
//

class HyperLogLog {
public:
  HyperLogLog() : buckets(NBUCKETS) {}

  void insert(u32 hash) {
    update_maximum(buckets[hash & (NBUCKETS - 1)], std::countl_zero(hash) + 1);
  }

  i64 get_cardinality() const;

  void merge(const HyperLogLog &other) {
    for (i64 i = 0; i < NBUCKETS; i++)
      update_maximum(buckets[i], other.buckets[i]);
  }

private:
  static constexpr i64 NBUCKETS = 2048;
  static constexpr double ALPHA = 0.79402;

  std::vector<std::atomic_uint8_t> buckets;
};

//
// glob.cc
//

class Glob {
  typedef enum { STRING, STAR, QUESTION, BRACKET } Kind;

  struct Element {
    Element(Kind k) : kind(k) {}
    Kind kind;
    std::string str;
    std::bitset<256> bitset;
  };

public:
  static std::optional<Glob> compile(std::string_view pat);
  bool match(std::string_view str);

private:
  Glob(std::vector<Element> &&vec) : elements(vec) {}
  static bool do_match(std::string_view str, std::span<Element> elements);

  std::vector<Element> elements;
};

//
// multi-glob.cc
//

class MultiGlob {
public:
  bool add(std::string_view pat, u32 val);
  bool empty() const { return strings.empty(); }
  std::optional<u32> find(std::string_view str);

private:
  struct TrieNode {
    u32 value = -1;
    TrieNode *suffix_link = nullptr;
    std::unique_ptr<TrieNode> children[256];
  };

  void compile();
  void fix_suffix_links(TrieNode &node);
  void fix_values();

  std::vector<std::string> strings;
  std::unique_ptr<TrieNode> root;
  std::vector<std::pair<Glob, u32>> globs;
  std::once_flag once;
  bool is_compiled = false;
};

//
// uuid.cc
//

std::array<u8, 16> get_uuid_v4();

//
// filepath.cc
//

template <typename T>
std::filesystem::path filepath(const T &path) {
  return {path, std::filesystem::path::format::generic_format};
}

std::string get_realpath(std::string_view path);
std::string path_clean(std::string_view path);
std::filesystem::path to_abs_path(std::filesystem::path path);

//
// demangle.cc
//

std::string_view demangle(std::string_view name);
std::optional<std::string_view> cpp_demangle(std::string_view name);

//
// compress.cc
//

class Compressor {
public:
  virtual void write_to(u8 *buf) = 0;
  virtual ~Compressor() {}
  i64 compressed_size = 0;
};

class ZlibCompressor : public Compressor {
public:
  ZlibCompressor(u8 *buf, i64 size);
  void write_to(u8 *buf) override;

private:
  std::vector<std::vector<u8>> shards;
  u64 checksum = 0;
};

class ZstdCompressor : public Compressor {
public:
  ZstdCompressor(u8 *buf, i64 size);
  void write_to(u8 *buf) override;

private:
  std::vector<std::vector<u8>> shards;
};

//
// perf.cc
//

// Counter is used to collect statistics numbers.
class Counter {
public:
  Counter(std::string_view name, i64 value = 0) : name(name) {
    static std::mutex mu;
    std::scoped_lock lock(mu);
    instances.push_back(this);
    values[this] = value;
  }

  Counter &operator++(int) {
    if (enabled) [[unlikely]]
      values[this]++;
    return *this;
  }

  Counter &operator+=(int delta) {
    if (enabled) [[unlikely]]
      values[this] += delta;
    return *this;
  }

  static void print();

  static inline bool enabled = false;

private:
  i64 get_value();

  std::string_view name;
  static thread_local std::unordered_map<Counter*, i64> values;

  static inline std::vector<Counter *> instances;
};

// Timer and TimeRecord records elapsed time (wall clock time)
// used by each pass of the linker.
struct TimerRecord {
  TimerRecord(std::string name, TimerRecord *parent = nullptr);
  void stop();

  std::string name;
  TimerRecord *parent;
  std::vector<TimerRecord *> children;
  i64 start;
  i64 end;
  i64 user;
  i64 sys;
  bool stopped = false;
};

void
print_timer_records(std::vector<std::unique_ptr<TimerRecord>> &);

template <typename Context>
class Timer {
public:
  Timer(Context &ctx, std::string name, Timer *parent = nullptr) {
    record = new TimerRecord(name, parent ? parent->record : nullptr);
    ctx.timer_records.push_back(std::unique_ptr<TimerRecord>(record));
  }

  Timer(const Timer &) = delete;

  ~Timer() {
    record->stop();
  }

  void stop() {
    record->stop();
  }

private:
  TimerRecord *record;
};

//
// tar.cc
//

// TarFile is a class to create a tar file.
//
// If you pass `--repro` to mold, mold collects all input files and
// put them into `<output-file-path>.repro.tar`, so that it is easy to
// run the same command with the same command line arguments.
class TarWriter {
public:
  static std::unique_ptr<TarWriter>
  open(std::string output_path, std::string basedir);

  ~TarWriter();
  void append(std::string path, std::string_view data);

private:
  static constexpr i64 BLOCK_SIZE = 512;

  TarWriter(FILE *out, std::string basedir) : out(out), basedir(basedir) {}

  FILE *out = nullptr;
  std::string basedir;
};

//
// Memory-mapped file
//

// MappedFile represents an mmap'ed input file.
// mold uses mmap-IO only.
template <typename Context>
class MappedFile {
public:
  static MappedFile *open(Context &ctx, std::string path);
  static MappedFile *must_open(Context &ctx, std::string path);

  ~MappedFile() { unmap(); }
  void unmap();

  MappedFile *slice(Context &ctx, std::string name, u64 start, u64 size);

  std::string_view get_contents() {
    return std::string_view((char *)data, size);
  }

  i64 get_offset() const {
    return parent ? (data - parent->data + parent->get_offset()) : 0;
  }

  // Returns a string that uniquely identify a file that is possibly
  // in an archive.
  std::string get_identifier() const {
    if (parent) {
      // We use the file offset within an archive as an identifier
      // because archive members may have the same name.
      return parent->name + ":" + std::to_string(get_offset());
    }

    if (thin_parent) {
      // If this is a thin archive member, the filename part is
      // guaranteed to be unique.
      return thin_parent->name + ":" + name;
    }
    return name;
  }

  std::string name;
  u8 *data = nullptr;
  i64 size = 0;
  bool given_fullpath = true;
  MappedFile *parent = nullptr;
  MappedFile *thin_parent = nullptr;
  int fd = -1;
#ifdef _WIN32
  HANDLE file_handle = INVALID_HANDLE_VALUE;
#endif
};

template <typename Context>
MappedFile<Context> *MappedFile<Context>::open(Context &ctx, std::string path) {
  if (path.starts_with('/') && !ctx.arg.chroot.empty())
    path = ctx.arg.chroot + "/" + path_clean(path);

  i64 fd;
#ifdef _WIN32
    fd = ::_open(path.c_str(), O_RDONLY);
#else
    fd = ::open(path.c_str(), O_RDONLY);
#endif

  if (fd == -1) {
    if (errno != ENOENT)
      Fatal(ctx) << "opening " << path << " failed: " << errno_string();
    return nullptr;
  }

  struct stat st;
  if (fstat(fd, &st) == -1)
    Fatal(ctx) << path << ": fstat failed: " << errno_string();

  MappedFile *mf = new MappedFile;
  ctx.mf_pool.push_back(std::unique_ptr<MappedFile>(mf));

  mf->name = path;
  mf->size = st.st_size;

  if (st.st_size > 0) {
#ifdef _WIN32
    HANDLE handle = CreateFileMapping((HANDLE)_get_osfhandle(fd),
                                      nullptr, PAGE_READWRITE, 0,
                                      st.st_size, nullptr);
    if (!handle)
      Fatal(ctx) << path << ": CreateFileMapping failed: " << GetLastError();
    mf->file_handle = handle;
    mf->data = (u8 *)MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, 0, 0, st.st_size);
    if (!mf->data)
      Fatal(ctx) << path << ": MapViewOfFile failed: " << GetLastError();
#else
    mf->data = (u8 *)mmap(nullptr, st.st_size, PROT_READ | PROT_WRITE,
                          MAP_PRIVATE, fd, 0);
    if (mf->data == MAP_FAILED)
      Fatal(ctx) << path << ": mmap failed: " << errno_string();
#endif
    }

  close(fd);
  return mf;
}

template <typename Context>
MappedFile<Context> *
MappedFile<Context>::must_open(Context &ctx, std::string path) {
  if (MappedFile *mf = MappedFile::open(ctx, path))
    return mf;
  Fatal(ctx) << "cannot open " << path << ": " << errno_string();
}

template <typename Context>
MappedFile<Context> *
MappedFile<Context>::slice(Context &ctx, std::string name, u64 start, u64 size) {
  MappedFile *mf = new MappedFile;
  mf->name = name;
  mf->data = data + start;
  mf->size = size;
  mf->parent = this;

  ctx.mf_pool.push_back(std::unique_ptr<MappedFile>(mf));
  return mf;
}

template <typename Context>
void MappedFile<Context>::unmap() {
  if (size == 0 || parent || !data)
    return;

#ifdef _WIN32
  UnmapViewOfFile(data);
  if (file_handle != INVALID_HANDLE_VALUE)
    CloseHandle(file_handle);
#else
  munmap(data, size);
#endif

  data = nullptr;
}

} // namespace mold
