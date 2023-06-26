// clang-format off
#include "third_party/mold/common.h"

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

namespace mold {

inline u32 get_umask() {
  u32 orig_umask = umask(0);
  umask(orig_umask);
  return orig_umask;
}

template <typename Context>
static std::pair<i64, char *>
open_or_create_file(Context &ctx, std::string path, i64 filesize, i64 perm) {
  std::string tmpl = filepath(path).parent_path() / ".mold-XXXXXX";
  char *path2 = (char *)save_string(ctx, tmpl).data();

  i64 fd = mkstemp(path2);
  if (fd == -1)
    Fatal(ctx) << "cannot open " << path2 <<  ": " << errno_string();

  // Reuse an existing file if exists and writable because on Linux,
  // writing to an existing file is much faster than creating a fresh
  // file and writing to it.
  if (ctx.overwrite_output_file && rename(path.c_str(), path2) == 0) {
    ::close(fd);
    fd = ::open(path2, O_RDWR | O_CREAT, perm);
    if (fd != -1 && !ftruncate(fd, filesize) && !fchmod(fd, perm & ~get_umask()))
      return {fd, path2};

    unlink(path2);
    fd = ::open(path2, O_RDWR | O_CREAT, perm);
    if (fd == -1)
      Fatal(ctx) << "cannot open " << path2 << ": " << errno_string();
  }

  if (ftruncate(fd, filesize))
    Fatal(ctx) << "ftruncate failed: " << errno_string();

  if (fchmod(fd, (perm & ~get_umask())) == -1)
    Fatal(ctx) << "fchmod failed: " << errno_string();
  return {fd, path2};
}

template <typename Context>
class MemoryMappedOutputFile : public OutputFile<Context> {
public:
  MemoryMappedOutputFile(Context &ctx, std::string path, i64 filesize, i64 perm)
    : OutputFile<Context>(path, filesize, true) {
    i64 fd;
    std::tie(fd, output_tmpfile) = open_or_create_file(ctx, path, filesize, perm);

    this->buf = (u8 *)mmap(nullptr, filesize, PROT_READ | PROT_WRITE,
                           MAP_SHARED, fd, 0);
    if (this->buf == MAP_FAILED)
      Fatal(ctx) << path << ": mmap failed: " << errno_string();
    ::close(fd);

    mold::output_buffer_start = this->buf;
    mold::output_buffer_end = this->buf + filesize;
  }

  ~MemoryMappedOutputFile() {
    if (fd2 != -1)
      ::close(fd2);
  }

  void close(Context &ctx) override {
    Timer t(ctx, "close_file");

    if (!this->is_unmapped)
      munmap(this->buf, this->filesize);

    // If an output file already exists, open a file and then remove it.
    // This is the fastest way to unlink a file, as it does not make the
    // system to immediately release disk blocks occupied by the file.
    fd2 = ::open(this->path.c_str(), O_RDONLY);
    if (fd2 != -1)
      unlink(this->path.c_str());

    if (rename(output_tmpfile, this->path.c_str()) == -1)
      Fatal(ctx) << this->path << ": rename failed: " << errno_string();
    output_tmpfile = nullptr;
  }

private:
  int fd2 = -1;
};

template <typename Context>
class MallocOutputFile : public OutputFile<Context> {
public:
  MallocOutputFile(Context &ctx, std::string path, i64 filesize, i64 perm)
    : OutputFile<Context>(path, filesize, false), perm(perm) {
    this->buf = (u8 *)mmap(NULL, filesize, PROT_READ | PROT_WRITE,
                           MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (this->buf == MAP_FAILED)
      Fatal(ctx) << "mmap failed: " << errno_string();
  }

  void close(Context &ctx) override {
    Timer t(ctx, "close_file");

    if (this->path == "-") {
      fwrite(this->buf, this->filesize, 1, stdout);
      fclose(stdout);
      return;
    }

    i64 fd = ::open(this->path.c_str(), O_RDWR | O_CREAT, perm);
    if (fd == -1)
      Fatal(ctx) << "cannot open " << this->path << ": " << errno_string();

    FILE *fp = fdopen(fd, "w");
    fwrite(this->buf, this->filesize, 1, fp);
    fclose(fp);
  }

private:
  i64 perm;
};

template <typename Context>
std::unique_ptr<OutputFile<Context>>
OutputFile<Context>::open(Context &ctx, std::string path, i64 filesize, i64 perm) {
  Timer t(ctx, "open_file");

  if (path.starts_with('/') && !ctx.arg.chroot.empty())
    path = ctx.arg.chroot + "/" + path_clean(path);

  bool is_special = false;
  if (path == "-") {
    is_special = true;
  } else {
    struct stat st;
    if (stat(path.c_str(), &st) == 0 && (st.st_mode & S_IFMT) != S_IFREG)
      is_special = true;
  }

  OutputFile<Context> *file;
  if (is_special)
    file = new MallocOutputFile(ctx, path, filesize, perm);
  else
    file = new MemoryMappedOutputFile(ctx, path, filesize, perm);

#ifdef MADV_HUGEPAGE
  // Enable transparent huge page for an output memory-mapped file.
  // On Linux, it has an effect only on tmpfs mounted with `huge=advise`,
  // but it can make the linker ~10% faster. You can try it by creating
  // a tmpfs with the following commands
  //
  //  $ mkdir tmp
  //  $ sudo mount -t tmpfs -o size=2G,huge=advise none tmp
  //
  // and then specifying a path under the directory as an output file.
  madvise(file->buf, filesize, MADV_HUGEPAGE);
#endif

  if (ctx.arg.filler != -1)
    memset(file->buf, ctx.arg.filler, filesize);
  return std::unique_ptr<OutputFile>(file);
}

} // namespace mold
