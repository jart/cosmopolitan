/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/calls.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"

struct SourceLocation {
  const char *filename;
  int line;
  int column;
};

struct AccessInfo {
  const uint8_t *addr;
  const uint8_t *first_bad_addr;
  size_t size;
  bool iswrite;
  unsigned long ip;
};

struct Global {
  const uint8_t *addr;
  size_t size;
  size_t size_with_redzone;
  const void *name;
  const void *module_name;
  unsigned long has_cxx_init;
  struct kasan_source_location *location;
  char *odr_indicator;
};

privileged void __asan_init(void) {
}

privileged void __asan_version_mismatch_check_v8(void) {
}

privileged void __asan_register_globals(struct Global globals[], int n) {
}

privileged void __asan_unregister_globals(struct Global globals[], int n) {
}

privileged void __asan_report_load_n(uint8_t *p, int n) {
}

privileged void __asan_report_store_n(uint8_t *p, int n) {
  __asan_report_load_n(p, n);
}

privileged void __asan_loadN(uintptr_t ptr, size_t size) {
}

privileged void __asan_storeN(uintptr_t ptr, size_t size) {
}

privileged uintptr_t __asan_stack_malloc(size_t size, int classid) {
  return 0;
}

privileged void __asan_stack_free(uintptr_t ptr, size_t size, int classid) {
}

privileged void __asan_handle_no_return(void) {
  DebugBreak();
}

privileged void __asan_alloca_poison(uintptr_t addr, uintptr_t size) {
}

privileged void __asan_allocas_unpoison(uintptr_t top, uintptr_t bottom) {
}

privileged void *__asan_addr_is_in_fake_stack(void *fakestack, void *addr,
                                              void **beg, void **end) {
  return NULL;
}

privileged void *__asan_get_current_fake_stack(void) {
  return NULL;
}
