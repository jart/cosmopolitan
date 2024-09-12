// Copyright 2024 Justine Alexandra Roberts Tunney
//
// Permission to use, copy, modify, and/or distribute this software for
// any purpose with or without fee is hereby granted, provided that the
// above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include "libc/calls/calls.h"
#include "libc/elf/def.h"
#include "libc/elf/scalar.h"
#include "libc/elf/struct/ehdr.h"
#include "libc/elf/struct/shdr.h"
#include "libc/elf/struct/sym.h"
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/serialize.h"
#include "libc/stdio/sysparam.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "third_party/getopt/getopt.internal.h"

bool FLAG_quiet;
const char *FLAG_prefix;
const char *FLAG_suffix;
const char *path;

wontreturn void PrintUsage(int fd, int exitcode) {
  tinyprint(fd, "\n\
NAME\n\
\n\
  Cosmopolitan Symbol Renamer\n\
\n\
SYNOPSIS\n\
\n\
  ",
            path, " -s SUFFIX OBJECT...\n\
\n\
DESCRIPTION\n\
\n\
  This program rewrites ELF objects (i.e. foo.o files) so that an\n\
  arbitrary string is appended to each public symbol's name.\n\
\n\
FLAGS\n\
\n\
  -h            show help\n\
  -q            quiet mode\n\
  -p PREFIX     specifies symbol prefix\n\
  -s SUFFIX     specifies symbol suffix\n\
\n\
",
            NULL);
  exit(exitcode);
}

wontreturn void Die(const char *reason) {
  tinyprint(2, path, ": ", reason, "\n", NULL);
  exit(1);
}

wontreturn void DieSys(const char *func) {
  tinyprint(2, path, ": ", func, " failed with ", strerror(errno), "\n", NULL);
  exit(1);
}

wontreturn void DieOom(void) {
  Die("out of memory");
}

static void *Malloc(size_t n) {
  void *p;
  if (!(p = malloc(n)))
    DieOom();
  return p;
}

static void *Realloc(void *p, size_t n) {
  if (!(p = realloc(p, n)))
    DieOom();
  return p;
}

void ProcessFile(void) {

  // open file
  int fildes;
  if ((fildes = open(path, O_RDWR)) == -1)
    DieSys("open");

  // read elf header
  ssize_t got;
  Elf64_Ehdr ehdr;
  if ((got = pread(fildes, &ehdr, sizeof(ehdr), 0)) == -1)
    DieSys("pread");
  if (got != sizeof(ehdr))
    DieSys("file too small");
  if (READ32LE(ehdr.e_ident) != READ32LE(ELFMAG))
    DieSys("file doesn't have elf magic");
  if (ehdr.e_ident[EI_CLASS] == ELFCLASS32)
    DieSys("32-bit elf isn't supported");
  if (ehdr.e_shentsize != sizeof(Elf64_Shdr))
    Die("unsupported e_shentsize");
  if (!ehdr.e_shnum)
    Die("elf has no section headers");

  // read elf section headers
  ssize_t need = ehdr.e_shnum * sizeof(Elf64_Shdr);
  Elf64_Shdr *shdrs = Malloc(need);
  if ((got = pread(fildes, shdrs, need, ehdr.e_shoff)) == -1)
    DieSys("pread");
  if (got != need)
    Die("found truncated elf section headers");

  // find elf symbol table
  char *stab;
  Elf64_Xword stablen;
  Elf64_Sym *syms = 0;
  Elf64_Xword sym0 = 0;
  Elf64_Xword symcount = 0;
  Elf64_Off fixoff_symtab_offset = 0;
  Elf64_Off fixoff_strtab_offset = 0;
  Elf64_Off fixoff_strtab_size = 0;
  for (int i = 0; i < ehdr.e_shnum; ++i) {
    if (shdrs[i].sh_type == SHT_SYMTAB) {

      // read elf symbol table
      if (!shdrs[i].sh_size)
        Die("elf symbol table empty");
      if (shdrs[i].sh_entsize != sizeof(Elf64_Sym))
        Die("unsupported sht_symtab sh_entsize");
      need = shdrs[i].sh_size;
      syms = Malloc(need);
      if ((got = pread(fildes, syms, need, shdrs[i].sh_offset)) == -1)
        DieSys("pread");
      if (got != need)
        Die("truncated elf symbol table");
      symcount = need / sizeof(Elf64_Sym);
      sym0 = shdrs[i].sh_info;

      // read elf string table
      if (!(0 <= shdrs[i].sh_link && shdrs[i].sh_link < ehdr.e_shnum))
        Die("out of range section index");
      int j = shdrs[i].sh_link;
      if (shdrs[j].sh_type != SHT_STRTAB)
        Die("invalid string table reference");
      stablen = shdrs[j].sh_size;
      stab = Malloc(stablen + 1);
      stab[stablen] = 0;
      if ((got = pread(fildes, stab, stablen, shdrs[j].sh_offset)) == -1)
        DieSys("pread");
      if (got != stablen)
        Die("truncated elf symbol table");

      // save file offsets of fields we're going to mutate later
      fixoff_symtab_offset = ehdr.e_shoff + i * sizeof(Elf64_Shdr) +
                             offsetof(Elf64_Shdr, sh_offset);
      fixoff_strtab_offset = ehdr.e_shoff + j * sizeof(Elf64_Shdr) +
                             offsetof(Elf64_Shdr, sh_offset);
      fixoff_strtab_size = (ehdr.e_shoff + j * sizeof(Elf64_Shdr) +
                            offsetof(Elf64_Shdr, sh_size));
      break;
    }
  }
  if (!syms)
    Die("elf sht_symtab section not found");

  // make better names for non-local symbols
  int renamed = 0;
  size_t prefixlen = strlen(FLAG_prefix);
  size_t suffixlen = strlen(FLAG_suffix);
  for (Elf64_Xword i = sym0; i < symcount; ++i) {
    if (syms[i].st_shndx == SHN_ABS)
      continue;
    if (syms[i].st_shndx == SHN_UNDEF)
      continue;
    if (syms[i].st_name >= stablen)
      Die("out of bounds symbol name");
    if (!strlen(stab + syms[i].st_name))
      Die("found function with empty string as its name");
    Elf64_Word name = syms[i].st_name;
    size_t namelen = strlen(stab + name);
    if (ELF64_ST_TYPE(syms[i].st_info) != STT_FUNC) {
      // resymbol is intended for compiling mathematical objects
      // multiple times, for all the cpu microarchitectures. any
      // global variables should be moved to a different object.
      const char *kind = "non-function symbol";
      if (ELF64_ST_TYPE(syms[i].st_info) == STT_OBJECT ||
          ELF64_ST_TYPE(syms[i].st_info) == STT_COMMON)
        kind = "global variable";
      else if (ELF64_ST_TYPE(syms[i].st_info) == STT_TLS)
        kind = "tls global variable";
      else if (ELF64_ST_TYPE(syms[i].st_info) == STT_GNU_IFUNC)
        kind = "ifunc or target_clones";
      tinyprint(2, path, ": resymbol won't rewrite object with ", kind, ": ",
                stab + name, "\n", NULL);
      exit(2);
    }
    size_t len = prefixlen + namelen + suffixlen;
    size_t newname = stablen;
    if (newname + len > 0x7fffffff)
      Die("elf string table too long");
    stablen += len + 1;
    stab = Realloc(stab, stablen);
    char *p = stab + newname;
    if (prefixlen)
      p = mempcpy(p, FLAG_prefix, prefixlen);
    p = mempcpy(p, stab + name, namelen);
    p = mempcpy(p, FLAG_suffix, suffixlen + 1);
    syms[i].st_name = newname;
    if (!FLAG_quiet)
      tinyprint(1, stab + name, " ", stab + newname, "\n", NULL);
    ++renamed;
  }
  if (!renamed)
    Die("object doesn't have any non-local symbols");

  // get size of object file
  ssize_t filesize;
  if ((filesize = lseek(fildes, 0, SEEK_END)) == -1)
    DieSys("lseek");

  // write out new symbol table
  ssize_t wrote;
  Elf64_Off symtaboff = (filesize + 63) & -64;
  Elf64_Xword symtablen = symcount * sizeof(Elf64_Sym);
  if ((wrote = pwrite(fildes, syms, symtablen, symtaboff)) == -1)
    DieSys("pwrite");
  if (wrote != symtablen)
    Die("write truncated");
  filesize = symtaboff + symtablen;

  // write out new string table
  Elf64_Off staboff = (filesize + 63) & -64;
  if ((wrote = pwrite(fildes, stab, stablen, staboff)) == -1)
    DieSys("pwrite");
  if (wrote != stablen)
    Die("write truncated");
  filesize = staboff + stablen;

  // write new symbol table offset to elf section header
  if ((wrote = pwrite(fildes, &symtaboff, sizeof(symtaboff),
                      fixoff_symtab_offset)) == -1)
    DieSys("pwrite");
  if (wrote != sizeof(symtaboff))
    Die("write truncated");

  // write new string table offset to elf section header
  if ((wrote = pwrite(fildes, &staboff, sizeof(staboff),
                      fixoff_strtab_offset)) == -1)
    DieSys("pwrite");
  if (wrote != sizeof(staboff))
    Die("write truncated");

  // write new string table size to elf section header
  if ((wrote = pwrite(fildes, &stablen, sizeof(stablen),  //
                      fixoff_strtab_size)) == -1)
    DieSys("pwrite");
  if (wrote != sizeof(staboff))
    Die("write truncated");

  // finish work
  if (close(fildes))
    DieSys("close");
}

int main(int argc, char *argv[]) {

  // get program name
  if (!(path = argv[0]))
    path = "resymbol";

  // parse flags
  int opt;
  while ((opt = getopt(argc, argv, "p:s:qh")) != -1) {
    switch (opt) {
      case 'q':
        FLAG_quiet = true;
        break;
      case 'p':
        if (strchr(optarg, ' '))
          Die("symbols with space character unsupported");
        FLAG_prefix = optarg;
        break;
      case 's':
        if (strchr(optarg, ' '))
          Die("symbols with space character unsupported");
        FLAG_suffix = optarg;
        break;
      case 'h':
        PrintUsage(1, 0);
      default:
        PrintUsage(2, 1);
    }
  }
  if (optind == argc)
    Die("missing operand");
  if (!FLAG_prefix && !FLAG_suffix)
    Die("missing name mangling flag");
  if (!FLAG_prefix)
    FLAG_prefix = "";
  if (!FLAG_suffix)
    FLAG_suffix = "";

  // process arguments
  for (int i = optind; i < argc; ++i) {
    path = argv[i];
    ProcessFile();
  }
}
