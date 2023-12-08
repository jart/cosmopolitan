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
#ifdef __x86_64__

void __asan_report_load(void *, int);
void __asan_report_store(void *, int);
void *__asan_stack_malloc(size_t, int);
void __asan_stack_free(char *, size_t, int);

void __asan_report_load1(void *p) {
  __asan_report_load(p, 1);
}
void __asan_report_load2(void *p) {
  __asan_report_load(p, 2);
}
void __asan_report_load4(void *p) {
  __asan_report_load(p, 4);
}
void __asan_report_load8(void *p) {
  __asan_report_load(p, 8);
}
void __asan_report_load16(void *p) {
  __asan_report_load(p, 16);
}
void __asan_report_load32(void *p) {
  __asan_report_load(p, 32);
}
void __asan_report_load_n(void *p, int n) {
  __asan_report_load(p, n);
}

void __asan_report_store1(void *p) {
  __asan_report_store(p, 1);
}
void __asan_report_store2(void *p) {
  __asan_report_store(p, 2);
}
void __asan_report_store4(void *p) {
  __asan_report_store(p, 4);
}
void __asan_report_store8(void *p) {
  __asan_report_store(p, 8);
}
void __asan_report_store16(void *p) {
  __asan_report_store(p, 16);
}
void __asan_report_store32(void *p) {
  __asan_report_store(p, 32);
}
void __asan_report_store_n(void *p, int n) {
  __asan_report_store(p, n);
}

void *__asan_stack_malloc_0(size_t n) {
  return __asan_stack_malloc(n, 0);
}
void *__asan_stack_malloc_1(size_t n) {
  return __asan_stack_malloc(n, 1);
}
void *__asan_stack_malloc_2(size_t n) {
  return __asan_stack_malloc(n, 2);
}
void *__asan_stack_malloc_3(size_t n) {
  return __asan_stack_malloc(n, 3);
}
void *__asan_stack_malloc_4(size_t n) {
  return __asan_stack_malloc(n, 4);
}
void *__asan_stack_malloc_5(size_t n) {
  return __asan_stack_malloc(n, 5);
}
void *__asan_stack_malloc_6(size_t n) {
  return __asan_stack_malloc(n, 6);
}
void *__asan_stack_malloc_7(size_t n) {
  return __asan_stack_malloc(n, 7);
}
void *__asan_stack_malloc_8(size_t n) {
  return __asan_stack_malloc(n, 8);
}
void *__asan_stack_malloc_9(size_t n) {
  return __asan_stack_malloc(n, 9);
}
void *__asan_stack_malloc_10(size_t n) {
  return __asan_stack_malloc(n, 10);
}

void __asan_stack_free_0(char *p, size_t n) {
  __asan_stack_free(p, n, 0);
}
void __asan_stack_free_1(char *p, size_t n) {
  __asan_stack_free(p, n, 1);
}
void __asan_stack_free_2(char *p, size_t n) {
  __asan_stack_free(p, n, 2);
}
void __asan_stack_free_3(char *p, size_t n) {
  __asan_stack_free(p, n, 3);
}
void __asan_stack_free_4(char *p, size_t n) {
  __asan_stack_free(p, n, 4);
}
void __asan_stack_free_5(char *p, size_t n) {
  __asan_stack_free(p, n, 5);
}
void __asan_stack_free_6(char *p, size_t n) {
  __asan_stack_free(p, n, 6);
}
void __asan_stack_free_7(char *p, size_t n) {
  __asan_stack_free(p, n, 7);
}
void __asan_stack_free_8(char *p, size_t n) {
  __asan_stack_free(p, n, 8);
}
void __asan_stack_free_9(char *p, size_t n) {
  __asan_stack_free(p, n, 9);
}
void __asan_stack_free_10(char *p, size_t n) {
  __asan_stack_free(p, n, 10);
}

void __asan_load1() {
  __builtin_trap();
}
void __asan_load2() {
  __builtin_trap();
}
void __asan_load4() {
  __builtin_trap();
}
void __asan_load8() {
  __builtin_trap();
}
void __asan_load16() {
  __builtin_trap();
}
void __asan_load32() {
  __builtin_trap();
}

void __asan_store1() {
  __builtin_trap();
}
void __asan_store2() {
  __builtin_trap();
}
void __asan_store4() {
  __builtin_trap();
}
void __asan_store8() {
  __builtin_trap();
}
void __asan_store16() {
  __builtin_trap();
}
void __asan_store32() {
  __builtin_trap();
}

#endif /* __x86_64__ */
