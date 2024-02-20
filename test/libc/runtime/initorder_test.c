/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
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
#include "libc/stdio/stdio.h"

// orders of events
//  -4         pc() bios
//  -4  ApeLoader() unix
//  -3    WinMain() win32
//  -3     _start() unix
//  -2      cosmo() x86
//  -2     cosmo2() aarch64
//  -1      _init() x86
//   0   ASMPREINIT ok
//   1   ASMINIT105 ok
//   2      CTOR110 ok
//   3   ASMINIT115 ok
//   4      CTOR120 ok
//   5     CTOR1000 ok
//   6    CTOR10000 ok
//   7    CTOR40000 ok
//   8    CTOR60000 ok
//   9   ASMCTOR113 ok
//  10   ASMCTOR103 ok
//  11    CTOR65500 ok
//  12    CTOR65534 ok
//  13         CTOR ok
//  14    CTOR65535 ok
//  15      ASMINIT ok
//  16      ASMCTOR ok
//   n       main()

#define N 18

#define COSMOINIT -1

#define CTOR      1
#define CTOR110   110
#define CTOR120   120
#define CTOR1000  1000
#define CTOR10000 10000
#define CTOR40000 40000
#define CTOR60000 60000
#define CTOR65500 65500
#define CTOR65534 65534
#define CTOR65535 65535

#define ASMINIT    2
#define ASMINIT105 105
#define ASMINIT115 115

#define ASMCTOR    3
#define ASMCTOR103 103
#define ASMCTOR113 113

#define ASMPREINIT    4
#define ASMPREINIT107 107
#define ASMPREINIT117 117

#ifdef __x86__
#define GUFF " #"
#else
#define GUFF " //"
#endif

int event;
int events[N];

const char *getname(int x) {
  switch (x) {
    case 0:
      return "0";
    case CTOR:
      return "CTOR";
    case CTOR110:
      return "CTOR110";
    case CTOR120:
      return "CTOR120";
    case CTOR1000:
      return "CTOR1000";
    case CTOR10000:
      return "CTOR10000";
    case CTOR40000:
      return "CTOR40000";
    case CTOR60000:
      return "CTOR60000";
    case CTOR65500:
      return "CTOR65500";
    case CTOR65534:
      return "CTOR65534";
    case CTOR65535:
      return "CTOR65535";
    case ASMINIT:
      return "ASMINIT";
    case ASMINIT105:
      return "ASMINIT105";
    case ASMINIT115:
      return "ASMINIT115";
    case ASMCTOR:
      return "ASMCTOR";
    case ASMCTOR103:
      return "ASMCTOR103";
    case ASMCTOR113:
      return "ASMCTOR113";
    case ASMPREINIT:
      return "ASMPREINIT";
    case ASMPREINIT107:
      return "ASMPREINIT107";
    case ASMPREINIT117:
      return "ASMPREINIT117";
    default:
      return "???";
  }
}

__attribute__((__constructor__)) void ctor(void) {
  events[event++] = CTOR;
}

__attribute__((__constructor__(110))) void ctor110(void) {
  events[event++] = CTOR110;
}

__attribute__((__constructor__(120))) void ctor120(void) {
  events[event++] = CTOR120;
}

__attribute__((__constructor__(1000))) void ctor1000(void) {
  events[event++] = CTOR1000;
}

__attribute__((__constructor__(10000))) void ctor10000(void) {
  events[event++] = CTOR10000;
}

__attribute__((__constructor__(40000))) void ctor40000(void) {
  events[event++] = CTOR40000;
}

__attribute__((__constructor__(60000))) void ctor60000(void) {
  events[event++] = CTOR60000;
}

__attribute__((__constructor__(65500))) void ctor65500(void) {
  events[event++] = CTOR65500;
}

__attribute__((__constructor__(65534))) void ctor65534(void) {
  events[event++] = CTOR65534;
}

__attribute__((__constructor__(65535))) void ctor65535(void) {
  events[event++] = CTOR65535;
}

void asminit(void) {
  events[event++] = ASMINIT;
}
__attribute__((__section__(
    ".init_array,\"aw\",@init_array" GUFF))) void *const kAsminit[] = {asminit};

void asminit105(void) {
  events[event++] = ASMINIT105;
}
__attribute__((__section__(
    ".init_array.105,\"aw\",@init_array" GUFF))) void *const kAsminit105[] = {
    asminit105};

void asminit115(void) {
  events[event++] = ASMINIT115;
}
__attribute__((__section__(
    ".init_array.115,\"aw\",@init_array" GUFF))) void *const kAsminit115[] = {
    asminit115};

void asmpreinit(void) {
  events[event++] = ASMPREINIT;
}
__attribute__((__section__(
    ".preinit_array,\"a\",@preinit_array" GUFF))) void *const kAsmpreinit[] = {
    asmpreinit};

void asmpreinit107(void) {
  events[event++] = ASMPREINIT107;
}
__attribute__((
    __section__(".preinit_array.107,\"a\",@preinit_array" GUFF))) void
    *const kAsmpreinit107[] = {asmpreinit107};

void asmctor(void) {
  events[event++] = ASMCTOR;
}
__attribute__((__section__(
    ".ctors,\"aw\",@init_array" GUFF))) void *const kAsmctor[] = {asmctor};

void asmctor103(void) {
  events[event++] = ASMCTOR103;
}
__attribute__((__section__(
    ".ctors.103,\"aw\",@init_array" GUFF))) void *const kAsmctor103[] = {
    asmctor103};

void asmctor113(void) {
  events[event++] = ASMCTOR113;
}
__attribute__((__section__(
    ".ctors.113,\"aw\",@init_array" GUFF))) void *const kAsmctor113[] = {
    asmctor113};

const int want[N] = {
    ASMPREINIT,  //
    ASMINIT105,  //
    CTOR110,     //
    ASMINIT115,  //
    CTOR120,     //
    CTOR1000,    //
    CTOR10000,   //
    CTOR40000,   //
    CTOR60000,   //
    ASMCTOR113,  //
    ASMCTOR103,  //
    CTOR65500,   //
    CTOR65534,   //
    CTOR,        //
    CTOR65535,   //
    ASMINIT,     //
    ASMCTOR,     //
};

int main() {
  int fails = 0;
  printf("\nevents:\n");
  for (int i = 0; i < N; ++i) {
    printf("%3d %12s ", i, getname(events[i]));
    if (events[i] == want[i]) {
      printf("ok");
    } else {
      printf("should be %s", getname(want[i]));
      ++fails;
    }
    printf("\n");
  }
  printf("\n");
  return fails;
}
