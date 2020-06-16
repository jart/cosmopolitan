/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
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
#include "ape/lib/pc.h"
#include "ape/config.h"
#include "ape/macros.h"
#include "ape/notice.inc"
.section .real,"ax",@progbits
.source	__FILE__
.code16

	.globl	g_ptsp
	.hidden	g_ptsp
	.type	g_ptsp,@object
	.size	g_ptsp,XLM_PAGE_TABLE_STACK_POINTER_SIZE
	g_ptsp = ape.xlm + XLM_PAGE_TABLE_STACK_POINTER

	.globl	g_ptsp_xlm
	.hidden	g_ptsp_xlm
	.type	g_ptsp_xlm,@object
	.size	g_ptsp_xlm,XLM_PAGE_TABLE_STACK_POINTER_SIZE
	g_ptsp_xlm = XLM(PAGE_TABLE_STACK_POINTER)
