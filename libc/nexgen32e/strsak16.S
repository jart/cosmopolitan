/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 sw=8 fenc=utf-8                                     :vi│
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
#include "libc/nexgen32e/x86feature.h"
#include "libc/macros.h"

/	Returns pointer to first instance of character.
/
/	@param	rdi is a non-null NUL-terminated char16_t string pointer
/	@param	esi is the search word
/	@return	rax points to character, or to NUL word if not found
/	@note	this won't return NULL if search character is NUL
strchrnul16:
	.leafprologue
	.profilable
	or	$-1,%r9
	jmp	0f
	.endfn	strchrnul16,globl

/	Returns pointer to first instance of character.
/
/	@param	rdi is a non-null NUL-terminated char16_t string pointer
/	@param	esi is the search word
/	@return	rax points to first result, or NULL if not found
/	@note	this won't return NULL if search character is NUL
/	@asyncsignalsafe
strchr16:
	.leafprologue
	.profilable
	xor	%r9,%r9
0:	mov	%esi,%edx
	xor	%r11d,%r11d
	or	$-1,%rsi
	xor	%r8,%r8
	jmp	strsak16
	.endfn	strchr16,globl

/	Returns pointer to first instance of character in range.
/
/	@param	rdi is a non-null pointer to memory
/	@param	esi is the search word
/	@return	rax points to word if found, or else undefined behavior
rawmemchr16:
	or	$-1,%rdx
/	fallthrough
	.endfn	rawmemchr16,globl

/	Returns pointer to first instance of character in range.
/
/	@param	rdi is a non-null pointer to memory
/	@param	esi is the search word
/	@param	rdx is length of memory in shorts
/	@return	rax points to word if found or NULL
/	@asyncsignalsafe
memchr16:
	.leafprologue
	.profilable
	xchg	%rsi,%rdx
	mov	%edx,%r11d
	xor	%r8,%r8
	xor	%r10,%r10
	jmp	strsak16
	.endfn	memchr16,globl

/	Returns length of char16_t string w/ security blankets.
/
/	This is like strnlen() except it'll return 0 if (1) RDI is NULL
/	or (2) a NUL-terminator wasn't found in the first RSI shorts.
/
/	@param	rdi is a nullable NUL-terminated char16_t string pointer
/	@param	rsi is the maximum number of shorts to consider
/	@return	rax is the number of shorts, excluding the NUL
strnlen16_s:
	.leafprologue
	.profilable
	xor	%eax,%eax
	xor	%r10d,%r10d
	test	%rdi,%rdi
	jnz	0f
	.leafepilogue
	.endfn	strnlen16_s,globl

/	Returns length of NUL-terminated char16_t string.
/
/	@param	rdi is non-null NUL-terminated char16_t string pointer
/	@return	rax is the number of shorts, excluding the NUL
/	@asyncsignalsafe
strlen16:
	or	$-1,%rsi
/	fallthrough
	.endfn	strlen16,globl

/	Returns length of NUL-terminated memory, with limit.
/
/	@param	rdi is non-null memory
/	@param	rsi is the maximum number of shorts to consider
/	@return	rax is the number of shorts, excluding the NUL
/	@asyncsignalsafe
strnlen16:
	.leafprologue
	.profilable
	or	$-1,%r10
0:	xor	%edx,%edx
	xor	%r11d,%r11d
	mov	%rdi,%r8
/	fallthrough
	.endfn	strnlen16,globl

/	Swiss Army Knife of string char16_t scanning.
/	Sixteen fast functions in one.
/
/	@param	rdi is non-null string memory
/	@param	rsi is max number of shorts to consider
/	@param	dx is search character #1
/	@param	r11w is search character #2
/	@param	r8 is subtracted from result (for length vs. pointer)
/	@param	r9 masks result if DH is found (for NUL vs. NULL)
/	@param	r10 masks result on shorts exhausted (for length v. NULL)
/	@return	rax end pointer after r8/r9/r10 modifications
strsak16:
	lea	-2(%rdi),%rax
1:	add	$2,%rax
	sub	$1,%rsi
	jb	.Lend
	test	$31,%al
	jz	.Lfast
.Lword:	mov	(%rax),%cx
	cmp	%cx,%dx
	je	.Ldone
	cmp	%cx,%r11w
	je	.Lnul
	jmp	1b
.Ldone:	sub	%r8,%rax
	jmp	.Lret
.Lend:	mov	%r10,%r9
.Lnul:	sub	%r8,%rax
	and	%r9,%rax
.Lret:	test	%r8,%r8
	jz	0f
	shr	%rax
0:	.leafepilogue
.Lslow:	add	$32,%rsi
	jmp	.Lword
.Lfast:
#if !X86_NEED(AVX2)
	testb	X86_HAVE(AVX2)+kCpuids(%rip)
	jz	.Lword
#endif
	movzwl	%dx,%ecx
	movd	%ecx,%xmm0
	movzwl	%r11w,%ecx
	movd	%ecx,%xmm1
	vpbroadcastw %xmm0,%ymm0
	vpbroadcastw %xmm1,%ymm1
	sub	$32,%rax
1:	add	$32,%rax
	sub	$16,%rsi
	jb	.Lslow
	vmovdqa	(%rax),%ymm2
	vpcmpeqw %ymm0,%ymm2,%ymm3
	vpcmpeqw %ymm1,%ymm2,%ymm2
	vpor	%ymm3,%ymm2,%ymm2
	vpmovmskb %ymm2,%ecx
	bsf	%ecx,%ecx
	je	1b
	vzeroupper
	add	%rcx,%rax
	jmp	.Lword
	.endfn	strsak16
	.source	__FILE__
