/*-*- mode:asm; indent-tabs-mode:t; tab-width:8; coding:utf-8               -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
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
#include "libc/macros.h"
.yoink	__FILE__

__asan_addr_is_in_fake_stack:
__asan_alloca_poison:
__asan_allocas_unpoison:
__asan_get_current_fake_stack:
__asan_handle_no_return:
__asan_init:
__asan_loadN:
__asan_register_globals:
__asan_report_load_n:
__asan_report_store_n:
__asan_stack_free:
__asan_stack_malloc:
__asan_storeN:
__asan_unregister_globals:
__asan_version_mismatch_check_v8:
	xor	%eax,%eax
	ret
	.endfn	__asan_addr_is_in_fake_stack,globl,weak
	.endfn	__asan_alloca_poison,globl,weak
	.endfn	__asan_allocas_unpoison,globl,weak
	.endfn	__asan_get_current_fake_stack,globl,weak
	.endfn	__asan_handle_no_return,globl,weak
	.endfn	__asan_init,globl,weak
	.endfn	__asan_loadN,globl,weak
	.endfn	__asan_register_globals,globl,weak
	.endfn	__asan_report_load_n,globl,weak
	.endfn	__asan_report_store_n,globl,weak
	.endfn	__asan_stack_free,globl,weak
	.endfn	__asan_stack_malloc,globl,weak
	.endfn	__asan_storeN,globl,weak
	.endfn	__asan_unregister_globals,globl,weak
	.endfn	__asan_version_mismatch_check_v8,globl,weak
