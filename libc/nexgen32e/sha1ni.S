/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│                                                                              │
│  Copyright 2015 Intel Corporation                                            │
│                                                                              │
│  Redistribution and use in source and binary forms, with or without          │
│  modification, are permitted provided that the following conditions          │
│  are met:                                                                    │
│                                                                              │
│    * Redistributions of source code must retain the above copyright          │
│      notice, this list of conditions and the following disclaimer.           │
│    * Redistributions in binary form must reproduce the above copyright       │
│      notice, this list of conditions and the following disclaimer in         │
│      the documentation and/or other materials provided with the              │
│      distribution.                                                           │
│    * Neither the name of Intel Corporation nor the names of its              │
│      contributors may be used to endorse or promote products derived         │
│      from this software without specific prior written permission.           │
│                                                                              │
│  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS         │
│  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT           │
│  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR       │
│  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT        │
│  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,       │
│  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT            │
│  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,       │
│  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY       │
│  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT         │
│  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE       │
│  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.        │
│                                                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/macros.internal.h"

.text
.balign 32
.ident "\n\
Intel SHA-NI (BSD-3 License)\n\
Copyright 2015 Intel Corporation\n\
Sean Gulley <sean.m.gulley@intel.com>\n\
Tim Chen <tim.c.chen@linux.intel.com>\n"
.include "libc/disclaimer.inc"

#define FRAME_SIZE	32
#define DIGEST_PTR	%rdi
#define DATA_PTR	%rsi
#define NUM_BLKS	%rdx
#define ABCD		%xmm0
#define E0		%xmm1	/* Need two E's b/c they ping pong */
#define E1		%xmm2
#define MSG0		%xmm3
#define MSG1		%xmm4
#define MSG2		%xmm5
#define MSG3		%xmm6
#define SHUF_MASK	%xmm7

//	Performs Intel® SHA-NI™ optimized SHA-1 update.
//
//	The function takes a pointer to the current hash values, a
//	pointer to the input data, and a number of 64 byte blocks to
//	process. Once all blocks have been processed, the digest pointer
//	is updated with the resulting hash value. The function only
//	processes complete blocks, there is no functionality to store
//	partial blocks. All message padding and hash value
//	initialization must be done outside the update function.
//
//	The indented lines in the loop are instructions related to
//	rounds processing. The non-indented lines are instructions
//	related to the message schedule.
//
//	    void sha1_transform_ni(uint32_t digest[static 5],
//	                           const void *data,
//	                           uint32_t numBlocks);
//
//	@param	%rdi points to output digest
//	@param	%rsi points to input data
//	@param	%rdx is number of 64-byte blocks to process
//	@see	X86_HAVE(SHA)
	.ftrace1
sha1_transform_ni:
	.ftrace2
	push	%rbp
	mov	%rsp,%rbp
	sub	$FRAME_SIZE,%rsp
	shl	$6,NUM_BLKS		# convert to bytes
	jz	.Ldone_hash
	add	DATA_PTR,NUM_BLKS	# pointer to end of data

//	load initial hash values
	movdqa	UPPER_WORD_MASK(%rip),E1
	pinsrd	$3,1*16(DIGEST_PTR),E0
	movdqu	0*16(DIGEST_PTR),ABCD
	pand	E1,E0
	pshufd	$0x1B,ABCD,ABCD

	movdqa	PSHUFFLE_BYTE_FLIP_MASK(%rip),SHUF_MASK

.Lloop0:
//	Save hash values for addition after rounds
	movdqa		E0,(0*16)(%rsp)
	movdqa		ABCD,(1*16)(%rsp)

//	Rounds 0-3
	movdqu		0*16(DATA_PTR),MSG0
	pshufb		SHUF_MASK,MSG0
	paddd		MSG0,E0
	movdqa		ABCD,E1
	sha1rnds4	$0,E0,ABCD

//	Rounds 4-7
	movdqu		1*16(DATA_PTR),MSG1
	pshufb		SHUF_MASK,MSG1
	sha1nexte	MSG1,E1
	movdqa		ABCD,E0
	sha1rnds4	$0,E1,ABCD
	sha1msg1	MSG1,MSG0

//	Rounds 8-11
	movdqu		2*16(DATA_PTR),MSG2
	pshufb		SHUF_MASK,MSG2
	sha1nexte	MSG2,E0
	movdqa		ABCD,E1
	sha1rnds4	$0,E0,ABCD
	sha1msg1	MSG2,MSG1
	pxor		MSG2,MSG0

//	Rounds 12-15
	movdqu		3*16(DATA_PTR),MSG3
	pshufb		SHUF_MASK,MSG3
	sha1nexte	MSG3,E1
	movdqa		ABCD,E0
	sha1msg2	MSG3,MSG0
	sha1rnds4	$0,E1,ABCD
	sha1msg1	MSG3,MSG2
	pxor		MSG3,MSG1

//	Rounds 16-19
	sha1nexte	MSG0,E0
	movdqa		ABCD,E1
	sha1msg2	MSG0,MSG1
	sha1rnds4	$0,E0,ABCD
	sha1msg1	MSG0,MSG3
	pxor		MSG0,MSG2

//	Rounds 20-23
	sha1nexte	MSG1,E1
	movdqa		ABCD,E0
	sha1msg2	MSG1,MSG2
	sha1rnds4	$1,E1,ABCD
	sha1msg1	MSG1,MSG0
	pxor		MSG1,MSG3

//	Rounds 24-27
	sha1nexte	MSG2,E0
	movdqa		ABCD,E1
	sha1msg2	MSG2,MSG3
	sha1rnds4	$1,E0,ABCD
	sha1msg1	MSG2,MSG1
	pxor		MSG2,MSG0

//	Rounds 28-31
	sha1nexte	MSG3,E1
	movdqa		ABCD,E0
	sha1msg2	MSG3,MSG0
	sha1rnds4	$1,E1,ABCD
	sha1msg1	MSG3,MSG2
	pxor		MSG3,MSG1

//	Rounds 32-35
	sha1nexte	MSG0,E0
	movdqa		ABCD,E1
	sha1msg2	MSG0,MSG1
	sha1rnds4	$1,E0,ABCD
	sha1msg1	MSG0,MSG3
	pxor		MSG0,MSG2

//	Rounds 36-39
	sha1nexte	MSG1,E1
	movdqa		ABCD,E0
	sha1msg2	MSG1,MSG2
	sha1rnds4	$1,E1,ABCD
	sha1msg1	MSG1,MSG0
	pxor		MSG1,MSG3

//	Rounds 40-43
	sha1nexte	MSG2,E0
	movdqa		ABCD,E1
	sha1msg2	MSG2,MSG3
	sha1rnds4	$2,E0,ABCD
	sha1msg1	MSG2,MSG1
	pxor		MSG2,MSG0

//	Rounds 44-47
	sha1nexte	MSG3,E1
	movdqa		ABCD,E0
	sha1msg2	MSG3,MSG0
	sha1rnds4	$2,E1,ABCD
	sha1msg1	MSG3,MSG2
	pxor		MSG3,MSG1

//	Rounds 48-51
	sha1nexte	MSG0,E0
	movdqa		ABCD,E1
	sha1msg2	MSG0,MSG1
	sha1rnds4	$2,E0,ABCD
	sha1msg1	MSG0,MSG3
	pxor		MSG0,MSG2

//	Rounds 52-55
	sha1nexte	MSG1,E1
	movdqa		ABCD,E0
	sha1msg2	MSG1,MSG2
	sha1rnds4	$2,E1,ABCD
	sha1msg1	MSG1,MSG0
	pxor		MSG1,MSG3

//	Rounds 56-59
	sha1nexte	MSG2,E0
	movdqa		ABCD,E1
	sha1msg2	MSG2,MSG3
	sha1rnds4	$2,E0,ABCD
	sha1msg1	MSG2,MSG1
	pxor		MSG2,MSG0

//	Rounds 60-63
	sha1nexte	MSG3,E1
	movdqa		ABCD,E0
	sha1msg2	MSG3,MSG0
	sha1rnds4	$3,E1,ABCD
	sha1msg1	MSG3,MSG2
	pxor		MSG3,MSG1

//	Rounds 64-67
	sha1nexte	MSG0,E0
	movdqa		ABCD,E1
	sha1msg2	MSG0,MSG1
	sha1rnds4	$3,E0,ABCD
	sha1msg1	MSG0,MSG3
	pxor		MSG0,MSG2

//	Rounds 68-71
	sha1nexte	MSG1,E1
	movdqa		ABCD,E0
	sha1msg2	MSG1,MSG2
	sha1rnds4	$3,E1,ABCD
	pxor		MSG1,MSG3

//	Rounds 72-75
	sha1nexte	MSG2,E0
	movdqa		ABCD,E1
	sha1msg2	MSG2,MSG3
	sha1rnds4	$3,E0,ABCD

//	Rounds 76-79
	sha1nexte	MSG3,E1
	movdqa		ABCD,E0
	sha1rnds4	$3,E1,ABCD

//	Add current hash values with previously saved
	sha1nexte	(0*16)(%rsp),E0
	paddd		(1*16)(%rsp),ABCD

//	Increment data pointer and loop if more to process
	add	$64,DATA_PTR
	cmp	NUM_BLKS,DATA_PTR
	jne	.Lloop0

//	Write hash values back in the correct order
	pshufd	$0x1B,ABCD,ABCD
	movdqu	ABCD,0*16(DIGEST_PTR)
	pextrd	$3,E0,1*16(DIGEST_PTR)

.Ldone_hash:
	leave
	ret
	.endfn	sha1_transform_ni,globl

.section .rodata.cst16.PSHUFFLE_BYTE_FLIP_MASK, "aM", @progbits, 16
.balign 16
PSHUFFLE_BYTE_FLIP_MASK:
	.octa 0x000102030405060708090a0b0c0d0e0f

.section .rodata.cst16.UPPER_WORD_MASK, "aM", @progbits, 16
.balign 16
UPPER_WORD_MASK:
	.octa 0xFFFFFFFF000000000000000000000000
