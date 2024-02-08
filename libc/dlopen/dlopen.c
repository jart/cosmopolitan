/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/atomic.h"
#include "libc/calls/blockcancel.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/dlopen/dlfcn.h"
#include "libc/elf/def.h"
#include "libc/elf/elf.h"
#include "libc/elf/scalar.h"
#include "libc/elf/struct/auxv.h"
#include "libc/elf/struct/ehdr.h"
#include "libc/elf/struct/phdr.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/strace.internal.h"
#include "libc/limits.h"
#include "libc/nt/dll.h"
#include "libc/nt/enum/filemapflags.h"
#include "libc/nt/enum/pageflags.h"
#include "libc/nt/memory.h"
#include "libc/nt/runtime.h"
#include "libc/proc/posix_spawn.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/serialize.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/errfuns.h"
#include "libc/temp.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

/**
 * @fileoverview Cosmopolitan Dynamic Linker.
 *
 * Every program built using Cosmopolitan is statically-linked. However
 * there are some cases, e.g. GUIs and video drivers, where linking the
 * host platform libraries is desirable. So what we do in such cases is
 * launch a stub executable using the host platform's libc, and longjmp
 * back into this executable. The stub executable passes back to us the
 * platform-specific dlopen() implementation, which we shall then wrap.
 *
 * @kudos jacereda for figuring out how to do this
 */

#define AMD_REXB    0x41
#define AMD_REXW    0x48
#define AMD_MOV_IMM 0xb8

#define ARM_REG_OFF 0
#define ARM_IMM_OFF 5
#define ARM_IDX_OFF 21
#define ARM_MOV_NEX 0xf2800000u

#define XNU_RTLD_LAZY   1
#define XNU_RTLD_NOW    2
#define XNU_RTLD_LOCAL  4
#define XNU_RTLD_GLOBAL 8

#define HELPER \
  "#include <dlfcn.h>\n\
#include <stdio.h>\n\
#include <stdlib.h>\n\
int main(int argc, char **argv, char **envp) {\n\
  char *ep;\n\
  long addr;\n\
  if (argc != 2) {\n\
    fprintf(stderr, \"%s: not intended to be run directly\\n\", argv[0]);\n\
    return 1;\n\
  }\n\
  addr = strtol(argv[1], &ep, 10);\n\
  if (*ep) {\n\
    fprintf(stderr, \"%s: invalid function address\\n\", argv[0]);\n\
    return 2;\n\
  }\n\
  return ((int (*)(void *))addr)((void *[]){\n\
      dlopen,\n\
      dlsym,\n\
      dlclose,\n\
      dlerror,\n\
  });\n\
}\n"

struct Loaded {
  char *base;
  char *entry;
  Elf64_Ehdr eh;
  Elf64_Phdr ph[25];
};

struct {
  atomic_uint once;
  bool is_supported;
  struct CosmoTib *tib;
  void *(*dlopen)(const char *, int);
  void *(*dlsym)(void *, const char *);
  int (*dlclose)(void *);
  char *(*dlerror)(void);
  jmp_buf jb;
} __foreign;

long __sysv2nt14();
long foreign_tramp();

static _Thread_local char dlerror_buf[128];

static const char *get_tmp_dir(void) {
  const char *tmpdir;
  if (!(tmpdir = getenv("TMPDIR")) || !*tmpdir) {
    if (!(tmpdir = getenv("HOME")) || !*tmpdir) {
      tmpdir = ".";
    }
  }
  return tmpdir;
}

static int is_file_newer_than(const char *path, const char *other) {
  struct stat st1, st2;
  if (stat(path, &st1)) {
    return -1;
  }
  if (stat(other, &st2)) {
    if (errno == ENOENT) {
      return 2;
    } else {
      return -1;
    }
  }
  return timespec_cmp(st1.st_mtim, st2.st_mtim) > 0;
}

static unsigned elf2prot(unsigned x) {
  unsigned r = 0;
  if (x & PF_R) r += PROT_READ;
  if (x & PF_W) r += PROT_WRITE;
  if (x & PF_X) r += PROT_EXEC;
  return r;
}

static int get_host_elf_machine(void) {
#ifdef __x86_64__
  return EM_NEXGEN32E;
#elif defined(__aarch64__)
  return EM_AARCH64;
#elif defined(__powerpc64__)
  return EM_PPC64;
#elif defined(__riscv)
  return EM_RISCV;
#elif defined(__s390x__)
  return EM_S390;
#else
#error "unsupported architecture"
#endif
}

static char *elf_map(int fd, Elf64_Ehdr *ehdr, Elf64_Phdr *phdr, long pagesz,
                     char *interp_path, size_t interp_size) {
  Elf64_Addr maxva = 0;
  Elf64_Addr minva = -1;
  for (Elf64_Phdr *p = phdr; p < phdr + ehdr->e_phnum; p++) {
    if (p->p_type != PT_LOAD) {
      continue;
    }
    if (p->p_vaddr < minva) {
      minva = p->p_vaddr & -pagesz;
    }
    if (p->p_vaddr + p->p_memsz > maxva) {
      maxva = p->p_vaddr + p->p_memsz;
    }
  }
  uint8_t *base =
      __sys_mmap(0, maxva - minva, PROT_NONE,
                 MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0, 0);
  if (base == MAP_FAILED) {
    return MAP_FAILED;
  }
  for (Elf64_Phdr *p = phdr; p < phdr + ehdr->e_phnum; p++) {
    if (p->p_type != PT_LOAD) {
      if (p->p_type == PT_INTERP && interp_size &&
          (p->p_filesz >= interp_size - 1 ||
           pread(fd, interp_path, p->p_filesz, p->p_offset) != p->p_filesz)) {
        return MAP_FAILED;
      }
      continue;
    }
    Elf64_Addr skew = p->p_vaddr & (pagesz - 1);
    Elf64_Off off = p->p_offset - skew;
    Elf64_Addr a = p->p_vaddr + p->p_filesz;
    Elf64_Addr b = (a + (pagesz - 1)) & -pagesz;
    Elf64_Addr c = p->p_vaddr + p->p_memsz;
    int prot2 = elf2prot(p->p_flags);
    int prot1 = prot2;
    if (b > a) {
      prot1 |= PROT_WRITE;
      prot1 &= ~PROT_EXEC;
    }
    if (__sys_mmap(base + p->p_vaddr - skew, skew + p->p_filesz, prot1,
                   MAP_FIXED | MAP_PRIVATE, fd, off, off) == MAP_FAILED) {
      return MAP_FAILED;
    }
    if (b > a) {
      bzero(base + a, b - a);
    }
    if (c > b && __sys_mmap(base + b, c - b, prot2,
                            MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0,
                            0) == MAP_FAILED) {
      return MAP_FAILED;
    }
    if (prot1 != prot2 &&
        sys_mprotect(base + p->p_vaddr - skew, skew + p->p_filesz, prot2)) {
      return MAP_FAILED;
    }
  }
  return (void *)base;
}

static bool elf_slurp(struct Loaded *l, int fd, const char *file) {
  if (pread(fd, &l->eh, 64, 0) != 64) {
    return false;
  }
  if (!IsElf64Binary(&l->eh, 64) ||                      //
      l->eh.e_phnum > sizeof(l->ph) / sizeof(*l->ph) ||  //
      l->eh.e_machine != get_host_elf_machine()) {
    enoexec();
    return false;
  }
  int bytes = l->eh.e_phnum * sizeof(l->ph[0]);
  if (pread(fd, l->ph, bytes, l->eh.e_phoff) != bytes) {
    return false;
  }
  l->entry = (char *)l->eh.e_entry;
  return true;
}

static dontinline bool elf_load(struct Loaded *l, const char *file, long pagesz,
                                char *interp_path, size_t interp_size) {
  int fd;
  if ((fd = open(file, O_RDONLY | O_CLOEXEC)) == -1) {
    return false;
  }
  if (!elf_slurp(l, fd, file)) {
    close(fd);
    return false;
  }
  if ((l->base = elf_map(fd, &l->eh, l->ph, pagesz, interp_path,
                         interp_size)) == MAP_FAILED) {
    close(fd);
    return false;
  }
  l->entry += (uintptr_t)l->base;
  close(fd);
  return true;
}

static long *push_strs(long *sp, char **list, int count) {
  *--sp = 0;
  while (count) *--sp = (long)list[--count];
  return sp;
}

static wontreturn dontinstrument void foreign_helper(void **p) {
  __foreign.dlopen = p[0];
  __foreign.dlsym = p[1];
  __foreign.dlclose = p[2];
  __foreign.dlerror = p[3];
  _longjmp(__foreign.jb, 1);
}

static dontinline void elf_exec(const char *file, char **envp) {

  // get microprocessor page size
  long pagesz = getauxval(AT_PAGESZ);

  // load helper executable into address space
  struct Loaded prog;
  char interp_path[256] = {0};
  if (!elf_load(&prog, file, pagesz, interp_path, sizeof(interp_path))) {
    return;
  }

  // load platform c library into address space
  struct Loaded interp;
  if (!elf_load(&interp, interp_path, pagesz, 0, 0)) {
    return;
  }

  // count environment variables
  int envc = 0;
  while (envp[envc]) envc++;

  // count auxiliary values
  int auxc = 0;
  Elf64_auxv_t *av;
  for (av = (Elf64_auxv_t *)__auxv; av->a_type; ++av) auxc++;

  // create environment block for embedded process
  // the platform libc will save its location for getenv(), etc.
  // we need just enough stack memory beneath it for initialization
  char *map;
  size_t stksize = 65536;
  size_t stkalign = 8 * 2;
  size_t argsize = (1 + 2 + 1 + envc + 1 + auxc * 2 + 1 + 3) * 8;
  size_t mapsize = (stksize + argsize + (pagesz - 1)) & -pagesz;
  size_t skew = (mapsize - argsize) & (stkalign - 1);
  if (IsFreebsd()) skew += 8;  // FreeBSD calls _start() like a C function
  map = __sys_mmap(0, mapsize, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0, 0);
  if (map == MAP_FAILED) return;
  long *sp = (long *)(map + mapsize - skew);

  // push argument string
  char *address_argument = (char *)(sp -= 3);
  FormatInt64(address_argument, (uintptr_t)foreign_helper);

  // push auxiliary values
  // these tell the platform libc how to load the executable
  *--sp = 0;
  unsigned long key, val;
  for (av = (Elf64_auxv_t *)__auxv; (key = av->a_type); ++av) {
    val = av->a_un.a_val;
    if (key == AT_PHDR) val = (long)(prog.base + prog.eh.e_phoff);
    if (key == AT_PHENT) val = prog.eh.e_phentsize;
    if (key == AT_PHNUM) val = prog.eh.e_phnum;
    if (key == AT_PAGESZ) val = pagesz;
    if (key == AT_BASE) val = (long)interp.base;
    if (key == AT_FLAGS) val = 0;
    if (key == AT_ENTRY) val = (long)prog.entry;
    if (key == AT_EXECFN) val = (long)program_invocation_name;
    *--sp = val;
    *--sp = key;
  }

  // push environment variable pointers
  sp = push_strs(sp, envp, envc);
  envp = (char **)sp;

  // push argument pointers
  *--sp = 0;
  *--sp = (long)address_argument;
  *--sp = (long)program_invocation_name;
  char **argv = (char **)sp;
  (void)argv;
  int argc = 2;
  *--sp = argc;

  STRACE("running dlopen importer %p...", interp.entry);

  // XXX: ideally we should set most registers to zero
#ifdef __x86_64__
  struct ps_strings {
    char **argv;
    int argc;
    char **envp;
    int envc;
  } pss = {argv, argc, envp, envc};
  asm volatile("mov\t%2,%%rsp\n\t"
               "jmpq\t*%1"
               : /* no outputs */
               : "D"(IsFreebsd() ? sp : 0), "S"(interp.entry), "d"(sp),
                 "b"(IsNetbsd() ? &pss : 0)
               : "memory");
  __builtin_unreachable();
#elif defined(__aarch64__)
  register long x0 asm("x0") = IsFreebsd() ? (long)sp : 0;
  register long x9 asm("x9") = (long)sp;
  register long x16 asm("x16") = (long)interp.entry;
  asm volatile("mov\tsp,x9\n\t"
               "br\tx16"
               : /* no outputs */
               : "r"(x0), "r"(x9), "r"(x16)
               : "memory");
  __builtin_unreachable();
#else
#error "unsupported architecture"
#endif
}

static char *dlerror_set(const char *str) {
  strlcpy(dlerror_buf, str, sizeof(dlerror_buf));
  return dlerror_buf;
}

static dontinline char *foreign_alloc_block(void) {
  char *p = 0;
  size_t sz = 65536;
  if (!IsWindows()) {
    p = __sys_mmap(0, sz, PROT_READ | PROT_WRITE | PROT_EXEC,
                   MAP_PRIVATE | MAP_ANONYMOUS | MAP_JIT, -1, 0, 0);
    if (p == MAP_FAILED) {
      p = 0;
    }
  } else {
    uintptr_t h;
    if ((h = CreateFileMapping(-1, 0, kNtPageExecuteReadwrite, 0, sz, 0))) {
      p = MapViewOfFileEx(h, kNtFileMapWrite | kNtFileMapExecute, 0, 0, sz, 0);
      CloseHandle(h);
    }
  }
  if (p) {
    WRITE32LE(p, 4);  // store used index
  } else {
    dlerror_set("out of memory");
  }
  return p;
}

static dontinline void *foreign_alloc(size_t n) {
  void *res;
  static char *block;
  static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_lock(&lock);
  if (!block || READ32LE(block) + n > 65536) {
    if (!(block = foreign_alloc_block())) {
      return 0;
    }
  }
  res = block + READ32LE(block);
  WRITE32LE(block, READ32LE(block) + n);
  pthread_mutex_unlock(&lock);
  return res;
}

static uint8_t *movimm(uint8_t p[static 16], int reg, uint64_t val) {
#ifdef __x86_64__
  int rex;
  rex = AMD_REXW;
  if (reg & 8) {
    rex |= AMD_REXB;
  }
  *p++ = rex;
  *p++ = AMD_MOV_IMM | (reg & 7);
  p = WRITE64LE(p, val);
#elif defined(__aarch64__)
  // ARM immediate moves are encoded as:
  //
  //     ┌64-bit
  //     │
  //     │┌{sign,???,zero,non}-extending
  //     ││
  //     ││       ┌short[4] index
  //     ││       │
  //     ││  MOV  │    immediate   register
  //     │├┐┌─┴──┐├┐┌──────┴───────┐┌─┴─┐
  //   0bmxx100101iivvvvvvvvvvvvvvvvrrrrr
  //
  // Which allows 16 bits to be loaded into a register at a time, with
  // tricks for clearing other parts of the register. For example, the
  // sign-extending mode will set the higher order shorts to all ones,
  // and it expects the immediate to be encoded using ones' complement
  uint32_t op;
  for (unsigned i = 0; i < 4; ++i) {
    op = ARM_MOV_NEX;
    op |= i << ARM_IDX_OFF;
    op |= reg << ARM_REG_OFF;
    op |= (val & 0xffff) << ARM_IMM_OFF;
    val >>= 16;
    *(uint32_t *)p = op;
    p += sizeof(uint32_t);
  }
#else
#error "unsupported architecture"
#endif
  return p;
}

static void *foreign_thunk_sysv(void *func) {
  uint8_t *code, *p;
#ifdef __x86_64__
  // it is no longer needed
  if (1) return func;
  // movabs $func,%rax
  // movabs $foreign_tramp,%r10
  // jmp *%r10
  if (!(p = code = foreign_alloc(23))) return 0;  // 10 + 10 + 3 = 23
  p = movimm(p, 0, (uintptr_t)func);
  p = movimm(p, 10, (uintptr_t)foreign_tramp);
  *p++ = 0x41;
  *p++ = 0xff;
  *p++ = 0xe2;
#elif defined(__aarch64__)
  __jit_begin();
  if ((p = code = foreign_alloc(36))) {
    p = movimm(p, 8, (uintptr_t)func);
    p = movimm(p, 10, (uintptr_t)foreign_tramp);
    *(uint32_t *)p = 0xd61f0140;  // br x10
    __clear_cache(code, p + 4);
  }
  __jit_end();
#else
#error "unsupported architecture"
#endif
  return code;
}

static void *foreign_thunk_nt(void *func) {
  uint8_t *code;
  if (!(code = foreign_alloc(27))) return 0;
  // push %rbp
  code[0] = 0x55;
  // mov %rsp,%rbp
  code[1] = 0x48;
  code[2] = 0x89;
  code[3] = 0xe5;
  // movabs $func,%rax
  code[4] = 0x48;
  code[5] = 0xb8;
  WRITE64LE(code + 6, (uintptr_t)func);
  // movabs $tramp,%r10
  code[14] = 0x49;
  code[15] = 0xba;
  WRITE64LE(code + 16, (uintptr_t)__sysv2nt14);
  // jmp *%r10
  code[24] = 0x41;
  code[25] = 0xff;
  code[26] = 0xe2;
  return code;
}

static dontinline bool foreign_compile(char exe[hasatleast PATH_MAX]) {

  // construct path
  strlcpy(exe, get_tmp_dir(), PATH_MAX);
  strlcat(exe, "/.cosmo/", PATH_MAX);
  if (mkdir(exe, 0755) && errno != EEXIST) {
    return false;
  }
  strlcat(exe, "dlopen-helper", PATH_MAX);

  // skip build if helper exists and this program is older
  bool helper_exe_exists;
  switch (is_file_newer_than(GetProgramExecutableName(), exe)) {
    case -1:
      return false;
    case 0:
      return true;
    case 1:
      helper_exe_exists = true;
      break;
    case 2:
      helper_exe_exists = false;
      break;
    default:
      __builtin_unreachable();
  }

  // skip build if helper has same source code
  int fd;
  char src[PATH_MAX];
  char sauce[sizeof(HELPER)];
  strlcpy(src, exe, PATH_MAX);
  strlcat(src, ".c", PATH_MAX);
  if (helper_exe_exists) {
    if ((fd = open(src, O_RDONLY | O_CLOEXEC)) != -1) {
      ssize_t got = pread(fd, sauce, sizeof(HELPER), 0);
      close(fd);
      if (got == sizeof(HELPER) - 1 &&
          !memcmp(sauce, HELPER, sizeof(HELPER) - 1)) {
        return true;
      }
    }
  }

  // create source file
  char tmp[PATH_MAX];
  strlcpy(tmp, src, PATH_MAX);
  strlcat(tmp, ".XXXXXX", PATH_MAX);
  if ((fd = mkostemp(tmp, O_CLOEXEC)) == -1) {
    return false;
  }
  if (write(fd, HELPER, sizeof(HELPER) - 1) != sizeof(HELPER) - 1) {
    close(fd);
    unlink(tmp);
    return false;
  }
  if (close(fd)) {
    unlink(tmp);
    return false;
  }
  if (rename(tmp, src)) {
    unlink(tmp);
    return false;
  }

  // create executable
  strlcpy(tmp, exe, PATH_MAX);
  strlcat(tmp, ".XXXXXX", PATH_MAX);
  if ((fd = mkostemp(tmp, O_CLOEXEC)) == -1) {
    return false;
  }
  int pid, ws;
  char *args[] = {
      "cc",
      "-pie",
      "-fPIC",
      src,
      "-o",
      tmp,
      IsLinux() ? "-Wl,-z,execstack" : "-DIGNORE",
      IsNetbsd() ? 0 : "-ldl",
      0,
  };
  errno_t err = posix_spawnp(&pid, args[0], NULL, NULL, args, environ);
  if (err) {
    unlink(tmp);
    errno = err;
    return false;
  }
  while (waitpid(pid, &ws, 0) == -1) {
    if (errno != EINTR) {
      unlink(tmp);
      return false;
    }
  }
  if (ws) {
    unlink(tmp);
    return false;
  }
  if (rename(tmp, exe)) {
    unlink(tmp);
    return false;
  }
  return true;
}

static bool foreign_setup(void) {
  // geth path of helper executable
  char exe[PATH_MAX];
  if (!foreign_compile(exe)) {
    return false;
  }
  // load helper executable into address space
#ifdef __x86_64__
  struct CosmoTib *cosmo_tib = __get_tls();
#endif
  if (!setjmp(__foreign.jb)) {
    elf_exec(exe, environ);
    return false;  // if elf_exec() returns, it failed
  }
#ifdef __x86_64__
  __foreign.tib = __get_tls();
  __set_tls(cosmo_tib);
#endif
  __foreign.dlopen = foreign_thunk_sysv(__foreign.dlopen);
  __foreign.dlsym = foreign_thunk_sysv(__foreign.dlsym);
  __foreign.dlclose = foreign_thunk_sysv(__foreign.dlclose);
  __foreign.dlerror = foreign_thunk_sysv(__foreign.dlerror);
  __foreign.is_supported = true;
  return true;
}

static void foreign_once(void) {
  foreign_setup();
}

static bool foreign_init(void) {
  bool res;
  cosmo_once(&__foreign.once, foreign_once);
  if (!(res = __foreign.is_supported)) {
    dlerror_set("dlopen() isn't supported on this platform");
  }
  return res;
}

static int dlclose_nt(void *handle) {
  int res;
  if (FreeLibrary((uintptr_t)handle)) {
    res = 0;
  } else {
    dlerror_set("FreeLibrary() failed");
    res = -1;
  }
  return res;
}

static void *dlopen_nt(const char *path, int mode) {
  int n;
  uintptr_t handle;
  char16_t path16[PATH_MAX + 2];
  if (mode & ~(RTLD_LOCAL | RTLD_LAZY | RTLD_NOW)) {
    dlerror_set("invalid mode");
    return 0;
  }
  if ((n = __mkntpath(path, path16)) == -1) {
    dlerror_set("path invalid");
    return 0;
  }
  if (n > 3 &&                 //
      path16[n - 3] == '.' &&  //
      path16[n - 2] == 's' &&  //
      path16[n - 1] == 'o') {
    path16[n - 2] = 'd';
    path16[n - 1] = 'l';
    path16[n + 0] = 'l';
    path16[n + 1] = 0;
  }
  if (!(handle = LoadLibrary(path16))) {
    dlerror_set("library not found");
  }
  return (void *)handle;
}

static void *dlsym_nt(void *handle, const char *name) {
  void *x64_abi_func;
  if ((x64_abi_func = GetProcAddress((uintptr_t)handle, name))) {
    return x64_abi_func;
  } else {
    dlerror_set("symbol not found: ");
    strlcat(dlerror_buf, name, sizeof(dlerror_buf));
    return 0;
  }
}

static void *dlopen_silicon(const char *path, int mode) {
  int n;
  int xnu_mode = 0;
  char path2[PATH_MAX + 5];
  if (mode & ~(RTLD_LOCAL | RTLD_LAZY | RTLD_NOW | RTLD_GLOBAL)) {
    xnu_mode = -1;  // punt error to system dlerror() impl
  }
  if (!(mode & RTLD_GLOBAL)) {
    xnu_mode |= XNU_RTLD_LOCAL;  // unlike Linux, XNU defaults to RTLD_GLOBAL
  }
  if (mode & RTLD_NOW) {
    xnu_mode |= XNU_RTLD_NOW;
  }
  if (mode & RTLD_LAZY) {
    xnu_mode |= XNU_RTLD_LAZY;
  }
  if ((n = strlen(path)) < PATH_MAX && n > 3 &&  //
      path[n - 3] == '.' &&                      //
      path[n - 2] == 's' &&                      //
      path[n - 1] == 'o') {
    memcpy(path2, path, n);
    path2[n - 2] = 'd';
    path2[n - 1] = 'y';
    path2[n + 0] = 'l';
    path2[n + 1] = 'i';
    path2[n + 2] = 'b';
    path2[n + 3] = 0;
    path = path2;
  }
  return __syslib->__dlopen(path, xnu_mode);
}

/**
 * Opens dynamic shared object using host platform libc.
 *
 * If a `path` ending with `.so` is passed on Windows or MacOS, then
 * this wrapper will automatically change it to `.dll` or `.dylib` to
 * increase its chance of successfully loading.
 *
 * WARNING: Our API uses a different naming because cosmo_dlopen() lacks
 * many of the features one would reasonably expect from a UNIX dlopen()
 * implementation; and we don't want to lead ./configure scripts astray.
 * Foreign libraries also can't link symbols defined by your executable,
 * which means using this for high-level language plugins is completely
 * out of the question. What cosmo_dlopen() can do is help you talk to
 * GPU and GUI libraries like CUDA and SDL.
 *
 * @param mode is a bitmask that can contain:
 *     - `RTLD_LOCAL` (default)
 *     - `RTLD_GLOBAL` (not supported on Windows)
 *     - `RTLD_LAZY`
 *     - `RTLD_NOW`
 * @return dso handle, or NULL w/ dlerror()
 * @note this non-standard API is feature gated; you need to pass the
 *     `-mcosmo` flag to `cosmocc` so that `<dlfcn.h>` will define it
 */
void *cosmo_dlopen(const char *path, int mode) {
  void *res;
  BLOCK_SIGNALS;
  BLOCK_CANCELATION;
  if (IsWindows()) {
    res = dlopen_nt(path, mode);
  } else if (IsXnuSilicon()) {
    res = dlopen_silicon(path, mode);
  } else if (IsXnu()) {
    dlerror_set("dlopen() isn't supported on x86-64 MacOS");
    res = 0;
  } else if (IsOpenbsd()) {
    // TODO(jart): implement workaround for msyscall() dilemma
    dlerror_set("dlopen() isn't supported on OpenBSD yet");
    res = 0;
  } else if (foreign_init()) {
    res = __foreign.dlopen(path, mode);
  } else {
    res = 0;
  }
  ALLOW_CANCELATION;
  ALLOW_SIGNALS;
  STRACE("dlopen(%#s, %d) → %p% m", path, mode, res);
  return res;
}

/**
 * Obtains address of symbol from dynamic shared object.
 *
 * WARNING: You almost always want to say this:
 *
 *     pFunction = cosmo_dltramp(cosmo_dlsym(dso, "function"));
 *
 * That will generate code at runtime for automatically translating to
 * Microsoft's x64 calling convention when appropriate. However the
 * automated solution doesn't always work. For example, the prototype:
 *
 *     void func(int, float);
 *
 * Won't be translated correctly, due to the differences in ABI. We're
 * able to smooth over most of them, but that's just one of several
 * examples where we can't. A good rule of thumb is:
 *
 *   - More than four float/double args is problematic
 *   - Having both integral and floating point parameters is bad
 *
 * For those kinds of functions, you need to translate the ABI by hand.
 * This can be accomplished using the GCC `__ms_abi__` attribute, where
 * you'd have two function pointer types branched upon `IsWindows()`.
 *
 * @param handle was opened by dlopen()
 * @return address of symbol, or NULL w/ dlerror()
 */
void *cosmo_dlsym(void *handle, const char *name) {
  void *func;
  if (IsWindows()) {
    func = dlsym_nt(handle, name);
  } else if (IsXnuSilicon()) {
    func = __syslib->__dlsym(handle, name);
  } else if (IsXnu()) {
    dlerror_set("dlopen() isn't supported on x86-64 MacOS");
    func = 0;
  } else if (foreign_init()) {
    func = __foreign.dlsym(handle, name);
  } else {
    func = 0;
  }
  STRACE("dlsym(%p, %#s) → %p", handle, name, func);
  return func;
}

/**
 * Trampolines foreign function pointer so it can be called safely.
 */
void *cosmo_dltramp(void *foreign_func) {
  if (!IsWindows()) {
    return foreign_thunk_sysv(foreign_func);
  } else {
    return foreign_thunk_nt(foreign_func);
  }
}

/**
 * Closes dynamic shared object.
 *
 * @param handle was opened by dlopen()
 * @return 0 on success, or -1 w/ dlerror()
 */
int cosmo_dlclose(void *handle) {
  int res;
  if (IsWindows()) {
    res = dlclose_nt(handle);
  } else if (IsXnuSilicon()) {
    res = __syslib->__dlclose(handle);
  } else if (IsXnu()) {
    dlerror_set("dlopen() isn't supported on x86-64 MacOS");
    res = -1;
  } else if (foreign_init()) {
    res = __foreign.dlclose(handle);
  } else {
    res = -1;
  }
  STRACE("dlclose(%p) → %d", handle, res);
  return res;
}

/**
 * Returns string describing last dlopen/dlsym/dlclose error.
 */
char *cosmo_dlerror(void) {
  char *res;
  if (IsXnuSilicon()) {
    res = __syslib->__dlerror();
  } else if (IsWindows() || IsXnu()) {
    res = dlerror_buf;
  } else if (foreign_init()) {
    res = __foreign.dlerror();
    res = dlerror_set(res);
  } else {
    res = dlerror_buf;
  }
  STRACE("dlerror() → %#s", res);
  return res;
}

#ifdef __x86_64__
static textstartup void dlopen_init() {
  if (IsLinux() || IsFreebsd()) {
    // switch from %fs to %gs for tls
    struct CosmoTib *tib = __get_tls();
    __morph_tls();
    __set_tls(tib);
  }
}
const void *const dlopen_ctor[] initarray = {
    dlopen_init,
};
#endif
