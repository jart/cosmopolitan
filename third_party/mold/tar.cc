// clang-format off
#include "third_party/mold/common.h"

namespace mold {

// A tar file consists of one or more Ustar header followed by data.
// Each Ustar header represents a single file in an archive.
//
// tar is an old file format, and its `name` field is only 100 bytes long.
// If `name` is longer than 100 bytes, we can emit a PAX header before a
// Ustar header to store a long filename.
//
// For simplicity, we always emit a PAX header even for a short filename.
struct UstarHeader {
  UstarHeader() {
    memset(this, 0, sizeof(*this));
  }

  void finalize() {
    memset(checksum, ' ', sizeof(checksum));
    memcpy(magic, "ustar", 5);
    memcpy(version, "00", 2);

    // Compute checksum
    int sum = 0;
    for (i64 i = 0; i < sizeof(*this); i++)
      sum += ((u8 *)this)[i];

    // We need to convince the compiler that sum isn't too big to silence
    // -Werror=format-truncation.
    ASSUME(sum < 01'000'000);
    snprintf(checksum, sizeof(checksum), "%06o", sum);
  }

  char name[100];
  char mode[8];
  char uid[8];
  char gid[8];
  char size[12];
  char mtime[12];
  char checksum[8];
  char typeflag[1];
  char linkname[100];
  char magic[6];
  char version[2];
  char uname[32];
  char gname[32];
  char devmajor[8];
  char devminor[8];
  char prefix[155];
  char pad[12];
};

static std::string encode_path(std::string basedir, std::string path) {
  path = path_clean(basedir + "/" + path);

  // Construct a string which contains something like
  // "16 path=foo/bar\n" where 16 is the size of the string
  // including the size string itself.
  i64 len = std::string(" path=\n").size() + path.size();
  i64 total = std::to_string(len).size() + len;
  total = std::to_string(total).size() + len;
  return std::to_string(total) + " path=" + path + "\n";
}

std::unique_ptr<TarWriter>
TarWriter::open(std::string output_path, std::string basedir) {
  FILE *out = fopen(output_path.c_str(), "w");
  if (!out)
    return nullptr;
  return std::unique_ptr<TarWriter>(new TarWriter(out, basedir));
}

TarWriter::~TarWriter() {
  fclose(out);
}

void TarWriter::append(std::string path, std::string_view data) {
  // Write PAX header
  static_assert(sizeof(UstarHeader) == BLOCK_SIZE);
  UstarHeader pax;

  std::string attr = encode_path(basedir, path);
  snprintf(pax.size, sizeof(pax.size), "%011zo", attr.size());
  pax.typeflag[0] = 'x';
  pax.finalize();
  fwrite(&pax, sizeof(pax), 1, out);

  // Write pathname
  fwrite(attr.data(), attr.size(), 1, out);
  fseek(out, align_to(ftell(out), BLOCK_SIZE), SEEK_SET);

  // Write Ustar header
  UstarHeader ustar;
  memcpy(ustar.mode, "0000664", 8);
  snprintf(ustar.size, sizeof(ustar.size), "%011zo", data.size());
  ustar.finalize();
  fwrite(&ustar, sizeof(ustar), 1, out);

  // Write file contents
  fwrite(data.data(), data.size(), 1, out);
  fseek(out, align_to(ftell(out), BLOCK_SIZE), SEEK_SET);

  // A tar file must ends with two empty blocks, so write such
  // terminator and seek back.
  u8 terminator[BLOCK_SIZE * 2] = {};
  fwrite(&terminator, BLOCK_SIZE * 2, 1, out);
  fseek(out, -BLOCK_SIZE * 2, SEEK_END);

  assert(ftell(out) % BLOCK_SIZE == 0);
}

} // namespace mold
