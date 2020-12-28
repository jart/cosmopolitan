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
/	@param	rdi is a non-null NUL-terminated wchar_t string pointer
/	@param	esi is the search word
/	@return	rax points to character, or to NUL word if not found
/	@note	this won't return NULL if search character is NUL
wcschrnul:
	.leafprologue
	.profilable
	or	$-1,%r9
	jmp	0f

/	Returns pointer to first instance of character.
/
/	@param	rdi is a non-null NUL-terminated wchar_t string pointer
/	@param	esi is the search word
/	@return	rax points to first result, or NULL if not found
/	@note	this won't return NULL if search character is NUL
/	@asyncsignalsafe
wcschr:	.leafprologue
	.profilable
	xor	%r9,%r9
0:	mov	%esi,%edx
	xor	%r11d,%r11d
	pushpop	-1,%rsi
	xor	%r8,%r8
	jmp	wcssak

/	Returns length of wchar_t string w/ security blankets.
/
/	This is like wcsnlen() except it'll return 0 if (1) RDI is NULL
/	or (2) a NUL-terminator wasn't found in the first RSI chars.
/
/	@param	rdi is a nullable NUL-terminated wchar_t string pointer
/	@param	rsi is the maximum number of chars to consider
/	@return	rax is the number of chars, excluding the NUL
wcsnlen_s:
	.leafprologue
	.profilable
	xor	%eax,%eax
	xor	%r10d,%r10d
	test	%rdi,%rdi
	jnz	0f
	.leafepilogue
	.endfn	wcsnlen_s,globl

/	Returns length of NUL-terminated wchar_t string.
/
/	@param	rdi is non-null NUL-terminated wchar_t string pointer
/	@return	rax is the number of chars, excluding the NUL
/	@asyncsignalsafe
wcslen:	or	$-1,%rsi
/	fallthrough

/	Returns length of NUL-terminated memory, with limit.
/
/	@param	rdi is non-null memory
/	@param	rsi is the maximum number of chars to consider
/	@return	rax is the number of chars, excluding the NUL
/	@asyncsignalsafe
wcsnlen:.leafprologue
	.profilable
	or	$-1,%r10
0:	xor	%edx,%edx
	xor	%r11d,%r11d
	mov	%rdi,%r8
	jmp	wcssak

/	Returns pointer to first instance of character in range.
/
/	@param	rdi is a non-null pointer to memory
/	@param	esi is the search word
/	@return	rax points to word if found, or else undefined behavior
rawwmemchr:
	or	$-1,%rdx
/	fallthrough

/	Returns pointer to first instance of character in range.
/
/	@param	rdi is a non-null pointer to memory
/	@param	esi is the int32_t search word (officially wchar_t)
/	@param	rdx is length of memory in chars
/	@return	rax points to word if found or NULL
/	@asyncsignalsafe
wmemchr:.leafprologue
	.profilable
	xor	%r8,%r8
	xor	%r10,%r10
	mov	%rdx,%rcx
	mov	%esi,%edx
	mov	%esi,%r11d
	mov	%rcx,%rsi
/	fallthrough

/	Swiss army knife of wchar_t string scanning.
/	Seven fast functions in one.
/
/	@param	rdi is non-null wchar_t string memory
/	@param	rsi is max number of chars to consider
/	@param	edx is search character #1
/	@param	r11d is search character #2
/	@param	r8 is subtracted from result (for length vs. pointer)
/	@param	r9 masks result if r11w is found (for NUL vs. NULL)
/	@param	r10 masks result on chars exhausted (for length v. NULL)
/	@return	rax end pointer after r8/r9/r10 modifications
wcssak:	lea	-4(%rdi),%rax
	.align	16
1:	add	$4,%rax
	sub	$1,%rsi
	jb	.Lend
	test	$31,%al
	jz	.Lfast
.Lint:	mov	(%rax),%ecx
	cmp	%ecx,%edx
	je	.Ldone
	cmp	%ecx,%r11d
	je	.Lnul
	jmp	1b
.Ldone:	sub	%r8,%rax
	jmp	.Lret
.Lend:	mov	%r10,%r9
.Lnul:	sub	%r8,%rax
	and	%r9,%rax
.Lret:	test	%r8,%r8
	jz	0f
	shr	$2,%rax
0:	.leafepilogue
.Lslow:	add	$8,%rsi
	jmp	.Lint
.Lfast:
#if !X86_NEED(AVX2)
	testb	X86_HAVE(AVX2)+kCpuids(%rip)
	jz	.Lint
#endif
	movd	%edx,%xmm0
	movd	%r11d,%xmm1
	vpbroadcastd %xmm0,%ymm0
	vpbroadcastd %xmm1,%ymm1
	sub	$32,%rax
1:	add	$32,%rax
	sub	$8,%rsi
	jb	.Lslow
	vmovdqa	(%rax),%ymm2
	vpcmpeqd %ymm0,%ymm2,%ymm3
	vpcmpeqd %ymm1,%ymm2,%ymm2
	vpor	%ymm3,%ymm2,%ymm2
	vpmovmskb %ymm2,%ecx
	bsf	%ecx,%ecx
	je	1b
	vzeroupper
	add	%rcx,%rax
	jmp	.Lint
	.endfn	wcssak
	.endfn	wmemchr,globl
	.endfn	rawwmemchr,globl
	.endfn	wcsnlen,globl
	.endfn	wcslen,globl
	.endfn	wcschr,globl
	.endfn	wcschrnul,globl
	.source	__FILE__
