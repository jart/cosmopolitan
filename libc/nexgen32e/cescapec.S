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
#include "ape/relocations.h"
#include "libc/macros.h"

/	Escapes byte for string literal.
/
/	This turns stuff like (char)0xFF into \0377. The returned
/	string is word-encoded, e.g. '\\'|'0'<<010|'3'<<020|etc.
/
/	@param	dil contains byte to escape
/	@see	libc/nexgen32e/cescapec.c
cescapec:
	movzbl	%dil,%edi
	lea	-7(%rdi),%ecx
	cmp	$85,%cl
	ja	1f
	mov	$'\\,%eax
	movzbl	%cl,%ecx
	jmp	*cescapectab(,%rcx,8)
.Lanchorpoint:
.LBEL:	mov	$'a',%ah
	ret
.LBS:	mov	$'b',%ah
	ret
.LHT:	mov	$'t',%ah
	ret
.LLF:	mov	$'n',%ah
	ret
.LVT:	mov	$'v',%ah
	ret
.LFF:	mov	$'f',%ah
	ret
.LCR:	mov	$'r',%ah
	ret
.LDQ:	mov	$'\"',%ah
	ret
.LSQ:	mov	$'\'',%ah
	ret
.LBSL:	mov	$'\\',%ah
	ret
#ifdef __STRICT_ANSI__
.LQM:	mov	$'?',%ah
	ret
#else
.LQM:
#endif
1:	mov	%edi,%eax
	lea	-0x20(%rax),%ecx
	cmp	$0x5E,%ecx
	ja	2f
	ret
2:	and	$-64,%eax
	mov	%edi,%ecx
	and	$56,%ecx
	shl	$13,%ecx
	and	$7,%edi
	shl	$24,%edi
	or	%ecx,%edi
	lea	(%rdi,%rax,4),%eax
	add	$'0<<030|'0<<020|'0<<010|'\\,%eax
	ret
	.endfn	cescapec,globl

	.initro 300,_init_cescapec
cescapectab.ro:
	.byte	1,.LBEL-.Lanchorpoint
	.byte	1,.LBS-.Lanchorpoint
	.byte	1,.LHT-.Lanchorpoint
	.byte	1,.LLF-.Lanchorpoint
	.byte	1,.LVT-.Lanchorpoint
	.byte	1,.LFF-.Lanchorpoint
	.byte	1,.LCR-.Lanchorpoint
	.byte	'\"-'\r-1,1b-.Lanchorpoint
	.byte	1,.LDQ-.Lanchorpoint
	.byte	'\'-'\"-1,1b-.Lanchorpoint
	.byte	1,.LSQ-.Lanchorpoint
	.byte	'?-'\'-1,1b-.Lanchorpoint
	.byte	1,.LQM-.Lanchorpoint
	.byte	'\\-'?-1,1b-.Lanchorpoint
	.byte	1,.LBSL-.Lanchorpoint
	.equ	.Lcescapectab.ro.size,.-cescapectab.ro
	.org	8 - .Lcescapectab.ro.size % 8 + .
	.endobj	cescapectab.ro,globl,hidden
	.previous

	.initbss 300,_init_cescapec
cescapectab:
	.rept	'\\-7+1
	.quad	0
	.endr
	.endobj	cescapectab
	.previous

	.init.start 300,_init_cescapec
	ezlea	.Lanchorpoint,dx
	mov	$.Lcescapectab.ro.size/2,%ch
0:	xor	%eax,%eax
	lodsb
	mov	%al,%cl
	lodsb
	add	%rdx,%rax
1:	stosq
	dec	%cl
	jnz	1b
	dec	%ch
	jnz	0b
	.if	.Lcescapectab.ro.size % 8
	add	$(8-.Lcescapectab.ro.size%8),%rsi
	.endif
	.init.end 300,_init_cescapec
	.source	__FILE__
