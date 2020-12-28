/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/fmt/conv.h"
#include "libc/macho.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "tool/decode/lib/asmcodegen.h"
#include "tool/decode/lib/flagger.h"
#include "tool/decode/lib/machoidnames.h"
#include "tool/decode/lib/titlegen.h"

/**
 * @fileoverview Apple Mach-O metadata disassembler.
 */

static const char *path;
static struct MachoHeader *macho;
static size_t machosize;

static void startfile(void) {
  showtitle("αcτµαlly pδrταblε εxεcµταblε", "tool/decode/macho", NULL, NULL,
            &kModelineAsm);
  printf("#include \"libc/macho.internal.h\"\n\n", path);
}

static void showmachoheader(void) {
#if !defined(TRUSTWORTHY) && !defined(MACHO_TRUSTWORTHY)
  if (sizeof(struct MachoHeader) > machosize) {
    fprintf(stderr, "error: %'s: MachoHeader overruns eof\n", path);
    exit(1);
  }
#endif
  showtitle(basename(path), "macho", "header", NULL, NULL);
  printf("\n");
  showinthex(macho->magic);
  showinthex(macho->arch);
  showinthex(macho->arch2);
  show(".long",
       firstnonnull(findnamebyid(kMachoFileTypeNames, macho->filetype),
                    format(b1, "%#x", macho->filetype)),
       "macho->filetype");
  showinthex(macho->loadcount);
  showinthex(macho->loadsize);
  show(".long",
       firstnonnull(RecreateFlags(kMachoFlagNames, macho->flags),
                    format(b1, "%#x", macho->flags)),
       "macho->flags");
  showinthex(macho->__reserved);
  printf("\n");
}

static void showmachosection(struct MachoSection *section) {
  show(".ascin", format(b1, "%`'s,16", section->name), "section->name");
  show(".ascin", format(b1, "%`'s,16", section->commandname),
       "section->commandname");
  showint64hex(section->vaddr);
  showint64hex(section->memsz);
  showinthex(section->offset);
  showinthex(section->alignlog2);
  showinthex(section->relotaboff);
  showinthex(section->relocount);
  showinthex(section->attr);
  show(".long",
       format(b1, "%d,%d,%d", section->__reserved[0], section->__reserved[1],
              section->__reserved[2]),
       "section->__reserved");
}

static void showmacholoadsegment(unsigned i, struct MachoLoadSegment *loadseg) {
  assert(loadseg->size ==
         sizeof(struct MachoLoadSegment) +
             loadseg->sectioncount * sizeof(struct MachoSection));
#if !defined(TRUSTWORTHY) && !defined(MACHO_TRUSTWORTHY)
  if ((intptr_t)loadseg + sizeof(struct MachoLoadSegment) +
          loadseg->sectioncount * sizeof(struct MachoSection) >
      (intptr_t)macho + machosize) {
    abort();
  }
#endif
  show(".ascin", format(b1, "%`'s,16", loadseg->name), "loadseg->name");
  showint64hex(loadseg->vaddr);
  showint64hex(loadseg->memsz);
  showint64hex(loadseg->offset);
  showint64hex(loadseg->filesz);
  show(".long",
       firstnonnull(RecreateFlags(kMachoVmProtNames, loadseg->maxprot),
                    format(b1, "%#x", loadseg->maxprot)),
       "loadseg->maxprot");
  show(".long",
       firstnonnull(RecreateFlags(kMachoVmProtNames, loadseg->initprot),
                    format(b1, "%#x", loadseg->initprot)),
       "loadseg->initprot");
  showinthex(loadseg->sectioncount);
  show(".long",
       firstnonnull(RecreateFlags(kMachoSegmentFlagNames, loadseg->flags),
                    format(b1, "%#x", loadseg->flags)),
       "loadseg->flags");
  for (unsigned j = 0; j < loadseg->sectioncount; ++j) {
    printf("%d:", (i + 1) * 100 + (j + 1) * 10);
    showmachosection((struct MachoSection *)((intptr_t)loadseg +
                                             sizeof(struct MachoLoadSegment) +
                                             j * sizeof(struct MachoSection)));
  }
}

static void showmacholoadsymtabshowall(struct MachoLoadSymtab *ls) {
  assert(ls->size == sizeof(struct MachoLoadSymtab));
#if !defined(TRUSTWORTHY) && !defined(MACHO_TRUSTWORTHY)
  if ((intptr_t)ls + sizeof(struct MachoLoadSymtab) >
      (intptr_t)macho + machosize) {
    abort();
  }
#endif
  showinthex(ls->offset);
  showinthex(ls->count);
  showinthex(ls->stroff);
  showinthex(ls->strsize);
}

static void showmacholoaduuid(struct MachoLoadUuid *lu) {
  assert(lu->size == sizeof(struct MachoLoadUuid));
#if !defined(TRUSTWORTHY) && !defined(MACHO_TRUSTWORTHY)
  if ((intptr_t)lu + sizeof(struct MachoLoadUuid) >
      (intptr_t)macho + machosize) {
    abort();
  }
#endif
  printf("\t.byte\t");
  for (unsigned i = 0; i < 16; ++i) {
    if (i) {
      if (i == 8) {
        printf("\n\t.byte\t");
      } else {
        printf(",");
      }
    }
    printf("%#hhx", lu->uuid[i]);
  }
  printf("\n");
}

static void showmacholoadsourceversion(
    struct MachoLoadSourceVersionCommand *sv) {
  assert(sv->size == sizeof(struct MachoLoadSourceVersionCommand));
#if !defined(TRUSTWORTHY) && !defined(MACHO_TRUSTWORTHY)
  if ((intptr_t)sv + sizeof(struct MachoLoadSourceVersionCommand) >
      (intptr_t)macho + machosize) {
    abort();
  }
#endif
  showint64hex(sv->version);
}

static void showmacholoadunixthread(struct MachoLoadThreadCommand *lc) {
  assert(lc->size == 4 + 4 + 4 + 4 + lc->count * 4);
  showinthex(lc->flavor);
  showint(lc->count);
  for (unsigned i = 0; i < lc->count; ++i) {
    showinthex(lc->wut[i]);
  }
}

static void showmacholoadcommand(struct MachoLoadCommand *lc, unsigned i) {
#if !defined(TRUSTWORTHY) && !defined(MACHO_TRUSTWORTHY)
  if ((intptr_t)lc > (intptr_t)macho + machosize ||
      (intptr_t)lc + lc->size > (intptr_t)macho + machosize) {
    abort();
  }
#endif
  showorg((intptr_t)lc - (intptr_t)macho);
  printf("%d:", (i + 1) * 10);
  show(".long",
       firstnonnull(findnamebyid(kMachoLoadCommandNames, lc->command),
                    format(b1, "%#x", lc->command)),
       "lc->command");
  showinthex(lc->size);
  switch (lc->command) {
    case MAC_LC_SEGMENT_64:
      showmacholoadsegment(i, (struct MachoLoadSegment *)lc);
      break;
    case MAC_LC_SYMTAB:
      showmacholoadsymtabshowall((struct MachoLoadSymtab *)lc);
      break;
    case MAC_LC_UUID:
      showmacholoaduuid((struct MachoLoadUuid *)lc);
      break;
    case MAC_LC_SOURCE_VERSION:
      showmacholoadsourceversion((struct MachoLoadSourceVersionCommand *)lc);
      break;
    case MAC_LC_UNIXTHREAD:
      showmacholoadunixthread((struct MachoLoadThreadCommand *)lc);
      break;
    default:
      break;
  }
  printf("\n");
}

static void showmacholoadcommands(void) {
#if !defined(TRUSTWORTHY) && !defined(MACHO_TRUSTWORTHY)
  if (sizeof(struct MachoHeader) + macho->loadsize > machosize) {
    fprintf(stderr, "error: %'s: macho->loadsize overruns eof\n", path);
    exit(1);
  }
#endif
  unsigned i = 0;
  const unsigned count = macho->loadcount;
  for (struct MachoLoadCommand *lc =
           (void *)((intptr_t)macho + sizeof(struct MachoHeader));
       i < count; ++i, lc = (void *)((intptr_t)lc + lc->size)) {
    showmacholoadcommand(lc, i);
  }
}

void showall(void) {
  startfile();
  showmachoheader();
  showmacholoadcommands();
}

int main(int argc, char *argv[]) {
  int64_t fd;
  struct stat st[1];
  if (argc != 2) fprintf(stderr, "usage: %s FILE\n", argv[0]), exit(1);
  if ((fd = open((path = argv[1]), O_RDONLY)) == -1 || fstat(fd, st) == -1 ||
      (macho = mmap(NULL, (machosize = st->st_size), PROT_READ, MAP_SHARED, fd,
                    0)) == MAP_FAILED) {
    fprintf(stderr, "error: %'s %m\n", path);
    exit(1);
  }
  if (macho->magic != 0xFEEDFACF) {
    fprintf(stderr, "error: %'s not a macho x64 executable\n", path);
    exit(1);
  }
  showall();
  munmap(macho, machosize);
  close(fd);
  return 0;
}
