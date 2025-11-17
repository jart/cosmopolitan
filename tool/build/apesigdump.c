/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2025 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/cosmo.h"
#include "libc/elf/def.h"
#include "libc/limits.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/serialize.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/sysparam.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/x/x.h"
#include "libc/zip.h"
#include "net/http/apesig.h"
#include "net/http/ssh.h"

const char *prog;

[[noreturn]] void Die(const char *thing, const char *reason) {
  fputs(thing, stderr);
  fputs(": fatal error: ", stderr);
  fputs(reason, stderr);
  fputs("\n", stderr);
  exit(1);
}

[[noreturn]] void DieSys(const char *thing) {
  perror(thing);
  exit(1);
}

[[noreturn]] void DieOom(void) {
  Die(prog, "out of memory");
}

void *Malloc(size_t n) {
  void *p;
  if (!(p = malloc(n)))
    DieOom();
  return p;
}

void *Realloc(void *p, size_t n) {
  if (!(p = realloc(p, n)))
    DieOom();
  return p;
}

const char *DescribeAlgorithm(int type) {
  switch (type) {
    case APESIG_ALGORITHM_ED25519:
      return "ed25519";
    default:
      return "<illegal>";
  }
}

const char *DescribeElfMachine(int emachine) {
  switch (emachine) {
    case EM_NEXGEN32E:
      return "amd64";
    case EM_AARCH64:
      return "arm64";
    case EM_PPC64:
      return "ppc64";
    case EM_S390:
      return "s390x";
    case EM_RISCV:
      return "riscv";
    default:
      return "<illegal>";
  }
}

const char *DescribeElfPhdrType(int type) {
  switch (type) {
    case PT_LOAD:
      return "PT_LOAD";
    default:
      return "<illegal>";
  }
}

const char *DescribeElfPhdrFlags(int flags) {
  switch (flags & (PF_R | PF_W | PF_X)) {
    case PF_R:
      return "r";
    case PF_X:
      return "x";
    case PF_R | PF_X:
      return "rx";
    case PF_R | PF_W:
      return "rw";
    case PF_R | PF_W | PF_X:
      return "rwx";
    default:
      return "<illegal>";
  }
}

void PrintApeBytes(struct ApeBytes *obj, int indent) {
  printf("%*ssize: %u\n", indent, "", obj->size);
  printf("%*sdata: %`#.*s\n", indent, "", (int)obj->size, obj->data);
}

void PrintApeSigSig(struct ApeSigSig *obj, int indent) {
  printf("%*salgorithm: %d (%s)\n", indent, "", obj->algorithm,
         DescribeAlgorithm(obj->algorithm));
  printf("%*ssig:\n", indent, "");
  PrintApeBytes(&obj->sig, indent + 2);
}

void PrintApeSigKey(struct ApeSigKey *obj, int indent) {
  printf("%*salgorithm: %d (%s)\n", indent, "", obj->algorithm,
         DescribeAlgorithm(obj->algorithm));
  printf("%*spub:\n", indent, "");
  PrintApeBytes(&obj->pub, indent + 2);
}

void PrintApeSigElfPhdr(struct ApeSigElfPhdr *obj, int indent) {
  printf("%*stype: %d (%s)\n", indent, "", obj->type,
         DescribeElfPhdrType(obj->type));
  printf("%*sflags: %d (%s)\n", indent, "", obj->flags,
         DescribeElfPhdrFlags(obj->flags));
  printf("%*ssig:\n", indent, "");
  PrintApeSigSig(&obj->sig, indent + 2);
}

void PrintApeSigElf(struct ApeSigElf *obj, int indent) {
  printf("%*smachine: %d (%s)\n", indent, "", obj->machine,
         DescribeElfMachine(obj->machine));
  printf("%*ssig:\n", indent, "");
  PrintApeSigSig(&obj->sig, indent + 2);
  printf("%*sphdrs_count: %d\n", indent, "", obj->phdrs_count);
  for (int i = 0; i < obj->phdrs_count; ++i) {
    printf("%*sphdrs[%d]:\n", indent, "", i);
    PrintApeSigElfPhdr(&obj->phdrs[i], indent + 2);
  }
}

void PrintApeSig(struct ApeSig *obj, int indent) {
  printf("%*spath: %`#s\n", indent, "", obj->path);
  printf("%*sissued: %ld\n", indent, "", obj->issued);
  printf("%*sexpires: %ld\n", indent, "", obj->expires);
  printf("%*sidentity: %`#s\n", indent, "", obj->identity);
  printf("%*sgithub: %`#s\n", indent, "", obj->github);
  printf("%*skey:\n", indent, "");
  PrintApeSigKey(&obj->key, indent + 2);
  printf("%*selfs_count: %d\n", indent, "", obj->elfs_count);
  for (int i = 0; i < obj->elfs_count; ++i) {
    printf("%*selfs[%d]:\n", indent, "", i);
    PrintApeSigElf(&obj->elfs[i], indent + 2);
  }
}

void PrintApeSigs(struct ApeSigs *obj, int indent) {
  printf("%*smagic: 0x%016lx\n", indent, "", obj->magic);
  printf("%*ssigs_count: %d\n", indent, "", obj->sigs_count);
  for (int i = 0; i < obj->sigs_count; ++i) {
    printf("%*ssigs[%d]:\n", indent, "", i);
    PrintApeSig(&obj->sigs[i], indent + 2);
  }
}

void DumpApeSigsData(const char *path, const uint8_t *data, size_t size) {
  int err;
  struct ApeSigs sigs;
  if ((err = ParseApeSigs(data, size, &sigs))) {
    char message[64];
    DescribeApeSigError(message, sizeof(message), err);
    Die(path, message);
  }
  printf("%s:\n", path);
  PrintApeSigs(&sigs, 2);
  DestroyApeSigs(&sigs);
}

void DumpApeSigsFile(const char *path, int fd, size_t zsize) {
  size_t size;
  uint8_t *data;
  if (!(data = xslurp(path, &size))) {
    perror(path);
    exit(1);
  }
  DumpApeSigsData(path, data, size);
  free(data);
}

void DumpApeSigsFromExecutable(const char *path, int fd, size_t zsize) {

  // read last 512 bytes of file
  int amt;
  off_t off;
  if (zsize <= 512) {
    off = 0;
    amt = zsize;
  } else {
    off = zsize - 512;
    amt = zsize - off;
  }
  char last64[512];
  if (pread(fd, last64, amt, off) != amt)
    DieSys(path);

  // search backwards for end-of-central-directory (eocd) record
  // the eocd (cdir) says where the central directory (cfile) array is located
  // we consistency check some legacy fields, to be extra sure that it is eocd
  unsigned cnt = 0;
  for (int i = amt - MIN(kZipCdirHdrMinSize, kZipCdir64LocatorSize); i >= 0;
       --i) {
    uint32_t magic = ZIP_READ32(last64 + i);
    if (magic == kZipCdir64LocatorMagic && i + kZipCdir64LocatorSize <= amt &&
        pread(fd, last64, kZipCdir64HdrMinSize,
              ZIP_LOCATE64_OFFSET(last64 + i)) == (long)kZipCdir64HdrMinSize &&
        ZIP_READ32(last64) == kZipCdir64HdrMagic &&
        ZIP_CDIR64_RECORDS(last64) == ZIP_CDIR64_RECORDSONDISK(last64) &&
        ZIP_CDIR64_RECORDS(last64) && ZIP_CDIR64_SIZE(last64) <= INT_MAX &&
        ZIP_CDIR64_RECORDS(last64) <= INT_MAX) {
      cnt = ZIP_CDIR64_RECORDS(last64);
      off = ZIP_CDIR64_OFFSET(last64);
      amt = ZIP_CDIR64_SIZE(last64);
      break;
    }
    if (magic == kZipCdirHdrMagic && i + kZipCdirHdrMinSize <= amt &&
        ZIP_CDIR_RECORDS(last64 + i) == ZIP_CDIR_RECORDSONDISK(last64 + i) &&
        ZIP_CDIR_RECORDS(last64 + i) && ZIP_CDIR_SIZE(last64 + i) <= INT_MAX &&
        ZIP_CDIR_RECORDS(last64 + i) <= INT_MAX &&
        ZIP_CDIR_OFFSET(last64 + i) != 0xffffffffu) {
      cnt = ZIP_CDIR_RECORDS(last64 + i);
      off = ZIP_CDIR_OFFSET(last64 + i);
      amt = ZIP_CDIR_SIZE(last64 + i);
      break;
    }
  }
  if (!cnt)
    Die(path, "no zip file named '" APESIG_ZIP_NAME "' found in executable");

  // read central directory
  uint8_t *cdir = Malloc(amt);
  if (pread(fd, cdir, amt, off) != amt)
    Die(path, "failed to read zip central directory");

  // extract existing signatures
  unsigned i = 0;
  unsigned entry_offset;
  for (entry_offset = 0;
       entry_offset + kZipCfileHdrMinSize <= amt && i < cnt &&
       entry_offset + ZIP_CFILE_HDRSIZE(cdir + entry_offset) <= amt;
       entry_offset += ZIP_CFILE_HDRSIZE(cdir + entry_offset), ++i) {
    if (ZIP_CFILE_MAGIC(cdir + entry_offset) != kZipCfileHdrMagic)
      Die(path, "corrupted zip central directory entry magic");
    if (ZIP_CFILE_NAMESIZE(cdir + entry_offset) == strlen(APESIG_ZIP_NAME) &&
        !memcmp(ZIP_CFILE_NAME(cdir + entry_offset), APESIG_ZIP_NAME,
                ZIP_CFILE_NAMESIZE(cdir + entry_offset))) {
      if (ZIP_CFILE_COMPRESSIONMETHOD(cdir + entry_offset))
        Die(path, APESIG_ZIP_NAME " file in zip was compressed");
      int64_t lfileoffset;
      if ((lfileoffset = GetZipCfileOffset(cdir + entry_offset)) == -1)
        Die(path, "bad zip local file offset");
      uint8_t *lfile = Malloc(kZipLfileHdrMinSize);
      if (pread(fd, lfile, kZipLfileHdrMinSize, lfileoffset) !=
          kZipLfileHdrMinSize)
        Die(path, "failed to read zip local file header");
      lfile = Realloc(lfile, ZIP_LFILE_HDRSIZE(lfile));
      if (pread(fd, lfile, ZIP_LFILE_HDRSIZE(lfile), lfileoffset) !=
          ZIP_LFILE_HDRSIZE(lfile))
        Die(path, "failed to read zip local file header");
      int64_t size;
      if ((size = GetZipLfileUncompressedSize(lfile)) == -1)
        Die(path, "failed to read zip asset size");
      uint8_t *data = Malloc(size);
      if (pread(fd, data, size, lfileoffset + ZIP_LFILE_HDRSIZE(lfile)) != size)
        Die(path, "failed to read zip asset data");
      free(lfile);
      free(cdir);
      DumpApeSigsData(path, data, size);
      free(data);
      return;
    }
  }
  Die(path, "no zip file named '" APESIG_ZIP_NAME "' found in executable");
}

void DumpApeSigs(const char *path) {

  // open file
  int fd;
  ssize_t zsize;
  if ((fd = open(path, O_RDONLY)) == -1)
    DieSys(path);
  if ((zsize = lseek(fd, 0, SEEK_END)) == -1)
    DieSys(path);

  // determine if sig file or executable
  uint8_t head[64];
  if (pread(fd, head, 64, 0) != 64 ||  //
      (READ32LE(head) != READ32LE("\177ELF") &&
       READ64LE(head) != READ64LE("MZqFpD='") &&
       READ64LE(head) != READ64LE("jartsr='") &&
       READ64LE(head) != READ64LE("APEDBG='"))) {
    DumpApeSigsFile(path, fd, zsize);
  } else {
    DumpApeSigsFromExecutable(path, fd, zsize);
  }

  // close file
  close(fd);
}

void GetProg(char **argv) {
  prog = argv[0];
  if (!prog)
    prog = "apesigdump";
}

int main(int argc, char *argv[]) {
  GetProg(argv);
  for (int i = 1; i < argc; ++i)
    DumpApeSigs(argv[i]);
}
