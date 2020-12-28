/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
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
#include "libc/macros.h"
.source	__FILE__

/	@fileoverview Address Sanitizer Linker Poison

__asan_addr_is_in_fake_stack:
	ud2
	.endfn	__asan_addr_is_in_fake_stack,weak

__asan_after_dynamic_init:
	ud2
	.endfn	__asan_after_dynamic_init,weak

__asan_alloca_poison:
	ud2
	.endfn	__asan_alloca_poison,weak

__asan_allocas_unpoison:
	ud2
	.endfn	__asan_allocas_unpoison,weak

__asan_before_dynamic_init:
	ud2
	.endfn	__asan_before_dynamic_init,weak

__asan_get_current_fake_stack:
	ud2
	.endfn	__asan_get_current_fake_stack,weak

__asan_handle_no_return:
	ud2
	.endfn	__asan_handle_no_return,weak

__asan_init:
	ud2
	.endfn	__asan_init,weak

__asan_load1:
	ud2
	.endfn	__asan_load1,weak

__asan_load2:
	ud2
	.endfn	__asan_load2,weak

__asan_load4:
	ud2
	.endfn	__asan_load4,weak

__asan_load8:
	ud2
	.endfn	__asan_load8,weak

__asan_load16:
	ud2
	.endfn	__asan_load16,weak

__asan_load32:
	ud2
	.endfn	__asan_load32,weak

__asan_noreentry:
	ud2
	.endfn	__asan_noreentry,weak

__asan_option_detect_stack_use_after_return:
	ud2
	.endfn	__asan_option_detect_stack_use_after_return,weak

__asan_poison_stack_memory:
	ud2
	.endfn	__asan_poison_stack_memory,weak

__asan_register_globals:
	ud2
	.endfn	__asan_register_globals,weak

__asan_report_load1:
	ud2
	.endfn	__asan_report_load1,weak

__asan_report_load2:
	ud2
	.endfn	__asan_report_load2,weak

__asan_report_load4:
	ud2
	.endfn	__asan_report_load4,weak

__asan_report_load8:
	ud2
	.endfn	__asan_report_load8,weak

__asan_report_load16:
	ud2
	.endfn	__asan_report_load16,weak

__asan_report_load_n:
	ud2
	.endfn	__asan_report_load_n,weak

__asan_report_store1:
	ud2
	.endfn	__asan_report_store1,weak

__asan_report_store2:
	ud2
	.endfn	__asan_report_store2,weak

__asan_report_store4:
	ud2
	.endfn	__asan_report_store4,weak

__asan_report_store8:
	ud2
	.endfn	__asan_report_store8,weak

__asan_report_store16:
	ud2
	.endfn	__asan_report_store16,weak

__asan_report_store32:
	ud2
	.endfn	__asan_report_store32,weak

__asan_report_store_n:
	ud2
	.endfn	__asan_report_store_n,weak

__asan_stack_free:
	ud2
	.endfn	__asan_stack_free,weak

__asan_stack_free_0:
	ud2
	.endfn	__asan_stack_free_0,weak

__asan_stack_free_1:
	ud2
	.endfn	__asan_stack_free_1,weak

__asan_stack_free_10:
	ud2
	.endfn	__asan_stack_free_10,weak

__asan_stack_free_2:
	ud2
	.endfn	__asan_stack_free_2,weak

__asan_stack_free_3:
	ud2
	.endfn	__asan_stack_free_3,weak

__asan_stack_free_4:
	ud2
	.endfn	__asan_stack_free_4,weak

__asan_stack_free_5:
	ud2
	.endfn	__asan_stack_free_5,weak

__asan_stack_free_6:
	ud2
	.endfn	__asan_stack_free_6,weak

__asan_stack_free_7:
	ud2
	.endfn	__asan_stack_free_7,weak

__asan_stack_free_8:
	ud2
	.endfn	__asan_stack_free_8,weak

__asan_stack_free_9:
	ud2
	.endfn	__asan_stack_free_9,weak

__asan_stack_malloc:
	ud2
	.endfn	__asan_stack_malloc,weak

__asan_stack_malloc_0:
	ud2
	.endfn	__asan_stack_malloc_0,weak

__asan_stack_malloc_1:
	ud2
	.endfn	__asan_stack_malloc_1,weak

__asan_stack_malloc_2:
	ud2
	.endfn	__asan_stack_malloc_2,weak

__asan_stack_malloc_3:
	ud2
	.endfn	__asan_stack_malloc_3,weak

__asan_stack_malloc_4:
	ud2
	.endfn	__asan_stack_malloc_4,weak

__asan_stack_malloc_5:
	ud2
	.endfn	__asan_stack_malloc_5,weak

__asan_stack_malloc_6:
	ud2
	.endfn	__asan_stack_malloc_6,weak

__asan_stack_malloc_7:
	ud2
	.endfn	__asan_stack_malloc_7,weak

__asan_stack_malloc_8:
	ud2
	.endfn	__asan_stack_malloc_8,weak

__asan_stack_malloc_9:
	ud2
	.endfn	__asan_stack_malloc_9,weak

__asan_stack_malloc_10:
	ud2
	.endfn	__asan_stack_malloc_10,weak

__asan_store1:
	ud2
	.endfn	__asan_store1,weak

__asan_store2:
	ud2
	.endfn	__asan_store2,weak

__asan_store4:
	ud2
	.endfn	__asan_store4,weak

__asan_store8:
	ud2
	.endfn	__asan_store8,weak

__asan_store16:
	ud2
	.endfn	__asan_store16,weak

__asan_store32:
	ud2
	.endfn	__asan_store32,weak

__asan_unpoison_stack_memory:
	ud2
	.endfn	__asan_unpoison_stack_memory,weak

__asan_unregister_globals:
	ud2
	.endfn	__asan_unregister_globals,weak

__asan_version_mismatch_check_v8:
	ud2
	.endfn	__asan_version_mismatch_check_v8,weak
