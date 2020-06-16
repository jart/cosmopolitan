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

	.globl	kBiosDataArea
	.hidden	kBiosDataArea
	.type	kBiosDataArea,@object
	.size	kBiosDataArea,XLM_BIOS_DATA_AREA_SIZE
	kBiosDataArea = ape.xlm + XLM_BIOS_DATA_AREA

	.globl	kBiosDataAreaXlm
	.hidden	kBiosDataAreaXlm
	.type	kBiosDataAreaXlm,@object
	.size	kBiosDataAreaXlm,XLM_BIOS_DATA_AREA_SIZE
	kBiosDataAreaXlm = XLM(BIOS_DATA_AREA)

	.section .sort.real.init.2.kBiosDataArea,"ax",@progbits
	movpp	%ds,%es			# copy bios data to valid page
	mov	$PC_BIOS_DATA_AREA,%si
	mov	$XLM(BIOS_DATA_AREA),%di
	mov	$XLM_BIOS_DATA_AREA_SIZE,%cx
	rep movsb
	.previous
