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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/weirdtypes.h"
#include "libc/cosmotime.h"
#include "libc/ctype.h"
#include "libc/dos.h"
#include "libc/elf/def.h"
#include "libc/elf/struct/ehdr.h"
#include "libc/elf/struct/phdr.h"
#include "libc/fmt/libgen.h"
#include "libc/fmt/wintime.internal.h"
#include "libc/limits.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/serialize.h"
#include "libc/stdckdint.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/sysparam.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/s.h"
#include "libc/time.h"
#include "libc/x/x.h"
#include "libc/zip.h"
#include "net/http/apesig.h"
#include "net/http/ssh.h"
#include "third_party/getopt/getopt.internal.h"
#include "third_party/haclstar/haclstar.h"
#include "third_party/zlib/zlib.h"

static const char *const help = "\
Copyright 2025 Justine Alexandra Roberts Tunney - ISC license\n\
Actually Portable Executable Code Signing Tool 1.0. Usage:\n\
apesign [FLAGS...] [PATH...]\n\
  -k PATH  path of ed25519 private key [defaults to ~/.ssh/id_ed25519]\n\
  -o PATH  puts sig in file (rather than adding to executable)\n\
  -i NAME  identity of signer [defaults to user@hostname]\n\
  -g USER  github username of signer [defaults to empty]\n\
  -p PATH  override pathname of executable in signature\n\
  -n       print an error if my key already signed it\n\
  -h       show this help information\n\
  -v       increase verbosity\n\
\n";

static int verbosity;
static const char *prog;
static bool dont_clobber;
static const char *output_path;
static const char *override_path;
static const char *signer_github;
static const char *signer_identity;
static bool signer_identity_specified;
static const char *private_key_path;
static uint8_t private_key[33];
static uint8_t public_key[32];

[[noreturn]] static void Die(const char *thing, const char *reason) {
  fputs(thing, stderr);
  fputs(": fatal error: ", stderr);
  fputs(reason, stderr);
  fputs("\n", stderr);
  exit(1);
}

[[noreturn]] static void DieSys(const char *thing) {
  perror(thing);
  exit(1);
}

[[noreturn]] static void DieOom(void) {
  Die(prog, "out of memory");
}

static void *Malloc(size_t n) {
  void *p;
  if (!(p = malloc(n)))
    DieOom();
  return p;
}

static char *StrDup(const char *s) {
  char *p;
  if (!(p = strdup(s)))
    DieOom();
  return p;
}

static void *Realloc(void *p, size_t n) {
  if (!(p = realloc(p, n)))
    DieOom();
  return p;
}

static char *StrCat(const char *a, const char *b) {
  char *p;
  size_t n, m;
  n = strlen(a);
  m = strlen(b);
  p = Malloc(n + m + 1);
  if (n)
    memcpy(p, a, n);
  memcpy(p + n, b, m + 1);
  return p;
}

static bool IsLegalGithub(const char *s) {
  for (; *s; ++s)
    if (!isalnum(*s) && *s != '-')
      return false;
  return true;
}

static bool IsLegalFilename(const char *s) {
  for (; *s; ++s)
    if (!isalnum(*s) &&  //
        *s != '-' &&     //
        *s != '_' &&     //
        *s != '.' &&     //
        *s != '/')
      return false;
  return true;
}

static bool IsLegalIdentity(const char *s) {
  for (; *s; ++s)
    if (!isalnum(*s) &&  //
        *s != ':' &&     //
        *s != '/' &&     //
        *s != '<' &&     //
        *s != '>' &&     //
        *s != '-' &&     //
        *s != '_' &&     //
        *s != '.' &&     //
        *s != ' ' &&     //
        *s != '@')
      return false;
  return true;
}

static void GetDosLocalTime(int64_t utcunixts, uint16_t *out_time,
                            uint16_t *out_date) {
  struct tm tm;
  localtime_r(&utcunixts, &tm);
  *out_time = DOS_TIME(tm.tm_hour, tm.tm_min, tm.tm_sec);
  *out_date = DOS_DATE(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

static void GetProg(char **argv) {
  prog = argv[0];
  if (!prog)
    prog = "apesign";
}

static bool FileExists(const char *path) {
  struct stat st;
  return !stat(path, &st);
}

static const char *GetDefaultPrivateKeyPath(void) {
  const char *path;
  if (!getuid()) {
    path = "/etc/ape.key";
    if (FileExists(path))
      return path;
    path = "/etc/ssh/ssh_host_ed25519_key.pub";
    if (FileExists(path))
      return path;
  }
  const char *home;
  if ((home = getenv("HOME"))) {
    path = StrCat(home, "/.ape.key");
    if (FileExists(path))
      return path;
    return StrCat(home, "/.ssh/id_ed25519");
  }
  return "ape.key";
}

static const char *GetDefaultIdentity(void) {
  const char *user;
  signer_github = "";
  if (!(user = getenv("USER")))
    user = "unknown";
  char host[128];
  if (gethostname(host, sizeof(host)))
    strcpy(host, "unknown");
  return StrCat(user, StrCat("@", host));
}

static void SetDefaults(void) {
  signer_identity = GetDefaultIdentity();
  private_key_path = GetDefaultPrivateKeyPath();
}

static void GetOpts(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "hnvg:i:k:o:p:")) != -1) {
    switch (opt) {
      case 'v':
        ++verbosity;
        break;
      case 'o':
        output_path = optarg;
        break;
      case 'n':
        dont_clobber = true;
        break;
      case 'p':
        override_path = optarg;
        break;
      case 'k':
        private_key_path = optarg;
        break;
      case 'i':
        signer_identity = optarg;
        signer_identity_specified = true;
        break;
      case 'g':
        signer_github = optarg;
        break;
      case 'h':
        fputs(help, stdout);
        exit(0);
      default:
        fputs(help, stderr);
        exit(1);
    }
  }
  if (!IsLegalIdentity(signer_identity))
    Die(prog, "signer identity has illegal characters");
  if (!IsLegalGithub(signer_github))
    Die(prog, "signer github username has illegal characters");
}

static void LoadPrivateKey(void) {
  char *s;
  size_t n;
  if (!(s = xslurp(private_key_path, &n)))
    DieSys(private_key_path);
  int err;
  char *comment;
  if ((err = ParseEd25519PrivateKey(s, n, private_key, &comment))) {
    char errstr[64];
    DescribeOpensshParseError(errstr, sizeof(errstr), err);
    Die(private_key_path, errstr);
  }
  if (!signer_identity_specified)
    signer_identity = comment;
  Hacl_Ed25519_secret_to_public(public_key, private_key);
}

static char *ChooseSigPath(const char *zpath) {
  if (override_path)
    return StrDup(override_path);
  if (IsLegalFilename(zpath))
    return StrDup(zpath);
  char *copy = StrDup(zpath);
  char *base = basename(copy);
  if (IsLegalFilename(base)) {
    char *res = StrDup(base);
    free(copy);
    return res;
  }
  free(copy);
  return 0;
}

static struct ApeSigSig Sign(const uint8_t private_key[32],  //
                             uint32_t len, const Hacl_Slice *msg) {
  struct ApeSigSig sig;
  sig.algorithm = APESIG_ALGORITHM_ED25519;
  sig.sig.size = 64;
  sig.sig.data = Malloc(64);
  Hacl_Ed25519_sign(sig.sig.data, private_key, len, msg);
  return sig;
}

static bool SignElf(struct ApeSig *sig, const char *epath, int efd,
                    const Elf64_Ehdr *ehdr) {

  // sanity check elf header
  if (ehdr->e_ident[EI_CLASS] == ELFCLASS32)
    return false;  // just ignore 32-bit elf binaries
  if (ehdr->e_phentsize != sizeof(Elf64_Phdr))
    Die(epath, "e_phentsize is wrong");

  // add record to signature file for this embedded elf binary
  if (ckd_add(&sig->elfs_count, sig->elfs_count, 1))
    Die(epath, "too many elf executables");
  sig->elfs = Realloc(sig->elfs, sig->elfs_count * sizeof(*sig->elfs));
  struct ApeSigElf *elfsig = &sig->elfs[sig->elfs_count - 1];
  memset(elfsig, 0, sizeof(*elfsig));
  elfsig->machine = ehdr->e_machine;

  // sign ehdr+phdrs concatenated
  size_t size = ehdr->e_phnum;
  alignas(16) uint8_t buf[1024];
  if ((size *= sizeof(Elf64_Phdr)) > 1024)
    Die(epath, "too many ELF program headers");
  ssize_t rc = pread(efd, buf, size, ehdr->e_phoff);
  if (rc == -1)
    Die(epath, "failed to read ELF program headers");
  if (rc != size)
    Die(epath, "truncated read of ELF program headers");
  elfsig->sig = Sign(private_key, 3,
                     (Hacl_Slice[]){
                         {1, (uint8_t[]){APESIG_SIG_ELF}},
                         {sizeof(*ehdr), ehdr},
                         {size, buf},
                     });

  // read load segments in program headers
  bool gotsome = false;
  Elf64_Phdr *phdr = (Elf64_Phdr *)buf;
  for (int i = 0; i < ehdr->e_phnum; ++i) {
    if (phdr[i].p_type == PT_INTERP)
      Die(epath, "ELF has PT_INTERP which isn't supported");
    if (phdr[i].p_type == PT_DYNAMIC)
      Die(epath, "ELF has PT_DYNAMIC which isn't supported");
    if (!phdr[i].p_memsz)
      continue;
    if (phdr[i].p_type != PT_LOAD)
      continue;
    if (phdr[i].p_filesz > phdr[i].p_memsz)
      Die(epath, "ELF p_filesz exceeds p_memsz");
    if (phdr[i].p_vaddr + phdr[i].p_memsz < phdr[i].p_vaddr ||
        phdr[i].p_vaddr + phdr[i].p_memsz + 65535 < phdr[i].p_vaddr)
      Die(epath, "ELF program header virtual interval corrupt");
    if (phdr[i].p_offset + phdr[i].p_filesz < phdr[i].p_offset ||
        phdr[i].p_offset + phdr[i].p_filesz + 65535 < phdr[i].p_offset)
      Die(epath, "ELF program header file interval corrupt");
    if (phdr[i].p_filesz > 0x7ffff000)
      Die(epath, "ELF program header data too large");
    if (!phdr[i].p_filesz)
      continue;

    // add record to signature file for this program header
    if (ckd_add(&elfsig->phdrs_count, elfsig->phdrs_count, 1))
      Die(epath, "too many elf program headers");
    elfsig->phdrs =
        Realloc(elfsig->phdrs, elfsig->phdrs_count * sizeof(*elfsig->phdrs));
    struct ApeSigElfPhdr *phdrsig = &elfsig->phdrs[elfsig->phdrs_count - 1];
    memset(phdrsig, 0, sizeof(*phdrsig));
    phdrsig->type = phdr[i].p_type;
    phdrsig->flags = phdr[i].p_flags;

    // load program header content
    size_t size = phdr[i].p_filesz;
    uint8_t *data = Malloc(size);
    if (pread(efd, data, size, phdr[i].p_offset) != size)
      Die(epath, "failed to read ELF program header");

    // sign program header content
    phdrsig->sig = Sign(private_key, 3,
                        (Hacl_Slice[]){
                            {1, (uint8_t[]){APESIG_SIG_PHDR}},
                            {sizeof(phdr[i]), &phdr[i]},
                            {size, data},
                        });

    // finish the job
    gotsome = true;
    free(data);
  }
  if (!gotsome)
    Die(epath, "found elf binary with no executable segments");
  return true;
}

static void ApeSign(const char *zpath) {

  // open executable
  int zfd;
  int oflags;
  long long zsize;
  if (output_path) {
    oflags = O_RDONLY;
  } else {
    oflags = O_RDWR;
  }
  if ((zfd = open(zpath, oflags, 0644)) == -1)
    DieSys(zpath);
  if ((zsize = lseek(zfd, 0, SEEK_END)) == -1)
    DieSys(zpath);

  // read last 512 bytes of file
  int amt;
  long long off;
  if (zsize <= 512) {
    off = 0;
    amt = zsize;
  } else {
    off = zsize - 512;
    amt = zsize - off;
  }
  char last64[512];
  if (pread(zfd, last64, amt, off) != amt)
    DieSys(zpath);

  // search backwards for end-of-central-directory (eocd) record
  // the eocd (cdir) says where the central directory (cfile) array is located
  // we consistency check some legacy fields, to be extra sure that it is eocd
  unsigned cnt = 0;
  bool have_zip = false;
  for (int i = amt - MIN(kZipCdirHdrMinSize, kZipCdir64LocatorSize); i >= 0;
       --i) {
    uint32_t magic = ZIP_READ32(last64 + i);
    if (magic == kZipCdir64LocatorMagic && i + kZipCdir64LocatorSize <= amt &&
        pread(zfd, last64, kZipCdir64HdrMinSize,
              ZIP_LOCATE64_OFFSET(last64 + i)) == (long)kZipCdir64HdrMinSize &&
        ZIP_READ32(last64) == kZipCdir64HdrMagic &&
        ZIP_CDIR64_RECORDS(last64) == ZIP_CDIR64_RECORDSONDISK(last64) &&
        ZIP_CDIR64_RECORDS(last64) && ZIP_CDIR64_SIZE(last64) <= INT_MAX) {
      cnt = ZIP_CDIR64_RECORDS(last64);
      off = ZIP_CDIR64_OFFSET(last64);
      amt = ZIP_CDIR64_SIZE(last64);
      have_zip = true;
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
      have_zip = true;
      break;
    }
  }
  if (!have_zip) {
    cnt = 0;
    amt = 0;
    off = zsize;
  }

  // read central directory
  size_t cdirsize = amt;
  size_t cdircount = cnt;
  size_t cdiroffset = off;
  uint8_t *cdir = Malloc(cdirsize);
  if (cdirsize)
    if (pread(zfd, cdir, cdirsize, cdiroffset) != cdirsize)
      Die(zpath, "failed to read zip central directory");

  // extract existing signatures
  int verify_count = 0;
  unsigned entry_offset;
  struct ApeSig *sig = 0;
  struct ApeSigs sigs = {APESIG_MAGIC};
  uint8_t *sigsdata = 0;  // `sigs` points into this
  for (entry_offset = 0;
       entry_offset + kZipCfileHdrMinSize <= cdirsize &&
       entry_offset + ZIP_CFILE_HDRSIZE(cdir + entry_offset) <= cdirsize;
       entry_offset += ZIP_CFILE_HDRSIZE(cdir + entry_offset), ++verify_count) {
    if (ZIP_CFILE_MAGIC(cdir + entry_offset) != kZipCfileHdrMagic)
      Die(zpath, "corrupted zip central directory entry magic");
    if (verify_count < cdircount &&
        ZIP_CFILE_NAMESIZE(cdir + entry_offset) == strlen(APESIG_ZIP_NAME) &&
        !memcmp(ZIP_CFILE_NAME(cdir + entry_offset), APESIG_ZIP_NAME,
                ZIP_CFILE_NAMESIZE(cdir + entry_offset))) {
      if (ZIP_CFILE_COMPRESSIONMETHOD(cdir + entry_offset))
        Die(zpath, APESIG_ZIP_NAME " file in zip was compressed");
      int64_t lfileoffset;
      if ((lfileoffset = GetZipCfileOffset(cdir + entry_offset)) == -1)
        Die(zpath, "bad zip local file offset");
      uint8_t *lfile = Malloc(kZipLfileHdrMinSize);
      if (pread(zfd, lfile, kZipLfileHdrMinSize, lfileoffset) !=
          kZipLfileHdrMinSize)
        Die(zpath, "failed to read zip local file header");
      lfile = Realloc(lfile, ZIP_LFILE_HDRSIZE(lfile));
      if (pread(zfd, lfile, ZIP_LFILE_HDRSIZE(lfile), lfileoffset) !=
          ZIP_LFILE_HDRSIZE(lfile))
        Die(zpath, "failed to read zip local file header");
      int64_t size;
      if ((size = GetZipLfileUncompressedSize(lfile)) == -1)
        Die(zpath, "failed to read zip asset size");
      free(sigsdata);
      sigsdata = Malloc(size);
      if (pread(zfd, sigsdata, size, lfileoffset + ZIP_LFILE_HDRSIZE(lfile)) !=
          size)
        Die(zpath, "failed to read zip asset data");
      int err;
      sig = 0;
      DestroyApeSigs(&sigs);
      if ((err = ParseApeSigs(sigsdata, size, &sigs))) {
        char message[64];
        DescribeApeSigError(message, sizeof(message), err);
        Die(zpath, message);
      }
      // replace signature if our key already signed it
      for (int i = 0; i < sigs.sigs_count; ++i) {
        if (sigs.sigs[i].key.algorithm == APESIG_ALGORITHM_ED25519 &&
            sigs.sigs[i].key.pub.size == 32 &&
            !memcmp(sigs.sigs[i].key.pub.data, public_key, 32)) {
          sig = &sigs.sigs[i];
          break;
        }
      }
      free(lfile);
    }
  }

  // start signature
  if (!sig) {
    if (ckd_add(&sigs.sigs_count, sigs.sigs_count, 1))
      Die(zpath, "too many ape signatures");
    sigs.sigs = Realloc(sigs.sigs, sigs.sigs_count * sizeof(*sigs.sigs));
    sig = &sigs.sigs[sigs.sigs_count - 1];
  } else {
    if (dont_clobber)
      Die(zpath, "executable already signed by same key");
    DestroyApeSig(sig);
  }
  memset(sig, 0, sizeof(*sig));
  struct timespec issued = timespec_real();
  sig->path = ChooseSigPath(zpath);
  sig->issued = issued.tv_sec;
  sig->identity = StrDup(signer_identity);
  sig->github = StrDup(signer_github);
  sig->key.algorithm = APESIG_ALGORITHM_ED25519;
  sig->key.pub.size = 32;
  sig->key.pub.data = Malloc(32);
  memcpy(sig->key.pub.data, public_key, 32);

  // sign program headers of elf executables in file
  ssize_t rc;
  bool gotsome = false;
  size_t headsize = 8192;
  uint8_t *head = Malloc(headsize);
  alignas(16) uint8_t ehdrbuf[64];
  if ((rc = pread(zfd, head, headsize, 0)) == -1)
    DieSys(zpath);
  headsize = rc;
  if (headsize >= 64 && READ32LE(head) == READ32LE("\177ELF")) {
    // this is an elf executable
    if (SignElf(sig, zpath, zfd, (Elf64_Ehdr *)head))
      gotsome = true;
  } else if (READ64LE(head) == READ64LE("MZqFpD='") ||
             READ64LE(head) == READ64LE("jartsr='") ||
             READ64LE(head) == READ64LE("APEDBG='")) {
    for (uint8_t *p = head, *pe = p + headsize; p < pe; ++p) {
      if (READ64LE(p) != READ64LE("printf '"))
        continue;
      int c;
      size_t i;
      for (i = 0, p += 8; p + 3 < pe && (c = *p++) != '\'';) {
        if (c == '\\') {
          if ('0' <= *p && *p <= '7') {
            c = *p++ - '0';
            if ('0' <= *p && *p <= '7') {
              c *= 8;
              c += *p++ - '0';
              if ('0' <= *p && *p <= '7') {
                c *= 8;
                c += *p++ - '0';
              }
            }
          }
        }
        ehdrbuf[i++] = c;
        if (i >= headsize)
          break;
      }
      if (i >= sizeof(ehdrbuf))
        if (READ32LE(ehdrbuf) == READ32LE("\177ELF"))
          if (SignElf(sig, zpath, zfd, (Elf64_Ehdr *)ehdrbuf))
            gotsome = true;
    }
  } else {
    Die(zpath, "file must be ape or elf executable");
  }
  if (!gotsome)
    Die(zpath, "ape or elf executable had no suitable elf binaries");
  free(head);

  // serialize signatures
  int err;
  struct ApeBytes sigbytes;
  if ((err = CheckApeSigs(&sigs)) ||
      (err = SerializeApeSigs(&sigs, &sigbytes))) {
    char message[64];
    DescribeApeSigError(message, sizeof(message), err);
    Die(zpath, message);
  }
  DestroyApeSigs(&sigs);

  // write sigs to output file instead if -o flag was passed
  if (output_path) {
    close(zfd);
    int fd;
    if ((fd = creat(output_path, 0644)) == -1)
      DieSys(output_path);
    if ((rc = write(fd, sigbytes.data, sigbytes.size)) == -1)
      DieSys(output_path);
    if (rc != sigbytes.size)
      Die(output_path, "failed to write signatures to output");
    if (close(fd))
      DieSys(output_path);
    return;
  }

  // check if old central directory is snug against end of file
  // i.e. only thing that follows cfiles in file is eocd record
  bool is_snug = false;
  if (have_zip && verify_count == cdircount && entry_offset == cdirsize) {
    size_t extraoffset = cdiroffset + cdirsize;
    size_t extrasize = zsize - extraoffset;
    uint8_t *extra = Malloc(extrasize);
    if (pread(zfd, extra, extrasize, extraoffset) == extrasize) {
      size_t i = 0;
      while (i < extrasize) {
        if (extrasize - i >= kZipCdir64HdrMinSize &&
            ZIP_CDIR64_MAGIC(extra + i) == kZipCdir64HdrMagic &&
            ZIP_CDIR64_HDRSIZE(extra + i) <= extrasize - i) {
          i += ZIP_CDIR64_HDRSIZE(extra + i);
        } else if (extrasize - i >= kZipCdir64LocatorSize &&
                   ZIP_LOCATE64_MAGIC(extra + i) == kZipCdir64HdrMagic) {
          i += kZipCdir64LocatorSize;
        } else if (extrasize - i >= kZipCdirHdrMinSize &&
                   ZIP_CDIR_MAGIC(extra + i) == kZipCdirHdrMagic &&
                   ZIP_CDIR_HDRSIZE(extra + i) <= extrasize - i) {
          i += ZIP_CDIR_HDRSIZE(extra + i);
        } else {
          break;
        }
      }
      if (i == extrasize) {
        is_snug = true;
      }
    }
    free(extra);
  }

  // delete old central directory if it's snug
  if (is_snug)
    if (!ftruncate(zfd, cdiroffset))
      zsize = cdiroffset;

  // write out zip local file record for signature
  uint16_t mtime, mdate;
  GetDosLocalTime(issued.tv_sec, &mtime, &mdate);
  uint32_t crc = crc32_z(0, sigbytes.data, sigbytes.size);
  char *name = APESIG_ZIP_NAME;
  size_t lfileoff = zsize;
  size_t namlen = strlen(name);
  size_t extlen = (2 + 2 + 8 * 2);
  size_t hdrlen = kZipLfileHdrMinSize + namlen + extlen;
  uint8_t *lochdr = Malloc(hdrlen);
  uint8_t *p = lochdr;
  p = ZIP_WRITE32(p, kZipLfileHdrMagic);
  p = ZIP_WRITE16(p, kZipEra2001);
  p = ZIP_WRITE16(p, 0);
  p = ZIP_WRITE16(p, kZipCompressionNone);
  p = ZIP_WRITE16(p, mtime);
  p = ZIP_WRITE16(p, mdate);
  p = ZIP_WRITE32(p, crc);
  p = ZIP_WRITE32(p, 0xffffffffu);  // compressed size
  p = ZIP_WRITE32(p, 0xffffffffu);  // uncompressed size
  p = ZIP_WRITE16(p, namlen);
  p = ZIP_WRITE16(p, extlen);
  p = mempcpy(p, name, namlen);
  p = ZIP_WRITE16(p, kZipExtraZip64);
  p = ZIP_WRITE16(p, 8 + 8);
  p = ZIP_WRITE64(p, sigbytes.size);  // uncompressed size
  p = ZIP_WRITE64(p, sigbytes.size);  // compressed size
  unassert(p == lochdr + hdrlen);
  if (pwrite(zfd, lochdr, hdrlen, zsize) != hdrlen)
    DieSys(zpath);
  free(lochdr);
  zsize += hdrlen;

  // write out the signature file data
  if (pwrite(zfd, sigbytes.data, sigbytes.size, zsize) != sigbytes.size)
    DieSys(zpath);
  zsize += sigbytes.size;
  free(sigbytes.data);

  // write signature record as first entry of central directory
  size_t cdirsize2 = 0;
  size_t cdircount2 = 0;
  size_t cdiroffset2 = zsize;
  extlen = (2 + 2 + 8 * 3) + (2 + 2 + 4 + 2 + 2 + 8 * 1);
  hdrlen = kZipCfileHdrMinSize + namlen + extlen;
  uint64_t mt = TimeSpecToWindowsTime(issued);
  uint16_t iattrs = 0;
  uint32_t eattrs = (uint32_t)(S_IFREG | 0644) << 16;
  uint8_t *cdirhdr = Malloc(hdrlen);
  p = cdirhdr;
  p = ZIP_WRITE32(p, kZipCfileHdrMagic);
  p = ZIP_WRITE16(p, kZipOsUnix << 8 | kZipEra2001);  // version made by
  p = ZIP_WRITE16(p, kZipEra2001);  // version needed to extract
  p = ZIP_WRITE16(p, 0);
  p = ZIP_WRITE16(p, kZipCompressionNone);
  p = ZIP_WRITE16(p, mtime);
  p = ZIP_WRITE16(p, mdate);
  p = ZIP_WRITE32(p, crc);
  p = ZIP_WRITE32(p, 0xffffffffu);  // compressed size
  p = ZIP_WRITE32(p, 0xffffffffu);  // uncompressed size
  p = ZIP_WRITE16(p, namlen);
  p = ZIP_WRITE16(p, extlen);
  p = ZIP_WRITE16(p, 0);            // comment length
  p = ZIP_WRITE16(p, 0);            // disk number start
  p = ZIP_WRITE16(p, iattrs);       // internal file attributes
  p = ZIP_WRITE32(p, eattrs);       // external file attributes
  p = ZIP_WRITE32(p, 0xffffffffu);  // lfile offset
  p = mempcpy(p, name, namlen);
  p = ZIP_WRITE16(p, kZipExtraZip64);
  p = ZIP_WRITE16(p, 8 + 8 + 8);
  p = ZIP_WRITE64(p, sigbytes.size);  // uncompressed size
  p = ZIP_WRITE64(p, sigbytes.size);  // compressed size
  p = ZIP_WRITE64(p, lfileoff);       // lfile offset
  p = ZIP_WRITE16(p, kZipExtraNtfs);
  p = ZIP_WRITE16(p, 4 + 2 + 2 + 8 * 1);
  p = ZIP_WRITE32(p, 0);      // ntfs reserved
  p = ZIP_WRITE16(p, 1);      // ntfs attribute tag
  p = ZIP_WRITE16(p, 8 * 1);  // ntfs attribute size
  p = ZIP_WRITE64(p, mt);     // last modification time
  unassert(p == cdirhdr + hdrlen);
  if (pwrite(zfd, cdirhdr, hdrlen, zsize) != hdrlen)
    DieSys(zpath);
  free(cdirhdr);
  cdirsize2 += hdrlen;
  cdircount2 += 1;
  zsize += hdrlen;

  // copy old central directory entries into file
  size_t count;
  for (count = entry_offset = 0;
       count < cdircount && entry_offset + kZipCfileHdrMinSize <= cdirsize &&
       entry_offset + ZIP_CFILE_HDRSIZE(cdir + entry_offset) <= cdirsize;
       entry_offset += ZIP_CFILE_HDRSIZE(cdir + entry_offset), ++count) {
    if (ZIP_CFILE_NAMESIZE(cdir + entry_offset) == strlen(APESIG_ZIP_NAME) &&
        !memcmp(ZIP_CFILE_NAME(cdir + entry_offset), APESIG_ZIP_NAME,
                ZIP_CFILE_NAMESIZE(cdir + entry_offset)))
      continue;
    if (pwrite(zfd, cdir + entry_offset, ZIP_CFILE_HDRSIZE(cdir + entry_offset),
               zsize) != ZIP_CFILE_HDRSIZE(cdir + entry_offset))
      DieSys(zpath);
    zsize += ZIP_CFILE_HDRSIZE(cdir + entry_offset);
    cdirsize2 += ZIP_CFILE_HDRSIZE(cdir + entry_offset);
    cdircount2 += 1;
  }
  free(cdir);

  // write out end of central directory
  uint8_t
      eocd[kZipCdirHdrMinSize + kZipCdir64HdrMinSize + kZipCdir64LocatorSize];
  p = eocd;
  p = ZIP_WRITE32(p, kZipCdir64HdrMagic);
  p = ZIP_WRITE64(p, kZipCdir64HdrMinSize - 12);      // size of eocd64
  p = ZIP_WRITE16(p, kZipOsUnix << 8 | kZipEra2001);  // version made by
  p = ZIP_WRITE16(p, kZipEra2001);  // version needed to extract
  p = ZIP_WRITE32(p, 0);            // number of this disk
  p = ZIP_WRITE32(p, 0);  // number of disk with start of central directory
  p = ZIP_WRITE64(p, cdircount2);   // number of records on disk
  p = ZIP_WRITE64(p, cdircount2);   // number of records
  p = ZIP_WRITE64(p, cdirsize2);    // size of central directory
  p = ZIP_WRITE64(p, cdiroffset2);  // offset of start of central directory
  p = ZIP_WRITE32(p, kZipCdir64LocatorMagic);
  p = ZIP_WRITE32(p, 0);      // number of disk with eocd64
  p = ZIP_WRITE64(p, zsize);  // offset of eocd64
  p = ZIP_WRITE32(p, 1);      // total number of disks
  p = ZIP_WRITE32(p, kZipCdirHdrMagic);
  p = ZIP_WRITE16(p, 0);            // number of this disk
  p = ZIP_WRITE16(p, 0);            // number of disks
  p = ZIP_WRITE16(p, cdircount2);   // number of records on disk
  p = ZIP_WRITE16(p, cdircount2);   // number of records
  p = ZIP_WRITE32(p, cdirsize2);    // size of central directory
  p = ZIP_WRITE32(p, 0xffffffffu);  // offset of central directory
  p = ZIP_WRITE16(p, 0);            // comment length
  unassert(p == eocd + sizeof(eocd));
  if (pwrite(zfd, eocd, sizeof(eocd), zsize) != sizeof(eocd))
    DieSys(zpath);

  // finish file
  if (close(zfd))
    DieSys(zpath);

  if (verbosity) {
    fputs(zpath, stderr);
    fputs(": signed executable\n", stderr);
  }
}

int main(int argc, char *argv[]) {
  GetProg(argv);
  SetDefaults();
  GetOpts(argc, argv);
  LoadPrivateKey();
  if (optind == argc)
    Die(prog, "missing argument");
  if (argc - optind > 1 && output_path)
    Die(prog, "too many arguments");
  for (int i = optind; i < argc; ++i)
    ApeSign(argv[i]);
}
