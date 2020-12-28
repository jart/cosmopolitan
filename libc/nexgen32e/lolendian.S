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
