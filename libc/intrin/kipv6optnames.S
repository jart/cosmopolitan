/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/macros.h"

	.macro	.e e s
	.long	\e - kIpv6Optnames
	.long	.L\@ - kIpv6Optnames
	.rodata.str1.1
.L\@:	.string	"\s"
	.previous
	.endm

	.section .rodata
	.balign	4
	.underrun
kIpv6Optnames:
	.e	IPV6_V6ONLY,"V6ONLY"
	.e	IPV6_CHECKSUM,"CHECKSUM"
	.e	IPV6_JOIN_GROUP,"JOIN_GROUP"
	.e	IPV6_LEAVE_GROUP,"LEAVE_GROUP"
	.e	IPV6_MULTICAST_HOPS,"MULTICAST_HOPS"
	.e	IPV6_MULTICAST_IF,"MULTICAST_IF"
	.e	IPV6_MULTICAST_LOOP,"MULTICAST_LOOP"
	.e	IPV6_UNICAST_HOPS,"UNICAST_HOPS"
	.e	IPV6_RECVTCLASS,"RECVTCLASS"
	.e	IPV6_TCLASS,"TCLASS"
	.e	IPV6_DONTFRAG,"DONTFRAG"
	.e	IPV6_HOPLIMIT,"HOPLIMIT"
	.e	IPV6_HOPOPTS,"HOPOPTS"
	.e	IPV6_PKTINFO,"PKTINFO"
	.e	IPV6_RECVRTHDR,"RECVRTHDR"
	.e	IPV6_RTHDR,"RTHDR"
	.long	MAGNUM_TERMINATOR
	.endobj	kIpv6Optnames,globl,hidden
	.overrun
