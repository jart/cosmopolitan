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

/	@fileoverview Byte-order conversion functions.
/
/	Endianness is deceptively complicated to the uninitiated. Many
/	helpers have been written by our top minds to address perceived
/	difficulties. These ones got through standardization processes.
/	To protect their legacy, all 19 functions have been implemented
/	in just 17 bytes.
/
/	@asyncsignalsafe
/	@see	read32le(), read32be(), etc.

bswap_64:
htobe64:
htole64:
be64toh:
le64toh:mov	%rdi,%rax
	bswap	%rax
	ret
	.endfn	le64toh,globl
	.endfn	be64toh,globl
	.endfn	htole64,globl
	.endfn	htobe64,globl
	.endfn	bswap_64,globl

bswap_32:
htobe32:
htole32:
be32toh:
le32toh:
ntohl:	
htonl:	mov	%edi,%eax
	bswap	%eax
	ret
	.endfn	htonl,globl
	.endfn	htole32,globl
	.endfn	le32toh,globl
	.endfn	be32toh,globl
	.endfn	htobe32,globl
	.endfn	ntohl,globl
	.endfn	bswap_32,globl

bswap_16:
htobe16:
htole16:
be16toh:
le16toh:
ntohs:	
htons:	movzwl	%di,%eax
	xchg	%al,%ah
	ret
	.endfn	htobe16,globl
	.endfn	htons,globl
	.endfn	le16toh,globl
	.endfn	be16toh,globl
	.endfn	htole16,globl
	.endfn	ntohs,globl
	.endfn	bswap_16,globl
	.source	__FILE__
